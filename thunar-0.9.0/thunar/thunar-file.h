/* $Id: thunar-file.h 26421 2007-12-02 13:46:28Z benny $ */
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

#ifndef __THUNAR_FILE_H__
#define __THUNAR_FILE_H__

#include <thunar/thunar-enum-types.h>
#include <thunar/thunar-metafile.h>
#include <thunarx/thunarx.h>

G_BEGIN_DECLS;

typedef struct _ThunarFileClass ThunarFileClass;
typedef struct _ThunarFile      ThunarFile;

#define THUNAR_TYPE_FILE            (thunar_file_get_type ())
#define THUNAR_FILE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), THUNAR_TYPE_FILE, ThunarFile))
#define THUNAR_FILE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), THUNAR_TYPE_FILE, ThunarFileClass))
#define THUNAR_IS_FILE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THUNAR_TYPE_FILE))
#define THUNAR_IS_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), THUNAR_TYPE_FILE))
#define THUNAR_FILE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), THUNAR_TYPE_FILE, ThunarFileClass))

/**
 * ThunarFileDateType:
 * @THUNAR_FILE_DATE_ACCESSED : date of last access to the file.
 * @THUNAR_FILE_DATE_CHANGED  : date of last change to the file meta data or the content.
 * @THUNAR_FILE_DATE_MODIFIED : date of last modification of the file's content.
 *
 * The various dates that can be queried about a #ThunarFile. Note, that not all
 * #ThunarFile implementations support all types listed above. See the documentation
 * of the thunar_file_get_date() method for details.
 **/
typedef enum
{
  THUNAR_FILE_DATE_ACCESSED,
  THUNAR_FILE_DATE_CHANGED,
  THUNAR_FILE_DATE_MODIFIED,
} ThunarFileDateType;

/**
 * ThunarFileIconState:
 * @THUNAR_FILE_ICON_STATE_DEFAULT : the default icon for the file.
 * @THUNAR_FILE_ICON_STATE_DROP    : the drop accept icon for the file.
 * @THUNAR_FILE_ICON_STATE_OPEN    : the folder is expanded.
 *
 * The various file icon states that are used within the file manager
 * views.
 **/
typedef enum /*< enum >*/
{
  THUNAR_FILE_ICON_STATE_DEFAULT,
  THUNAR_FILE_ICON_STATE_DROP,
  THUNAR_FILE_ICON_STATE_OPEN,
} ThunarFileIconState;

/**
 * ThunarFileThumbState:
 * @THUNAR_FILE_THUMB_STATE_MASK    : the mask to extract the thumbnail state.
 * @THUNAR_FILE_THUMB_STATE_UNKNOWN : unknown whether there's a thumbnail.
 * @THUNAR_FILE_THUMB_STATE_NONE    : no thumbnail is available.
 * @THUNAR_FILE_THUMB_STATE_READY   : a thumbnail is available.
 * @THUNAR_FILE_THUMB_STATE_LOADING : a thumbnail is being generated.
 *
 * The state of the thumbnailing for a given #ThunarFile.
 **/
typedef enum /*< flags >*/
{
  THUNAR_FILE_THUMB_STATE_MASK    = 0x03,
  THUNAR_FILE_THUMB_STATE_UNKNOWN = 0x00,
  THUNAR_FILE_THUMB_STATE_NONE    = 0x01,
  THUNAR_FILE_THUMB_STATE_READY   = 0x02,
  THUNAR_FILE_THUMB_STATE_LOADING = 0x03,
} ThunarFileThumbState;

#define THUNAR_FILE_EMBLEM_NAME_SYMBOLIC_LINK "emblem-symbolic-link"
#define THUNAR_FILE_EMBLEM_NAME_CANT_READ "emblem-noread"
#define THUNAR_FILE_EMBLEM_NAME_CANT_WRITE "emblem-nowrite"
#define THUNAR_FILE_EMBLEM_NAME_DESKTOP "emblem-desktop"

struct _ThunarFileClass
{
  GObjectClass __parent__;

  /* signals */
  void (*destroy) (ThunarFile *file);
};

struct _ThunarFile
{
  GObject __parent__;

  /*< private >*/
  ThunarVfsInfo *info;
  guint flags;
};

GType             thunar_file_get_type             (void) G_GNUC_CONST;

