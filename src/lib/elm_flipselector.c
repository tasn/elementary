#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#define ELM_WIDGET_ITEM_PROTECTED
#include <Elementary.h>
#include "elm_priv.h"
#include "elm_widget_flipselector.h"

#define MY_CLASS ELM_FLIPSELECTOR_CLASS

#define MY_CLASS_NAME "Elm_Flipselector"
#define MY_CLASS_NAME_LEGACY "elm_flipselector"

/* TODO: ideally, the default theme would use map{} blocks on the TEXT
   parts to implement their fading in/out propertly (as in the clock
   widget) */
/* TODO: if one ever wants to extend it to receiving generic widgets
   as items, be my guest. in this case, remember to implement the
   items tooltip infra. */
/* TODO: fix default theme image borders for looong strings as item
   labels. */
/* TODO: set text elipsis on labels if one enforces mininum size on
 * the overall widget less the required for displaying it. */
/* TODO: find a way to, in the default theme, to detect we are
 * bootstrapping (receiving the 1st message) and populate the downmost
 * TEXT parts with the same text as the upmost, where appropriate. */

#define FLIP_FIRST_INTERVAL (0.85)
#define FLIP_MIN_INTERVAL   (0.1)
#define MSG_FLIP_DOWN       (1)
#define MSG_FLIP_UP         (2)
#define MAX_LEN_DEFAULT     (50)

#define DATA_GET            eina_list_data_get

static const char SIG_SELECTED[] = "selected";
static const char SIG_UNDERFLOWED[] = "underflowed";
static const char SIG_OVERFLOWED[] = "overflowed";
static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_SELECTED, ""},
   {SIG_UNDERFLOWED, ""},
   {SIG_OVERFLOWED, ""},
   {SIG_WIDGET_LANG_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_ACCESS_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_LAYOUT_FOCUSED, ""}, /**< handled by elm_layout */
   {SIG_LAYOUT_UNFOCUSED, ""}, /**< handled by elm_layout */
   {NULL, NULL}
};

static Eina_Bool _key_action_flip(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"flip", _key_action_flip},
   {NULL, NULL}
};

EOLIAN static void
_elm_flipselector_elm_layout_sizing_eval(Eo *obj, Elm_Flipselector_Data *sd)
{
   char *tmp = NULL;
   Evas_Coord minw = -1, minh = -1, w, h;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   if (sd->evaluating) return;

   elm_coords_finger_size_adjust(1, &minw, 2, &minh);

   sd->evaluating = EINA_TRUE;

   if (sd->sentinel)
     {
        const char *label = elm_object_item_text_get(DATA_GET(sd->sentinel));
        const char *src = elm_layout_text_get(obj, "elm.top");

        if (src)
            tmp = strdup(src);
        elm_layout_text_set(obj, "elm.top", label);
     }

   edje_object_size_min_restricted_calc
     (wd->resize_obj, &minw, &minh, minw, minh);
   elm_coords_finger_size_adjust(1, &minw, 2, &minh);
   evas_object_size_hint_min_get(obj, &w, &h);

   if (sd->sentinel)
     {
        elm_layout_text_set(obj, "elm.top", tmp);
        free(tmp);
     }

   if (w > minw) minw = w;
   if (h > minh) minh = h;

   sd->evaluating = EINA_FALSE;

   evas_object_size_hint_min_set(obj, minw, minh);
}

static void
_update_view(Evas_Object *obj)
{
   const char *label;
   Elm_Object_Item *eo_item;

   ELM_FLIPSELECTOR_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   label = NULL;
   eo_item = DATA_GET(sd->current);
   ELM_FLIPSELECTOR_ITEM_DATA_GET(eo_item, item);
   if (item) label = item->label;

   elm_layout_text_set(obj, "elm.top", label ? label : "");
   elm_layout_text_set(obj, "elm.bottom", label ? label : "");

   edje_object_message_signal_process(wd->resize_obj);
}

