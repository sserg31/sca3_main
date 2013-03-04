/* $Id: thunarx-preferences-provider.c 26421 2007-12-02 13:46:28Z benny $ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <thunarx/thunarx-preferences-provider.h>
#include <thunarx/thunarx-private.h>
#include <thunarx/thunarx-alias.h>



GType
thunarx_preferences_provider_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarxPreferencesProviderIface),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        0,
        NULL,
        NULL,
      };

      /* register the preferences provider interface */
      type = g_type_register_static (G_TYPE_INTERFACE, I_("ThunarxPreferencesProvider"), &info, 0);
      g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

  return type;
}



/**
 * thunarx_preferences_provider_get_actions:
 * @provider : a #ThunarxPreferencesProvider.
 * @window   : the #GtkWindow within which the actions will be used.
 *
 * Returns the list of #GtkAction<!---->s that @provider has to offer
 * as preferences within @window. These actions will usually be added
 * to the builtin list of preferences in the "Edit" menu of the file
 * manager's @window.
 *
 * Plugin writers that implement this interface should make sure to
 * choose descriptive action names and tooltips, and not to crowd the
 * "Edit" menu too much. That said, think twice before implementing
 * this interface, as too many preference actions will render the
 * file manager useless over time!
 *
 * The caller is responsible to free the returned list of actions using
 * something like this when no longer needed:
 * <informalexample><programlisting>
 * g_list_foreach (list, (GFunc) g_object_unref, NULL);
 * g_list_free (list);
 * </programlisting></informalexample>
 *
 * Return value: the list of #GtkAction<!---->s that @provider has
 *               to offer as preferences within @window.
 **/
GList*
thunarx_preferences_provider_get_actions (ThunarxPreferencesProvider *provider,
                                          GtkWidget                  *window)
{
  GList *actions;

  g_return_val_if_fail (THUNARX_IS_PREFERENCES_PROVIDER (provider), NULL);
  g_return_val_if_fail (GTK_IS_WINDOW (window), NULL);

  if (THUNARX_PREFERENCES_PROVIDER_GET_IFACE (provider)->get_actions != NULL)
    {
      /* query the actions from the implementation */
      actions = (*THUNARX_PREFERENCES_PROVIDER_GET_IFACE (provider)->get_actions) (provider, window);

      /* take a reference on the provider for each action */
      thunarx_object_list_take_reference (actions, provider);
    }
  else
    {
      actions = NULL;
    }

  return actions;
}



#define __THUNARX_PREFERENCES_PROVIDER_C__
#include <thunarx/thunarx-aliasdef.c>
