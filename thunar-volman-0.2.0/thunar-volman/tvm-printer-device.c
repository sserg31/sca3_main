/* $Id: tvm-printer-device.c 2364 2007-01-15 17:57:27Z benny $ */
/*-
 * Copyright (c) 2007 Benedikt Meurer <benny@xfce.org>.
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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <thunar-volman/tvm-printer-device.h>
#include <thunar-volman/tvm-run.h>



/**
 * tvm_printer_device_added:
 * @preferences : a #TvmPreferences.
 * @context     : a #LibHalContext.
 * @udi         : the HAL device UDI of the newly added printer device.
 * @capability  : the capability, which caused this handler to be run.
 * @error       : return location for errors or %NULL.
 *
 * See #TvmDeviceCallback for further information.
 *
 * Return value: %TRUE if handled, %FALSE if not handled or an
 *               unrecoverable error occurred.
 **/
gboolean
tvm_printer_device_added (TvmPreferences *preferences,
                          LibHalContext  *context,
                          const gchar    *udi,
                          const gchar    *capability,
                          GError        **error)
{
  gboolean result = FALSE;
  gboolean autoprinter;
  gchar   *autoprinter_command;

  g_return_val_if_fail (exo_hal_udi_validate (udi, -1, NULL), FALSE);
  g_return_val_if_fail (TVM_IS_PREFERENCES (preferences), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
  g_return_val_if_fail (capability != NULL, FALSE);
  g_return_val_if_fail (context != NULL, FALSE);

  /* check if we have a command to run for newly connected printers */
  g_object_get (G_OBJECT (preferences), "autoprinter", &autoprinter, "autoprinter-command", &autoprinter_command, NULL);
  if (G_LIKELY (autoprinter && autoprinter_command != NULL && *autoprinter_command != '\0'))
    {
      /* run the selected command for the user */
      result = tvm_run_command (context, udi, autoprinter_command, NULL, NULL, error);
    }
  g_free (autoprinter_command);

  return result;
}




