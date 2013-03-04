/* $Id: thunar-window.c 26421 2007-12-02 13:46:28Z benny $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <thunar/thunar-application.h>
#include <thunar/thunar-clipboard-manager.h>
#include <thunar/thunar-compact-view.h>
#include <thunar/thunar-details-view.h>
#include <thunar/thunar-dialogs.h>
#include <thunar/thunar-shortcuts-pane.h>
#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-gtk-extensions.h>
#include <thunar/thunar-history.h>
#include <thunar/thunar-icon-view.h>
#include <thunar/thunar-launcher.h>
#include <thunar/thunar-location-buttons.h>
#include <thunar/thunar-location-dialog.h>
#include <thunar/thunar-location-entry.h>
#include <thunar/thunar-marshal.h>
#include <thunar/thunar-pango-extensions.h>
#include <thunar/thunar-preferences-dialog.h>
#include <thunar/thunar-preferences.h>
#include <thunar/thunar-private.h>
#include <thunar/thunar-statusbar.h>
#include <thunar/thunar-stock.h>
#include <thunar/thunar-throbber.h>
#include <thunar/thunar-trash-action.h>
#include <thunar/thunar-tree-pane.h>
#include <thunar/thunar-window.h>
#include <thunar/thunar-window-ui.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_CURRENT_DIRECTORY,
  PROP_SHOW_HIDDEN,
  PROP_UI_MANAGER,
  PROP_ZOOM_LEVEL,
};

/* Signal identifiers */
enum
{
  BACK,
  RELOAD,
  TOGGLE_SIDEPANE,
  ZOOM_IN,
  ZOOM_OUT,
  ZOOM_RESET,
  LAST_SIGNAL,
};



static void     thunar_window_class_init                  (ThunarWindowClass      *klass);
static void     thunar_window_init                        (ThunarWindow           *window);
static void     thunar_window_dispose                     (GObject                *object);
static void     thunar_window_finalize                    (GObject                *object);
static void     thunar_window_get_property                (GObject                *object,
                                                           guint                   prop_id,
                                                           GValue                 *value,
                                                           GParamSpec             *pspec);
static void     thunar_window_set_property                (GObject                *object,
                                                           guint                   prop_id,
                                                           const GValue           *value,
                                                           GParamSpec             *pspec);
static gboolean thunar_window_back                        (ThunarWindow           *window);
static gboolean thunar_window_reload                      (ThunarWindow           *window);
static gboolean thunar_window_toggle_sidepane             (ThunarWindow           *window);
static gboolean thunar_window_zoom_in                     (ThunarWindow           *window);
static gboolean thunar_window_zoom_out                    (ThunarWindow           *window);
static gboolean thunar_window_zoom_reset                  (ThunarWindow           *window);
static void     thunar_window_realize                     (GtkWidget              *widget);
static void     thunar_window_unrealize                   (GtkWidget              *widget);
static gboolean thunar_window_configure_event             (GtkWidget              *widget,
                                                           GdkEventConfigure      *event);
static void     thunar_window_merge_custom_preferences    (ThunarWindow           *window);
static void     thunar_window_install_location_bar        (ThunarWindow           *window,
                                                           GType                   type);
static void     thunar_window_install_sidepane            (ThunarWindow           *window,
                                                           GType                   type);
static void     thunar_window_start_open_location         (ThunarWindow           *window,
                                                           const gchar            *initial_text);
