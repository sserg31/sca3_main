/* $Id: thunar-dbus-service.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>
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

#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>

#include <glib/gstdio.h>

#include <thunar/thunar-application.h>
#include <thunar/thunar-chooser-dialog.h>
#include <thunar/thunar-dbus-service.h>
#include <thunar/thunar-file.h>
#include <thunar/thunar-gdk-extensions.h>
#include <thunar/thunar-preferences-dialog.h>
#include <thunar/thunar-private.h>
#include <thunar/thunar-properties-dialog.h>



static void     thunar_dbus_service_class_init                  (ThunarDBusServiceClass *klass);
static void     thunar_dbus_service_init                        (ThunarDBusService      *dbus_service);
static void     thunar_dbus_service_finalize                    (GObject                *object);
static gboolean thunar_dbus_service_connect_trash_bin           (ThunarDBusService      *dbus_service,
                                                                 GError                **error);
static gboolean thunar_dbus_service_parse_uri_and_display       (ThunarDBusService      *dbus_service,
                                                                 const gchar            *uri,
                                                                 const gchar            *display,
                                                                 ThunarFile            **file_return,
                                                                 GdkScreen             **screen_return,
                                                                 GError                **error);
static void     thunar_dbus_service_trash_bin_changed           (ThunarDBusService      *dbus_service,
                                                                 ThunarFile             *trash_bin);
static gboolean thunar_dbus_service_display_chooser_dialog      (ThunarDBusService      *dbus_service,
                                                                 const gchar            *uri,
                                                                 gboolean                open,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_display_folder              (ThunarDBusService      *dbus_service,
                                                                 const gchar            *uri,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_display_folder_and_select   (ThunarDBusService      *dbus_service,
                                                                 const gchar            *uri,
                                                                 const gchar            *filename,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_display_file_properties     (ThunarDBusService      *dbus_service,
                                                                 const gchar            *uri,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_launch                      (ThunarDBusService      *dbus_service,
                                                                 const gchar            *uri,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_display_preferences_dialog  (ThunarDBusService      *dbus_service,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_display_trash               (ThunarDBusService      *dbus_service,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_empty_trash                 (ThunarDBusService      *dbus_service,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_move_to_trash               (ThunarDBusService      *dbus_service,
                                                                 gchar                 **filenames,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_query_trash                 (ThunarDBusService      *dbus_service,
                                                                 gboolean               *empty,
                                                                 GError                **error);
static gboolean thunar_dbus_service_bulk_rename                 (ThunarDBusService      *dbus_service,
                                                                 const gchar            *working_directory,
                                                                 gchar                 **filenames,
                                                                 gboolean                standalone,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_launch_files                (ThunarDBusService      *dbus_service,
                                                                 const gchar            *working_directory,
                                                                 gchar                 **filenames,
                                                                 const gchar            *display,
                                                                 GError                **error);
static gboolean thunar_dbus_service_terminate                   (ThunarDBusService      *dbus_service,
                                                                 GError                **error);



struct _ThunarDBusServiceClass
{
  GObjectClass __parent__;
};

struct _ThunarDBusService
{
  GObject __parent__;

  DBusGConnection *connection;
  ThunarFile      *trash_bin;
};



static GObjectClass *thunar_dbus_service_parent_class;



GType
thunar_dbus_service_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarDBusServiceClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_dbus_service_class_init,
        NULL,
        NULL,
        sizeof (ThunarDBusService),
        0,
        (GInstanceInitFunc) thunar_dbus_service_init,
        NULL,
      };

      type = g_type_register_static (G_TYPE_OBJECT, I_("ThunarDBusService"), &info, 0);
    }

  return type;
}



static void
thunar_dbus_service_class_init (ThunarDBusServiceClass *klass)
{
  extern const DBusGObjectInfo dbus_glib_thunar_dbus_service_object_info;
  GObjectClass                *gobject_class;

  /* determine the parent type class */
  thunar_dbus_service_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_dbus_service_finalize;

  /* install the D-BUS info for our class */
  dbus_g_object_type_install_info (G_TYPE_FROM_CLASS (klass), &dbus_glib_thunar_dbus_service_object_info);

  /**
   * ThunarDBusService::trash-changed:
   * @dbus_service : a #ThunarDBusService.
   * @full         : %TRUE if the trash bin now contains atleast
   *                 one item, %FALSE otherwise.
   *
   * This signal is emitted whenever the state of the trash bin
   * changes. Note that this signal is only emitted after the
   * trash has previously been queried by a D-BUS client.
   **/
  g_signal_new (I_("trash-changed"),
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST,
                0, NULL, NULL,
                g_cclosure_marshal_VOID__BOOLEAN,
                G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}



