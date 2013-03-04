/* $Id: thunar-enum-types.c 26421 2007-12-02 13:46:28Z benny $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo/exo.h>

#include <thunar/thunar-enum-types.h>



static void thunar_icon_size_from_zoom_level (const GValue *src_value,
                                              GValue       *dst_value);



GType
thunar_renamer_mode_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_RENAMER_MODE_NAME,   "THUNAR_RENAMER_MODE_NAME",   N_ ("Name only"),       },
        { THUNAR_RENAMER_MODE_SUFFIX, "THUNAR_RENAMER_MODE_SUFFIX", N_ ("Suffix only"),     },
        { THUNAR_RENAMER_MODE_BOTH,   "THUNAR_RENAMER_MODE_BOTH",   N_ ("Name and Suffix"), },
        { 0,                          NULL,                         NULL,                   },
      };

      type = g_enum_register_static (I_("ThunarRenamerMode"), values);
    }

  return type;
}



GType
thunar_color_style_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_COLOR_STYLE_SOLID,     "THUNAR_COLOR_STYLE_SOLID",     "solid",     },
        { THUNAR_COLOR_STYLE_HGRADIENT, "THUNAR_COLOR_STYLE_HGRADIENT", "hgradient", },
        { THUNAR_COLOR_STYLE_VGRADIENT, "THUNAR_COLOR_STYLE_VGRADIENT", "vgradient", },
        { 0,                            NULL,                           NULL,        },
      };

      type = g_enum_register_static (I_("ThunarColorStyle"), values);
    }

  return type;
}



GType
thunar_date_style_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_DATE_STYLE_SIMPLE, "THUNAR_DATE_STYLE_SIMPLE", "simple", },
        { THUNAR_DATE_STYLE_SHORT,  "THUNAR_DATE_STYLE_SHORT",  "short",  },
        { THUNAR_DATE_STYLE_LONG,   "THUNAR_DATE_STYLE_LONG",   "long",   },
        { THUNAR_DATE_STYLE_ISO,    "THUNAR_DATE_STYLE_ISO",    "iso",   },
        { 0,                        NULL,                       NULL,     },
      };

      type = g_enum_register_static (I_("ThunarDateStyle"), values);
    }

  return type;
}



GType
thunar_column_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_COLUMN_DATE_ACCESSED, "THUNAR_COLUMN_DATE_ACCESSED", N_ ("Date Accessed"), },
        { THUNAR_COLUMN_DATE_MODIFIED, "THUNAR_COLUMN_DATE_MODIFIED", N_ ("Date Modified"), },
        { THUNAR_COLUMN_GROUP,         "THUNAR_COLUMN_GROUP",         N_ ("Group"),         },
        { THUNAR_COLUMN_MIME_TYPE,     "THUNAR_COLUMN_MIME_TYPE",     N_ ("MIME Type"),     },
        { THUNAR_COLUMN_NAME,          "THUNAR_COLUMN_NAME",          N_ ("Name"),          },
        { THUNAR_COLUMN_OWNER,         "THUNAR_COLUMN_OWNER",         N_ ("Owner"),         },
        { THUNAR_COLUMN_PERMISSIONS,   "THUNAR_COLUMN_PERMISSIONS",   N_ ("Permissions"),   },
        { THUNAR_COLUMN_SIZE,          "THUNAR_COLUMN_SIZE",          N_ ("Size"),          },
        { THUNAR_COLUMN_TYPE,          "THUNAR_COLUMN_TYPE",          N_ ("Type"),          },
        { THUNAR_COLUMN_FILE,          "THUNAR_COLUMN_FILE",          N_ ("File"),          },
        { THUNAR_COLUMN_FILE_NAME,     "THUNAR_COLUMN_FILE_NAME",     N_ ("File Name"),     },
        { 0,                           NULL,                          NULL,                 },
      };

      type = g_enum_register_static (I_("ThunarColumn"), values);
    }

  return type;
}



GType
thunar_icon_size_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_ICON_SIZE_SMALLEST, "THUNAR_ICON_SIZE_SMALLEST", "smallest", },
        { THUNAR_ICON_SIZE_SMALLER,  "THUNAR_ICON_SIZE_SMALLER",  "smaller",  },
        { THUNAR_ICON_SIZE_SMALL,    "THUNAR_ICON_SIZE_SMALL",    "small",    },
        { THUNAR_ICON_SIZE_NORMAL,   "THUNAR_ICON_SIZE_NORMAL",   "normal",   },
        { THUNAR_ICON_SIZE_LARGE,    "THUNAR_ICON_SIZE_LARGE",    "large",    },
        { THUNAR_ICON_SIZE_LARGER,   "THUNAR_ICON_SIZE_LARGER",   "larger",   },
        { THUNAR_ICON_SIZE_LARGEST,  "THUNAR_ICON_SIZE_LARGEST",  "largest",  },
        { 0,                         NULL,                        NULL,       },
      };

      type = g_enum_register_static (I_("ThunarIconSize"), values);
    }

  return type;
}



GType
thunar_recursive_permissions_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_RECURSIVE_PERMISSIONS_ASK,    "THUNAR_RECURSIVE_PERMISSIONS_ASK",    "ask",    },
        { THUNAR_RECURSIVE_PERMISSIONS_ALWAYS, "THUNAR_RECURSIVE_PERMISSIONS_ALWAYS", "always", },
        { THUNAR_RECURSIVE_PERMISSIONS_NEVER,  "THUNAR_RECURSIVE_PERMISSIONS_NEVER",  "never",  },
        { 0,                                   NULL,                                  NULL,     },
      };

      type = g_enum_register_static (I_("ThunarRecursivePermissions"), values);
    }

  return type;
}



GType
thunar_wallpaper_style_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_WALLPAPER_STYLE_CENTERED,  "THUNAR_WALLPAPER_STYLE_CENTERED",  "centered",  },
        { THUNAR_WALLPAPER_STYLE_SCALED,    "THUNAR_WALLPAPER_STYLE_SCALED",    "scaled",    },
        { THUNAR_WALLPAPER_STYLE_STRETCHED, "THUNAR_WALLPAPER_STYLE_STRETCHED", "stretched", },
        { THUNAR_WALLPAPER_STYLE_TILED,     "THUNAR_WALLPAPER_STYLE_TILED",     "tiled",     },
        { 0,                                NULL,                               NULL,        },
      };

      type = g_enum_register_static (I_("ThunarWallpaperStyle"), values);
    }

  return type;
}



GType
thunar_zoom_level_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GEnumValue values[] =
      {
        { THUNAR_ZOOM_LEVEL_SMALLEST, "THUNAR_ZOOM_LEVEL_SMALLEST", "smallest", },
        { THUNAR_ZOOM_LEVEL_SMALLER,  "THUNAR_ZOOM_LEVEL_SMALLER",  "smaller",  },
        { THUNAR_ZOOM_LEVEL_SMALL,    "THUNAR_ZOOM_LEVEL_SMALL",    "small",    },
        { THUNAR_ZOOM_LEVEL_NORMAL,   "THUNAR_ZOOM_LEVEL_NORMAL",   "normal",   },
        { THUNAR_ZOOM_LEVEL_LARGE,    "THUNAR_ZOOM_LEVEL_LARGE",    "large",    },
        { THUNAR_ZOOM_LEVEL_LARGER,   "THUNAR_ZOOM_LEVEL_LARGER",   "larger",   },
        { THUNAR_ZOOM_LEVEL_LARGEST,  "THUNAR_ZOOM_LEVEL_LARGEST",  "largest",  },
        { 0,                          NULL,                         NULL,       },
      };

      type = g_enum_register_static (I_("ThunarZoomLevel"), values);

      /* register transformation function for ThunarZoomLevel->ThunarIconSize */
      g_value_register_transform_func (type, THUNAR_TYPE_ICON_SIZE, thunar_icon_size_from_zoom_level);
    }

  return type;
}



