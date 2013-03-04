/* $Id: xfsm-util.h 20833 2006-04-09 18:08:17Z benny $ */
/*-
 * Copyright (c) 2003-2004 Benedikt Meurer <benny@xfce.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *                                                                              
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *                                                                              
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef __XFSM_UTIL_H__
#define __XFSM_UTIL_H__

#include <gtk/gtk.h>

#include <libxfce4util/libxfce4util.h>

G_BEGIN_DECLS;

GtkWidget	*xfsm_imgbtn_new(const gchar *, const gchar *, GtkWidget **);

gboolean xfsm_start_application (gchar      **command,
                                 gchar      **environment,
                                 GdkScreen   *screen,
                                 const gchar *current_directory,
                                 const gchar *client_machine,
                                 const gchar *user_id);

void xfsm_place_trash_window (GtkWindow *window,
                              GdkScreen *screen,
                              gint       monitor);

/* XXX - move to libxfce4util? */
gchar **xfsm_strv_copy (gchar **v);
gboolean xfsm_strv_equal (gchar **a, gchar **b);

void xfsm_window_add_border (GtkWindow *window);

void xfsm_window_grab_input (GtkWindow *window);

XfceRc *xfsm_open_config (gboolean readonly);

G_END_DECLS;

#endif /* !__XFSM_UTIL_H__ */
