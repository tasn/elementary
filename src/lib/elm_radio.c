#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "elm_widget_radio.h"
#include "elm_widget_layout.h"

#define MY_CLASS ELM_RADIO_CLASS

#define MY_CLASS_NAME "Elm_Radio"
#define MY_CLASS_NAME_LEGACY "elm_radio"

static const Elm_Layout_Part_Alias_Description _text_aliases[] =
{
   {"default", "elm.text"},
   {NULL, NULL}
};

static const char SIG_CHANGED[] = "changed";
static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_CHANGED, ""},
   {SIG_WIDGET_LANG_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_ACCESS_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_LAYOUT_FOCUSED, ""}, /**< handled by elm_layout */
   {SIG_LAYOUT_UNFOCUSED, ""}, /**< handled by elm_layout */
   {NULL, NULL}
};

static Eina_Bool _key_action_activate(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"activate", _key_action_activate},
   {NULL, NULL}
};

static void
_state_set(Evas_Object *obj, Eina_Bool state, Eina_Bool activate)
{
   ELM_RADIO_DATA_GET(obj, sd);

   if (state != sd->state)
     {
        sd->state = state;
        if (sd->state)
          {
             // FIXME: to do animation during state change , we need different signal
             // so that we can distinguish between state change by user or state change
             // by calling state_change() api. Keep both the signal for backward compatibility
             // and only emit "elm,state,radio,on" when activate is false  when we can break ABI.
             if (activate) elm_layout_signal_emit(obj, "elm,activate,radio,on", "elm");
             elm_layout_signal_emit(obj, "elm,state,radio,on", "elm");
          }
        else
          {
             // FIXME: to do animation during state change , we need different signal
             // so that we can distinguish between state change by user or state change
             // by calling state_change() api. Keep both the signal for backward compatibility
             // and only emit "elm,state,radio,off"when activate is false when we can break ABI.
             if (activate) elm_layout_signal_emit(obj, "elm,activate,radio,off", "elm");
             elm_layout_signal_emit(obj, "elm,state,radio,off", "elm");
          }
        if (_elm_config->atspi_mode)
          {
             if (sd->state)
               {
                  elm_interface_atspi_accessible_state_changed_signal_emit(obj, ELM_ATSPI_STATE_CHECKED, EINA_TRUE);
               }
             else
               elm_interface_atspi_accessible_state_changed_signal_emit(obj, ELM_ATSPI_STATE_CHECKED, EINA_FALSE);
          }
     }
}

static void
_state_set_all(Elm_Radio_Data *sd, Eina_Bool activate)
{
   const Eina_List *l;
   Eina_Bool disabled = EINA_FALSE;
   Evas_Object *child, *selected = NULL;

   EINA_LIST_FOREACH(sd->group->radios, l, child)
     {
        ELM_RADIO_DATA_GET(child, sdc);

        if (sdc->state) selected = child;
        if (sdc->value == sd->group->value)
          {
             _state_set(child, EINA_TRUE, activate);
             if (!sdc->state) disabled = EINA_TRUE;
          }
        else _state_set(child, EINA_FALSE, activate);
     }

   if ((disabled) && (selected)) _state_set(selected, 1, activate);
}

static void
_activate(Evas_Object *obj)
{
   ELM_RADIO_DATA_GET(obj, sd);

   if (sd->group->value == sd->value) return;

   if ((!_elm_config->access_mode) ||
       (_elm_access_2nd_click_timeout(obj)))
     {
        sd->group->value = sd->value;
        if (sd->group->valuep) *(sd->group->valuep) = sd->group->value;

        _state_set_all(sd, EINA_TRUE);

        if (_elm_config->access_mode)
          _elm_access_say(E_("State: On"));
        eo_do(obj, eo_event_callback_call
          (ELM_RADIO_EVENT_CHANGED, NULL));

     }
}

