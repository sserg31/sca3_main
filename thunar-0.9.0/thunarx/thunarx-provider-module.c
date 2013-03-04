/* $Id: thunarx-provider-module.c 26421 2007-12-02 13:46:28Z benny $ */
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

#include <gmodule.h>

#include <thunarx/thunarx-private.h>
#include <thunarx/thunarx-provider-module.h>
#include <thunarx/thunarx-provider-plugin.h>
#include <thunarx/thunarx-alias.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_RESIDENT,
};



static void     thunarx_provider_module_class_init    (ThunarxProviderModuleClass  *klass);
static void     thunarx_provider_module_plugin_init   (ThunarxProviderPluginIface  *iface);
static void     thunarx_provider_module_get_property  (GObject                     *object,
                                                       guint                        prop_id,
                                                       GValue                      *value,
                                                       GParamSpec                  *pspec);
static void     thunarx_provider_module_set_property  (GObject                     *object,
                                                       guint                        prop_id,
                                                       const GValue                *value,
                                                       GParamSpec                  *pspec);
static gboolean thunarx_provider_module_load          (GTypeModule                 *type_module);
static void     thunarx_provider_module_unload        (GTypeModule                 *type_module);
static gboolean thunarx_provider_module_get_resident  (const ThunarxProviderPlugin *plugin);
static void     thunarx_provider_module_set_resident  (ThunarxProviderPlugin       *plugin,
                                                       gboolean                     resident);



struct _ThunarxProviderModuleClass
{
  GTypeModuleClass __parent__;
};

struct _ThunarxProviderModule
{
  GTypeModule __parent__;

  GModule *library;
  gboolean resident;

  void (*initialize) (ThunarxProviderModule *module);
  void (*shutdown)   (void);
  void (*list_types) (const GType **types,
                      gint         *n_types);
};



static GObjectClass *thunarx_provider_module_parent_class;



GType
thunarx_provider_module_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ThunarxProviderModuleClass),
        NULL,
        NULL,
        (GClassInitFunc) thunarx_provider_module_class_init,
        NULL,
        NULL,
        sizeof (ThunarxProviderModule),
        0,
        NULL,
        NULL,
      };

      static const GInterfaceInfo plugin_info =
      {
        (GInterfaceInitFunc) thunarx_provider_module_plugin_init,
        NULL,
        NULL,
      };

      type = g_type_register_static (G_TYPE_TYPE_MODULE, I_("ThunarxProviderModule"), &info, 0);
      g_type_add_interface_static (type, THUNARX_TYPE_PROVIDER_PLUGIN, &plugin_info);
    }

  return type;
}



static void
thunarx_provider_module_class_init (ThunarxProviderModuleClass *klass)
{
  GTypeModuleClass *gtype_module_class;
  GObjectClass     *gobject_class;

  /* determine the parent class */
  thunarx_provider_module_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->get_property = thunarx_provider_module_get_property;
  gobject_class->set_property = thunarx_provider_module_set_property;

  gtype_module_class = G_TYPE_MODULE_CLASS (klass);
  gtype_module_class->load = thunarx_provider_module_load;
  gtype_module_class->unload = thunarx_provider_module_unload;

  /* overload ThunarxProviderPlugin's properties */
  g_object_class_override_property (gobject_class,
                                    PROP_RESIDENT,
                                    "resident");
}



static void
thunarx_provider_module_plugin_init (ThunarxProviderPluginIface *iface)
{
  iface->get_resident = thunarx_provider_module_get_resident;
  iface->set_resident = thunarx_provider_module_set_resident;

  /* GTypeModule wrapper implementation */
  iface->register_type = (gpointer) g_type_module_register_type;
  iface->add_interface = (gpointer) g_type_module_add_interface;
  iface->register_enum = (gpointer) g_type_module_register_enum;
  iface->register_flags = (gpointer) g_type_module_register_flags;
}



