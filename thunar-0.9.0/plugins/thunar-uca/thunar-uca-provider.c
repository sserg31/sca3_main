/* $Id: thunar-uca-provider.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <thunar-vfs/thunar-vfs.h>

#include <thunar-uca/thunar-uca-chooser.h>
#include <thunar-uca/thunar-uca-context.h>
#include <thunar-uca/thunar-uca-model.h>
#include <thunar-uca/thunar-uca-private.h>
#include <thunar-uca/thunar-uca-provider.h>



static void   thunar_uca_provider_class_init                (ThunarUcaProviderClass           *klass);
static void   thunar_uca_provider_menu_provider_init        (ThunarxMenuProviderIface         *iface);
static void   thunar_uca_provider_preferences_provider_init (ThunarxPreferencesProviderIface  *iface);
static void   thunar_uca_provider_init                      (ThunarUcaProvider                *uca_provider);
static void   thunar_uca_provider_finalize                  (GObject                          *object);
static GList *thunar_uca_provider_get_actions               (ThunarxPreferencesProvider       *preferences_provider,
                                                             GtkWidget                        *window);
static GList *thunar_uca_provider_get_file_actions          (ThunarxMenuProvider              *menu_provider,
                                                             GtkWidget                        *window,
                                                             GList                            *files);
static GList *thunar_uca_provider_get_folder_actions        (ThunarxMenuProvider              *menu_provider,
                                                             GtkWidget                        *window,
                                                             ThunarxFileInfo                  *folder);
static void   thunar_uca_provider_activated                 (ThunarUcaProvider                *uca_provider,
                                                             GtkAction                        *action);
static void   thunar_uca_provider_child_watch               (GPid                              pid,
                                                             gint                              status,
                                                             gpointer                          user_data);
static void   thunar_uca_provider_child_watch_destroy       (gpointer                          user_data);



struct _ThunarUcaProviderClass
{
  GObjectClass __parent__;
};

struct _ThunarUcaProvider
{
  GObject __parent__;

  ThunarUcaModel *model;
  gint            last_action_id; /* used to generate unique action names */

  /* child watch support for the last spawned child process
   * to be able to refresh the folder contents after the
   * child process has terminated.
   */
  gchar          *child_watch_path;
  gint            child_watch_id;
};



static GQuark thunar_uca_context_quark;
static GQuark thunar_uca_folder_quark;
static GQuark thunar_uca_row_quark;



THUNARX_DEFINE_TYPE_WITH_CODE (ThunarUcaProvider,
                               thunar_uca_provider,
                               G_TYPE_OBJECT,
                               THUNARX_IMPLEMENT_INTERFACE (THUNARX_TYPE_MENU_PROVIDER,
                                                            thunar_uca_provider_menu_provider_init)
                               THUNARX_IMPLEMENT_INTERFACE (THUNARX_TYPE_PREFERENCES_PROVIDER,
                                                            thunar_uca_provider_preferences_provider_init));



static void
thunar_uca_provider_class_init (ThunarUcaProviderClass *klass)
{
  GObjectClass *gobject_class;

  /* setup the "thunar-uca-context", "thunar-uca-folder" and "thunar-uca-row" quarks */
  thunar_uca_context_quark = g_quark_from_string ("thunar-uca-context");
  thunar_uca_folder_quark = g_quark_from_string ("thunar-uca-folder");
  thunar_uca_row_quark = g_quark_from_string ("thunar-uca-row");

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_uca_provider_finalize;
}



static void
thunar_uca_provider_menu_provider_init (ThunarxMenuProviderIface *iface)
{
  iface->get_file_actions = thunar_uca_provider_get_file_actions;
  iface->get_folder_actions = thunar_uca_provider_get_folder_actions;
}



static void
thunar_uca_provider_preferences_provider_init (ThunarxPreferencesProviderIface *iface)
{
  iface->get_actions = thunar_uca_provider_get_actions;
}



static void
thunar_uca_provider_init (ThunarUcaProvider *uca_provider)
{
  /* setup the i18n support first */
  thunar_uca_i18n_init ();

  /* grab a reference on the default model */
  uca_provider->model = thunar_uca_model_get_default ();

  /* initialize child watch support */
  uca_provider->child_watch_path = NULL;
  uca_provider->child_watch_id = -1;
}



static void
thunar_uca_provider_finalize (GObject *object)
{
  ThunarUcaProvider *uca_provider = THUNAR_UCA_PROVIDER (object);
  GSource           *source;

  /* give up maintaince of any pending child watch */
  if (G_UNLIKELY (uca_provider->child_watch_id >= 0))
    {
      /* reset the callback function to g_spawn_close_pid() so the plugin can be
       * safely unloaded and the child will still not become a zombie afterwards.
       * This also resets the child_watch_id and child_watch_path properties.
       */
      source = g_main_context_find_source_by_id (NULL, uca_provider->child_watch_id);
      g_source_set_callback (source, (GSourceFunc) g_spawn_close_pid, NULL, NULL);
    }

  /* drop our reference on the model */
  g_object_unref (G_OBJECT (uca_provider->model));

  (*G_OBJECT_CLASS (thunar_uca_provider_parent_class)->finalize) (object);
}



