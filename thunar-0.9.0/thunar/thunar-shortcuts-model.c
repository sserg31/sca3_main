/* $Id: thunar-shortcuts-model.c 26421 2007-12-02 13:46:28Z benny $ */
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

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <thunar/thunar-file.h>
#include <thunar/thunar-shortcuts-model.h>
#include <thunar/thunar-private.h>



#define THUNAR_SHORTCUT(obj) ((ThunarShortcut *) (obj))



typedef struct _ThunarShortcut ThunarShortcut;

typedef enum
{
  THUNAR_SHORTCUT_SEPARATOR,
  THUNAR_SHORTCUT_SYSTEM_DEFINED,
  THUNAR_SHORTCUT_REMOVABLE_MEDIA,
  THUNAR_SHORTCUT_USER_DEFINED,
} ThunarShortcutType;



static void               thunar_shortcuts_model_class_init         (ThunarShortcutsModelClass *klass);
static void               thunar_shortcuts_model_tree_model_init    (GtkTreeModelIface         *iface);
static void               thunar_shortcuts_model_drag_source_init   (GtkTreeDragSourceIface    *iface);
static void               thunar_shortcuts_model_init               (ThunarShortcutsModel      *model);
static void               thunar_shortcuts_model_finalize           (GObject                   *object);
static GtkTreeModelFlags  thunar_shortcuts_model_get_flags          (GtkTreeModel              *tree_model);
static gint               thunar_shortcuts_model_get_n_columns      (GtkTreeModel              *tree_model);
static GType              thunar_shortcuts_model_get_column_type    (GtkTreeModel              *tree_model,
                                                                     gint                       index);
static gboolean           thunar_shortcuts_model_get_iter           (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreePath               *path);
static GtkTreePath       *thunar_shortcuts_model_get_path           (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static void               thunar_shortcuts_model_get_value          (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     gint                       column,
                                                                     GValue                    *value);
static gboolean           thunar_shortcuts_model_iter_next          (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static gboolean           thunar_shortcuts_model_iter_children      (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreeIter               *parent);
static gboolean           thunar_shortcuts_model_iter_has_child     (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static gint               thunar_shortcuts_model_iter_n_children    (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter);
static gboolean           thunar_shortcuts_model_iter_nth_child     (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreeIter               *parent,
                                                                     gint                       n);
static gboolean           thunar_shortcuts_model_iter_parent        (GtkTreeModel              *tree_model,
                                                                     GtkTreeIter               *iter,
                                                                     GtkTreeIter               *child);
static gboolean           thunar_shortcuts_model_row_draggable      (GtkTreeDragSource         *source,
                                                                     GtkTreePath               *path);
static gboolean           thunar_shortcuts_model_drag_data_get      (GtkTreeDragSource         *source,
                                                                     GtkTreePath               *path,
                                                                     GtkSelectionData          *selection_data);
static gboolean           thunar_shortcuts_model_drag_data_delete   (GtkTreeDragSource         *source,
                                                                     GtkTreePath               *path);
static void               thunar_shortcuts_model_add_shortcut       (ThunarShortcutsModel      *model,
                                                                     ThunarShortcut            *shortcut,
                                                                     GtkTreePath               *path);
static void               thunar_shortcuts_model_remove_shortcut    (ThunarShortcutsModel      *model,
                                                                     ThunarShortcut            *shortcut);
static void               thunar_shortcuts_model_load               (ThunarShortcutsModel      *model);
static void               thunar_shortcuts_model_save               (ThunarShortcutsModel      *model);
static void               thunar_shortcuts_model_monitor            (ThunarVfsMonitor          *monitor,
                                                                     ThunarVfsMonitorHandle    *handle,
                                                                     ThunarVfsMonitorEvent      event,
                                                                     ThunarVfsPath             *handle_path,
                                                                     ThunarVfsPath             *event_path,
                                                                     gpointer                   user_data);
static void               thunar_shortcuts_model_file_changed       (ThunarFile                *file,
                                                                     ThunarShortcutsModel      *model);
static void               thunar_shortcuts_model_file_destroy       (ThunarFile                *file,
                                                                     ThunarShortcutsModel      *model);
static void               thunar_shortcuts_model_volume_changed     (ThunarVfsVolume           *volume,
                                                                     ThunarShortcutsModel      *model);
static void               thunar_shortcuts_model_volumes_added      (ThunarVfsVolumeManager    *volume_manager,
                                                                     GList                     *volumes,
                                                                     ThunarShortcutsModel      *model);
static void               thunar_shortcuts_model_volumes_removed    (ThunarVfsVolumeManager    *volume_manager,
                                                                     GList                     *volumes,
                                                                     ThunarShortcutsModel      *model);
static void               thunar_shortcut_free                      (ThunarShortcut            *shortcut,
                                                                     ThunarShortcutsModel      *model);



struct _ThunarShortcutsModelClass
{
  GObjectClass __parent__;
};

struct _ThunarShortcutsModel
{
  GObject __parent__;

  /* the model stamp is only used when debugging is
   * enabled, to make sure we don't accept iterators
   * generated by another model.
   */
#ifndef NDEBUG
  guint                   stamp;
#endif

  GList                  *shortcuts;
  GList                  *hidden_volumes;
  ThunarVfsVolumeManager *volume_manager;

  ThunarVfsMonitor       *monitor;
  ThunarVfsMonitorHandle *handle;
};

struct _ThunarShortcut
{
  ThunarShortcutType type;

  gchar              *name;
  ThunarFile         *file;
  ThunarVfsVolume    *volume;
};



static GObjectClass *thunar_shortcuts_model_parent_class;



GType
thunar_shortcuts_model_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarShortcutsModelClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_shortcuts_model_class_init,
        NULL,
        NULL,
        sizeof (ThunarShortcutsModel),
        0,
        (GInstanceInitFunc) thunar_shortcuts_model_init,
        NULL,
      };

      static const GInterfaceInfo tree_model_info =
      {
        (GInterfaceInitFunc) thunar_shortcuts_model_tree_model_init,
        NULL,
        NULL,
      };

      static const GInterfaceInfo drag_source_info =
      {
        (GInterfaceInitFunc) thunar_shortcuts_model_drag_source_init,
        NULL,
        NULL,
      };

      type = g_type_register_static (G_TYPE_OBJECT, I_("ThunarShortcutsModel"), &info, 0);
      g_type_add_interface_static (type, GTK_TYPE_TREE_MODEL, &tree_model_info);
      g_type_add_interface_static (type, GTK_TYPE_TREE_DRAG_SOURCE, &drag_source_info);
    }

  return type;
}


    
static void
thunar_shortcuts_model_class_init (ThunarShortcutsModelClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  thunar_shortcuts_model_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_shortcuts_model_finalize;
}



