/* Generated by dbus-binding-tool; do not edit! */


#ifndef __dbus_glib_marshal_thunar_dbus_service_MARSHAL_H__
#define __dbus_glib_marshal_thunar_dbus_service_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

#ifdef G_ENABLE_DEBUG
#define g_marshal_value_peek_boolean(v)  g_value_get_boolean (v)
#define g_marshal_value_peek_char(v)     g_value_get_char (v)
#define g_marshal_value_peek_uchar(v)    g_value_get_uchar (v)
#define g_marshal_value_peek_int(v)      g_value_get_int (v)
#define g_marshal_value_peek_uint(v)     g_value_get_uint (v)
#define g_marshal_value_peek_long(v)     g_value_get_long (v)
#define g_marshal_value_peek_ulong(v)    g_value_get_ulong (v)
#define g_marshal_value_peek_int64(v)    g_value_get_int64 (v)
#define g_marshal_value_peek_uint64(v)   g_value_get_uint64 (v)
#define g_marshal_value_peek_enum(v)     g_value_get_enum (v)
#define g_marshal_value_peek_flags(v)    g_value_get_flags (v)
#define g_marshal_value_peek_float(v)    g_value_get_float (v)
#define g_marshal_value_peek_double(v)   g_value_get_double (v)
#define g_marshal_value_peek_string(v)   (char*) g_value_get_string (v)
#define g_marshal_value_peek_param(v)    g_value_get_param (v)
#define g_marshal_value_peek_boxed(v)    g_value_get_boxed (v)
#define g_marshal_value_peek_pointer(v)  g_value_get_pointer (v)
#define g_marshal_value_peek_object(v)   g_value_get_object (v)
#else /* !G_ENABLE_DEBUG */
/* WARNING: This code accesses GValues directly, which is UNSUPPORTED API.
 *          Do not access GValues directly in your code. Instead, use the
 *          g_value_get_*() functions
 */
#define g_marshal_value_peek_boolean(v)  (v)->data[0].v_int
#define g_marshal_value_peek_char(v)     (v)->data[0].v_int
#define g_marshal_value_peek_uchar(v)    (v)->data[0].v_uint
#define g_marshal_value_peek_int(v)      (v)->data[0].v_int
#define g_marshal_value_peek_uint(v)     (v)->data[0].v_uint
#define g_marshal_value_peek_long(v)     (v)->data[0].v_long
#define g_marshal_value_peek_ulong(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_int64(v)    (v)->data[0].v_int64
#define g_marshal_value_peek_uint64(v)   (v)->data[0].v_uint64
#define g_marshal_value_peek_enum(v)     (v)->data[0].v_long
#define g_marshal_value_peek_flags(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_float(v)    (v)->data[0].v_float
#define g_marshal_value_peek_double(v)   (v)->data[0].v_double
#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_param(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_boxed(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_pointer(v)  (v)->data[0].v_pointer
#define g_marshal_value_peek_object(v)   (v)->data[0].v_pointer
#endif /* !G_ENABLE_DEBUG */