static void     thunar_window_action_open_new_window      (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_empty_trash          (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_close_all_windows    (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_close                (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_preferences          (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_reload               (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_pathbar_changed      (GtkToggleAction        *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_toolbar_changed      (GtkToggleAction        *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_shortcuts_changed    (GtkToggleAction        *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_tree_changed         (GtkToggleAction        *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_statusbar_changed    (GtkToggleAction        *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_toolbar_changed      (GtkToggleAction        *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_zoom_in              (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_zoom_out             (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_zoom_reset           (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_view_changed         (GtkRadioAction         *action,
                                                           GtkRadioAction         *current,
                                                           ThunarWindow           *window);
static void     thunar_window_action_go_up                (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_open_home            (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_open_templates       (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_open_trash           (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_open_location        (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_contents             (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_about                (GtkAction              *action,
                                                           ThunarWindow           *window);
static void     thunar_window_action_show_hidden          (GtkToggleAction        *action,
                                                           ThunarWindow           *window);
static void     thunar_window_current_directory_changed   (ThunarFile             *current_directory,
                                                           ThunarWindow           *window);
static void     thunar_window_current_directory_destroy   (ThunarFile             *current_directory,
                                                           ThunarWindow           *window);
static void     thunar_window_connect_proxy               (GtkUIManager           *manager,
                                                           GtkAction              *action,
                                                           GtkWidget              *proxy,
                                                           ThunarWindow           *window);
static void     thunar_window_disconnect_proxy            (GtkUIManager           *manager,
                                                           GtkAction              *action,
                                                           GtkWidget              *proxy,
                                                           ThunarWindow           *window);
static void     thunar_window_menu_item_selected          (GtkWidget              *menu_item,
                                                           ThunarWindow           *window);
static void     thunar_window_menu_item_deselected        (GtkWidget              *menu_item,
                                                           ThunarWindow           *window);
static void     thunar_window_notify_loading              (ThunarView             *view,
                                                           GParamSpec             *pspec,
                                                           ThunarWindow           *window);
static void     thunar_window_volume_pre_unmount          (ThunarVfsVolumeManager *volume_manager,
                                                           ThunarVfsVolume        *volume,
                                                           ThunarWindow           *window);
static gboolean thunar_window_merge_idle                  (gpointer                user_data);
static void     thunar_window_merge_idle_destroy          (gpointer                user_data);
static gboolean thunar_window_save_geometry_timer         (gpointer                user_data);
static void     thunar_window_save_geometry_timer_destroy (gpointer                user_data);



struct _ThunarWindowClass
{
  GtkWindowClass __parent__;

  /* internal action signals */
  gboolean (*back)            (ThunarWindow *window);
  gboolean (*reload)          (ThunarWindow *window);
  gboolean (*toggle_sidepane) (ThunarWindow *window);
  gboolean (*zoom_in)         (ThunarWindow *window);
  gboolean (*zoom_out)        (ThunarWindow *window);
  gboolean (*zoom_reset)      (ThunarWindow *window);
};

struct _ThunarWindow
{
  GtkWindow __parent__;

  /* support for custom preferences actions */
  ThunarxProviderFactory *provider_factory;
  gint                    custom_preferences_merge_id;

  ThunarClipboardManager *clipboard;

  ThunarPreferences      *preferences;

  ThunarIconFactory      *icon_factory;

  GtkActionGroup         *action_group;
  GtkUIManager           *ui_manager;

  /* to be able to change folder on "volume-pre-unmount" if required */
  ThunarVfsVolumeManager *volume_manager;

  /* closures for the menu_item_selected()/menu_item_deselected() callbacks */
  GClosure               *menu_item_selected_closure;
  GClosure               *menu_item_deselected_closure;

  GtkWidget              *table;
  GtkWidget              *throbber;
  GtkWidget              *paned;
  GtkWidget              *sidepane;
  GtkWidget              *view_box;
  GtkWidget              *view;
  GtkWidget              *statusbar;

  /* support for two different styles of location bars */
  GtkWidget              *location_bar;
  GtkWidget              *location_toolbar;

  ThunarHistory          *history;
  ThunarLauncher         *launcher;

  ThunarFile             *current_directory;

  /* zoom-level support */
  ThunarZoomLevel         zoom_level;

  /* menu merge idle source */
  gint                    merge_idle_id;

  /* support to remember window geometry */
  gint                    save_geometry_timer_id;

  /* scroll_to_file support */
  GHashTable             *scroll_to_files;

  /* support to toggle side pane using F9,
   * see the toggle_sidepane() function.
   */
  GType                   toggle_sidepane_type;
};



static const GtkActionEntry action_entries[] =
{
  { "file-menu", NULL, N_ ("_File"), NULL, },
  { "open-new-window", NULL, N_ ("Open New _Window"), "<control>N", N_ ("Open a new Thunar window for the displayed location"), G_CALLBACK (thunar_window_action_open_new_window), },
  { "sendto-menu", NULL, N_ ("_Send To"), NULL, },
  { "empty-trash", NULL, N_ ("_Empty Trash"), NULL, N_ ("Delete all files and folders in the Trash"), G_CALLBACK (thunar_window_action_empty_trash), },
  { "close-all-windows", NULL, N_ ("Close _All Windows"), "<control><shift>W", N_ ("Close all Thunar windows"), G_CALLBACK (thunar_window_action_close_all_windows), },
  { "close", GTK_STOCK_CLOSE, N_ ("_Close"), "<control>W", N_ ("Close this window"), G_CALLBACK (thunar_window_action_close), },
  { "edit-menu", NULL, N_ ("_Edit"), NULL, },
  { "preferences", GTK_STOCK_PREFERENCES, N_ ("Pr_eferences..."), NULL, N_ ("Edit Thunars Preferences"), G_CALLBACK (thunar_window_action_preferences), },
  { "view-menu", NULL, N_ ("_View"), NULL, },
  { "reload", GTK_STOCK_REFRESH, N_ ("_Reload"), "<control>R", N_ ("Reload the current folder"), G_CALLBACK (thunar_window_action_reload), },
  { "view-location-selector-menu", NULL, N_ ("_Location Selector"), NULL, },
  { "view-side-pane-menu", NULL, N_ ("_Side Pane"), NULL, },
  { "zoom-in", GTK_STOCK_ZOOM_IN, N_ ("Zoom I_n"), "<control>plus", N_ ("Show the contents in more detail"), G_CALLBACK (thunar_window_action_zoom_in), },
  { "zoom-out", GTK_STOCK_ZOOM_OUT, N_ ("Zoom _Out"), "<control>minus", N_ ("Show the contents in less detail"), G_CALLBACK (thunar_window_action_zoom_out), },
  { "zoom-reset", GTK_STOCK_ZOOM_100, N_ ("Normal Si_ze"), "<control>0", N_ ("Show the contents at the normal size"), G_CALLBACK (thunar_window_action_zoom_reset), },
  { "go-menu", NULL, N_ ("_Go"), NULL, },
  { "open-parent", GTK_STOCK_GO_UP, N_ ("Open _Parent"), "<alt>Up", N_ ("Open the parent folder"), G_CALLBACK (thunar_window_action_go_up), },
  { "open-home", THUNAR_STOCK_HOME, N_ ("_Home"), "<alt>Home", N_ ("Go to the home folder"), G_CALLBACK (thunar_window_action_open_home), },
  { "open-templates", THUNAR_STOCK_TEMPLATES, N_ ("T_emplates"), NULL, N_ ("Go to the templates folder"), G_CALLBACK (thunar_window_action_open_templates), },
  { "open-location", NULL, N_ ("_Open Location..."), "<control>L", N_ ("Specify a location to open"), G_CALLBACK (thunar_window_action_open_location), },
  { "help-menu", NULL, N_ ("_Help"), NULL, },
  { "contents", GTK_STOCK_HELP, N_ ("_Contents"), "F1", N_ ("Display Thunar user manual"), G_CALLBACK (thunar_window_action_contents), },
  { "about", GTK_STOCK_ABOUT, N_ ("_About"), NULL, N_ ("Display information about Thunar"), G_CALLBACK (thunar_window_action_about), },
};

static const GtkToggleActionEntry toggle_action_entries[] =
{
  { "show-hidden", NULL, N_ ("Show _Hidden Files"), "<control>H", N_ ("Toggles the display of hidden files in the current window"), G_CALLBACK (thunar_window_action_show_hidden), FALSE, },
  { "view-location-selector-pathbar", NULL, N_ ("_Pathbar Style"), NULL, N_ ("Modern approach with buttons that correspond to folders"), G_CALLBACK (thunar_window_action_pathbar_changed), FALSE, },
  { "view-location-selector-toolbar", NULL, N_ ("_Toolbar Style"), NULL, N_ ("Traditional approach with location bar and navigation buttons"), G_CALLBACK (thunar_window_action_toolbar_changed), FALSE, },
  { "view-side-pane-shortcuts", NULL, N_ ("_Shortcuts"), "<control>B", N_ ("Toggles the visibility of the shortcuts pane"), G_CALLBACK (thunar_window_action_shortcuts_changed), FALSE, },
  { "view-side-pane-tree", NULL, N_ ("_Tree"), "<control>T", N_ ("Toggles the visibility of the tree pane"), G_CALLBACK (thunar_window_action_tree_changed), FALSE, },
  { "view-statusbar", NULL, N_ ("St_atusbar"), NULL, N_ ("Change the visibility of this window's statusbar"), G_CALLBACK (thunar_window_action_statusbar_changed), FALSE, },
};



static GObjectClass *thunar_window_parent_class;
static guint         window_signals[LAST_SIGNAL];



GType
thunar_window_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarWindowClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_window_class_init,
        NULL,
        NULL,
        sizeof (ThunarWindow),
        0,
        (GInstanceInitFunc) thunar_window_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_WINDOW, I_("ThunarWindow"), &info, 0);
    }

  return type;
}



static void
thunar_window_class_init (ThunarWindowClass *klass)
{
  GtkWidgetClass *gtkwidget_class;
  GtkBindingSet  *binding_set;
  GObjectClass   *gobject_class;

  /* determine the parent type class */
  thunar_window_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = thunar_window_dispose;
  gobject_class->finalize = thunar_window_finalize;
  gobject_class->get_property = thunar_window_get_property;
  gobject_class->set_property = thunar_window_set_property;

  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->realize = thunar_window_realize;
  gtkwidget_class->unrealize = thunar_window_unrealize;
  gtkwidget_class->configure_event = thunar_window_configure_event;

  klass->back = thunar_window_back;
  klass->reload = thunar_window_reload;
  klass->toggle_sidepane = thunar_window_toggle_sidepane;
  klass->zoom_in = thunar_window_zoom_in;
  klass->zoom_out = thunar_window_zoom_out;
  klass->zoom_reset = thunar_window_zoom_reset;

  /**
   * ThunarWindow:current-directory:
   *
   * The directory currently displayed within this #ThunarWindow
   * or %NULL.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_CURRENT_DIRECTORY,
                                   g_param_spec_object ("current-directory",
                                                        "current-directory",
                                                        "current-directory",
                                                        THUNAR_TYPE_FILE,
                                                        EXO_PARAM_READWRITE));

  /**
   * ThunarWindow:show-hidden:
   *
   * Whether to show hidden files in the current window.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_SHOW_HIDDEN,
                                   g_param_spec_boolean ("show-hidden",
                                                         "show-hidden",
                                                         "show-hidden",
                                                         FALSE,
                                                         EXO_PARAM_READABLE));

  /**
   * ThunarWindow:ui-manager:
   *
   * The #GtkUIManager used for this #ThunarWindow. This property
   * can only be read and is garantied to always contain a valid
   * #GtkUIManager instance (thus it's never %NULL).
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_UI_MANAGER,
                                   g_param_spec_object ("ui-manager",
                                                        "ui-manager",
                                                        "ui-manager",
                                                        GTK_TYPE_UI_MANAGER,
                                                        EXO_PARAM_READABLE));

  /**
   * ThunarWindow:zoom-level:
   *
   * The #ThunarZoomLevel applied to the #ThunarView currently
   * shown within this window.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ZOOM_LEVEL,
                                   g_param_spec_enum ("zoom-level",
                                                      "zoom-level",
                                                      "zoom-level",
                                                      THUNAR_TYPE_ZOOM_LEVEL,
                                                      THUNAR_ZOOM_LEVEL_NORMAL,
                                                      EXO_PARAM_READWRITE));

  /**
   * ThunarWindow::back:
   * @window : a #ThunarWindow instance.
   *
   * Emitted whenever the user requests to go to the
   * previous visited folder. This is an internal
   * signal used to bind the action to keys.
   **/
  window_signals[BACK] =
    g_signal_new (I_("back"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (ThunarWindowClass, back),
                  g_signal_accumulator_true_handled, NULL,
                  _thunar_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN, 0);

  /**
   * ThunarWindow::reload:
   * @window : a #ThunarWindow instance.
   *
   * Emitted whenever the user requests to reload the contents
   * of the currently displayed folder. This is an internal
   * signal used to bind the action to keys.
   **/
  window_signals[RELOAD] =
    g_signal_new (I_("reload"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (ThunarWindowClass, reload),
                  g_signal_accumulator_true_handled, NULL,
                  _thunar_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN, 0);

  /**
   * ThunarWindow::reload:
   * @window : a #ThunarWindow instance.
   *
   * Emitted whenever the user toggles the visibility of the
   * sidepane. This is an internal signal used to bind the
   * action to keys.
   **/
  window_signals[TOGGLE_SIDEPANE] =
    g_signal_new (I_("toggle-sidepane"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (ThunarWindowClass, toggle_sidepane),
                  g_signal_accumulator_true_handled, NULL,
                  _thunar_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN, 0);

  /**
   * ThunarWindow::zoom-in:
   * @window : a #ThunarWindow instance.
   *
   * Emitted whenever the user requests to zoom in. This
   * is an internal signal used to bind the action to keys.
   **/
  window_signals[ZOOM_IN] =
    g_signal_new (I_("zoom-in"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (ThunarWindowClass, zoom_in),
                  g_signal_accumulator_true_handled, NULL,
                  _thunar_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN, 0);

  /**
   * ThunarWindow::zoom-out:
   * @window : a #ThunarWindow instance.
   *
   * Emitted whenever the user requests to zoom out. This
   * is an internal signal used to bind the action to keys.
   **/
  window_signals[ZOOM_OUT] =
    g_signal_new (I_("zoom-out"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (ThunarWindowClass, zoom_out),
                  g_signal_accumulator_true_handled, NULL,
                  _thunar_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN, 0);

  /**
   * ThunarWindow::zoom-reset:
   * @window : a #ThunarWindow instance.
   *
   * Emitted whenever the user requests reset the zoom level.
   * This is an internal signal used to bind the action to keys.
   **/
  window_signals[ZOOM_RESET] =
    g_signal_new (I_("zoom-reset"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (ThunarWindowClass, zoom_reset),
                  g_signal_accumulator_true_handled, NULL,
                  _thunar_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN, 0);

  /* setup the key bindings for the windows */
  binding_set = gtk_binding_set_by_class (klass);
  gtk_binding_entry_add_signal (binding_set, GDK_BackSpace, 0, "back", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_F5, 0, "reload", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_F9, 0, "toggle-sidepane", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Add, GDK_CONTROL_MASK, "zoom-in", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Subtract, GDK_CONTROL_MASK, "zoom-out", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_0, GDK_CONTROL_MASK, "zoom-reset", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Insert, GDK_CONTROL_MASK, "zoom-reset", 0);
}



static inline gint
view_type2index (GType type)
{
  /* this necessary for platforms where sizeof(GType) != sizeof(gint),
   * see http://bugzilla.xfce.org/show_bug.cgi?id=2726 for details.
   */
  if (sizeof (GType) == sizeof (gint))
    {
      /* no need to map anything */
      return (gint) type;
    }
  else
    {
      /* map from types to unique indices */
      if (G_LIKELY (type == THUNAR_TYPE_COMPACT_VIEW))
        return 0;
      else if (type == THUNAR_TYPE_DETAILS_VIEW)
        return 1;
      else
        return 2;
    }
}



static inline GType
view_index2type (gint index)
{
  /* this necessary for platforms where sizeof(GType) != sizeof(gint),
   * see http://bugzilla.xfce.org/show_bug.cgi?id=2726 for details.
   */
  if (sizeof (GType) == sizeof (gint))
    {
      /* no need to map anything */
      return (GType) index;
    }
  else
    {
      /* map from indices to unique types */
      switch (index)
        {
        case 0:  return THUNAR_TYPE_COMPACT_VIEW;
        case 1:  return THUNAR_TYPE_DETAILS_VIEW;
        default: return THUNAR_TYPE_ICON_VIEW;
        }
    }
}



static void
thunar_window_init (ThunarWindow *window)
{
  GtkRadioAction *radio_action;
  GtkAccelGroup  *accel_group;
  GtkWidget      *separator;
  GtkWidget      *menubar;
  GtkWidget      *label;
  GtkWidget      *ebox;
  GtkWidget      *item;
  GtkAction      *action;
  gboolean        show_hidden;
  gboolean        visible;
  GSList         *group;
  gchar          *type_name;
  GType           type;
  gint            position;
  gint            width;
  gint            height;

  /* grab a reference on the provider factory */
  window->provider_factory = thunarx_provider_factory_get_default ();

  /* grab a reference on the preferences */
  window->preferences = thunar_preferences_get ();

  /* allocate the scroll_to_files mapping */
  window->scroll_to_files = g_hash_table_new_full (g_direct_hash, g_direct_equal, g_object_unref, g_object_unref);

  /* connect to the volume manager */
  window->volume_manager = thunar_vfs_volume_manager_get_default ();
  g_signal_connect (G_OBJECT (window->volume_manager), "volume-pre-unmount", G_CALLBACK (thunar_window_volume_pre_unmount), window);

  /* allocate a closure for the menu_item_selected() callback */
  window->menu_item_selected_closure = g_cclosure_new_object (G_CALLBACK (thunar_window_menu_item_selected), G_OBJECT (window));
  g_closure_ref (window->menu_item_selected_closure);
  g_closure_sink (window->menu_item_selected_closure);

  /* allocate a closure for the menu_item_deselected() callback */
  window->menu_item_deselected_closure = g_cclosure_new_object (G_CALLBACK (thunar_window_menu_item_deselected), G_OBJECT (window));
  g_closure_ref (window->menu_item_deselected_closure);
  g_closure_sink (window->menu_item_deselected_closure);

  window->icon_factory = thunar_icon_factory_get_default ();

  /* setup the action group for this window */
  window->action_group = gtk_action_group_new ("ThunarWindow");
  gtk_action_group_set_translation_domain (window->action_group, GETTEXT_PACKAGE);
  gtk_action_group_add_actions (window->action_group, action_entries, G_N_ELEMENTS (action_entries), GTK_WIDGET (window));
  gtk_action_group_add_toggle_actions (window->action_group, toggle_action_entries, G_N_ELEMENTS (toggle_action_entries), GTK_WIDGET (window));

  /* initialize the "show-hidden" action using the last value from the preferences */
  action = gtk_action_group_get_action (window->action_group, "show-hidden");
  g_object_get (G_OBJECT (window->preferences), "last-show-hidden", &show_hidden, NULL);
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), show_hidden);

  /* synchronize the "show-hidden" state with the "last-show-hidden" preference */
  exo_binding_new (G_OBJECT (window), "show-hidden", G_OBJECT (window->preferences), "last-show-hidden");

  /* setup the history support */
  window->history = g_object_new (THUNAR_TYPE_HISTORY, "action-group", window->action_group, NULL);
  g_signal_connect_swapped (G_OBJECT (window->history), "change-directory", G_CALLBACK (thunar_window_set_current_directory), window);
  exo_binding_new (G_OBJECT (window), "current-directory", G_OBJECT (window->history), "current-directory");

  /* setup the "open-trash" action */
  action = thunar_trash_action_new ();
  g_signal_connect (G_OBJECT (action), "activate", G_CALLBACK (thunar_window_action_open_trash), window);
  gtk_action_group_add_action (window->action_group, action);
  g_object_unref (G_OBJECT (action));

  /*
   * add view options
   */
  radio_action = gtk_radio_action_new ("view-as-icons", _("View as _Icons"), _("Display folder content in an icon view"),
                                       NULL, view_type2index (THUNAR_TYPE_ICON_VIEW));
  gtk_action_group_add_action_with_accel (window->action_group, GTK_ACTION (radio_action), "<control>1");
  gtk_radio_action_set_group (radio_action, NULL);
  group = gtk_radio_action_get_group (radio_action);
  g_object_unref (G_OBJECT (radio_action));

  radio_action = gtk_radio_action_new ("view-as-detailed-list", _("View as _Detailed List"), _("Display folder content in a detailed list view"),
                                       NULL, view_type2index (THUNAR_TYPE_DETAILS_VIEW));
  gtk_action_group_add_action_with_accel (window->action_group, GTK_ACTION (radio_action), "<control>2");
  gtk_radio_action_set_group (radio_action, group);
  group = gtk_radio_action_get_group (radio_action);
  g_object_unref (G_OBJECT (radio_action));

  radio_action = gtk_radio_action_new ("view-as-compact-list", _("View as _Compact List"), _("Display folder content in a compact list view"),
                                       NULL, view_type2index (THUNAR_TYPE_COMPACT_VIEW));
  gtk_action_group_add_action_with_accel (window->action_group, GTK_ACTION (radio_action), "<control>3");
  gtk_radio_action_set_group (radio_action, group);
  group = gtk_radio_action_get_group (radio_action);
  g_object_unref (G_OBJECT (radio_action));

  window->ui_manager = gtk_ui_manager_new ();
  g_signal_connect (G_OBJECT (window->ui_manager), "connect-proxy", G_CALLBACK (thunar_window_connect_proxy), window);
  g_signal_connect (G_OBJECT (window->ui_manager), "disconnect-proxy", G_CALLBACK (thunar_window_disconnect_proxy), window);
  gtk_ui_manager_insert_action_group (window->ui_manager, window->action_group, 0);
  gtk_ui_manager_add_ui_from_string (window->ui_manager, thunar_window_ui, thunar_window_ui_length, NULL);

  accel_group = gtk_ui_manager_get_accel_group (window->ui_manager);
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

  /* setup the launcher support */
  window->launcher = thunar_launcher_new ();
  thunar_launcher_set_widget (window->launcher, GTK_WIDGET (window));
  thunar_component_set_ui_manager (THUNAR_COMPONENT (window->launcher), window->ui_manager);
  exo_binding_new (G_OBJECT (window), "current-directory", G_OBJECT (window->launcher), "current-directory");
  g_signal_connect_swapped (G_OBJECT (window->launcher), "change-directory", G_CALLBACK (thunar_window_set_current_directory), window);

  /* determine the default window size from the preferences */
  g_object_get (G_OBJECT (window->preferences), "last-window-width", &width, "last-window-height", &height, NULL);
  gtk_window_set_default_size (GTK_WINDOW (window), width, height);

  window->table = gtk_table_new (6, 1, FALSE);
  gtk_container_add (GTK_CONTAINER (window), window->table);
  gtk_widget_show (window->table);

  menubar = gtk_ui_manager_get_widget (window->ui_manager, "/main-menu");
  gtk_table_attach (GTK_TABLE (window->table), menubar, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (menubar);

  /* append the menu item for the throbber */
  item = gtk_menu_item_new ();
  gtk_widget_set_sensitive (GTK_WIDGET (item), FALSE);
  gtk_menu_item_set_right_justified (GTK_MENU_ITEM (item), TRUE);
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), item);
  gtk_widget_show (item);

  /* place the throbber into the menu item */
  window->throbber = thunar_throbber_new ();
  gtk_container_add (GTK_CONTAINER (item), window->throbber);
  gtk_widget_show (window->throbber);

  /* check if we need to add the root warning */
  if (G_UNLIKELY (geteuid () == 0))
    {
      /* install default settings for the root warning text box */
      gtk_rc_parse_string ("style\"thunar-window-root-style\"{bg[NORMAL]=\"#b4254b\"\nfg[NORMAL]=\"#fefefe\"}\n"
                           "widget\"ThunarWindow.*.root-warning\"style\"thunar-window-root-style\"\n"
                           "widget\"ThunarWindow.*.root-warning.GtkLabel\"style\"thunar-window-root-style\"\n");

      /* add the box for the root warning */
      ebox = gtk_event_box_new ();
      gtk_widget_set_name (ebox, "root-warning");
      gtk_table_attach (GTK_TABLE (window->table), ebox, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show (ebox);

      /* add the label with the root warning */
      label = gtk_label_new (_("Warning, you are using the root account, you may harm your system."));
      gtk_misc_set_padding (GTK_MISC (label), 6, 3);
      gtk_container_add (GTK_CONTAINER (ebox), label);
      gtk_widget_show (label);

      separator = gtk_hseparator_new ();
      gtk_table_attach (GTK_TABLE (window->table), separator, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show (separator);
    }

  window->paned = gtk_hpaned_new ();
  gtk_container_set_border_width (GTK_CONTAINER (window->paned), 6);
  gtk_table_attach (GTK_TABLE (window->table), window->paned, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_show (window->paned);

  /* determine the last separator position and apply it to the paned view */
  g_object_get (G_OBJECT (window->preferences), "last-separator-position", &position, NULL);
  gtk_paned_set_position (GTK_PANED (window->paned), position);

  /* always remember the last separator position for newly opened windows */
  exo_binding_new (G_OBJECT (window->paned), "position", G_OBJECT (window->preferences), "last-separator-position");

  window->view_box = gtk_vbox_new (FALSE, 6);
  gtk_paned_pack2 (GTK_PANED (window->paned), window->view_box, TRUE, FALSE);
  gtk_widget_show (window->view_box);

  /* determine the selected locatin selector */
  g_object_get (G_OBJECT (window->preferences), "last-location-bar", &type_name, NULL);
  if (exo_str_is_equal (type_name, g_type_name (THUNAR_TYPE_LOCATION_BUTTONS)))
    type = THUNAR_TYPE_LOCATION_BUTTONS;
  else if (exo_str_is_equal (type_name, g_type_name (THUNAR_TYPE_LOCATION_ENTRY)))
    type = THUNAR_TYPE_LOCATION_ENTRY;
  else
    type = G_TYPE_NONE;
  g_free (type_name);

  /* activate the selected location selector */
  action = gtk_action_group_get_action (window->action_group, "view-location-selector-pathbar");
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), (type == THUNAR_TYPE_LOCATION_BUTTONS));
  action = gtk_action_group_get_action (window->action_group, "view-location-selector-toolbar");
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), (type == THUNAR_TYPE_LOCATION_ENTRY));

  /* determine the selected side pane (FIXME: Should probably be last-shortcuts-visible and last-tree-visible preferences) */
  g_object_get (G_OBJECT (window->preferences), "last-side-pane", &type_name, NULL);
  if (exo_str_is_equal (type_name, g_type_name (THUNAR_TYPE_SHORTCUTS_PANE)))
    type = THUNAR_TYPE_SHORTCUTS_PANE;
  else if (exo_str_is_equal (type_name, g_type_name (THUNAR_TYPE_TREE_PANE)))
    type = THUNAR_TYPE_TREE_PANE;
  else
    type = G_TYPE_NONE;
  g_free (type_name);

  /* activate the selected side pane */
  action = gtk_action_group_get_action (window->action_group, "view-side-pane-shortcuts");
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), (type == THUNAR_TYPE_SHORTCUTS_PANE));
  action = gtk_action_group_get_action (window->action_group, "view-side-pane-tree");
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), (type == THUNAR_TYPE_TREE_PANE));

  /* determine the default view */
  g_object_get (G_OBJECT (window->preferences), "default-view", &type_name, NULL);
  type = g_type_from_name (type_name);
  g_free (type_name);

  /* check if we should display the statusbar by default */
  g_object_get (G_OBJECT (window->preferences), "last-statusbar-visible", &visible, NULL);
  action = gtk_action_group_get_action (window->action_group, "view-statusbar");
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), visible);

  /* determine the last selected view if we don't have a valid default */
  if (!g_type_is_a (type, THUNAR_TYPE_VIEW))
    {
      g_object_get (G_OBJECT (window->preferences), "last-view", &type_name, NULL);
      type = g_type_from_name (type_name);
      g_free (type_name);
    }

  /* activate the selected view */
  action = gtk_action_group_get_action (window->action_group, "view-as-icons");
  exo_gtk_radio_action_set_current_value (GTK_RADIO_ACTION (action), view_type2index (g_type_is_a (type, THUNAR_TYPE_VIEW) ? type : THUNAR_TYPE_ICON_VIEW));
  g_signal_connect (G_OBJECT (action), "changed", G_CALLBACK (thunar_window_action_view_changed), window);
  thunar_window_action_view_changed (GTK_RADIO_ACTION (action), GTK_RADIO_ACTION (action), window);

  /* schedule asynchronous menu action merging */
  window->merge_idle_id = g_idle_add_full (G_PRIORITY_LOW + 20, thunar_window_merge_idle, window, thunar_window_merge_idle_destroy);
}



static void
thunar_window_dispose (GObject *object)
{
  ThunarWindow *window = THUNAR_WINDOW (object);

  /* destroy the save geometry timer source */
  if (G_UNLIKELY (window->save_geometry_timer_id > 0))
    g_source_remove (window->save_geometry_timer_id);

  /* destroy the merge idle source */
  if (G_UNLIKELY (window->merge_idle_id > 0))
    g_source_remove (window->merge_idle_id);

  /* un-merge the custom preferences */
  if (G_LIKELY (window->custom_preferences_merge_id != 0))
    {
      gtk_ui_manager_remove_ui (window->ui_manager, window->custom_preferences_merge_id);
      window->custom_preferences_merge_id = 0;
    }

  /* disconnect from the current-directory */
  thunar_window_set_current_directory (window, NULL);

  (*G_OBJECT_CLASS (thunar_window_parent_class)->dispose) (object);
}



static void
thunar_window_finalize (GObject *object)
{
  ThunarWindow *window = THUNAR_WINDOW (object);

  /* drop our references on the menu_item_selected()/menu_item_deselected() closures */
  g_closure_unref (window->menu_item_deselected_closure);
  g_closure_unref (window->menu_item_selected_closure);

  /* disconnect from the volume manager */
  g_signal_handlers_disconnect_matched (G_OBJECT (window->volume_manager), G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, window);
  g_object_unref (G_OBJECT (window->volume_manager));

  /* disconnect from the ui manager */
  g_signal_handlers_disconnect_matched (G_OBJECT (window->ui_manager), G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, window);
  g_object_unref (G_OBJECT (window->ui_manager));

  g_object_unref (G_OBJECT (window->action_group));
  g_object_unref (G_OBJECT (window->icon_factory));
  g_object_unref (G_OBJECT (window->launcher));
  g_object_unref (G_OBJECT (window->history));

  /* release our reference on the provider factory */
  g_object_unref (G_OBJECT (window->provider_factory));

  /* release the preferences reference */
  g_object_unref (G_OBJECT (window->preferences));

  /* release the scroll_to_files hash table */
  g_hash_table_destroy (window->scroll_to_files);

  (*G_OBJECT_CLASS (thunar_window_parent_class)->finalize) (object);
}



static void
thunar_window_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  ThunarWindow *window = THUNAR_WINDOW (object);
  GtkAction    *action;

  switch (prop_id)
    {
    case PROP_CURRENT_DIRECTORY:
      g_value_set_object (value, thunar_window_get_current_directory (window));
      break;

    case PROP_SHOW_HIDDEN:
      action = gtk_action_group_get_action (window->action_group, "show-hidden");
      g_value_set_boolean (value, gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
      break;

    case PROP_UI_MANAGER:
      g_value_set_object (value, window->ui_manager);
      break;

    case PROP_ZOOM_LEVEL:
      g_value_set_enum (value, window->zoom_level);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_window_set_property (GObject            *object,
                            guint               prop_id,
                            const GValue       *value,
                            GParamSpec         *pspec)
{
  ThunarWindow *window = THUNAR_WINDOW (object);

  switch (prop_id)
    {
    case PROP_CURRENT_DIRECTORY:
      thunar_window_set_current_directory (window, g_value_get_object (value));
      break;

    case PROP_ZOOM_LEVEL:
      thunar_window_set_zoom_level (window, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gboolean
thunar_window_back (ThunarWindow *window)
{
  GtkAction *action;

  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), FALSE);

  /* activate the "back" action */
  action = thunar_gtk_ui_manager_get_action_by_name (window->ui_manager, "back");
  if (G_LIKELY (action != NULL))
    {
      gtk_action_activate (action);
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_window_reload (ThunarWindow *window)
{
  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), FALSE);

  /* force the view to reload */
  if (G_LIKELY (window->view != NULL))
    {
      thunar_view_reload (THUNAR_VIEW (window->view));
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_window_toggle_sidepane (ThunarWindow *window)
{
  GtkAction *action;
  gchar     *type_name;

  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), FALSE);

  /* check if a side pane is currently active */
  if (G_LIKELY (window->sidepane != NULL))
    {
      /* determine the currently active side pane type */
      window->toggle_sidepane_type = G_OBJECT_TYPE (window->sidepane);

      /* just reset both side pane actions */
      action = gtk_action_group_get_action (window->action_group, "view-side-pane-shortcuts");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);
      action = gtk_action_group_get_action (window->action_group, "view-side-pane-tree");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);
    }
  else
    {
      /* check if we have a previously remembered toggle type */
      if (G_UNLIKELY (window->toggle_sidepane_type == G_TYPE_INVALID))
        {
          /* guess type based on the last-side-pane preference, default to shortcuts */
          g_object_get (G_OBJECT (window->preferences), "last-side-pane", &type_name, NULL);
          if (exo_str_is_equal (type_name, g_type_name (THUNAR_TYPE_TREE_PANE)))
            window->toggle_sidepane_type = THUNAR_TYPE_TREE_PANE;
          else
            window->toggle_sidepane_type = THUNAR_TYPE_SHORTCUTS_PANE;
          g_free (type_name);
        }

      /* activate the given side pane */
      action = gtk_action_group_get_action (window->action_group, "view-side-pane-shortcuts");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), (window->toggle_sidepane_type == THUNAR_TYPE_SHORTCUTS_PANE));
      action = gtk_action_group_get_action (window->action_group, "view-side-pane-tree");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), (window->toggle_sidepane_type == THUNAR_TYPE_TREE_PANE));
    }

  return TRUE;
}



static gboolean
thunar_window_zoom_in (ThunarWindow *window)
{
  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), FALSE);

  /* check if we can still zoom in */
  if (G_LIKELY (window->zoom_level < THUNAR_ZOOM_N_LEVELS - 1))
    {
      thunar_window_set_zoom_level (window, window->zoom_level + 1);
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_window_zoom_out (ThunarWindow *window)
{
  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), FALSE);

  /* check if we can still zoom out */
  if (G_LIKELY (window->zoom_level > 0))
    {
      thunar_window_set_zoom_level (window, window->zoom_level - 1);
      return TRUE;
    }

  return FALSE;
}



static gboolean
thunar_window_zoom_reset (ThunarWindow *window)
{
  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), FALSE);

  /* tell the view to reset it's zoom level */
  if (G_LIKELY (window->view != NULL))
    {
      thunar_view_reset_zoom_level (THUNAR_VIEW (window->view));
      return TRUE;
    }

  return FALSE;
}



static void
thunar_window_realize (GtkWidget *widget)
{
  ThunarWindow *window = THUNAR_WINDOW (widget);

  /* let the GtkWidget class perform the realize operation */
  (*GTK_WIDGET_CLASS (thunar_window_parent_class)->realize) (widget);

  /* connect to the clipboard manager of the new display and be sure to redraw the window
   * whenever the clipboard contents change to make sure we always display up2date state.
   */
  window->clipboard = thunar_clipboard_manager_get_for_display (gtk_widget_get_display (widget));
  g_signal_connect_swapped (G_OBJECT (window->clipboard), "changed",
                            G_CALLBACK (gtk_widget_queue_draw), widget);
}



static void
thunar_window_unrealize (GtkWidget *widget)
{
  ThunarWindow *window = THUNAR_WINDOW (widget);

  /* disconnect from the clipboard manager */
  g_signal_handlers_disconnect_by_func (G_OBJECT (window->clipboard), gtk_widget_queue_draw, widget);
  g_object_unref (G_OBJECT (window->clipboard));

  /* let the GtkWidget class unrealize the window */
  (*GTK_WIDGET_CLASS (thunar_window_parent_class)->unrealize) (widget);
}



static gboolean
thunar_window_configure_event (GtkWidget         *widget,
                               GdkEventConfigure *event)
{
  ThunarWindow *window = THUNAR_WINDOW (widget);

  /* check if we have a new dimension here */
  if (widget->allocation.width != event->width || widget->allocation.height != event->height)
    {
      /* drop any previous timer source */
      if (window->save_geometry_timer_id > 0)
        g_source_remove (window->save_geometry_timer_id);

      /* check if we should schedule another save timer */
      if (GTK_WIDGET_VISIBLE (widget))
        {
          /* save the geometry one second after the last configure event */
          window->save_geometry_timer_id = g_timeout_add_full (G_PRIORITY_LOW, 1000, thunar_window_save_geometry_timer,
                                                               window, thunar_window_save_geometry_timer_destroy);
        }
    }

  /* let Gtk+ handle the configure event */
  return (*GTK_WIDGET_CLASS (thunar_window_parent_class)->configure_event) (widget, event);
}



static void
thunar_window_install_location_bar (ThunarWindow *window,
                                    GType         type)
{
  GtkToolItem *item;

  _thunar_return_if_fail (type == G_TYPE_NONE || g_type_is_a (type, THUNAR_TYPE_LOCATION_BAR));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* drop the previous location bar (if any) */
  if (G_UNLIKELY (window->location_bar != NULL))
    {
      /* check if it was toolbar'ed (and thereby need to be disconnected from the toolbar) */
      if (!thunar_location_bar_is_standalone (THUNAR_LOCATION_BAR (window->location_bar)))
        {
          /* disconnect the toolbar from the window */
          gtk_container_remove (GTK_CONTAINER (window->table), window->location_toolbar);
          window->location_toolbar = NULL;
        }

      /* destroy the location bar */
      gtk_widget_destroy (window->location_bar);
      window->location_bar = NULL;
    }

  /* check if we have a new location bar */
  if (G_LIKELY (type != G_TYPE_NONE))
    {
      /* allocate the new location bar widget */
      window->location_bar = g_object_new (type, "ui-manager", window->ui_manager, NULL);
      exo_binding_new (G_OBJECT (window), "current-directory", G_OBJECT (window->location_bar), "current-directory");
      g_signal_connect_swapped (G_OBJECT (window->location_bar), "change-directory", G_CALLBACK (thunar_window_set_current_directory), window);

      /* connect the location widget to the view (if any) */
      if (G_LIKELY (window->view != NULL))
        exo_binding_new (G_OBJECT (window->view), "selected-files", G_OBJECT (window->location_bar), "selected-files");

      /* check if the location bar should be placed into a toolbar */
      if (!thunar_location_bar_is_standalone (THUNAR_LOCATION_BAR (window->location_bar)))
        {
          /* setup the toolbar for the location bar */
          window->location_toolbar = gtk_ui_manager_get_widget (window->ui_manager, "/location-toolbar");
          gtk_table_attach (GTK_TABLE (window->table), window->location_toolbar, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
          gtk_widget_show (window->location_toolbar);

          /* add a separator before the location bar (destroyed with the location bar) */
          item = gtk_separator_tool_item_new ();
          g_signal_connect_object (G_OBJECT (window->location_bar), "destroy", G_CALLBACK (gtk_widget_destroy), item, G_CONNECT_SWAPPED);
          gtk_toolbar_insert (GTK_TOOLBAR (window->location_toolbar), item, -1);
          gtk_widget_show (GTK_WIDGET (item));

          /* add the location bar tool item (destroyed with the location bar) */
          item = gtk_tool_item_new ();
          gtk_tool_item_set_expand (item, TRUE);
          g_signal_connect_object (G_OBJECT (window->location_bar), "destroy", G_CALLBACK (gtk_widget_destroy), item, G_CONNECT_SWAPPED);
          gtk_toolbar_insert (GTK_TOOLBAR (window->location_toolbar), item, -1);
          gtk_widget_show (GTK_WIDGET (item));

          /* add the location bar itself */
          gtk_container_add (GTK_CONTAINER (item), window->location_bar);
        }
      else
        {
          /* it's a standalone location bar, just place it above the view */
          gtk_box_pack_start (GTK_BOX (window->view_box), window->location_bar, FALSE, FALSE, 0);
        }

      /* display the new location bar widget */
      gtk_widget_show (window->location_bar);
    }

  /* remember the setting */
  g_object_set (G_OBJECT (window->preferences), "last-location-bar", g_type_name (type), NULL);
}



static void
thunar_window_install_sidepane (ThunarWindow *window,
                                GType         type)
{
  _thunar_return_if_fail (type == G_TYPE_NONE || g_type_is_a (type, THUNAR_TYPE_SIDE_PANE));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* drop the previous side pane (if any) */
  if (G_UNLIKELY (window->sidepane != NULL))
    {
      gtk_widget_destroy (window->sidepane);
      window->sidepane = NULL;
    }

  /* check if we have a new sidepane widget */
  if (G_LIKELY (type != G_TYPE_NONE))
    {
      /* allocate the new side pane widget */
      window->sidepane = g_object_new (type, NULL);
      thunar_component_set_ui_manager (THUNAR_COMPONENT (window->sidepane), window->ui_manager);
      exo_binding_new (G_OBJECT (window), "show-hidden", G_OBJECT (window->sidepane), "show-hidden");
      exo_binding_new (G_OBJECT (window), "current-directory", G_OBJECT (window->sidepane), "current-directory");
      g_signal_connect_swapped (G_OBJECT (window->sidepane), "change-directory", G_CALLBACK (thunar_window_set_current_directory), window);
      gtk_paned_pack1 (GTK_PANED (window->paned), window->sidepane, FALSE, FALSE);
      gtk_widget_show (window->sidepane);

      /* connect the side pane widget to the view (if any) */
      if (G_LIKELY (window->view != NULL))
        exo_binding_new (G_OBJECT (window->view), "selected-files", G_OBJECT (window->sidepane), "selected-files");
    }

  /* remember the setting */
  g_object_set (G_OBJECT (window->preferences), "last-side-pane", g_type_name (type), NULL);
}



static void
thunar_window_merge_custom_preferences (ThunarWindow *window)
{
  GList *providers;
  GList *actions;
  GList *ap, *pp;

  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));
  _thunar_return_if_fail (window->custom_preferences_merge_id == 0);

  /* determine the available preferences providers */
  providers = thunarx_provider_factory_list_providers (window->provider_factory, THUNARX_TYPE_PREFERENCES_PROVIDER);
  if (G_LIKELY (providers != NULL))
    {
      /* allocate a new merge id from the UI manager */
      window->custom_preferences_merge_id = gtk_ui_manager_new_merge_id (window->ui_manager);

      /* add actions from all providers */
      for (pp = providers; pp != NULL; pp = pp->next)
        {
          /* determine the available actions for the provider */
          actions = thunarx_preferences_provider_get_actions (THUNARX_PREFERENCES_PROVIDER (pp->data), GTK_WIDGET (window));
          for (ap = actions; ap != NULL; ap = ap->next)
            {
              /* add the action to the action group */
              gtk_action_group_add_action (window->action_group, GTK_ACTION (ap->data));

              /* add the action to the UI manager */
              gtk_ui_manager_add_ui (window->ui_manager,
                                     window->custom_preferences_merge_id,
                                     "/main-menu/edit-menu/placeholder-custom-preferences",
                                     gtk_action_get_name (GTK_ACTION (ap->data)),
                                     gtk_action_get_name (GTK_ACTION (ap->data)),
                                     GTK_UI_MANAGER_MENUITEM, FALSE);

              /* release the reference on the action */
              g_object_unref (G_OBJECT (ap->data));
            }

          /* release the reference on the provider */
          g_object_unref (G_OBJECT (pp->data));

          /* release the action list */
          g_list_free (actions);
        }

      /* release the provider list */
      g_list_free (providers);
    }
}



static void
thunar_window_start_open_location (ThunarWindow *window,
                                   const gchar  *initial_text)
{
  ThunarFile *selected_file;
  GtkWidget  *dialog;
  GError     *error = NULL;

  /* bring up the "Open Location"-dialog if the window has no location bar or the location bar
   * in the window does not support text entry by the user.
   */
  if (window->location_bar == NULL || !thunar_location_bar_accept_focus (THUNAR_LOCATION_BAR (window->location_bar), initial_text))
    {
      /* allocate the "Open Location" dialog */
      dialog = thunar_location_dialog_new ();
      gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
      gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);
      gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));
      thunar_location_dialog_set_selected_file (THUNAR_LOCATION_DIALOG (dialog), thunar_window_get_current_directory (window));

      /* setup the initial text (if any) */
      if (G_UNLIKELY (initial_text != NULL))
        {
          /* show, grab focus, set text and move cursor to the end */
          gtk_widget_show_now (dialog);
          gtk_widget_grab_focus (THUNAR_LOCATION_DIALOG (dialog)->entry);
          gtk_entry_set_text (GTK_ENTRY (THUNAR_LOCATION_DIALOG (dialog)->entry), initial_text);
          gtk_editable_set_position (GTK_EDITABLE (THUNAR_LOCATION_DIALOG (dialog)->entry), -1);
        }

      /* run the dialog */
      if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
        {
          /* check if we have a new directory or a file to launch */
          selected_file = thunar_location_dialog_get_selected_file (THUNAR_LOCATION_DIALOG (dialog));
          if (thunar_file_is_directory (selected_file))
            {
              /* open the new directory */
              thunar_window_set_current_directory (window, selected_file);
            }
          else
            {
              /* be sure to hide the location dialog first */
              gtk_widget_hide (dialog);

              /* try to launch the selected file */
              if (!thunar_file_launch (selected_file, GTK_WIDGET (window), &error))
                {
                  thunar_dialogs_show_error (GTK_WIDGET (window), error, _("Failed to launch \"%s\""), thunar_file_get_display_name (selected_file));
                  g_error_free (error);
                }
            }
        }

      /* destroy the dialog */
      gtk_widget_destroy (dialog);
    }
}



static void
thunar_window_action_open_new_window (GtkAction    *action,
                                      ThunarWindow *window)
{
  ThunarApplication *application;
  ThunarFile        *start_file;
  GtkWidget         *new_window;

  /* popup a new window */
  application = thunar_application_get ();
  new_window = thunar_application_open_window (application, window->current_directory,
                                               gtk_widget_get_screen (GTK_WIDGET (window)));
  g_object_unref (G_OBJECT (application));

  /* determine the first visible file in the current window */
  if (window->view != NULL && thunar_view_get_visible_range (THUNAR_VIEW (window->view), &start_file, NULL))
    {
      /* scroll the new window to the same file */
      thunar_window_scroll_to_file (THUNAR_WINDOW (new_window), start_file, FALSE, TRUE, 0.0f, 0.0f);

      /* release the file reference */
      g_object_unref (G_OBJECT (start_file));
    }
}



static void
thunar_window_action_empty_trash (GtkAction    *action,
                                  ThunarWindow *window)
{
  ThunarApplication *application;

  /* launch the operation */
  application = thunar_application_get ();
  thunar_application_empty_trash (application, GTK_WIDGET (window));
  g_object_unref (G_OBJECT (application));
}



static void
thunar_window_action_close_all_windows (GtkAction    *action,
                                        ThunarWindow *window)
{
  ThunarApplication *application;
  GList             *windows;

  /* query the list of currently open windows */
  application = thunar_application_get ();
  windows = thunar_application_get_windows (application);
  g_object_unref (G_OBJECT (application));

  /* destroy all open windows */
  g_list_foreach (windows, (GFunc) gtk_widget_destroy, NULL);
  g_list_free (windows);
}



static void
thunar_window_action_close (GtkAction    *action,
                            ThunarWindow *window)
{
  gtk_widget_destroy (GTK_WIDGET (window));
}



static void
thunar_window_action_preferences (GtkAction    *action,
                                  ThunarWindow *window)
{
  GtkWidget *dialog;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* allocate and display a preferences dialog */
  dialog = thunar_preferences_dialog_new (GTK_WINDOW (window));
  gtk_widget_show (dialog);
}



static void
thunar_window_action_reload (GtkAction    *action,
                             ThunarWindow *window)
{
  gboolean result;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* force the view to reload */
  g_signal_emit (G_OBJECT (window), window_signals[RELOAD], 0, &result);
}



static void
thunar_window_action_pathbar_changed (GtkToggleAction *action,
                                      ThunarWindow    *window)
{
  GtkAction *other_action;
  GType      type;

  _thunar_return_if_fail (GTK_IS_TOGGLE_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* determine the new type of location bar */
  type = gtk_toggle_action_get_active (action) ? THUNAR_TYPE_LOCATION_BUTTONS : G_TYPE_NONE;

  /* install the new location bar */
  thunar_window_install_location_bar (window, type);

  /* check if we actually installed anything */
  if (G_LIKELY (type != G_TYPE_NONE))
    {
      /* reset the state of the toolbar action (without firing the handler) */
      other_action = gtk_action_group_get_action (window->action_group, "view-location-selector-toolbar");
      g_signal_handlers_block_by_func (G_OBJECT (other_action), thunar_window_action_toolbar_changed, window);
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (other_action), FALSE);
      g_signal_handlers_unblock_by_func (G_OBJECT (other_action), thunar_window_action_toolbar_changed, window);
    }
}



static void
thunar_window_action_toolbar_changed (GtkToggleAction *action,
                                      ThunarWindow    *window)
{
  GtkAction *other_action;
  GType      type;

  _thunar_return_if_fail (GTK_IS_TOGGLE_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* determine the new type of location bar */
  type = gtk_toggle_action_get_active (action) ? THUNAR_TYPE_LOCATION_ENTRY : G_TYPE_NONE;

  /* install the new location bar */
  thunar_window_install_location_bar (window, type);

  /* check if we actually installed anything */
  if (G_LIKELY (type != G_TYPE_NONE))
    {
      /* reset the state of the pathbar action (without firing the handler) */
      other_action = gtk_action_group_get_action (window->action_group, "view-location-selector-pathbar");
      g_signal_handlers_block_by_func (G_OBJECT (other_action), thunar_window_action_pathbar_changed, window);
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (other_action), FALSE);
      g_signal_handlers_unblock_by_func (G_OBJECT (other_action), thunar_window_action_pathbar_changed, window);
    }
}



static void
thunar_window_action_shortcuts_changed (GtkToggleAction *action,
                                        ThunarWindow    *window)
{
  GtkAction *other_action;
  GType      type;

  /* determine the new type of side pane */
  type = gtk_toggle_action_get_active (action) ? THUNAR_TYPE_SHORTCUTS_PANE : G_TYPE_NONE;

  /* install the new sidepane */
  thunar_window_install_sidepane (window, type);

  /* check if we actually installed anything */
  if (G_LIKELY (type != G_TYPE_NONE))
    {
      /* reset the state of the tree pane action (without firing the handler) */
      other_action = gtk_action_group_get_action (window->action_group, "view-side-pane-tree");
      g_signal_handlers_block_by_func (G_OBJECT (other_action), thunar_window_action_tree_changed, window);
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (other_action), FALSE);
      g_signal_handlers_unblock_by_func (G_OBJECT (other_action), thunar_window_action_tree_changed, window);
    }
}



static void
thunar_window_action_tree_changed (GtkToggleAction *action,
                                   ThunarWindow    *window)
{
  GtkAction *other_action;
  GType      type;

  /* determine the new type of side pane */
  type = gtk_toggle_action_get_active (action) ? THUNAR_TYPE_TREE_PANE : G_TYPE_NONE;

  /* install the new sidepane */
  thunar_window_install_sidepane (window, type);

  /* check if we actually installed anything */
  if (G_LIKELY (type != G_TYPE_NONE))
    {
      /* reset the state of the shortcuts pane action (without firing the handler) */
      other_action = gtk_action_group_get_action (window->action_group, "view-side-pane-shortcuts");
      g_signal_handlers_block_by_func (G_OBJECT (other_action), thunar_window_action_shortcuts_changed, window);
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (other_action), FALSE);
      g_signal_handlers_unblock_by_func (G_OBJECT (other_action), thunar_window_action_shortcuts_changed, window);
    }
}



static void
thunar_window_action_statusbar_changed (GtkToggleAction *action,
                                        ThunarWindow    *window)
{
  gboolean active;

  _thunar_return_if_fail (GTK_IS_TOGGLE_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* determine the new state of the action */
  active = gtk_toggle_action_get_active (action);
  
  /* check if we should drop the statusbar */
  if (!active && window->statusbar != NULL)
    {
      /* just get rid of the statusbar */
      gtk_widget_destroy (window->statusbar);
      window->statusbar = NULL;
    }
  else if (active && window->statusbar == NULL)
    {
      /* setup a new statusbar */
      window->statusbar = thunar_statusbar_new ();
      gtk_table_attach (GTK_TABLE (window->table), window->statusbar, 0, 1, 6, 7, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show (window->statusbar);

      /* connect to the view (if any) */
      if (G_LIKELY (window->view != NULL))
        exo_binding_new (G_OBJECT (window->view), "statusbar-text", G_OBJECT (window->statusbar), "text");
    }

  /* remember the setting */
  g_object_set (G_OBJECT (window->preferences), "last-statusbar-visible", active, NULL);
}



static void
thunar_window_action_zoom_in (GtkAction    *action,
                              ThunarWindow *window)
{
  gboolean result;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* increase the zoom level */
  g_signal_emit (G_OBJECT (window), window_signals[ZOOM_IN], 0, &result);
}



static void
thunar_window_action_zoom_out (GtkAction    *action,
                               ThunarWindow *window)
{
  gboolean result;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* decrease the zoom level */
  g_signal_emit (G_OBJECT (window), window_signals[ZOOM_OUT], 0, &result);
}



static void
thunar_window_action_zoom_reset (GtkAction    *action,
                                 ThunarWindow *window)
{
  gboolean result;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* reset zoom level */
  g_signal_emit (G_OBJECT (window), window_signals[ZOOM_RESET], 0, &result);
}



static void
thunar_window_action_view_changed (GtkRadioAction *action,
                                   GtkRadioAction *current,
                                   ThunarWindow   *window)
{
  ThunarFile *file = NULL;
  GType       type;

  /* drop the previous view (if any) */
  if (G_LIKELY (window->view != NULL))
    {
      /* get first visible file in the previous view */
      if (!thunar_view_get_visible_range (THUNAR_VIEW (window->view), &file, NULL))
        file = NULL;

      /* destroy and disconnect the previous view */
      gtk_widget_destroy (window->view);

      /* update the UI (else GtkUIManager will crash on merging) */
      gtk_ui_manager_ensure_update (window->ui_manager);
    }

  /* determine the new type of view */
  type = view_index2type (gtk_radio_action_get_current_value (action));

  /* allocate a new view of the requested type */
  if (G_LIKELY (type != G_TYPE_NONE))
    {
      /* allocate and setup a new view */
      window->view = g_object_new (type, "ui-manager", window->ui_manager, NULL);
      g_signal_connect (G_OBJECT (window->view), "notify::loading", G_CALLBACK (thunar_window_notify_loading), window);
      g_signal_connect_swapped (G_OBJECT (window->view), "start-open-location", G_CALLBACK (thunar_window_start_open_location), window);
      g_signal_connect_swapped (G_OBJECT (window->view), "change-directory", G_CALLBACK (thunar_window_set_current_directory), window);
      exo_binding_new (G_OBJECT (window), "current-directory", G_OBJECT (window->view), "current-directory");
      exo_binding_new (G_OBJECT (window), "show-hidden", G_OBJECT (window->view), "show-hidden");
      exo_binding_new (G_OBJECT (window->view), "loading", G_OBJECT (window->throbber), "animated");
      exo_binding_new (G_OBJECT (window->view), "selected-files", G_OBJECT (window->launcher), "selected-files");
      exo_mutual_binding_new (G_OBJECT (window->view), "zoom-level", G_OBJECT (window), "zoom-level");
      gtk_box_pack_end (GTK_BOX (window->view_box), window->view, TRUE, TRUE, 0);
      gtk_widget_grab_focus (window->view);
      gtk_widget_show (window->view);

      /* connect to the location bar (if any) */
      if (G_LIKELY (window->location_bar != NULL))
        exo_binding_new (G_OBJECT (window->view), "selected-files", G_OBJECT (window->location_bar), "selected-files");

      /* connect to the sidepane (if any) */
      if (G_LIKELY (window->sidepane != NULL))
        exo_binding_new (G_OBJECT (window->view), "selected-files", G_OBJECT (window->sidepane), "selected-files");

      /* connect to the statusbar (if any) */
      if (G_LIKELY (window->statusbar != NULL))
        exo_binding_new (G_OBJECT (window->view), "statusbar-text", G_OBJECT (window->statusbar), "text");

      /* scroll to the previously visible file in the old view */
      if (G_UNLIKELY (file != NULL))
        thunar_view_scroll_to_file (THUNAR_VIEW (window->view), file, FALSE, TRUE, 0.0f, 0.0f);
    }
  else
    {
      /* this should not happen under normal conditions */
      window->view = NULL;
    }

  /* remember the setting */
  g_object_set (G_OBJECT (window->preferences), "last-view", g_type_name (type), NULL);

  /* release the file reference */
  if (G_UNLIKELY (file != NULL))
    g_object_unref (G_OBJECT (file));
}



static void
thunar_window_action_go_up (GtkAction    *action,
                            ThunarWindow *window)
{
  ThunarFile *parent;
  GError     *error = NULL;

  parent = thunar_file_get_parent (window->current_directory, &error);
  if (G_LIKELY (parent != NULL))
    {
      thunar_window_set_current_directory (window, parent);
      g_object_unref (G_OBJECT (parent));
    }
  else
    {
      thunar_dialogs_show_error (GTK_WIDGET (window), error, _("Failed to open parent folder"));
      g_error_free (error);
    }
}



static void
thunar_window_action_open_home (GtkAction    *action,
                                ThunarWindow *window)
{
  ThunarVfsPath *home_path;
  ThunarFile    *home_file;
  GError        *error = NULL;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* determine the path to the home directory */
  home_path = thunar_vfs_path_get_for_home ();

  /* determine the file for the home directory */
  home_file = thunar_file_get_for_path (home_path, &error);
  if (G_UNLIKELY (home_file == NULL))
    {
      /* display an error to the user */
      thunar_dialogs_show_error (GTK_WIDGET (window), error, _("Failed to open the home folder"));
      g_error_free (error);
    }
  else
    {
      /* open the home folder */
      thunar_window_set_current_directory (window, home_file);
      g_object_unref (G_OBJECT (home_file));
    }

  /* release our reference on the home path */
  thunar_vfs_path_unref (home_path);
}



static void
thunar_window_action_open_templates (GtkAction    *action,
                                     ThunarWindow *window)
{
  ThunarVfsPath *home_path;
  ThunarVfsPath *templates_path;
  ThunarFile    *templates_file = NULL;
  GtkWidget     *dialog;
  GtkWidget     *button;
  GtkWidget     *label;
  GtkWidget     *image;
  GtkWidget     *hbox;
  GtkWidget     *vbox;
  gboolean       show_about_templates;
  GError        *error = NULL;
  gchar         *absolute_path;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* determine the path to the ~/Templates folder */
  home_path = thunar_vfs_path_get_for_home ();
  templates_path = thunar_vfs_path_relative (home_path, "Templates");
  thunar_vfs_path_unref (home_path);

  /* make sure that the ~/Templates folder exists */
  absolute_path = thunar_vfs_path_dup_string (templates_path);
  xfce_mkdirhier (absolute_path, 0755, &error);
  g_free (absolute_path);

  /* determine the file for the ~/Templates path */
  if (G_LIKELY (error == NULL))
    templates_file = thunar_file_get_for_path (templates_path, &error);

  /* open the ~/Templates folder */
  if (G_LIKELY (templates_file != NULL))
    {
      /* go to the ~/Templates folder */
      thunar_window_set_current_directory (window, templates_file);
      g_object_unref (G_OBJECT (templates_file));

      /* check whether we should display the "About Templates" dialog */
      g_object_get (G_OBJECT (window->preferences), "misc-show-about-templates", &show_about_templates, NULL);
      if (G_UNLIKELY (show_about_templates))
        {
          /* display the "About Templates" dialog */
          dialog = gtk_dialog_new_with_buttons (_("About Templates"), GTK_WINDOW (window),
                                                GTK_DIALOG_DESTROY_WITH_PARENT
                                                | GTK_DIALOG_NO_SEPARATOR,
                                                GTK_STOCK_OK, GTK_RESPONSE_OK,
                                                NULL);
          gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

          hbox = gtk_hbox_new (FALSE, 6);
          gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
          gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, TRUE, TRUE, 0);
          gtk_widget_show (hbox);

          image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
          gtk_misc_set_alignment (GTK_MISC (image), 0.5f, 0.0f);
          gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
          gtk_widget_show (image);

          vbox = gtk_vbox_new (FALSE, 18);
          gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
          gtk_widget_show (vbox);

          label = gtk_label_new (_("All files in this folder will appear in the \"Create Document\" menu."));
          gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
          gtk_label_set_attributes (GTK_LABEL (label), thunar_pango_attr_list_big_bold ());
          gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
          gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
          gtk_widget_show (label);

          label = gtk_label_new (_("If you frequently create certain kinds of documents, "
                                   "make a copy of one and put it in this folder. Thunar "
                                   "will add an entry for this document in the \"Create "
                                   "Document\" menu.\n\n"
                                   "You can then select the entry from the \"Create "
                                   "Document\" menu and a copy of the document will "
                                   "be created in the directory you are viewing."));
          gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
          gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
          gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
          gtk_widget_show (label);

          button = gtk_check_button_new_with_mnemonic (_("Do _not display this message again"));
          exo_mutual_binding_new_with_negation (G_OBJECT (window->preferences), "misc-show-about-templates", G_OBJECT (button), "active");
          gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
          gtk_widget_show (button);

          gtk_dialog_run (GTK_DIALOG (dialog));
          gtk_widget_destroy (dialog);
        }
    }

  /* display an error dialog if something went wrong */
  if (G_UNLIKELY (error != NULL))
    {
      thunar_dialogs_show_error (GTK_WIDGET (window), error, _("Failed to open the templates folder"));
      g_error_free (error);
    }

  /* release our reference on the ~/Templates path */
  thunar_vfs_path_unref (templates_path);
}



static void
thunar_window_action_open_trash (GtkAction    *action,
                                 ThunarWindow *window)
{
  ThunarVfsPath *trash_bin_path;
  ThunarFile    *trash_bin;
  GError        *error = NULL;

  _thunar_return_if_fail (GTK_IS_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* determine the path to the trash bin */
  trash_bin_path = thunar_vfs_path_get_for_trash ();

  /* determine the file for the trash bin */
  trash_bin = thunar_file_get_for_path (trash_bin_path, &error);
  if (G_UNLIKELY (trash_bin == NULL))
    {
      /* display an error to the user */
      thunar_dialogs_show_error (GTK_WIDGET (window), error, _("Failed to display the contents of the trash can"));
      g_error_free (error);
    }
  else
    {
      /* open the trash folder */
      thunar_window_set_current_directory (window, trash_bin);
      g_object_unref (G_OBJECT (trash_bin));
    }

  /* release our reference on the trash bin path */
  thunar_vfs_path_unref (trash_bin_path);
}



static void
thunar_window_action_open_location (GtkAction    *action,
                                    ThunarWindow *window)
{
  /* just use the "start-open-location" callback */
  thunar_window_start_open_location (window, NULL);
}



static void
thunar_window_action_contents (GtkAction    *action,
                               ThunarWindow *window)
{
  /* display the documentation index */
  thunar_dialogs_show_help (window, NULL, NULL);
}



static void
thunar_window_action_about (GtkAction    *action,
                            ThunarWindow *window)
{
  /* just popup the about dialog */
  thunar_dialogs_show_about (GTK_WINDOW (window), PACKAGE_NAME,
                             _("Thunar is a fast and easy to use file manager\n"
                               "for the Xfce Desktop Environment."));
}



static void
thunar_window_action_show_hidden (GtkToggleAction *action,
                                  ThunarWindow    *window)
{
  _thunar_return_if_fail (GTK_IS_TOGGLE_ACTION (action));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* just emit the "notify" signal for the "show-hidden"
   * signal and the view will automatically sync its state.
   */
  g_object_notify (G_OBJECT (window), "show-hidden");
}



static void
thunar_window_current_directory_changed (ThunarFile   *current_directory,
                                         ThunarWindow *window)
{
  GtkAction *action;
  GdkPixbuf *icon;
  gchar     *title;

  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));
  _thunar_return_if_fail (THUNAR_IS_FILE (current_directory));
  _thunar_return_if_fail (window->current_directory == current_directory);

  /* update the "Empty Trash" action */
  action = gtk_action_group_get_action (window->action_group, "empty-trash");
  gtk_action_set_sensitive (action, (thunar_file_get_size (current_directory) > 0));
  gtk_action_set_visible (action, (thunar_file_is_root (current_directory) && thunar_file_is_trashed (current_directory)));

  /* set window title and icon */
  title = g_strdup_printf ("%s - %s", thunar_file_get_display_name (current_directory), _("File Manager"));
  icon = thunar_icon_factory_load_file_icon (window->icon_factory, current_directory, THUNAR_FILE_ICON_STATE_DEFAULT, 48);
  gtk_window_set_title (GTK_WINDOW (window), title);
  gtk_window_set_icon (GTK_WINDOW (window), icon);
  g_object_unref (G_OBJECT (icon));
  g_free (title);
}



static void
thunar_window_current_directory_destroy (ThunarFile   *current_directory,
                                         ThunarWindow *window)
{
  ThunarVfsPath *path;
  ThunarVfsInfo *info;
  ThunarFile    *file = NULL;

  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));
  _thunar_return_if_fail (THUNAR_IS_FILE (current_directory));
  _thunar_return_if_fail (window->current_directory == current_directory);

  /* determine the path of the current directory */
  path = thunar_file_get_path (current_directory);

  /* determine the first still present parent directory */
  for (path = thunar_vfs_path_get_parent (path); file == NULL && path != NULL; path = thunar_vfs_path_get_parent (path))
    {
      /* try to determine the info for the path */
      info = thunar_vfs_info_new_for_path (path, NULL);
      if (G_LIKELY (info != NULL))
        {
          /* check if we have a directory here */
          if (info->type == THUNAR_VFS_FILE_TYPE_DIRECTORY)
            file = thunar_file_get_for_info (info);

          /* release the file info */
          thunar_vfs_info_unref (info);
        }
    }

  /* check if we have a new folder */
  if (G_LIKELY (file != NULL))
    {
      /* enter the new folder */
      thunar_window_set_current_directory (window, file);

      /* release the file reference */
      g_object_unref (G_OBJECT (file));
    }
}



