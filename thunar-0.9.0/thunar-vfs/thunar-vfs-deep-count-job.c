/* $Id: thunar-vfs-deep-count-job.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <thunar-vfs/thunar-vfs-deep-count-job.h>
#include <thunar-vfs/thunar-vfs-io-trash.h>
#include <thunar-vfs/thunar-vfs-job-private.h>
#include <thunar-vfs/thunar-vfs-marshal.h>
#include <thunar-vfs/thunar-vfs-path-private.h>
#include <thunar-vfs/thunar-vfs-private.h>
#include <thunar-vfs/thunar-vfs-alias.h>

/* use g_lstat() and g_stat() on win32 */
#if defined(G_OS_WIN32)
#include <glib/gstdio.h>
#else
#define g_lstat(path, statb) (lstat ((path), (statb)))
#define g_stat(path, statb) (stat ((path), (statb)))
#endif



/* Signal identifiers */
enum
{
  STATUS_READY,
  LAST_SIGNAL,
};



static void     thunar_vfs_deep_count_job_class_init    (ThunarVfsJobClass      *klass);
static void     thunar_vfs_deep_count_job_finalize      (GObject                *object);
static void     thunar_vfs_deep_count_job_execute       (ThunarVfsJob           *job);
static gboolean thunar_vfs_deep_count_job_process       (ThunarVfsDeepCountJob  *deep_count_job,
                                                         const gchar            *dir_path,
                                                         struct stat            *statb);
static void     thunar_vfs_deep_count_job_status_ready  (ThunarVfsDeepCountJob  *deep_count_job);



struct _ThunarVfsDeepCountJobClass
{
  ThunarVfsJobClass __parent__;

  /* signals */
  void (*status_ready) (ThunarVfsJob *job,
                        guint64       total_size,
                        guint         file_count,
                        guint         directory_count,
                        guint         unreadable_directory_count);
};

struct _ThunarVfsDeepCountJob
{
  ThunarVfsJob    __parent__;

  gboolean        follow_links;
  ThunarVfsPath  *path;

  /* the time of the last "status-ready" emission */
  GTimeVal        last_time;

  /* status information */
  guint64         total_size;
  guint           file_count;
  guint           directory_count;
  guint           unreadable_directory_count;
};



static GObjectClass *thunar_vfs_deep_count_job_parent_class;
static guint         deep_count_signals[LAST_SIGNAL];



GType
thunar_vfs_deep_count_job_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = _thunar_vfs_g_type_register_simple (THUNAR_VFS_TYPE_JOB,
                                                 "ThunarVfsDeepCountJob",
                                                 sizeof (ThunarVfsDeepCountJobClass),
                                                 thunar_vfs_deep_count_job_class_init,
                                                 sizeof (ThunarVfsDeepCountJob),
                                                 NULL,
                                                 0);
    }

  return type;
}



