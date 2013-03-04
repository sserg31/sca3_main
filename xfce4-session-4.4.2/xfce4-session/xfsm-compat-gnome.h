/* $Id: xfsm-compat-gnome.h 20833 2006-04-09 18:08:17Z benny $ */
/*-
 * Copyright (c) 2004 Benedikt Meurer <benny@xfce.org>
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
 *
 * Most parts of this file where taken from gnome-session.
 */

#ifndef __XFSM_COMPAT_GNOME_H__
#define __XFSM_COMPAT_GNOME_H__

#include <xfce4-session/xfsm-splash-screen.h>

void xfsm_compat_gnome_startup (XfsmSplashScreen *splash);
void xfsm_compat_gnome_shutdown (void);

#endif /* !__XFSM_COMPAT_GNOME_H__ */