static void
thunar_window_connect_proxy (GtkUIManager *manager,
                             GtkAction    *action,
                             GtkWidget    *proxy,
                             ThunarWindow *window)
{
  /* we want to get informed when the user hovers a menu item */
  if (GTK_IS_MENU_ITEM (proxy))
    {
      g_signal_connect_closure (G_OBJECT (proxy), "select", window->menu_item_selected_closure, FALSE);
      g_signal_connect_closure (G_OBJECT (proxy), "deselect", window->menu_item_deselected_closure, FALSE);
    }
}



static void
thunar_window_disconnect_proxy (GtkUIManager *manager,
                                GtkAction    *action,
                                GtkWidget    *proxy,
                                ThunarWindow *window)
{
  /* undo what we did in connect_proxy() */
  if (GTK_IS_MENU_ITEM (proxy))
    {
      g_signal_handlers_disconnect_matched (G_OBJECT (proxy), G_SIGNAL_MATCH_CLOSURE, 0, 0, window->menu_item_selected_closure, NULL, NULL);
      g_signal_handlers_disconnect_matched (G_OBJECT (proxy), G_SIGNAL_MATCH_CLOSURE, 0, 0, window->menu_item_deselected_closure, NULL, NULL);
    }
}



static void
thunar_window_menu_item_selected (GtkWidget    *menu_item,
                                  ThunarWindow *window)
{
  GtkAction *action;
  gchar     *tooltip;
  gint       id;

  /* we can only display tooltips if we have a statusbar */
  if (G_LIKELY (window->statusbar != NULL))
    {
      /* determine the action for the menu item */
      action = g_object_get_data (G_OBJECT (menu_item), I_("gtk-action"));
      if (G_UNLIKELY (action == NULL))
        return;

      /* determine the tooltip from the action */
      g_object_get (G_OBJECT (action), "tooltip", &tooltip, NULL);
      if (G_LIKELY (tooltip != NULL))
        {
          id = gtk_statusbar_get_context_id (GTK_STATUSBAR (window->statusbar), "Menu tooltip");
          gtk_statusbar_push (GTK_STATUSBAR (window->statusbar), id, tooltip);
          g_free (tooltip);
        }
    }
}



