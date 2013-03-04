/* $Id: thunar-permissions-chooser.c 26421 2007-12-02 13:46:28Z benny $ */
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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <thunar/thunar-dialogs.h>
#include <thunar/thunar-enum-types.h>
#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-gtk-extensions.h>
#include <thunar/thunar-pango-extensions.h>
#include <thunar/thunar-permissions-chooser.h>
#include <thunar/thunar-preferences.h>
#include <thunar/thunar-private.h>



/* Use native strlcpy() if available */
#if defined(HAVE_STRLCPY)
#define g_strlcpy(dst, src, size) (strlcpy ((dst), (src), (size)))
#endif



/* Property identifiers */
enum
{
  PROP_0,
  PROP_FILE,
  PROP_MUTABLE,
};

/* Column identifiers for the group combo box */
enum
{
  THUNAR_PERMISSIONS_STORE_COLUMN_NAME,
  THUNAR_PERMISSIONS_STORE_COLUMN_GID,
  THUNAR_PERMISSIONS_STORE_N_COLUMNS,
};



static void                 thunar_permissions_chooser_class_init       (ThunarPermissionsChooserClass  *klass);
static void                 thunar_permissions_chooser_init             (ThunarPermissionsChooser       *chooser);
static void                 thunar_permissions_chooser_finalize         (GObject                        *object);
static void                 thunar_permissions_chooser_get_property     (GObject                        *object,
                                                                         guint                           prop_id,
                                                                         GValue                         *value,
                                                                         GParamSpec                     *pspec);
static void                 thunar_permissions_chooser_set_property     (GObject                        *object,
                                                                         guint                           prop_id,
                                                                         const GValue                   *value,
                                                                         GParamSpec                     *pspec);
static gint                 thunar_permissions_chooser_ask_recursive    (ThunarPermissionsChooser       *chooser);
static void                 thunar_permissions_chooser_change_group     (ThunarPermissionsChooser       *chooser,
                                                                         ThunarVfsGroupId                gid);
static void                 thunar_permissions_chooser_change_mode      (ThunarPermissionsChooser       *chooser,
                                                                         ThunarVfsFileMode               dir_mask,
                                                                         ThunarVfsFileMode               dir_mode,
                                                                         ThunarVfsFileMode               file_mask,
                                                                         ThunarVfsFileMode               file_mode);
static void                 thunar_permissions_chooser_access_changed   (ThunarPermissionsChooser       *chooser,
                                                                         GtkWidget                      *combo);
static void                 thunar_permissions_chooser_file_changed     (ThunarPermissionsChooser       *chooser,
                                                                         ThunarFile                     *file);
static void                 thunar_permissions_chooser_group_changed    (ThunarPermissionsChooser       *chooser,
                                                                         GtkWidget                      *combo);
static void                 thunar_permissions_chooser_program_toggled  (ThunarPermissionsChooser       *chooser,
                                                                         GtkWidget                      *button);
static void                 thunar_permissions_chooser_fixperm_clicked  (ThunarPermissionsChooser       *chooser,
                                                                         GtkWidget                      *button);
static ThunarVfsJobResponse thunar_permissions_chooser_job_ask          (ThunarPermissionsChooser       *chooser,
                                                                         const gchar                    *message,
                                                                         ThunarVfsJobResponse            choices,
                                                                         ThunarVfsJob                   *job);
static void                 thunar_permissions_chooser_job_cancel       (ThunarPermissionsChooser       *chooser);
static void                 thunar_permissions_chooser_job_error        (ThunarPermissionsChooser       *chooser,
                                                                         GError                         *error,
                                                                         ThunarVfsJob                   *job);
static void                 thunar_permissions_chooser_job_finished     (ThunarPermissionsChooser       *chooser,
                                                                         ThunarVfsJob                   *job);
static void                 thunar_permissions_chooser_job_percent      (ThunarPermissionsChooser       *chooser,  
                                                                         gdouble                         percent,
                                                                         ThunarVfsJob                   *job);
static void                 thunar_permissions_chooser_job_start        (ThunarPermissionsChooser       *chooser,
                                                                         ThunarVfsJob                   *job,
                                                                         gboolean                        recursive);
static gboolean             thunar_permissions_chooser_row_separator    (GtkTreeModel                   *model,
                                                                         GtkTreeIter                    *iter,
                                                                         gpointer                        data);



struct _ThunarPermissionsChooserClass
{
  GtkVBoxClass __parent__;
};

struct _ThunarPermissionsChooser
{
  GtkVBox __parent__;

  ThunarFile   *file;

  /* the main table widget, which contains everything but the job control stuff */
  GtkWidget    *table;

  GtkWidget    *user_label;
  GtkWidget    *group_combo;
  GtkWidget    *access_combos[3];
  GtkWidget    *program_button;
  GtkWidget    *fixperm_label;
  GtkWidget    *fixperm_button;

  /* job control stuff */
  ThunarVfsJob *job;
  GtkWidget    *job_progress;
};



static GObjectClass *thunar_permissions_chooser_parent_class;



GType
thunar_permissions_chooser_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarPermissionsChooserClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_permissions_chooser_class_init,
        NULL,
        NULL,
        sizeof (ThunarPermissionsChooser),
        0,
        (GInstanceInitFunc) thunar_permissions_chooser_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_VBOX, I_("ThunarPermissionsChooser"), &info, 0);
    }

  return type;
}



