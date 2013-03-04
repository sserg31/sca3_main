/* $Id: thunar-shortcuts-icon-renderer.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>.
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

#include <thunar/thunar-gobject-extensions.h>
#include <thunar/thunar-icon-factory.h>
#include <thunar/thunar-shortcuts-icon-renderer.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_VOLUME,
};



static void thunar_shortcuts_icon_renderer_class_init   (ThunarShortcutsIconRendererClass *klass);
static void thunar_shortcuts_icon_renderer_init         (ThunarShortcutsIconRenderer      *shortcuts_icon_renderer);
static void thunar_shortcuts_icon_renderer_finalize     (GObject                          *object);
static void thunar_shortcuts_icon_renderer_get_property (GObject                          *object,
                                                         guint                             prop_id,
                                                         GValue                           *value,
                                                         GParamSpec                       *pspec);
static void thunar_shortcuts_icon_renderer_set_property (GObject                          *object,
                                                         guint                             prop_id,
                                                         const GValue                     *value,
                                                         GParamSpec                       *pspec);
static void thunar_shortcuts_icon_renderer_render       (GtkCellRenderer                  *renderer,
                                                         GdkWindow                        *window,
                                                         GtkWidget                        *widget,
                                                         GdkRectangle                     *background_area,
                                                         GdkRectangle                     *cell_area,
                                                         GdkRectangle                     *expose_area,
                                                         GtkCellRendererState              flags);



struct _ThunarShortcutsIconRendererClass
{
  ThunarIconRendererClass __parent__;
};

struct _ThunarShortcutsIconRenderer
{
  ThunarIconRenderer __parent__;

  ThunarVfsVolume   *volume;
};



static GObjectClass *thunar_shortcuts_icon_renderer_parent_class;



GType
thunar_shortcuts_icon_renderer_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarShortcutsIconRendererClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_shortcuts_icon_renderer_class_init,
        NULL,
        NULL,
        sizeof (ThunarShortcutsIconRenderer),
        0,
        (GInstanceInitFunc) thunar_shortcuts_icon_renderer_init,
        NULL,
      };

      type = g_type_register_static (THUNAR_TYPE_ICON_RENDERER, I_("ThunarShortcutsIconRenderer"), &info, 0);
    }

  return type;
}



static void
thunar_shortcuts_icon_renderer_class_init (ThunarShortcutsIconRendererClass *klass)
{
  GtkCellRendererClass *gtkcell_renderer_class;
  GObjectClass         *gobject_class;

  /* determine the parent type class */
  thunar_shortcuts_icon_renderer_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_shortcuts_icon_renderer_finalize;
  gobject_class->get_property = thunar_shortcuts_icon_renderer_get_property;
  gobject_class->set_property = thunar_shortcuts_icon_renderer_set_property;

  gtkcell_renderer_class = GTK_CELL_RENDERER_CLASS (klass);
  gtkcell_renderer_class->render = thunar_shortcuts_icon_renderer_render;

  /**
   * ThunarShortcutsIconRenderer:volume:
   *
   * The #ThunarVfsVolume for which to render an icon or %NULL to fallback
   * to the default icon renderering (see #ThunarIconRenderer).
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_VOLUME,
                                   g_param_spec_object ("volume", "volume", "volume",
                                                        THUNAR_VFS_TYPE_VOLUME,
                                                        EXO_PARAM_READWRITE));
}



static void
thunar_shortcuts_icon_renderer_init (ThunarShortcutsIconRenderer *shortcuts_icon_renderer)
{
  /* no padding please */
  GTK_CELL_RENDERER (shortcuts_icon_renderer)->xpad = 0;
  GTK_CELL_RENDERER (shortcuts_icon_renderer)->ypad = 0;
}



static void
thunar_shortcuts_icon_renderer_finalize (GObject *object)
{
  ThunarShortcutsIconRenderer *shortcuts_icon_renderer = THUNAR_SHORTCUTS_ICON_RENDERER (object);

  /* release the volume (if any) */
  if (G_UNLIKELY (shortcuts_icon_renderer->volume != NULL))
    g_object_unref (G_OBJECT (shortcuts_icon_renderer->volume));

  (*G_OBJECT_CLASS (thunar_shortcuts_icon_renderer_parent_class)->finalize) (object);
}



