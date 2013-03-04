/* $Id: thunar-location-entry.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <thunar/thunar-dialogs.h>
#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-gtk-extensions.h>
#include <thunar/thunar-icon-factory.h>
#include <thunar/thunar-location-entry.h>
#include <thunar/thunar-marshal.h>
#include <thunar/thunar-path-entry.h>
#include <thunar/thunar-private.h>
#include <thunar/thunar-shortcuts-model.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_CURRENT_DIRECTORY,
  PROP_SELECTED_FILES,
  PROP_UI_MANAGER,
};

/* Signal identifiers */
enum
{
  RESET,
  LAST_SIGNAL,
};



static void        thunar_location_entry_class_init            (ThunarLocationEntryClass *klass);
static void        thunar_location_entry_component_init        (ThunarComponentIface     *iface);
static void        thunar_location_entry_navigator_init        (ThunarNavigatorIface     *iface);
static void        thunar_location_entry_location_bar_init     (ThunarLocationBarIface   *iface);
static void        thunar_location_entry_init                  (ThunarLocationEntry      *location_entry);
static void        thunar_location_entry_finalize              (GObject                  *object);
static void        thunar_location_entry_get_property          (GObject                  *object,
                                                                guint                     prop_id,
                                                                GValue                   *value,
                                                                GParamSpec               *pspec);
static void        thunar_location_entry_set_property          (GObject                  *object,
                                                                guint                     prop_id,
                                                                const GValue             *value,
                                                                GParamSpec               *pspec);
static ThunarFile *thunar_location_entry_get_current_directory (ThunarNavigator          *navigator);
static void        thunar_location_entry_set_current_directory (ThunarNavigator          *navigator,
                                                                ThunarFile               *current_directory);
static gboolean    thunar_location_entry_accept_focus          (ThunarLocationBar        *location_bar,
                                                                const gchar              *initial_text);
static void        thunar_location_entry_activate              (GtkWidget                *path_entry,
                                                                ThunarLocationEntry      *location_entry);
static gboolean    thunar_location_entry_reset                 (ThunarLocationEntry      *location_entry);
static void        thunar_location_entry_button_clicked        (GtkWidget                *button,
                                                                ThunarLocationEntry      *location_entry);
static void        thunar_location_entry_item_activated        (GtkWidget                *item,
                                                                ThunarLocationEntry      *location_entry);



struct _ThunarLocationEntryClass
{
  GtkHBoxClass __parent__;

  /* internal action signals */
  gboolean (*reset) (ThunarLocationEntry *location_entry);
};

struct _ThunarLocationEntry
{
  GtkHBox __parent__;

  ThunarFile *current_directory;
  GtkWidget  *path_entry;
};



static GObjectClass *thunar_location_entry_parent_class;



GType
thunar_location_entry_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarLocationEntryClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_location_entry_class_init,
        NULL,
        NULL,
        sizeof (ThunarLocationEntry),
        0,
        (GInstanceInitFunc) thunar_location_entry_init,
        NULL,
      };

      static const GInterfaceInfo component_info =
      {
        (GInterfaceInitFunc) thunar_location_entry_component_init,
        NULL,
        NULL,
      };

      static const GInterfaceInfo navigator_info =
      {
        (GInterfaceInitFunc) thunar_location_entry_navigator_init,
        NULL,
        NULL,
      };

      static const GInterfaceInfo location_bar_info =
      {
        (GInterfaceInitFunc) thunar_location_entry_location_bar_init,
        NULL,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_HBOX, I_("ThunarLocationEntry"), &info, 0);
      g_type_add_interface_static (type, THUNAR_TYPE_NAVIGATOR, &navigator_info);
      g_type_add_interface_static (type, THUNAR_TYPE_COMPONENT, &component_info);
      g_type_add_interface_static (type, THUNAR_TYPE_LOCATION_BAR, &location_bar_info);
    }

  return type;
}