static void
thunar_dbus_service_init (ThunarDBusService *dbus_service)
{
  GError *error = NULL;

  /* try to connect to the session bus */
  dbus_service->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (G_LIKELY (dbus_service->connection != NULL))
    {
      /* register the /org/xfce/FileManager object for Thunar */
      dbus_g_connection_register_g_object (dbus_service->connection, "/org/xfce/FileManager", G_OBJECT (dbus_service));

      /* request the org.xfce.Thunar name for Thunar */
      dbus_bus_request_name (dbus_g_connection_get_connection (dbus_service->connection), "org.xfce.Thunar", DBUS_NAME_FLAG_REPLACE_EXISTING, NULL);

      /* request the org.xfce.FileManager name for Thunar */
      dbus_bus_request_name (dbus_g_connection_get_connection (dbus_service->connection), "org.xfce.FileManager", DBUS_NAME_FLAG_REPLACE_EXISTING, NULL);
    }
  else
    {
      /* notify the user that D-BUS service won't be available */
      g_fprintf (stderr, "Thunar: Failed to connect to the D-BUS session bus: %s\n", error->message);
      g_error_free (error);
    }
}



static void
thunar_dbus_service_finalize (GObject *object)
{
  ThunarDBusService *dbus_service = THUNAR_DBUS_SERVICE (object);

  /* release the D-BUS connection object */
  if (G_LIKELY (dbus_service->connection != NULL))
    dbus_g_connection_unref (dbus_service->connection);

  /* check if we are connected to the trash bin */
  if (G_LIKELY (dbus_service->trash_bin != NULL))
    {
      /* unwatch the trash bin */
      thunar_file_unwatch (dbus_service->trash_bin);

      /* release the trash bin */
      g_signal_handlers_disconnect_by_func (G_OBJECT (dbus_service->trash_bin), thunar_dbus_service_trash_bin_changed, dbus_service);
      g_object_unref (G_OBJECT (dbus_service->trash_bin));
    }

  (*G_OBJECT_CLASS (thunar_dbus_service_parent_class)->finalize) (object);
}



static gboolean
thunar_dbus_service_connect_trash_bin (ThunarDBusService *dbus_service,
                                       GError           **error)
{
  ThunarVfsPath *trash_bin_path;

  /* check if we're not already connected to the trash bin */
  if (G_UNLIKELY (dbus_service->trash_bin == NULL))
    {
      /* try to connect to the trash bin */
      trash_bin_path = thunar_vfs_path_get_for_trash ();
      dbus_service->trash_bin = thunar_file_get_for_path (trash_bin_path, error);
      if (G_LIKELY (dbus_service->trash_bin != NULL))
        {
          /* watch the trash bin for changes */
          thunar_file_watch (dbus_service->trash_bin);

          /* stay informed about changes to the trash bin */
          g_signal_connect_swapped (G_OBJECT (dbus_service->trash_bin), "changed",
                                    G_CALLBACK (thunar_dbus_service_trash_bin_changed),
                                    dbus_service);
        }
      thunar_vfs_path_unref (trash_bin_path);
    }

  return (dbus_service->trash_bin != NULL);
}



static gboolean
thunar_dbus_service_parse_uri_and_display (ThunarDBusService *dbus_service,
                                           const gchar       *uri,
                                           const gchar       *display,
                                           ThunarFile       **file_return,
                                           GdkScreen        **screen_return,
                                           GError           **error)
{
  /* try to open the display */
  *screen_return = thunar_gdk_screen_open (display, error);
  if (G_UNLIKELY (*screen_return == NULL))
    return FALSE;

  /* try to determine the file for the URI */
  *file_return = thunar_file_get_for_uri (uri, error);
  if (G_UNLIKELY (*file_return == NULL))
    {
      g_object_unref (G_OBJECT (*screen_return));
      return FALSE;
    }

  return TRUE;
}