/**
 * thunar_zoom_level_to_icon_size:
 * @zoom_level : a #ThunarZoomLevel.
 *
 * Returns the #ThunarIconSize corresponding to the @zoom_level.
 *
 * Return value: the #ThunarIconSize for @zoom_level.
 **/
ThunarIconSize
thunar_zoom_level_to_icon_size (ThunarZoomLevel zoom_level)
{
  switch (zoom_level)
    {
    case THUNAR_ZOOM_LEVEL_SMALLEST: return THUNAR_ICON_SIZE_SMALLEST;
    case THUNAR_ZOOM_LEVEL_SMALLER:  return THUNAR_ICON_SIZE_SMALLER;
    case THUNAR_ZOOM_LEVEL_SMALL:    return THUNAR_ICON_SIZE_SMALL;
    case THUNAR_ZOOM_LEVEL_NORMAL:   return THUNAR_ICON_SIZE_NORMAL;
    case THUNAR_ZOOM_LEVEL_LARGE:    return THUNAR_ICON_SIZE_LARGE;
    case THUNAR_ZOOM_LEVEL_LARGER:   return THUNAR_ICON_SIZE_LARGER;
    default:                         return THUNAR_ICON_SIZE_LARGEST;
    }
}



static void
thunar_icon_size_from_zoom_level (const GValue *src_value,
                                  GValue       *dst_value)
{
  g_value_set_enum (dst_value, thunar_zoom_level_to_icon_size (g_value_get_enum (src_value)));
}




