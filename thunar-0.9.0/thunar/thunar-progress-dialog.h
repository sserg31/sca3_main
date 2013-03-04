/* $Id: thunar-progress-dialog.h 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
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

#ifndef __THUNAR_PROGRESS_DIALOG_H__
#define __THUNAR_PROGRESS_DIALOG_H__

#include <thunar-vfs/thunar-vfs.h>

G_BEGIN_DECLS;

typedef struct _ThunarProgressDialogClass ThunarProgressDialogClass;
typedef struct _ThunarProgressDialog      ThunarProgressDialog;

#define THUNAR_TYPE_PROGRESS_DIALOG             (thunar_progress_dialog_get_type ())
#define THUNAR_PROGRESS_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), THUNAR_TYPE_PROGRESS_DIALOG, ThunarProgressDialog))
#define THUNAR_PROGRESS_DIALOG_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), THUNAR_TYPE_PROGRESS_DIALOG, ThunarProgressDialogClass))
#define THUNAR_IS_PROGRESS_DIALOG(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THUNAR_TYPE_PROGRESS_DIALOG))
#define THUNAR_IS_PROGRESS_DIALOG_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), THUNAR_TYPE_PROGRESS_DIALOG))
#define THUNAR_PROGRESS_DIALOG_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), THUNAR_TYPE_PROGRESS_DIALOG, ThunarProgressDialogClass))

GType         thunar_progress_dialog_get_type      (void) G_GNUC_CONST;

GtkWidget    *thunar_progress_dialog_new           (void) G_GNUC_MALLOC;
GtkWidget    *thunar_progress_dialog_new_with_job  (ThunarVfsJob         *job) G_GNUC_MALLOC;

ThunarVfsJob *thunar_progress_dialog_get_job       (ThunarProgressDialog *dialog);
void          thunar_progress_dialog_set_job       (ThunarProgressDialog *dialog,
                                                    ThunarVfsJob         *job);

G_END_DECLS;

#endif /* !__THUNAR_PROGRESS_DIALOG_H__ */