static void
thunar_shortcuts_model_tree_model_init (GtkTreeModelIface *iface)
{
  iface->get_flags = thunar_shortcuts_model_get_flags;
  iface->get_n_columns = thunar_shortcuts_model_get_n_columns;
  iface->get_column_type = thunar_shortcuts_model_get_column_type;
  iface->get_iter = thunar_shortcuts_model_get_iter;
  iface->get_path = thunar_shortcuts_model_get_path;
  iface->get_value = thunar_shortcuts_model_get_value;
  iface->iter_next = thunar_shortcuts_model_iter_next;
  iface->iter_children = thunar_shortcuts_model_iter_children;
  iface->iter_has_child = thunar_shortcuts_model_iter_has_child;
  iface->iter_n_children = thunar_shortcuts_model_iter_n_children;
  iface->iter_nth_child = thunar_shortcuts_model_iter_nth_child;
  iface->iter_parent = thunar_shortcuts_model_iter_parent;
}



static void
thunar_shortcuts_model_drag_source_init (GtkTreeDragSourceIface *iface)
{
  iface->row_draggable = thunar_shortcuts_model_row_draggable;
  iface->drag_data_get = thunar_shortcuts_model_drag_data_get;
  iface->drag_data_delete = thunar_shortcuts_model_drag_data_delete;
}



static void
thunar_shortcuts_model_init (ThunarShortcutsModel *model)
{
  ThunarVfsVolume *volume;
  ThunarShortcut  *shortcut;
  ThunarVfsPath   *system_path_list[4];
  ThunarVfsPath   *fhome;
  ThunarVfsPath   *fpath;
  GtkTreePath     *path;
  ThunarFile      *file;
  GList           *volumes;
  GList           *lp;
  guint            n;

#ifndef NDEBUG
  model->stamp = g_random_int ();
#endif

  /* connect to the volume manager */
  model->volume_manager = thunar_vfs_volume_manager_get_default ();
  g_signal_connect (G_OBJECT (model->volume_manager), "volumes-added", G_CALLBACK (thunar_shortcuts_model_volumes_added), model);
  g_signal_connect (G_OBJECT (model->volume_manager), "volumes-removed", G_CALLBACK (thunar_shortcuts_model_volumes_removed), model);

  /* determine the system-defined paths */
  system_path_list[0] = thunar_vfs_path_get_for_home ();
  system_path_list[1] = thunar_vfs_path_get_for_trash ();
  system_path_list[2] = thunar_vfs_path_relative (system_path_list[0], "Desktop");
  system_path_list[3] = thunar_vfs_path_get_for_root ();

  /* will be used to append the shortcuts to the list */
  path = gtk_tree_path_new_from_indices (0, -1);

  /* append the system defined items ('Home', 'Trash', 'File System') */
  for (n = 0; n < G_N_ELEMENTS (system_path_list); ++n)
    {
      /* determine the file for the path */
      file = thunar_file_get_for_path (system_path_list[n], NULL);
      if (G_LIKELY (file != NULL))
        {
          /* create the shortcut */
          shortcut = _thunar_slice_new0 (ThunarShortcut);
          shortcut->type = THUNAR_SHORTCUT_SYSTEM_DEFINED;
          shortcut->file = file;

          /* append the shortcut to the list */
          thunar_shortcuts_model_add_shortcut (model, shortcut, path);
          gtk_tree_path_next (path);
        }

      /* release the system defined path */
      thunar_vfs_path_unref (system_path_list[n]);
    }

  /* prepend the removable media volumes */
  volumes = thunar_vfs_volume_manager_get_volumes (model->volume_manager);
  for (lp = volumes; lp != NULL; lp = lp->next)
    {
      /* monitor the volume for changes */
      volume = THUNAR_VFS_VOLUME (lp->data);
      g_object_ref (G_OBJECT (volume));
      g_signal_connect (G_OBJECT (volume), "changed",
                        G_CALLBACK (thunar_shortcuts_model_volume_changed), model);

      /* we list only present, removable devices here */
      if (thunar_vfs_volume_is_removable (volume) && thunar_vfs_volume_is_present (volume))
        {
          /* generate the shortcut (w/o a file, else we might
           * prevent the volume from being unmounted)
           */
          shortcut = _thunar_slice_new0 (ThunarShortcut);
          shortcut->type = THUNAR_SHORTCUT_REMOVABLE_MEDIA;
          shortcut->volume = volume;

          /* link the shortcut to the list */
          thunar_shortcuts_model_add_shortcut (model, shortcut, path);
          gtk_tree_path_next (path);
        }
      else
        {
          /* schedule the volume for later checking, not removable or there's no medium present */
          model->hidden_volumes = g_list_prepend (model->hidden_volumes, volume);
        }
    }

  /* prepend the row separator (only supported with Gtk+ 2.6) */
#if GTK_CHECK_VERSION(2,6,0)
  shortcut = _thunar_slice_new0 (ThunarShortcut);
  shortcut->type = THUNAR_SHORTCUT_SEPARATOR;
  thunar_shortcuts_model_add_shortcut (model, shortcut, path);
  gtk_tree_path_next (path);
#endif

  /* determine the URI to the Gtk+ bookmarks file */
  fhome = thunar_vfs_path_get_for_home ();
  fpath = thunar_vfs_path_relative (fhome, ".gtk-bookmarks");
  thunar_vfs_path_unref (fhome);

  /* register with the alteration monitor for the bookmarks file */
  model->monitor = thunar_vfs_monitor_get_default ();
  model->handle = thunar_vfs_monitor_add_file (model->monitor, fpath, thunar_shortcuts_model_monitor, G_OBJECT (model));

  /* read the Gtk+ bookmarks file */
  thunar_shortcuts_model_load (model);

  /* cleanup */
  thunar_vfs_path_unref (fpath);
  gtk_tree_path_free (path);
}



