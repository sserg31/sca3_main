/* $Id: thunar-abstract-dialog.c 26421 2007-12-02 13:46:28Z benny $ */
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

#include <gdk/gdkkeysyms.h>

#include <thunar/thunar-abstract-dialog.h>



static void thunar_abstract_dialog_class_init (ThunarAbstractDialogClass *klass);
static void thunar_abstract_dialog_close      (GtkDialog                 *dialog);



GType
thunar_abstract_dialog_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarAbstractDialogClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_abstract_dialog_class_init,
        NULL,
        NULL,
        sizeof (ThunarAbstractDialog),
        0,
        NULL,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_DIALOG, I_("ThunarAbstractDialog"), &info, G_TYPE_FLAG_ABSTRACT);
    }

  return type;
}



static void
thunar_abstract_dialog_class_init (ThunarAbstractDialogClass *klass)
{
  GtkDialogClass *gtkdialog_class;
  GtkBindingSet  *binding_set;

  gtkdialog_class = GTK_DIALOG_CLASS (klass);
  gtkdialog_class->close = thunar_abstract_dialog_close;

  /* connect additional key bindings to the GtkDialog::close action signal */
  binding_set = gtk_binding_set_by_class (klass);
  gtk_binding_entry_add_signal (binding_set, GDK_w, GDK_CONTROL_MASK, "close", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_W, GDK_CONTROL_MASK, "close", 0);
}



static void
thunar_abstract_dialog_close (GtkDialog *dialog)
{
  GdkEvent *event;

  /* verify that dialog is realized */
  if (G_LIKELY (GTK_WIDGET_REALIZED (dialog)))
    {
      /* send a delete event to the dialog */
      event = gdk_event_new (GDK_DELETE);
      event->any.window = g_object_ref (GTK_WIDGET (dialog)->window);
      event->any.send_event = TRUE;
      gtk_main_do_event (event);
      gdk_event_free (event);
    }
}