static void
thunar_permissions_chooser_class_init (ThunarPermissionsChooserClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  thunar_permissions_chooser_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_permissions_chooser_finalize;
  gobject_class->get_property = thunar_permissions_chooser_get_property;
  gobject_class->set_property = thunar_permissions_chooser_set_property;

  /**
   * ThunarPermissionsChooser:file:
   *
   * The #ThunarFile whose permissions will be edited/viewed.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FILE,
                                   g_param_spec_object ("file", "file", "file",
                                                        THUNAR_TYPE_FILE,
                                                        EXO_PARAM_READWRITE));

  /**
   * ThunarPermissionsChooser:mutable:
   *
   * Whether the current #ThunarFile<!---->s permissions are
   * mutable.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_MUTABLE,
                                   g_param_spec_boolean ("mutable",
                                                         "mutable",
                                                         "mutable",
                                                         FALSE,
                                                         EXO_PARAM_READABLE));
}



static void
thunar_permissions_chooser_init (ThunarPermissionsChooser *chooser)
{
  GtkCellRenderer *renderer_text;
  GtkListStore    *store;
  GtkTreeIter      iter;
  GtkWidget       *separator;
  GtkWidget       *button;
  GtkWidget       *label;
  GtkWidget       *image;
  GtkWidget       *hbox;
  gint             row = 0;

  /* setup the chooser */
  gtk_container_set_border_width (GTK_CONTAINER (chooser), 12);

  /* allocate the store for the permission combos */
  store = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (store, &iter); /* 0000 */
  gtk_list_store_set (store, &iter, 0, _("None"), -1);
  gtk_list_store_append (store, &iter); /* 0002 */
  gtk_list_store_set (store, &iter, 0, _("Write only"), -1);
  gtk_list_store_append (store, &iter); /* 0004 */
  gtk_list_store_set (store, &iter, 0, _("Read only"), -1);
  gtk_list_store_append (store, &iter); /* 0006 */
  gtk_list_store_set (store, &iter, 0, _("Read & Write"), -1);

  /* allocate the shared renderer for the various combo boxes */
  renderer_text = gtk_cell_renderer_text_new ();

  chooser->table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (chooser->table), 12);
  gtk_table_set_row_spacings (GTK_TABLE (chooser->table), 6);
  gtk_box_pack_start (GTK_BOX (chooser), chooser->table, TRUE, TRUE, 0);
  gtk_widget_show (chooser->table);

  label = gtk_label_new (_("Owner:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_table_attach (GTK_TABLE (chooser->table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_table_attach (GTK_TABLE (chooser->table), hbox, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 6);
  gtk_widget_show (hbox);

  chooser->user_label = gtk_label_new (_("Unknown"));
  gtk_misc_set_alignment (GTK_MISC (chooser->user_label), 0.0f, 0.5f);
  gtk_box_pack_start (GTK_BOX (hbox), chooser->user_label, TRUE, TRUE, 0);
  thunar_gtk_label_set_a11y_relation (GTK_LABEL (label), chooser->user_label);
  gtk_widget_show (chooser->user_label);

  row += 1;

  label = gtk_label_new (_("Access:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_table_attach (GTK_TABLE (chooser->table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  chooser->access_combos[2] = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (chooser->access_combos[2]), renderer_text, TRUE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (chooser->access_combos[2]), renderer_text, "text", 0);
  exo_binding_new (G_OBJECT (chooser), "mutable", G_OBJECT (chooser->access_combos[2]), "sensitive");
  g_signal_connect_swapped (G_OBJECT (chooser->access_combos[2]), "changed", G_CALLBACK (thunar_permissions_chooser_access_changed), chooser);
  gtk_table_attach (GTK_TABLE (chooser->table), chooser->access_combos[2], 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  thunar_gtk_label_set_a11y_relation (GTK_LABEL (label), chooser->access_combos[2]);
  gtk_widget_show (chooser->access_combos[2]);

  row += 1;

  separator = gtk_alignment_new (0.0f, 0.0f, 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (chooser->table), separator, 0, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 6);
  gtk_widget_show (separator);

  row += 1;

  label = gtk_label_new (_("Group:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_table_attach (GTK_TABLE (chooser->table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  chooser->group_combo = gtk_combo_box_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (chooser->group_combo), renderer_text, TRUE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (chooser->group_combo), renderer_text, "text", THUNAR_PERMISSIONS_STORE_COLUMN_NAME);
  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (chooser->group_combo), thunar_permissions_chooser_row_separator, NULL, NULL);
  exo_binding_new (G_OBJECT (chooser), "mutable", G_OBJECT (chooser->group_combo), "sensitive");
  g_signal_connect_swapped (G_OBJECT (chooser->group_combo), "changed", G_CALLBACK (thunar_permissions_chooser_group_changed), chooser);
  gtk_table_attach (GTK_TABLE (chooser->table), chooser->group_combo, 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  thunar_gtk_label_set_a11y_relation (GTK_LABEL (label), chooser->group_combo);
  gtk_widget_show (chooser->group_combo);

  row += 1;

  label = gtk_label_new (_("Access:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_table_attach (GTK_TABLE (chooser->table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  chooser->access_combos[1] = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (chooser->access_combos[1]), renderer_text, TRUE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (chooser->access_combos[1]), renderer_text, "text", 0);
  exo_binding_new (G_OBJECT (chooser), "mutable", G_OBJECT (chooser->access_combos[1]), "sensitive");
  g_signal_connect_swapped (G_OBJECT (chooser->access_combos[1]), "changed", G_CALLBACK (thunar_permissions_chooser_access_changed), chooser);
  gtk_table_attach (GTK_TABLE (chooser->table), chooser->access_combos[1], 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  thunar_gtk_label_set_a11y_relation (GTK_LABEL (label), chooser->access_combos[1]);
  gtk_widget_show (chooser->access_combos[1]);

  row += 1;

  separator = gtk_alignment_new (0.0f, 0.0f, 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (chooser->table), separator, 0, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 6);
  gtk_widget_show (separator);

  row += 1;

  label = gtk_label_new (_("Others:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_table_attach (GTK_TABLE (chooser->table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  chooser->access_combos[0] = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (chooser->access_combos[0]), renderer_text, TRUE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (chooser->access_combos[0]), renderer_text, "text", 0);
  exo_binding_new (G_OBJECT (chooser), "mutable", G_OBJECT (chooser->access_combos[0]), "sensitive");
  g_signal_connect_swapped (G_OBJECT (chooser->access_combos[0]), "changed", G_CALLBACK (thunar_permissions_chooser_access_changed), chooser);
  gtk_table_attach (GTK_TABLE (chooser->table), chooser->access_combos[0], 1, 2, row, row + 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  thunar_gtk_label_set_a11y_relation (GTK_LABEL (label), chooser->access_combos[0]);
  gtk_widget_show (chooser->access_combos[0]);

  row += 1;

  separator = gtk_alignment_new (0.0f, 0.0f, 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (chooser->table), separator, 0, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 6);
  gtk_widget_show (separator);

  row += 1;

  label = gtk_label_new (_("Program:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_bold ());
  gtk_table_attach (GTK_TABLE (chooser->table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  chooser->program_button = gtk_check_button_new_with_mnemonic (_("Allow this file to _run as a program"));
  exo_binding_new (G_OBJECT (chooser->program_button), "visible", G_OBJECT (label), "visible");
  exo_binding_new (G_OBJECT (chooser), "mutable", G_OBJECT (chooser->program_button), "sensitive");
  g_signal_connect_swapped (G_OBJECT (chooser->program_button), "toggled", G_CALLBACK (thunar_permissions_chooser_program_toggled), chooser);
  gtk_table_attach (GTK_TABLE (chooser->table), chooser->program_button, 1, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  thunar_gtk_label_set_a11y_relation (GTK_LABEL (label), chooser->program_button);
  gtk_widget_show (chooser->program_button);

  row += 1;

  hbox = gtk_hbox_new (FALSE, 6);
  exo_binding_new (G_OBJECT (chooser), "mutable", G_OBJECT (hbox), "sensitive");
  exo_binding_new (G_OBJECT (chooser->program_button), "visible", G_OBJECT (hbox), "visible");
  gtk_table_attach (GTK_TABLE (chooser->table), hbox, 1, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_LARGE_TOOLBAR);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_widget_show (image);

  label = gtk_label_new (_("Allowing untrusted programs to run\npresents a security risk to your system."));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_small_italic ());
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 6);
  exo_binding_new (G_OBJECT (chooser), "mutable", G_OBJECT (hbox), "sensitive");
  gtk_table_attach (GTK_TABLE (chooser->table), hbox, 1, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_LARGE_TOOLBAR);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_widget_show (image);

  chooser->fixperm_label = gtk_label_new (_("The folder permissions are inconsistent, you\nmay not be able to work with files in this folder."));
  gtk_misc_set_alignment (GTK_MISC (chooser->fixperm_label), 0.0f, 0.5f);
  gtk_label_set_attributes (GTK_LABEL (chooser->fixperm_label), thunar_pango_attr_list_small_italic ());
  exo_binding_new (G_OBJECT (chooser->fixperm_label), "visible", G_OBJECT (hbox), "visible");
  gtk_box_pack_start (GTK_BOX (hbox), chooser->fixperm_label, TRUE, TRUE, 0);
  gtk_widget_show (chooser->fixperm_label);

  row += 1;

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_table_attach (GTK_TABLE (chooser->table), hbox, 1, 2, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  chooser->fixperm_button = gtk_button_new_with_mnemonic (_("Correct folder permissions..."));
  thunar_gtk_widget_set_tooltip (chooser->fixperm_button, _("Click here to automatically fix the folder permissions."));
  g_signal_connect_swapped (G_OBJECT (chooser->fixperm_button), "clicked", G_CALLBACK (thunar_permissions_chooser_fixperm_clicked), chooser);
  exo_binding_new (G_OBJECT (chooser->fixperm_button), "visible", G_OBJECT (hbox), "visible");
  gtk_box_pack_end (GTK_BOX (hbox), chooser->fixperm_button, FALSE, FALSE, 0);
  gtk_widget_show (chooser->fixperm_button);

  /* the job control stuff */
  hbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (chooser), hbox, FALSE, FALSE, 0);

  chooser->job_progress = gtk_progress_bar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (chooser->job_progress), _("Please wait..."));
  exo_binding_new (G_OBJECT (chooser->job_progress), "visible", G_OBJECT (hbox), "visible");
  gtk_box_pack_start (GTK_BOX (hbox), chooser->job_progress, TRUE, TRUE, 0);

  button = gtk_button_new ();
  thunar_gtk_widget_set_tooltip (button, _("Stop applying permissions recursively."));
  g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK (thunar_permissions_chooser_job_cancel), chooser);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  image = gtk_image_new_from_stock (GTK_STOCK_CANCEL, GTK_ICON_SIZE_MENU);
  gtk_container_add (GTK_CONTAINER (button), image);
  gtk_widget_show (image);

  /* release the shared combo store */
  g_object_unref (G_OBJECT (store));
}



static void
thunar_permissions_chooser_finalize (GObject *object)
{
  ThunarPermissionsChooser *chooser = THUNAR_PERMISSIONS_CHOOSER (object);

  /* cancel any pending job */
  if (G_UNLIKELY (chooser->job != NULL))
    {
      /* cancel the job (if not already done) */
      thunar_vfs_job_cancel (chooser->job);

      /* disconnect from the job */
      g_signal_handlers_disconnect_matched (chooser->job, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, chooser);
      g_object_unref (G_OBJECT (chooser->job));
      chooser->job = NULL;
    }

  /* drop the reference on the file (if any) */
  thunar_permissions_chooser_set_file (chooser, NULL);

  (*G_OBJECT_CLASS (thunar_permissions_chooser_parent_class)->finalize) (object);
}



static void
thunar_permissions_chooser_get_property (GObject    *object,
                                         guint       prop_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
  ThunarPermissionsChooser *chooser = THUNAR_PERMISSIONS_CHOOSER (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, thunar_permissions_chooser_get_file (chooser));
      break;

    case PROP_MUTABLE:
      g_value_set_boolean (value, (chooser->file != NULL) && thunar_file_is_chmodable (chooser->file));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_permissions_chooser_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  ThunarPermissionsChooser *chooser = THUNAR_PERMISSIONS_CHOOSER (object);

  switch (prop_id)
    {
    case PROP_FILE:
      thunar_permissions_chooser_set_file (chooser, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gboolean
thunar_permissions_chooser_ask_recursive (ThunarPermissionsChooser *chooser)
{
  ThunarRecursivePermissionsMode mode;
  ThunarPreferences             *preferences;
  GtkWidget                     *toplevel;
  GtkWidget                     *dialog;
  GtkWidget                     *button;
  GtkWidget                     *label;
  GtkWidget                     *image;
  GtkWidget                     *hbox;
  GtkWidget                     *vbox;
  gint                           response;

  /* grab a reference on the preferences */
  preferences = thunar_preferences_get ();

  /* determine the current recursive permissions mode */
  g_object_get (G_OBJECT (preferences), "misc-recursive-permissions", &mode, NULL);

  /* check if we should ask the user first */
  if (G_UNLIKELY (mode == THUNAR_RECURSIVE_PERMISSIONS_ASK))
    {
      /* determine the toplevel widget for the chooser */
      toplevel = gtk_widget_get_toplevel (GTK_WIDGET (chooser));

      /* allocate the question dialog */
      dialog = gtk_dialog_new_with_buttons (_("Question"), GTK_WINDOW (toplevel),
                                            GTK_DIALOG_DESTROY_WITH_PARENT
                                            | GTK_DIALOG_NO_SEPARATOR
                                            | GTK_DIALOG_MODAL,
                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                            GTK_STOCK_NO, GTK_RESPONSE_NO,
                                            GTK_STOCK_YES, GTK_RESPONSE_YES,
                                            NULL);
      gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);

      hbox = gtk_hbox_new (FALSE, 6);
      gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
      gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, TRUE, TRUE, 0);
      gtk_widget_show (hbox);

      image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
      gtk_misc_set_alignment (GTK_MISC (image), 0.5f, 0.0f);
      gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
      gtk_widget_show (image);

      vbox = gtk_vbox_new (FALSE, 6);
      gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
      gtk_widget_show (vbox);

      label = gtk_label_new (_("Apply recursively?"));
      gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
      gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_big_bold ());
      gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
      gtk_widget_show (label);

      label = gtk_label_new (_("Do you want to apply your changes recursively to\nall files and subfolders below the selected folder?"));
      gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
      gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
      gtk_widget_show (label);

      button = gtk_check_button_new_with_mnemonic (_("Do _not ask me again"));
      thunar_gtk_widget_set_tooltip (button, _("If you select this option your choice will be remembered and you won't be asked "
                                               "again. You can use the preferences dialog to alter your choice afterwards."));
      gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
      gtk_widget_show (button);

      /* run the dialog and save the selected option (if requested) */
      response = gtk_dialog_run (GTK_DIALOG (dialog));
      switch (response)
        {
        case GTK_RESPONSE_YES:
          if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
            g_object_set (G_OBJECT (preferences), "misc-recursive-permissions", THUNAR_RECURSIVE_PERMISSIONS_ALWAYS, NULL);
          break;

        case GTK_RESPONSE_NO:
          if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
            g_object_set (G_OBJECT (preferences), "misc-recursive-permissions", THUNAR_RECURSIVE_PERMISSIONS_NEVER, NULL);
          break;

        default:
          break;
        }

      /* destroy the dialog resources */
      gtk_widget_destroy (dialog);
    }
  else if (mode == THUNAR_RECURSIVE_PERMISSIONS_ALWAYS)
    {
      response = GTK_RESPONSE_YES;
    }
  else
    {
      response = GTK_RESPONSE_NO;
    }

  /* release the reference on the preferences */
  g_object_unref (G_OBJECT (preferences));

  return response;
}



static void
thunar_permissions_chooser_change_group (ThunarPermissionsChooser *chooser,
                                         ThunarVfsGroupId          gid)
{
  ThunarVfsJob *job;
  gboolean      recursive = FALSE;
  GError       *error = NULL;
  gint          response;

  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (THUNAR_IS_FILE (chooser->file));

  /* check if we should operate recursively */
  if (thunar_file_is_directory (chooser->file))
    {
      response = thunar_permissions_chooser_ask_recursive (chooser);
      switch (response)
        {
        case GTK_RESPONSE_YES:
          recursive = TRUE;
          break;

        case GTK_RESPONSE_NO:
          recursive = FALSE;
          break;

        default:  /* cancelled by the user */
          thunar_file_changed (chooser->file);
          return;
        }
    }

  /* try to allocate the new job */
  job = thunar_vfs_change_group (thunar_file_get_path (chooser->file), gid, recursive, &error);
  if (G_UNLIKELY (job == NULL))
    {
      /* display an error to the user */
      thunar_dialogs_show_error (GTK_WIDGET (chooser), error, _("Failed to change group"));
      g_error_free (error);
    }
  else
    {
      /* handle the job */
      thunar_permissions_chooser_job_start (chooser, job, recursive);
      g_object_unref (G_OBJECT (job));
    }
}



static void
thunar_permissions_chooser_change_mode (ThunarPermissionsChooser *chooser,
                                        ThunarVfsFileMode         dir_mask,
                                        ThunarVfsFileMode         dir_mode,
                                        ThunarVfsFileMode         file_mask,
                                        ThunarVfsFileMode         file_mode)
{
  ThunarVfsJob *job;
  gboolean      recursive = FALSE;
  GError       *error = NULL;
  gint          response;

  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (THUNAR_IS_FILE (chooser->file));

  /* check if we should operate recursively */
  if (thunar_file_is_directory (chooser->file))
    {
      response = thunar_permissions_chooser_ask_recursive (chooser);
      switch (response)
        {
        case GTK_RESPONSE_YES:
          recursive = TRUE;
          break;

        case GTK_RESPONSE_NO:
          recursive = FALSE;
          break;

        default:  /* cancelled by the user */
          thunar_file_changed (chooser->file);
          return;
        }
    }

  /* try to allocate the new job */
  job = thunar_vfs_change_mode (thunar_file_get_path (chooser->file), dir_mask, dir_mode, file_mask, file_mode, recursive, &error);
  if (G_UNLIKELY (job == NULL))
    {
      /* display an error to the user */
      thunar_dialogs_show_error (GTK_WIDGET (chooser), error, _("Failed to apply new permissions"));
      g_error_free (error);
    }
  else
    {
      /* handle the job */
      thunar_permissions_chooser_job_start (chooser, job, recursive);
      g_object_unref (G_OBJECT (job));
    }
}



static void
thunar_permissions_chooser_access_changed (ThunarPermissionsChooser *chooser,
                                           GtkWidget                *combo)
{
  ThunarVfsFileMode file_mask;
  ThunarVfsFileMode file_mode;
  ThunarVfsFileMode dir_mask;
  ThunarVfsFileMode dir_mode;
  guint             n;

  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (GTK_IS_COMBO_BOX (combo));

  /* verify that we have a valid file */
  if (G_UNLIKELY (chooser->file == NULL))
    return;

  /* determine the new mode from the combo box */
  for (n = 0; chooser->access_combos[n] != combo && n < G_N_ELEMENTS (chooser->access_combos); ++n);
  dir_mode = file_mode = (gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) << 1) << (n * 3);
  dir_mask = file_mask = 0006 << (n * 3);

  /* keep exec bit in sync for folders */
  if (thunar_file_is_directory (chooser->file))
    {
      /* if either read or write is, set exec as well, else unset exec as well */
      if ((dir_mode & (0004 << (n * 3))) != 0)
        dir_mode |= (0001 << (n * 3));
      dir_mask = 0007 << (n * 3);
    }

  /* change the permissions */
  thunar_permissions_chooser_change_mode (chooser, dir_mask, dir_mode, file_mask, file_mode);
}



static gint
group_compare (gconstpointer group_a,
               gconstpointer group_b,
               gpointer      group_primary)
{
  ThunarVfsGroupId group_primary_id = thunar_vfs_group_get_id (THUNAR_VFS_GROUP (group_primary));
  ThunarVfsGroupId group_a_id = thunar_vfs_group_get_id (THUNAR_VFS_GROUP (group_a));
  ThunarVfsGroupId group_b_id = thunar_vfs_group_get_id (THUNAR_VFS_GROUP (group_b));

  /* check if the groups are equal */
  if (group_a_id == group_b_id)
    return 0;

  /* the primary group is always sorted first */
  if (group_a_id == group_primary_id)
    return -1;
  else if (group_b_id == group_primary_id)
    return 1;

  /* system groups (< 100) are always sorted last */
  if (group_a_id < 100 && group_b_id >= 100)
    return 1;
  else if (group_b_id < 100 && group_a_id >= 100)
    return -1;

  /* otherwise just sort by name */
  return g_ascii_strcasecmp (thunar_vfs_group_get_name (THUNAR_VFS_GROUP (group_a)), thunar_vfs_group_get_name (THUNAR_VFS_GROUP (group_b)));
}



static void
thunar_permissions_chooser_file_changed (ThunarPermissionsChooser *chooser,
                                         ThunarFile               *file)
{
  ThunarVfsUserManager *user_manager;
  ThunarVfsFileMode     mode;
  ThunarVfsGroup       *group;
  ThunarVfsUser        *user;
  GtkListStore         *store;
  GtkTreeIter           iter;
  const gchar          *user_name;
  const gchar          *real_name;
  GList                *groups;
  GList                *lp;
  gchar                 buffer[1024];
  guint                 n;

  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (THUNAR_IS_FILE (file));
  _thunar_return_if_fail (chooser->file == file);

  /* allocate a new store for the group combo box */
  g_signal_handlers_block_by_func (G_OBJECT (chooser->group_combo), thunar_permissions_chooser_group_changed, chooser);
  store = gtk_list_store_new (THUNAR_PERMISSIONS_STORE_N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT);
  gtk_combo_box_set_model (GTK_COMBO_BOX (chooser->group_combo), GTK_TREE_MODEL (store));

  /* determine the owner of the new file */
  user = thunar_file_get_user (file);
  if (G_LIKELY (user != NULL))
    {
      /* determine the group of the new file */
      group = thunar_file_get_group (file);
      if (G_LIKELY (group != NULL))
        {
          /* check if we have superuser privileges */
          if (G_UNLIKELY (geteuid () == 0))
            {
              /* determine all groups in the system */
              user_manager = thunar_vfs_user_manager_get_default ();
              groups = thunar_vfs_user_manager_get_all_groups (user_manager);
              g_object_unref (G_OBJECT (user_manager));
            }
          else
            {
              /* determine the groups for the user and take a copy */
              groups = g_list_copy (thunar_vfs_user_get_groups (user));
              g_list_foreach (groups, (GFunc) g_object_ref, NULL);
            }

          /* make sure that the group list includes the file group */
          if (G_UNLIKELY (g_list_find (groups, group) == NULL))
            groups = g_list_prepend (groups, g_object_ref (G_OBJECT (group)));

          /* sort the groups according to group_compare() */
          groups = g_list_sort_with_data (groups, group_compare, group);

          /* add the groups to the store */
          for (lp = groups, n = 0; lp != NULL; lp = lp->next)
            {
              /* append a separator after the primary group and after the user-groups (not system groups) */
              if (thunar_vfs_group_get_id (groups->data) == thunar_vfs_group_get_id (group) && lp != groups && n == 0)
                {
                  gtk_list_store_append (store, &iter);
                  n += 1;
                }
              else if (lp != groups && thunar_vfs_group_get_id (lp->data) < 100 && n == 1)
                {
                  gtk_list_store_append (store, &iter);
                  n += 1;
                }

              /* append a new item for the group */
              gtk_list_store_append (store, &iter);
              gtk_list_store_set (store, &iter,
                                  THUNAR_PERMISSIONS_STORE_COLUMN_NAME, thunar_vfs_group_get_name (lp->data),
                                  THUNAR_PERMISSIONS_STORE_COLUMN_GID, thunar_vfs_group_get_id (lp->data),
                                  -1);

              /* set the active iter for the combo box if this group is the primary group */
              if (G_UNLIKELY (lp->data == group))
                gtk_combo_box_set_active_iter (GTK_COMBO_BOX (chooser->group_combo), &iter);
            }

          /* cleanup */
          g_list_foreach (groups, (GFunc) g_object_unref, NULL);
          g_object_unref (G_OBJECT (group));
          g_list_free (groups);
        }

      /* determine sane display name for the owner */
      user_name = thunar_vfs_user_get_name (user);
      real_name = thunar_vfs_user_get_real_name (user);
      if (G_LIKELY (real_name != NULL))
        g_snprintf (buffer, sizeof (buffer), "%s (%s)", real_name, user_name);
      else
        g_strlcpy (buffer, user_name, sizeof (buffer));
      gtk_label_set_text (GTK_LABEL (chooser->user_label), buffer);
      g_object_unref (G_OBJECT (user));
    }
  else
    {
      gtk_label_set_text (GTK_LABEL (chooser->user_label), _("Unknown file owner"));
    }

  /* determine the file mode and update the combo boxes */
  mode = thunar_file_get_mode (file);
  for (n = 0; n < G_N_ELEMENTS (chooser->access_combos); ++n)
    {
      g_signal_handlers_block_by_func (G_OBJECT (chooser->access_combos[n]), thunar_permissions_chooser_access_changed, chooser);
      gtk_combo_box_set_active (GTK_COMBO_BOX (chooser->access_combos[n]), ((mode >> (n * 3)) & 0007) >> 1);
      g_signal_handlers_unblock_by_func (G_OBJECT (chooser->access_combos[n]), thunar_permissions_chooser_access_changed, chooser);
    }

  /* update the program setting based on the mode (only visible for regular files, allowed for execution) */
  g_signal_handlers_block_by_func (G_OBJECT (chooser->program_button), thunar_permissions_chooser_program_toggled, chooser);
  g_object_set (G_OBJECT (chooser->program_button), "visible", thunar_file_is_regular (file)
      && (thunarx_file_info_has_mime_type (THUNARX_FILE_INFO (file), "application/x-executable")
       || thunarx_file_info_has_mime_type (THUNARX_FILE_INFO (file), "application/x-shellscript")), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chooser->program_button), (mode & 0111) != 0);
  g_signal_handlers_unblock_by_func (G_OBJECT (chooser->program_button), thunar_permissions_chooser_program_toggled, chooser);

  /* update the "inconsistent folder permissions" warning and the "fix permissions" button based on the mode */
  if (thunar_file_is_directory (file) && ((mode & 0111) != ((mode >> 2) & 0111)))
    {
      /* display the "fix permissions" button if we can modify the file */
      g_object_set (G_OBJECT (chooser->fixperm_button), "visible", thunar_file_is_chmodable (file), NULL);

      /* always display the warning even if we cannot fix it */
      gtk_widget_show (chooser->fixperm_label);
    }
  else
    {
      /* hide both the warning text and the fix button */
      gtk_widget_hide (chooser->fixperm_button);
      gtk_widget_hide (chooser->fixperm_label);
    }

  /* release our reference on the new combo store and unblock the combo */
  g_signal_handlers_unblock_by_func (G_OBJECT (chooser->group_combo), thunar_permissions_chooser_group_changed, chooser);
  g_object_unref (G_OBJECT (store));

  /* emit notification on "mutable", so all widgets update their sensitivity */
  g_object_notify (G_OBJECT (chooser), "mutable");
}



static void
thunar_permissions_chooser_group_changed (ThunarPermissionsChooser *chooser,
                                          GtkWidget                *combo)
{
  ThunarVfsGroupId gid;
  GtkTreeModel    *model;
  GtkTreeIter      iter;

  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (chooser->group_combo == combo);
  _thunar_return_if_fail (GTK_IS_COMBO_BOX (combo));

  /* verify that we have a valid file */
  if (G_UNLIKELY (chooser->file == NULL))
    return;

  /* determine the tree model from the combo box */
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));

  /* determine the iterator for the selected item */
  if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter))
    {
      /* determine the group id for the selected item... */
      gtk_tree_model_get (model, &iter, THUNAR_PERMISSIONS_STORE_COLUMN_GID, &gid, -1);

      /* ...and try to change the group to the new gid */
      thunar_permissions_chooser_change_group (chooser, gid);
    }
}



static void
thunar_permissions_chooser_program_toggled (ThunarPermissionsChooser *chooser,
                                            GtkWidget                *button)
{
  ThunarVfsFileMode mode;

  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (chooser->program_button == button);
  _thunar_return_if_fail (GTK_IS_TOGGLE_BUTTON (button));

  /* verify that we have a valid file */
  if (G_UNLIKELY (chooser->file == NULL))
    return;

  /* determine the new mode based on the toggle state */
  mode = (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button))) ? 0111 : 0000;

  /* apply the new mode (only the executable bits for files) */
  thunar_permissions_chooser_change_mode (chooser, 0000, 0000, 0111, mode);
}



static void
thunar_permissions_chooser_fixperm_clicked (ThunarPermissionsChooser *chooser,
                                            GtkWidget                *button)
{
  ThunarVfsFileMode mode;
  ThunarVfsJob     *job;
  GtkWidget        *dialog;
  GtkWidget        *window;
  GError           *error = NULL;
  gint              response;

  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (chooser->fixperm_button == button);
  _thunar_return_if_fail (GTK_IS_BUTTON (button));

  /* verify that we have a valid file */
  if (G_UNLIKELY (chooser->file == NULL))
    return;

  /* determine the toplevel widget */
  window = gtk_widget_get_toplevel (GTK_WIDGET (chooser));
  if (G_UNLIKELY (window == NULL))
    return;

  /* popup a confirm dialog */
  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                   GTK_DIALOG_DESTROY_WITH_PARENT
                                   | GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_QUESTION,
                                   GTK_BUTTONS_NONE,
                                   _("Correct folder permissions automatically?"));
  gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
  gtk_dialog_add_button (GTK_DIALOG (dialog), _("Correct folder permissions"), GTK_RESPONSE_OK);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), _("The folder permissions will be reset to a consistent state. Only users "
                                                                           "allowed to read the contents of this folder will be allowed to enter the "
                                                                           "folder afterwards."));
  response = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

  /* check if we should apply the changes */
  if (G_LIKELY (chooser->file != NULL && response == GTK_RESPONSE_OK))
    {
      /* determine the current mode */
      mode = thunar_file_get_mode (chooser->file);

      /* determine the new mode (making sure the owner can read/enter the folder) */
      mode = (THUNAR_VFS_FILE_MODE_USR_READ | THUNAR_VFS_FILE_MODE_USR_EXEC)
           | (((mode & THUNAR_VFS_FILE_MODE_GRP_READ) != 0) ? THUNAR_VFS_FILE_MODE_GRP_EXEC : 0)
           | (((mode & THUNAR_VFS_FILE_MODE_OTH_READ) != 0) ? THUNAR_VFS_FILE_MODE_OTH_EXEC : 0);

      /* try to allocate the new job */
      job = thunar_vfs_change_mode (thunar_file_get_path (chooser->file), 0511, mode, 0000, 0000, FALSE, &error);
      if (G_UNLIKELY (job == NULL))
        {
          /* display an error to the user */
          thunar_dialogs_show_error (GTK_WIDGET (chooser), error, _("Failed to apply new permissions"));
          g_error_free (error);
        }
      else
        {
          /* handle the job */
          thunar_permissions_chooser_job_start (chooser, job, FALSE);
          g_object_unref (G_OBJECT (job));
        }
    }
}



static ThunarVfsJobResponse
thunar_permissions_chooser_job_ask (ThunarPermissionsChooser *chooser,
                                    const gchar              *message,
                                    ThunarVfsJobResponse      choices,
                                    ThunarVfsJob             *job)
{
  GtkWidget *toplevel;

  _thunar_return_val_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser), THUNAR_VFS_JOB_RESPONSE_CANCEL);
  _thunar_return_val_if_fail (g_utf8_validate (message, -1, NULL), THUNAR_VFS_JOB_RESPONSE_CANCEL);
  _thunar_return_val_if_fail (THUNAR_VFS_IS_JOB (job), THUNAR_VFS_JOB_RESPONSE_CANCEL);
  _thunar_return_val_if_fail (chooser->job == job, THUNAR_VFS_JOB_RESPONSE_CANCEL);

  /* be sure to display the progress bar prior to opening the question dialog */
  gtk_widget_show_now (chooser->job_progress);

  /* determine the toplevel window for the chooser */
  toplevel = gtk_widget_get_toplevel (GTK_WIDGET (chooser));
  if (G_UNLIKELY (toplevel == NULL))
    return THUNAR_VFS_JOB_RESPONSE_CANCEL;

  /* display the question dialog */
  return thunar_dialogs_show_job_ask (GTK_WINDOW (toplevel), message, choices);
}



