/* $Id: thunar-properties-dialog.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2007 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <thunar/thunar-abstract-dialog.h>
#include <thunar/thunar-chooser-button.h>
#include <thunar/thunar-dialogs.h>
#include <thunar/thunar-emblem-chooser.h>
#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-gtk-extensions.h>
#include <thunar/thunar-icon-factory.h>
#include <thunar/thunar-marshal.h>
#include <thunar/thunar-pango-extensions.h>
#include <thunar/thunar-permissions-chooser.h>
#include <thunar/thunar-preferences.h>
#include <thunar/thunar-private.h>
#include <thunar/thunar-properties-dialog.h>
#include <thunar/thunar-size-label.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_FILE,
};

/* Signal identifiers */
enum
{
  RELOAD,
  LAST_SIGNAL,
};



static void     thunar_properties_dialog_class_init           (ThunarPropertiesDialogClass *klass);
static void     thunar_properties_dialog_init                 (ThunarPropertiesDialog      *dialog);
static void     thunar_properties_dialog_dispose              (GObject                     *object);
static void     thunar_properties_dialog_finalize             (GObject                     *object);
static void     thunar_properties_dialog_get_property         (GObject                     *object,
                                                               guint                        prop_id,
                                                               GValue                      *value,
                                                               GParamSpec                  *pspec);
static void     thunar_properties_dialog_set_property         (GObject                     *object,
                                                               guint                        prop_id,
                                                               const GValue                *value,
                                                               GParamSpec                  *pspec);
static void     thunar_properties_dialog_response             (GtkDialog                   *dialog,
                                                               gint                         response);
static gboolean thunar_properties_dialog_reload               (ThunarPropertiesDialog      *dialog);
static void     thunar_properties_dialog_activate             (GtkWidget                   *entry,
                                                               ThunarPropertiesDialog      *dialog);
static gboolean thunar_properties_dialog_focus_out_event      (GtkWidget                   *entry,
                                                               GdkEventFocus               *event,
                                                               ThunarPropertiesDialog      *dialog);
static void     thunar_properties_dialog_icon_button_clicked  (GtkWidget                   *button,
                                                               ThunarPropertiesDialog      *dialog);
static void     thunar_properties_dialog_update               (ThunarPropertiesDialog      *dialog);
static void     thunar_properties_dialog_update_providers     (ThunarPropertiesDialog      *dialog);
static gboolean thunar_properties_dialog_rename_idle          (gpointer                     user_data);
static void     thunar_properties_dialog_rename_idle_destroy  (gpointer                     user_data);



struct _ThunarPropertiesDialogClass
{
  ThunarAbstractDialogClass __parent__;

  /* signals */
  gboolean (*reload) (ThunarPropertiesDialog *dialog);
};

struct _ThunarPropertiesDialog
{
  ThunarAbstractDialog    __parent__;

  ThunarxProviderFactory *provider_factory;
  GList                  *provider_pages;

  ThunarPreferences      *preferences;

  ThunarVfsVolumeManager *volume_manager;
  ThunarFile             *file;

  GtkWidget              *notebook;
  GtkWidget              *icon_button;
  GtkWidget              *icon_image;
  GtkWidget              *name_entry;
  GtkWidget              *kind_ebox;
  GtkWidget              *kind_label;
  GtkWidget              *openwith_chooser;
  GtkWidget              *link_label;
  GtkWidget              *origin_label;
  GtkWidget              *deleted_label;
  GtkWidget              *modified_label;
  GtkWidget              *accessed_label;
  GtkWidget              *freespace_label;
  GtkWidget              *volume_image;
  GtkWidget              *volume_label;
  GtkWidget              *permissions_chooser;

  guint                   rename_idle_id;
};



static GObjectClass *thunar_properties_dialog_parent_class;



GType
thunar_properties_dialog_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarPropertiesDialogClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_properties_dialog_class_init,
        NULL,
        NULL,
        sizeof (ThunarPropertiesDialog),
        0,
        (GInstanceInitFunc) thunar_properties_dialog_init,
        NULL,
      };

      type = g_type_register_static (THUNAR_TYPE_ABSTRACT_DIALOG, I_("ThunarPropertiesDialog"), &info, 0);
    }

  return type;
}



