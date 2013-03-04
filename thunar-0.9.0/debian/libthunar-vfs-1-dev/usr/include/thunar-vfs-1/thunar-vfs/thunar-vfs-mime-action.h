/* $Id: thunar-vfs-mime-action.h 26421 2007-12-02 13:46:28Z benny $ */
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

#if !defined (THUNAR_VFS_INSIDE_THUNAR_VFS_H) && !defined (THUNAR_VFS_COMPILATION)
#error "Only <thunar-vfs/thunar-vfs.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __THUNAR_VFS_MIME_ACTION_H__
#define __THUNAR_VFS_MIME_ACTION_H__

#include <thunar-vfs/thunar-vfs-mime-handler.h>

G_BEGIN_DECLS;

typedef struct _ThunarVfsMimeActionClass ThunarVfsMimeActionClass;
typedef struct _ThunarVfsMimeAction      ThunarVfsMimeAction;

#define THUNAR_VFS_TYPE_MIME_ACTION             (thunar_vfs_mime_action_get_type ())
#define THUNAR_VFS_MIME_ACTION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), THUNAR_VFS_TYPE_MIME_ACTION, ThunarVfsMimeAction))
#define THUNAR_VFS_MIME_ACTION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), THUNAR_VFS_TYPE_MIME_ACTION, ThunarVfsMimeActionClass))
#define THUNAR_VFS_IS_MIME_ACTION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THUNAR_VFS_TYPE_MIME_ACTION))
#define THUNAR_VFS_IS_MIME_ACTION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), THUNAR_VFS_TYPE_MIME_ACTION))
#define THUNAR_VFS_MIME_ACTION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), THUNAR_VFS_TYPE_MIME_ACTION, ThunarVfsMimeActionClass))

GType thunar_vfs_mime_action_get_type (void) G_GNUC_CONST;

G_END_DECLS;

#endif /* !__THUNAR_VFS_MIME_ACTION_H__ */
