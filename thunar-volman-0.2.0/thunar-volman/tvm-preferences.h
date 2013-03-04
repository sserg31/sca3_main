/* $Id: tvm-preferences.h 2340 2007-01-11 23:11:32Z benny $ */
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

#ifndef __TVM_PREFERENCES_H__
#define __TVM_PREFERENCES_H__

#include <exo/exo.h>

G_BEGIN_DECLS;

typedef struct _TvmPreferencesClass TvmPreferencesClass;
typedef struct _TvmPreferences      TvmPreferences;

#define TVM_TYPE_PREFERENCES             (tvm_preferences_get_type ())
#define TVM_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TVM_TYPE_PREFERENCES, TvmPreferences))
#define TVM_PREFERENCES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), TVM_TYPE_PREFERENCES, TvmPreferencesClass))
#define TVM_IS_PREFERENCES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TVM_TYPE_PREFERENCES))
#define TVM_IS_PREFERENCES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), TVM_TYPE_PREFERENCES))
#define TVM_PREFERENCES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), TVM_TYPE_PREFERENCES, TvmPreferencesClass))

GType           tvm_preferences_get_type (void) G_GNUC_CONST;

TvmPreferences *tvm_preferences_get      (void);

G_END_DECLS;

#endif /* !__TVM_PREFERENCES_H__ */