static void
thunar_window_menu_item_deselected (GtkWidget    *menu_item,
                                    ThunarWindow *window)
{
  gint id;

  /* we can only undisplay tooltips if we have a statusbar */
  if (G_LIKELY (window->statusbar != NULL))
    {
      /* drop the last tooltip from the statusbar */
      id = gtk_statusbar_get_context_id (GTK_STATUSBAR (window->statusbar), "Menu tooltip");
      gtk_statusbar_pop (GTK_STATUSBAR (window->statusbar), id);
    }
}



static void
thunar_window_notify_loading (ThunarView   *view,
                              GParamSpec   *pspec,
                              ThunarWindow *window)
{
  GdkCursor *cursor;

  _thunar_return_if_fail (THUNAR_IS_VIEW (view));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));
  _thunar_return_if_fail (THUNAR_VIEW (window->view) == view);

  if (GTK_WIDGET_REALIZED (window))
    {
      /* setup the proper cursor */
      if (thunar_view_get_loading (view))
        {
          cursor = gdk_cursor_new (GDK_WATCH);
          gdk_window_set_cursor (GTK_WIDGET (window)->window, cursor);
          gdk_cursor_unref (cursor);
        }
      else
        {
          gdk_window_set_cursor (GTK_WIDGET (window)->window, NULL);
        }
    }
}
  