static void
thunar_shortcuts_model_finalize (GObject *object)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (object);
  GList                *lp;

  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));

  /* free all shortcuts */
  g_list_foreach (model->shortcuts, (GFunc) thunar_shortcut_free, model);
  g_list_free (model->shortcuts);

  /* free all hidden volumes */
  for (lp = model->hidden_volumes; lp != NULL; lp = lp->next)
    {
      g_signal_handlers_disconnect_by_func (G_OBJECT (lp->data), thunar_shortcuts_model_volume_changed, model);
      g_object_unref (G_OBJECT (lp->data));
    }
  g_list_free (model->hidden_volumes);

  /* detach from the VFS monitor */
  thunar_vfs_monitor_remove (model->monitor, model->handle);
  g_object_unref (G_OBJECT (model->monitor));

  /* unlink from the volume manager */
  g_signal_handlers_disconnect_matched (G_OBJECT (model->volume_manager), G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, model);
  g_object_unref (G_OBJECT (model->volume_manager));

  (*G_OBJECT_CLASS (thunar_shortcuts_model_parent_class)->finalize) (object);
}



static GtkTreeModelFlags
thunar_shortcuts_model_get_flags (GtkTreeModel *tree_model)
{
  return GTK_TREE_MODEL_ITERS_PERSIST | GTK_TREE_MODEL_LIST_ONLY;
}



static gint
thunar_shortcuts_model_get_n_columns (GtkTreeModel *tree_model)
{
  return THUNAR_SHORTCUTS_MODEL_N_COLUMNS;
}



static GType
thunar_shortcuts_model_get_column_type (GtkTreeModel *tree_model,
                                        gint          index)
{
  switch (index)
    {
    case THUNAR_SHORTCUTS_MODEL_COLUMN_NAME:
      return G_TYPE_STRING;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_FILE:
      return THUNAR_TYPE_FILE;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_VOLUME:
      return THUNAR_VFS_TYPE_VOLUME;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_MUTABLE:
      return G_TYPE_BOOLEAN;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_SEPARATOR:
      return G_TYPE_BOOLEAN;
    }

  _thunar_assert_not_reached ();
  return G_TYPE_INVALID;
}



static gboolean
thunar_shortcuts_model_get_iter (GtkTreeModel *tree_model,
                                 GtkTreeIter  *iter,
                                 GtkTreePath  *path)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (tree_model);
  GList                 *lp;

  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), FALSE);
  _thunar_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

  /* determine the list item for the path */
  lp = g_list_nth (model->shortcuts, gtk_tree_path_get_indices (path)[0]);
  if (G_LIKELY (lp != NULL))
    {
      GTK_TREE_ITER_INIT (*iter, model->stamp, lp);
      return TRUE;
    }

  return FALSE;
}



static GtkTreePath*
thunar_shortcuts_model_get_path (GtkTreeModel *tree_model,
                                 GtkTreeIter  *iter)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (tree_model);
  gint                   index;

  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), NULL);
  _thunar_return_val_if_fail (iter->stamp == model->stamp, NULL);

  /* lookup the list item in the shortcuts list */
  index = g_list_position (model->shortcuts, iter->user_data);
  if (G_LIKELY (index >= 0))
    return gtk_tree_path_new_from_indices (index, -1);

  return NULL;
}



static void
thunar_shortcuts_model_get_value (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter,
                                  gint          column,
                                  GValue       *value)
{
  ThunarShortcut *shortcut;
  ThunarFile     *file;

  _thunar_return_if_fail (iter->stamp == THUNAR_SHORTCUTS_MODEL (tree_model)->stamp);
  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (tree_model));

  /* determine the shortcut for the list item */
  shortcut = THUNAR_SHORTCUT (((GList *) iter->user_data)->data);

  switch (column)
    {
    case THUNAR_SHORTCUTS_MODEL_COLUMN_NAME:
      g_value_init (value, G_TYPE_STRING);
      if (G_UNLIKELY (shortcut->volume != NULL))
        g_value_set_static_string (value, thunar_vfs_volume_get_name (shortcut->volume));
      else if (shortcut->name != NULL)
        g_value_set_static_string (value, shortcut->name);
      else if (shortcut->file != NULL)
        g_value_set_static_string (value, thunar_file_get_display_name (shortcut->file));
      else
        g_value_set_static_string (value, "");
      break;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_FILE:
      g_value_init (value, THUNAR_TYPE_FILE);
      if (shortcut->volume != NULL && shortcut->file == NULL)
        {
          /* try to determine the file on-demand */
          file = thunar_file_get_for_path (thunar_vfs_volume_get_mount_point (shortcut->volume), NULL);
          if (G_LIKELY (file != NULL))
            g_value_take_object (value, file);
        }
      else
        {
          g_value_set_object (value, shortcut->file);
        }
      break;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_VOLUME:
      g_value_init (value, THUNAR_VFS_TYPE_VOLUME);
      g_value_set_object (value, shortcut->volume);
      break;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_MUTABLE:
      g_value_init (value, G_TYPE_BOOLEAN);
      g_value_set_boolean (value, shortcut->type == THUNAR_SHORTCUT_USER_DEFINED);
      break;

    case THUNAR_SHORTCUTS_MODEL_COLUMN_SEPARATOR:
      g_value_init (value, G_TYPE_BOOLEAN);
      g_value_set_boolean (value, shortcut->type == THUNAR_SHORTCUT_SEPARATOR);
      break;

    default:
      _thunar_assert_not_reached ();
    }
}



static gboolean
thunar_shortcuts_model_iter_next (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter)
{
  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (tree_model), FALSE);
  _thunar_return_val_if_fail (iter->stamp == THUNAR_SHORTCUTS_MODEL (tree_model)->stamp, FALSE);

  iter->user_data = g_list_next (iter->user_data);
  return (iter->user_data != NULL);
}



