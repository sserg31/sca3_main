/* $Id: xfsm-legacy.c 22459 2006-07-16 08:17:54Z benny $ */
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
 *
 * Most of the code in this file is borred from ksmserver, the KDE session
 * management server.
 * Copyright (C) 2000 Matthias Ettrich <ettrich@kde.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* After all, this code is mostly a hack now, one should cleanup the
 * stuff before 4.2!
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <gdk/gdkx.h>

#include <libxfcegui4/libxfcegui4.h>

#include <libxfsm/xfsm-util.h>


#ifdef LEGACY_SESSION_MANAGEMENT

#define WM_SAVE_YOURSELF_TIMEOUT  4000

enum
{
  SM_ERROR,
  SM_WMCOMMAND,
  SM_WMSAVEYOURSELF,
};


typedef struct _SmWindow SmWindow;
struct _SmWindow
{
  gint    type;
  gchar **wm_command;
  gchar  *wm_client_machine;
  gchar  *wm_class1;
  gchar  *wm_class2;
  Window  wid;
  gint    screen_num;
};
#define SM_WINDOW(window) ((SmWindow *) (window))

typedef struct
{
  gint screen_num;
  gchar **command;
} SmRestartApp;
#define SM_RESTART_APP(a) ((SmRestartApp *) (a))


static GList *restart_apps = NULL;
static GList *window_list = NULL;


/* X Atoms */
static Atom _XA_WM_PROTOCOLS     = None;
static Atom _XA_WM_SAVE_YOURSELF = None;
static Atom _XA_WM_CLIENT_LEADER = None;
static Atom _XA_SM_CLIENT_ID     = None;


static SmWindow*
sm_window_new (Window wid, gint screen_num, gint type,
               gchar *wm_class1, gchar *wm_class2)
{
  SmWindow *smw;

  smw = g_new0 (SmWindow, 1);
  smw->wid = wid;
  smw->type = type;
  smw->wm_class1 = wm_class1;
  smw->wm_class2 = wm_class2;
  smw->screen_num = screen_num;

  return smw;
}


static void
sm_window_free (SmWindow *window)
{
  g_return_if_fail (window != NULL);

  if (window->wm_command != NULL)
    g_strfreev (window->wm_command);
  if (window->wm_client_machine != NULL)
    g_free (window->wm_client_machine);
  if (window->wm_class1)
    g_free (window->wm_class1);
  if (window->wm_class2)
    g_free (window->wm_class2);
  g_free (window);
}


static void
sm_window_list_clear (void)
{
  GList *lp;

  for (lp = window_list; lp != NULL; lp = lp->next)
    sm_window_free (SM_WINDOW (lp->data));

  g_list_free (window_list);
  window_list = NULL;
}


static gboolean
sm_window_list_contains (Window window)
{
  GList *lp;

  for (lp = window_list; lp != NULL; lp = lp->next)
    if (SM_WINDOW (lp->data)->wid == window)
      return TRUE;

  return FALSE;
}


static int
wins_error_handler (Display *display, XErrorEvent *event)
{
  GList *lp;
  for (lp = window_list; lp != NULL; lp = lp->next)
    if (SM_WINDOW (lp->data)->wid == event->resourceid)
      SM_WINDOW (lp->data)->type = SM_ERROR;
  return 0;
}


static gboolean
has_xsmp_support (Window window)
{
  XTextProperty tp;
  gboolean has_it = FALSE;

  if (XGetTextProperty (gdk_display, window, &tp, _XA_SM_CLIENT_ID))
    {
      if (tp.encoding == XA_STRING && tp.format == 8 && tp.nitems != 0)
        has_it = TRUE;

      if (tp.value != NULL)
        XFree ((char *) tp.value);
    }

  return has_it;
}


static gchar**
get_wmcommand (Window window)
{
  gchar **result = NULL;
  Status status;
  char **argv;
  int argc, i;

  gdk_error_trap_push ();
  status = XGetCommand (gdk_display, window, &argv, &argc);
  if (gdk_error_trap_pop ())
    return NULL;

  if (status && argv && argc > 0)
    {
      result = g_new0 (gchar *, argc + 1);
      for (i = 0; i < argc; ++i)
        result[i] = g_strdup (argv[i]);
      XFreeStringList (argv);
    }

  return result;
}