static void
thunar_dbus_service_trash_bin_changed (ThunarDBusService *dbus_service,
                                       ThunarFile        *trash_bin)
{
  _thunar_return_if_fail (THUNAR_IS_DBUS_SERVICE (dbus_service));
  _thunar_return_if_fail (dbus_service->trash_bin == trash_bin);
  _thunar_return_if_fail (THUNAR_IS_FILE (trash_bin));

  /* emit the "trash-changed" signal with the new state */
  g_signal_emit_by_name (G_OBJECT (dbus_service), "trash-changed", (thunar_file_get_size (trash_bin) > 0));
}



static gboolean
thunar_dbus_service_display_chooser_dialog (ThunarDBusService *dbus_service,
                                            const gchar       *uri,
                                            gboolean           open,
                                            const gchar       *display,
                                            GError           **error)
{
  ThunarFile *file;
  GdkScreen  *screen;

  /* parse uri and display parameters */
  if (!thunar_dbus_service_parse_uri_and_display (dbus_service, uri, display, &file, &screen, error))
    return FALSE;

  /* popup the chooser dialog */
  thunar_show_chooser_dialog (screen, file, open);

  /* cleanup */
  g_object_unref (G_OBJECT (screen));
  g_object_unref (G_OBJECT (file));

  return TRUE;
}



static gboolean
thunar_dbus_service_display_folder (ThunarDBusService *dbus_service,
                                    const gchar       *uri,
                                    const gchar       *display,
                                    GError           **error)
{
  ThunarApplication *application;
  ThunarFile        *file;
  GdkScreen         *screen;

  /* parse uri and display parameters */
  if (!thunar_dbus_service_parse_uri_and_display (dbus_service, uri, display, &file, &screen, error))
    return FALSE;

  /* popup a new window for the folder */
  application = thunar_application_get ();
  thunar_application_open_window (application, file, screen);
  g_object_unref (G_OBJECT (application));

  /* cleanup */
  g_object_unref (G_OBJECT (screen));
  g_object_unref (G_OBJECT (file));

  return TRUE;
}



static gboolean
thunar_dbus_service_display_folder_and_select (ThunarDBusService *dbus_service,
                                               const gchar       *uri,
                                               const gchar       *filename,
                                               const gchar       *display,
                                               GError           **error)
{
  ThunarApplication *application;
  ThunarVfsPath     *path;
  ThunarFile        *file;
  ThunarFile        *folder;
  GdkScreen         *screen;
  GtkWidget         *window;

  /* verify that filename is valid */
  if (G_UNLIKELY (filename == NULL || *filename == '\0' || strchr (filename, '/') != NULL))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Invalid filename \"%s\""), filename);
      return FALSE;
    }

  /* parse uri and display parameters */
  if (!thunar_dbus_service_parse_uri_and_display (dbus_service, uri, display, &folder, &screen, error))
    return FALSE;

  /* popup a new window for the folder */
  application = thunar_application_get ();
  window = thunar_application_open_window (application, folder, screen);
  g_object_unref (G_OBJECT (application));

  /* determine the path for the filename relative to the folder */
  path = thunar_vfs_path_relative (thunar_file_get_path (folder), filename);
  if (G_LIKELY (path != NULL))
    {
      /* try to determine the file for the path */
      file = thunar_file_get_for_path (path, NULL);
      if (G_LIKELY (file != NULL))
        {
          /* tell the window to scroll to the given file and select it */
          thunar_window_scroll_to_file (THUNAR_WINDOW (window), file, TRUE, TRUE, 0.5f, 0.5f);

          /* release the file reference */
          g_object_unref (G_OBJECT (file));
        }

      /* release the path */
      thunar_vfs_path_unref (path);
    }

  /* cleanup */
  g_object_unref (G_OBJECT (screen));
  g_object_unref (G_OBJECT (folder));

  return TRUE;
}