static gboolean
thunar_shortcuts_model_iter_children (GtkTreeModel *tree_model,
                                      GtkTreeIter  *iter,
                                      GtkTreeIter  *parent)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (tree_model);

  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), FALSE);

  if (G_LIKELY (parent == NULL && model->shortcuts != NULL))
    {
      GTK_TREE_ITER_INIT (*iter, model->stamp, model->shortcuts);
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_shortcuts_model_iter_has_child (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter)
{
  return FALSE;
}



static gint
thunar_shortcuts_model_iter_n_children (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (tree_model);

  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), 0);

  return (iter == NULL) ? g_list_length (model->shortcuts) : 0;
}



static gboolean
thunar_shortcuts_model_iter_nth_child (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter,
                                       GtkTreeIter  *parent,
                                       gint          n)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (tree_model);

  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), FALSE);

  if (G_LIKELY (parent == NULL))
    {
      GTK_TREE_ITER_INIT (*iter, model->stamp, g_list_nth (model->shortcuts, n));
      return (iter->user_data != NULL);
    }

  return FALSE;
}



static gboolean
thunar_shortcuts_model_iter_parent (GtkTreeModel *tree_model,
                                    GtkTreeIter  *iter,
                                    GtkTreeIter  *child)
{
  return FALSE;
}



static gboolean
thunar_shortcuts_model_row_draggable (GtkTreeDragSource *source,
                                      GtkTreePath       *path)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (source);
  ThunarShortcut       *shortcut;

  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), FALSE);
  _thunar_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

  /* lookup the ThunarShortcut for the path */
  shortcut = g_list_nth_data (model->shortcuts, gtk_tree_path_get_indices (path)[0]);

  /* special shortcuts cannot be reordered */
  return (shortcut != NULL && shortcut->type == THUNAR_SHORTCUT_USER_DEFINED);
}



static gboolean
thunar_shortcuts_model_drag_data_get (GtkTreeDragSource *source,
                                      GtkTreePath       *path,
                                      GtkSelectionData  *selection_data)
{
  /* we simply return FALSE here, as the drag handling is done in
   * the ThunarShortcutsView class.
   */
  return FALSE;
}



static gboolean
thunar_shortcuts_model_drag_data_delete (GtkTreeDragSource *source,
                                         GtkTreePath       *path)
{
  /* we simply return FALSE here, as this function can only be
   * called if the user is re-arranging shortcuts within the
   * model, which will be handle by the exchange method.
   */
  return FALSE;
}



static void
thunar_shortcuts_model_add_shortcut (ThunarShortcutsModel *model,
                                     ThunarShortcut       *shortcut,
                                     GtkTreePath          *path)
{
  GtkTreeIter iter;

  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (shortcut->file == NULL || THUNAR_IS_FILE (shortcut->file));
  _thunar_return_if_fail (gtk_tree_path_get_depth (path) > 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (path)[0] >= 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (path)[0] <= g_list_length (model->shortcuts));

  /* we want to stay informed about changes to the file */
  if (G_LIKELY (shortcut->file != NULL))
    {
      /* watch the file for changes */
      thunar_file_watch (shortcut->file);

      /* connect appropriate signals */
      g_signal_connect (G_OBJECT (shortcut->file), "changed",
                        G_CALLBACK (thunar_shortcuts_model_file_changed), model);
      g_signal_connect (G_OBJECT (shortcut->file), "destroy",
                        G_CALLBACK (thunar_shortcuts_model_file_destroy), model);
    }

  /* insert the new shortcut to the shortcuts list */
  model->shortcuts = g_list_insert (model->shortcuts, shortcut, gtk_tree_path_get_indices (path)[0]);

  /* tell everybody that we have a new shortcut */
  gtk_tree_model_get_iter (GTK_TREE_MODEL (model), &iter, path);
  gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), path, &iter);
}



static void
thunar_shortcuts_model_remove_shortcut (ThunarShortcutsModel *model,
                                        ThunarShortcut       *shortcut)
{
  GtkTreePath *path;
  gint         index;

  /* determine the index of the shortcut */
  index = g_list_index (model->shortcuts, shortcut);
  if (G_LIKELY (index >= 0))
    {
      /* unlink the shortcut from the model */
      model->shortcuts = g_list_remove (model->shortcuts, shortcut);

      /* tell everybody that we have lost a shortcut */
      path = gtk_tree_path_new_from_indices (index, -1);
      gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), path);
      gtk_tree_path_free (path);

      /* actually free the shortcut */
      thunar_shortcut_free (shortcut, model);

      /* the shortcuts list was changed, so write the gtk bookmarks file */
      thunar_shortcuts_model_save (model);
    }
}



static void
thunar_shortcuts_model_load (ThunarShortcutsModel *model)
{
  ThunarShortcut *shortcut;
  ThunarVfsPath   *file_path;
  GtkTreePath     *path;
  ThunarFile      *file;
  gchar           *bookmarks_path;
  gchar            line[2048];
  gchar           *name;
  FILE            *fp;

  /* determine the path to the GTK+ bookmarks file */
  bookmarks_path = xfce_get_homefile (".gtk-bookmarks", NULL);

  /* append the GTK+ bookmarks (if any) */
  fp = fopen (bookmarks_path, "r");
  if (G_LIKELY (fp != NULL))
    {
      /* allocate a tree path for appending to the model */
      path = gtk_tree_path_new_from_indices (g_list_length (model->shortcuts), -1);

      while (fgets (line, sizeof (line), fp) != NULL)
        {
          /* strip leading/trailing whitespace */
          g_strstrip (line);

          /* skip over the URI */
          for (name = line; *name != '\0' && !g_ascii_isspace (*name); ++name)
            ;

          /* zero-terminate the URI */
          *name++ = '\0';

          /* check if we have a name */
          for (; g_ascii_isspace (*name); ++name)
            ;

          /* parse the URI */
          file_path = thunar_vfs_path_new (line, NULL);
          if (G_UNLIKELY (file_path == NULL))
            continue;

          /* try to open the file corresponding to the uri */
          file = thunar_file_get_for_path (file_path, NULL);
          thunar_vfs_path_unref (file_path);
          if (G_UNLIKELY (file == NULL))
            continue;

          /* make sure the file refers to a directory */
          if (G_UNLIKELY (!thunar_file_is_directory (file)))
            {
              g_object_unref (G_OBJECT (file));
              continue;
            }

          /* create the shortcut entry */
          shortcut = _thunar_slice_new0 (ThunarShortcut);
          shortcut->type = THUNAR_SHORTCUT_USER_DEFINED;
          shortcut->file = file;
          shortcut->name = (*name != '\0') ? g_strdup (name) : NULL;

          /* append the shortcut to the list */
          thunar_shortcuts_model_add_shortcut (model, shortcut, path);
          gtk_tree_path_next (path);
        }

      /* clean up */
      gtk_tree_path_free (path);
      fclose (fp);
    }

  /* clean up */
  g_free (bookmarks_path);
}



