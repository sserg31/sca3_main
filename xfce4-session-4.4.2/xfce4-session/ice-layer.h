/* $Id: ice-layer.h 20833 2006-04-09 18:08:17Z benny $ */
/*-
 * Copyright (c) 2003-2004 Benedikt Meurer <benny@xfce.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *                                                                              
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *                                                                              
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef __XFSM_ICE_LAYER_H__
#define __XFSM_ICE_LAYER_H__

#include <X11/ICE/ICElib.h>

#include <glib.h>

Bool     ice_auth_proc       (char         *hostname);
gboolean ice_setup_listeners (int           num_listeners,
                              IceListenObj *listen_objs);
void     ice_cleanup         (void);

#endif	/* !__XFSM_ICE_LAYER_H__ */
