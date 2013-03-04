/* $Id: xfsm-global.h 20833 2006-04-09 18:08:17Z benny $ */
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

#ifndef __XFSM_GLOBAL_H__
#define __XFSM_GLOBAL_H__

#include <xfce4-session/xfsm-splash-screen.h>


typedef struct _FailsafeClient FailsafeClient;
struct _FailsafeClient
{
  gchar    **command;
  GdkScreen *screen;
};
  

#define DEFAULT_SESSION_NAME "Default"


extern gboolean          verbose;
extern gboolean          compat_kde;
extern gboolean          compat_gnome;
extern GList            *starting_properties;
extern GList            *pending_properties;
extern GList            *restart_properties;
extern GList            *running_clients;
extern gchar            *session_name;
extern gchar            *session_file;
extern GList            *failsafe_clients;
extern gboolean          failsafe_mode;
extern gint              shutdown_type;
extern XfsmSplashScreen *splash_screen;


#if defined(G_HAVE_ISO_VARARGS)

#define xfsm_verbose(...)             \
G_STMT_START{                         \
  if (G_UNLIKELY (verbose))           \
    xfsm_verbose_real (__VA_ARGS__);  \
}G_STMT_END

#elif defined(G_HAVE_GNUC_VARARGS)

#define xfsm_verbose(format, ...)     \
G_STMT_START{                         \
  if (G_UNLIKELY (verbose))           \
    xfsm_verbose_real ( ## format);   \
}G_STMT_END
  
#endif

void xfsm_enable_verbose (void);
void xfsm_verbose_real (const gchar *format, ...) G_GNUC_PRINTF (1, 2);


#endif /* !__XFSM_GLOBAL_H__ */