EOLIAN static void
_elm_flipselector_item_elm_widget_item_part_text_set(Eo *eo_item,
                                                     Elm_Flipselector_Item_Data *item,
                                                     const char *part,
                                                     const char *label)
{
   Eina_List *l;

   if (!label) return;

   if (part && strcmp(part, "default")) return;

   ELM_FLIPSELECTOR_DATA_GET(WIDGET(item), sd);

   if (!sd->items) return;

   l = eina_list_data_find_list(sd->items, eo_item);
   if (!l) return;

   eina_stringshare_del(item->label);
   item->label = eina_stringshare_add_length(label, sd->max_len);

   if (strlen(label) > strlen(elm_object_item_text_get(DATA_GET(sd->sentinel))))
     sd->sentinel = l;

   if (sd->current == l)
     {
        _update_view(WIDGET(item));
        elm_layout_sizing_eval(WIDGET(item));
     }
}

EOLIAN static const char *
_elm_flipselector_item_elm_widget_item_part_text_get(Eo *eo_it EINA_UNUSED,
                                                     Elm_Flipselector_Item_Data *it,
                                                     const char *part)
{
   if (part && strcmp(part, "default")) return NULL;

   return it->label;
}

EOLIAN static void
_elm_flipselector_item_elm_widget_item_signal_emit(Eo *eo_it EINA_UNUSED,
                                                   Elm_Flipselector_Item_Data *it,
                                                   const char *emission,
                                                   const char *source)
{
   edje_object_signal_emit(VIEW(it), emission, source);
}

static inline void
_flipselector_walk(Elm_Flipselector_Data *sd)
{
   if (sd->walking < 0)
     {
        ERR("walking was negative. fixed!\n");
        sd->walking = 0;
     }
   sd->walking++;
}

static void
_sentinel_eval(Elm_Flipselector_Data *sd)
{
   Elm_Object_Item *it;
   Eina_List *l;

   if (!sd->items)
     {
        sd->sentinel = NULL;
        return;
     }

   sd->sentinel = sd->items;

   EINA_LIST_FOREACH(sd->items, l, it)
     {
        if (strlen(elm_object_item_text_get(it)) >
            strlen(elm_object_item_text_get(DATA_GET(sd->sentinel))))
          sd->sentinel = l;
     }
}

static inline void
_flipselector_unwalk(Elm_Flipselector_Data *sd)
{
   sd->walking--;

   if (sd->walking < 0)
     {
        ERR("walking became negative. fixed!\n");
        sd->walking = 0;
     }
   if (sd->walking) return;
}

static void
_on_item_changed(Elm_Flipselector_Data *sd)
{
   Elm_Object_Item *eo_item;

   eo_item = DATA_GET(sd->current);
   ELM_FLIPSELECTOR_ITEM_DATA_GET(eo_item, item);
   if (!item) return;
   if (sd->deleting) return;

   if (item->func)
     item->func((void *)WIDGET_ITEM_DATA_GET(eo_item), WIDGET(item), eo_item);
   eo_event_callback_call
     (sd->obj, EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, eo_item);
}

static void
_send_msg(Elm_Flipselector_Data *sd,
          int flipside,
          char *label)
{
   Edje_Message_String msg;
   ELM_WIDGET_DATA_GET_OR_RETURN(sd->obj, wd);

   msg.str = label;
   edje_object_message_send
     (wd->resize_obj, EDJE_MESSAGE_STRING, flipside, &msg);
   edje_object_message_signal_process(wd->resize_obj);

   _on_item_changed(sd);
}