ThunarFile       *thunar_file_get_for_info         (ThunarVfsInfo          *info);
ThunarFile       *thunar_file_get_for_path         (ThunarVfsPath          *path,
                                                    GError                **error);
ThunarFile       *thunar_file_get_for_uri          (const gchar            *uri,
                                                    GError                **error);

ThunarFile       *thunar_file_get_parent           (const ThunarFile       *file,
                                                    GError                **error);

gboolean          thunar_file_execute              (ThunarFile             *file,
                                                    GdkScreen              *screen,
                                                    GList                  *path_list,
                                                    GError                **error);

gboolean          thunar_file_launch               (ThunarFile             *file,
                                                    gpointer                parent,
                                                    GError                **error);

gboolean          thunar_file_rename               (ThunarFile             *file,
                                                    const gchar            *name,
                                                    GError                **error);

GdkDragAction     thunar_file_accepts_drop         (ThunarFile             *file,
                                                    GList                  *path_list,
                                                    GdkDragContext         *context,
                                                    GdkDragAction          *suggested_action_return);

const gchar      *thunar_file_get_display_name     (const ThunarFile       *file);

ThunarVfsFileTime thunar_file_get_date             (const ThunarFile       *file,
                                                    ThunarFileDateType      date_type);

gchar            *thunar_file_get_date_string      (const ThunarFile       *file,
                                                    ThunarFileDateType      date_type,
                                                    ThunarDateStyle         date_style) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gchar            *thunar_file_get_mode_string      (const ThunarFile       *file) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gchar            *thunar_file_get_size_string      (const ThunarFile       *file) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

ThunarVfsVolume  *thunar_file_get_volume           (const ThunarFile       *file,
                                                    ThunarVfsVolumeManager *volume_manager);

ThunarVfsGroup   *thunar_file_get_group            (const ThunarFile       *file);
ThunarVfsUser    *thunar_file_get_user             (const ThunarFile       *file);

gchar            *thunar_file_get_deletion_date    (const ThunarFile       *file,
                                                    ThunarDateStyle         date_style) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gchar            *thunar_file_get_original_path    (const ThunarFile       *file) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gboolean          thunar_file_is_chmodable         (const ThunarFile       *file);
gboolean          thunar_file_is_renameable        (const ThunarFile       *file);

GList            *thunar_file_get_emblem_names     (ThunarFile              *file);
void              thunar_file_set_emblem_names     (ThunarFile              *file,
                                                    GList                   *emblem_names);

gboolean          thunar_file_set_custom_icon      (ThunarFile              *file,
                                                    const gchar             *custom_icon,
                                                    GError                 **error) G_GNUC_WARN_UNUSED_RESULT;

const gchar      *thunar_file_get_icon_name        (const ThunarFile        *file,
                                                    ThunarFileIconState     icon_state,
                                                    GtkIconTheme           *icon_theme);

const gchar      *thunar_file_get_metadata         (ThunarFile             *file,
                                                    ThunarMetafileKey       key,
                                                    const gchar            *default_value);
void              thunar_file_set_metadata         (ThunarFile             *file,
                                                    ThunarMetafileKey       key,
                                                    const gchar            *value,
                                                    const gchar            *default_value);

void              thunar_file_watch                (ThunarFile             *file);
void              thunar_file_unwatch              (ThunarFile             *file);

void              thunar_file_reload               (ThunarFile             *file);

void              thunar_file_destroy              (ThunarFile             *file);


gint              thunar_file_compare_by_name      (const ThunarFile       *file_a,
                                                    const ThunarFile       *file_b,
                                                    gboolean                case_sensitive);


ThunarFile       *thunar_file_cache_lookup         (const ThunarVfsPath    *path);


GList            *thunar_file_list_get_applications  (GList *file_list);
GList            *thunar_file_list_to_path_list      (GList *file_list);


/**
 * thunar_file_has_parent:
 * @file : a #ThunarFile instance.
 *
 * Checks whether it is possible to determine the parent #ThunarFile
 * for @file.
 *
 * Return value: whether @file has a parent.
 **/
#define thunar_file_has_parent(file) (!thunar_vfs_path_is_root (THUNAR_FILE ((file))->info->path))