static void
thunar_window_volume_pre_unmount (ThunarVfsVolumeManager *volume_manager,
                                  ThunarVfsVolume        *volume,
                                  ThunarWindow           *window)
{
  ThunarVfsPath *path;
  ThunarFile    *file;
  GtkAction     *action;

  _thunar_return_if_fail (THUNAR_VFS_IS_VOLUME_MANAGER (volume_manager));
  _thunar_return_if_fail (THUNAR_VFS_IS_VOLUME (volume));
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));

  /* nothing to do if we don't have a current directory */
  if (G_UNLIKELY (window->current_directory == NULL))
    return;

  /* determine the mount point for the volume */
  path = thunar_vfs_volume_get_mount_point (volume);
  if (G_UNLIKELY (path == NULL))
    return;

  /* check if a ThunarFile is known for the mount point */
  file = thunar_file_cache_lookup (path);
  if (G_UNLIKELY (file == NULL))
    return;

  /* check if the file is the current directory or an ancestor of the current directory */
  if (window->current_directory == file || thunar_file_is_ancestor (window->current_directory, file))
    {
      /* change to the home folder */
      action = gtk_action_group_get_action (window->action_group, "open-home");
      if (G_LIKELY (action != NULL))
        gtk_action_activate (action);
    }
}



static gboolean
thunar_window_merge_idle (gpointer user_data)
{
  ThunarWindow *window = THUNAR_WINDOW (user_data);

  /* merge custom preferences from the providers */
  GDK_THREADS_ENTER ();
  thunar_window_merge_custom_preferences (window);
  GDK_THREADS_LEAVE ();

  return FALSE;
}



