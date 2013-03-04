/* $Id: thunar-dialogs.c 26421 2007-12-02 13:46:28Z benny $ */
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
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <thunar/thunar-dialogs.h>
#include <thunar/thunar-icon-factory.h>
#include <thunar/thunar-pango-extensions.h>
#include <thunar/thunar-preferences.h>
#include <thunar/thunar-private.h>
#include <thunar/thunar-util.h>



/**
 * thunar_dialogs_show_about:
 * @parent : the parent #GtkWindow or %NULL.
 * @title  : the software title.
 * @format : the printf()-style format for the main text in the about dialog.
 * @...    : argument list for the @format.
 *
 * Displays the Thunar about dialog with @format as main text.
 **/
void
thunar_dialogs_show_about (GtkWindow   *parent,
                           const gchar *title,
                           const gchar *format,
                           ...)
{
  static const gchar *artists[] =
  {
    "Young Hahn <youngjin.hahn@gmail.com>",
    NULL,
  };

  static const gchar *authors[] =
  {
    "Benedikt Meurer <benny@xfce.org>",
    NULL,
  };
  
  static const gchar *documenters[] =
  {
    "Benedikt Meurer <benny@xfce.org>",
    NULL,
  };

  GdkPixbuf *logo;
  va_list    args;
  gchar     *comments;

  _thunar_return_if_fail (parent == NULL || GTK_IS_WINDOW (parent));

  /* determine the comments */
  va_start (args, format);
  comments = g_strdup_vprintf (format, args);
  va_end (args);

  
  /* try to load the about logo */
  logo = gdk_pixbuf_new_from_file (DATADIR "/pixmaps/Thunar/Thunar-about-logo.png", NULL);

  /* open the about dialog */
  gtk_about_dialog_set_email_hook (exo_url_about_dialog_hook, NULL, NULL);
  gtk_about_dialog_set_url_hook (exo_url_about_dialog_hook, NULL, NULL);
  gtk_show_about_dialog (parent,
                         "artists", artists,
                         "authors", authors,
                         "comments", comments,
                         "copyright", "Copyright \302\251 2004-2007 Benedikt Meurer",
                         "destroy-with-parent", TRUE,
                         "documenters", documenters,
                         "license", XFCE_LICENSE_GPL,
                         "logo", logo,
#if GTK_CHECK_VERSION(2,11,0)
                         "program-name", title,
#else
                         "name", title,
#endif
                         "translator-credits", _("translator-credits"),
                         "version", PACKAGE_VERSION,
                         "website", "http://thunar.xfce.org/",
                         NULL);

  /* cleanup */
  if (G_LIKELY (logo != NULL))
    g_object_unref (G_OBJECT (logo));
  g_free (comments);
}



/**
 * thunar_dialogs_show_error:
 * @parent : a #GtkWidget on which the error dialog should be shown, or a #GdkScreen
 *           if no #GtkWidget is known. May also be %NULL, in which case the default
 *           #GdkScreen will be used.
 * @error  : a #GError, which gives a more precise description of the problem or %NULL.
 * @format : the printf()-style format for the primary problem description.
 * @...    : argument list for the @format.
 *
 * Displays an error dialog on @widget using the @format as primary message and optionally
 * displaying @error as secondary error text.
 *
 * If @widget is not %NULL and @widget is part of a #GtkWindow, the function makes sure
 * that the toplevel window is visible prior to displaying the error dialog.
 **/
void
thunar_dialogs_show_error (gpointer      parent,
                           const GError *error,
                           const gchar  *format,
                           ...)
{
  GtkWidget *dialog;
  GtkWindow *window;
  GdkScreen *screen;
  va_list    args;
  gchar     *primary_text;

  _thunar_return_if_fail (parent == NULL || GDK_IS_SCREEN (parent) || GTK_IS_WIDGET (parent));

  /* parse the parent pointer */
  screen = thunar_util_parse_parent (parent, &window);

  /* determine the primary error text */
  va_start (args, format);
  primary_text = g_strdup_vprintf (format, args);
  va_end (args);

  /* allocate the error dialog */
  dialog = gtk_message_dialog_new (window,
                                   GTK_DIALOG_DESTROY_WITH_PARENT
                                   | GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE,
                                   "%s.", primary_text);

  /* move the dialog to the appropriate screen */
  if (G_UNLIKELY (window == NULL && screen != NULL))
    gtk_window_set_screen (GTK_WINDOW (dialog), screen);

  /* set secondary text if an error is provided */
  if (G_LIKELY (error != NULL))
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", error->message);

  /* display the dialog */
  gtk_dialog_run (GTK_DIALOG (dialog));

  /* cleanup */
  gtk_widget_destroy (dialog);
  g_free (primary_text);
}