static void
thunar_shortcuts_model_monitor (ThunarVfsMonitor       *monitor,
                                ThunarVfsMonitorHandle *handle,
                                ThunarVfsMonitorEvent   event,
                                ThunarVfsPath          *handle_path,
                                ThunarVfsPath          *event_path,
                                gpointer                user_data)
{
  ThunarShortcutsModel *model = THUNAR_SHORTCUTS_MODEL (user_data);
  ThunarShortcut       *shortcut;
  GtkTreePath           *path;
  GList                 *lp;
  gint                   index;

  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (model->monitor == monitor);
  _thunar_return_if_fail (model->handle == handle);

  /* drop all existing user-defined shortcuts from the model */
  for (index = 0, lp = model->shortcuts; lp != NULL; )
    {
      /* grab the shortcut */
      shortcut = THUNAR_SHORTCUT (lp->data);

      /* advance to the next list item */
      lp = g_list_next (lp);

      /* drop the shortcut if it is user-defined */
      if (shortcut->type == THUNAR_SHORTCUT_USER_DEFINED)
        {
          /* unlink the shortcut from the model */
          model->shortcuts = g_list_remove (model->shortcuts, shortcut);
          
          /* tell everybody that we have lost a shortcut */
          path = gtk_tree_path_new_from_indices (index, -1);
          gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), path);
          gtk_tree_path_free (path);

          /* actually free the shortcut */
          thunar_shortcut_free (shortcut, model);
        }
      else
        {
          ++index;
        }
    }

  /* reload the shortcuts model */
  thunar_shortcuts_model_load (model);
}



static void
thunar_shortcuts_model_save (ThunarShortcutsModel *model)
{
  ThunarShortcut *shortcut;
  gchar           *bookmarks_path;
  gchar           *tmp_path;
  gchar           *uri;
  GList           *lp;
  FILE            *fp;
  gint             fd;

  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));

  /* open a temporary file for writing */
  tmp_path = xfce_get_homefile (".gtk-bookmarks.XXXXXX", NULL);
  fd = g_mkstemp (tmp_path);
  if (G_UNLIKELY (fd < 0))
    {
      g_warning ("Failed to open `%s' for writing: %s",
                 tmp_path, g_strerror (errno));
      g_free (tmp_path);
      return;
    }

  /* write the uris of user customizable shortcuts */
  fp = fdopen (fd, "w");
  for (lp = model->shortcuts; lp != NULL; lp = lp->next)
    {
      shortcut = THUNAR_SHORTCUT (lp->data);
      if (shortcut->type == THUNAR_SHORTCUT_USER_DEFINED)
        {
          uri = thunar_vfs_path_dup_uri (thunar_file_get_path (shortcut->file));
          if (G_LIKELY (shortcut->name != NULL))
            fprintf (fp, "%s %s\n", uri, shortcut->name);
          else
            fprintf (fp, "%s\n", uri);
          g_free (uri);
        }
    }

  /* we're done writing the temporary file */
  fclose (fp);

  /* move the temporary file to it's final location (atomic writing) */
  bookmarks_path = xfce_get_homefile (".gtk-bookmarks", NULL);
  if (rename (tmp_path, bookmarks_path) < 0)
    {
      g_warning ("Failed to write `%s': %s",
                 bookmarks_path, g_strerror (errno));
      unlink (tmp_path);
    }

  /* cleanup */
  g_free (bookmarks_path);
  g_free (tmp_path);
}



static void
thunar_shortcuts_model_file_changed (ThunarFile           *file,
                                     ThunarShortcutsModel *model)
{
  ThunarShortcut *shortcut;
  GtkTreePath     *path;
  GtkTreeIter      iter;
  GList           *lp;
  gint             index;

  _thunar_return_if_fail (THUNAR_IS_FILE (file));
  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
 
  /* check if the file still refers to a directory, else we cannot keep
   * it on the shortcuts list, and so we'll treat it like the file
   * was destroyed (and thereby removed)
   */
  if (G_UNLIKELY (!thunar_file_is_directory (file)))
    {
      thunar_shortcuts_model_file_destroy (file, model);
      return;
    }

  for (index = 0, lp = model->shortcuts; lp != NULL; ++index, lp = lp->next)
    {
      shortcut = THUNAR_SHORTCUT (lp->data);
      if (shortcut->file == file)
        {
          GTK_TREE_ITER_INIT (iter, model->stamp, lp);

          path = gtk_tree_path_new_from_indices (index, -1);
          gtk_tree_model_row_changed (GTK_TREE_MODEL (model), path, &iter);
          gtk_tree_path_free (path);
          break;
        }
    }
}



static void
thunar_shortcuts_model_file_destroy (ThunarFile           *file,
                                     ThunarShortcutsModel *model)
{
  ThunarShortcut *shortcut = NULL;
  GList          *lp;

  _thunar_return_if_fail (THUNAR_IS_FILE (file));
  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));

  /* lookup the shortcut matching the file */
  for (lp = model->shortcuts; lp != NULL; lp = lp->next)
    {
      shortcut = THUNAR_SHORTCUT (lp->data);
      if (shortcut->file == file)
        break;
    }

  /* verify that we actually found a shortcut */
  _thunar_assert (lp != NULL);
  _thunar_assert (THUNAR_IS_FILE (shortcut->file));

  /* drop the shortcut from the model */
  thunar_shortcuts_model_remove_shortcut (model, shortcut);
}



