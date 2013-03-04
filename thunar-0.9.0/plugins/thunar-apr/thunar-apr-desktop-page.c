/* $Id: thunar-apr-desktop-page.c 26421 2007-12-02 13:46:28Z benny $ */
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

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <exo/exo.h>

#include <thunar-apr/thunar-apr-desktop-page.h>

/* use g_access() on win32 */
#if GLIB_CHECK_VERSION(2,8,0) && defined(G_OS_WIN32)
#include <glib/gstdio.h>
#else
#define g_access(filename, mode) (access ((filename), (mode)))
#endif



static void     thunar_apr_desktop_page_class_init      (ThunarAprDesktopPageClass  *klass);
static void     thunar_apr_desktop_page_init            (ThunarAprDesktopPage       *desktop_page);
static void     thunar_apr_desktop_page_finalize        (GObject                    *object);
static void     thunar_apr_desktop_page_file_changed    (ThunarAprAbstractPage      *abstract_page,
                                                         ThunarxFileInfo            *file);
static void     thunar_apr_desktop_page_save            (ThunarAprDesktopPage       *desktop_page,
                                                         GtkWidget                  *widget);
static void     thunar_apr_desktop_page_save_widget     (ThunarAprDesktopPage       *desktop_page,
                                                         GtkWidget                  *widget,
                                                         GKeyFile                   *key_file);
static void     thunar_apr_desktop_page_activated       (GtkWidget                  *entry,
                                                         ThunarAprDesktopPage       *desktop_page);
static gboolean thunar_apr_desktop_page_focus_out_event (GtkWidget                  *entry,
                                                         GdkEventFocus              *event,
                                                         ThunarAprDesktopPage       *desktop_page);
static void     thunar_apr_desktop_page_toggled         (GtkWidget                  *button,
                                                         ThunarAprDesktopPage       *desktop_page);



struct _ThunarAprDesktopPageClass
{
  ThunarAprAbstractPageClass __parent__;
};

struct _ThunarAprDesktopPage
{
  ThunarAprAbstractPage __parent__;
  
  GtkTooltips          *tooltips;

  GtkWidget            *description_entry;
  GtkWidget            *command_entry;
  GtkWidget            *url_entry;
  GtkWidget            *comment_entry;
  GtkWidget            *snotify_button;
  GtkWidget            *terminal_button;

  /* the values of the entries remember when
   * the file was saved last time to avoid a
   * race condition between saving the file
   * and the FAM notification which is received
   * with some delay and would otherwise over-
   * ride the (possibly changed) values of the
   * entries.
   */
  gchar                *description_text;
  gchar                *command_text;
  gchar                *url_text;
  gchar                *comment_text;
};



THUNARX_DEFINE_TYPE (ThunarAprDesktopPage,
                     thunar_apr_desktop_page,
                     THUNAR_APR_TYPE_ABSTRACT_PAGE);



static void
thunar_apr_desktop_page_class_init (ThunarAprDesktopPageClass *klass)
{
  ThunarAprAbstractPageClass *thunarapr_abstract_page_class;
  GObjectClass               *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_apr_desktop_page_finalize;

  thunarapr_abstract_page_class = THUNAR_APR_ABSTRACT_PAGE_CLASS (klass);
  thunarapr_abstract_page_class->file_changed = thunar_apr_desktop_page_file_changed;
}




