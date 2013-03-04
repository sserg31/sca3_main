/* $Id: main.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2007 os-cillation e.K.
 *
 * Written by Benedikt Meurer <benny@xfce.org>.
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

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <glib/gstdio.h>

#include <thunar/thunar-application.h>
#include <thunar/thunar-dbus-client.h>
#include <thunar/thunar-dbus-service.h>
#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-session-client.h>
#include <thunar/thunar-stock.h>



/* --- globals --- */
static gboolean opt_bulk_rename = FALSE;
static gboolean opt_daemon = FALSE;
static gchar   *opt_sm_client_id = NULL;
static gboolean opt_quit = FALSE;
static gboolean opt_version = FALSE;



/* --- command line options --- */
static GOptionEntry option_entries[] =
{
  { "bulk-rename", 'B', 0, G_OPTION_ARG_NONE, &opt_bulk_rename, N_ ("Open the bulk rename dialog"), NULL, },
#ifdef HAVE_DBUS
  { "daemon", 0, 0, G_OPTION_ARG_NONE, &opt_daemon, N_ ("Run in daemon mode"), NULL, },
#else
  { "daemon", 0, 0, G_OPTION_ARG_NONE, &opt_daemon, N_ ("Run in daemon mode (not supported)"), NULL, },
#endif
  { "sm-client-id", 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &opt_sm_client_id, NULL, NULL, },
#ifdef HAVE_DBUS
  { "quit", 'q', 0, G_OPTION_ARG_NONE, &opt_quit, N_ ("Quit a running Thunar instance"), NULL, },
#else
  { "quit", 'q', 0, G_OPTION_ARG_NONE, &opt_quit, N_ ("Quit a running Thunar instance (not supported)"), NULL, },
#endif
  { "version", 'v', 0, G_OPTION_ARG_NONE, &opt_version, N_ ("Print version information and exit"), NULL, },
  { NULL, },
};