static Eina_Bool
_key_action_activate(Evas_Object *obj, const char *params EINA_UNUSED)
{
   _activate(obj);
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_radio_elm_widget_event(Eo *obj, Elm_Radio_Data *_pd EINA_UNUSED, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
   (void) src;
   Evas_Event_Key_Down *ev = event_info;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, MY_CLASS_NAME, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

/* FIXME: replicated from elm_layout just because radio's icon spot
 * is elm.swallow.content, not elm.swallow.icon. Fix that whenever we
 * can changed the theme API */
static void
_icon_signal_emit(Evas_Object *obj)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   char buf[64];

   snprintf(buf, sizeof(buf), "elm,state,icon,%s",
            elm_layout_content_get(obj, "icon") ? "visible" : "hidden");

   elm_layout_signal_emit(obj, buf, "elm");
   edje_object_message_signal_process(wd->resize_obj);
}

EOLIAN static Eina_Bool
_elm_radio_elm_widget_theme_apply(Eo *obj, Elm_Radio_Data *sd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);
   Eina_Bool int_ret = EINA_FALSE;
   eo_do_super(obj, ELM_CHECK_CLASS, int_ret = elm_obj_widget_theme_apply());
   if (!int_ret) return EINA_FALSE;

   if (sd->state) elm_layout_signal_emit(obj, "elm,state,radio,on", "elm");
   else elm_layout_signal_emit(obj, "elm,state,radio,off", "elm");

   if (sd->state) _state_set(obj, EINA_FALSE, EINA_FALSE);

   edje_object_message_signal_process(wd->resize_obj);

   /* FIXME: replicated from elm_layout just because radio's icon
    * spot is elm.swallow.content, not elm.swallow.icon. Fix that
    * whenever we can changed the theme API */
   _icon_signal_emit(obj);

   eo_do(obj, elm_obj_layout_sizing_eval());

   return EINA_TRUE;
}

static void
_radio_on_cb(void *data,
             Evas_Object *obj EINA_UNUSED,
             const char *emission EINA_UNUSED,
             const char *source EINA_UNUSED)
{
   _activate(data);
}

static char *
_access_info_cb(void *data EINA_UNUSED, Evas_Object *obj)
{
   const char *txt = elm_widget_access_info_get(obj);

   if (!txt) txt = elm_layout_text_get(obj, NULL);
   if (txt) return strdup(txt);

   return NULL;
}

static char *
_access_state_cb(void *data EINA_UNUSED, Evas_Object *obj)
{
   ELM_RADIO_DATA_GET(obj, sd);

   if (elm_widget_disabled_get(obj)) return strdup(E_("State: Disabled"));
   if (sd->state) return strdup(E_("State: On"));

   return strdup(E_("State: Off"));
}

EOLIAN static void
_elm_radio_evas_object_smart_add(Eo *obj, Elm_Radio_Data *priv)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   eo_do_super(obj, ELM_CHECK_CLASS, evas_obj_smart_add());
   elm_widget_sub_object_parent_add(obj);

   if (!elm_layout_theme_set(obj, "radio", "base", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   elm_layout_signal_callback_add
     (obj, "elm,action,radio,toggle", "*", _radio_on_cb, obj);

   priv->group = calloc(1, sizeof(Group));
   priv->group->radios = eina_list_append(priv->group->radios, obj);

   elm_widget_can_focus_set(obj, EINA_TRUE);

   elm_layout_sizing_eval(obj);

   _elm_access_object_register(obj, wd->resize_obj);
   _elm_access_text_set
     (_elm_access_info_get(obj), ELM_ACCESS_TYPE, E_("Radio"));
   _elm_access_callback_set
     (_elm_access_info_get(obj), ELM_ACCESS_INFO, _access_info_cb, obj);
   _elm_access_callback_set
     (_elm_access_info_get(obj), ELM_ACCESS_STATE, _access_state_cb, obj);
}

EOLIAN static void
_elm_radio_evas_object_smart_del(Eo *obj, Elm_Radio_Data *sd)
{
   sd->group->radios = eina_list_remove(sd->group->radios, obj);
   if (!sd->group->radios) free(sd->group);

   eo_do_super(obj, ELM_CHECK_CLASS, evas_obj_smart_del());
}

EOLIAN static const Elm_Layout_Part_Alias_Description*
_elm_radio_elm_layout_text_aliases_get(Eo *obj EINA_UNUSED, Elm_Radio_Data *_pd EINA_UNUSED)
{
   return _text_aliases;
}

EAPI Evas_Object *
elm_radio_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

EOLIAN static Eo *
_elm_radio_eo_base_constructor(Eo *obj, Elm_Radio_Data *_pd EINA_UNUSED)
{
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());
   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_RADIO_BUTTON));

   return obj;
}