static void
manage_actions (GtkWindow *window)
{
  GtkWidget *dialog;

  dialog = g_object_new (THUNAR_UCA_TYPE_CHOOSER, NULL);
  gtk_window_set_transient_for (GTK_WINDOW (dialog), window);
  gtk_widget_show (dialog);
}



static GList*
thunar_uca_provider_get_actions (ThunarxPreferencesProvider *preferences_provider,
                                 GtkWidget                  *window)
{
  GtkAction *action;
  GClosure  *closure;

  action = gtk_action_new ("ThunarUca::manage-actions", _("Configure c_ustom actions..."),
                           _("Setup custom actions that will appear in the file managers context menus"), NULL);
  closure = g_cclosure_new_object_swap (G_CALLBACK (manage_actions), G_OBJECT (window));
  g_signal_connect_closure (G_OBJECT (action), "activate", closure, TRUE);

  return g_list_prepend (NULL, action);
}



static GList*
thunar_uca_provider_get_file_actions (ThunarxMenuProvider *menu_provider,
                                      GtkWidget           *window,
                                      GList               *files)
{
  GtkTreeRowReference *row;
  ThunarUcaProvider   *uca_provider = THUNAR_UCA_PROVIDER (menu_provider);
  ThunarUcaContext    *uca_context = NULL;
  GtkTreeIter          iter;
  GtkAction           *action;
  GList               *actions = NULL;
  GList               *paths;
  GList               *lp;
  gchar               *stock_id;
  gchar               *tooltip;
  gchar               *label;
  gchar               *name;

  paths = thunar_uca_model_match (uca_provider->model, files);
  for (lp = g_list_last (paths); lp != NULL; lp = lp->prev)
    {
      /* try to lookup the tree iter for the specified tree path */
      if (gtk_tree_model_get_iter (GTK_TREE_MODEL (uca_provider->model), &iter, lp->data))
        {
          /* determine the label, tooltip and stock-id for the item */
          gtk_tree_model_get (GTK_TREE_MODEL (uca_provider->model), &iter,
                              THUNAR_UCA_MODEL_COLUMN_NAME, &label,
                              THUNAR_UCA_MODEL_COLUMN_STOCK_ID, &stock_id,
                              THUNAR_UCA_MODEL_COLUMN_DESCRIPTION, &tooltip,
                              -1);

          /* generate a unique action name */
          name = g_strdup_printf ("ThunarUca::action-%d", ++uca_provider->last_action_id);

          /* create the new action with the given parameters */
          action = gtk_action_new (name, label, tooltip, stock_id);

          /* grab a tree row reference on the given path */
          row = gtk_tree_row_reference_new (GTK_TREE_MODEL (uca_provider->model), lp->data);
          g_object_set_qdata_full (G_OBJECT (action), thunar_uca_row_quark, row,
                                   (GDestroyNotify) gtk_tree_row_reference_free);

          /* allocate a new context on-demand */
          if (G_LIKELY (uca_context == NULL))
            uca_context = thunar_uca_context_new (window, files);
          else
            uca_context = thunar_uca_context_ref (uca_context);
          g_object_set_qdata_full (G_OBJECT (action), thunar_uca_context_quark, uca_context, (GDestroyNotify) thunar_uca_context_unref);

          /* connect the "activate" signal */
          g_signal_connect_data (G_OBJECT (action), "activate", G_CALLBACK (thunar_uca_provider_activated),
                                 g_object_ref (G_OBJECT (uca_provider)), (GClosureNotify) g_object_unref,
                                 G_CONNECT_SWAPPED);

          /* add the action to the return list */
          actions = g_list_prepend (actions, action);

          /* cleanup */
          g_free (stock_id);
          g_free (tooltip);
          g_free (label);
          g_free (name);
        }

      /* release the tree path */
      gtk_tree_path_free (lp->data);
    }
  g_list_free (paths);

  return actions;
}



static GList*
thunar_uca_provider_get_folder_actions (ThunarxMenuProvider *menu_provider,
                                        GtkWidget           *window,
                                        ThunarxFileInfo     *folder)
{
  GList *actions;
  GList  files;
  GList *lp;

  /* fake a file list... */
  files.data = folder;
  files.next = NULL;
  files.prev = NULL;

  /* ...and use the get_file_actions() method */
  actions = thunarx_menu_provider_get_file_actions (menu_provider, window, &files);

  /* mark the actions, so we can properly detect the working directory */
  for (lp = actions; lp != NULL; lp = lp->next)
    g_object_set_qdata (G_OBJECT (lp->data), thunar_uca_folder_quark, GUINT_TO_POINTER (TRUE));

  return actions;
}