static void
thunar_permissions_chooser_job_cancel (ThunarPermissionsChooser *chooser)
{
  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));

  /* verify that we have a job to cancel */
  if (G_UNLIKELY (chooser->job == NULL))
    return;

  /* cancel the job (if not already done) */
  thunar_vfs_job_cancel (chooser->job);

  /* disconnect from the job */
  g_signal_handlers_disconnect_matched (chooser->job, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, chooser);
  g_object_unref (G_OBJECT (chooser->job));
  chooser->job = NULL;

  /* hide the progress bar */
  gtk_widget_hide (chooser->job_progress);

  /* make the remaining widgets sensitive again */
  gtk_widget_set_sensitive (chooser->table, TRUE);
}



static void
thunar_permissions_chooser_job_error (ThunarPermissionsChooser *chooser,
                                      GError                   *error,
                                      ThunarVfsJob             *job)
{
  GtkWidget *toplevel;
  
  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (error != NULL && error->message != NULL);
  _thunar_return_if_fail (THUNAR_VFS_IS_JOB (job));
  _thunar_return_if_fail (chooser->job == job);

  /* be sure to display the progress bar prior to opening the error dialog */
  gtk_widget_show_now (chooser->job_progress);

  /* determine the toplevel widget for the chooser */
  toplevel = gtk_widget_get_toplevel (GTK_WIDGET (chooser));
  if (G_UNLIKELY (toplevel == NULL))
    return;

  /* popup the error message dialog */
  thunar_dialogs_show_job_error (GTK_WINDOW (toplevel), error);
}