static gchar*
get_wmclientmachine (Window window)
{
  XTextProperty tp;
  gchar *result = NULL;
  Status status;

  gdk_error_trap_push ();
  status = XGetWMClientMachine (gdk_display, window, &tp);
  if (gdk_error_trap_pop ())
    return NULL;
  
  if (status)
    {
      if (tp.encoding == XA_STRING && tp.format == 8 && tp.nitems != 0)
        result = g_strdup ((char *) tp.value);

      if (tp.value != NULL)
        XFree ((char *) tp.value);
    }

  if (result == NULL)
    result = g_strdup ("localhost");

  return result;
}

static Window
get_clientleader (Window window)
{
  Atom type;
  int format, status;
  unsigned long nitems = 0;
  unsigned long extra = 0;
  unsigned char *data = 0;
  Window result = window;

  status = XGetWindowProperty (gdk_display, window, _XA_WM_CLIENT_LEADER,
                               0, 10000, FALSE, XA_WINDOW, &type, &format,
                               &nitems, &extra, &data);
  if (status  == Success)
    {
      if (data != NULL && nitems > 0)
          result = *((Window *) data);
      XFree(data);
    }

  return result;
}

#endif


void
xfsm_legacy_perform_session_save (void)
{
#ifdef LEGACY_SESSION_MANAGEMENT
  XErrorHandler old_handler;
  NetkScreen *screen;
  GList *windows;
  GList *lp;
  Window leader;
  Display *display;
  Atom *protocols;
  int nprotocols;
  XClassHint class_hint;
  SmWindow *sm_window;
  int n, i;
  int type;
  gchar *wmclass1;
  gchar *wmclass2;
  Window root, window;
  XEvent ev;
  int awaiting_replies = 0;
  GTimer *timer;

  /* clear window list */
  sm_window_list_clear ();

  /* query X atoms */
  if (_XA_WM_SAVE_YOURSELF == None)
    {
      _XA_SM_CLIENT_ID = XInternAtom (gdk_display, "SM_CLIENT_ID", False);
      _XA_WM_PROTOCOLS = XInternAtom (gdk_display, "WM_PROTOCOLS", False);
      _XA_WM_SAVE_YOURSELF = XInternAtom (gdk_display, "WM_SAVE_YOURSELF",
                                          False);
      _XA_WM_CLIENT_LEADER = XInternAtom (gdk_display, "WM_CLIENT_LEADER",
                                          False);
    }

  /* install custom X error handler */
  old_handler = XSetErrorHandler (wins_error_handler);

  /* query mapped windows on all screens */
  for (n = 0; n < ScreenCount (gdk_display); ++n)
    {
      screen = netk_screen_get (n);
      netk_screen_force_update (screen);

      windows = netk_screen_get_windows (screen);

      for (lp = windows; lp != NULL; lp = lp->next)
        {
          window = netk_window_get_xid (NETK_WINDOW (lp->data));
          leader = get_clientleader (window);
          if (leader == None || sm_window_list_contains (leader)
              || has_xsmp_support (window) || has_xsmp_support (leader))
            continue;

          type = SM_WMCOMMAND;
          wmclass1 = NULL;
          wmclass2 = NULL;

          nprotocols = 0;
          protocols = NULL;

          if (XGetWMProtocols (gdk_display, leader, &protocols, &nprotocols))
            {
              for (i = 0; i < nprotocols; ++i)
                if (protocols[i] == _XA_WM_SAVE_YOURSELF)
                  {
                    type = SM_WMSAVEYOURSELF;
                    break;
                  }
              XFree ((void *) protocols);
            }

          if (XGetClassHint (gdk_display, leader, &class_hint))
            {
              wmclass2 = g_strdup (class_hint.res_class);
              wmclass1 = g_strdup (class_hint.res_name);
              XFree (class_hint.res_class);
              XFree (class_hint.res_name);
            }

          sm_window = sm_window_new (leader, n, type, wmclass1, wmclass2);
          window_list = g_list_append (window_list, sm_window);
        }
    }

  /* open fresh display for sending WM_SAVE_YOURSELF commands */
  XSync (gdk_display, False);
  display = XOpenDisplay (DisplayString (gdk_display));
  if (display == NULL)
    {
      XSetErrorHandler (old_handler);
      return;
    }

  /* grab keyboard/pointer (XXX - check pointer pos first?) */
  root = DefaultRootWindow (display);
  XGrabKeyboard (display, root, False, GrabModeAsync, GrabModeAsync,
                 CurrentTime);
  XGrabPointer (display, root, False, Button1Mask | Button2Mask | Button3Mask,
                GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  for (lp = window_list; lp != NULL; lp = lp->next)
    {
      sm_window = SM_WINDOW (lp->data);
      if (sm_window->type == SM_WMSAVEYOURSELF)
        {
          ++awaiting_replies;

          bzero (&ev, sizeof (ev));

          ev.xclient.type = ClientMessage;
          ev.xclient.window = sm_window->wid;
          ev.xclient.message_type = _XA_WM_PROTOCOLS;
          ev.xclient.format = 32;
          ev.xclient.data.l[0] = _XA_WM_SAVE_YOURSELF;
          ev.xclient.data.l[1] = CurrentTime;
          XSelectInput (display, sm_window->wid,
                        PropertyChangeMask | StructureNotifyMask);
          XSendEvent (display, sm_window->wid, False, 0, &ev);
        }
    }

  /* wait for change in WM_COMMAND with timeout */
  XFlush (display);
  timer = g_timer_new ();
  while (awaiting_replies > 0)
    {
      if (XPending (display))
        {
          XNextEvent (display, &ev);
          if (ev.type == UnmapNotify || (ev.type == PropertyNotify
                && ev.xproperty.atom == XA_WM_COMMAND))
            {
              for (lp = window_list; lp != NULL; lp = lp->next)
                {
                  if (SM_WINDOW (lp->data)->wid == ev.xany.window
                      && SM_WINDOW (lp->data)->type != SM_WMCOMMAND)
                    {
                      --awaiting_replies;
                      if (SM_WINDOW (lp->data)->type != SM_ERROR)
                        SM_WINDOW (lp->data)->type = SM_WMCOMMAND;
                    }
                }
            }
        }
      else
        {
          struct timeval tv;
          fd_set fds;
          int msecs;
          int fd;

          msecs = (int)(g_timer_elapsed (timer, NULL) * 1000);
          if (msecs >= WM_SAVE_YOURSELF_TIMEOUT)
            break;

          fd = ConnectionNumber (display);
          FD_ZERO (&fds);
          FD_SET (fd, &fds);
          tv.tv_sec = (WM_SAVE_YOURSELF_TIMEOUT - msecs) / 1000;
          tv.tv_usec = ((WM_SAVE_YOURSELF_TIMEOUT - msecs) % 1000) * 1000;
          select (fd + 1, &fds, NULL, &fds, &tv);
        }
    }
  g_timer_destroy (timer);

  /* Terminate work in new display */
  XAllowEvents (display, ReplayPointer, CurrentTime);
  XAllowEvents (display, ReplayKeyboard, CurrentTime);
  XSync (display, False);
  XCloseDisplay (display);
  XSetErrorHandler (old_handler);

  for (lp = window_list; lp != NULL; lp = lp->next)
    {
      sm_window = SM_WINDOW (lp->data);
      if (sm_window->type != SM_ERROR)
        {
          sm_window->wm_command = get_wmcommand (sm_window->wid);
          sm_window->wm_client_machine = get_wmclientmachine (sm_window->wid);
          if (sm_window->wm_command == NULL || sm_window->wm_client_machine == NULL)
            sm_window->type = SM_ERROR;
        }
    }
#endif
}


void
xfsm_legacy_store_session (XfceRc *rc)
{
#ifdef LEGACY_SESSION_MANAGEMENT
  int count = 0;
  SmWindow *sm_window;
  GList *lp;
  gchar buffer[256];

  for (lp = window_list; lp != NULL; lp = lp->next)
    {
      sm_window = SM_WINDOW (lp->data);
      if (sm_window->type != SM_ERROR)
        {
          /* xmms is b0rked, surprise, surprise */
          if ((sm_window->wm_class1 != NULL
                && strcmp (sm_window->wm_class1, "xmms") == 0)
              || (sm_window->wm_class2 != NULL
                && strcmp (sm_window->wm_class2, "xmms") == 0))
            continue;

          if (sm_window->wm_command == NULL
              || sm_window->wm_client_machine == NULL)
            continue;

          g_snprintf (buffer, 256, "Legacy%d_Screen", count);
          xfce_rc_write_int_entry (rc, buffer, sm_window->screen_num);

          g_snprintf (buffer, 256, "Legacy%d_Command", count);
          xfce_rc_write_list_entry (rc, buffer, sm_window->wm_command, NULL);

          g_snprintf (buffer, 256, "Legacy%d_ClientMachine", count);
          xfce_rc_write_entry (rc, buffer, sm_window->wm_client_machine);

          ++count;
        }
    }

  xfce_rc_write_int_entry (rc, "LegacyCount", count);
#endif
}


void
xfsm_legacy_load_session (XfceRc *rc)
{
#ifdef LEGACY_SESSION_MANAGEMENT
  gchar buffer[256];
  int count;
  int i;
  gchar **command;
  SmRestartApp *app;
  int screen_num;

  count = xfce_rc_read_int_entry (rc, "LegacyCount", 0);
  for (i = 0; i < count; ++i)
    {
      g_snprintf (buffer, 256, "Legacy%d_Screen", i);
      screen_num = xfce_rc_read_int_entry (rc, buffer, 0);

      g_snprintf (buffer, 256, "Legacy%d_Command", i);
      command = xfce_rc_read_list_entry (rc, buffer, NULL);
      if (command == NULL)
        continue;

      app = g_new0 (SmRestartApp, 1);
      app->screen_num = screen_num;
      app->command = command;

      restart_apps = g_list_append (restart_apps, app);
    }
#endif
}


void
xfsm_legacy_init (void)
{
#ifdef LEGACY_SESSION_MANAGEMENT
  Atom dt_save_mode;
  Atom dt_restore_mode;
  Display *dpy;
  Window root;
  int n;

  dpy = gdk_display;

  /* Some CDE apps are broken (thanks again to Craig for the Sun Box :).
   * Bugfix found on http://bugzilla.gnome.org/long_list.cgi?buglist=81343
   * and implemented in gnome-session. The following code extends what
   * gnome does, since it seems to be necessary to set both properties
   * per screen, not just for the first screen. Anybody with access to
   * CDE source code to enlighten me?
   *                                          -- bm, 20040530
   */
  dt_save_mode = XInternAtom (dpy, "_DT_SAVE_MODE", False);
  dt_restore_mode = XInternAtom (dpy, "_DT_RESTORE_MODE", False);
  for (n = 0; n < ScreenCount (dpy); ++n)
    {
      root = RootWindow (dpy, n);
      XChangeProperty (dpy, root, dt_save_mode, XA_STRING, 8,
                       PropModeReplace, (unsigned char *)"xfce4", sizeof ("xfce4"));
      XChangeProperty (dpy, root, dt_restore_mode, XA_STRING, 8,
                       PropModeReplace, (unsigned char *)"xfce4", sizeof ("xfce4"));
    }
#endif
}


void
xfsm_legacy_startup (void)
{
#ifdef LEGACY_SESSION_MANAGEMENT
  GdkScreen *screen;
  GList *lp;

  for (lp = restart_apps; lp != NULL; lp = lp->next)
    {
      screen = gdk_display_get_screen (gdk_display_get_default (),
                                       SM_RESTART_APP (lp->data)->screen_num);
      xfsm_start_application (SM_RESTART_APP (lp->data)->command, NULL,
                              screen, NULL, NULL, NULL);
      g_strfreev (SM_RESTART_APP (lp->data)->command);
      g_free (lp->data);
    }

  g_list_free (restart_apps);
  restart_apps = NULL;
#endif
}


void
xfsm_legacy_shutdown (void)
{
#ifdef LEGACY_SESSION_MANAGEMENT
  SmWindow *sm_window;
  GList *lp;

  gdk_error_trap_push ();

  /* kill 'em all! */
  for (lp = window_list; lp != NULL; lp = lp->next)
    {
      sm_window = SM_WINDOW (lp->data);
      if (sm_window->wid != None)
        XKillClient (gdk_display, sm_window->wid);
    }

  sm_window_list_clear ();

  gdk_flush ();

  gdk_error_trap_pop ();
#endif
}



