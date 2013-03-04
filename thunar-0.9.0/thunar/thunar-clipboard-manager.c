/* $Id: thunar-clipboard-manager.c 26421 2007-12-02 13:46:28Z benny $ */
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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <thunar/thunar-application.h>
#include <thunar/thunar-clipboard-manager.h>
#include <thunar/thunar-dialogs.h>
#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-private.h>



enum
{
  PROP_0,
  PROP_CAN_PASTE,
};

enum
{
  CHANGED,
  LAST_SIGNAL,
};

enum
{
  TARGET_GNOME_COPIED_FILES,
  TARGET_UTF8_STRING,
};



static void thunar_clipboard_manager_class_init         (ThunarClipboardManagerClass *klass);
static void thunar_clipboard_manager_init               (ThunarClipboardManager      *manager);
static void thunar_clipboard_manager_finalize           (GObject                     *object);
static void thunar_clipboard_manager_get_property       (GObject                     *object,
                                                         guint                        prop_id,
                                                         GValue                      *value,
                                                         GParamSpec                  *pspec);
static void thunar_clipboard_manager_file_destroyed     (ThunarFile                  *file,
                                                         ThunarClipboardManager      *manager);
static void thunar_clipboard_manager_owner_changed      (GtkClipboard                *clipboard,
                                                         GdkEventOwnerChange         *event,
                                                         ThunarClipboardManager      *manager);
static void thunar_clipboard_manager_contents_received  (GtkClipboard                *clipboard,
                                                         GtkSelectionData            *selection_data,
                                                         gpointer                     user_data);
static void thunar_clipboard_manager_targets_received   (GtkClipboard                *clipboard,
                                                         GtkSelectionData            *selection_data,
                                                         gpointer                     user_data);
static void thunar_clipboard_manager_get_callback       (GtkClipboard                *clipboard,
                                                         GtkSelectionData            *selection_data,
                                                         guint                        info,
                                                         gpointer                     user_data);
static void thunar_clipboard_manager_clear_callback     (GtkClipboard                *clipboard,
                                                         gpointer                     user_data);
static void thunar_clipboard_manager_transfer_files     (ThunarClipboardManager      *manager,
                                                         gboolean                     copy,
                                                         GList                       *files);



struct _ThunarClipboardManagerClass
{
  GObjectClass __parent__;

  void (*changed) (ThunarClipboardManager *manager);
};

struct _ThunarClipboardManager
{
  GObject __parent__;

  GtkClipboard *clipboard;
  gboolean      can_paste;
  GdkAtom       x_special_gnome_copied_files;

  gboolean      files_cutted;
  GList        *files;
};

typedef struct
{
  ThunarClipboardManager *manager;
  ThunarVfsPath          *target_path;
  GtkWidget              *widget;
  GClosure               *new_files_closure;
} ThunarClipboardPasteRequest;



static const GtkTargetEntry clipboard_targets[] =
{
  { "x-special/gnome-copied-files", 0, TARGET_GNOME_COPIED_FILES },
  { "UTF8_STRING", 0, TARGET_UTF8_STRING }
};

static GObjectClass *thunar_clipboard_manager_parent_class;
static GQuark        thunar_clipboard_manager_quark = 0;
static guint         manager_signals[LAST_SIGNAL];



GType
thunar_clipboard_manager_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarClipboardManagerClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_clipboard_manager_class_init,
        NULL,
        NULL,
        sizeof (ThunarClipboardManager),
        0,
        (GInstanceInitFunc) thunar_clipboard_manager_init,
        NULL,
      };

      type = g_type_register_static (G_TYPE_OBJECT, I_("ThunarClipboardManager"), &info, 0);
    }

  return type;
}