static void
thunar_apr_desktop_page_init (ThunarAprDesktopPage *desktop_page)
{
  AtkRelationSet *relations;
  PangoAttribute *attribute;
  PangoAttrList  *attr_list;
  AtkRelation    *relation;
  AtkObject      *object;
  GtkWidget      *table;
  GtkWidget      *label;

  gtk_container_set_border_width (GTK_CONTAINER (desktop_page), 12);

  /* allocate the shared tooltips */
  desktop_page->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (desktop_page->tooltips));

  /* allocate shared bold Pango attributes */
  attr_list = pango_attr_list_new ();
  attribute = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
  attribute->start_index = 0;
  attribute->end_index = -1;
  pango_attr_list_insert (attr_list, attribute);

  table = gtk_table_new (7, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table), 0);
  gtk_container_add (GTK_CONTAINER (desktop_page), table);
  gtk_widget_show (table);

  label = gtk_label_new (_("Description:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), attr_list);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  desktop_page->description_entry = gtk_entry_new ();
  gtk_tooltips_set_tip (desktop_page->tooltips, desktop_page->description_entry, _("The generic name of the entry, for example \"Web Browser\" "
                                                                                   "in case of Firefox."), NULL);
  g_signal_connect (G_OBJECT (desktop_page->description_entry), "activate", G_CALLBACK (thunar_apr_desktop_page_activated), desktop_page);
  g_signal_connect (G_OBJECT (desktop_page->description_entry), "focus-out-event", G_CALLBACK (thunar_apr_desktop_page_focus_out_event), desktop_page);
  gtk_table_attach (GTK_TABLE (table), desktop_page->description_entry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (desktop_page->description_entry);

  exo_binding_new (G_OBJECT (desktop_page->description_entry), "visible", G_OBJECT (label), "visible");

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (desktop_page->description_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  label = gtk_label_new (_("Command:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), attr_list);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  desktop_page->command_entry = gtk_entry_new ();
  gtk_tooltips_set_tip (desktop_page->tooltips, desktop_page->command_entry, _("The program to execute, possibly with arguments."), NULL);
  g_signal_connect (G_OBJECT (desktop_page->command_entry), "activate", G_CALLBACK (thunar_apr_desktop_page_activated), desktop_page);
  g_signal_connect (G_OBJECT (desktop_page->command_entry), "focus-out-event", G_CALLBACK (thunar_apr_desktop_page_focus_out_event), desktop_page);
  gtk_table_attach (GTK_TABLE (table), desktop_page->command_entry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (desktop_page->command_entry);

  exo_binding_new (G_OBJECT (desktop_page->command_entry), "visible", G_OBJECT (label), "visible");

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (desktop_page->command_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  label = gtk_label_new (_("URL:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), attr_list);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  desktop_page->url_entry = gtk_entry_new ();
  gtk_tooltips_set_tip (desktop_page->tooltips, desktop_page->url_entry, _("The URL to access."), NULL);
  g_signal_connect (G_OBJECT (desktop_page->url_entry), "activate", G_CALLBACK (thunar_apr_desktop_page_activated), desktop_page);
  g_signal_connect (G_OBJECT (desktop_page->url_entry), "focus-out-event", G_CALLBACK (thunar_apr_desktop_page_focus_out_event), desktop_page);
  gtk_table_attach (GTK_TABLE (table), desktop_page->url_entry, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (desktop_page->url_entry);

  exo_binding_new (G_OBJECT (desktop_page->url_entry), "visible", G_OBJECT (label), "visible");

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (desktop_page->url_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  label = gtk_label_new (_("Comment:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), attr_list);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  desktop_page->comment_entry = gtk_entry_new ();
  gtk_tooltips_set_tip (desktop_page->tooltips, desktop_page->comment_entry, _("Tooltip for the entry, for example \"View sites on the Internet\" "
                                                                               "in case of Firefox. Should not be redundant with the name or the "
                                                                               "description."), NULL);
  g_signal_connect (G_OBJECT (desktop_page->comment_entry), "activate", G_CALLBACK (thunar_apr_desktop_page_activated), desktop_page);
  g_signal_connect (G_OBJECT (desktop_page->comment_entry), "focus-out-event", G_CALLBACK (thunar_apr_desktop_page_focus_out_event), desktop_page);
  gtk_table_attach (GTK_TABLE (table), desktop_page->comment_entry, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (desktop_page->comment_entry);

  exo_binding_new (G_OBJECT (desktop_page->comment_entry), "visible", G_OBJECT (label), "visible");

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (desktop_page->comment_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  /* add spacing between the entries and the options */
  gtk_table_set_row_spacing (GTK_TABLE (table), 4, 24);

  label = gtk_label_new (_("Options:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), attr_list);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 5, 6, GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (label);

  desktop_page->snotify_button = gtk_check_button_new_with_mnemonic (_("Use _startup notification"));
  gtk_tooltips_set_tip (desktop_page->tooltips, desktop_page->snotify_button, _("Select this option to enable startup notification when the command "
                                                                                "is run from the file manager or the menu. Not every application supports "
                                                                                "startup notification."), NULL);
  g_signal_connect (G_OBJECT (desktop_page->snotify_button), "toggled", G_CALLBACK (thunar_apr_desktop_page_toggled), desktop_page);
  gtk_table_attach (GTK_TABLE (table), desktop_page->snotify_button, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (desktop_page->snotify_button);

  desktop_page->terminal_button = gtk_check_button_new_with_mnemonic (_("Run in _terminal"));
  gtk_tooltips_set_tip (desktop_page->tooltips, desktop_page->terminal_button, _("Select this option to run the command in a terminal window."), NULL);
  g_signal_connect (G_OBJECT (desktop_page->terminal_button), "toggled", G_CALLBACK (thunar_apr_desktop_page_toggled), desktop_page);
  gtk_table_attach (GTK_TABLE (table), desktop_page->terminal_button, 1, 2, 6, 7, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 3);
  gtk_widget_show (desktop_page->terminal_button);

  /* set Atk label relation for the buttons */
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  object = gtk_widget_get_accessible (desktop_page->snotify_button);
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));
  object = gtk_widget_get_accessible (desktop_page->terminal_button);
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  exo_binding_new (G_OBJECT (desktop_page->snotify_button), "visible", G_OBJECT (label), "visible");

  /* release shared bold Pango attributes */
  pango_attr_list_unref (attr_list);
}



static void
thunar_apr_desktop_page_finalize (GObject *object)
{
  ThunarAprDesktopPage *desktop_page = THUNAR_APR_DESKTOP_PAGE (object);

  /* release the shared tooltips */
  g_object_unref (G_OBJECT (desktop_page->tooltips));

  /* release the saved texts */
  g_free (desktop_page->description_text);
  g_free (desktop_page->command_text);
  g_free (desktop_page->url_text);
  g_free (desktop_page->comment_text);

  (*G_OBJECT_CLASS (thunar_apr_desktop_page_parent_class)->finalize) (object);
}



static void
thunar_apr_desktop_page_file_changed (ThunarAprAbstractPage *abstract_page,
                                      ThunarxFileInfo       *file)
{
  ThunarAprDesktopPage *desktop_page = THUNAR_APR_DESKTOP_PAGE (abstract_page);
  GKeyFile             *key_file;
  gboolean              writable;
  gboolean              enabled;
  GError               *error = NULL;
  gchar                *filename;
  gchar                *value;
  gchar                *type;
  gchar                *uri;

  /* allocate the key file memory */
  key_file = g_key_file_new ();

  /* determine the local path to the file */
  uri = thunarx_file_info_get_uri (file);
  filename = g_filename_from_uri (uri, NULL, NULL);
  g_free (uri);

  /* try to load the file contents */
  if (filename != NULL && g_key_file_load_from_file (key_file, filename, G_KEY_FILE_NONE, NULL))
    {
      /* determine the type of the .desktop file (default to "Application") */
      type = g_key_file_get_string (key_file, "Desktop Entry", "Type", NULL);
      if (G_UNLIKELY (type == NULL))
        type = g_strdup ("Application");

      /* change page title depending on the type */
      if (strcmp (type, "Application") == 0)
        thunarx_property_page_set_label (THUNARX_PROPERTY_PAGE (desktop_page), _("Launcher"));
      else if (strcmp (type, "Link") == 0)
        thunarx_property_page_set_label (THUNARX_PROPERTY_PAGE (desktop_page), _("Link"));
      else
        thunarx_property_page_set_label (THUNARX_PROPERTY_PAGE (desktop_page), type);

      /* update the "Description" entry */
      value = g_key_file_get_locale_string (key_file, "Desktop Entry", "GenericName", NULL, NULL);
      if (!exo_str_is_equal (value, desktop_page->description_text))
        {
          /* update the entry */
          gtk_entry_set_text (GTK_ENTRY (desktop_page->description_entry), (value != NULL) ? value : "");

          /* update the saved value */
          g_free (desktop_page->description_text);
          desktop_page->description_text = value;
        }
      else
        {
          g_free (value);
        }

      /* update the "Comment" entry */
      value = g_key_file_get_locale_string (key_file, "Desktop Entry", "Comment", NULL, NULL);
      if (!exo_str_is_equal (value, desktop_page->comment_text))
        {
          /* update the entry */
          gtk_entry_set_text (GTK_ENTRY (desktop_page->comment_entry), (value != NULL) ? value : "");

          /* update the saved value */
          g_free (desktop_page->comment_text);
          desktop_page->comment_text = value;
        }
      else
        {
          g_free (value);
        }

      /* update the type dependant entries */
      if (strcmp (type, "Application") == 0)
        {
          /* update the "Command" entry */
          value = g_key_file_get_string (key_file, "Desktop Entry", "Exec", NULL);
          if (!exo_str_is_equal (value, desktop_page->command_text))
            {
              /* update the entry */
              gtk_entry_set_text (GTK_ENTRY (desktop_page->command_entry), (value != NULL) ? value : "");

              /* update the saved value */
              g_free (desktop_page->command_text);
              desktop_page->command_text = value;
            }
          else
            {
              g_free (value);
            }

          /* update the "Use startup notification" button */
          enabled = g_key_file_get_boolean (key_file, "Desktop Entry", "StartupNotify", &error);
          g_signal_handlers_block_by_func (G_OBJECT (desktop_page->snotify_button), thunar_apr_desktop_page_toggled, desktop_page);
          gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (desktop_page->snotify_button), (error == NULL && enabled));
          g_signal_handlers_unblock_by_func (G_OBJECT (desktop_page->snotify_button), thunar_apr_desktop_page_toggled, desktop_page);
          g_clear_error (&error);

          /* update the "Run in terminal" button */
          enabled = g_key_file_get_boolean (key_file, "Desktop Entry", "Terminal", &error);
          g_signal_handlers_block_by_func (G_OBJECT (desktop_page->terminal_button), thunar_apr_desktop_page_toggled, desktop_page);
          gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (desktop_page->terminal_button), (error == NULL && enabled));
          g_signal_handlers_unblock_by_func (G_OBJECT (desktop_page->terminal_button), thunar_apr_desktop_page_toggled, desktop_page);
          g_clear_error (&error);

          /* update visibility of the specific widgets */
          gtk_widget_show (desktop_page->command_entry);
          gtk_widget_hide (desktop_page->url_entry);
          gtk_widget_show (desktop_page->snotify_button);
          gtk_widget_show (desktop_page->terminal_button);
        }
      else if (strcmp (type, "Link") == 0)
        {
          /* update the "URL" entry */
          value = g_key_file_get_string (key_file, "Desktop Entry", "URL", NULL);
          if (!exo_str_is_equal (value, desktop_page->url_text))
            {
              /* update the entry */
              gtk_entry_set_text (GTK_ENTRY (desktop_page->url_entry), (value != NULL) ? value : "");

              /* update the saved value */
              g_free (desktop_page->url_text);
              desktop_page->url_text = value;
            }
          else
            {
              g_free (value);
            }

          /* update visibility of the specific widgets */
          gtk_widget_hide (desktop_page->command_entry);
          gtk_widget_show (desktop_page->url_entry);
          gtk_widget_hide (desktop_page->snotify_button);
          gtk_widget_hide (desktop_page->terminal_button);
        }
      else
        {
          /* hide the specific widgets */
          gtk_widget_hide (desktop_page->command_entry);
          gtk_widget_hide (desktop_page->url_entry);
          gtk_widget_hide (desktop_page->snotify_button);
          gtk_widget_hide (desktop_page->terminal_button);
        }

      /* check if the file is writable... */
      writable = (g_access (filename, W_OK) == 0);

      /* ...and update the editability of the entries */
      gtk_entry_set_editable (GTK_ENTRY (desktop_page->description_entry), writable);
      gtk_entry_set_editable (GTK_ENTRY (desktop_page->command_entry), writable);
      gtk_entry_set_editable (GTK_ENTRY (desktop_page->url_entry), writable);
      gtk_entry_set_editable (GTK_ENTRY (desktop_page->comment_entry), writable);
      gtk_widget_set_sensitive (desktop_page->snotify_button, writable);
      gtk_widget_set_sensitive (desktop_page->terminal_button, writable);

      /* cleanup */
      g_free (type);
    }
  else
    {
      /* reset page title */
      thunarx_property_page_set_label (THUNARX_PROPERTY_PAGE (desktop_page), _("Unknown"));

      /* hide all widgets */
      gtk_widget_hide (desktop_page->description_entry);
      gtk_widget_hide (desktop_page->command_entry);
      gtk_widget_hide (desktop_page->url_entry);
      gtk_widget_hide (desktop_page->comment_entry);
      gtk_widget_hide (desktop_page->snotify_button);
      gtk_widget_hide (desktop_page->terminal_button);
    }

  /* cleanup */
  g_key_file_free (key_file);
  g_free (filename);
}



static void
thunar_apr_desktop_page_save (ThunarAprDesktopPage *desktop_page,
                              GtkWidget            *widget)
{
  GtkWidget *toplevel;
  GtkWidget *message;
  GKeyFile  *key_file;
  GError    *error = NULL;
  gchar     *filename;
  gchar     *data;
  gchar     *uri;
  gsize      data_length;
  FILE      *fp;

  /* verify that we still have a valid file */
  if (THUNAR_APR_ABSTRACT_PAGE (desktop_page)->file == NULL)
    return;

  /* determine the local path to the file */
  uri = thunarx_file_info_get_uri (THUNAR_APR_ABSTRACT_PAGE (desktop_page)->file);
  filename = g_filename_from_uri (uri, NULL, NULL);
  g_free (uri);

  /* verify that we have a valid local path */
  if (G_UNLIKELY (filename == NULL))
    return;

  /* allocate the key file resources */
  key_file = g_key_file_new ();

  /* try to parse the key file */
  if (g_key_file_load_from_file (key_file, filename, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error))
    {
      /* save the widget changes to the key file */
      thunar_apr_desktop_page_save_widget (desktop_page, widget, key_file);

      /* determine the content of the key file */
      data = g_key_file_to_data (key_file, &data_length, &error);
      if (G_LIKELY (data != NULL))
        {
          /* try to save the key file content to disk */
          fp = fopen (filename, "w");
          if (G_LIKELY (fp != NULL))
            {
              if (fwrite (data, data_length, 1, fp) != 1)
                error = g_error_new_literal (G_FILE_ERROR, g_file_error_from_errno (errno), g_strerror (errno));
              fclose (fp);
            }
          else
            {
              error = g_error_new_literal (G_FILE_ERROR, g_file_error_from_errno (errno), g_strerror (errno));
            }

          /* cleanup */
          g_free (data);
        }
    }

  /* check if we succeed */
  if (G_UNLIKELY (error != NULL))
    {
      /* display an error dialog to the user */
      toplevel = gtk_widget_get_toplevel (GTK_WIDGET (desktop_page));
      message = gtk_message_dialog_new ((GtkWindow *) toplevel,
                                        GTK_DIALOG_DESTROY_WITH_PARENT
                                        | GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("Failed to save \"%s\"."), filename);
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s.", error->message);
      gtk_dialog_run (GTK_DIALOG (message));
      gtk_widget_destroy (message);
      g_error_free (error);
    }

  /* cleanup */
  g_key_file_free (key_file);
  g_free (filename);
}



static void
thunar_apr_desktop_page_save_widget (ThunarAprDesktopPage *desktop_page,
                                     GtkWidget            *widget,
                                     GKeyFile             *key_file)
{
  const gchar * const *locale;
  gchar               *key;

  if (widget == desktop_page->description_entry)
    {
      /* update the saved description text */
      g_free (desktop_page->description_text);
      desktop_page->description_text = gtk_editable_get_chars (GTK_EDITABLE (widget), 0, -1);

      /* save the new description (localized if required) */
      for (locale = g_get_language_names (); *locale != NULL; ++locale)
        {
          key = g_strdup_printf ("GenericName[%s]", *locale);
          if (g_key_file_has_key (key_file, "Desktop Entry", key, NULL))
            {
              g_key_file_set_string (key_file, "Desktop Entry", key, desktop_page->description_text);
              g_free (key);
              break;
            }
          g_free (key);
        }

      /* fallback to unlocalized description */
      if (G_UNLIKELY (*locale == NULL))
        g_key_file_set_string (key_file, "Desktop Entry", "GenericName", desktop_page->description_text);
    }
  else if (widget == desktop_page->command_entry)
    {
      /* update the saved command */
      g_free (desktop_page->command_text);
      desktop_page->command_text = gtk_editable_get_chars (GTK_EDITABLE (widget), 0, -1);

      /* save the unlocalized command */
      g_key_file_set_string (key_file, "Desktop Entry", "Exec", desktop_page->command_text);
    }
  else if (widget == desktop_page->url_entry)
    {
      /* update the saved URL */
      g_free (desktop_page->url_text);
      desktop_page->url_text = gtk_editable_get_chars (GTK_EDITABLE (widget), 0, -1);

      /* save the unlocalized url */
      g_key_file_set_string (key_file, "Desktop Entry", "URL", desktop_page->url_text);
    }
  else if (widget == desktop_page->comment_entry)
    {
      /* update the saved comment text */
      g_free (desktop_page->comment_text);
      desktop_page->comment_text = gtk_editable_get_chars (GTK_EDITABLE (widget), 0, -1);

      /* save the new comment (localized if required) */
      for (locale = g_get_language_names (); *locale != NULL; ++locale)
        {
          key = g_strdup_printf ("Comment[%s]", *locale);
          if (g_key_file_has_key (key_file, "Desktop Entry", key, NULL))
            {
              g_key_file_set_string (key_file, "Desktop Entry", key, desktop_page->comment_text);
              g_free (key);
              break;
            }
          g_free (key);
        }

      /* fallback to unlocalized comment */
      if (G_UNLIKELY (*locale == NULL))
        g_key_file_set_string (key_file, "Desktop Entry", "Comment", desktop_page->comment_text);
    }
  else if (widget == desktop_page->snotify_button)
    {
      g_key_file_set_boolean (key_file, "Desktop Entry", "StartupNotify", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)));
    }
  else if (widget == desktop_page->terminal_button)
    {
      g_key_file_set_boolean (key_file, "Desktop Entry", "Terminal", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)));
    }
  else
    {
      g_assert_not_reached ();
    }
}



static void
thunar_apr_desktop_page_activated (GtkWidget            *entry,
                                   ThunarAprDesktopPage *desktop_page)
{
  g_return_if_fail (GTK_IS_ENTRY (entry));
  g_return_if_fail (THUNAR_APR_IS_DESKTOP_PAGE (desktop_page));

  /* check if the entry is editable, and if so, save the file */
  if (gtk_editable_get_editable (GTK_EDITABLE (entry)))
    thunar_apr_desktop_page_save (desktop_page, entry);
}



static gboolean
thunar_apr_desktop_page_focus_out_event (GtkWidget            *entry,
                                         GdkEventFocus        *event,
                                         ThunarAprDesktopPage *desktop_page)
{
  g_return_val_if_fail (GTK_IS_ENTRY (entry), FALSE);
  g_return_val_if_fail (THUNAR_APR_IS_DESKTOP_PAGE (desktop_page), FALSE);

  /* check if the entry is editable, and if so, save the file */
  if (gtk_editable_get_editable (GTK_EDITABLE (entry)))
    thunar_apr_desktop_page_save (desktop_page, entry);
  return FALSE;
}



static void
thunar_apr_desktop_page_toggled (GtkWidget            *button,
                                 ThunarAprDesktopPage *desktop_page)
{
  g_return_if_fail (GTK_IS_TOGGLE_BUTTON (button));
  g_return_if_fail (THUNAR_APR_IS_DESKTOP_PAGE (desktop_page));

  /* save the file */
  thunar_apr_desktop_page_save (desktop_page, button);
}