static void
thunar_permissions_chooser_job_finished (ThunarPermissionsChooser *chooser,
                                         ThunarVfsJob             *job)
{
  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (THUNAR_VFS_IS_JOB (job));
  _thunar_return_if_fail (chooser->job == job);

  /* we can just use job_cancel(), since the job is already done */
  thunar_permissions_chooser_job_cancel (chooser);
}



static void
thunar_permissions_chooser_job_percent (ThunarPermissionsChooser *chooser,  
                                        gdouble                   percent,
                                        ThunarVfsJob             *job)
{
  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (THUNAR_VFS_IS_JOB (job));
  _thunar_return_if_fail (chooser->job == job);

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (chooser->job_progress), percent / 100.0);
  gtk_widget_show (chooser->job_progress);
}



static void
thunar_permissions_chooser_job_start (ThunarPermissionsChooser *chooser,
                                      ThunarVfsJob             *job,
                                      gboolean                  recursive)
{
  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (THUNAR_VFS_IS_JOB (job));
  _thunar_return_if_fail (chooser->job == NULL);

  /* take a reference to the job and connect signals */
  chooser->job = g_object_ref (G_OBJECT (job));
  g_signal_connect_swapped (G_OBJECT (job), "ask", G_CALLBACK (thunar_permissions_chooser_job_ask), chooser);
  g_signal_connect_swapped (G_OBJECT (job), "error", G_CALLBACK (thunar_permissions_chooser_job_error), chooser);
  g_signal_connect_swapped (G_OBJECT (job), "finished", G_CALLBACK (thunar_permissions_chooser_job_finished), chooser);

  /* don't connect percent for single file operations */
  if (G_UNLIKELY (recursive))
    g_signal_connect_swapped (G_OBJECT (job), "percent", G_CALLBACK (thunar_permissions_chooser_job_percent), chooser);
  
  /* setup the progress bar */
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (chooser->job_progress), 0.0);

  /* make the majority of widgets insensitive if doing recursively */
  gtk_widget_set_sensitive (chooser->table, !recursive);
}