/**
 * thunar_dialogs_show_help:
 * @parent : a #GtkWidget on which the user manual should be shown, or a #GdkScreen
 *           if no #GtkWidget is known. May also be %NULL, in which case the default
 *           #GdkScreen will be used.
 * @page   : the name of the page of the user manual to display or %NULL to display
 *           the overview page.
 * @offset : the offset of the topic in the @page to display or %NULL to just display
 *           @page.
 *
 * Displays the Thunar user manual. If @page is not %NULL it specifies the basename
 * of the HTML file to display. @offset may refer to a anchor in the @page.
 **/
void
thunar_dialogs_show_help (gpointer     parent,
                          const gchar *page,
                          const gchar *offset)
{
  GdkScreen *screen;
  GError    *error = NULL;
  gchar     *command;
  gchar     *tmp;

  /* determine the screen on which to launch the help */
  screen = thunar_util_parse_parent (parent, NULL);

  /* generate the command for the documentation browser */
  command = g_strdup (LIBEXECDIR "/ThunarHelp");

  /* check if a page is given */
  if (G_UNLIKELY (page != NULL))
    {
      /* append page as second parameter */
      tmp = g_strconcat (command, " ", page, NULL);
      g_free (command);
      command = tmp;

      /* check if an offset is given */
      if (G_UNLIKELY (offset != NULL))
        {
          /* append offset as third parameter */
          tmp = g_strconcat (command, " ", offset, NULL);
          g_free (command);
          command = tmp;
        }
    }

  /* try to run the documentation browser */
  if (!gdk_spawn_command_line_on_screen (screen, command, &error))
    {
      /* display an error message to the user */
      thunar_dialogs_show_error (parent, error, _("Failed to open the documentation browser"));
      g_error_free (error);
    }

  /* cleanup */
  g_free (command);
}



/**
 * thunar_dialogs_show_job_ask:
 * @parent   : the parent #GtkWindow or %NULL.
 * @question : the question text.
 * @choices  : possible responses.
 *
 * Utility function to display a question dialog for the ThunarVfsJob::ask
 * signal.
 *
 * Return value: the #ThunarVfsJobResponse.
 **/
ThunarVfsJobResponse
thunar_dialogs_show_job_ask (GtkWindow           *parent,
                             const gchar         *question,
                             ThunarVfsJobResponse choices)
{
  const gchar *separator;
  const gchar *mnemonic;
  GtkWidget   *message;
  GtkWidget   *button;
  GString     *secondary = g_string_sized_new (256);
  GString     *primary = g_string_sized_new (256);
  gint         response;
  gint         n, m;

  _thunar_return_val_if_fail (parent == NULL || GTK_IS_WINDOW (parent), THUNAR_VFS_JOB_RESPONSE_CANCEL);
  _thunar_return_val_if_fail (g_utf8_validate (question, -1, NULL), THUNAR_VFS_JOB_RESPONSE_CANCEL);

  /* try to separate the question into primary and secondary parts */
  separator = strstr (question, ": ");
  if (G_LIKELY (separator != NULL))
    {
      /* primary is everything before the colon, plus a dot */
      g_string_append_len (primary, question, separator - question);
      g_string_append_c (primary, '.');

      /* secondary is everything after the colon (skipping whitespace) */
      do
        ++separator;
      while (g_ascii_isspace (*separator));
      g_string_append (secondary, separator);
    }
  else
    {
      /* otherwise separate based on the \n\n */
      separator = strstr (question, "\n\n");
      if (G_LIKELY (separator != NULL))
        {
          /* primary is everything before the newlines */
          g_string_append_len (primary, question, separator - question);

          /* secondary is everything after the newlines (skipping whitespace) */
          while (g_ascii_isspace (*separator))
            ++separator;
          g_string_append (secondary, separator);
        }
      else
        {
          /* everything is primary */
          g_string_append (primary, question);
        }
    }

  /* allocate the question message dialog */
  message = gtk_message_dialog_new (parent,
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_NONE,
                                    "%s", primary->str);
  if (G_LIKELY (*secondary->str != '\0'))
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s", secondary->str);

  /* add the buttons based on the possible choices */
  for (n = 5; n >= 0; --n)
    {
      /* "Cancel" should be the last button, but "Retry" was added last */
      m = (n == 5) ? 3 : (n == 3) ? 5 : n;

      /* check if the response is set */
      response = choices & (1 << m);
      if (response == 0)
        continue;

      switch (response)
        {
        case THUNAR_VFS_JOB_RESPONSE_YES:
          mnemonic = _("_Yes");
          break;

        case THUNAR_VFS_JOB_RESPONSE_YES_ALL:
          mnemonic = _("Yes to _all");
          break;

        case THUNAR_VFS_JOB_RESPONSE_NO:
          mnemonic = _("_No");
          break;

        case THUNAR_VFS_JOB_RESPONSE_NO_ALL:
          mnemonic = _("N_o to all");
          break;

        case THUNAR_VFS_JOB_RESPONSE_RETRY:
          mnemonic = _("_Retry");
          break;

        case THUNAR_VFS_JOB_RESPONSE_CANCEL:
          response = GTK_RESPONSE_CANCEL;
          mnemonic = _("_Cancel");
          break;

        default:
          g_assert_not_reached ();
          break;
        }

      button = gtk_button_new_with_mnemonic (mnemonic);
      GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
      gtk_dialog_add_action_widget (GTK_DIALOG (message), button, response);
      gtk_widget_show (button);

      gtk_dialog_set_default_response (GTK_DIALOG (message), response);
    }

  /* run the question dialog */
  response = gtk_dialog_run (GTK_DIALOG (message));
  gtk_widget_destroy (message);

  /* transform the result as required */
  if (G_UNLIKELY (response <= 0))
    response = THUNAR_VFS_JOB_RESPONSE_CANCEL;

  /* cleanup */
  g_string_free (secondary, TRUE);
  g_string_free (primary, TRUE);

  return response;
}