/**
 * thunar_file_get_info:
 * @file : a #ThunarFile instance.
 *
 * Returns the #ThunarVfsInfo for @file.
 *
 * Note, that there's no reference taken for the caller on the
 * returned #ThunarVfsInfo, so if you need the object for a longer
 * perioud, you'll need to take a reference yourself using the
 * thunar_vfs_info_ref() method.
 *
 * Return value: the #ThunarVfsInfo for @file.
 **/
#define thunar_file_get_info(file) (THUNAR_FILE ((file))->info)

/**
 * thunar_file_get_path:
 * @file  : a #ThunarFile instance.
 *
 * Returns the #ThunarVfsPath, that refers to the location of the @file.
 *
 * Note, that there's no reference taken for the caller on the
 * returned #ThunarVfsPath, so if you need the object for a longer
 * period, you'll need to take a reference yourself using the
 * thunar_vfs_path_ref() function.
 *
 * Return value: the path to the @file.
 **/
#define thunar_file_get_path(file) (THUNAR_FILE ((file))->info->path)

/**
 * thunar_file_get_mime_info:
 * @file : a #ThunarFile instance.
 *
 * Returns the MIME type information for the given @file object. This
 * function is garantied to always return a valid #ThunarVfsMimeInfo.
 *
 * Note, that there's no reference taken for the caller on the
 * returned #ThunarVfsMimeInfo, so if you need the object for a
 * longer period, you'll need to take a reference yourself using
 * the thunar_vfs_mime_info_ref() function.
 *
 * Return value: the MIME type.
 **/
#define thunar_file_get_mime_info(file) (THUNAR_FILE ((file))->info->mime_info)

/**
 * thunar_file_get_kind:
 * @file : a #ThunarFile instance.
 *
 * Returns the kind of @file.
 *
 * Return value: the kind of @file.
 **/
#define thunar_file_get_kind(file) (THUNAR_FILE ((file))->info->type)

/**
 * thunar_file_get_mode:
 * @file : a #ThunarFile instance.
 *
 * Returns the permission bits of @file.
 *
 * Return value: the permission bits of @file.
 **/
#define thunar_file_get_mode(file) (THUNAR_FILE ((file))->info->mode)

/**
 * thunar_file_get_size:
 * @file : a #ThunarFile instance.
 *
 * Tries to determine the size of @file in bytes and
 * returns the size.
 *
 * Return value: the size of @file in bytes.
 **/
#define thunar_file_get_size(file) (THUNAR_FILE ((file))->info->size)

/**
 * thunar_file_get_free_space:
 * @file              : a #ThunarFile instance.
 * @free_space_return : return location for the amount of
 *                      free space or %NULL.
 *
 * Determines the amount of free space of the volume on
 * which @file resides. Returns %TRUE if the amount of
 * free space was determined successfully and placed into
 * @free_space_return, else %FALSE will be returned.
 *
 * Return value: %TRUE if successfull, else %FALSE.
 **/
#define thunar_file_get_free_space(file, free_space_return) (thunar_vfs_info_get_free_space (THUNAR_FILE ((file))->info, (free_space_return)))

/**
 * thunar_file_dup_uri:
 * @file : a #ThunarFile instance.
 *
 * Returns the URI for the @file. The caller is responsible
 * to free the returned string when no longer needed.
 *
 * Return value: the URI for @file.
 **/
#define thunar_file_dup_uri(file) (thunar_vfs_path_dup_uri (thunar_file_get_path ((file))))


/**
 * thunar_file_get_custom_icon:
 * @file : a #ThunarFile instance.
 *
 * Queries the custom icon from @file if any,
 * else %NULL is returned. The custom icon
 * can be either a themed icon name or an
 * absolute path to an icon file in the local
 * file system.
 *
 * Return value: the custom icon for @file
 *               or %NULL.
 **/
#define thunar_file_get_custom_icon(file) (thunar_vfs_info_get_custom_icon (THUNAR_FILE ((file))->info))



/**
 * thunar_file_changed:
 * @file : a #ThunarFile instance.
 *
 * Emits the ::changed signal on @file. This function is meant to be called
 * by derived classes whenever they notice changes to the @file.
 **/
#define thunar_file_changed(file)                         \
G_STMT_START{                                             \
  thunarx_file_info_changed (THUNARX_FILE_INFO ((file))); \
}G_STMT_END