/* BOOLEAN:STRING,BOOLEAN,STRING,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:1) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOOLEAN_STRING_POINTER (GClosure     *closure,
                                                                                          GValue       *return_value,
                                                                                          guint         n_param_values,
                                                                                          const GValue *param_values,
                                                                                          gpointer      invocation_hint,
                                                                                          gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOOLEAN_STRING_POINTER (GClosure     *closure,
                                                                              GValue       *return_value G_GNUC_UNUSED,
                                                                              guint         n_param_values,
                                                                              const GValue *param_values,
                                                                              gpointer      invocation_hint G_GNUC_UNUSED,
                                                                              gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_BOOLEAN_STRING_POINTER) (gpointer     data1,
                                                                           gpointer     arg_1,
                                                                           gboolean     arg_2,
                                                                           gpointer     arg_3,
                                                                           gpointer     arg_4,
                                                                           gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_BOOLEAN_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 5);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_BOOLEAN_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_boolean (param_values + 2),
                       g_marshal_value_peek_string (param_values + 3),
                       g_marshal_value_peek_pointer (param_values + 4),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:POINTER,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:2) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__POINTER_POINTER (GClosure     *closure,
                                                                            GValue       *return_value,
                                                                            guint         n_param_values,
                                                                            const GValue *param_values,
                                                                            gpointer      invocation_hint,
                                                                            gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__POINTER_POINTER (GClosure     *closure,
                                                                GValue       *return_value G_GNUC_UNUSED,
                                                                guint         n_param_values,
                                                                const GValue *param_values,
                                                                gpointer      invocation_hint G_GNUC_UNUSED,
                                                                gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__POINTER_POINTER) (gpointer     data1,
                                                             gpointer     arg_1,
                                                             gpointer     arg_2,
                                                             gpointer     data2);
  register GMarshalFunc_BOOLEAN__POINTER_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__POINTER_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_pointer (param_values + 1),
                       g_marshal_value_peek_pointer (param_values + 2),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,BOXED,BOOLEAN,STRING,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:3) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOXED_BOOLEAN_STRING_POINTER (GClosure     *closure,
                                                                                                GValue       *return_value,
                                                                                                guint         n_param_values,
                                                                                                const GValue *param_values,
                                                                                                gpointer      invocation_hint,
                                                                                                gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOXED_BOOLEAN_STRING_POINTER (GClosure     *closure,
                                                                                    GValue       *return_value G_GNUC_UNUSED,
                                                                                    guint         n_param_values,
                                                                                    const GValue *param_values,
                                                                                    gpointer      invocation_hint G_GNUC_UNUSED,
                                                                                    gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_BOXED_BOOLEAN_STRING_POINTER) (gpointer     data1,
                                                                                 gpointer     arg_1,
                                                                                 gpointer     arg_2,
                                                                                 gboolean     arg_3,
                                                                                 gpointer     arg_4,
                                                                                 gpointer     arg_5,
                                                                                 gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_BOXED_BOOLEAN_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 6);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_BOXED_BOOLEAN_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_boxed (param_values + 2),
                       g_marshal_value_peek_boolean (param_values + 3),
                       g_marshal_value_peek_string (param_values + 4),
                       g_marshal_value_peek_pointer (param_values + 5),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:BOXED,STRING,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:4) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__BOXED_STRING_POINTER (GClosure     *closure,
                                                                                 GValue       *return_value,
                                                                                 guint         n_param_values,
                                                                                 const GValue *param_values,
                                                                                 gpointer      invocation_hint,
                                                                                 gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__BOXED_STRING_POINTER (GClosure     *closure,
                                                                     GValue       *return_value G_GNUC_UNUSED,
                                                                     guint         n_param_values,
                                                                     const GValue *param_values,
                                                                     gpointer      invocation_hint G_GNUC_UNUSED,
                                                                     gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__BOXED_STRING_POINTER) (gpointer     data1,
                                                                  gpointer     arg_1,
                                                                  gpointer     arg_2,
                                                                  gpointer     arg_3,
                                                                  gpointer     data2);
  register GMarshalFunc_BOOLEAN__BOXED_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 4);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__BOXED_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_boxed (param_values + 1),
                       g_marshal_value_peek_string (param_values + 2),
                       g_marshal_value_peek_pointer (param_values + 3),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,STRING,STRING,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:5) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_STRING_POINTER (GClosure     *closure,
                                                                                         GValue       *return_value,
                                                                                         guint         n_param_values,
                                                                                         const GValue *param_values,
                                                                                         gpointer      invocation_hint,
                                                                                         gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_STRING_POINTER (GClosure     *closure,
                                                                             GValue       *return_value G_GNUC_UNUSED,
                                                                             guint         n_param_values,
                                                                             const GValue *param_values,
                                                                             gpointer      invocation_hint G_GNUC_UNUSED,
                                                                             gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_STRING_STRING_POINTER) (gpointer     data1,
                                                                          gpointer     arg_1,
                                                                          gpointer     arg_2,
                                                                          gpointer     arg_3,
                                                                          gpointer     arg_4,
                                                                          gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_STRING_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 5);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_STRING_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_string (param_values + 2),
                       g_marshal_value_peek_string (param_values + 3),
                       g_marshal_value_peek_pointer (param_values + 4),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,STRING,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:6) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_POINTER (GClosure     *closure,
                                                                                  GValue       *return_value,
                                                                                  guint         n_param_values,
                                                                                  const GValue *param_values,
                                                                                  gpointer      invocation_hint,
                                                                                  gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_POINTER (GClosure     *closure,
                                                                      GValue       *return_value G_GNUC_UNUSED,
                                                                      guint         n_param_values,
                                                                      const GValue *param_values,
                                                                      gpointer      invocation_hint G_GNUC_UNUSED,
                                                                      gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_STRING_POINTER) (gpointer     data1,
                                                                   gpointer     arg_1,
                                                                   gpointer     arg_2,
                                                                   gpointer     arg_3,
                                                                   gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 4);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_string (param_values + 2),
                       g_marshal_value_peek_pointer (param_values + 3),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,BOXED,STRING,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:7) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOXED_STRING_POINTER (GClosure     *closure,
                                                                                        GValue       *return_value,
                                                                                        guint         n_param_values,
                                                                                        const GValue *param_values,
                                                                                        gpointer      invocation_hint,
                                                                                        gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOXED_STRING_POINTER (GClosure     *closure,
                                                                            GValue       *return_value G_GNUC_UNUSED,
                                                                            guint         n_param_values,
                                                                            const GValue *param_values,
                                                                            gpointer      invocation_hint G_GNUC_UNUSED,
                                                                            gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_BOXED_STRING_POINTER) (gpointer     data1,
                                                                         gpointer     arg_1,
                                                                         gpointer     arg_2,
                                                                         gpointer     arg_3,
                                                                         gpointer     arg_4,
                                                                         gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_BOXED_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 5);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_BOXED_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_boxed (param_values + 2),
                       g_marshal_value_peek_string (param_values + 3),
                       g_marshal_value_peek_pointer (param_values + 4),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:8) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_POINTER (GClosure     *closure,
                                                                           GValue       *return_value,
                                                                           guint         n_param_values,
                                                                           const GValue *param_values,
                                                                           gpointer      invocation_hint,
                                                                           gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_POINTER (GClosure     *closure,
                                                               GValue       *return_value G_GNUC_UNUSED,
                                                               guint         n_param_values,
                                                               const GValue *param_values,
                                                               gpointer      invocation_hint G_GNUC_UNUSED,
                                                               gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_POINTER) (gpointer     data1,
                                                            gpointer     arg_1,
                                                            gpointer     arg_2,
                                                            gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_pointer (param_values + 2),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:POINTER (/tmp/dbus-binding-tool-c-marshallers.YV052T:9) */
