#include <Elementary.h>
#include "elm_priv.h"

/**
 * @defgroup Check Check
 *
 * The check widget allows for toggling a value between true or false (1 or 0).
 *
 * Signals that you can add callbacks for are:
 *
 * changed - This is called whenever the user changes the state of one of the
 * check object.
 *
 * Check objects are a lot like radio objects in layout and functionality
 * except they do not work as a group, but independently and only toggle the
 * value of a boolean from false to true (0 or 1). elm_check_state_set() sets
 * the boolean state (1 for true, 0 for false), and elm_check_state_get()
 * returns the current state. For convenience, like the radio objects, you
 * can set a pointer to a boolean directly with elm_check_state_pointer_set()
 * for it to modify.
 */
typedef struct _Widget_Data Widget_Data;

struct _Widget_Data
{
   Evas_Object *chk;
   Evas_Object *icon;
   Eina_Bool state;
   Eina_Bool *statep;
   const char *label;
};

static void _del_hook(Evas_Object *obj);
static void _theme_hook(Evas_Object *obj);
static void _disable_hook(Evas_Object *obj);
static void _sizing_eval(Evas_Object *obj);
static void _changed_size_hints(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _sub_del(void *data, Evas_Object *obj, void *event_info);
static void _signal_check_off(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _signal_check_on(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _signal_check_toggle(void *data, Evas_Object *obj, const char *emission, const char *source);

static void
_del_hook(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   if (wd->label) eina_stringshare_del(wd->label);
   free(wd);
}

static void
_theme_hook(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   _elm_theme_set(wd->chk, "check", "base", "default");
   if (wd->icon)
     edje_object_signal_emit(wd->chk, "elm,state,icon,visible", "elm");
   else
     edje_object_signal_emit(wd->chk, "elm,state,icon,hidden", "elm");
   if (wd->state)
     edje_object_signal_emit(wd->chk, "elm,state,check,on", "elm");
   else
     edje_object_signal_emit(wd->chk, "elm,state,check,off", "elm");
   if (wd->label)
     edje_object_signal_emit(wd->chk, "elm,state,text,visible", "elm");
   else
     edje_object_signal_emit(wd->chk, "elm,state,text,hidden", "elm");
   edje_object_part_text_set(wd->chk, "elm.text", wd->label);
   edje_object_message_signal_process(wd->chk);
   edje_object_scale_set(wd->chk, elm_widget_scale_get(obj) * _elm_config->scale);
   _sizing_eval(obj);
}

static void
_disable_hook(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   if (elm_widget_disabled_get(obj))
     edje_object_signal_emit(wd->chk, "elm,state,disabled", "elm");
   else
     edje_object_signal_emit(wd->chk, "elm,state,enabled", "elm");
}

static void
_sizing_eval(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   Evas_Coord minw = -1, minh = -1, maxw = -1, maxh = -1;

   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   edje_object_size_min_restricted_calc(wd->chk, &minw, &minh, minw, minh);
   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   evas_object_size_hint_min_set(obj, minw, minh);
   evas_object_size_hint_max_set(obj, maxw, maxh);
}

static void
_changed_size_hints(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Widget_Data *wd = elm_widget_data_get(data);
   if (obj != wd->icon) return;
   edje_object_part_swallow(wd->chk, "elm.swallow.content", obj);
   Evas_Coord mw, mh;
   evas_object_size_hint_min_get(obj, &mw, &mh);
   _sizing_eval(data);
}

static void
_sub_del(void *data, Evas_Object *obj, void *event_info)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   Evas_Object *sub = event_info;
   if (sub == wd->icon)
     {
	edje_object_signal_emit(wd->chk, "elm,state,icon,hidden", "elm");
	evas_object_event_callback_del
	  (sub, EVAS_CALLBACK_CHANGED_SIZE_HINTS, _changed_size_hints);
	wd->icon = NULL;
	_sizing_eval(obj);
     }
}

static void
_signal_check_off(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Widget_Data *wd = elm_widget_data_get(data);
   wd->state = EINA_FALSE;
   if (wd->statep) *wd->statep = wd->state;
   edje_object_signal_emit(wd->chk, "elm,state,check,off", "elm");
   evas_object_smart_callback_call(data, "changed", NULL);
}

static void
_signal_check_on(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Widget_Data *wd = elm_widget_data_get(data);
   wd->state = EINA_TRUE;
   if (wd->statep) *wd->statep = wd->state;
   edje_object_signal_emit(wd->chk, "elm,state,check,on", "elm");
   evas_object_smart_callback_call(data, "changed", NULL);
}

static void
_signal_check_toggle(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Widget_Data *wd = elm_widget_data_get(data);
   wd->state = !wd->state;
   if (wd->statep) *wd->statep = wd->state;
   if (wd->state)
     edje_object_signal_emit(wd->chk, "elm,state,check,on", "elm");
   else
     edje_object_signal_emit(wd->chk, "elm,state,check,off", "elm");
   evas_object_smart_callback_call(data, "changed", NULL);
}

/**
 * Add a new Check object
 *
 * @param parent The parent object
 * @return The new object or NULL if it cannot be created
 *
 * @ingroup Check
 */
EAPI Evas_Object *
elm_check_add(Evas_Object *parent)
{
   Evas_Object *obj;
   Evas *e;
   Widget_Data *wd;

   wd = ELM_NEW(Widget_Data);
   e = evas_object_evas_get(parent);
   obj = elm_widget_add(e);
   elm_widget_type_set(obj, "check");
   elm_widget_sub_object_add(parent, obj);
   elm_widget_data_set(obj, wd);
   elm_widget_del_hook_set(obj, _del_hook);
   elm_widget_theme_hook_set(obj, _theme_hook);
   elm_widget_disable_hook_set(obj, _disable_hook);

   wd->chk = edje_object_add(e);
   _elm_theme_set(wd->chk, "check", "base", "default");
   edje_object_signal_callback_add(wd->chk, "elm,action,check,on", "", _signal_check_on, obj);
   edje_object_signal_callback_add(wd->chk, "elm,action,check,off", "", _signal_check_off, obj);
   edje_object_signal_callback_add(wd->chk, "elm,action,check,toggle", "", _signal_check_toggle, obj);
   elm_widget_resize_object_set(obj, wd->chk);

   evas_object_smart_callback_add(obj, "sub-object-del", _sub_del, obj);

   _sizing_eval(obj);
   return obj;
}

/**
 * Set the text label of the check object
 *
 * @param obj The check object
 * @param label The text label string in UTF-8
 *
 * @ingroup Check
 */
EAPI void
elm_check_label_set(Evas_Object *obj, const char *label)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   Evas_Coord mw, mh;

   if (wd->label) eina_stringshare_del(wd->label);
   if (label)
     {
	wd->label = eina_stringshare_add(label);
	edje_object_signal_emit(wd->chk, "elm,state,text,visible", "elm");
	edje_object_message_signal_process(wd->chk);
     }
   else
     {
	wd->label = NULL;
	edje_object_signal_emit(wd->chk, "elm,state,text,hidden", "elm");
	edje_object_message_signal_process(wd->chk);
     }
   edje_object_part_text_set(wd->chk, "elm.text", label);
   _sizing_eval(obj);
}

/**
 * Get the text label of the check object
 *
 * @param obj The check object
 * @return The text label string in UTF-8
 *
 * @ingroup Check
 */
EAPI const char*
elm_check_label_get(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   if (!wd) return NULL;

   return wd->label;
}

/**
 * Set the icon object of the check object
 *
 * Once the icon object is set, it will become a child of the check object and
 * be deleted when the check object is deleted. If another icon object is set
 * then the previous one becomes orophaned and will no longer be deleted along
 * with the check.
 *
 * @param obj The check object
 * @param icon The icon object
 *
 * @ingroup Check
 */
EAPI void
elm_check_icon_set(Evas_Object *obj, Evas_Object *icon)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   if ((wd->icon != icon) && (wd->icon))
     elm_widget_sub_object_del(obj, wd->icon);
   wd->icon = icon;
   if (icon)
     {
	elm_widget_sub_object_add(obj, icon);
	edje_object_part_swallow(wd->chk, "elm.swallow.content", icon);
	edje_object_signal_emit(wd->chk, "elm,state,icon,visible", "elm");
	evas_object_event_callback_add(icon, EVAS_CALLBACK_CHANGED_SIZE_HINTS,
				       _changed_size_hints, obj);
	_sizing_eval(obj);
     }
}