static void
thunar_location_entry_class_init (ThunarLocationEntryClass *klass)
{
  GtkBindingSet *binding_set;
  GObjectClass  *gobject_class;

  /* determine the parent type class */
  thunar_location_entry_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_location_entry_finalize;
  gobject_class->get_property = thunar_location_entry_get_property;
  gobject_class->set_property = thunar_location_entry_set_property;

  klass->reset = thunar_location_entry_reset;

  /* override ThunarNavigator's properties */
  g_object_class_override_property (gobject_class, PROP_CURRENT_DIRECTORY, "current-directory");

  /* override ThunarComponent's properties */
  g_object_class_override_property (gobject_class, PROP_SELECTED_FILES, "selected-files");
  g_object_class_override_property (gobject_class, PROP_UI_MANAGER, "ui-manager");

  /**
   * ThunarLocationEntry::reset:
   * @location_entry : a #ThunarLocationEntry.
   *
   * Emitted by @location_entry whenever the user requests to
   * reset the @location_entry contents to the current directory.
   * This is an internal signal used to bind the action to keys.
   **/
  g_signal_new (I_("reset"),
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                G_STRUCT_OFFSET (ThunarLocationEntryClass, reset),
                g_signal_accumulator_true_handled, NULL,
                _thunar_marshal_BOOLEAN__VOID,
                G_TYPE_BOOLEAN, 0);

  /* setup the key bindings for the location entry */
  binding_set = gtk_binding_set_by_class (klass);
  gtk_binding_entry_add_signal (binding_set, GDK_Escape, 0, "reset", 0);
}



static void
thunar_location_entry_component_init (ThunarComponentIface *iface)
{
  iface->get_selected_files = (gpointer) exo_noop_null;
  iface->set_selected_files = (gpointer) exo_noop;
  iface->get_ui_manager = (gpointer) exo_noop_null;
  iface->set_ui_manager = (gpointer) exo_noop;
}



static void
thunar_location_entry_navigator_init (ThunarNavigatorIface *iface)
{
  iface->get_current_directory = thunar_location_entry_get_current_directory;
  iface->set_current_directory = thunar_location_entry_set_current_directory;
}



static void
thunar_location_entry_location_bar_init (ThunarLocationBarIface *iface)
{
  iface->accept_focus = thunar_location_entry_accept_focus;
  iface->is_standalone = (gpointer) exo_noop_false;
}



static void
thunar_location_entry_init (ThunarLocationEntry *location_entry)
{
  GtkSizeGroup *size_group;
  GtkWidget    *button;
  GtkWidget    *arrow;

  gtk_box_set_spacing (GTK_BOX (location_entry), 0);
  gtk_container_set_border_width (GTK_CONTAINER (location_entry), 4);

  location_entry->path_entry = thunar_path_entry_new ();
  exo_binding_new (G_OBJECT (location_entry), "current-directory", G_OBJECT (location_entry->path_entry), "current-file");
  g_signal_connect_after (G_OBJECT (location_entry->path_entry), "activate", G_CALLBACK (thunar_location_entry_activate), location_entry);
  gtk_box_pack_start (GTK_BOX (location_entry), location_entry->path_entry, TRUE, TRUE, 0);
  gtk_widget_show (location_entry->path_entry);

  button = gtk_toggle_button_new ();
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (thunar_location_entry_button_clicked), location_entry);
  g_signal_connect (G_OBJECT (button), "pressed", G_CALLBACK (thunar_location_entry_button_clicked), location_entry);
  gtk_box_pack_start (GTK_BOX (location_entry), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  arrow = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_NONE);
  gtk_container_add (GTK_CONTAINER (button), arrow);
  gtk_widget_show (arrow);

  /* make sure the entry and the button request the same height */
  size_group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);
  gtk_size_group_add_widget (size_group, location_entry->path_entry);
  gtk_size_group_add_widget (size_group, button);
  g_object_unref (G_OBJECT (size_group));
}



static void
thunar_location_entry_finalize (GObject *object)
{
  /* disconnect from the selected files and the UI manager */
  thunar_component_set_selected_files (THUNAR_COMPONENT (object), NULL);
  thunar_component_set_ui_manager (THUNAR_COMPONENT (object), NULL);

  /* disconnect from the current directory */
  thunar_navigator_set_current_directory (THUNAR_NAVIGATOR (object), NULL);

  (*G_OBJECT_CLASS (thunar_location_entry_parent_class)->finalize) (object);
}