static void
thunar_window_merge_idle_destroy (gpointer user_data)
{
  THUNAR_WINDOW (user_data)->merge_idle_id = 0;
}



static gboolean
thunar_window_save_geometry_timer (gpointer user_data)
{
  GdkWindowState state;
  ThunarWindow  *window = THUNAR_WINDOW (user_data);
  gboolean       remember_geometry;
  gint           width;
  gint           height;

  GDK_THREADS_ENTER ();

  /* check if we should remember the window geometry */
  g_object_get (G_OBJECT (window->preferences), "misc-remember-geometry", &remember_geometry, NULL);
  if (G_LIKELY (remember_geometry))
    {
      /* check if the window is still visible */
      if (GTK_WIDGET_VISIBLE (window))
        {
          /* determine the current state of the window */
          state = gdk_window_get_state (GTK_WIDGET (window)->window);

          /* don't save geometry for maximized or fullscreen windows */
          if ((state & (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN)) == 0)
            {
              /* determine the current width/height of the window... */
              gtk_window_get_size (GTK_WINDOW (window), &width, &height);

              /* ...and remember them as default for new windows */
              g_object_set (G_OBJECT (window->preferences), "last-window-width", width, "last-window-height", height, NULL);
            }
        }
    }

  GDK_THREADS_LEAVE ();

  return FALSE;
}