static void
thunar_vfs_deep_count_job_class_init (ThunarVfsJobClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  thunar_vfs_deep_count_job_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_vfs_deep_count_job_finalize;

  klass->execute = thunar_vfs_deep_count_job_execute;

  /**
   * ThunarVfsDeepCountJob::status-ready:
   * @job                        : a #ThunarVfsJob.
   * @total_size                 : the total size in bytes.
   * @file_count                 : the number of files.
   * @directory_count            : the number of directories.
   * @unreadable_directory_count : the number of unreadable directories.
   *
   * Emitted by the @job to inform listeners about new status.
   **/
  deep_count_signals[STATUS_READY] =
    g_signal_new (I_("status-ready"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET (ThunarVfsDeepCountJobClass, status_ready),
                  NULL, NULL,
                  _thunar_vfs_marshal_VOID__UINT64_UINT_UINT_UINT,
                  G_TYPE_NONE, 4,
                  G_TYPE_UINT64,
                  G_TYPE_UINT,
                  G_TYPE_UINT,
                  G_TYPE_UINT);
}



static void
thunar_vfs_deep_count_job_finalize (GObject *object)
{
  ThunarVfsDeepCountJob *deep_count_job = THUNAR_VFS_DEEP_COUNT_JOB (object);

  /* release the base path */
  if (G_LIKELY (deep_count_job->path != NULL))
    thunar_vfs_path_unref (deep_count_job->path);

  (*G_OBJECT_CLASS (thunar_vfs_deep_count_job_parent_class)->finalize) (object);
}



static void
thunar_vfs_deep_count_job_execute (ThunarVfsJob *job)
{
  ThunarVfsDeepCountJob *deep_count_job = THUNAR_VFS_DEEP_COUNT_JOB (job);
  struct stat            statb;
  GError                *err = NULL;
  gchar                 *absolute_path;
  GList                 *path_list;
  GList                 *lp;

  /* check if we should count the trash root folder */
  if (G_UNLIKELY (_thunar_vfs_path_is_trash (deep_count_job->path)))
    {
      /* count the trash root folder as directory */
      deep_count_job->directory_count += 1;

      /* scan the trash root folder */
      path_list = _thunar_vfs_io_trash_scandir (deep_count_job->path, deep_count_job->follow_links, NULL, &err);
    }
  else
    {
      /* just count the single item */
      path_list = thunar_vfs_path_list_prepend (NULL, deep_count_job->path);
    }

  /* process all paths */
  for (lp = path_list; err == NULL && lp != NULL; lp = lp->next)
    {
      /* try to translate the path object to an absolute local path */
      absolute_path = _thunar_vfs_path_translate_dup_string (lp->data, THUNAR_VFS_PATH_SCHEME_FILE, &err);
      if (G_LIKELY (absolute_path != NULL))
        {
          /* try to stat the file (handle broken links properly) */
          if (g_stat (absolute_path, &statb) < 0 && g_lstat (absolute_path, &statb) < 0)
            {
              /* tell the listeners that the job failed */
              _thunar_vfs_set_g_error_from_errno3 (&err);
            }
          else if (!S_ISDIR (statb.st_mode))
            {
              /* the base path is not a directory */
              deep_count_job->total_size += statb.st_size;
              deep_count_job->file_count += 1;
            }
          else
            {
              /* process the directory recursively */
              if (!thunar_vfs_deep_count_job_process (deep_count_job, absolute_path, &statb))
                {
                  /* base directory not readable */
                  g_set_error (&err, G_FILE_ERROR, G_FILE_ERROR_IO, _("Failed to read folder contents"));
                }
            }

          /* release the base path */
          g_free (absolute_path);
        }
    }

  /* check if we had an error */
  if (G_UNLIKELY (err != NULL))
    {
      /* forward the error to the job owner */
      _thunar_vfs_job_error (job, err);
      g_error_free (err);
    }
  else
    {
      /* emit "status-ready" signal */
      _thunar_vfs_job_emit (THUNAR_VFS_JOB (deep_count_job), deep_count_signals[STATUS_READY],
                            0, deep_count_job->total_size, deep_count_job->file_count,
                            deep_count_job->directory_count, deep_count_job->unreadable_directory_count);
    }

  /* release the path_list */
  thunar_vfs_path_list_free (path_list);
}



static gboolean
thunar_vfs_deep_count_job_process (ThunarVfsDeepCountJob *deep_count_job,
                                   const gchar           *dir_path,
                                   struct stat           *statb)
{
  const gchar *name;
  gchar       *path;
  GDir        *dp;

  /* try to open the directory */
  dp = g_dir_open (dir_path, 0, NULL);
  if (G_LIKELY (dp == NULL))
    {
      /* unreadable directory */
      return FALSE;
    }

  /* process all items in this directory */
  while (!thunar_vfs_job_cancelled (THUNAR_VFS_JOB (deep_count_job)))
    {
      /* determine the next item */
      name = g_dir_read_name (dp);
      if (G_UNLIKELY (name == NULL))
        break;

      /* check if we should emit "status-ready" */
      thunar_vfs_deep_count_job_status_ready (deep_count_job);

      /* determine the full path to the item */
      path = g_build_filename (dir_path, name, NULL);

      /* stat the item */
      if (G_LIKELY (g_stat (path, statb) == 0))
        {
          /* add the size of this item */
          deep_count_job->total_size += statb->st_size;

          /* check if we have a directory here */
          if (S_ISDIR (statb->st_mode))
            {
              /* check if this is a symlink to a folder */
              if (g_lstat (path, statb) == 0 && (!S_ISLNK (statb->st_mode) || deep_count_job->follow_links))
                {
                  /* process the directory recursively */
                  if (thunar_vfs_deep_count_job_process (deep_count_job, path, statb))
                    {
                      /* directory was readable */
                      deep_count_job->directory_count += 1;
                    }
                  else
                    {
                      /* directory was unreadable */
                      deep_count_job->unreadable_directory_count += 1;
                    }
                }
              else
                {
                  /* count the symlink as file */
                  deep_count_job->file_count += 1;
                }
            }
          else
            {
              /* count it as file */
              deep_count_job->file_count += 1;
            }
        }
      else
        {
          /* check if we have a broken symlink here */
          if (g_lstat (path, statb) == 0 && S_ISLNK (statb->st_mode))
            {
              /* count the broken symlink as file */
              deep_count_job->total_size += statb->st_size;
              deep_count_job->file_count += 1;
            }
        }

      /* release the path */
      g_free (path);
    }

  /* close the dir handle */
  g_dir_close (dp);

  /* readable directory */
  return TRUE;
}



static void
thunar_vfs_deep_count_job_status_ready (ThunarVfsDeepCountJob *deep_count_job)
{
  GTimeVal current_time;

  /* check if we should update (at most every 128 files, but not more than fourth per second) */
  if (((deep_count_job->unreadable_directory_count + deep_count_job->directory_count + deep_count_job->file_count) % 128) == 0)
    {
      /* determine the current time */
      g_get_current_time (&current_time);

      /* check if more than 250ms elapsed since the last "status-ready" */
      if (((current_time.tv_sec - deep_count_job->last_time.tv_sec) * 1000ull
            + (current_time.tv_usec - deep_count_job->last_time.tv_usec) / 1000ull) >= 250ull)
        {
          /* remember the current time */
          deep_count_job->last_time = current_time;

          /* emit "status-ready" signal */
          _thunar_vfs_job_emit (THUNAR_VFS_JOB (deep_count_job), deep_count_signals[STATUS_READY],
                                0, deep_count_job->total_size, deep_count_job->file_count,
                                deep_count_job->directory_count, deep_count_job->unreadable_directory_count);
        }
    }
}



/**
 * thunar_vfs_deep_count_job_new:
 * @path  : a #ThunarVfsPath.
 * @flags : the #ThunarVfsDeepCountFlags which control the
 *          behavior of the returned job.
 *
 * Allocates a new #ThunarVfsDeepCountJob, which counts
 * the size of the file at @path or if @path is a directory
 * counts the size of all items in the directory and its
 * subdirectories.
 *
 * The caller is responsible to free the returned job
 * using g_object_unref() when no longer needed.
 *
 * Return value: the newly allocated #ThunarVfsDeepCountJob.
 **/
ThunarVfsJob*
thunar_vfs_deep_count_job_new (ThunarVfsPath          *path,
                               ThunarVfsDeepCountFlags flags)
{
  ThunarVfsDeepCountJob *deep_count_job;

  g_return_val_if_fail (path != NULL, NULL);

  /* allocate the new job */
  deep_count_job = g_object_new (THUNAR_VFS_TYPE_DEEP_COUNT_JOB, NULL);
  deep_count_job->path = thunar_vfs_path_ref (path);
  deep_count_job->follow_links = (flags & THUNAR_VFS_DEEP_COUNT_FLAGS_FOLLOW_SYMLINKS);

  return THUNAR_VFS_JOB (deep_count_job);
}



#define __THUNAR_VFS_DEEP_COUNT_JOB_C__
#include <thunar-vfs/thunar-vfs-aliasdef.c>