static void
thunar_shortcuts_model_volume_changed (ThunarVfsVolume      *volume,
                                       ThunarShortcutsModel *model)
{
  ThunarShortcut *shortcut = NULL;
  GtkTreePath     *path;
  GtkTreeIter      iter;
  GList           *lp;
  gint             index;

  _thunar_return_if_fail (THUNAR_VFS_IS_VOLUME (volume));
  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));

  /* check if the volume is on the hidden list */
  lp = g_list_find (model->hidden_volumes, volume);
  if (lp != NULL)
    {
      /* check if we need to display the volume now */
      if (thunar_vfs_volume_is_present (volume) && thunar_vfs_volume_is_removable (volume))
        {
          /* remove the volume from the list of hidden volumes */
          model->hidden_volumes = g_list_delete_link (model->hidden_volumes, lp);

          /* find the insert position */
          for (index = 0, lp = model->shortcuts; lp != NULL; ++index, lp = lp->next)
            {
              shortcut = THUNAR_SHORTCUT (lp->data);
              if (shortcut->type == THUNAR_SHORTCUT_SEPARATOR
                  || shortcut->type == THUNAR_SHORTCUT_USER_DEFINED)
                break;
            }

          /* allocate a new shortcut */
          shortcut = _thunar_slice_new0 (ThunarShortcut);
          shortcut->type = THUNAR_SHORTCUT_REMOVABLE_MEDIA;
          shortcut->volume = volume;

          /* the volume is present now, so we have to display it */
          path = gtk_tree_path_new_from_indices (index, -1);
          thunar_shortcuts_model_add_shortcut (model, shortcut, path);
          gtk_tree_path_free (path);
        }
    }
  else
    {
      /* lookup the shortcut that contains the given volume */
      for (index = 0, lp = model->shortcuts; lp != NULL; ++index, lp = lp->next)
        {
          shortcut = THUNAR_SHORTCUT (lp->data);
          if (shortcut->volume == volume)
            break;
        }

      /* verify that we actually found the shortcut */
      _thunar_assert (shortcut != NULL);
      _thunar_assert (shortcut->volume == volume);

      /* check if we need to hide the volume now */
      if (!thunar_vfs_volume_is_present (volume) || !thunar_vfs_volume_is_removable (volume))
        {
          /* need to ref here, because the file_destroy() handler will drop the refcount. */
          g_object_ref (G_OBJECT (volume));

          /* move the volume to the hidden list */
          model->hidden_volumes = g_list_prepend (model->hidden_volumes, volume);

          /* remove the shortcut from the user interface */
          thunar_shortcuts_model_remove_shortcut (model, shortcut);

          /* need to reconnect to the volume, as the shortcut removal drops the handler */
          g_signal_connect (G_OBJECT (volume), "changed", G_CALLBACK (thunar_shortcuts_model_volume_changed), model);
        }
      else
        {
          /* generate an iterator for the path */
          GTK_TREE_ITER_INIT (iter, model->stamp, lp);

          /* tell the view that the volume has changed in some way */
          path = gtk_tree_path_new_from_indices (index, -1);
          gtk_tree_model_row_changed (GTK_TREE_MODEL (model), path, &iter);
          gtk_tree_path_free (path);
        }
    }
}



static void
thunar_shortcuts_model_volumes_added (ThunarVfsVolumeManager *volume_manager,
                                      GList                  *volumes,
                                      ThunarShortcutsModel   *model)
{
  GList *lp;

  _thunar_return_if_fail (THUNAR_VFS_IS_VOLUME_MANAGER (volume_manager));
  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (model->volume_manager == volume_manager);

  /* process all newly added volumes */
  for (lp = volumes; lp != NULL; lp = lp->next)
    {
      /* take a reference on the volume... */
      g_object_ref (G_OBJECT (lp->data));

      /* ...place the volume on the hidden list... */
      model->hidden_volumes = g_list_prepend (model->hidden_volumes, lp->data);

      /* ...connect the "changed" signal handler ... */
      g_signal_connect (G_OBJECT (lp->data), "changed", G_CALLBACK (thunar_shortcuts_model_volume_changed), model);

      /* ...and let the "changed" handler place the volume where appropriate */
      thunar_shortcuts_model_volume_changed (lp->data, model);
    }
}



static void
thunar_shortcuts_model_volumes_removed (ThunarVfsVolumeManager *volume_manager,
                                        GList                  *volumes,
                                        ThunarShortcutsModel   *model)
{
  GList *hp;
  GList *lp;

  _thunar_return_if_fail (THUNAR_VFS_IS_VOLUME_MANAGER (volume_manager));
  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (model->volume_manager == volume_manager);
  
  /* process all removed volumes */
  for (lp = volumes; lp != NULL; lp = lp->next)
    {
      /* disconnect the "changed" signal handler from the volume */
      g_signal_handlers_disconnect_by_func (G_OBJECT (lp->data), thunar_shortcuts_model_volume_changed, model);

      /* check if the volume is on the hidden list */
      hp = g_list_find (model->hidden_volumes, lp->data);
      if (G_LIKELY (hp != NULL))
        {
          /* drop the volume from the hidden list and drop our reference */
          model->hidden_volumes = g_list_delete_link (model->hidden_volumes, hp);
          g_object_unref (G_OBJECT (lp->data));
        }
      else
        {
          /* must be an active shortcut then... */
          for (hp = model->shortcuts; hp != NULL; hp = hp->next)
            if (THUNAR_SHORTCUT (hp->data)->volume == lp->data)
              break;

          /* something is broken if we don't have a shortcut here */
          _thunar_assert (hp != NULL);
          _thunar_assert (THUNAR_SHORTCUT (hp->data)->volume == lp->data);

          /* drop the shortcut from the model */
          thunar_shortcuts_model_remove_shortcut (model, hp->data);
        }
    }
}