static void
thunar_window_save_geometry_timer_destroy (gpointer user_data)
{
  THUNAR_WINDOW (user_data)->save_geometry_timer_id = 0;
}



/**
 * thunar_window_new:
 *
 * Allocates a new #ThunarWindow instance, which isn't
 * associated with any directory.
 *
 * Return value: the newly allocated #ThunarWindow instance.
 **/
GtkWidget*
thunar_window_new (void)
{
  return g_object_new (THUNAR_TYPE_WINDOW, NULL);
}



/**
 * thunar_window_get_current_directory:
 * @window : a #ThunarWindow instance.
 * 
 * Queries the #ThunarFile instance, which represents the directory
 * currently displayed within @window. %NULL is returned if @window
 * is not currently associated with any directory.
 *
 * Return value: the directory currently displayed within @window or %NULL.
 **/
ThunarFile*
thunar_window_get_current_directory (ThunarWindow *window)
{
  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), NULL);
  return window->current_directory;
}



/**
 * thunar_window_set_current_directory:
 * @window            : a #ThunarWindow instance.
 * @current_directory : the new directory or %NULL.
 **/
void
thunar_window_set_current_directory (ThunarWindow *window,
                                     ThunarFile   *current_directory)
{
  ThunarFile *file;

  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));
  _thunar_return_if_fail (current_directory == NULL || THUNAR_IS_FILE (current_directory));

  /* check if we already display the requested directory */
  if (G_UNLIKELY (window->current_directory == current_directory))
    return;

  /* disconnect from the previously active directory */
  if (G_LIKELY (window->current_directory != NULL))
    {
      /* determine the fist visible file in the previous directory */
      if (window->view != NULL && thunar_view_get_visible_range (THUNAR_VIEW (window->view), &file, NULL))
        {
          /* add the file to our internal mapping of directories to scroll files */
          g_hash_table_replace (window->scroll_to_files, g_object_ref (G_OBJECT (window->current_directory)), file);
        }

      /* disconnect signals and release reference */
      g_signal_handlers_disconnect_by_func (G_OBJECT (window->current_directory), thunar_window_current_directory_destroy, window);
      g_signal_handlers_disconnect_by_func (G_OBJECT (window->current_directory), thunar_window_current_directory_changed, window);
      g_object_unref (G_OBJECT (window->current_directory));
    }

  /* activate the new directory */
  window->current_directory = current_directory;

  /* connect to the new directory */
  if (G_LIKELY (current_directory != NULL))
    {
      /* take a reference on the file and connect the "changed"/"destroy" signals */
      g_signal_connect (G_OBJECT (current_directory), "changed", G_CALLBACK (thunar_window_current_directory_changed), window);
      g_signal_connect (G_OBJECT (current_directory), "destroy", G_CALLBACK (thunar_window_current_directory_destroy), window);
      g_object_ref (G_OBJECT (current_directory));
    
      /* update window icon and title */
      thunar_window_current_directory_changed (current_directory, window);

      /* grab the focus to the main view */
      if (G_LIKELY (window->view != NULL))
        gtk_widget_grab_focus (window->view);
    }

  /* enable the 'Open new window' action if we have a valid directory */
  thunar_gtk_action_group_set_action_sensitive (window->action_group, "open-new-window", (current_directory != NULL));

  /* enable the 'Up' action if possible for the new directory */
  thunar_gtk_action_group_set_action_sensitive (window->action_group, "open-parent", (current_directory != NULL
                                                && thunar_file_has_parent (current_directory)));

  /* tell everybody that we have a new "current-directory",
   * we do this first so other widgets display the new
   * state already while the folder view is loading.
   */
  g_object_notify (G_OBJECT (window), "current-directory");

  /* check if we have a valid current directory */
  if (G_LIKELY (window->current_directory != NULL))
    {
      /* check if we have a scroll_to_file for the new directory and scroll to the file */
      file = g_hash_table_lookup (window->scroll_to_files, window->current_directory);
      if (G_LIKELY (file != NULL))
        thunar_window_scroll_to_file (window, file, FALSE, TRUE, 0.0f, 0.0f);
    }
}