/**
 * thunar_dialogs_show_job_ask_replace:
 * @parent   : the parent #GtkWindow or %NULL.
 * @src_info : the #ThunarVfsInfo of the source file.
 * @dst_info : the #ThunarVfsInfo of the destination file that
 *             may be replaced with the source file.
 *
 * Asks the user whether to replace the destination file with the
 * source file identified by @src_info.
 *
 * Return value: the selected #ThunarVfsJobResponse.
 **/
ThunarVfsJobResponse
thunar_dialogs_show_job_ask_replace (GtkWindow     *parent,
                                     ThunarVfsInfo *src_info,
                                     ThunarVfsInfo *dst_info)
{
  ThunarIconFactory *icon_factory;
  ThunarPreferences *preferences;
  ThunarDateStyle    date_style;
  GtkIconTheme      *icon_theme;
  ThunarFile        *src_file;
  ThunarFile        *dst_file;
  GtkWidget         *dialog;
  GtkWidget         *table;
  GtkWidget         *image;
  GtkWidget         *label;
  GdkPixbuf         *icon;
  gchar             *date_string;
  gchar             *size_string;
  gchar             *text;
  gint               response;

  _thunar_return_val_if_fail (parent == NULL || GTK_IS_WINDOW (parent), THUNAR_VFS_JOB_RESPONSE_CANCEL);
  _thunar_return_val_if_fail (src_info != NULL, THUNAR_VFS_JOB_RESPONSE_CANCEL);
  _thunar_return_val_if_fail (dst_info != NULL, THUNAR_VFS_JOB_RESPONSE_CANCEL);

  /* determine the style used to format dates */
  preferences = thunar_preferences_get ();
  g_object_get (G_OBJECT (preferences), "misc-date-style", &date_style, NULL);
  g_object_unref (G_OBJECT (preferences));

  /* determine the files for the infos */
  src_file = thunar_file_get_for_info (src_info);
  dst_file = thunar_file_get_for_info (dst_info);

  /* setup the confirmation dialog */
  dialog = gtk_dialog_new_with_buttons (_("Confirm to replace files"),
                                        parent,
                                        GTK_DIALOG_MODAL
                                        | GTK_DIALOG_NO_SEPARATOR
                                        | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        _("_Skip"), THUNAR_VFS_JOB_RESPONSE_NO,
                                        _("Replace _All"), THUNAR_VFS_JOB_RESPONSE_YES_ALL,
                                        _("_Replace"), THUNAR_VFS_JOB_RESPONSE_YES,
                                        NULL);
  gtk_dialog_set_alternative_button_order (GTK_DIALOG (dialog),
                                           THUNAR_VFS_JOB_RESPONSE_YES,
                                           THUNAR_VFS_JOB_RESPONSE_YES_ALL,
                                           THUNAR_VFS_JOB_RESPONSE_NO,
                                           GTK_RESPONSE_CANCEL,
                                           -1);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), THUNAR_VFS_JOB_RESPONSE_YES);

  /* determine the icon factory to use */
  icon_theme = gtk_icon_theme_get_for_screen (gtk_widget_get_screen (dialog));
  icon_factory = thunar_icon_factory_get_for_icon_theme (icon_theme);

  table = g_object_new (GTK_TYPE_TABLE,
                        "border-width", 10,
                        "n-columns", 3,
                        "n-rows", 5,
                        "row-spacing", 6,
                        "column-spacing", 5,
                        NULL);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), table, TRUE, TRUE, 0);
  gtk_widget_show (table);

  image = gtk_image_new_from_icon_name ("stock_folder-copy", GTK_ICON_SIZE_BUTTON);
  gtk_misc_set_alignment (GTK_MISC (image), 0.5f, 0.0f);
  gtk_misc_set_padding (GTK_MISC (image), 6, 6);
  gtk_table_attach (GTK_TABLE (table), image, 0, 1, 0, 1, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_show (image);

  text = g_strdup_printf (_("This folder already contains a file \"%s\"."), thunar_file_get_display_name (dst_file));
  label = gtk_label_new (text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_big ());
  gtk_table_attach (GTK_TABLE (table), label, 1, 3, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);
  g_free (text);

  text = g_strdup_printf (Q_("ReplaceDialogPart1|Do you want to replace the existing file"));
  label = gtk_label_new (text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 3, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);
  g_free (text);

  icon = thunar_icon_factory_load_file_icon (icon_factory, dst_file, THUNAR_FILE_ICON_STATE_DEFAULT, 48);
  image = gtk_image_new_from_pixbuf (icon);
  gtk_misc_set_padding (GTK_MISC (image), 6, 6);
  gtk_table_attach (GTK_TABLE (table), image, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  g_object_unref (G_OBJECT (icon));
  gtk_widget_show (image);

  size_string = thunar_file_get_size_string (dst_file);
  date_string = thunar_file_get_date_string (dst_file, THUNAR_FILE_DATE_MODIFIED, date_style);
  text = g_strdup_printf ("%s %s\n%s %s", _("Size:"), size_string, _("Modified:"), date_string);
  label = gtk_label_new (text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, 2, 3, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);
  g_free (size_string);
  g_free (date_string);
  g_free (text);

  text = g_strdup_printf (Q_("ReplaceDialogPart2|with the following file?"));
  label = gtk_label_new (text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 3, 3, 4, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);
  g_free (text);

  icon = thunar_icon_factory_load_file_icon (icon_factory, src_file, THUNAR_FILE_ICON_STATE_DEFAULT, 48);
  image = gtk_image_new_from_pixbuf (icon);
  gtk_misc_set_padding (GTK_MISC (image), 6, 6);
  gtk_table_attach (GTK_TABLE (table), image, 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
  g_object_unref (G_OBJECT (icon));
  gtk_widget_show (image);

  size_string = thunar_file_get_size_string (src_file);
  date_string = thunar_file_get_date_string (src_file, THUNAR_FILE_DATE_MODIFIED, date_style);
  text = g_strdup_printf ("%s %s\n%s %s", _("Size:"), size_string, _("Modified:"), date_string);
  label = gtk_label_new (text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, 4, 5, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);
  g_free (size_string);
  g_free (date_string);
  g_free (text);

  /* run the dialog */
  response = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

  /* cleanup */
  g_object_unref (G_OBJECT (icon_factory));
  g_object_unref (G_OBJECT (dst_file));
  g_object_unref (G_OBJECT (src_file));

  /* translate GTK responses */
  if (G_UNLIKELY (response < 0))
    response = THUNAR_VFS_JOB_RESPONSE_CANCEL;

  return response;
}



/**
 * thunar_dialogs_show_job_error:
 * @parent : the parent #GtkWindow or %NULL.
 * @error  : the #GError provided by the #ThunarVfsJob.
 *
 * Utility function to display a message dialog for the
 * ThunarVfsJob::error signal.
 **/
void
thunar_dialogs_show_job_error (GtkWindow *parent,
                               GError    *error)
{
  const gchar *separator;
  GtkWidget   *message;
  GString     *secondary = g_string_sized_new (256);
  GString     *primary = g_string_sized_new (256);

  _thunar_return_if_fail (parent == NULL || GTK_IS_WINDOW (parent));
  _thunar_return_if_fail (error != NULL && error->message != NULL);

  /* try to separate the message into primary and secondary parts */
  separator = strstr (error->message, ": ");
  if (G_LIKELY (separator > error->message))
    {
      /* primary is everything before the colon, plus a dot */
      g_string_append_len (primary, error->message, separator - error->message);
      g_string_append_c (primary, '.');

      /* secondary is everything after the colon (plus a dot) */
      do
        ++separator;
      while (g_ascii_isspace (*separator));
      g_string_append (secondary, separator);
      if (separator[strlen (separator - 1)] != '.')
        g_string_append_c (secondary, '.');
    }
  else
    {
      /* primary is everything, secondary is empty */
      g_string_append (primary, error->message);
    }

  /* allocate and display the error message dialog */
  message = gtk_message_dialog_new (parent,
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_NONE,
                                    "%s", primary->str);
  if (G_LIKELY (*secondary->str != '\0'))
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s", secondary->str);
  gtk_dialog_add_button (GTK_DIALOG (message), GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL);
  gtk_dialog_run (GTK_DIALOG (message));
  gtk_widget_destroy (message);

  /* cleanup */
  g_string_free (secondary, TRUE);
  g_string_free (primary, TRUE);
}