static gboolean
thunar_dbus_service_display_file_properties (ThunarDBusService *dbus_service,
                                             const gchar       *uri,
                                             const gchar       *display,
                                             GError           **error)
{
  ThunarApplication *application;
  ThunarFile        *file;
  GdkScreen         *screen;
  GtkWidget         *dialog;

  /* parse uri and display parameters */
  if (!thunar_dbus_service_parse_uri_and_display (dbus_service, uri, display, &file, &screen, error))
    return FALSE;

  /* popup the file properties dialog */
  dialog = thunar_properties_dialog_new ();
  gtk_window_set_screen (GTK_WINDOW (dialog), screen);
  thunar_properties_dialog_set_file (THUNAR_PROPERTIES_DIALOG (dialog), file);
  gtk_widget_show (dialog);

  /* let the application take control over the dialog */
  application = thunar_application_get ();
  thunar_application_take_window (application, GTK_WINDOW (dialog));
  g_object_unref (G_OBJECT (application));

  /* cleanup */
  g_object_unref (G_OBJECT (screen));
  g_object_unref (G_OBJECT (file));

  return TRUE;
}



static gboolean
thunar_dbus_service_launch (ThunarDBusService *dbus_service,
                            const gchar       *uri,
                            const gchar       *display,
                            GError           **error)
{
  ThunarFile *file;
  GdkScreen  *screen;
  gboolean    result = FALSE;

  /* parse uri and display parameters */
  if (thunar_dbus_service_parse_uri_and_display (dbus_service, uri, display, &file, &screen, error))
    {
      /* try to launch the file on the given screen */
      result = thunar_file_launch (file, screen, error);

      /* cleanup */
      g_object_unref (G_OBJECT (screen));
      g_object_unref (G_OBJECT (file));
    }

  return result;
}



static gboolean
thunar_dbus_service_display_preferences_dialog (ThunarDBusService *dbus_service,
                                                const gchar       *display,
                                                GError           **error)
{
  ThunarApplication *application;
  GdkScreen         *screen;
  GtkWidget         *dialog;

  /* try to open the screen for the display name */
  screen = thunar_gdk_screen_open (display, error);
  if (G_UNLIKELY (screen == NULL))
    return FALSE;

  /* popup the preferences dialog... */
  dialog = thunar_preferences_dialog_new (NULL);
  gtk_window_set_screen (GTK_WINDOW (dialog), screen);
  gtk_widget_show (dialog);

  /* ...and let the application take care of it */
  application = thunar_application_get ();
  thunar_application_take_window (application, GTK_WINDOW (dialog));
  g_object_unref (G_OBJECT (application));

  /* cleanup */
  g_object_unref (G_OBJECT (screen));

  return TRUE;
}



