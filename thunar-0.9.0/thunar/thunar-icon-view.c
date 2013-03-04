/* $Id: thunar-icon-view.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>
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

#include <thunar/thunar-icon-view.h>
#include <thunar/thunar-private.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_TEXT_BESIDE_ICONS,
};



static void         thunar_icon_view_class_init             (ThunarIconViewClass *klass);
static void         thunar_icon_view_init                   (ThunarIconView      *icon_view);
static void         thunar_icon_view_set_property           (GObject             *object,
                                                             guint                prop_id,
                                                             const GValue        *value,
                                                             GParamSpec          *pspec);
static AtkObject   *thunar_icon_view_get_accessible         (GtkWidget           *widget);
static void         thunar_icon_view_zoom_level_changed     (ThunarStandardView  *standard_view);



struct _ThunarIconViewClass
{
  ThunarAbstractIconViewClass __parent__;
};

struct _ThunarIconView
{
  ThunarAbstractIconView __parent__;
};



static GObjectClass *thunar_icon_view_parent_class;



GType
thunar_icon_view_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarIconViewClass),
        NULL,
        NULL,
        (GClassInitFunc) thunar_icon_view_class_init,
        NULL,
        NULL,
        sizeof (ThunarIconView),
        0,
        (GInstanceInitFunc) thunar_icon_view_init,
        NULL,
      };

      type = g_type_register_static (THUNAR_TYPE_ABSTRACT_ICON_VIEW, I_("ThunarIconView"), &info, 0);
    }

  return type;
}



static void
thunar_icon_view_class_init (ThunarIconViewClass *klass)
{
  ThunarStandardViewClass *thunarstandard_view_class;
  GtkWidgetClass          *gtkwidget_class;
  GObjectClass            *gobject_class;

  /* determine the parent type class */
  thunar_icon_view_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->set_property = thunar_icon_view_set_property;

  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->get_accessible = thunar_icon_view_get_accessible;

  thunarstandard_view_class = THUNAR_STANDARD_VIEW_CLASS (klass);
  thunarstandard_view_class->zoom_level_property_name = "last-icon-view-zoom-level";

  /**
   * ThunarIconView::text-beside-icons:
   *
   * Write-only property to specify whether text should be
   * display besides the icon rather than below.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT_BESIDE_ICONS,
                                   g_param_spec_boolean ("text-beside-icons",
                                                         "text-beside-icons",
                                                         "text-beside-icons",
                                                         FALSE,
                                                         EXO_PARAM_WRITABLE));
}



static void
thunar_icon_view_init (ThunarIconView *icon_view)
{
  /* setup the icon renderer */
  g_object_set (G_OBJECT (THUNAR_STANDARD_VIEW (icon_view)->icon_renderer),
                "ypad", 3u,
                NULL);

  /* setup the name renderer */
  g_object_set (G_OBJECT (THUNAR_STANDARD_VIEW (icon_view)->name_renderer),
                "wrap-mode", PANGO_WRAP_WORD_CHAR,
                NULL);

  /* synchronize the "text-beside-icons" property with the global preference */
  exo_binding_new (G_OBJECT (THUNAR_STANDARD_VIEW (icon_view)->preferences), "misc-text-beside-icons", G_OBJECT (icon_view), "text-beside-icons");
}



static void
thunar_icon_view_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  ThunarStandardView *standard_view = THUNAR_STANDARD_VIEW (object);

  switch (prop_id)
    {
    case PROP_TEXT_BESIDE_ICONS:
      if (G_UNLIKELY (g_value_get_boolean (value)))
        {
          exo_icon_view_set_orientation (EXO_ICON_VIEW (GTK_BIN (standard_view)->child), GTK_ORIENTATION_HORIZONTAL);
          g_object_set (G_OBJECT (standard_view->name_renderer), "wrap-width", 128, "yalign", 0.5f, NULL);

          /* disconnect the "zoom-level" signal handler, since we're using a fixed wrap-width here */
          g_signal_handlers_disconnect_by_func (object, thunar_icon_view_zoom_level_changed, NULL);
        }
      else
        {
          exo_icon_view_set_orientation (EXO_ICON_VIEW (GTK_BIN (standard_view)->child), GTK_ORIENTATION_VERTICAL);
          g_object_set (G_OBJECT (standard_view->name_renderer), "yalign", 0.0f, NULL);

          /* connect the "zoom-level" signal handler as the wrap-width is now synced with the "zoom-level" */
          g_signal_connect (object, "notify::zoom-level", G_CALLBACK (thunar_icon_view_zoom_level_changed), NULL);
          thunar_icon_view_zoom_level_changed (standard_view);
        }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static AtkObject*
thunar_icon_view_get_accessible (GtkWidget *widget)
{
  AtkObject *object;

  /* query the atk object for the icon view class */
  object = (*GTK_WIDGET_CLASS (thunar_icon_view_parent_class)->get_accessible) (widget);

  /* set custom Atk properties for the icon view */
  if (G_LIKELY (object != NULL))
    {
      atk_object_set_description (object, _("Icon based directory listing"));
      atk_object_set_name (object, _("Icon view"));
      atk_object_set_role (object, ATK_ROLE_DIRECTORY_PANE);
    }

  return object;
}



static void
thunar_icon_view_zoom_level_changed (ThunarStandardView *standard_view)
{
  gint wrap_width;

  _thunar_return_if_fail (THUNAR_IS_STANDARD_VIEW (standard_view));

  /* determine the "wrap-width" depending on the "zoom-level" */
  switch (thunar_view_get_zoom_level (THUNAR_VIEW (standard_view)))
    {
    case THUNAR_ZOOM_LEVEL_SMALLEST:
      wrap_width = 48;
      break;

    case THUNAR_ZOOM_LEVEL_SMALLER:
      wrap_width = 64;
      break;

    case THUNAR_ZOOM_LEVEL_SMALL:
      wrap_width = 72;
      break;

    case THUNAR_ZOOM_LEVEL_NORMAL:
      wrap_width = 112;
      break;

    default:
      wrap_width = 128;
      break;
    }

  /* set the new "wrap-width" for the text renderer */
  g_object_set (G_OBJECT (standard_view->name_renderer), "wrap-width", wrap_width, NULL);
}




