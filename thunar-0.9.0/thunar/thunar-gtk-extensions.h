/* $Id: thunar-gtk-extensions.h 26421 2007-12-02 13:46:28Z benny $ */
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

#ifndef __THUNAR_GTK_EXTENSIONS_H__
#define __THUNAR_GTK_EXTENSIONS_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS;

void         thunar_gtk_action_set_tooltip                (GtkAction          *action,
                                                           const gchar        *format,
                                                           ...) G_GNUC_INTERNAL G_GNUC_PRINTF (2, 3);

void         thunar_gtk_action_group_set_action_sensitive (GtkActionGroup     *action_group,
                                                           const gchar        *action_name,
                                                           gboolean            sensitive) G_GNUC_INTERNAL;

void         thunar_gtk_icon_factory_insert_icon          (GtkIconFactory     *icon_factory,
                                                           const gchar        *stock_id,
                                                           const gchar        *icon_name) G_GNUC_INTERNAL;

void         thunar_gtk_label_set_a11y_relation           (GtkLabel           *label,
                                                           GtkWidget          *widget) G_GNUC_INTERNAL;

void         thunar_gtk_menu_run                          (GtkMenu            *menu,
                                                           gpointer            parent,
                                                           GtkMenuPositionFunc func,
                                                           gpointer            data,
                                                           guint               button,
                                                           guint32             activate_time) G_GNUC_INTERNAL;

GtkAction   *thunar_gtk_ui_manager_get_action_by_name     (GtkUIManager       *ui_manager,
                                                           const gchar        *action_name) G_GNUC_INTERNAL;

void         thunar_gtk_widget_set_tooltip                (GtkWidget          *widget,
                                                           const gchar        *format,
                                                           ...) G_GNUC_INTERNAL G_GNUC_PRINTF (2, 3);

G_END_DECLS;

#endif /* !__THUNAR_GTK_EXTENSIONS_H__ */
