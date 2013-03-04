/* $Id: thunar-vfs-exec.h 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2007 Benedikt Meurer <benny@xfce.org>
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

#ifndef __THUNAR_VFS_EXEC_H__
#define __THUNAR_VFS_EXEC_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS;

gboolean thunar_vfs_exec_parse     (const gchar    *exec,
                                    GList          *path_list,
                                    const gchar    *icon,
                                    const gchar    *name,
                                    const gchar    *path,
                                    gboolean        terminal,
                                    gint           *argc,
                                    gchar        ***argv,
                                    GError        **error) G_GNUC_INTERNAL;

gboolean thunar_vfs_exec_on_screen (GdkScreen      *screen,
                                    const gchar    *working_directory,
                                    gchar         **argv,
                                    gchar         **envp,
                                    GSpawnFlags     flags,
                                    gboolean        startup_notify,
                                    const gchar    *icon_name,
                                    GError        **error) G_GNUC_INTERNAL;

gboolean thunar_vfs_exec_sync      (const gchar    *command_line,
                                    GError        **error,
                                    ...) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__THUNAR_VFS_EXEC_H__ */