static void
thunar_shortcut_free (ThunarShortcut       *shortcut,
                      ThunarShortcutsModel *model)
{
  if (G_LIKELY (shortcut->file != NULL))
    {
      /* drop the file watch */
      thunar_file_unwatch (shortcut->file);

      /* unregister from the file */
      g_signal_handlers_disconnect_matched (G_OBJECT (shortcut->file),
                                            G_SIGNAL_MATCH_DATA, 0,
                                            0, NULL, NULL, model);
      g_object_unref (G_OBJECT (shortcut->file));
    }

  if (G_LIKELY (shortcut->volume != NULL))
    {
      g_signal_handlers_disconnect_matched (G_OBJECT (shortcut->volume),
                                            G_SIGNAL_MATCH_DATA, 0,
                                            0, NULL, NULL, model);
      g_object_unref (G_OBJECT (shortcut->volume));
    }

  /* release the shortcut name */
  g_free (shortcut->name);

  /* release the shortcut itself */
  _thunar_slice_free (ThunarShortcut, shortcut);
}



/**
 * thunar_shortcuts_model_get_default:
 *
 * Returns the default #ThunarShortcutsModel instance shared by
 * all #ThunarShortcutsView instances.
 *
 * Call #g_object_unref() on the returned object when you
 * don't need it any longer.
 *
 * Return value: the default #ThunarShortcutsModel instance.
 **/
ThunarShortcutsModel*
thunar_shortcuts_model_get_default (void)
{
  static ThunarShortcutsModel *model = NULL;

  if (G_UNLIKELY (model == NULL))
    {
      model = g_object_new (THUNAR_TYPE_SHORTCUTS_MODEL, NULL);
      g_object_add_weak_pointer (G_OBJECT (model), (gpointer) &model);
    }
  else
    {
      g_object_ref (G_OBJECT (model));
    }

  return model;
}



/**
 * thunar_shortcuts_model_iter_for_file:
 * @model : a #ThunarShortcutsModel instance.
 * @file  : a #ThunarFile instance.
 * @iter  : pointer to a #GtkTreeIter.
 *
 * Tries to lookup the #GtkTreeIter, that belongs to a shortcut, which
 * refers to @file and stores it to @iter. If no such #GtkTreeIter was
 * found, %FALSE will be returned and @iter won't be changed. Else
 * %TRUE will be returned and @iter will be set appropriately.
 *
 * Return value: %TRUE if @file was found, else %FALSE.
 **/
gboolean
thunar_shortcuts_model_iter_for_file (ThunarShortcutsModel *model,
                                      ThunarFile           *file,
                                      GtkTreeIter          *iter)
{
  ThunarVfsPath *mount_point;
  GList         *lp;
  
  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), FALSE);
  _thunar_return_val_if_fail (THUNAR_IS_FILE (file), FALSE);
  _thunar_return_val_if_fail (iter != NULL, FALSE);

  for (lp = model->shortcuts; lp != NULL; lp = lp->next)
    {
      /* check if we have a file that matches */
      if (THUNAR_SHORTCUT (lp->data)->file == file)
        {
          GTK_TREE_ITER_INIT (*iter, model->stamp, lp);
          return TRUE;
        }

      /* but maybe we have a mounted(!) volume with a matching mount point */
      if (THUNAR_SHORTCUT (lp->data)->volume != NULL && thunar_vfs_volume_is_mounted (THUNAR_SHORTCUT (lp->data)->volume))
        {
          /* check if we have a mount point for the volume */
          mount_point = thunar_vfs_volume_get_mount_point (THUNAR_SHORTCUT (lp->data)->volume);
          if (G_LIKELY (mount_point != NULL && thunar_vfs_path_equal (mount_point, thunar_file_get_path (file))))
            {
              GTK_TREE_ITER_INIT (*iter, model->stamp, lp);
              return TRUE;
            }
        }
    }

  return FALSE;
}



/**
 * thunar_shortcuts_model_drop_possible:
 * @model : a #ThunarShortcutstModel.
 * @path  : a #GtkTreePath.
 *
 * Determines whether a drop is possible before the given @path, at the same depth
 * as @path. I.e., can we drop data at that location. @path does not have to exist;
 * the return value will almost certainly be FALSE if the parent of @path doesn't
 * exist, though.
 *
 * Return value: %TRUE if it's possible to drop data before @path, else %FALSE.
 **/
gboolean
thunar_shortcuts_model_drop_possible (ThunarShortcutsModel *model,
                                      GtkTreePath          *path)
{
  ThunarShortcut *shortcut;

  _thunar_return_val_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model), FALSE);
  _thunar_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

  /* determine the list item for the path */
  shortcut = g_list_nth_data (model->shortcuts, gtk_tree_path_get_indices (path)[0]);

  /* append to the list is always possible */
  if (G_LIKELY (shortcut == NULL))
    return TRUE;

  /* cannot drop before special shortcuts! */
  return (shortcut->type == THUNAR_SHORTCUT_USER_DEFINED);
}



/**
 * thunar_shortcuts_model_add:
 * @model    : a #ThunarShortcutsModel.
 * @dst_path : the destination path.
 * @file     : the #ThunarFile that should be added to the shortcuts list.
 *
 * Adds the shortcut @file to the @model at @dst_path, unless @file is
 * already present in @model in which case no action is performed.
 **/
void
thunar_shortcuts_model_add (ThunarShortcutsModel *model,
                            GtkTreePath          *dst_path,
                            ThunarFile           *file)
{
  ThunarShortcut *shortcut;
  GList           *lp;

  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (gtk_tree_path_get_depth (dst_path) > 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (dst_path)[0] >= 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (dst_path)[0] <= g_list_length (model->shortcuts));
  _thunar_return_if_fail (THUNAR_IS_FILE (file));

  /* verify that the file is not already in use as shortcut */
  for (lp = model->shortcuts; lp != NULL; lp = lp->next)
    if (THUNAR_SHORTCUT (lp->data)->file == file)
      return;

  /* create the new shortcut that will be inserted */
  shortcut = _thunar_slice_new0 (ThunarShortcut);
  shortcut->type = THUNAR_SHORTCUT_USER_DEFINED;
  shortcut->file = g_object_ref (G_OBJECT (file));

  /* add the shortcut to the list at the given position */
  thunar_shortcuts_model_add_shortcut (model, shortcut, dst_path);

  /* the shortcuts list was changed, so write the gtk bookmarks file */
  thunar_shortcuts_model_save (model);
}



