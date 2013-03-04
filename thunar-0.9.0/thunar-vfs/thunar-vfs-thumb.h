/* $Id: thunar-vfs-thumb.h 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2004-2006 Benedikt Meurer <benny@xfce.org>
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

#ifndef __THUNAR_VFS_THUMB_H__
#define __THUNAR_VFS_THUMB_H__

#include <thunar-vfs/thunar-vfs-info.h>
#include <thunar-vfs/thunar-vfs-job.h>

G_BEGIN_DECLS;

typedef struct _ThunarVfsThumbFactoryClass ThunarVfsThumbFactoryClass;
typedef struct _ThunarVfsThumbFactory      ThunarVfsThumbFactory;

#define THUNAR_VFS_TYPE_THUMB_FACTORY             (thunar_vfs_thumb_factory_get_type ())
#define THUNAR_VFS_THUMB_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), THUNAR_VFS_TYPE_THUMB_FACTORY, ThunarVfsThumbFactory))
#define THUNAR_VFS_THUMB_FACTORY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), THUNAR_VFS_TYPE_THUMB_FACTORY, ThunarVfsThumbFactoryClass))
#define THUNAR_VFS_IS_THUMB_FACTORY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THUNAR_VFS_TYPE_THUMB_FACTORY))
#define THUNAR_VFS_IS_THUMB_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), THUNAR_VFS_TYPE_THUMB_FACTORY))
#define THUNAR_VFS_THUMB_FACTORY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), THUNAR_VFS_TYPE_THUMB_FACTORY, ThunarVfsThumbFactoryClass))

/**
 * ThunarVfsThumbSize:
 * @THUNAR_VFS_THUMB_SIZE_NORMAL : thumbnails at size 128x128.
 * @THUNAR_VFS_THUMB_SIZE_LARGE  : thumbnails at size 256x256.
 *
 * The desired size of thumbnails loaded by a #ThunarVfsThumbFactory.
 **/
typedef enum
{
  THUNAR_VFS_THUMB_SIZE_NORMAL,
  THUNAR_VFS_THUMB_SIZE_LARGE,
} ThunarVfsThumbSize;

GType                  thunar_vfs_thumb_factory_get_type              (void) G_GNUC_CONST;

ThunarVfsThumbFactory *thunar_vfs_thumb_factory_new                   (ThunarVfsThumbSize       size) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gchar                 *thunar_vfs_thumb_factory_lookup_thumbnail      (ThunarVfsThumbFactory   *factory,
                                                                       const ThunarVfsInfo     *info) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gboolean               thunar_vfs_thumb_factory_can_thumbnail         (ThunarVfsThumbFactory   *factory,
                                                                       const ThunarVfsInfo     *info);

gboolean               thunar_vfs_thumb_factory_has_failed_thumbnail  (ThunarVfsThumbFactory   *factory,
                                                                       const ThunarVfsInfo     *info);

GdkPixbuf             *thunar_vfs_thumb_factory_generate_thumbnail    (ThunarVfsThumbFactory   *factory,
                                                                       const ThunarVfsInfo     *info) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gboolean               thunar_vfs_thumb_factory_store_thumbnail       (ThunarVfsThumbFactory   *factory,
                                                                       const GdkPixbuf         *pixbuf,
                                                                       const ThunarVfsInfo     *info,
                                                                       GError                 **error);


gchar   *thunar_vfs_thumbnail_for_path  (const ThunarVfsPath *path,
                                         ThunarVfsThumbSize   size) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gboolean thunar_vfs_thumbnail_is_valid  (const gchar         *thumbnail,
                                         const gchar         *uri,
                                         ThunarVfsFileTime    mtime);

G_END_DECLS;

#endif /* !__THUNAR_VFS_THUMB_H__ */