static void
thunar_properties_dialog_class_init (ThunarPropertiesDialogClass *klass)
{
  GtkDialogClass *gtkdialog_class;
  GtkBindingSet  *binding_set;
  GObjectClass   *gobject_class;

  /* determine the parent type class */
  thunar_properties_dialog_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = thunar_properties_dialog_dispose;
  gobject_class->finalize = thunar_properties_dialog_finalize;
  gobject_class->get_property = thunar_properties_dialog_get_property;
  gobject_class->set_property = thunar_properties_dialog_set_property;

  gtkdialog_class = GTK_DIALOG_CLASS (klass);
  gtkdialog_class->response = thunar_properties_dialog_response;

  klass->reload = thunar_properties_dialog_reload;

  /**
   * ThunarPropertiesDialog:file:
   *
   * The #ThunarFile whose properties are currently displayed by
   * this #ThunarPropertiesDialog. This property may also be %NULL
   * in which case nothing is displayed.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FILE,
                                   g_param_spec_object ("file", "file", "file",
                                                        THUNAR_TYPE_FILE,
                                                        EXO_PARAM_READWRITE));

  /**
   * ThunarPropertiesDialog::reload:
   * @dialog : a #ThunarPropertiesDialog.
   *
   * Emitted whenever the user requests reset the reload the
   * file properties. This is an internal signal used to bind
   * the action to keys.
   **/
  g_signal_new (I_("reload"),
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                G_STRUCT_OFFSET (ThunarPropertiesDialogClass, reload),
                g_signal_accumulator_true_handled, NULL,
                _thunar_marshal_BOOLEAN__VOID,
                G_TYPE_BOOLEAN, 0);

  /* setup the key bindings for the properties dialog */
  binding_set = gtk_binding_set_by_class (klass);
  gtk_binding_entry_add_signal (binding_set, GDK_F5, 0, "reload", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_r, GDK_CONTROL_MASK, "reload", 0);
}