static void
thunar_shortcuts_icon_renderer_get_property (GObject    *object,
                                             guint       prop_id,
                                             GValue     *value,
                                             GParamSpec *pspec)
{
  ThunarShortcutsIconRenderer *shortcuts_icon_renderer = THUNAR_SHORTCUTS_ICON_RENDERER (object);

  switch (prop_id)
    {
    case PROP_VOLUME:
      g_value_set_object (value, shortcuts_icon_renderer->volume);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_shortcuts_icon_renderer_set_property (GObject      *object,
                                             guint         prop_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
{
  ThunarShortcutsIconRenderer *shortcuts_icon_renderer = THUNAR_SHORTCUTS_ICON_RENDERER (object);

  switch (prop_id)
    {
    case PROP_VOLUME:
      if (G_UNLIKELY (shortcuts_icon_renderer->volume != NULL))
        g_object_unref (G_OBJECT (shortcuts_icon_renderer->volume));
      shortcuts_icon_renderer->volume = (gpointer) g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_shortcuts_icon_renderer_render (GtkCellRenderer     *renderer,
                                       GdkWindow           *window,
                                       GtkWidget           *widget,
                                       GdkRectangle        *background_area,
                                       GdkRectangle        *cell_area,
                                       GdkRectangle        *expose_area,
                                       GtkCellRendererState flags)
{
  ThunarShortcutsIconRenderer *shortcuts_icon_renderer = THUNAR_SHORTCUTS_ICON_RENDERER (renderer);
  ThunarIconFactory           *icon_factory;
  GtkIconTheme                *icon_theme;
  GdkRectangle                 draw_area;
  GdkRectangle                 icon_area;
  const gchar                 *icon_name;
  GdkPixbuf                   *icon;
  GdkPixbuf                   *temp;

  /* check if we have a volume set */
  if (G_UNLIKELY (shortcuts_icon_renderer->volume != NULL))
    {
      /* load the volume icon */
      icon_theme = gtk_icon_theme_get_for_screen (gdk_drawable_get_screen (window));
      icon_factory = thunar_icon_factory_get_for_icon_theme (icon_theme);
      icon_name = thunar_vfs_volume_lookup_icon_name (shortcuts_icon_renderer->volume, icon_theme);
      icon = thunar_icon_factory_load_icon (icon_factory, icon_name, THUNAR_ICON_RENDERER (renderer)->size, NULL, FALSE);

      /* render the icon (if any) */
      if (G_LIKELY (icon != NULL))
        {
          /* determine the real icon size */
          icon_area.width = gdk_pixbuf_get_width (icon);
          icon_area.height = gdk_pixbuf_get_height (icon);

          /* scale down the icon on-demand */
          if (G_UNLIKELY (icon_area.width > cell_area->width || icon_area.height > cell_area->height))
            {
              /* scale down to fit */
              temp = exo_gdk_pixbuf_scale_down (icon, TRUE, cell_area->width, cell_area->height);
              g_object_unref (G_OBJECT (icon));
              icon = temp;

              /* determine the icon dimensions again */
              icon_area.width = gdk_pixbuf_get_width (icon);
              icon_area.height = gdk_pixbuf_get_height (icon);
            }

          icon_area.x = cell_area->x + (cell_area->width - icon_area.width) / 2;
          icon_area.y = cell_area->y + (cell_area->height - icon_area.height) / 2;

          /* check whether the icon is affected by the expose event */
          if (gdk_rectangle_intersect (expose_area, &icon_area, &draw_area))
            {
              /* render the invalid parts of the icon */
              gdk_draw_pixbuf (window, widget->style->black_gc, icon,
                               draw_area.x - icon_area.x, draw_area.y - icon_area.y,
                               draw_area.x, draw_area.y, draw_area.width, draw_area.height,
                               GDK_RGB_DITHER_NORMAL, 0, 0);
            }

          /* cleanup */
          g_object_unref (G_OBJECT (icon));
        }

      /* cleanup */
      g_object_unref (G_OBJECT (icon_factory));
    }
  else
    {
      /* fallback to the default icon renderering */
      (*GTK_CELL_RENDERER_CLASS (thunar_shortcuts_icon_renderer_parent_class)->render) (renderer, window, widget, background_area,
                                                                                        cell_area, expose_area, flags);
    }
}



/**
 * thunar_shortcuts_icon_renderer_new:
 *
 * Allocates a new #ThunarShortcutsIconRenderer instance.
 *
 * Return value: the newly allocated #ThunarShortcutsIconRenderer.
 **/
GtkCellRenderer*
thunar_shortcuts_icon_renderer_new (void)
{
  return g_object_new (THUNAR_TYPE_SHORTCUTS_ICON_RENDERER, NULL);
}

