#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#include <Eo.h>
#include <Emodel.h>
#include <Elementary.h>

#include "elm_view_form.h"
#include "elm_view_list.h"

#include "elm_priv.h"
#include <assert.h>

#define MY_CLASS ELM_VIEW_FORM_CLASS
#define MY_CLASS_NAME "View_Form"

typedef struct _Elm_View_Form_Data Elm_View_Form_Data;
typedef struct _Elm_View_Form_Widget Elm_View_Form_Widget;

/**
 * @brief Local-use callbacks
 */
typedef void (*Elm_View_Form_Event_Cb)(Elm_View_Form_Widget *, Elm_View_Form_Data *, Evas_Object *);
typedef void (*Elm_View_Form_Widget_Object_Set_Cb)(Eo *, Evas_Object *, Eina_Value *, const char *);

struct _Elm_View_Form_Widget
{
   Eina_Stringshare *widget_propname;
   Evas_Object *widget_obj;
   Elm_View_Form_Event_Cb widget_obj_value_update_cb;
   Elm_View_Form_Widget_Object_Set_Cb widget_obj_set_cb;
};

struct _Elm_View_Form_Data
{
   Eo *model_obj;
   Eina_Value *properties;
   Eina_List *l;
};

static Eina_Bool
_emodel_properties_change_cb(void *data, Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   const Emodel_Property_Event *evt = event_info;
   Elm_View_Form_Data *priv = data;
   Emodel_Property_Pair *pair = NULL;
   Eina_List *l, *fe = NULL;
   Elm_View_Form_Widget *w = NULL;

   EINA_SAFETY_ON_NULL_RETURN_VAL(priv, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(evt, EINA_TRUE);

   //update all widgets with this property
   EINA_LIST_FOREACH(priv->l, l, w)
     {
        EINA_LIST_FOREACH(evt->changed_properties, fe, pair)
          {
              if(w->widget_propname == pair->property)
                {
                    w->widget_obj_set_cb(priv->model_obj, w->widget_obj, &pair->value, pair->property);
                    break;
                }
          }
     }

   return EINA_TRUE;
}

static void
_update_model_properties(Elm_View_Form_Data *priv)
{
   Eina_Value value;
   Eina_List *l;
   Elm_View_Form_Widget *w;
   //update all widgets property
   EINA_LIST_FOREACH(priv->l, l, w)
     {
        eo_do(priv->model_obj, emodel_property_get(w->widget_propname, &value));
        w->widget_obj_set_cb(priv->model_obj, w->widget_obj, &value, w->widget_propname);
        eina_value_flush(&value);
     }
}

/**
 * @brief Set widget.
 * Works, so far, for widget(s): Entry, Label
 */
static void
_elm_evas_object_text_set_cb(Eo *obj EINA_UNUSED, Evas_Object *widget, Eina_Value *value, const char *propname EINA_UNUSED)
{
   char *text = eina_value_to_string(value);
   const char *c_text = elm_object_text_get(widget);
   EINA_SAFETY_ON_NULL_RETURN(text);

   if (strcmp(text, c_text) != 0)
     {
         elm_object_text_set(widget, text);
     }
   free(text);
}

static void
_elm_evas_object_thumb_set_cb(Eo *obj EINA_UNUSED, Evas_Object *thumb, Eina_Value *value, const char *propname EINA_UNUSED)
{
   char *filename = eina_value_to_string(value);
   EINA_SAFETY_ON_NULL_RETURN(filename);
   EINA_SAFETY_ON_TRUE_RETURN(strlen(filename) >= PATH_MAX);

   elm_thumb_file_set(thumb, filename, NULL);
   elm_thumb_reload(thumb);
   free(filename);
}

/**
 * @brief Evas object callback implementation.
 *    Updates Widget's value if not the same object
 *    and the widget itself.
 */
static void
_elm_evas_object_text_changed_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Eina_Value value;
   Eina_List *l;
   Elm_View_Form_Data *priv = (Elm_View_Form_Data *)data;
   Elm_View_Form_Widget *w = NULL;

   EINA_LIST_FOREACH(priv->l, l, w)
     {
        if (w->widget_obj == obj)
           break;
     }

   EINA_SAFETY_ON_NULL_RETURN(w);
   eina_value_setup(&value, EINA_VALUE_TYPE_STRING);
   eina_value_set(&value, elm_object_text_get(obj));
   eo_do(priv->model_obj, emodel_property_set(w->widget_propname, value));
   eina_value_flush(&value);
}
/**
 * @brief Add new widget object.
 *    Adds new widget object on the list
 *    and perform initial setup.
 */