static void
thunar_properties_dialog_init (ThunarPropertiesDialog *dialog)
{
  GtkWidget *chooser;
  GtkWidget *table;
  GtkWidget *label;
  GtkWidget *box;
  GtkWidget *spacer;
  gint       row = 0;

  /* acquire a reference on the preferences and monitor the "misc-date-style" setting */
  dialog->preferences = thunar_preferences_get ();
  g_signal_connect_swapped (G_OBJECT (dialog->preferences), "notify::misc-date-style",
                            G_CALLBACK (thunar_properties_dialog_reload), dialog);

  dialog->provider_factory = thunarx_provider_factory_get_default ();
  dialog->volume_manager = thunar_vfs_volume_manager_get_default ();

  gtk_dialog_add_buttons (GTK_DIALOG (dialog),
                          GTK_STOCK_HELP, GTK_RESPONSE_HELP,
                          GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
                          NULL);
  gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
  gtk_window_set_default_size (GTK_WINDOW (dialog), 400, 430);

  dialog->notebook = gtk_notebook_new ();
  gtk_container_set_border_width (GTK_CONTAINER (dialog->notebook), 6);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), dialog->notebook, TRUE, TRUE, 0);
  gtk_widget_show (dialog->notebook);

  table = gtk_table_new (2, 2, FALSE);
  label = gtk_label_new (_("General"));
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_container_set_border_width (GTK_CONTAINER (table), 6);
  gtk_notebook_append_page (GTK_NOTEBOOK (dialog->notebook), table, label);
  gtk_widget_show (label);
  gtk_widget_show (table);


  /*
     First box (icon, name)
   */
  box = gtk_hbox_new (FALSE, 6);
  gtk_table_attach (GTK_TABLE (table), box, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (box);

  dialog->icon_button = gtk_button_new ();
  g_signal_connect (G_OBJECT (dialog->icon_button), "clicked", G_CALLBACK (thunar_properties_dialog_icon_button_clicked), dialog);
  gtk_box_pack_start (GTK_BOX (box), dialog->icon_button, FALSE, TRUE, 0);
  gtk_widget_show (dialog->icon_button);

  dialog->icon_image = gtk_image_new ();
  gtk_box_pack_start (GTK_BOX (box), dialog->icon_image, FALSE, TRUE, 0);
  gtk_widget_show (dialog->icon_image);

  label = gtk_label_new (_("Name:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_box_pack_end (GTK_BOX (box), label, TRUE, TRUE, 0);
  gtk_widget_show (label);

  dialog->name_entry = g_object_new (GTK_TYPE_ENTRY, "editable", FALSE, NULL);
  g_signal_connect (G_OBJECT (dialog->name_entry), "activate", G_CALLBACK (thunar_properties_dialog_activate), dialog);
  g_signal_connect (G_OBJECT (dialog->name_entry), "focus-out-event", G_CALLBACK (thunar_properties_dialog_focus_out_event), dialog);
  gtk_table_attach (GTK_TABLE (table), dialog->name_entry, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->name_entry);

  ++row;


  spacer = g_object_new (GTK_TYPE_ALIGNMENT, "height-request", 12, NULL);
  gtk_table_attach (GTK_TABLE (table), spacer, 0, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (spacer);

  ++row;


  /*
     Second box (kind, open with, link target)
   */
  label = gtk_label_new (_("Kind:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  exo_binding_new (G_OBJECT (label), "visible", G_OBJECT (spacer), "visible");
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->kind_ebox = gtk_event_box_new ();
  gtk_event_box_set_above_child (GTK_EVENT_BOX (dialog->kind_ebox), TRUE);
  gtk_event_box_set_visible_window (GTK_EVENT_BOX (dialog->kind_ebox), FALSE);
  exo_binding_new (G_OBJECT (dialog->kind_ebox), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->kind_ebox, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->kind_ebox);

  dialog->kind_label = g_object_new (GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->kind_label), TRUE);
  gtk_label_set_ellipsize (GTK_LABEL (dialog->kind_label), PANGO_ELLIPSIZE_END);
  gtk_container_add (GTK_CONTAINER (dialog->kind_ebox), dialog->kind_label);
  gtk_widget_show (dialog->kind_label);

  ++row;

  label = gtk_label_new (_("Open With:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->openwith_chooser = thunar_chooser_button_new ();
  exo_binding_new (G_OBJECT (dialog), "file", G_OBJECT (dialog->openwith_chooser), "file");
  exo_binding_new (G_OBJECT (dialog->openwith_chooser), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->openwith_chooser, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->openwith_chooser);

  ++row;

  label = gtk_label_new (_("Link Target:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->link_label = g_object_new (GTK_TYPE_LABEL, "ellipsize", PANGO_ELLIPSIZE_START, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->link_label), TRUE);
  exo_binding_new (G_OBJECT (dialog->link_label), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->link_label, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->link_label);

  ++row;

  /* TRANSLATORS: Try to come up with a short translation of "Original Path" (which is the path
   * where the trashed file/folder was located before it was moved to the trash), otherwise the
   * properties dialog width will be messed up.
   */
  label = gtk_label_new (_("Original Path:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->origin_label = g_object_new (GTK_TYPE_LABEL, "ellipsize", PANGO_ELLIPSIZE_START, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->origin_label), TRUE);
  exo_binding_new (G_OBJECT (dialog->origin_label), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->origin_label, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->origin_label);

  ++row;


  spacer = g_object_new (GTK_TYPE_ALIGNMENT, "height-request", 12, NULL);
  gtk_table_attach (GTK_TABLE (table), spacer, 0, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (spacer);

  ++row;


  /*
     Third box (deleted, modified, accessed)
   */
  label = gtk_label_new (_("Deleted:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->deleted_label = g_object_new (GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->deleted_label), TRUE);
  exo_binding_new (G_OBJECT (dialog->deleted_label), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->deleted_label, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->deleted_label);

  ++row;

  label = gtk_label_new (_("Modified:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->modified_label = g_object_new (GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->modified_label), TRUE);
  exo_binding_new (G_OBJECT (dialog->modified_label), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->modified_label, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->modified_label);

  ++row;

  label = gtk_label_new (_("Accessed:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->accessed_label = g_object_new (GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->accessed_label), TRUE);
  exo_binding_new (G_OBJECT (dialog->accessed_label), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->accessed_label, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->accessed_label);

  ++row;


  spacer = g_object_new (GTK_TYPE_ALIGNMENT, "height-request", 12, NULL);
  gtk_table_attach (GTK_TABLE (table), spacer, 0, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (spacer);

  ++row;


  /*
     Fourth box (size, volume, free space)
   */
  label = gtk_label_new (_("Size:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  label = thunar_size_label_new ();
  exo_binding_new (G_OBJECT (dialog), "file", G_OBJECT (label), "file");
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  ++row;

  label = gtk_label_new (_("Volume:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  box = gtk_hbox_new (FALSE, 6);
  exo_binding_new (G_OBJECT (box), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), box, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (box);

  dialog->volume_image = gtk_image_new ();
  exo_binding_new (G_OBJECT (dialog->volume_image), "visible", G_OBJECT (box), "visible");
  gtk_box_pack_start (GTK_BOX (box), dialog->volume_image, FALSE, TRUE, 0);
  gtk_widget_show (dialog->volume_image);

  dialog->volume_label = g_object_new (GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->volume_label), TRUE);
  exo_binding_new (G_OBJECT (dialog->volume_label), "visible", G_OBJECT (dialog->volume_image), "visible");
  gtk_box_pack_start (GTK_BOX (box), dialog->volume_label, TRUE, TRUE, 0);
  gtk_widget_show (dialog->volume_label);

  ++row;

  label = gtk_label_new (_("Free Space:"));
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  dialog->freespace_label = g_object_new (GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
  gtk_label_set_selectable (GTK_LABEL (dialog->freespace_label), TRUE);
  exo_binding_new (G_OBJECT (dialog->freespace_label), "visible", G_OBJECT (label), "visible");
  gtk_table_attach (GTK_TABLE (table), dialog->freespace_label, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (dialog->freespace_label);

  ++row;


  spacer = g_object_new (GTK_TYPE_ALIGNMENT, "height-request", 12, NULL);
  gtk_table_attach (GTK_TABLE (table), spacer, 0, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (spacer);

  ++row;


  /*
     Emblem chooser
   */
  label = gtk_label_new (_("Emblems"));
  chooser = thunar_emblem_chooser_new ();
  exo_binding_new (G_OBJECT (dialog), "file", G_OBJECT (chooser), "file");
  gtk_notebook_append_page (GTK_NOTEBOOK (dialog->notebook), chooser, label);
  gtk_widget_show (chooser);
  gtk_widget_show (label);


  /*
     Permissions chooser
   */
  label = gtk_label_new (_("Permissions"));
  dialog->permissions_chooser = thunar_permissions_chooser_new ();
  exo_binding_new (G_OBJECT (dialog), "file", G_OBJECT (dialog->permissions_chooser), "file");
  gtk_notebook_append_page (GTK_NOTEBOOK (dialog->notebook), dialog->permissions_chooser, label);
  gtk_widget_show (dialog->permissions_chooser);
  gtk_widget_show (label);


  /* place the initial focus on the name entry widget */
  gtk_widget_grab_focus (dialog->name_entry);
}



static void
thunar_properties_dialog_dispose (GObject *object)
{
  ThunarPropertiesDialog *dialog = THUNAR_PROPERTIES_DIALOG (object);

  /* reset the file displayed by the dialog */
  thunar_properties_dialog_set_file (dialog, NULL);

  (*G_OBJECT_CLASS (thunar_properties_dialog_parent_class)->dispose) (object);
}



static void
thunar_properties_dialog_finalize (GObject *object)
{
  ThunarPropertiesDialog *dialog = THUNAR_PROPERTIES_DIALOG (object);

  /* disconnect from the preferences */
  g_signal_handlers_disconnect_by_func (G_OBJECT (dialog->preferences), thunar_properties_dialog_reload, dialog);
  g_object_unref (G_OBJECT (dialog->preferences));

  /* release the provider property pages */
  g_list_foreach (dialog->provider_pages, (GFunc) g_object_unref, NULL);
  g_list_free (dialog->provider_pages);

  /* drop the reference on the provider factory */
  g_object_unref (G_OBJECT (dialog->provider_factory));

  /* drop the reference on the volume manager */
  g_object_unref (G_OBJECT (dialog->volume_manager));

  /* be sure to cancel any pending rename idle source */
  if (G_UNLIKELY (dialog->rename_idle_id != 0))
    g_source_remove (dialog->rename_idle_id);

  (*G_OBJECT_CLASS (thunar_properties_dialog_parent_class)->finalize) (object);
}



static void
thunar_properties_dialog_get_property (GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  ThunarPropertiesDialog *dialog = THUNAR_PROPERTIES_DIALOG (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, thunar_properties_dialog_get_file (dialog));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_properties_dialog_set_property (GObject      *object,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
  ThunarPropertiesDialog *dialog = THUNAR_PROPERTIES_DIALOG (object);

  switch (prop_id)
    {
    case PROP_FILE:
      thunar_properties_dialog_set_file (dialog, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_properties_dialog_response (GtkDialog *dialog,
                                   gint       response)
{
  if (response == GTK_RESPONSE_CLOSE)
    {
      gtk_widget_destroy (GTK_WIDGET (dialog));
    }
  else if (response == GTK_RESPONSE_HELP)
    {
      thunar_dialogs_show_help (dialog, "working-with-files-and-folders", "file-properties");
    }
  else if (GTK_DIALOG_CLASS (thunar_properties_dialog_parent_class)->response != NULL)
    {
      (*GTK_DIALOG_CLASS (thunar_properties_dialog_parent_class)->response) (dialog, response);
    }
}



static gboolean
thunar_properties_dialog_reload (ThunarPropertiesDialog *dialog)
{
  /* verify that we still have a file */
  if (G_LIKELY (dialog->file != NULL))
    {
      /* reload the file status */
      thunar_file_reload (dialog->file);

      /* we handled the event */
      return TRUE;
    }
  else
    {
      /* did not handle the event */
      return FALSE;
    }
}



static void
thunar_properties_dialog_activate (GtkWidget              *entry,
                                   ThunarPropertiesDialog *dialog)
{
  if (G_LIKELY (dialog->rename_idle_id == 0))
    {
      dialog->rename_idle_id = g_idle_add_full (G_PRIORITY_DEFAULT, thunar_properties_dialog_rename_idle,
                                                dialog, thunar_properties_dialog_rename_idle_destroy);
    }
}



static gboolean
thunar_properties_dialog_focus_out_event (GtkWidget              *entry,
                                          GdkEventFocus          *event,
                                          ThunarPropertiesDialog *dialog)
{
  thunar_properties_dialog_activate (entry, dialog);
  return FALSE;
}



static void
thunar_properties_dialog_icon_button_clicked (GtkWidget              *button,
                                              ThunarPropertiesDialog *dialog)
{
  const gchar *custom_icon;
  GtkWidget   *chooser;
  GError      *err = NULL;
  gchar       *title;
  gchar       *icon;

  _thunar_return_if_fail (THUNAR_IS_PROPERTIES_DIALOG (dialog));
  _thunar_return_if_fail (GTK_IS_BUTTON (button));

  /* make sure we still have a file */
  if (G_UNLIKELY (dialog->file == NULL))
    return;

  /* allocate the icon chooser */
  title = g_strdup_printf (_("Select an Icon for \"%s\""), thunar_file_get_display_name (dialog->file));
  chooser = exo_icon_chooser_dialog_new (title, GTK_WINDOW (dialog),
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                         NULL);
  gtk_dialog_set_alternative_button_order (GTK_DIALOG (chooser), GTK_RESPONSE_ACCEPT, GTK_RESPONSE_CANCEL, -1);
  gtk_dialog_set_default_response (GTK_DIALOG (chooser), GTK_RESPONSE_ACCEPT);
  g_free (title);

  /* use the custom_icon of the file as default */
  custom_icon = thunar_file_get_custom_icon (dialog->file);
  if (G_LIKELY (custom_icon != NULL && *custom_icon != '\0'))
    exo_icon_chooser_dialog_set_icon (EXO_ICON_CHOOSER_DIALOG (chooser), custom_icon);

  /* run the icon chooser dialog and make sure the dialog still has a file */
  if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT && dialog->file != NULL)
    {
      /* determine the selected icon and use it for the file */
      icon = exo_icon_chooser_dialog_get_icon (EXO_ICON_CHOOSER_DIALOG (chooser));
      if (!thunar_file_set_custom_icon (dialog->file, icon, &err))
        {
          /* hide the icon chooser dialog first */
          gtk_widget_hide (chooser);

          /* tell the user that we failed to change the icon of the .desktop file */
          thunar_dialogs_show_error (GTK_WIDGET (dialog), err, _("Failed to change icon of \"%s\""), thunar_file_get_display_name (dialog->file));
          g_error_free (err);
        }
      g_free (icon);
    }

  /* destroy the chooser */
  gtk_widget_destroy (chooser);
}



static void
thunar_properties_dialog_update_providers (ThunarPropertiesDialog *dialog)
{
  GtkWidget *label_widget;
  GList     *providers;
  GList     *pages = NULL;
  GList      files;
  GList     *tmp;
  GList     *lp;

  /* load the property page providers from the provider factory */
  providers = thunarx_provider_factory_list_providers (dialog->provider_factory, THUNARX_TYPE_PROPERTY_PAGE_PROVIDER);
  if (G_LIKELY (providers != NULL))
    {
      /* determine the (one-element) file list */
      files.data = dialog->file; files.next = files.prev = NULL;

      /* load the pages offered by the menu providers */
      for (lp = providers; lp != NULL; lp = lp->next)
        {
          tmp = thunarx_property_page_provider_get_pages (lp->data, &files);
          pages = g_list_concat (pages, tmp);
          g_object_unref (G_OBJECT (lp->data));
        }
      g_list_free (providers);
    }

  /* destroy any previous set pages */
  for (lp = dialog->provider_pages; lp != NULL; lp = lp->next)
    {
      gtk_widget_destroy (GTK_WIDGET (lp->data));
      g_object_unref (G_OBJECT (lp->data));
    }
  g_list_free (dialog->provider_pages);

  /* apply the new set of pages */
  dialog->provider_pages = pages;
  for (lp = pages; lp != NULL; lp = lp->next)
    {
      label_widget = thunarx_property_page_get_label_widget (THUNARX_PROPERTY_PAGE (lp->data));
      gtk_notebook_append_page (GTK_NOTEBOOK (dialog->notebook), GTK_WIDGET (lp->data), label_widget);
      g_object_ref (G_OBJECT (lp->data));
      gtk_widget_show (lp->data);
    }
}



static void
thunar_properties_dialog_update (ThunarPropertiesDialog *dialog)
{
  ThunarIconFactory *icon_factory;
  ThunarVfsFileSize  size;
  ThunarVfsMimeInfo *info;
  ThunarDateStyle    date_style;
  ThunarVfsVolume   *volume;
  GtkIconTheme      *icon_theme;
  const gchar       *icon_name;
  const gchar       *name;
  GdkPixbuf         *icon;
  glong              offset;
  gchar             *display_name;
  gchar             *size_string;
  gchar             *str;

  _thunar_return_if_fail (THUNAR_IS_PROPERTIES_DIALOG (dialog));
  _thunar_return_if_fail (THUNAR_IS_FILE (dialog->file));

  icon_theme = gtk_icon_theme_get_for_screen (gtk_widget_get_screen (GTK_WIDGET (dialog)));
  icon_factory = thunar_icon_factory_get_for_icon_theme (icon_theme);

  /* determine the style used to format dates */
  g_object_get (G_OBJECT (dialog->preferences), "misc-date-style", &date_style, NULL);

  /* update the properties dialog title */
  str = g_strdup_printf (_("%s - Properties"), thunar_file_get_display_name (dialog->file));
  gtk_window_set_title (GTK_WINDOW (dialog), str);
  g_free (str);

  /* update the icon */
  icon = thunar_icon_factory_load_file_icon (icon_factory, dialog->file, THUNAR_FILE_ICON_STATE_DEFAULT, 48);
  gtk_image_set_from_pixbuf (GTK_IMAGE (dialog->icon_image), icon);
  gtk_window_set_icon (GTK_WINDOW (dialog), icon);
  if (G_LIKELY (icon != NULL))
    g_object_unref (G_OBJECT (icon));

  /* check if the icon may be changed (only for writable .desktop files) */
  g_object_ref (G_OBJECT (dialog->icon_image));
  gtk_container_remove (GTK_CONTAINER (gtk_widget_get_parent (dialog->icon_image)), dialog->icon_image);
  if (thunar_file_is_writable (dialog->file) && thunar_file_is_desktop_file (dialog->file))
    {
      gtk_container_add (GTK_CONTAINER (dialog->icon_button), dialog->icon_image);
      gtk_widget_show (dialog->icon_button);
    }
  else
    {
      gtk_box_pack_start (GTK_BOX (gtk_widget_get_parent (dialog->icon_button)), dialog->icon_image, FALSE, TRUE, 0);
      gtk_widget_hide (dialog->icon_button);
    }
  g_object_unref (G_OBJECT (dialog->icon_image));

  /* update the name (if it differs) */
  gtk_entry_set_editable (GTK_ENTRY (dialog->name_entry), thunar_file_is_renameable (dialog->file));
  name = thunar_file_get_display_name (dialog->file);
  if (G_LIKELY (strcmp (name, gtk_entry_get_text (GTK_ENTRY (dialog->name_entry))) != 0))
    {
      gtk_entry_set_text (GTK_ENTRY (dialog->name_entry), name);

      /* grab the input focus to the name entry */
      gtk_widget_grab_focus (dialog->name_entry);

      /* select the pre-dot part of the name */
      str = strrchr (name, '.');
      if (G_LIKELY (str != NULL))
        {
          /* calculate the offset */
          offset = g_utf8_pointer_to_offset (name, str);

          /* select the region */
          if (G_LIKELY (offset > 0))
            gtk_entry_select_region (GTK_ENTRY (dialog->name_entry), 0, offset);
        }
    }

  /* update the mime type */
  info = thunar_file_get_mime_info (dialog->file);
  if (G_UNLIKELY (strcmp (thunar_vfs_mime_info_get_name (info), "inode/symlink") == 0))
    str = g_strdup (_("broken link"));
  else if (G_UNLIKELY (thunar_file_is_symlink (dialog->file)))
    str = g_strdup_printf (_("link to %s"), thunar_vfs_mime_info_get_comment (info));
  else
    str = g_strdup (thunar_vfs_mime_info_get_comment (info));
  thunar_gtk_widget_set_tooltip (dialog->kind_ebox, thunar_vfs_mime_info_get_name (info));
  gtk_label_set_text (GTK_LABEL (dialog->kind_label), str);
  g_free (str);

  /* update the application chooser (shown only for non-executable regular files!) */
  g_object_set (G_OBJECT (dialog->openwith_chooser),
                "visible", (thunar_file_is_regular (dialog->file) && !thunar_file_is_executable (dialog->file)),
                NULL);

  /* update the link target */
  str = thunar_file_is_symlink (dialog->file) ? thunar_file_read_link (dialog->file, NULL) : NULL;
  if (G_UNLIKELY (str != NULL))
    {
      display_name = g_filename_display_name (str);
      gtk_label_set_text (GTK_LABEL (dialog->link_label), display_name);
      gtk_widget_show (dialog->link_label);
      g_free (display_name);
      g_free (str);
    }
  else
    {
      gtk_widget_hide (dialog->link_label);
    }

  /* update the original path */
  str = thunar_file_get_original_path (dialog->file);
  if (G_UNLIKELY (str != NULL))
    {
      display_name = g_filename_display_name (str);
      gtk_label_set_text (GTK_LABEL (dialog->origin_label), display_name);
      gtk_widget_show (dialog->origin_label);
      g_free (display_name);
      g_free (str);
    }
  else
    {
      gtk_widget_hide (dialog->origin_label);
    }

  /* update the deleted time */
  str = thunar_file_get_deletion_date (dialog->file, date_style);
  if (G_LIKELY (str != NULL))
    {
      gtk_label_set_text (GTK_LABEL (dialog->deleted_label), str);
      gtk_widget_show (dialog->deleted_label);
      g_free (str);
    }
  else
    {
      gtk_widget_hide (dialog->deleted_label);
    }

  /* update the modified time */
  str = thunar_file_get_date_string (dialog->file, THUNAR_FILE_DATE_MODIFIED, date_style);
  if (G_LIKELY (str != NULL))
    {
      gtk_label_set_text (GTK_LABEL (dialog->modified_label), str);
      gtk_widget_show (dialog->modified_label);
      g_free (str);
    }
  else
    {
      gtk_widget_hide (dialog->modified_label);
    }

  /* update the accessed time */
  str = thunar_file_get_date_string (dialog->file, THUNAR_FILE_DATE_ACCESSED, date_style);
  if (G_LIKELY (str != NULL))
    {
      gtk_label_set_text (GTK_LABEL (dialog->accessed_label), str);
      gtk_widget_show (dialog->accessed_label);
      g_free (str);
    }
  else
    {
      gtk_widget_hide (dialog->accessed_label);
    }

  /* update the free space (only for folders) */
  if (thunar_file_is_directory (dialog->file) && thunar_file_get_free_space (dialog->file, &size))
    {
      size_string = thunar_vfs_humanize_size (size, NULL, 0);
      gtk_label_set_text (GTK_LABEL (dialog->freespace_label), size_string);
      gtk_widget_show (dialog->freespace_label);
      g_free (size_string);
    }
  else
    {
      gtk_widget_hide (dialog->freespace_label);
    }

  /* update the volume */
  volume = thunar_file_is_local (dialog->file) ? thunar_file_get_volume (dialog->file, dialog->volume_manager) : NULL;
  if (G_LIKELY (volume != NULL))
    {
      icon_name = thunar_vfs_volume_lookup_icon_name (volume, icon_theme);
      icon = thunar_icon_factory_load_icon (icon_factory, icon_name, 16, NULL, FALSE);
      gtk_image_set_from_pixbuf (GTK_IMAGE (dialog->volume_image), icon);
      if (G_LIKELY (icon != NULL))
        g_object_unref (G_OBJECT (icon));

      name = thunar_vfs_volume_get_name (volume);
      gtk_label_set_text (GTK_LABEL (dialog->volume_label), name);
      gtk_widget_show (dialog->volume_label);
    }
  else
    {
      gtk_widget_hide (dialog->volume_label);
    }

  /* cleanup */
  g_object_unref (G_OBJECT (icon_factory));
}



static gboolean
thunar_properties_dialog_rename_idle (gpointer user_data)
{
  ThunarPropertiesDialog *dialog = THUNAR_PROPERTIES_DIALOG (user_data);
  const gchar            *old_name;
  GError                 *error = NULL;
  gchar                  *new_name;

  /* check if we still have a valid file and if the user is allowed to rename */
  if (G_UNLIKELY (dialog->file == NULL || !GTK_WIDGET_SENSITIVE (dialog->name_entry)))
    return FALSE;

  GDK_THREADS_ENTER ();

  /* determine new and old name */
  new_name = gtk_editable_get_chars (GTK_EDITABLE (dialog->name_entry), 0, -1);
  old_name = thunar_file_get_display_name (dialog->file);
  if (g_utf8_collate (new_name, old_name) != 0)
    {
      /* try to rename the file to the new name */
      if (!thunar_file_rename (dialog->file, new_name, &error))
        {
          /* reset the entry widget to the old name */
          gtk_entry_set_text (GTK_ENTRY (dialog->name_entry), old_name);

          /* display an error message */
          thunar_dialogs_show_error (GTK_WIDGET (dialog), error, _("Failed to rename \"%s\""), old_name);

          /* release the error */
          g_error_free (error);
        }
    }
  g_free (new_name);

  GDK_THREADS_LEAVE ();

  return FALSE;
}



static void
thunar_properties_dialog_rename_idle_destroy (gpointer user_data)
{
  THUNAR_PROPERTIES_DIALOG (user_data)->rename_idle_id = 0;
}



/**
 * thunar_properties_dialog_new:
 *
 * Allocates a new #ThunarPropertiesDialog instance,
 * that is not associated with any #ThunarFile.
 *
 * Return value: the newly allocated #ThunarPropertiesDialog
 *               instance.
 **/
GtkWidget*
thunar_properties_dialog_new (void)
{
  return g_object_new (THUNAR_TYPE_PROPERTIES_DIALOG, NULL);
}



/**
 * thunar_properties_dialog_get_file:
 * @dialog : a #ThunarPropertiesDialog.
 *
 * Returns the #ThunarFile currently being displayed
 * by @dialog or %NULL if @dialog doesn't display
 * any file right now.
 *
 * Return value: the #ThunarFile displayed by @dialog
 *               or %NULL.
 **/
ThunarFile*
thunar_properties_dialog_get_file (ThunarPropertiesDialog *dialog)
{
  _thunar_return_val_if_fail (THUNAR_IS_PROPERTIES_DIALOG (dialog), NULL);
  return dialog->file;
}



/**
 * thunar_properties_dialog_set_file:
 * @dialog : a #ThunarPropertiesDialog.
 * @file   : a #ThunarFile or %NULL.
 *
 * Sets the #ThunarFile that is displayed by @dialog
 * to @file.
 **/
void
thunar_properties_dialog_set_file (ThunarPropertiesDialog *dialog,
                                   ThunarFile             *file)
{
  _thunar_return_if_fail (THUNAR_IS_PROPERTIES_DIALOG (dialog));
  _thunar_return_if_fail (file == NULL || THUNAR_IS_FILE (file));

  /* check if we already display that file */
  if (G_UNLIKELY (dialog->file == file))
    return;

  /* disconnect from any previously set file */
  if (dialog->file != NULL)
    {
      /* unregister our file watch */
      thunar_file_unwatch (dialog->file);

      /* unregister handlers */
      g_signal_handlers_disconnect_by_func (G_OBJECT (dialog->file), thunar_properties_dialog_update, dialog);
      g_signal_handlers_disconnect_by_func (G_OBJECT (dialog->file), gtk_widget_destroy, dialog);

      g_object_unref (G_OBJECT (dialog->file));
    }

  /* activate the new file */
  dialog->file = file;

  /* connect to the new file */
  if (file != NULL)
    {
      g_object_ref (G_OBJECT (file));

      /* watch the file for changes */
      thunar_file_watch (file);

      /* install signal handlers */
      g_signal_connect_swapped (G_OBJECT (file), "changed", G_CALLBACK (thunar_properties_dialog_update), dialog);
      g_signal_connect_swapped (G_OBJECT (file), "destroy", G_CALLBACK (gtk_widget_destroy), dialog);

      /* update the UI for the new file */
      thunar_properties_dialog_update (dialog);

      /* update the provider property pages */
      thunar_properties_dialog_update_providers (dialog);

      /* hide the permissions chooser for trashed files */
      if (thunar_file_is_trashed (file))
        gtk_widget_hide (dialog->permissions_chooser);
      else
        gtk_widget_show (dialog->permissions_chooser);
    }

  /* tell everybody that we have a new file here */
  g_object_notify (G_OBJECT (dialog), "file");
}



