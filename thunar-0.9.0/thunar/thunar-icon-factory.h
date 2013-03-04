/* $Id: thunar-icon-factory.h 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
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

#ifndef __THUNAR_ICON_FACTORY_H__
#define __THUNAR_ICON_FACTORY_H__

#include <thunar/thunar-file.h>

G_BEGIN_DECLS;

typedef struct _ThunarIconFactoryClass ThunarIconFactoryClass;
typedef struct _ThunarIconFactory      ThunarIconFactory;

#define THUNAR_TYPE_ICON_FACTORY            (thunar_icon_factory_get_type ())
#define THUNAR_ICON_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), THUNAR_TYPE_ICON_FACTORY, ThunarIconFactory))
#define THUNAR_ICON_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), THUNAR_TYPE_ICON_FACTORY, ThunarIconFactoryClass))
#define THUNAR_IS_ICON_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THUNAR_TYPE_ICON_FACTORY))
#define THUNAR_IS_ICON_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), THUNAR_TYPE_ICON_FACTORY))
#define THUNAR_ICON_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), THUNAR_TYPE_ICON_FACTORY, ThunarIconFactoryClass))

/**
 * THUNAR_THUMBNAIL_SIZE:
 * The icon size which is used for loading and storing
 * thumbnails in Thunar.
 **/
#define THUNAR_THUMBNAIL_SIZE (128)

/**
 * THUNAR_EMBLEM_MAX_ATTACH_POINTS:
 * The maximum number of attach points for icons managed
 * by the #ThunarIconFactory.
 **/
#define THUNAR_EMBLEM_MAX_ATTACH_POINTS (12)

/**
 * ThunarEmblemAttachPoints:
 * Holds the emblem attach points for a given icon.
 */
typedef struct
{
	gint     num_points;
	GdkPoint points[THUNAR_EMBLEM_MAX_ATTACH_POINTS];
} ThunarEmblemAttachPoints;

GType                  thunar_icon_factory_get_type           (void) G_GNUC_CONST;

ThunarIconFactory     *thunar_icon_factory_get_default        (void);
ThunarIconFactory     *thunar_icon_factory_get_for_icon_theme (GtkIconTheme             *icon_theme);

GtkIconTheme          *thunar_icon_factory_get_icon_theme     (const ThunarIconFactory  *factory);

#if 0
ThunarVfsThumbFactory *thunar_icon_factory_get_thumb_factory  (const ThunarIconFactory  *factory);
#endif

GdkPixbuf             *thunar_icon_factory_load_icon          (ThunarIconFactory        *factory,
                                                               const gchar              *name,
                                                               gint                      size,
                                                               ThunarEmblemAttachPoints *attach_points,
                                                               gboolean                  wants_default);

GdkPixbuf             *thunar_icon_factory_load_file_icon     (ThunarIconFactory        *factory,
                                                               ThunarFile               *file,
                                                               ThunarFileIconState       icon_state,
                                                               gint                      icon_size);

G_END_DECLS;

#endif /* !__THUNAR_ICON_FACTORY_H__ */
