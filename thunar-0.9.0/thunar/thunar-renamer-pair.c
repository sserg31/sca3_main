/* $Id: thunar-renamer-pair.c 26421 2007-12-02 13:46:28Z benny $ */
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

#include <thunar/thunar-private.h>
#include <thunar/thunar-renamer-pair.h>



GType
thunar_renamer_pair_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = g_boxed_type_register_static (I_("ThunarRenamerPair"),
                                           (GBoxedCopyFunc) thunar_renamer_pair_copy,
                                           (GBoxedFreeFunc) thunar_renamer_pair_free);
    }

  return type;
}



/**
 * thunar_renamer_pair_new:
 * @file : a #ThunarFile.
 * @name : the new name for @file.
 *
 * Allocates a new #ThunarRenamerPair for the
 * given @file and @name.
 *
 * The caller is responsible to free the returned pair
 * using thunar_renamer_pair_free() when no longer
 * needed.
 *
 * Return value: the newly allocated #ThunarRenamerPair.
 **/
ThunarRenamerPair*
thunar_renamer_pair_new (ThunarFile  *file,
                         const gchar *name)
{
  ThunarRenamerPair *renamer_pair;

  _thunar_return_val_if_fail (THUNAR_IS_FILE (file), NULL);
  _thunar_return_val_if_fail (g_utf8_validate (name, -1, NULL), NULL);

  renamer_pair = _thunar_slice_new (ThunarRenamerPair);
  renamer_pair->file = g_object_ref (G_OBJECT (file));
  renamer_pair->name = g_strdup (name);

  return renamer_pair;
}



/**
 * thunar_renamer_pair_copy:
 * @renamer_pair : a #ThunarRenamerPair.
 *
 * Allocates a copy of the specified @renamer_pair.
 *
 * The caller is responsible to free the returned pair
 * using thunar_renamer_pair_free() when no longer
 * needed.
 *
 * Return value: the newly allocated copy of @renamer_pair.
 **/
ThunarRenamerPair*
thunar_renamer_pair_copy (ThunarRenamerPair *renamer_pair)
{
  _thunar_return_val_if_fail (renamer_pair != NULL, NULL);
  return thunar_renamer_pair_new (renamer_pair->file, renamer_pair->name);
}



/**
 * thunar_renamer_pair_free:
 * @renamer_pair : a #ThunarRenamerPair.
 *
 * Frees the specified @renamer_pair.
 **/
void
thunar_renamer_pair_free (ThunarRenamerPair *renamer_pair)
{
  if (G_LIKELY (renamer_pair != NULL))
    {
      g_object_unref (G_OBJECT (renamer_pair->file));
      g_free (renamer_pair->name);
      _thunar_slice_free (ThunarRenamerPair, renamer_pair);
    }
}



/**
 * thunar_renamer_pair_list_copy:
 * @renamer_pair_list : a #GList of #ThunarRenamerPair<!---->s.
 *
 * Takes a deep copy of the @renamer_pair_list.
 *
 * The caller is responsible to free the returned list using
 * thunar_renamer_pair_list_free() when no longer needed.
 *
 * Return value: a deep copy of @renamer_pair_list.
 **/
GList*
thunar_renamer_pair_list_copy (GList *renamer_pair_list)
{
  GList *result = NULL;
  GList *lp;

  for (lp = g_list_last (renamer_pair_list); lp != NULL; lp = lp->prev)
    result = g_list_prepend (result, thunar_renamer_pair_copy (lp->data));

  return result;
}
 


/**
 * thunar_renamer_pair_list_free:
 * @renamer_pair_list : a #GList of #ThunarRenamerPair<!---->s.
 *
 * Releases the @renamer_pair_list and all #ThunarRenamerPair<!---->s
 * in the list.
 **/
void
thunar_renamer_pair_list_free (GList *renamer_pair_list)
{
  g_list_foreach (renamer_pair_list, (GFunc) thunar_renamer_pair_free, NULL);
  g_list_free (renamer_pair_list);
}