/**
 * thunar_file_is_local:
 * @file : a #ThunarFile instance.
 *
 * Returns %TRUE if @file is a local file with the
 * %THUNAR_VFS_PATH_SCHEME_FILE scheme.
 *
 * Return value: %TRUE if @file is local.
 **/
#define thunar_file_is_local(file) (thunar_vfs_path_get_scheme (thunar_file_get_path ((file))) == THUNAR_VFS_PATH_SCHEME_FILE)

/**
 * thunar_file_is_trashed:
 * @file : a #ThunarFile instance.
 *
 * Returns %TRUE if @file is a local file with the
 * %THUNAR_VFS_PATH_SCHEME_TRASH scheme.
 *
 * Return value: %TRUE if @file is in the trash, or
 *               the trash folder itself.
 **/
#define thunar_file_is_trashed(file) (thunar_vfs_path_get_scheme (thunar_file_get_path ((file))) == THUNAR_VFS_PATH_SCHEME_TRASH)

/**
 * thunar_file_is_ancestor:
 * @file     : a #ThunarFile instance.
 * @ancestor : another #ThunarFile instance.
 *
 * Determines whether @file is somewhere inside @ancestor,
 * possibly with intermediate folders.
 *
 * Return value: %TRUE if @ancestor contains @file as a
 *               child, grandchild, great grandchild, etc.
 **/
#define thunar_file_is_ancestor(file, ancestor) (thunar_vfs_path_is_ancestor (thunar_file_get_path ((file)), thunar_file_get_path ((ancestor))))

/**
 * thunar_file_is_directory:
 * @file : a #ThunarFile instance.
 *
 * Checks whether @file refers to a directory.
 *
 * Return value: %TRUE if @file is a directory.
 **/
#define thunar_file_is_directory(file) (THUNAR_FILE ((file))->info->type == THUNAR_VFS_FILE_TYPE_DIRECTORY)

/**
 * thunar_file_is_executable:
 * @file : a #ThunarFile instance.
 *
 * Determines whether the owner of the current process is allowed
 * to execute the @file (or enter the directory refered to by
 * @file).
 *
 * Return value: %TRUE if @file can be executed.
 **/
#define thunar_file_is_executable(file) (THUNAR_FILE ((file))->info->flags & THUNAR_VFS_FILE_FLAGS_EXECUTABLE)

/**
 * thunar_file_is_readable:
 * @file : a #ThunarFile instance.
 *
 * Determines whether the owner of the current process is allowed
 * to read the @file.
 *
 * Return value: %TRUE if @file can be read.
 **/
#define thunar_file_is_readable(file) (THUNAR_FILE ((file))->info->flags & THUNAR_VFS_FILE_FLAGS_READABLE)

/**
 * thunar_file_is_writable:
 * @file : a #ThunarFile instance.
 *
 * Determines whether the owner of the current process is allowed
 * to write the @file.
 *
 * Return value: %TRUE if @file can be read.
 **/
#define thunar_file_is_writable(file) (THUNAR_FILE ((file))->info->flags & THUNAR_VFS_FILE_FLAGS_WRITABLE)

/**
 * thunar_file_is_hidden:
 * @file : a #ThunarFile instance.
 *
 * Checks whether @file can be considered a hidden file.
 *
 * Return value: %TRUE if @file is a hidden file, else %FALSE.
 **/
#define thunar_file_is_hidden(file) ((THUNAR_FILE ((file))->info->flags & THUNAR_VFS_FILE_FLAGS_HIDDEN) != 0)

/**
 * thunar_file_is_home:
 * @file : a #ThunarFile.
 *
 * Checks whether @file refers to the users home directory.
 *
 * Return value: %TRUE if @file is the users home directory.
 **/
#define thunar_file_is_home(file) (thunar_vfs_path_is_home (THUNAR_FILE ((file))->info->path))

/**
 * thunar_file_is_desktop:
 * @file : a #ThunarFile.
 *
 * Checks whether @file refers to the users desktop directory.
 *
 * Return value: %TRUE if @file is the users desktop directory.
 **/
#define thunar_file_is_desktop(file) (!thunar_vfs_path_is_root (thunar_file_get_path (file))                              \
                                   && thunar_vfs_path_is_home (thunar_vfs_path_get_parent (thunar_file_get_path (file)))  \
                                   && exo_str_is_equal (thunar_file_get_display_name (file), "Desktop"))