static gboolean
thunar_permissions_chooser_row_separator (GtkTreeModel *model,
                                          GtkTreeIter  *iter,
                                          gpointer      data)
{
  gchar *name;

  /* determine the value of the "name" column */
  gtk_tree_model_get (model, iter, THUNAR_PERMISSIONS_STORE_COLUMN_NAME, &name, -1);
  if (G_LIKELY (name != NULL))
    {
      g_free (name);
      return FALSE;
    }

  return TRUE;
}



/**
 * thunar_permissions_chooser_new:
 *
 * Allocates a new #ThunarPermissionsChooser instance.
 *
 * Return value: the newly allocated #ThunarPermissionsChooser.
 **/
GtkWidget*
thunar_permissions_chooser_new (void)
{
  return g_object_new (THUNAR_TYPE_PERMISSIONS_CHOOSER, NULL);
}



/**
 * thunar_permissions_chooser_get_file:
 * @chooser : a #ThunarPermissionsChooser.
 *
 * Returns the #ThunarFile associated with the specified @chooser.
 *
 * Return value: the file associated with @chooser.
 **/
ThunarFile*
thunar_permissions_chooser_get_file (ThunarPermissionsChooser *chooser)
{
  _thunar_return_val_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser), NULL);
  return chooser->file;
}



