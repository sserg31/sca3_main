/* $Id: tvm-input-device.h 2340 2007-01-11 23:11:32Z benny $ */
/*-
 * Copyright (c) 2007 Benedikt Meurer <benny@xfce.org>.
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

#ifndef __TVM_INPUT_DEVICE_H__
#define __TVM_INPUT_DEVICE_H__

#include <thunar-volman/tvm-device.h>

G_BEGIN_DECLS;

gboolean tvm_input_device_added (TvmPreferences *preferences,
                                 LibHalContext  *context,
                                 const gchar    *udi,
                                 const gchar    *capability,
                                 GError        **error) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__TVM_INPUT_DEVICE_H__ */