static void
thunar_location_entry_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  switch (prop_id)
    {
    case PROP_CURRENT_DIRECTORY:
      g_value_set_object (value, thunar_navigator_get_current_directory (THUNAR_NAVIGATOR (object)));
      break;

    case PROP_SELECTED_FILES:
      g_value_set_boxed (value, thunar_component_get_selected_files (THUNAR_COMPONENT (object)));
      break;

    case PROP_UI_MANAGER:
      g_value_set_object (value, thunar_component_get_ui_manager (THUNAR_COMPONENT (object)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_location_entry_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  switch (prop_id)
    {
    case PROP_CURRENT_DIRECTORY:
      thunar_navigator_set_current_directory (THUNAR_NAVIGATOR (object), g_value_get_object (value));
      break;

    case PROP_SELECTED_FILES:
      thunar_component_set_selected_files (THUNAR_COMPONENT (object), g_value_get_boxed (value));
      break;

    case PROP_UI_MANAGER:
      thunar_component_set_ui_manager (THUNAR_COMPONENT (object), g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static ThunarFile*
thunar_location_entry_get_current_directory (ThunarNavigator *navigator)
{
  return THUNAR_LOCATION_ENTRY (navigator)->current_directory;
}



static void
thunar_location_entry_set_current_directory (ThunarNavigator *navigator,
                                             ThunarFile      *current_directory)
{
  ThunarLocationEntry *location_entry = THUNAR_LOCATION_ENTRY (navigator);

  /* disconnect from the previous directory */
  if (G_LIKELY (location_entry->current_directory != NULL))
    g_object_unref (G_OBJECT (location_entry->current_directory));

  /* activate the new directory */
  location_entry->current_directory = current_directory;

  /* connect to the new directory */
  if (G_LIKELY (current_directory != NULL))
    g_object_ref (G_OBJECT (current_directory));

  /* notify listeners */
  g_object_notify (G_OBJECT (location_entry), "current-directory");
}



static gboolean
thunar_location_entry_accept_focus (ThunarLocationBar *location_bar,
                                    const gchar       *initial_text)
{
  ThunarLocationEntry *location_entry = THUNAR_LOCATION_ENTRY (location_bar);

  /* give the keyboard focus to the path entry */
  gtk_widget_grab_focus (location_entry->path_entry);

  /* check if we have an initial text for the location bar */
  if (G_LIKELY (initial_text != NULL))
    {
      /* setup the new text */
      gtk_entry_set_text (GTK_ENTRY (location_entry->path_entry), initial_text);

      /* move the cursor to the end of the text */
      gtk_editable_set_position (GTK_EDITABLE (location_entry->path_entry), -1);
    }
  else
    {
      /* select the whole path in the path entry */
      gtk_editable_select_region (GTK_EDITABLE (location_entry->path_entry), 0, -1);
    }

  return TRUE;
}



static void
thunar_location_entry_activate (GtkWidget           *path_entry,
                                ThunarLocationEntry *location_entry)
{
  ThunarFile *file;
  GError     *error = NULL;

  _thunar_return_if_fail (THUNAR_IS_LOCATION_ENTRY (location_entry));
  _thunar_return_if_fail (location_entry->path_entry == path_entry);

  /* determine the current file from the path entry */
  file = thunar_path_entry_get_current_file (THUNAR_PATH_ENTRY (path_entry));
  if (G_LIKELY (file != NULL))
    {
      /* check if we have a new directory or a file to launch */
      if (thunar_file_is_directory (file))
        {
          /* open the new directory */
          thunar_navigator_change_directory (THUNAR_NAVIGATOR (location_entry), file);
        }
      else
        {
          /* try to launch the selected file */
          if (!thunar_file_launch (file, path_entry, &error))
            {
              thunar_dialogs_show_error (path_entry, error, _("Failed to launch \"%s\""), thunar_file_get_display_name (file));
              g_error_free (error);
            }

          /* be sure to reset the current file of the path entry */
          if (G_LIKELY (location_entry->current_directory != NULL))
            thunar_path_entry_set_current_file (THUNAR_PATH_ENTRY (path_entry), location_entry->current_directory);
        }
    }
}



static gboolean
thunar_location_entry_reset (ThunarLocationEntry *location_entry)
{
  /* just reset the path entry to our current directory... */
  thunar_path_entry_set_current_file (THUNAR_PATH_ENTRY (location_entry->path_entry), location_entry->current_directory);

  /* ...and select the whole text again */
  gtk_editable_select_region (GTK_EDITABLE (location_entry->path_entry), 0, -1);

  return TRUE;
}



static void
menu_position (GtkMenu  *menu,
               gint     *x,
               gint     *y,
               gboolean *push_in,
               gpointer  entry)
{
  GtkRequisition entry_request;
  GtkRequisition menu_request;
  GdkRectangle   geometry;
  GdkScreen     *screen;
  GtkWidget     *toplevel = gtk_widget_get_toplevel (entry);
  gint           monitor;
  gint           x0;
  gint           y0;

  gtk_widget_translate_coordinates (GTK_WIDGET (entry), toplevel, 0, 0, &x0, &y0);

  gtk_widget_size_request (GTK_WIDGET (entry), &entry_request);
  gtk_widget_size_request (GTK_WIDGET (menu), &menu_request);

  gdk_window_get_position (GTK_WIDGET (entry)->window, x, y);

  *x += x0 + gtk_container_get_border_width (GTK_CONTAINER (entry));
  *y += y0 + (entry_request.height - gtk_container_get_border_width (GTK_CONTAINER (entry)));

  /* verify the the menu is on-screen */
  screen = gtk_widget_get_screen (GTK_WIDGET (entry));
  if (G_LIKELY (screen != NULL))
    {
      monitor = gdk_screen_get_monitor_at_point (screen, *x, *y);
      gdk_screen_get_monitor_geometry (screen, monitor, &geometry);
      if (*y + menu_request.height > geometry.y + geometry.height)
        *y -= menu_request.height - entry_request.height;
    }

  *push_in = TRUE;
}



static void
thunar_location_entry_button_clicked (GtkWidget           *button,
                                      ThunarLocationEntry *location_entry)
{
  ThunarShortcutsModel *model;
  ThunarIconFactory    *icon_factory;
  ThunarVfsVolume      *volume;
  GtkIconTheme         *icon_theme;
  const gchar          *icon_name;
  GtkTreeIter           iter;
  ThunarFile           *file;
  GtkWidget            *image;
  GtkWidget            *item;
  GtkWidget            *menu;
  GdkPixbuf            *icon;
  gint                  icon_size;
  gint                  width;

  _thunar_return_if_fail (THUNAR_IS_LOCATION_ENTRY (location_entry));
  _thunar_return_if_fail (GTK_IS_TOGGLE_BUTTON (button));

  /* allocate a new menu */
  menu = gtk_menu_new ();

  /* determine the icon theme and factory */
  icon_theme = gtk_icon_theme_get_for_screen (gtk_widget_get_screen (button));
  icon_factory = thunar_icon_factory_get_for_icon_theme (icon_theme);

  /* determine the icon size for menus */
  gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &icon_size, &icon_size);

  /* load the menu items from the shortcuts model */
  model = thunar_shortcuts_model_get_default ();
  if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), &iter))
    {
      do
        {
          /* determine the file and volume for the item */
          gtk_tree_model_get (GTK_TREE_MODEL (model), &iter,
                              THUNAR_SHORTCUTS_MODEL_COLUMN_FILE, &file,
                              THUNAR_SHORTCUTS_MODEL_COLUMN_VOLUME, &volume,
                              -1);

          /* check if we have a separator here */
          if (G_UNLIKELY (file == NULL && volume == NULL))
            {
              /* generate a separator the menu */
              item = gtk_separator_menu_item_new ();
            }
          else if (G_UNLIKELY (volume != NULL))
            {
              /* generate an image menu item for the volume */
              item = gtk_image_menu_item_new_with_label (thunar_vfs_volume_get_name (volume));

              /* load the icon for the volume */
              icon_name = thunar_vfs_volume_lookup_icon_name (volume, icon_theme);
              icon = thunar_icon_factory_load_icon (icon_factory, icon_name, icon_size, NULL, FALSE);
              if (G_LIKELY (icon != NULL))
                {
                  /* generate an image for the menu item */
                  image = gtk_image_new_from_pixbuf (icon);
                  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image);
                  g_object_unref (G_OBJECT (icon));
                  gtk_widget_show (image);
                }
            }
          else
            {
              /* generate an image menu item for the file */
              item = gtk_image_menu_item_new_with_label (thunar_file_get_display_name (file));

              /* load the icon for the file and generate the image for the menu item */
              icon = thunar_icon_factory_load_file_icon (icon_factory, file, THUNAR_FILE_ICON_STATE_DEFAULT, icon_size);
              image = gtk_image_new_from_pixbuf (icon);
              gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image);
              g_object_unref (G_OBJECT (icon));
              gtk_widget_show (image);
            }

          /* connect the file and volume to the item */
          g_object_set_data_full (G_OBJECT (item), I_("thunar-vfs-volume"), volume, g_object_unref);
          g_object_set_data_full (G_OBJECT (item), I_("thunar-file"), file, g_object_unref);

          /* append the new item to the menu */
          g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (thunar_location_entry_item_activated), location_entry);
          gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
          gtk_widget_show (item);
        }
      while (gtk_tree_model_iter_next (GTK_TREE_MODEL (model), &iter));
    }

  /* make sure the menu has atleast the same width as the location entry */
  width = GTK_WIDGET (location_entry)->allocation.width - 2 * gtk_container_get_border_width (GTK_CONTAINER (location_entry));
  if (G_LIKELY (menu->allocation.width < width))
    gtk_widget_set_size_request (menu, width, -1);

  /* select the first visible or selectable item in the menu */
  gtk_menu_shell_select_first (GTK_MENU_SHELL (menu), TRUE);

  /* enable the button, making sure that we do not recurse on the "clicked" signal by temporarily blocking the handler */
  g_signal_handlers_block_by_func (G_OBJECT (button), G_CALLBACK (thunar_location_entry_button_clicked), location_entry);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  g_signal_handlers_unblock_by_func (G_OBJECT (button), G_CALLBACK (thunar_location_entry_button_clicked), location_entry);

  /* run the menu, taking ownership over the menu object */
  thunar_gtk_menu_run (GTK_MENU (menu), button, menu_position, location_entry, 1, gtk_get_current_event_time ());

  /* disable the button, making sure that we do not recurse on the "clicked" signal by temporarily blocking the handler */
  g_signal_handlers_block_by_func (G_OBJECT (button), G_CALLBACK (thunar_location_entry_button_clicked), location_entry);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
  g_signal_handlers_unblock_by_func (G_OBJECT (button), G_CALLBACK (thunar_location_entry_button_clicked), location_entry);

  /* clean up */
  g_object_unref (G_OBJECT (icon_factory));
  g_object_unref (G_OBJECT (model));
}



