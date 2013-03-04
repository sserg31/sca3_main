/* $Id: generate.c 20833 2006-04-09 18:08:17Z benny $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <gtk/gtk.h>


static GdkPixbuf*
create_slide (GdkPixbuf *base, int steps)
{
  GdkPixbuf *result;
  int rw, rh;
  int bw, bh;
  int i;
  guchar *pixels, *p;
  int rowstride;
  int n_channels;
  int x, y;

  bw = gdk_pixbuf_get_width (base);
  bh = gdk_pixbuf_get_height (base);

  rw = bw * steps;
  rh = bh;

  result = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, rw, rh);
  rowstride = gdk_pixbuf_get_rowstride (result);
  pixels = gdk_pixbuf_get_pixels (result);
  n_channels = gdk_pixbuf_get_n_channels (result);

  for (i = 0; i < steps; ++i)
    {
      gdk_pixbuf_copy_area (base, 0, 0, bw, bh,
                            result, i * bw, 0);

      for (x = 0; x < bw; ++x)
        {
          for (y = 0; y < bh; ++y)
            {
              p = pixels + y * rowstride + (i * bw + x) * n_channels;
              p[3] = ((i + 1) * p[3]) / (steps + 1);
            }
        }
    }

  return result;
}


int main (int argc, char **argv)
{
  GdkPixbuf *base;
  GdkPixbuf *result;

  gtk_init (&argc, &argv);

  if (argc != 3)
    { 
      fprintf (stderr, "Usage: generate <file> <steps>\n");
      return EXIT_FAILURE;
    }

  base = gdk_pixbuf_new_from_file (argv[1], NULL);
  if (base == NULL)
    {
      fprintf (stderr, "generate: Unable to open file %s\n", argv[1]);
      return EXIT_FAILURE;
    }

  result = create_slide (base, atoi (argv[2]));

  gdk_pixbuf_save (result, "slide.png", "png", NULL, NULL);

  printf ("generate: New slide written to slide.png successfully.\n");

  return EXIT_SUCCESS;
}