int
main (int argc, char **argv)
{
  ThunarSessionClient *session_client;
#ifdef HAVE_DBUS
  ThunarDBusService   *dbus_service;
#endif
  ThunarApplication   *application;
  GError              *error = NULL;
  gchar               *working_directory;
  gchar              **filenames = NULL;

  /* setup translation domain */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  /* setup application name */
  g_set_application_name (_("Thunar"));

#ifdef G_ENABLE_DEBUG
  /* Do NOT remove this line for now, If something doesn't work,
   * fix your code instead!
   */
  g_log_set_always_fatal (G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);
#endif

  /* initialize the GThread system */
  if (!g_thread_supported ())
    g_thread_init (NULL);

  /* initialize Gtk+ */
  if (!gtk_init_with_args (&argc, &argv, _("[FILES...]"), option_entries, GETTEXT_PACKAGE, &error))
    {
      /* check if we have an error message */
      if (G_LIKELY (error == NULL))
        {
          /* no error message, the GUI initialization failed */
          const gchar *display_name = gdk_get_display_arg_name ();
          g_fprintf (stderr, _("Thunar: Failed to open display: %s\n"), (display_name != NULL) ? display_name : " ");
        }
      else
        {
          /* yep, there's an error, so print it */
          g_fprintf (stderr, _("Thunar: %s\n"), error->message);
          g_error_free (error);
        }
      return EXIT_FAILURE;
    }

  /* check if we should print version information */
  if (G_UNLIKELY (opt_version))
    {
      g_print ("%s %s (Xfce %s)\n\n", PACKAGE_NAME, PACKAGE_VERSION, xfce_version_string ());
      g_print ("%s\n", "Copyright (c) 2004-2007");
      g_print ("\t%s\n\n", _("The Thunar development team. All rights reserved."));
      g_print ("%s\n\n", _("Written by Benedikt Meurer <benny@xfce.org>."));
      g_print (_("Please report bugs to <%s>."), PACKAGE_BUGREPORT);
      g_print ("\n");
      return EXIT_SUCCESS;
    }

  /* register additional transformation functions */
  thunar_g_initialize_transformations ();

#ifdef HAVE_DBUS
  /* check if we should terminate a running Thunar instance */
  if (G_UNLIKELY (opt_quit))
    {
      /* try to terminate whatever is running */
      if (!thunar_dbus_client_terminate (&error))
        {
          g_fprintf (stderr, "Thunar: Failed to terminate running instance: %s\n", error->message);
          g_error_free (error);
          return EXIT_FAILURE;
        }

      return EXIT_SUCCESS;
    }
#endif

  /* determine the current working directory */
  working_directory = g_get_current_dir ();

  /* check if atleast one filename was specified, else
   * fall back to opening the current working directory
   * if daemon mode is not requested.
   */
  if (G_LIKELY (argc > 1))
    {
      /* use the specified filenames */
      filenames = g_strdupv (argv + 1);
    }
  else if (opt_bulk_rename)
    {
      /* default to an empty list */
      filenames = g_new (gchar *, 1);
      filenames[0] = NULL;
    }
  else if (!opt_daemon)
    {
      /* use the current working directory */
      filenames = g_new (gchar *, 2);
      filenames[0] = g_strdup (working_directory);
      filenames[1] = NULL;
    }

#ifdef HAVE_DBUS
  /* check if we can reuse an existing instance */
  if ((!opt_bulk_rename && filenames != NULL && thunar_dbus_client_launch_files (working_directory, filenames, NULL, NULL))
      || (opt_bulk_rename && thunar_dbus_client_bulk_rename (working_directory, filenames, TRUE, NULL, NULL)))
    {
      /* stop any running startup notification */
      gdk_notify_startup_complete ();

      /* that worked, let's get outa here */
      g_free (working_directory);
      g_strfreev (filenames);
      return EXIT_SUCCESS;
    }
#endif

  /* initialize the ThunarVFS library */
  thunar_vfs_init ();

  /* initialize the thunar stock items/icons */
  thunar_stock_init ();

  /* acquire a reference on the global application */
  application = thunar_application_get ();

#ifdef HAVE_DBUS
  /* setup daemon mode if requested and supported */
  thunar_application_set_daemon (application, opt_daemon);
#endif

  /* use the Thunar icon as default for new windows */
  gtk_window_set_default_icon_name ("Thunar");

  /* check if we should open the bulk rename dialog */
  if (G_UNLIKELY (opt_bulk_rename))
    {
      /* try to open the bulk rename dialog */
      if (!thunar_application_bulk_rename (application, working_directory, filenames, TRUE, NULL, &error))
        goto error0;
    }
  else if (filenames != NULL && !thunar_application_process_filenames (application, working_directory, filenames, NULL, &error))
    {
      /* we failed to process the filenames or the bulk rename failed */
error0:
      g_fprintf (stderr, "Thunar: %s\n", error->message);
      g_object_unref (G_OBJECT (application));
      thunar_vfs_shutdown ();
      g_error_free (error);
      return EXIT_FAILURE;
    }

  /* release working directory and filenames */
  g_free (working_directory);
  g_strfreev (filenames);

  /* connect to the session manager */
  session_client = thunar_session_client_new (opt_sm_client_id);

  /* do not enter the main loop, unless we have atleast one window or we are in daemon mode */
  if (thunar_application_has_windows (application) || thunar_application_get_daemon (application))
    {
      /* attach the D-BUS service */
#ifdef HAVE_DBUS
      dbus_service = g_object_new (THUNAR_TYPE_DBUS_SERVICE, NULL);
#endif

      /* enter the main loop */
      gtk_main ();

      /* detach the D-BUS service */
#ifdef HAVE_DBUS
      g_object_unref (G_OBJECT (dbus_service));
#endif
    }

  /* disconnect from the session manager */
  g_object_unref (G_OBJECT (session_client));

  /* release the application reference */
  g_object_unref (G_OBJECT (application));

  /* shutdown the VFS library */
  thunar_vfs_shutdown ();

  return EXIT_SUCCESS;
}