static Eina_Bool
_elm_view_widget_add(Elm_View_Form_Data *priv, const char *propname, Evas_Object *widget_obj)
{
   Eina_Value value;
   Elm_View_Form_Widget *w = calloc(1, sizeof(Elm_View_Form_Widget));
   EINA_SAFETY_ON_NULL_RETURN_VAL(w, EINA_FALSE);

   w->widget_propname = eina_stringshare_add(propname);
   w->widget_obj = widget_obj;
   priv->l = eina_list_append(priv->l, w);

   if(eo_isa(widget_obj, ELM_ENTRY_CLASS))
     {
        w->widget_obj_set_cb = _elm_evas_object_text_set_cb;
        evas_object_event_callback_add(w->widget_obj, EVAS_CALLBACK_KEY_DOWN, _elm_evas_object_text_changed_cb, priv);
     }
   else if(eo_isa(widget_obj, ELM_LABEL_CLASS))
     {
        w->widget_obj_set_cb = _elm_evas_object_text_set_cb;
     }
   else if(eo_isa(widget_obj, ELM_THUMB_CLASS))
     {
        w->widget_obj_set_cb = _elm_evas_object_thumb_set_cb;
     }
   else
     {
        // Widget yet not supported
        EINA_SAFETY_ON_NULL_RETURN_VAL(NULL, EINA_FALSE);
     }

   if (eo_do(priv->model_obj, emodel_property_get(propname, &value)) == EINA_TRUE)
     {
         w->widget_obj_set_cb(priv->model_obj, w->widget_obj, &value, w->widget_propname);
         eina_value_flush(&value);
     }

   return EINA_TRUE;
}
/**
 * Helper functions - End
 */


/**
 * @brief constructor
 */
static void
_elm_view_form_constructor(Eo *obj, Elm_View_Form_Data *_pd, Eo *model)
{
   Elm_View_Form_Data *priv = (Elm_View_Form_Data *)_pd;
   eo_do_super(obj, MY_CLASS, eo_constructor());
   priv->model_obj = model;
   if (priv->model_obj != NULL)
     {
        eo_ref(priv->model_obj);
        eo_do(priv->model_obj, eo_event_callback_add(EMODEL_EVENT_PROPERTIES_CHANGED, _emodel_properties_change_cb, priv));
     }
}

static void
_elm_view_form_eo_base_constructor(Eo *obj, Elm_View_Form_Data *_pd EINA_UNUSED)
{
   eo_error_set(obj);
   fprintf(stderr, "only custom constructor can be used with '%s' class", MY_CLASS_NAME);
}


/**
 * @brief destructor
 */
static void
_elm_view_form_eo_base_destructor(Eo *obj, Elm_View_Form_Data *priv)
{
   Elm_View_Form_Widget *w = NULL;
   EINA_LIST_FREE(priv->l, w)
     {
        eina_stringshare_del(w->widget_propname);
        free(w);
     }

   eo_do_super(obj, MY_CLASS, eo_destructor());
}


static void
_elm_view_form_model_set(Eo *obj EINA_UNUSED, Elm_View_Form_Data *priv, Eo *model)
{
   if (priv->model_obj != NULL)
     {
        eo_do(priv->model_obj, eo_event_callback_del(EMODEL_EVENT_PROPERTIES_CHANGED, _emodel_properties_change_cb, priv));
        eo_unref(priv->model_obj);
     }

   priv->model_obj = model;

   if (priv->model_obj != NULL)
     {
        eo_ref(priv->model_obj);
        eo_do(priv->model_obj, eo_event_callback_add(EMODEL_EVENT_PROPERTIES_CHANGED, _emodel_properties_change_cb, priv));
        _update_model_properties(priv);
     }
}

static void
_elm_view_form_widget_add(Eo *obj EINA_UNUSED, Elm_View_Form_Data *priv, const char *propname, Evas_Object *evas)
{
   Eina_Bool status;

   EINA_SAFETY_ON_NULL_RETURN(evas);
   EINA_SAFETY_ON_NULL_RETURN(propname);

   status = _elm_view_widget_add(priv, propname, evas);
   EINA_SAFETY_ON_FALSE_RETURN(status);
}

#include "elm_view_form.eo.c"