static void
thunar_uca_provider_activated (ThunarUcaProvider *uca_provider,
                               GtkAction         *action)
{
  GtkTreeRowReference *row;
  ThunarUcaContext    *uca_context;
  GtkTreePath         *path;
  GtkTreeIter          iter;
  GtkWidget           *dialog;
  GtkWidget           *window;
  gboolean             succeed;
  GSource             *source;
  GError              *error = NULL;
  GList               *files;
  gchar              **argv;
  gchar               *working_directory = NULL;
  gchar               *filename;
  gchar               *label;
  gchar               *uri;
  gint                 argc;
  gint                 pid;

  g_return_if_fail (THUNAR_UCA_IS_PROVIDER (uca_provider));
  g_return_if_fail (GTK_IS_ACTION (action));

  /* check if the row reference is still valid */
  row = g_object_get_qdata (G_OBJECT (action), thunar_uca_row_quark);
  if (G_UNLIKELY (!gtk_tree_row_reference_valid (row)))
    return;

  /* determine the iterator for the item */
  path = gtk_tree_row_reference_get_path (row);
  gtk_tree_model_get_iter (GTK_TREE_MODEL (uca_provider->model), &iter, path);
  gtk_tree_path_free (path);

  /* determine the files and the window for the action */
  uca_context = g_object_get_qdata (G_OBJECT (action), thunar_uca_context_quark);
  window = thunar_uca_context_get_window (uca_context);
  files = thunar_uca_context_get_files (uca_context);

  /* determine the argc/argv for the item */
  succeed = thunar_uca_model_parse_argv (uca_provider->model, &iter, files, &argc, &argv, &error);
  if (G_LIKELY (succeed))
    {
      /* determine the working from the first file */
      if (G_LIKELY (files != NULL))
        {
          /* determine the filename of the first selected file */
          uri = thunarx_file_info_get_uri (files->data);
          filename = g_filename_from_uri (uri, NULL, NULL);
          if (G_LIKELY (filename != NULL))
            {
              /* if this is a folder action, we just use the filename as working directory */
              if (g_object_get_qdata (G_OBJECT (action), thunar_uca_folder_quark) != NULL)
                {
                  working_directory = filename;
                  filename = NULL;
                }
              else
                {
                  working_directory = g_path_get_dirname (filename);
                }
            }
          g_free (filename);
          g_free (uri);
        }

      /* spawn the command on the window's screen */
      succeed = gdk_spawn_on_screen (gtk_widget_get_screen (GTK_WIDGET (window)), working_directory,
                                     argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH,
                                     NULL, NULL, &pid, &error);

      /* check if we succeed */
      if (G_LIKELY (succeed))
        {
          /* check if we already have a child watch */
          if (G_UNLIKELY (uca_provider->child_watch_id >= 0))
            {
              /* reset the callback function to g_spawn_close_pid() so the plugin can be
               * safely unloaded and the child will still not become a zombie afterwards.
               */
              source = g_main_context_find_source_by_id (NULL, uca_provider->child_watch_id);
              g_source_set_callback (source, (GSourceFunc) g_spawn_close_pid, NULL, NULL);
            }

          /* schedule the new child watch */
          uca_provider->child_watch_id = g_child_watch_add_full (G_PRIORITY_LOW, pid, thunar_uca_provider_child_watch,
                                                                 uca_provider, thunar_uca_provider_child_watch_destroy);

          /* take over ownership of the working directory as child watch path */
          uca_provider->child_watch_path = working_directory;
          working_directory = NULL;
        }

      /* cleanup */
      g_free (working_directory);
      g_strfreev (argv);
    }

  /* present error message to the user */
  if (G_UNLIKELY (!succeed))
    {
      g_object_get (G_OBJECT (action), "label", &label, NULL);
      dialog = gtk_message_dialog_new ((GtkWindow *) window,
                                       GTK_DIALOG_DESTROY_WITH_PARENT
                                       | GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_CLOSE,
                                       _("Failed to launch action \"%s\"."), label);
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", error->message);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      g_error_free (error);
      g_free (label);
    }
}



static void
thunar_uca_provider_child_watch (GPid     pid,
                                 gint     status,
                                 gpointer user_data)
{
  ThunarUcaProvider *uca_provider = THUNAR_UCA_PROVIDER (user_data);
  ThunarVfsMonitor  *monitor;
  ThunarVfsPath     *path;

  GDK_THREADS_ENTER ();

  /* verify that we still have a valid child_watch_path */
  if (G_LIKELY (uca_provider->child_watch_path != NULL))
    {
      /* determine the corresponding ThunarVfsPath */
      path = thunar_vfs_path_new (uca_provider->child_watch_path, NULL);
      if (G_LIKELY (path != NULL))
        {
          /* schedule a changed notification on the path */
          monitor = thunar_vfs_monitor_get_default ();
          thunar_vfs_monitor_feed (monitor, THUNAR_VFS_MONITOR_EVENT_CHANGED, path);
          g_object_unref (G_OBJECT (monitor));

          /* release the ThunarVfsPath */
          thunar_vfs_path_unref (path);
        }
    }

  /* need to cleanup */
  g_spawn_close_pid (pid);

  GDK_THREADS_LEAVE ();
}



static void
thunar_uca_provider_child_watch_destroy (gpointer user_data)
{
  ThunarUcaProvider *uca_provider = THUNAR_UCA_PROVIDER (user_data);

  /* reset child watch id and path */
  g_free (uca_provider->child_watch_path);
  uca_provider->child_watch_path = NULL;
  uca_provider->child_watch_id = -1;
}