/**
 * thunar_shortcuts_model_move:
 * @model    : a #ThunarShortcutsModel.
 * @src_path : the source path.
 * @dst_path : the destination path.
 *
 * Moves the shortcut at @src_path to @dst_path, adjusting other
 * shortcut's positions as required.
 **/
void
thunar_shortcuts_model_move (ThunarShortcutsModel *model,
                             GtkTreePath          *src_path,
                             GtkTreePath          *dst_path)
{
  ThunarShortcut *shortcut;
  GtkTreePath     *path;
  GList           *lp;
  gint            *order;
  gint             index_src;
  gint             index_dst;
  gint             index;

  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (gtk_tree_path_get_depth (src_path) > 0);
  _thunar_return_if_fail (gtk_tree_path_get_depth (dst_path) > 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (src_path)[0] >= 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (src_path)[0] < g_list_length (model->shortcuts));
  _thunar_return_if_fail (gtk_tree_path_get_indices (dst_path)[0] > 0);

  index_src = gtk_tree_path_get_indices (src_path)[0];
  index_dst = gtk_tree_path_get_indices (dst_path)[0];

  if (G_UNLIKELY (index_src == index_dst))
    return;

  /* generate the order for the rows prior the dst/src rows */
  order = g_newa (gint, g_list_length (model->shortcuts));
  for (index = 0, lp = model->shortcuts; index < index_src && index < index_dst; ++index, lp = lp->next)
    order[index] = index;

  if (index == index_src)
    {
      shortcut = THUNAR_SHORTCUT (lp->data);

      for (; index < index_dst; ++index, lp = lp->next)
        {
          lp->data = lp->next->data;
          order[index] = index + 1;
        }

      lp->data = shortcut;
      order[index++] = index_src;
    }
  else
    {
      for (; index < index_src; ++index, lp = lp->next)
        ;

      _thunar_assert (index == index_src);

      shortcut = THUNAR_SHORTCUT (lp->data);

      for (; index > index_dst; --index, lp = lp->prev)
        {
          lp->data = lp->prev->data;
          order[index] = index - 1;
        }

      _thunar_assert (index == index_dst);

      lp->data = shortcut;
      order[index] = index_src;
      index = index_src + 1;
    }

  /* generate the remaining order */
  for (; index < g_list_length (model->shortcuts); ++index)
    order[index] = index;

  /* tell all listeners about the reordering just performed */
  path = gtk_tree_path_new ();
  gtk_tree_model_rows_reordered (GTK_TREE_MODEL (model), path, NULL, order);
  gtk_tree_path_free (path);

  /* the shortcuts list was changed, so write the gtk bookmarks file */
  thunar_shortcuts_model_save (model);
}



/**
 * thunar_shortcuts_model_remove:
 * @model : a #ThunarShortcutsModel.
 * @path  : the #GtkTreePath of the shortcut to remove.
 *
 * Removes the shortcut at @path from the @model and syncs to
 * on-disk storage. @path must refer to a valid, user-defined
 * shortcut, as you cannot remove system-defined entities (they
 * are managed internally).
 **/
void
thunar_shortcuts_model_remove (ThunarShortcutsModel *model,
                               GtkTreePath          *path)
{
  ThunarShortcut *shortcut;

  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (gtk_tree_path_get_depth (path) > 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (path)[0] >= 0);
  _thunar_return_if_fail (gtk_tree_path_get_indices (path)[0] < g_list_length (model->shortcuts));

  /* lookup the shortcut for the given path */
  shortcut = g_list_nth_data (model->shortcuts, gtk_tree_path_get_indices (path)[0]);

  /* verify that the shortcut is removable */
  _thunar_assert (shortcut->type == THUNAR_SHORTCUT_USER_DEFINED);
  _thunar_assert (THUNAR_IS_FILE (shortcut->file));

  /* remove the shortcut (using the file destroy handler) */
  thunar_shortcuts_model_file_destroy (shortcut->file, model);
}



/**
 * thunar_shortcuts_model_rename:
 * @model : a #ThunarShortcutsModel.
 * @iter  : the #GtkTreeIter which refers to the shortcut that
 *          should be renamed to @name.
 * @name  : the new name for the shortcut at @path or %NULL to
 *          return to the default name.
 *
 * Renames the shortcut at @iter to the new @name in @model.
 *
 * @name may be %NULL or an empty to reset the shortcut to
 * its default name.
 **/
void
thunar_shortcuts_model_rename (ThunarShortcutsModel *model,
                               GtkTreeIter          *iter,
                               const gchar          *name)
{
  ThunarShortcut *shortcut;
  GtkTreePath    *path;

  _thunar_return_if_fail (name == NULL || g_utf8_validate (name, -1, NULL));
  _thunar_return_if_fail (THUNAR_IS_SHORTCUTS_MODEL (model));
  _thunar_return_if_fail (iter->stamp == model->stamp);
  _thunar_return_if_fail (iter->user_data != NULL);

  /* lookup the shortcut for the given path */
  shortcut = THUNAR_SHORTCUT (((GList *) iter->user_data)->data);

  /* verify the shortcut */
  _thunar_assert (shortcut->type == THUNAR_SHORTCUT_USER_DEFINED);
  _thunar_assert (THUNAR_IS_FILE (shortcut->file));

  /* perform the rename */
  if (G_UNLIKELY (shortcut->name != NULL))
    g_free (shortcut->name);
  if (G_UNLIKELY (name == NULL || *name == '\0'))
    shortcut->name = NULL;
  else
    shortcut->name = g_strdup (name);

  /* notify the views about the change */
  path = gtk_tree_model_get_path (GTK_TREE_MODEL (model), iter);
  gtk_tree_model_row_changed (GTK_TREE_MODEL (model), path, iter);
  gtk_tree_path_free (path);

  /* save the changes to the model */
  thunar_shortcuts_model_save (model);
}