/**
 * thunar_file_is_regular:
 * @file : a #ThunarFile.
 *
 * Checks whether @file refers to a regular file.
 *
 * Return value: %TRUE if @file is a regular file.
 **/
#define thunar_file_is_regular(file) (THUNAR_FILE ((file))->info->type == THUNAR_VFS_FILE_TYPE_REGULAR)

/**
 * thunar_file_is_root:
 * @file : a #ThunarFile.
 *
 * Checks whether @file refers to the root directory.
 *
 * Return value: %TRUE if @file is the root directory.
 **/
#define thunar_file_is_root(file) (thunar_vfs_path_is_root (THUNAR_FILE ((file))->info->path))

/**
 * thunar_file_is_symlink:
 * @file : a #ThunarFile.
 *
 * Returns %TRUE if @file is a symbolic link.
 *
 * Return value: %TRUE if @file is a symbolic link.
 **/
#define thunar_file_is_symlink(file) ((THUNAR_FILE ((file))->info->flags & THUNAR_VFS_FILE_FLAGS_SYMLINK) != 0)

/**
 * thunar_file_is_desktop_file:
 * @file : a #ThunarFile.
 *
 * Returns %TRUE if @file is a .desktop file, but not a .directory file.
 *
 * Return value: %TRUE if @file is a .desktop file.
 **/
#define thunar_file_is_desktop_file(file) (exo_str_is_equal (thunar_vfs_mime_info_get_name (thunar_file_get_mime_info ((file))), "application/x-desktop") \
                                        && !exo_str_is_equal (thunar_vfs_path_get_name (thunar_file_get_path ((file))), ".directory"))

/**
 * thunar_file_get_display_name:
 * @file : a #ThunarFile instance.
 *
 * Returns the @file name in the UTF-8 encoding, which is
 * suitable for displaying the file name in the GUI.
 *
 * Return value: the @file name suitable for display.
 **/
#define thunar_file_get_display_name(file) (THUNAR_FILE ((file))->info->display_name)

/**
 * thunar_file_read_link:
 * @file  : a #ThunarFile instance.
 * @error : return location for errors or %NULL.
 *
 * Simple wrapper to thunar_vfs_info_read_link().
 *
 * Return value: the link target of @file or %NULL
 *               if an error occurred.
 **/
#define thunar_file_read_link(file, error) (thunar_vfs_info_read_link (THUNAR_FILE ((file))->info, (error)))



/**
 * thunar_file_get_thumb_state:
 * @file : a #ThunarFile.
 *
 * Returns the current #ThunarFileThumbState for @file. This
 * method is intended to be used by #ThunarIconFactory only.
 *
 * Return value: the #ThunarFileThumbState for @file.
 **/
#define thunar_file_get_thumb_state(file) (THUNAR_FILE ((file))->flags & THUNAR_FILE_THUMB_STATE_MASK)

/**
 * thunar_file_set_thumb_state:
 * @file        : a #ThunarFile.
 * @thumb_state : the new #ThunarFileThumbState.
 *
 * Sets the #ThunarFileThumbState for @file
 * to @thumb_state. This method is intended
 * to be used by #ThunarIconFactory only.
 **/ 
#define thunar_file_set_thumb_state(file, thumb_state)                    \
G_STMT_START{                                                             \
  ThunarFile *f = THUNAR_FILE ((file));                                   \
  f->flags = (f->flags & ~THUNAR_FILE_THUMB_STATE_MASK) | (thumb_state);  \
}G_STMT_END



/**
 * thunar_file_list_copy:
 * @file_list : a list of #ThunarFile<!---->s.
 *
 * Returns a deep-copy of @file_list, which must be
 * freed using thunar_file_list_free().
 *
 * Return value: a deep copy of @file_list.
 **/
#define thunar_file_list_copy(file_list) (thunarx_file_info_list_copy ((file_list)))

/**
 * thunar_file_list_free:
 * @file_list : a list of #ThunarFile<!---->s.
 *
 * Unrefs the #ThunarFile<!---->s contained in @file_list
 * and frees the list itself.
 **/
#define thunar_file_list_free(file_list)      \
G_STMT_START{                                 \
  thunarx_file_info_list_free ((file_list));  \
}G_STMT_END


G_END_DECLS;

#endif /* !__THUNAR_FILE_H__ */