/**
 * thunar_window_get_zoom_level:
 * @window : a #ThunarWindow instance.
 *
 * Returns the #ThunarZoomLevel for @window.
 *
 * Return value: the #ThunarZoomLevel for @indow.
 **/
ThunarZoomLevel
thunar_window_get_zoom_level (ThunarWindow *window)
{
  _thunar_return_val_if_fail (THUNAR_IS_WINDOW (window), THUNAR_ZOOM_LEVEL_NORMAL);
  return window->zoom_level;
}



/**
 * thunar_window_set_zoom_level:
 * @window     : a #ThunarWindow instance.
 * @zoom_level : the new zoom level for @window.
 *
 * Sets the zoom level for @window to @zoom_level.
 **/
void
thunar_window_set_zoom_level (ThunarWindow   *window,
                              ThunarZoomLevel zoom_level)
{
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));
  _thunar_return_if_fail (zoom_level >= 0 && zoom_level < THUNAR_ZOOM_N_LEVELS);

  /* check if we have a new zoom level */
  if (G_LIKELY (window->zoom_level != zoom_level))
    {
      /* remember the new zoom level */
      window->zoom_level = zoom_level;

      /* update the "Zoom In" and "Zoom Out" actions */
      thunar_gtk_action_group_set_action_sensitive (window->action_group, "zoom-in", (zoom_level < THUNAR_ZOOM_N_LEVELS - 1));
      thunar_gtk_action_group_set_action_sensitive (window->action_group, "zoom-out", (zoom_level > 0));

      /* notify listeners */
      g_object_notify (G_OBJECT (window), "zoom-level");
    }
}



/**
 * thunar_window_scroll_to_file:
 * @window    : a #ThunarWindow instance.
 * @file      : a #ThunarFile.
 * @select    : if %TRUE the @file will also be selected.
 * @use_align : %TRUE to use the alignment arguments.
 * @row_align : the vertical alignment.
 * @col_align : the horizontal alignment.
 *
 * Tells the @window to scroll to the @file
 * in the current view.
 **/
void
thunar_window_scroll_to_file (ThunarWindow *window,
                              ThunarFile   *file,
                              gboolean      select,
                              gboolean      use_align,
                              gfloat        row_align,
                              gfloat        col_align)
{
  _thunar_return_if_fail (THUNAR_IS_WINDOW (window));
  _thunar_return_if_fail (THUNAR_IS_FILE (file));

  /* verify that we have a valid view */
  if (G_LIKELY (window->view != NULL))
    thunar_view_scroll_to_file (THUNAR_VIEW (window->view), file, select, use_align, row_align, col_align);
}