extern void dbus_glib_marshal_thunar_dbus_service_BOOLEAN__POINTER (GClosure     *closure,
                                                                    GValue       *return_value,
                                                                    guint         n_param_values,
                                                                    const GValue *param_values,
                                                                    gpointer      invocation_hint,
                                                                    gpointer      marshal_data);
void
dbus_glib_marshal_thunar_dbus_service_BOOLEAN__POINTER (GClosure     *closure,
                                                        GValue       *return_value G_GNUC_UNUSED,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint G_GNUC_UNUSED,
                                                        gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__POINTER) (gpointer     data1,
                                                     gpointer     arg_1,
                                                     gpointer     data2);
  register GMarshalFunc_BOOLEAN__POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_pointer (param_values + 1),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

G_END_DECLS

#endif /* __dbus_glib_marshal_thunar_dbus_service_MARSHAL_H__ */

#include <dbus/dbus-glib.h>
static const DBusGMethodInfo dbus_glib_thunar_dbus_service_methods[] = {
  { (GCallback) thunar_dbus_service_display_chooser_dialog, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOOLEAN_STRING_POINTER, 0 },
  { (GCallback) thunar_dbus_service_display_folder, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_POINTER, 74 },
  { (GCallback) thunar_dbus_service_display_folder_and_select, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_STRING_POINTER, 132 },
  { (GCallback) thunar_dbus_service_display_file_properties, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_POINTER, 212 },
  { (GCallback) thunar_dbus_service_launch, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_STRING_POINTER, 278 },
  { (GCallback) thunar_dbus_service_display_preferences_dialog, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_POINTER, 329 },
  { (GCallback) thunar_dbus_service_display_trash, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_POINTER, 390 },
  { (GCallback) thunar_dbus_service_empty_trash, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_POINTER, 433 },
  { (GCallback) thunar_dbus_service_move_to_trash, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__BOXED_STRING_POINTER, 474 },
  { (GCallback) thunar_dbus_service_query_trash, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__POINTER_POINTER, 531 },
  { (GCallback) thunar_dbus_service_bulk_rename, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOXED_BOOLEAN_STRING_POINTER, 573 },
  { (GCallback) thunar_dbus_service_launch_files, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__STRING_BOXED_STRING_POINTER, 667 },
  { (GCallback) thunar_dbus_service_terminate, dbus_glib_marshal_thunar_dbus_service_BOOLEAN__POINTER, 747 },
};

const DBusGObjectInfo dbus_glib_thunar_dbus_service_object_info = {
  0,
  dbus_glib_thunar_dbus_service_methods,
  13,
"org.xfce.FileManager\0DisplayChooserDialog\0S\0uri\0I\0s\0open\0I\0b\0display\0I\0s\0\0org.xfce.FileManager\0DisplayFolder\0S\0uri\0I\0s\0display\0I\0s\0\0org.xfce.FileManager\0DisplayFolderAndSelect\0S\0uri\0I\0s\0filename\0I\0s\0display\0I\0s\0\0org.xfce.FileManager\0DisplayFileProperties\0S\0uri\0I\0s\0display\0I\0s\0\0org.xfce.FileManager\0Launch\0S\0uri\0I\0s\0display\0I\0s\0\0org.xfce.FileManager\0DisplayPreferencesDialog\0S\0display\0I\0s\0\0org.xfce.Trash\0DisplayTrash\0S\0display\0I\0s\0\0org.xfce.Trash\0EmptyTrash\0S\0display\0I\0s\0\0org.xfce.Trash\0MoveToTrash\0S\0filenames\0I\0as\0display\0I\0s\0\0org.xfce.Trash\0QueryTrash\0S\0full\0O\0F\0N\0b\0\0org.xfce.Thunar\0BulkRename\0S\0working-directory\0I\0s\0filenames\0I\0as\0standalone\0I\0b\0display\0I\0s\0\0org.xfce.Thunar\0LaunchFiles\0S\0working-directory\0I\0s\0filenames\0I\0as\0display\0I\0s\0\0org.xfce.Thunar\0Terminate\0S\0\0\0",
"org.xfce.Trash\0TrashChanged\0\0",
"\0"
};

