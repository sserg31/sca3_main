/* $Id: tvm-pango-extensions.h 2340 2007-01-11 23:11:32Z benny $ */
/*-
 * Copyright (c) 2006-2007 Benedikt Meurer <benny@xfce.org>
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

#ifndef __TVM_PANGO_EXTENSIONS_H__
#define __TVM_PANGO_EXTENSIONS_H__

#include <pango/pango.h>

G_BEGIN_DECLS;

PangoAttrList *tvm_pango_attr_list_big               (void) G_GNUC_CONST G_GNUC_INTERNAL;
PangoAttrList *tvm_pango_attr_list_big_bold          (void) G_GNUC_CONST G_GNUC_INTERNAL;
PangoAttrList *tvm_pango_attr_list_bold              (void) G_GNUC_CONST G_GNUC_INTERNAL;
PangoAttrList *tvm_pango_attr_list_italic            (void) G_GNUC_CONST G_GNUC_INTERNAL;
PangoAttrList *tvm_pango_attr_list_small_italic      (void) G_GNUC_CONST G_GNUC_INTERNAL;
PangoAttrList *tvm_pango_attr_list_underline_single  (void) G_GNUC_CONST G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__TVM_PANGO_EXTENSIONS_H__ */