static void
thunar_clipboard_manager_class_init (ThunarClipboardManagerClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  thunar_clipboard_manager_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_clipboard_manager_finalize;
  gobject_class->get_property = thunar_clipboard_manager_get_property;

  /**
   * ThunarClipboardManager:can-paste:
   *
   * This property tells whether the current clipboard content of
   * this #ThunarClipboardManager can be pasted into a folder
   * displayed by a #ThunarView.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_CAN_PASTE,
                                   g_param_spec_boolean ("can-paste", "can-pase", "can-paste",
                                                         FALSE,
                                                         EXO_PARAM_READABLE));

  /**
   * ThunarClipboardManager::changed:
   * @manager : a #ThunarClipboardManager.
   *
   * This signal is emitted whenever the contents of the
   * clipboard associated with @manager changes.
   **/
  manager_signals[CHANGED] =
    g_signal_new (I_("changed"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (ThunarClipboardManagerClass, changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}



static void
thunar_clipboard_manager_init (ThunarClipboardManager *manager)
{
  manager->x_special_gnome_copied_files = gdk_atom_intern ("x-special/gnome-copied-files", FALSE);
}



static void
thunar_clipboard_manager_finalize (GObject *object)
{
  ThunarClipboardManager *manager = THUNAR_CLIPBOARD_MANAGER (object);
  GList                  *lp;

  /* release any pending files */
  for (lp = manager->files; lp != NULL; lp = lp->next)
    {
      g_signal_handlers_disconnect_by_func (G_OBJECT (lp->data), thunar_clipboard_manager_file_destroyed, manager);
      g_object_unref (G_OBJECT (lp->data));
    }
  g_list_free (manager->files);

  /* disconnect from the clipboard */
  g_signal_handlers_disconnect_by_func (G_OBJECT (manager->clipboard), thunar_clipboard_manager_owner_changed, manager);
  g_object_set_qdata (G_OBJECT (manager->clipboard), thunar_clipboard_manager_quark, NULL);
  g_object_unref (G_OBJECT (manager->clipboard));

  (*G_OBJECT_CLASS (thunar_clipboard_manager_parent_class)->finalize) (object);
}



static void
thunar_clipboard_manager_get_property (GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  ThunarClipboardManager *manager = THUNAR_CLIPBOARD_MANAGER (object);

  switch (prop_id)
    {
    case PROP_CAN_PASTE:
      g_value_set_boolean (value, thunar_clipboard_manager_get_can_paste (manager));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_clipboard_manager_file_destroyed (ThunarFile             *file,
                                         ThunarClipboardManager *manager)
{
  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  _thunar_return_if_fail (g_list_find (manager->files, file) != NULL);

  /* remove the file from our list */
  manager->files = g_list_remove (manager->files, file);

  /* disconnect from the file */
  g_signal_handlers_disconnect_by_func (G_OBJECT (file), thunar_clipboard_manager_file_destroyed, manager);
  g_object_unref (G_OBJECT (file));
}



static void
thunar_clipboard_manager_owner_changed (GtkClipboard           *clipboard,
                                        GdkEventOwnerChange    *event,
                                        ThunarClipboardManager *manager)
{
  _thunar_return_if_fail (GTK_IS_CLIPBOARD (clipboard));
  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  _thunar_return_if_fail (manager->clipboard == clipboard);

  /* need to take a reference on the manager, because the clipboards
   * "targets received callback" mechanism is not cancellable.
   */
  g_object_ref (G_OBJECT (manager));

  /* request the list of supported targets from the new owner */
  gtk_clipboard_request_contents (clipboard, gdk_atom_intern ("TARGETS", FALSE),
                                  thunar_clipboard_manager_targets_received, manager);
}



static void
thunar_clipboard_manager_contents_received (GtkClipboard     *clipboard,
                                            GtkSelectionData *selection_data,
                                            gpointer          user_data)
{
  ThunarClipboardPasteRequest *request = user_data;
  ThunarClipboardManager      *manager = THUNAR_CLIPBOARD_MANAGER (request->manager);
  ThunarApplication           *application;
  gboolean                     path_copy = TRUE;
  GList                       *path_list = NULL;
  gchar                       *data;

  /* check whether the retrieval worked */
  if (G_LIKELY (selection_data->length > 0))
    {
      /* be sure the selection data is zero-terminated */
      data = (gchar *) selection_data->data;
      data[selection_data->length] = '\0';

      /* check whether to copy or move */
      if (g_ascii_strncasecmp (data, "copy\n", 5) == 0)
        {
          path_copy = TRUE;
          data += 5;
        }
      else if (g_ascii_strncasecmp (data, "cut\n", 4) == 0)
        {
          path_copy = FALSE;
          data += 4;
        }

      /* determine the path list stored with the selection */
      path_list = thunar_vfs_path_list_from_string (data, NULL);
    }

  /* perform the action if possible */
  if (G_LIKELY (path_list != NULL))
    {
      application = thunar_application_get ();
      if (G_LIKELY (path_copy))
        thunar_application_copy_into (application, request->widget, path_list, request->target_path, request->new_files_closure);
      else
        thunar_application_move_into (application, request->widget, path_list, request->target_path, request->new_files_closure);
      g_object_unref (G_OBJECT (application));
      thunar_vfs_path_list_free (path_list);

      /* clear the clipboard if it contained "cutted data"
       * (gtk_clipboard_clear takes care of not clearing
       * the selection if we don't own it)
       */
      if (G_UNLIKELY (!path_copy))
        gtk_clipboard_clear (manager->clipboard);

      /* check the contents of the clipboard again
       * if either the Xserver or our GTK+ version
       * doesn't support the XFixes extension.
       */
#if GTK_CHECK_VERSION(2,6,0)
      if (!gdk_display_supports_selection_notification (gtk_clipboard_get_display (manager->clipboard)))
#endif
        {
          thunar_clipboard_manager_owner_changed (manager->clipboard, NULL, manager);
        }
    }
  else
    {
      /* tell the user that we cannot paste */
      thunar_dialogs_show_error (request->widget, NULL, _("There is nothing on the clipboard to paste"));
    }

  /* free the request */
  if (G_LIKELY (request->widget != NULL))
    g_object_remove_weak_pointer (G_OBJECT (request->widget), (gpointer) &request->widget);
  if (G_LIKELY (request->new_files_closure != NULL))
    g_closure_unref (request->new_files_closure);
  g_object_unref (G_OBJECT (request->manager));
  thunar_vfs_path_unref (request->target_path);
  _thunar_slice_free (ThunarClipboardPasteRequest, request);
}



static void
thunar_clipboard_manager_targets_received (GtkClipboard     *clipboard,
                                           GtkSelectionData *selection_data,
                                           gpointer          user_data)
{
  ThunarClipboardManager *manager = THUNAR_CLIPBOARD_MANAGER (user_data);
  GdkAtom                *targets;
  gint                    n_targets;
  gint                    n;

  _thunar_return_if_fail (GTK_IS_CLIPBOARD (clipboard));
  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  _thunar_return_if_fail (manager->clipboard == clipboard);

  /* reset the "can-paste" state */
  manager->can_paste = FALSE;

  /* check the list of targets provided by the owner */
  if (gtk_selection_data_get_targets (selection_data, &targets, &n_targets))
    {
      for (n = 0; n < n_targets; ++n)
        if (targets[n] == manager->x_special_gnome_copied_files)
          {
            manager->can_paste = TRUE;
            break;
          }

      g_free (targets);
    }

  /* notify listeners that we have a new clipboard state */
  g_signal_emit (G_OBJECT (manager), manager_signals[CHANGED], 0);
  g_object_notify (G_OBJECT (manager), "can-paste");

  /* drop the reference taken for the callback */
  g_object_unref (G_OBJECT (manager));
}



static void
thunar_clipboard_manager_get_callback (GtkClipboard     *clipboard,
                                       GtkSelectionData *selection_data,
                                       guint             target_info,
                                       gpointer          user_data)
{
  ThunarClipboardManager *manager = THUNAR_CLIPBOARD_MANAGER (user_data);
  GList                  *path_list = NULL;
  gchar                  *string_list;
  gchar                  *data;

  _thunar_return_if_fail (GTK_IS_CLIPBOARD (clipboard));
  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  _thunar_return_if_fail (manager->clipboard == clipboard);

  /* determine the path list from the file list */
  path_list = thunar_file_list_to_path_list (manager->files);

  /* determine the string representation of the path list */
  string_list = thunar_vfs_path_list_to_string (path_list);

  switch (target_info)
    {
    case TARGET_GNOME_COPIED_FILES:
      data = g_strconcat (manager->files_cutted ? "cut\n" : "copy\n", string_list, NULL);
      gtk_selection_data_set (selection_data, selection_data->target, 8, (guchar *) data, strlen (data));
      g_free (data);
      break;

    case TARGET_UTF8_STRING:
      gtk_selection_data_set (selection_data, selection_data->target, 8, (guchar *) string_list, strlen (string_list));
      break;

    default:
      _thunar_assert_not_reached ();
    }

  /* cleanup */
  thunar_vfs_path_list_free (path_list);
  g_free (string_list);
}



static void
thunar_clipboard_manager_clear_callback (GtkClipboard *clipboard,
                                         gpointer      user_data)
{
  ThunarClipboardManager *manager = THUNAR_CLIPBOARD_MANAGER (user_data);
  GList                  *lp;

  _thunar_return_if_fail (GTK_IS_CLIPBOARD (clipboard));
  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  _thunar_return_if_fail (manager->clipboard == clipboard);

  /* release the pending files */
  for (lp = manager->files; lp != NULL; lp = lp->next)
    {
      g_signal_handlers_disconnect_by_func (G_OBJECT (lp->data), thunar_clipboard_manager_file_destroyed, manager);
      g_object_unref (G_OBJECT (lp->data));
    }
  g_list_free (manager->files);
  manager->files = NULL;
}



static void
thunar_clipboard_manager_transfer_files (ThunarClipboardManager *manager,
                                         gboolean                copy,
                                         GList                  *files)
{
  ThunarFile *file;
  GList      *lp;

  /* release any pending files */
  for (lp = manager->files; lp != NULL; lp = lp->next)
    {
      g_signal_handlers_disconnect_by_func (G_OBJECT (lp->data), thunar_clipboard_manager_file_destroyed, manager);
      g_object_unref (G_OBJECT (lp->data));
    }
  g_list_free (manager->files);

  /* remember the transfer operation */
  manager->files_cutted = !copy;

  /* setup the new file list */
  for (lp = files, manager->files = NULL; lp != NULL; lp = lp->next)
    {
      file = g_object_ref (G_OBJECT (lp->data));
      manager->files = g_list_prepend (manager->files, file);
      g_signal_connect (G_OBJECT (file), "destroy", G_CALLBACK (thunar_clipboard_manager_file_destroyed), manager);
    }

  /* acquire the CLIPBOARD ownership */
  gtk_clipboard_set_with_owner (manager->clipboard, clipboard_targets,
                                G_N_ELEMENTS (clipboard_targets),
                                thunar_clipboard_manager_get_callback,
                                thunar_clipboard_manager_clear_callback,
                                G_OBJECT (manager));

  /* Need to fake a "owner-change" event here if the Xserver doesn't support clipboard notification */
#if GTK_CHECK_VERSION(2,6,0)
  if (!gdk_display_supports_selection_notification (gtk_clipboard_get_display (manager->clipboard)))
#endif
    {
      thunar_clipboard_manager_owner_changed (manager->clipboard, NULL, manager);
    }
}



/**
 * thunar_clipboard_manager_get_for_display:
 * @display : a #GdkDisplay.
 *
 * Determines the #ThunarClipboardManager that is used to manage
 * the clipboard on the given @display.
 *
 * The caller is responsible for freeing the returned object
 * using g_object_unref() when it's no longer needed.
 *
 * Return value: the #ThunarClipboardManager for @display.
 **/
ThunarClipboardManager*
thunar_clipboard_manager_get_for_display (GdkDisplay *display)
{
  ThunarClipboardManager *manager;
  GtkClipboard           *clipboard;

  _thunar_return_val_if_fail (GDK_IS_DISPLAY (display), NULL);

  /* generate the quark on-demand */
  if (G_UNLIKELY (thunar_clipboard_manager_quark == 0))
    thunar_clipboard_manager_quark = g_quark_from_static_string ("thunar-clipboard-manager");

  /* figure out the clipboard for the given display */
  clipboard = gtk_clipboard_get_for_display (display, GDK_SELECTION_CLIPBOARD);

  /* check if a clipboard manager exists */
  manager = g_object_get_qdata (G_OBJECT (clipboard), thunar_clipboard_manager_quark);
  if (G_LIKELY (manager != NULL))
    {
      g_object_ref (G_OBJECT (manager));
      return manager;
    }

  /* allocate a new manager */
  manager = g_object_new (THUNAR_TYPE_CLIPBOARD_MANAGER, NULL);
  manager->clipboard = g_object_ref (G_OBJECT (clipboard));
  g_object_set_qdata (G_OBJECT (clipboard), thunar_clipboard_manager_quark, manager);

  /* listen for the "owner-change" signal on the clipboard */
  g_signal_connect (G_OBJECT (manager->clipboard), "owner-change",
                    G_CALLBACK (thunar_clipboard_manager_owner_changed), manager);

  return manager;
}



/**
 * thunar_clipboard_manager_get_can_paste:
 * @manager : a #ThunarClipboardManager.
 *
 * Tells whether the contents of the clipboard represented
 * by @manager can be pasted into a folder.
 *
 * Return value: %TRUE if the contents of the clipboard
 *               represented by @manager can be pasted
 *               into a folder.
 **/
gboolean
thunar_clipboard_manager_get_can_paste (ThunarClipboardManager *manager)
{
  _thunar_return_val_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager), FALSE);
  return manager->can_paste;
}



/**
 * thunar_clipboard_manager_has_cutted_file:
 * @manager : a #ThunarClipboardManager.
 * @file    : a #ThunarFile.
 *
 * Checks whether @file was cutted to the given @manager earlier.
 *
 * Return value: %TRUE if @file is on the cutted list of @manager.
 **/
gboolean
thunar_clipboard_manager_has_cutted_file (ThunarClipboardManager *manager,
                                          const ThunarFile       *file)
{
  _thunar_return_val_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager), FALSE);
  _thunar_return_val_if_fail (THUNAR_IS_FILE (file), FALSE);

  return (manager->files_cutted && g_list_find (manager->files, file) != NULL);
}



/**
 * thunar_clipboard_manager_copy_files:
 * @manager : a #ThunarClipboardManager.
 * @files   : a list of #ThunarFile<!---->s.
 *
 * Sets the clipboard represented by @manager to
 * contain the @files and marks them to be copied
 * when the user pastes from the clipboard.
 **/
void
thunar_clipboard_manager_copy_files (ThunarClipboardManager *manager,
                                     GList                  *files)
{
  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  thunar_clipboard_manager_transfer_files (manager, TRUE, files);
}



/**
 * thunar_clipboard_manager_cut_files:
 * @manager : a #ThunarClipboardManager.
 * @files   : a list of #ThunarFile<!---->s.
 *
 * Sets the clipboard represented by @manager to
 * contain the @files and marks them to be moved
 * when the user pastes from the clipboard.
 **/
void
thunar_clipboard_manager_cut_files (ThunarClipboardManager *manager,
                                    GList                  *files)
{
  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  thunar_clipboard_manager_transfer_files (manager, FALSE, files);
}



/**
 * thunar_clipboard_manager_paste_files:
 * @manager           : a #ThunarClipboardManager.
 * @target_path       : the #ThunarVfsPath of the folder to which the contents on the clipboard
 *                      should be pasted.
 * @widget            : a #GtkWidget, on which to perform the paste or %NULL if no widget is
 *                      known.
 * @new_files_closure : a #GClosure to connect to the job's "new-files" signal,
 *                      which will be emitted when the job finishes with the
 *                      list of #ThunarVfsPath<!---->s created by the job, or
 *                      %NULL if you're not interested in the signal.
 *
 * Pastes the contents from the clipboard associated with @manager to the directory
 * referenced by @target_path.
 **/
void
thunar_clipboard_manager_paste_files (ThunarClipboardManager *manager,
                                      ThunarVfsPath          *target_path,
                                      GtkWidget              *widget,
                                      GClosure               *new_files_closure)
{
  ThunarClipboardPasteRequest *request;

  _thunar_return_if_fail (THUNAR_IS_CLIPBOARD_MANAGER (manager));
  _thunar_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));

  /* prepare the paste request */
  request = _thunar_slice_new0 (ThunarClipboardPasteRequest);
  request->manager = g_object_ref (G_OBJECT (manager));
  request->target_path = thunar_vfs_path_ref (target_path);
  request->widget = widget;

  /* take a reference on the closure (if any) */
  if (G_LIKELY (new_files_closure != NULL))
    {
      request->new_files_closure = new_files_closure;
      g_closure_ref (new_files_closure);
      g_closure_sink (new_files_closure);
    }

  /* get notified when the widget is destroyed prior to
   * completing the clipboard contents retrieval
   */
  if (G_LIKELY (request->widget != NULL))
    g_object_add_weak_pointer (G_OBJECT (request->widget), (gpointer) &request->widget);

  /* schedule the request */
  gtk_clipboard_request_contents (manager->clipboard, manager->x_special_gnome_copied_files,
                                  thunar_clipboard_manager_contents_received, request);
}