static void
thunar_location_entry_item_activated (GtkWidget           *item,
                                      ThunarLocationEntry *location_entry)
{
  ThunarVfsVolume *volume;
  ThunarFile      *file;
  GtkWidget       *window;
  GError          *error = NULL;

  _thunar_return_if_fail (GTK_IS_MENU_ITEM (item));
  _thunar_return_if_fail (THUNAR_IS_LOCATION_ENTRY (location_entry));

  /* determine the toplevel window */
  window = gtk_widget_get_toplevel (GTK_WIDGET (location_entry));

  /* check if the item corresponds to a volume */
  volume = g_object_get_data (G_OBJECT (item), "thunar-vfs-volume");
  if (G_UNLIKELY (volume != NULL))
    {
      /* check if the volume isn't already mounted */
      if (G_LIKELY (!thunar_vfs_volume_is_mounted (volume)))
        {
          /* try to mount the volume */
          if (!thunar_vfs_volume_mount (volume, window, &error))
            {
              /* display an error dialog to inform the user */
              thunar_dialogs_show_error (window, error, _("Failed to mount \"%s\""), thunar_vfs_volume_get_name (volume));
              g_error_free (error);
              return;
            }
        }

      /* try to determine the mount point of the volume */
      file = thunar_file_get_for_path (thunar_vfs_volume_get_mount_point (volume), &error);
      if (G_UNLIKELY (file == NULL))
        {
          /* display an error dialog to inform the user */
          thunar_dialogs_show_error (window, error, _("Failed to determine the mount point for %s"), thunar_vfs_volume_get_name (volume));
          g_error_free (error);
          return;
        }
    }
  else
    {
      /* determine the file from the item */
      file = g_object_get_data (G_OBJECT (item), "thunar-file");
      if (G_LIKELY (file != NULL))
        g_object_ref (G_OBJECT (file));
    }

  /* check if we have a file object now */
  if (G_LIKELY (file != NULL))
    {
      /* make sure that this is actually a directory */
      if (thunar_file_is_directory (file))
        {
          /* open the new directory */
          thunar_navigator_change_directory (THUNAR_NAVIGATOR (location_entry), file);
        }

      /* cleanup */
      g_object_unref (G_OBJECT (file));
    }
}



/**
 * thunar_location_entry_new:
 *
 * Allocates a new #ThunarLocationEntry instance.
 *
 * Return value: the newly allocated #ThunarLocationEntry.
 **/
GtkWidget*
thunar_location_entry_new (void)
{
  return g_object_new (THUNAR_TYPE_LOCATION_ENTRY, NULL);
}



