#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_COMPONENT_PROTECTED
#include <Elementary.h>

static Ecore_Event_Handler *hdl;

static Eina_Bool
_mouse_move_cb(void *data EINA_UNUSED, int type EINA_UNUSED, void *info)
{
   Ecore_Event_Mouse_Move *mm = info;
   Elm_Interface_Atspi_Accessible *root, *child, *new;
   Eina_List *children, *l;

   eo_do(ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, root = elm_interface_atspi_accessible_root_get());
   if (!root)
     return EINA_TRUE;

   eo_do(root, children = elm_interface_atspi_accessible_children_get());

   EINA_LIST_FOREACH(children, l, child)
     {
        if (eo_isa(child, ELM_WIN_CLASS))
          {
             if (elm_win_window_id_get(child) == mm->window)
               break;
          }
     }
   eina_list_free(children);

   if (child)
     {
        new = child;
        do {
             child = new;
             eo_do(child, new = elm_interface_atspi_component_accessible_at_point_get(EINA_FALSE, mm->x, mm->y));
        } while (new && eo_isa(new, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN));
     }
   if (child)
     {
        char *name;
        eo_do(child, name = elm_interface_atspi_accessible_name_get());
        fprintf(stderr, "%s (%s)\n", name, eo_class_name_get(child));
        free(name);
     }

   return EINA_TRUE;
}

static void
_accessible_at_point_start(void)
{
   hdl = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _mouse_move_cb, NULL);
}

static void
_accessible_at_point_end(void)
{
   if (hdl) ecore_event_handler_del(hdl);
   hdl = NULL;
}

static void
_cleanup_cb(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   _accessible_at_point_end();
}


static void
_accessible_at_point(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   if (elm_check_state_get(obj))
     _accessible_at_point_start();
   else
     _accessible_at_point_end();
}

void
test_atspi(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *box, *btn;

   win = elm_win_util_standard_add("AT-SPI", "AT-SPI");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_event_callback_add(win, EVAS_CALLBACK_FREE, _cleanup_cb, NULL);

   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   btn = elm_check_add(win);
   elm_object_text_set(btn, "AccessibleAtPoint");
   elm_object_style_set(btn, "toggle");
   evas_object_smart_callback_add(btn, "changed", _accessible_at_point, NULL);
   evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(btn);

   elm_box_pack_end(box, btn);

   evas_object_resize(win, 300, 300);
   evas_object_show(win);
}
