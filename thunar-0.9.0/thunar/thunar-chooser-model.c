/* $Id: thunar-chooser-model.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>.
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

#include <thunar/thunar-chooser-model.h>
#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-icon-factory.h>
#include <thunar/thunar-private.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_LOADING,
  PROP_MIME_INFO,
};



static void     thunar_chooser_model_class_init     (ThunarChooserModelClass  *klass);
static void     thunar_chooser_model_init           (ThunarChooserModel       *model);
static void     thunar_chooser_model_finalize       (GObject                  *object);
static void     thunar_chooser_model_get_property   (GObject                  *object,
                                                     guint                     prop_id,
                                                     GValue                   *value,
                                                     GParamSpec               *pspec);
static void     thunar_chooser_model_set_property   (GObject                  *object,
                                                     guint                     prop_id,
                                                     const GValue             *value,
                                                     GParamSpec               *pspec);
static void     thunar_chooser_model_append         (ThunarChooserModel       *model,
                                                     const gchar              *title,
                                                     const gchar              *icon_name,
                                                     GList                    *applications);
static void     thunar_chooser_model_import         (ThunarChooserModel       *model,
                                                     GList                    *applications);
static GList   *thunar_chooser_model_readdir        (ThunarChooserModel       *model,
                                                     const gchar              *absdir,
                                                     const gchar              *reldir);
static gpointer thunar_chooser_model_thread         (gpointer                  user_data);
static gboolean thunar_chooser_model_timer          (gpointer                  user_data);
static void     thunar_chooser_model_timer_destroy  (gpointer                  user_data);




struct _ThunarChooserModelClass
{
  GtkTreeStoreClass __parent__;
};

struct _ThunarChooserModel
{
  GtkTreeStore __parent__;

  ThunarVfsMimeInfo *mime_info;

  /* thread interaction */
  gint               timer_id;
  GThread           *thread;
  volatile gboolean  finished;
  volatile gboolean  cancelled;
};



static GObjectClass *thunar_chooser_model_parent_class;



GType
thunar_chooser_model_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarChooserModelClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_chooser_model_class_init,
        NULL,
        NULL,
        sizeof (ThunarChooserModel),
        0,
        (GInstanceInitFunc) thunar_chooser_model_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_TREE_STORE, I_("ThunarChooserModel"), &info, 0);
    }

  return type;
}



static void
thunar_chooser_model_class_init (ThunarChooserModelClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  thunar_chooser_model_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_chooser_model_finalize;
  gobject_class->get_property = thunar_chooser_model_get_property;
  gobject_class->set_property = thunar_chooser_model_set_property;

  /**
   * ThunarChooserModel::loading:
   *
   * Whether the contents of the #ThunarChooserModel are
   * currently being loaded from disk.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_LOADING,
                                   g_param_spec_boolean ("loading", "loading", "loading",
                                                         FALSE,
                                                         EXO_PARAM_READABLE));

  /**
   * ThunarChooserModel::mime-info:
   *
   * The #ThunarVfsMimeInfo info for the model.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_MIME_INFO,
                                   g_param_spec_boxed ("mime-info", "mime-info", "mime-info",
                                                       THUNAR_VFS_TYPE_MIME_INFO,
                                                       G_PARAM_CONSTRUCT_ONLY | EXO_PARAM_READWRITE));
}



static void
thunar_chooser_model_init (ThunarChooserModel *model)
{
  /* allocate the types array for the columns */
  GType column_types[THUNAR_CHOOSER_MODEL_N_COLUMNS] =
  {
    G_TYPE_STRING,
    G_TYPE_STRING,
    THUNAR_VFS_TYPE_MIME_APPLICATION,
    PANGO_TYPE_STYLE,
    G_TYPE_BOOLEAN,
    PANGO_TYPE_WEIGHT,
    G_TYPE_BOOLEAN,
  };

  /* register the column types */
  gtk_tree_store_set_column_types (GTK_TREE_STORE (model), G_N_ELEMENTS (column_types), column_types);

  /* start to load the applications installed on the system */
  model->thread = g_thread_create (thunar_chooser_model_thread, model, TRUE, NULL);

  /* start the timer to monitor the thread */
  model->timer_id = g_timeout_add_full (G_PRIORITY_LOW, 200, thunar_chooser_model_timer,
                                        model, thunar_chooser_model_timer_destroy);
}



