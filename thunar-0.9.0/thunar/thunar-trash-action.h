/* $Id: thunar-trash-action.h 26421 2007-12-02 13:46:28Z benny $ */
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

#ifndef __THUNAR_TRASH_ACTION_H__
#define __THUNAR_TRASH_ACTION_H__

#include <exo/exo.h>

G_BEGIN_DECLS;

typedef struct _ThunarTrashActionClass ThunarTrashActionClass;
typedef struct _ThunarTrashAction      ThunarTrashAction;

#define THUNAR_TYPE_TRASH_ACTION            (thunar_trash_action_get_type ())
#define THUNAR_TRASH_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), THUNAR_TYPE_TRASH_ACTION, ThunarTrashAction))
#define THUNAR_TRASH_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), THUNAR_TYPE_TRASH_ACTION, ThunarTrashActionClass))
#define THUNAR_IS_TRASH_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THUNAR_TYPE_TRASH_ACTION))
#define THUNAR_IS_TRASH_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), THUNAR_TYPE_TRASH_ACTION))
#define THUNAR_TRASH_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), THUNAR_TYPE_TRASH_ACTION, ThunarTrashActionClass))

GType      thunar_trash_action_get_type (void) G_GNUC_CONST;

GtkAction *thunar_trash_action_new      (void) G_GNUC_MALLOC;

G_END_DECLS;

#endif /* !__THUNAR_TRASH_ACTION_H__ */