EOLIAN static void
_elm_flipselector_item_eo_base_destructor(Eo *eo_item, Elm_Flipselector_Item_Data *item)
{
   Elm_Object_Item *eo_item2;
   Eina_List *l;

   ELM_FLIPSELECTOR_DATA_GET(WIDGET(item), sd);

   EINA_LIST_FOREACH(sd->items, l, eo_item2)
     {
        if (eo_item2 == eo_item)
          {
             if (sd->current == l)
               {
                  sd->current = l->prev;
                  if (!sd->current) sd->current = l->next;
                  if (sd->current)
                    {
                       eo_item2 = sd->current->data;
                       ELM_FLIPSELECTOR_ITEM_DATA_GET(eo_item2, item2);
                       _send_msg(sd, MSG_FLIP_DOWN, (char *)item2->label);
                    }
                  else _send_msg(sd, MSG_FLIP_DOWN, "");
               }
             sd->items = eina_list_remove_list(sd->items, l);
             break;
          }
     }

   if (eina_list_count(sd->items) <= 1)
      elm_layout_signal_emit
         (sd->obj, "elm,state,button,hidden", "elm");
   else
      elm_layout_signal_emit
         (sd->obj, "elm,state,button,visible", "elm");

   eina_stringshare_del(item->label);
   _sentinel_eval(sd);
   _update_view(sd->obj);

   eo_destructor(eo_super(eo_item, ELM_FLIPSELECTOR_ITEM_CLASS));
}

EOLIAN static Eo *
_elm_flipselector_item_eo_base_constructor(Eo *obj, Elm_Flipselector_Item_Data *it)
{
   obj = eo_constructor(eo_super(obj, ELM_FLIPSELECTOR_ITEM_CLASS));
   it->base = eo_data_scope_get(obj, ELM_WIDGET_ITEM_CLASS);

   return obj;
}

static Elm_Object_Item *
_item_new(Evas_Object *obj,
          const char *label,
          Evas_Smart_Cb func,
          const void *data)
{
   unsigned int len;
   Eo *eo_item;

   ELM_FLIPSELECTOR_DATA_GET(obj, sd);

   eo_add(&eo_item, ELM_FLIPSELECTOR_ITEM_CLASS, obj);
   if (!eo_item) return NULL;

   ELM_FLIPSELECTOR_ITEM_DATA_GET(eo_item, it);

   len = strlen(label);
   if (len > sd->max_len) len = sd->max_len;

   it->label = eina_stringshare_add_length(label, len);
   it->func = func;
   WIDGET_ITEM_DATA_SET(eo_item, data);

   /* TODO: no view here, but if one desires general contents in the
    * future... */
   return eo_item;
}

EOLIAN static Eina_Bool
_elm_flipselector_elm_widget_theme_apply(Eo *obj, Elm_Flipselector_Data *sd)
{
   const char *max_len;

   Eina_Bool int_ret = EINA_FALSE;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);

   int_ret = elm_obj_widget_theme_apply(eo_super(obj, MY_CLASS));
   if (!int_ret) return EINA_FALSE;

   max_len = edje_object_data_get(wd->resize_obj, "max_len");
   if (!max_len) sd->max_len = MAX_LEN_DEFAULT;
   else
     {
        sd->max_len = atoi(max_len);

        if (!sd->max_len) sd->max_len = MAX_LEN_DEFAULT;
     }

   _update_view(obj);
   elm_layout_sizing_eval(obj);

   return EINA_TRUE;
}

static void
_flip_up(Elm_Flipselector_Data *sd)
{
   Elm_Object_Item *eo_item;

   if (!sd->current) return;

   if (sd->deleting) return;
   if (sd->current == sd->items)
     {
        sd->current = eina_list_last(sd->items);
        eo_event_callback_call
          (sd->obj, ELM_FLIPSELECTOR_EVENT_UNDERFLOWED, NULL);
     }
   else
     sd->current = eina_list_prev(sd->current);

   eo_item = DATA_GET(sd->current);
   ELM_FLIPSELECTOR_ITEM_DATA_GET(eo_item, item);
   if (!item) return;

   _send_msg(sd, MSG_FLIP_UP, (char *)item->label);
}

