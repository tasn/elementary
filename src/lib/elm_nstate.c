#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Elementary.h>
#include "elm_nstate_eo.h"
#include "elm_button_eo.h"
#include "elm_priv.h"
#include "elm_widget_nstate.h"
#include "elm_widget_button.h"

#define MY_CLASS ELM_NSTATE_CLASS

#define MY_CLASS_NAME "Elm_Nstate"
#define MY_CLASS_NAME_LEGACY "elm_nstate"

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {NULL, NULL}
};

static Eina_Bool _key_action_activate(Evas_Object *obj, const char *params);
static void _state_active(Evas_Object *obj, Elm_Nstate_Data *sd);

static const Elm_Action key_actions[] = {
   {"activate", _key_action_activate},
   {NULL, NULL}
};

EOLIAN static Eo_Base *
_elm_nstate_eo_base_constructor(Eo *obj, Elm_Nstate_Data *pd EINA_UNUSED)
{
   obj = eo_constructor(eo_super(obj, MY_CLASS));
   evas_obj_type_set(obj, MY_CLASS_NAME_LEGACY);
   evas_obj_smart_callbacks_descriptions_set(obj, _smart_callbacks);
   //TODO: Add ATSPI call here

   return obj;
}

static void
_next_state_set(Elm_Nstate_Data *sd)
{
   ++sd->state;
   if (sd->state == sd->nstate) sd->state = 0;
}

static void
_state_active(Evas_Object *obj, Elm_Nstate_Data *sd)
{
   char buf[64];

   sprintf(buf, "elm,state,changed,%d", sd->state);
   elm_layout_signal_emit(obj, buf, "elm");
   edje_object_message_signal_process(elm_layout_edje_get(obj));
   elm_obj_layout_sizing_eval(obj);
   eo_event_callback_call(obj, ELM_NSTATE_EVENT_STATE_CHANGED, NULL);
}

static void
_on_state_changed(void *data,
                  Evas_Object *o EINA_UNUSED,
                  const char *emission EINA_UNUSED,
                  const char *source EINA_UNUSED)
{
   efl_ui_nstate_activate(data);
}

EOLIAN static void
_elm_nstate_evas_object_smart_add(Eo *obj, Elm_Nstate_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   evas_obj_smart_add(eo_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   pd->state = 0;
   // Default: 2 states
   pd->nstate = 2;

   if (!elm_layout_theme_set(obj, "nstate", "base", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   edje_object_signal_callback_add(wd->resize_obj, "elm,action,state,changed",
                                   "*", _on_state_changed, obj);
}

EOLIAN static void
_elm_nstate_evas_object_smart_del(Eo *obj, Elm_Nstate_Data *pd EINA_UNUSED)
{
   evas_obj_smart_del(eo_super(obj, MY_CLASS));
}

EOLIAN static int
_elm_nstate_count_get(Eo *obj EINA_UNUSED, Elm_Nstate_Data *pd)
{
   return pd->nstate;
}

EOLIAN static void
_elm_nstate_count_set(Eo *obj EINA_UNUSED, Elm_Nstate_Data *pd, int nstate)
{
   if (pd->nstate == nstate) return;

   pd->nstate = nstate;
   pd->state = 0;
}

EOLIAN static int
_elm_nstate_value_get(Eo *obj EINA_UNUSED, Elm_Nstate_Data *pd)
{
   return pd->state;
}

static Eina_Bool
_is_valid_state(Elm_Nstate_Data *sd, int state)
{
   if (sd->state == state || (state < 0 || state >= sd->nstate))
     return EINA_FALSE;

   return EINA_TRUE;
}

EOLIAN static void
_elm_nstate_value_set(Eo *obj, Elm_Nstate_Data *pd, int state)
{
   if (!_is_valid_state(pd, state)) return;

   pd->state = state;
   _state_active(obj, pd);
}

EOLIAN static Eina_Bool
_elm_nstate_elm_widget_theme_apply(Eo *obj, Elm_Nstate_Data *pd)
{
   Eina_Bool int_ret;

   int_ret = elm_obj_widget_theme_apply(eo_super(obj, MY_CLASS));
   if (!int_ret) return EINA_FALSE;

   _state_active(obj, pd);

   return EINA_TRUE;
}

static Eina_Bool
_key_action_activate(Evas_Object *obj, const char *params EINA_UNUSED)
{
   efl_ui_nstate_activate(obj);
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_nstate_elm_widget_event(Eo *obj, Elm_Nstate_Data *_pd EINA_UNUSED, Evas_Object *src EINA_UNUSED, Evas_Callback_Type type, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, MY_CLASS_NAME, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

EOLIAN static void
_elm_nstate_activate(Eo *obj, Elm_Nstate_Data *_pd)
{
   _next_state_set(_pd);
   _state_active(obj, _pd);
}

EOLIAN static void
_elm_nstate_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EAPI Evas_Object *
elm_nstate_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

#include "elm_nstate.eo.c"
