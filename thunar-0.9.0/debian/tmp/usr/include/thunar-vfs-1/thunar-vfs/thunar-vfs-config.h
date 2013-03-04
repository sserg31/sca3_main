/* $Id: thunar-vfs-config.h.in 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
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

#ifndef __THUNAR_VFS_CONFIG_H__
#define __THUNAR_VFS_CONFIG_H__

#include <exo/exo.h>

#if !defined (THUNAR_VFS_INSIDE_THUNAR_VFS_H) && !defined (THUNAR_VFS_COMPILATION)
#error "Only <thunar-vfs/thunar-vfs.h> can be included directly, this file may disappear or change contents."
#endif

G_BEGIN_DECLS;

/* verify that G_GNUC_WARN_UNUSED_RESULT is defined */
#if !defined(G_GNUC_WARN_UNUSED_RESULT)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define G_GNUC_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define G_GNUC_WARN_UNUSED_RESULT
#endif /* __GNUC__ */
#endif /* !defined(G_GNUC_WARN_UNUSED_RESULT) */

#define THUNAR_VFS_MAJOR_VERSION 0
#define THUNAR_VFS_MINOR_VERSION 9
#define THUNAR_VFS_MICRO_VERSION 0

#define THUNAR_VFS_CHECK_VERSION(major,minor,micro) \
  (THUNAR_VFS_MAJOR_VERSION > (major) \
   || (THUNAR_VFS_MAJOR_VERSION == (major) \
       && THUNAR_VFS_MINOR_VERSION > (minor)) \
   || (THUNAR_VFS_MAJOR_VERSION == (major) \
       && THUNAR_VFS_MINOR_VERSION == (minor) \
       && THUNAR_VFS_MICRO_VERSION >= (micro)))

extern const guint thunar_vfs_major_version;
extern const guint thunar_vfs_minor_version;
extern const guint thunar_vfs_micro_version;

const gchar *thunar_vfs_check_version (guint required_major,
                                       guint required_minor,
                                       guint required_micro) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS;

#endif /* !__THUNAR_VFS_CONFIG_H__ */