static void
_flip_down(Elm_Flipselector_Data *sd)
{
   Elm_Object_Item *eo_item;

   if (!sd->current) return;

   if (sd->deleting) return;
   sd->current = eina_list_next(sd->current);
   if (!sd->current)
     {
        sd->current = sd->items;
        eo_event_callback_call
          (sd->obj, ELM_FLIPSELECTOR_EVENT_OVERFLOWED, NULL);
     }

   eo_item = DATA_GET(sd->current);
   ELM_FLIPSELECTOR_ITEM_DATA_GET(eo_item, item);
   if (!item) return;

   _send_msg(sd, MSG_FLIP_DOWN, (char *)item->label);
}

static Eina_Bool
_key_action_flip(Evas_Object *obj, const char *params)
{
   ELM_FLIPSELECTOR_DATA_GET(obj, sd);
   const char *dir = params;

   ELM_SAFE_FREE(sd->spin, ecore_timer_del);

   /* TODO: if direction setting via API is not coming in, replace
      these calls by flip_{next,prev} */
   _flipselector_walk(sd);

   if (!strcmp(dir, "up")) _flip_up(sd);
   else if (!strcmp(dir, "down")) _flip_down(sd);
   else return EINA_FALSE;

   _flipselector_unwalk(sd);
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_flipselector_elm_widget_event(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd EINA_UNUSED, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   (void) src;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, MY_CLASS_NAME, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

static Eina_Bool
_signal_val_up(void *data)
{
   ELM_FLIPSELECTOR_DATA_GET(data, sd);

   _flipselector_walk(sd);

   if (sd->interval > FLIP_MIN_INTERVAL) sd->interval = sd->interval / 1.05;

   ecore_timer_interval_set(sd->spin, sd->interval);

   _flip_up(sd);

   _flipselector_unwalk(sd);

   return ECORE_CALLBACK_RENEW;
}

static void
_signal_val_up_start(void *data,
                     Evas_Object *obj EINA_UNUSED,
                     const char *emission EINA_UNUSED,
                     const char *source EINA_UNUSED)
{
   ELM_FLIPSELECTOR_DATA_GET(data, sd);

   sd->interval = sd->first_interval;

   ecore_timer_del(sd->spin);
   sd->spin = ecore_timer_add(sd->interval, _signal_val_up, data);

   _signal_val_up(data);
}

static Eina_Bool
_signal_val_down(void *data)
{
   ELM_FLIPSELECTOR_DATA_GET(data, sd);

   _flipselector_walk(sd);

   if (sd->interval > FLIP_MIN_INTERVAL) sd->interval = sd->interval / 1.05;
   ecore_timer_interval_set(sd->spin, sd->interval);

   _flip_down(sd);

   _flipselector_unwalk(sd);

   return ECORE_CALLBACK_RENEW;
}

static void
_signal_val_down_start(void *data,
                       Evas_Object *obj EINA_UNUSED,
                       const char *emission EINA_UNUSED,
                       const char *source EINA_UNUSED)
{
   ELM_FLIPSELECTOR_DATA_GET(data, sd);

   sd->interval = sd->first_interval;

   ecore_timer_del(sd->spin);
   sd->spin = ecore_timer_add(sd->interval, _signal_val_down, data);

   _signal_val_down(data);
}

static void
_signal_val_change_stop(void *data,
                        Evas_Object *obj EINA_UNUSED,
                        const char *emission EINA_UNUSED,
                        const char *source EINA_UNUSED)
{
   ELM_FLIPSELECTOR_DATA_GET(data, sd);

   ELM_SAFE_FREE(sd->spin, ecore_timer_del);
}

EOLIAN static void
_elm_flipselector_evas_object_smart_add(Eo *obj, Elm_Flipselector_Data *priv)
{
   evas_obj_smart_add(eo_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   if (!elm_layout_theme_set
       (obj, "flipselector", "base", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   elm_layout_signal_callback_add
     (obj, "elm,action,up,start", "*", _signal_val_up_start, obj);
   elm_layout_signal_callback_add
     (obj, "elm,action,up,stop", "*", _signal_val_change_stop, obj);
   elm_layout_signal_callback_add
     (obj, "elm,action,down,start", "*", _signal_val_down_start, obj);
   elm_layout_signal_callback_add
     (obj, "elm,action,down,stop", "*", _signal_val_change_stop, obj);

   priv->first_interval = FLIP_FIRST_INTERVAL;

   elm_widget_can_focus_set(obj, EINA_TRUE);

   elm_obj_widget_theme_apply(obj);
}

EOLIAN static void
_elm_flipselector_evas_object_smart_del(Eo *obj, Elm_Flipselector_Data *sd)
{
   sd->deleting = EINA_TRUE;

   if (sd->walking) ERR("flipselector deleted while walking.\n");

   while (sd->items)
     elm_wdg_item_del(DATA_GET(sd->items));

   ecore_timer_del(sd->spin);

   evas_obj_smart_del(eo_super(obj, MY_CLASS));
}

EAPI Evas_Object *
elm_flipselector_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = NULL;
   eo_add(&obj, MY_CLASS, parent);
   return obj;
}

EOLIAN static Eo *
_elm_flipselector_eo_base_constructor(Eo *obj, Elm_Flipselector_Data *sd)
{
   obj = eo_constructor(eo_super(obj, MY_CLASS));
   sd->obj = obj;
   evas_obj_type_set(obj, MY_CLASS_NAME_LEGACY);
   evas_obj_smart_callbacks_descriptions_set(obj, _smart_callbacks);
   elm_interface_atspi_accessible_role_set(obj, ELM_ATSPI_ROLE_LIST);

   return obj;
}

EOLIAN static void
_elm_flipselector_flip_next(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd)
{
   ELM_SAFE_FREE(sd->spin, ecore_timer_del);

   _flipselector_walk(sd);
   _flip_down(sd);
   _flipselector_unwalk(sd);
}

EOLIAN static void
_elm_flipselector_flip_prev(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd)
{
   ELM_SAFE_FREE(sd->spin, ecore_timer_del);

   _flipselector_walk(sd);
   _flip_up(sd);
   _flipselector_unwalk(sd);
}

EOLIAN static Elm_Object_Item *
_elm_flipselector_item_append(Eo *obj, Elm_Flipselector_Data *sd, const char *label, Evas_Smart_Cb func, const void *data)
{
   Elm_Object_Item *item;

   item = _item_new(obj, label, func, data);
   if (!item) return NULL;

   sd->items = eina_list_append(sd->items, item);
   if (!sd->current)
     {
        sd->current = sd->items;
        _update_view(obj);
     }

   if (!sd->sentinel ||
       (strlen(elm_object_item_text_get(item)) >
        strlen(elm_object_item_text_get(DATA_GET(sd->sentinel)))))
     {
        sd->sentinel = eina_list_last(sd->items);
        elm_layout_sizing_eval(obj);
     }

   if (eina_list_count(sd->items) > 1)
     elm_layout_signal_emit(obj, "elm,state,button,visible", "elm");

   return item;
}

EOLIAN static Elm_Object_Item *
_elm_flipselector_item_prepend(Eo *obj, Elm_Flipselector_Data *sd, const char *label, Evas_Smart_Cb func, void *data)
{
   Elm_Object_Item *item;

   item = _item_new(obj, label, func, data);
   if (!item) return NULL;

   sd->items = eina_list_prepend(sd->items, item);
   if (!sd->current)
     {
        sd->current = sd->items;
        _update_view(obj);
     }

   if (!sd->sentinel ||
       (strlen(elm_object_item_text_get(item)) >
        strlen(elm_object_item_text_get(DATA_GET(sd->sentinel)))))
     {
        sd->sentinel = sd->items;
        elm_layout_sizing_eval(obj);
     }

   if (eina_list_count(sd->items) >= 2)
     elm_layout_signal_emit(obj, "elm,state,button,visible", "elm");

   return item;
}

EOLIAN static const Eina_List*
_elm_flipselector_items_get(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd)
{
   return sd->items;
}

EOLIAN static Elm_Object_Item*
_elm_flipselector_first_item_get(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd)
{
   return eina_list_data_get(sd->items);
}

EOLIAN static Elm_Object_Item*
_elm_flipselector_last_item_get(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd)
{
   return eina_list_last_data_get(sd->items);
}

EOLIAN static Elm_Object_Item*
_elm_flipselector_selected_item_get(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd)
{
   return DATA_GET(sd->current);
}

EOLIAN static void
_elm_flipselector_item_selected_set(Eo *eo_item,
                                    Elm_Flipselector_Item_Data *item,
                                    Eina_Bool selected)
{
   Elm_Object_Item *_eo_item, *eo_cur;
   int flipside = MSG_FLIP_UP;
   Eina_List *l;

   ELM_FLIPSELECTOR_DATA_GET(WIDGET(item), sd);

   eo_cur = DATA_GET(sd->current);
   if ((selected) && (eo_cur == eo_item)) return;

   _flipselector_walk(sd);

   if ((!selected) && (eo_cur == eo_item))
     {
        EINA_LIST_FOREACH(sd->items, l, _eo_item)
          {
             ELM_FLIPSELECTOR_ITEM_DATA_GET(_eo_item, _item);
             sd->current = l;
             _send_msg(sd, MSG_FLIP_UP, (char *)_item->label);
             break;
          }
        _flipselector_unwalk(sd);
        return;
     }

   EINA_LIST_FOREACH(sd->items, l, _eo_item)
     {
        if (_eo_item == eo_cur) flipside = MSG_FLIP_DOWN;

        if (_eo_item == eo_item)
          {
             ELM_FLIPSELECTOR_ITEM_DATA_GET(_eo_item, _item);
             sd->current = l;
             _send_msg(sd, flipside, (char *)_item->label);
             break;
          }
     }

   _flipselector_unwalk(sd);
}

EOLIAN static Eina_Bool
_elm_flipselector_item_selected_get(Eo *eo_item,
                                    Elm_Flipselector_Item_Data *item)
{
   ELM_FLIPSELECTOR_DATA_GET(WIDGET(item), sd);

   return eina_list_data_get(sd->current) == eo_item;
}

EOLIAN static Elm_Object_Item *
_elm_flipselector_item_prev_get(const Eo *eo_item,
                                Elm_Flipselector_Item_Data *item)
{
   Eina_List *l;

   ELM_FLIPSELECTOR_DATA_GET(WIDGET(item), sd);

   if ((!sd->items)) return NULL;

   l = eina_list_data_find_list(sd->items, eo_item);
   if (l && l->prev) return DATA_GET(l->prev);

   return NULL;
}

EOLIAN static Elm_Object_Item *
_elm_flipselector_item_next_get(const Eo *eo_item,
                                Elm_Flipselector_Item_Data *item)
{
   Eina_List *l;

   ELM_FLIPSELECTOR_DATA_GET(WIDGET(item), sd);

   if ((!sd->items)) return NULL;

   l = eina_list_data_find_list(sd->items, eo_item);
   if (l && l->next) return DATA_GET(l->next);

   return NULL;
}

EOLIAN static void
_elm_flipselector_first_interval_set(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd, double interval)
{
   sd->first_interval = interval;
}

EOLIAN static double
_elm_flipselector_first_interval_get(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd)
{
   return sd->first_interval;
}

EOLIAN static Eina_Bool
_elm_flipselector_elm_widget_focus_next_manager_is(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd EINA_UNUSED)
{
   return EINA_FALSE;
}

EOLIAN static Eina_Bool
_elm_flipselector_elm_widget_focus_direction_manager_is(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *sd EINA_UNUSED)
{
   return EINA_FALSE;
}

EOLIAN static void
_elm_flipselector_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EOLIAN const Elm_Atspi_Action *
_elm_flipselector_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Flipselector_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "flip,up", "flip", "up", _key_action_flip},
          { "flip,down", "flip", "down", _key_action_flip},
          { NULL, NULL, NULL, NULL}
   };
   return &atspi_actions[0];
}

#include "elm_flipselector_item.eo.c"
#include "elm_flipselector.eo.c"