/**
 * Get the icon object of the check object
 *
 * @param obj The check object
 * @return The icon object
 *
 * @ingroup Check
 */
EAPI Evas_Object *
elm_check_icon_get(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   if (!wd) return NULL;
   return wd->icon;
}

/**
 * Set the on/off state of the check object
 *
 * This sets the state of the check and will also set the value if pointed to
 * to the state supplied, but will not call any callbacks.
 *
 * @param obj The check object
 * @param state The state to use (1 == on, 0 == off)
 *
 * @ingroup Check
 */
EAPI void
elm_check_state_set(Evas_Object *obj, Eina_Bool state)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   if (state != wd->state)
     {
	wd->state = state;
	if (wd->statep) *wd->statep = wd->state;
	if (wd->state)
	  edje_object_signal_emit(wd->chk, "elm,state,check,on", "elm");
	else
	  edje_object_signal_emit(wd->chk, "elm,state,check,off", "elm");
     }
}

/**
 * Get the state of the check object
 *
 * @param obj The check object
 * @return The boolean state
 *
 * @ingroup Check
 */
EAPI Eina_Bool
elm_check_state_get(const Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   return wd->state;
}

/**
 * Set a convenience pointer to a boolean to change
 *
 * This sets a pointer to a boolean, that, in addition to the check objects
 * state will also be modified directly. To stop setting the object pointed
 * to simply use NULL as the statep parameter. If statep is not NULL, then
 * when this is called, the check objects state will also be modified to
 * reflect the value of the boolean statep points to, just like calling
 * elm_check_state_set().
 *
 * @param obj The check object
 * @param statep Pointer to the boolean to modify
 *
 * @ingroup Check
 */
EAPI void
elm_check_state_pointer_set(Evas_Object *obj, Eina_Bool *statep)
{
   Widget_Data *wd = elm_widget_data_get(obj);

   if (statep)
     {
	wd->statep = statep;
	if (*wd->statep != wd->state)
	  {
	     wd->state = *wd->statep;
	     if (wd->state)
	       edje_object_signal_emit(wd->chk, "elm,state,check,on", "elm");
	     else
	       edje_object_signal_emit(wd->chk, "elm,state,check,off", "elm");
	  }
     }
   else
     {
	wd->statep = NULL;
     }
}