EOLIAN static void
_elm_radio_group_add(Eo *obj, Elm_Radio_Data *sd, Evas_Object *group)
{
   ELM_RADIO_DATA_GET(group, sdg);

   if (!sdg)
     {
        if (eina_list_count(sd->group->radios) == 1) return;
        sd->group->radios = eina_list_remove(sd->group->radios, obj);
        sd->group = calloc(1, sizeof(Group));
        sd->group->radios = eina_list_append(sd->group->radios, obj);
     }
   else if (sd->group == sdg->group)
     return;
   else
     {
        sd->group->radios = eina_list_remove(sd->group->radios, obj);
        if (!sd->group->radios) free(sd->group);
        sd->group = sdg->group;
        sd->group->radios = eina_list_append(sd->group->radios, obj);
     }
   if (sd->value == sd->group->value) _state_set(obj, EINA_TRUE, EINA_FALSE);
   else _state_set(obj, EINA_FALSE, EINA_FALSE);
}

EOLIAN static void
_elm_radio_state_value_set(Eo *obj, Elm_Radio_Data *sd, int value)
{
   sd->value = value;
   if (sd->value == sd->group->value) _state_set(obj, EINA_TRUE, EINA_FALSE);
   else _state_set(obj, EINA_FALSE, EINA_FALSE);
}

EOLIAN static int
_elm_radio_state_value_get(Eo *obj EINA_UNUSED, Elm_Radio_Data *sd)
{
   return sd->value;
}

EOLIAN static void
_elm_radio_value_set(Eo *obj EINA_UNUSED, Elm_Radio_Data *sd, int value)
{
   if (value == sd->group->value) return;
   sd->group->value = value;
   if (sd->group->valuep) *(sd->group->valuep) = sd->group->value;
   _state_set_all(sd, EINA_FALSE);
}

EOLIAN static int
_elm_radio_value_get(Eo *obj EINA_UNUSED, Elm_Radio_Data *sd)
{
   return sd->group->value;
}

EOLIAN static void
_elm_radio_value_pointer_set(Eo *obj EINA_UNUSED, Elm_Radio_Data *sd, int *valuep)
{
   if (valuep)
     {
        sd->group->valuep = valuep;
        if (*(sd->group->valuep) != sd->group->value)
          {
             sd->group->value = *(sd->group->valuep);
             _state_set_all(sd, EINA_FALSE);
          }
     }
   else sd->group->valuep = NULL;
}

EOLIAN static Evas_Object*
_elm_radio_selected_object_get(Eo *obj EINA_UNUSED, Elm_Radio_Data *sd)
{
   Eina_List *l;
   Evas_Object *child;

   EINA_LIST_FOREACH(sd->group->radios, l, child)
     {
        ELM_RADIO_DATA_GET(child, sdc);

        if (sdc->value == sd->group->value) return child;
     }

   return NULL;
}

EOLIAN static Eina_Bool
_elm_radio_elm_widget_activate(Eo *obj, Elm_Radio_Data *_pd EINA_UNUSED, Elm_Activate act)
{
   if (elm_widget_disabled_get(obj)) return EINA_FALSE;
   if (act != ELM_ACTIVATE_DEFAULT) return EINA_FALSE;

   _activate(obj);

   return EINA_TRUE;
}

EOLIAN static void
_elm_radio_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EOLIAN const Elm_Atspi_Action *
_elm_radio_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Radio_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "activate", "activate", NULL, _key_action_activate},
          { NULL, NULL, NULL, NULL }
   };
   return &atspi_actions[0];
}

EOLIAN Elm_Atspi_State_Set
_elm_radio_elm_interface_atspi_accessible_state_set_get(Eo *obj, Elm_Radio_Data *pd EINA_UNUSED)
{
   Elm_Atspi_State_Set ret;

   eo_do_super(obj, ELM_RADIO_CLASS, ret = elm_interface_atspi_accessible_state_set_get());
   if (obj == elm_radio_selected_object_get(obj))
     STATE_TYPE_SET(ret, ELM_ATSPI_STATE_CHECKED);

   return ret;
}

#include "elm_radio.eo.c"