/**
 * thunar_permissions_chooser_set_file:
 * @chooser : a #ThunarPermissionsChooser.
 * @file    : a #ThunarFile or %NULL.
 *
 * Associates @chooser with the specified @file.
 **/
void
thunar_permissions_chooser_set_file (ThunarPermissionsChooser *chooser,
                                     ThunarFile               *file)
{
  _thunar_return_if_fail (THUNAR_IS_PERMISSIONS_CHOOSER (chooser));
  _thunar_return_if_fail (file == NULL || THUNAR_IS_FILE (file));

  /* check if we already use that file */
  if (G_UNLIKELY (chooser->file == file))
    return;

  /* disconnect from the previous file */
  if (G_LIKELY (chooser->file != NULL))
    {
      g_signal_handlers_disconnect_by_func (G_OBJECT (chooser->file), thunar_permissions_chooser_file_changed, chooser);
      g_object_unref (G_OBJECT (chooser->file));
    }

  /* activate the new file */
  chooser->file = file;

  /* connect to the new file */
  if (G_LIKELY (file != NULL))
    {
      /* take a reference on the file */
      g_object_ref (G_OBJECT (file));

      /* stay informed about changes */
      g_signal_connect_swapped (G_OBJECT (file), "changed", G_CALLBACK (thunar_permissions_chooser_file_changed), chooser);

      /* update our state */
      thunar_permissions_chooser_file_changed (chooser, file);
    }

  /* notify listeners */
  g_object_notify (G_OBJECT (chooser), "file");
}