static void
thunar_chooser_model_finalize (GObject *object)
{
  ThunarChooserModel *model = THUNAR_CHOOSER_MODEL (object);
  GList              *applications;

  /* drop any pending timer */
  if (G_UNLIKELY (model->timer_id >= 0))
    g_source_remove (model->timer_id);

  /* cancel the thread (if running) */
  if (G_UNLIKELY (model->thread != NULL))
    {
      /* set the cancellation flag */
      model->cancelled = TRUE;

      /* join the thread */
      applications = g_thread_join (model->thread);

      /* ditch the returned application list */
      g_list_foreach (applications, (GFunc) g_object_unref, NULL);
      g_list_free (applications);
    }

  /* drop the mime info (if any) */
  if (G_LIKELY (model->mime_info != NULL))
    thunar_vfs_mime_info_unref (model->mime_info);

  (*G_OBJECT_CLASS (thunar_chooser_model_parent_class)->finalize) (object);
}



static void
thunar_chooser_model_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  ThunarChooserModel *model = THUNAR_CHOOSER_MODEL (object);

  switch (prop_id)
    {
    case PROP_LOADING:
      g_value_set_boolean (value, thunar_chooser_model_get_loading (model));
      break;

    case PROP_MIME_INFO:
      g_value_set_boxed (value, thunar_chooser_model_get_mime_info (model));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_chooser_model_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  ThunarChooserModel *model = THUNAR_CHOOSER_MODEL (object);

  switch (prop_id)
    {
    case PROP_MIME_INFO:
      if (G_LIKELY (model->mime_info == NULL))
        model->mime_info = g_value_dup_boxed (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_chooser_model_append (ThunarChooserModel *model,
                             const gchar        *title,
                             const gchar        *icon_name,
                             GList              *applications)
{
  GtkIconTheme *icon_theme;
  GtkTreeIter   parent_iter;
  GtkTreeIter   child_iter;
  GList        *lp;

  _thunar_return_if_fail (THUNAR_IS_CHOOSER_MODEL (model));
  _thunar_return_if_fail (title != NULL);
  _thunar_return_if_fail (icon_name != NULL);

  /* query the default icon theme */
  icon_theme = gtk_icon_theme_get_default ();

  /* insert the section title */
  gtk_tree_store_append (GTK_TREE_STORE (model), &parent_iter, NULL);
  gtk_tree_store_set (GTK_TREE_STORE (model), &parent_iter,
                      THUNAR_CHOOSER_MODEL_COLUMN_NAME, title,
                      THUNAR_CHOOSER_MODEL_COLUMN_ICON, icon_name,
                      THUNAR_CHOOSER_MODEL_COLUMN_WEIGHT, PANGO_WEIGHT_SEMIBOLD,
                      THUNAR_CHOOSER_MODEL_COLUMN_WEIGHT_SET, TRUE,
                      -1);

  /* check if we have any program items */
  if (G_LIKELY (applications != NULL))
    {
      /* insert the program items */
      for (lp = applications; lp != NULL; lp = lp->next)
        {
          /* append the tree row with the program data */
          gtk_tree_store_append (GTK_TREE_STORE (model), &child_iter, &parent_iter);
          gtk_tree_store_set (GTK_TREE_STORE (model), &child_iter,
                              THUNAR_CHOOSER_MODEL_COLUMN_NAME, thunar_vfs_mime_handler_get_name (lp->data),
                              THUNAR_CHOOSER_MODEL_COLUMN_ICON, thunar_vfs_mime_handler_lookup_icon_name (lp->data, icon_theme),
                              THUNAR_CHOOSER_MODEL_COLUMN_APPLICATION, lp->data,
                              -1);
        }
    }
  else
    {
      /* tell the user that we don't have any applications for this category */
      gtk_tree_store_append (GTK_TREE_STORE (model), &child_iter, &parent_iter);
      gtk_tree_store_set (GTK_TREE_STORE (model), &child_iter,
                          THUNAR_CHOOSER_MODEL_COLUMN_NAME, _("None available"),
                          THUNAR_CHOOSER_MODEL_COLUMN_STYLE, PANGO_STYLE_ITALIC,
                          THUNAR_CHOOSER_MODEL_COLUMN_STYLE_SET, TRUE,
                          -1);
    }
}



static void
thunar_chooser_model_import (ThunarChooserModel *model,
                             GList              *applications)
{
  ThunarVfsMimeDatabase *mime_database;
  GList                 *recommended;
  GList                 *other = NULL;
  GList                 *lp;
  GList                 *p;

  _thunar_return_if_fail (THUNAR_IS_CHOOSER_MODEL (model));
  _thunar_return_if_fail (model->mime_info != NULL);

  /* determine the recommended applications for the mime info */
  mime_database = thunar_vfs_mime_database_get_default ();
  recommended = thunar_vfs_mime_database_get_applications (mime_database, model->mime_info);
  g_object_unref (G_OBJECT (mime_database));

  /* add all applications to other that are not already on recommended */
  for (lp = applications; lp != NULL; lp = lp->next)
    {
      /* check if this application is already on the recommended list */
      for (p = recommended; p != NULL; p = p->next)
        if (thunar_vfs_mime_application_equal (p->data, lp->data))
          break;

      /* add to the list of other applications if it's not on recommended */
      if (G_LIKELY (p == NULL))
        other = g_list_append (other, lp->data);
    }

  /* append the "Recommended Applications:" category */
  thunar_chooser_model_append (model, _("Recommended Applications"), "preferences-desktop-default-applications", recommended);

  /* append the "Other Applications:" category */
  thunar_chooser_model_append (model, _("Other Applications"), "gnome-applications", other);

  /* cleanup */
  g_list_foreach (recommended, (GFunc) g_object_unref, NULL);
  g_list_free (recommended);
  g_list_free (other);
}



static GList*
thunar_chooser_model_readdir (ThunarChooserModel *model,
                              const gchar        *absdir,
                              const gchar        *reldir)
{
  ThunarVfsMimeApplication *application;
  const gchar              *name;
  GList                    *applications = NULL;
  gchar                    *abspath;
  gchar                    *relpath;
  gchar                    *p;
  GDir                     *dp;

  _thunar_return_val_if_fail (THUNAR_IS_CHOOSER_MODEL (model), NULL);
  _thunar_return_val_if_fail (reldir == NULL || !g_path_is_absolute (reldir), NULL);
  _thunar_return_val_if_fail (g_path_is_absolute (absdir), NULL);

  /* try to open the directory */
  dp = g_dir_open (absdir, 0, NULL);
  if (G_LIKELY (dp != NULL))
    {
      /* process the files within the directory */
      while (!model->cancelled)
        {
          /* read the next file entry */
          name = g_dir_read_name (dp);
          if (G_UNLIKELY (name == NULL))
            break;

          /* generate the absolute path to the file entry */
          abspath = g_build_filename (absdir, name, NULL);

          /* generate the relative path to the file entry */
          relpath = (reldir != NULL) ? g_build_filename (reldir, name, NULL) : g_strdup (name);

          /* check if we have a directory or a regular file here */
          if (g_file_test (abspath, G_FILE_TEST_IS_DIR))
            {
              /* recurse for directories */
              applications = g_list_concat (applications, thunar_chooser_model_readdir (model, abspath, relpath));
            }
          else if (g_file_test (abspath, G_FILE_TEST_IS_REGULAR) && g_str_has_suffix (name, ".desktop"))
            {
              /* generate the desktop-id from the relative path */
              for (p = relpath; *p != '\0'; ++p)
                if (*p == G_DIR_SEPARATOR)
                  *p = '-';

              /* try to load the application for the given desktop-id */
              application = thunar_vfs_mime_application_new_from_file (abspath, relpath);

              /* check if we have successfully loaded the application */
              if (G_LIKELY (application != NULL))
                {
                  /* check if the application supports atleast one mime-type */
                  if (thunar_vfs_mime_application_get_mime_types (application) != NULL)
                    applications = g_list_append (applications, application);
                  else
                    g_object_unref (G_OBJECT (application));
                }
            }

          /* cleanup */
          g_free (abspath);
          g_free (relpath);
        }

      /* close the directory handle */
      g_dir_close (dp);
    }

  return applications;
}



static gint
compare_application_by_name (gconstpointer a,
                             gconstpointer b)
{
  return strcmp (thunar_vfs_mime_handler_get_name (a),
                 thunar_vfs_mime_handler_get_name (b));
}



static gpointer
thunar_chooser_model_thread (gpointer user_data)
{
  ThunarChooserModel *model = THUNAR_CHOOSER_MODEL (user_data);
  GList              *applications = NULL;
  GList              *list;
  GList              *lp;
  GList              *p;
  gchar             **paths;
  guint               n;

  /* determine the available applications/ directories */
  paths = xfce_resource_lookup_all (XFCE_RESOURCE_DATA, "applications/");
  for (n = 0; !model->cancelled && paths[n] != NULL; ++n)
    {
      /* lookup the applications in this directory */
      list = thunar_chooser_model_readdir (model, paths[n], NULL);

      /* merge the applications with what we already have */
      for (lp = list; lp != NULL; lp = lp->next)
        {
          /* ignore hidden applications to be compatible with the Nautilus mess */
          if ((thunar_vfs_mime_handler_get_flags (lp->data) & THUNAR_VFS_MIME_HANDLER_HIDDEN) != 0)
            {
              g_object_unref (G_OBJECT (lp->data));
              continue;
            }

          /* check if we have that application already */
          for (p = applications; p != NULL; p = p->next)
            {
              /* compare the desktop-ids */
              if (thunar_vfs_mime_application_equal (p->data, lp->data))
                break;
            }

          /* no need to add if we have it already */
          if (G_UNLIKELY (p != NULL))
            {
              g_object_unref (G_OBJECT (lp->data));
              continue;
            }

          /* insert the application into the list */
          applications = g_list_insert_sorted (applications, lp->data, compare_application_by_name);
        }
      
      /* free the temporary list */
      g_list_free (list);
    }
  g_strfreev (paths);

  /* tell the model that we're done */
  model->finished = TRUE;

  return applications;
}



static gboolean
thunar_chooser_model_timer (gpointer user_data)
{
  ThunarChooserModel *model = THUNAR_CHOOSER_MODEL (user_data);
  gboolean            finished;
  GList              *applications;

  /* check if the applications are loaded */
  GDK_THREADS_ENTER ();
  finished = model->finished;
  if (G_LIKELY (finished))
    {
      /* grab the application list from the thread */
      applications = g_thread_join (model->thread);
      model->thread = NULL;

      /* process the applications list */
      thunar_chooser_model_import (model, applications);

      /* tell everybody that the model is loaded */
      g_object_notify (G_OBJECT (model), "loading");

      /* free the application list */
      g_list_foreach (applications, (GFunc) g_object_unref, NULL);
      g_list_free (applications);
    }
  GDK_THREADS_LEAVE ();

  return !finished;
}



static void
thunar_chooser_model_timer_destroy (gpointer user_data)
{
  THUNAR_CHOOSER_MODEL (user_data)->timer_id = -1;
}



/**
 * thunar_chooser_model_new:
 * @mime_info : a #ThunarVfsMimeInfo.
 *
 * Allocates a new #ThunarChooserModel for @mime_info.
 *
 * Return value: the newly allocated #ThunarChooserModel.
 **/
ThunarChooserModel*
thunar_chooser_model_new (ThunarVfsMimeInfo *mime_info)
{
  return g_object_new (THUNAR_TYPE_CHOOSER_MODEL,
                       "mime-info", mime_info,
                       NULL);
}



/**
 * thunar_chooser_model_get_loading:
 * @model : a #ThunarChooserModel.
 *
 * Returns %TRUE if @model is currently being loaded.
 *
 * Return value: %TRUE if @model is currently being loaded.
 **/
gboolean
thunar_chooser_model_get_loading (ThunarChooserModel *model)
{
  _thunar_return_val_if_fail (THUNAR_IS_CHOOSER_MODEL (model), FALSE);
  return (model->thread != NULL);
}



/**
 * thunar_chooser_model_get_mime_info:
 * @model : a #ThunarChooserModel.
 *
 * Returns the #ThunarVfsMimeInfo for @model.
 *
 * Return value: the #ThunarVfsMimeInfo for @model.
 **/
ThunarVfsMimeInfo*
thunar_chooser_model_get_mime_info (ThunarChooserModel *model)
{
  _thunar_return_val_if_fail (THUNAR_IS_CHOOSER_MODEL (model), NULL);
  return model->mime_info;
}



/**
 * thunar_chooser_model_remove:
 * @model : a #ThunarChooserModel.
 * @iter  : the #GtkTreeIter for the application to remove.
 * @error : return location for errors or %NULL.
 *
 * Tries to remove the application at the specified @iter from
 * the systems application database. Returns %TRUE on success,
 * otherwise %FALSE is returned.
 *
 * Return value: %TRUE on success, %FALSE otherwise.
 **/
gboolean
thunar_chooser_model_remove (ThunarChooserModel *model,
                             GtkTreeIter        *iter,
                             GError            **error)
{
  ThunarVfsMimeApplication *mime_application;
  ThunarVfsMimeDatabase    *mime_database;
  gboolean                  succeed;

  _thunar_return_val_if_fail (THUNAR_IS_CHOOSER_MODEL (model), FALSE);
  _thunar_return_val_if_fail (error == NULL || *error == NULL, FALSE);
  _thunar_return_val_if_fail (gtk_tree_store_iter_is_valid (GTK_TREE_STORE (model), iter), FALSE);

  /* determine the mime application for the iter */
  gtk_tree_model_get (GTK_TREE_MODEL (model), iter, THUNAR_CHOOSER_MODEL_COLUMN_APPLICATION, &mime_application, -1);
  if (G_UNLIKELY (mime_application == NULL))
    return TRUE;

  /* try to remove the application from the database */
  mime_database = thunar_vfs_mime_database_get_default ();
  succeed = thunar_vfs_mime_database_remove_application (mime_database, mime_application, error);
  g_object_unref (G_OBJECT (mime_application));
  g_object_unref (G_OBJECT (mime_database));

  /* if the removal was successfull, delete the row from the model */
  if (G_LIKELY (succeed))
    gtk_tree_store_remove (GTK_TREE_STORE (model), iter);

  return succeed;
}