static void
thunarx_provider_module_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  ThunarxProviderPlugin *plugin = THUNARX_PROVIDER_PLUGIN (object);

  switch (prop_id)
    {
    case PROP_RESIDENT:
      g_value_set_boolean (value, thunarx_provider_plugin_get_resident (plugin));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunarx_provider_module_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  ThunarxProviderPlugin *plugin = THUNARX_PROVIDER_PLUGIN (object);

  switch (prop_id)
    {
    case PROP_RESIDENT:
      thunarx_provider_plugin_set_resident (plugin, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gboolean
thunarx_provider_module_load (GTypeModule *type_module)
{
  ThunarxProviderModule *module = THUNARX_PROVIDER_MODULE (type_module);
  gchar                 *path;

  /* load the module using the runtime link editor */
  path = g_build_filename (THUNARX_DIRECTORY, type_module->name, NULL);
  module->library = g_module_open (path, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
  g_free (path);

  /* check if the load operation was successfull */
  if (G_UNLIKELY (module->library == NULL))
    {
      g_warning ("Failed to load plugin `%s': %s", type_module->name, g_module_error ());
      return FALSE;
    }

  /* verify that all required public symbols are present in the plugin's symbol table */
  if (!g_module_symbol (module->library, "thunar_extension_shutdown", (gpointer) &module->shutdown)
      || !g_module_symbol (module->library, "thunar_extension_initialize", (gpointer) &module->initialize)
      || !g_module_symbol (module->library, "thunar_extension_list_types", (gpointer) &module->list_types))
    {
      g_warning ("Plugin `%s' lacks required symbols.", type_module->name);
      g_module_close (module->library);
      return FALSE;
    }

  /* initialize the plugin */
  (*module->initialize) (module);

  /* ensure that the module will never be unloaded if it requests to be kept in memory */
  if (G_UNLIKELY (module->resident))
    g_module_make_resident (module->library);

  return TRUE;
}



static void
thunarx_provider_module_unload (GTypeModule *type_module)
{
  ThunarxProviderModule *module = THUNARX_PROVIDER_MODULE (type_module);

  /* shutdown the plugin */
  (*module->shutdown) ();

  /* unload the plugin from memory */
  g_module_close (module->library);

  /* reset module state */
  module->library = NULL;
  module->shutdown = NULL;
  module->initialize = NULL;
  module->list_types = NULL;
}



static gboolean
thunarx_provider_module_get_resident (const ThunarxProviderPlugin *plugin)
{
  return THUNARX_PROVIDER_MODULE (plugin)->resident;
}



static void
thunarx_provider_module_set_resident (ThunarxProviderPlugin *plugin,
                                      gboolean               resident)
{
  ThunarxProviderModule *module = THUNARX_PROVIDER_MODULE (plugin);

  if (G_LIKELY (module->resident != resident))
    {
      module->resident = resident;
      g_object_notify (G_OBJECT (module), "resident");
    }
}



/**
 * thunarx_provider_module_new:
 * @filename : the name of the library file.
 *
 * Allocates a new #ThunarxProviderModule for @filename.
 *
 * Return value: the newly allocated #ThunarxProviderModule.
 **/
ThunarxProviderModule*
thunarx_provider_module_new (const gchar *filename)
{
  ThunarxProviderModule *module;

  g_return_val_if_fail (filename != NULL, NULL);
  g_return_val_if_fail (*filename != '\0', NULL);

  module = g_object_new (THUNARX_TYPE_PROVIDER_MODULE, NULL);
  g_type_module_set_name (G_TYPE_MODULE (module), filename);

  return module;
}



/**
 * thunarx_provider_module_list_types:
 * @module  : a #ThunarxProviderModule.
 * @types   : return location for the #GType array pointer.
 * @n_types : return location for the number of types.
 *
 * Determines the #GType<!---->s provided by @module and returns
 * them in @types and @n_types.
 **/
void
thunarx_provider_module_list_types (const ThunarxProviderModule *module,
                                    const GType                **types,
                                    gint                        *n_types)
{
  g_return_if_fail (THUNARX_IS_PROVIDER_MODULE (module));
  g_return_if_fail (module->list_types != NULL);
  g_return_if_fail (n_types != NULL);
  g_return_if_fail (types != NULL);

  (*module->list_types) (types, n_types);
}



#define __THUNARX_PROVIDER_MODULE_C__
#include <thunarx/thunarx-aliasdef.c>