static gboolean
thunar_dbus_service_display_trash (ThunarDBusService *dbus_service,
                                   const gchar       *display,
                                   GError           **error)
{
  ThunarApplication *application;
  GdkScreen         *screen;

  /* connect to the trash bin on-demand */
  if (!thunar_dbus_service_connect_trash_bin (dbus_service, error))
    return FALSE;

  /* try to open the screen for the display name */
  screen = thunar_gdk_screen_open (display, error);
  if (G_LIKELY (screen != NULL))
    {
      /* tell the application to display the trash bin */
      application = thunar_application_get ();
      thunar_application_open_window (application, dbus_service->trash_bin, screen);
      g_object_unref (G_OBJECT (application));

      /* release the screen */
      g_object_unref (G_OBJECT (screen));
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_dbus_service_empty_trash (ThunarDBusService *dbus_service,
                                 const gchar       *display,
                                 GError           **error)
{
  ThunarApplication *application;
  GdkScreen         *screen;

  /* try to open the screen for the display name */
  screen = thunar_gdk_screen_open (display, error);
  if (G_LIKELY (screen != NULL))
    {
      /* tell the application to empty the trash bin */
      application = thunar_application_get ();
      thunar_application_empty_trash (application, screen);
      g_object_unref (G_OBJECT (application));

      /* release the screen */
      g_object_unref (G_OBJECT (screen));
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_dbus_service_move_to_trash (ThunarDBusService *dbus_service,
                                   gchar            **filenames,
                                   const gchar       *display,
                                   GError           **error)
{
  ThunarApplication *application;
  ThunarVfsPath     *target_path;
  ThunarVfsPath     *path;
  GdkScreen         *screen;
  GError            *err = NULL;
  GList             *path_list = NULL;
  gchar             *filename;
  guint              n;

  /* try to open the screen for the display name */
  screen = thunar_gdk_screen_open (display, &err);
  if (G_LIKELY (screen != NULL))
    {
      /* try to parse the specified filenames */
      for (n = 0; err == NULL && filenames[n] != NULL; ++n)
        {
          /* decode the filename (D-BUS uses UTF-8) */
          filename = g_filename_from_utf8 (filenames[n], -1, NULL, NULL, &err);
          if (G_LIKELY (err == NULL))
            {
              /* determine the path for the filename */
              path = thunar_vfs_path_new (filename, &err);
              if (G_LIKELY (path != NULL))
                path_list = g_list_append (path_list, path);
            }

          /* cleanup */
          g_free (filename);
        }

      /* check if we succeed */
      if (G_LIKELY (err == NULL))
        {
          /* tell the application to move the specified files to the trash */
          application = thunar_application_get ();
          target_path = thunar_vfs_path_get_for_trash ();
          thunar_application_move_into (application, screen, path_list, target_path, NULL);
          g_object_unref (G_OBJECT (application));
          thunar_vfs_path_unref (target_path);
        }

      /* cleanup */
      thunar_vfs_path_list_free (path_list);
      g_object_unref (G_OBJECT (screen));
    }

  /* check if we failed */
  if (G_UNLIKELY (err != NULL))
    {
      /* propagate the error */
      g_propagate_error (error, err);
      return FALSE;
    }

  return TRUE;
}



static gboolean
thunar_dbus_service_query_trash (ThunarDBusService *dbus_service,
                                 gboolean          *full,
                                 GError           **error)
{
  /* connect to the trash bin on-demand */
  if (thunar_dbus_service_connect_trash_bin (dbus_service, error))
    {
      /* check whether the trash bin is not empty */
      *full = (thunar_file_get_size (dbus_service->trash_bin) > 0);
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_dbus_service_bulk_rename (ThunarDBusService *dbus_service,
                                 const gchar       *working_directory,
                                 gchar            **filenames,
                                 gboolean           standalone,
                                 const gchar       *display,
                                 GError           **error)
{
  ThunarApplication *application;
  GdkScreen         *screen;
  gboolean           result = FALSE;
  gchar             *cwd;

  /* determine a proper working directory */
  cwd = (working_directory != NULL && *working_directory != '\0')
      ? g_strdup (working_directory)
      : g_get_current_dir ();

  /* try to open the screen for the display name */
  screen = thunar_gdk_screen_open (display, error);
  if (G_LIKELY (screen != NULL))
    {
      /* tell the application to display the bulk rename dialog */
      application = thunar_application_get ();
      result = thunar_application_bulk_rename (application, cwd, filenames, standalone, screen, error);
      g_object_unref (G_OBJECT (application));

      /* release the screen */
      g_object_unref (G_OBJECT (screen));
    }

  /* release the cwd */
  g_free (cwd);

  return result;
}



static gboolean
thunar_dbus_service_launch_files (ThunarDBusService *dbus_service,
                                  const gchar       *working_directory,
                                  gchar            **filenames,
                                  const gchar       *display,
                                  GError           **error)
{
  ThunarApplication *application;
  GdkScreen         *screen;
  gboolean           result = FALSE;

  /* verify that a valid working directory is given */
  if (G_UNLIKELY (!g_path_is_absolute (working_directory)))
    {
      /* LaunchFiles() invoked without a valid working directory */
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("The working directory must be an absolute path"));
      return FALSE;
    }

  /* verify that atleast one filename is given */
  if (G_UNLIKELY (filenames == NULL || *filenames == NULL))
    {
      /* LaunchFiles() invoked with an empty filename list */
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Atleast one filename must be specified"));
      return FALSE;
    }

  /* try to open the screen for the display name */
  screen = thunar_gdk_screen_open (display, error);
  if (G_LIKELY (screen != NULL))
    {
      /* let the application process the filenames */
      application = thunar_application_get ();
      result = thunar_application_process_filenames (application, working_directory, filenames, screen, error);
      g_object_unref (G_OBJECT (application));

      /* release the screen */
      g_object_unref (G_OBJECT (screen));
    }

  return result;
}



static gboolean
thunar_dbus_service_terminate (ThunarDBusService *dbus_service,
                               GError           **error)
{
  /* leave the Gtk main loop as soon as possible */
  gtk_main_quit ();

  /* we cannot fail */
  return TRUE;
}



#include <thunar/thunar-dbus-service-infos.h>
