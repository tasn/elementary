#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include "Elementary.h"
#include "private.h"

int _elm_ext_log_dom = -1;

static int init_count = 0;

void
external_elm_init(void)
{
   int argc = 0;
   char **argv = NULL;

   init_count++;
   DBG("elm_real_init\n");
   if (init_count > 1) return;
   ecore_app_args_get(&argc, &argv);
   elm_init(argc, argv);
}

static void
external_elm_shutdown(void)
{
   init_count--;
   DBG("elm_real_shutdown\n");
   if (init_count > 0) return;
   elm_shutdown();
}

void
external_signal(void *data EINA_UNUSED, Evas_Object *obj, const char *sig,
                const char *source)
{
   char *_signal = strdup(sig);
   char *p = _signal;
   Evas_Object *content;
   Edje_External_Type *type;

   if (!p) goto on_error;

   while ((*p!='\0') && (*p!=']'))
     p++;


   if ((*p=='\0') || (*(p+1)!=':'))
     {
        ERR("Invalid External Signal received: '%s' '%s'", sig, source);
        goto on_error;
     }

   *p = '\0';
   p+=2; //jump ']' and ':'

   type = evas_object_data_get(obj, "Edje_External_Type");
   if (!type)
     {
        ERR("no external type for object %p", obj);
        goto on_error;
     }
   if (!type->content_get)
     {
        ERR("external type '%s' from module '%s' does not provide content_get()",
            type->module_name, type->module);
        goto on_error;
     }

   content = type->content_get(type->data, obj, _signal);
   if (content)
     edje_object_signal_emit(content, sig + (p - _signal), source);

on_error:
   free(_signal);
   return;
}

const char *
external_translate(void *data EINA_UNUSED, const char *orig)
{
   // in future, mark all params as translatable and use dgettext()
   // with "elementary" text domain here.
   return orig;
}

typedef struct {
   const char *emission;
   const char *source;
   Evas_Object *edje;
} Elm_External_Signals_Proxy_Context;

static void
_external_signal_proxy_free_cb(void *data, Evas *e EINA_UNUSED,
                               Evas_Object *obj EINA_UNUSED,
                               void *event_info EINA_UNUSED)
{
   Elm_External_Signals_Proxy_Context *ctxt = data;
   external_elm_shutdown();
   free(ctxt);
}

static void
_external_signal_proxy_cb(void *data, Evas_Object *obj EINA_UNUSED,
                          void *event_info EINA_UNUSED)
{
   Elm_External_Signals_Proxy_Context *ctxt = data;
   // TODO: Is it worth to check Evas_Smart_Cb_Description and do something
   // TODO: with event_info given its description?
   edje_object_signal_emit(ctxt->edje, ctxt->emission, ctxt->source);
}

Eina_Bool
external_common_param_get(void *data EINA_UNUSED, const Evas_Object *obj,
                          Edje_External_Param *param)
{
   if (!strcmp(param->name, "style"))
     {
        if (param->type == EDJE_EXTERNAL_PARAM_TYPE_STRING)
          {
             param->s = elm_object_style_get(obj);
             return EINA_TRUE;
          }
     }
   else if (!strcmp(param->name, "disabled"))
     {
        if (param->type == EDJE_EXTERNAL_PARAM_TYPE_BOOL)
          {
             param->i = elm_object_disabled_get(obj);
             return EINA_TRUE;
          }
     }
   return EINA_FALSE;
}

Eina_Bool
external_common_param_set(void *data EINA_UNUSED, Evas_Object *obj,
                          const Edje_External_Param *param)
{
   if (!strcmp(param->name, "style"))
     {
         if (param->type == EDJE_EXTERNAL_PARAM_TYPE_STRING)
           {
              elm_object_style_set(obj, param->s);
              return EINA_TRUE;
           }
     }
   else if (!strcmp(param->name, "disabled"))
     {
         if (param->type == EDJE_EXTERNAL_PARAM_TYPE_BOOL)
           {
              elm_object_disabled_set(obj, param->i);
              return EINA_TRUE;
           }
     }
   return EINA_FALSE;
}

void
external_signals_proxy(Evas_Object *obj, Evas_Object *edje, const char *part_name)
{
   const Evas_Smart_Cb_Description **cls_descs, **inst_descs;
   unsigned int cls_count, inst_count, total;
   Elm_External_Signals_Proxy_Context *ctxt;

   evas_object_smart_callbacks_descriptions_get
     (obj, &cls_descs, &cls_count, &inst_descs, &inst_count);

   total = cls_count + inst_count;
   if (!total) return;
   ctxt = malloc(sizeof(Elm_External_Signals_Proxy_Context) * total);
   if (!ctxt) return;
   evas_object_event_callback_add
     (obj, EVAS_CALLBACK_FREE, _external_signal_proxy_free_cb, ctxt);

   for (; cls_count > 0; cls_count--,  cls_descs++, ctxt++)
     {
        const Evas_Smart_Cb_Description *d = *cls_descs;
        ctxt->emission = d->name;
        ctxt->source = part_name;
        ctxt->edje = edje;
        evas_object_smart_callback_add
           (obj, d->name, _external_signal_proxy_cb, ctxt);
     }

   for (; inst_count > 0; inst_count--,  inst_descs++, ctxt++)
     {
        const Evas_Smart_Cb_Description *d = *inst_descs;
        ctxt->emission = d->name;
        ctxt->source = part_name;
        ctxt->edje = edje;
        evas_object_smart_callback_add
           (obj, d->name, _external_signal_proxy_cb, ctxt);
     }
}

void
external_common_params_parse(void *mem, void *data EINA_UNUSED,
                             Evas_Object *obj EINA_UNUSED,
                             const Eina_List *params)
{
   Elm_Params *p;
   const Eina_List *l;
   Edje_External_Param *param;

   p = mem;
   EINA_LIST_FOREACH(params, l, param)
     {
        if (!strcmp(param->name, "style"))
          p->style = eina_stringshare_add(param->s);
        else if (!strcmp(param->name, "disabled"))
          {
             p->disabled = param->i;
             p->disabled_exists = EINA_TRUE;
          }
     }
}

void
external_common_state_set(void *data EINA_UNUSED, Evas_Object *obj,
                          const void *from_params, const void *to_params,
                          float pos EINA_UNUSED)
{
   const Elm_Params *p;
   if (to_params) p = to_params;
   else if (from_params) p = from_params;
   else return;

   if (p->style)
      elm_object_style_set(obj, p->style);
   if (p->disabled_exists)
     elm_object_disabled_set(obj, p->disabled);
}

Evas_Object *
external_common_param_icon_get(Evas_Object *obj, const Edje_External_Param *p)
{
   Evas_Object *edje, *parent_widget, *icon;
   const char *file;

   if ((!p) || (!p->s) || (p->type != EDJE_EXTERNAL_PARAM_TYPE_STRING))
       return NULL;

   edje = evas_object_smart_parent_get(obj);
   edje_object_file_get(edje, &file, NULL);

   parent_widget = elm_widget_parent_widget_get(obj);
   if (!parent_widget)
     parent_widget = edje;
   icon = elm_icon_add(parent_widget);

   if ((edje_file_group_exists(file, p->s)) &&
       (elm_image_file_set(icon, file, p->s)))
     return icon;
   if (elm_icon_standard_set(icon, p->s))
     return icon;

   ERR("Failed to set icon: '%s'", p->s);
   evas_object_del(icon);
   return NULL;
}

void
external_common_icon_param_parse(Evas_Object **icon, Evas_Object *obj,
                                 const Eina_List *params)
{
   Edje_External_Param *p = edje_external_param_find(params, "icon");
   *icon = external_common_param_icon_get(obj, p);
}

Evas_Object *
external_common_param_edje_object_get(Evas_Object *obj,
                                      const Edje_External_Param *p)
{
   Evas_Object *edje, *parent_widget, *ret;
   const char *file;

   if ((!p) || (!p->s) || (p->type != EDJE_EXTERNAL_PARAM_TYPE_STRING))
       return NULL;

   edje = evas_object_smart_parent_get(obj);
   edje_object_file_get(edje, &file, NULL);

   parent_widget = elm_widget_parent_widget_get(obj);
   if (!parent_widget)
     parent_widget = edje;

   ret = edje_object_add(evas_object_evas_get(parent_widget));

   if (edje_object_file_set(ret, file, p->s))
     return ret;

   evas_object_del(ret);
   return NULL;
}

void
external_common_params_free(void *params)
{
   Elm_Params *p = params;
   if (p->style)
     eina_stringshare_del(p->style);
}

#define DEFINE_TYPE(type_name) \
  extern const Edje_External_Type external_##type_name##_type;
#include "modules.inc"
#undef DEFINE_TYPE

static Edje_External_Type_Info elm_external_types[] =
{
#define DEFINE_TYPE(type_name)              \
  { "elm/"#type_name, &external_##type_name##_type },
#include "modules.inc"
#undef DEFINE_TYPE
   { NULL, NULL }
};

static Eina_Bool
elm_mod_init(void)
{
   _elm_ext_log_dom = eina_log_domain_register("elm-externals",
                                               EINA_COLOR_LIGHTBLUE);
   edje_external_type_array_register(elm_external_types);
   return EINA_TRUE;
}

static void
elm_mod_shutdown(void)
{
   edje_external_type_array_unregister(elm_external_types);
   if (_elm_ext_log_dom >= 0) eina_log_domain_unregister(_elm_ext_log_dom);
   _elm_ext_log_dom = -1;
}

EINA_MODULE_INIT(elm_mod_init);
EINA_MODULE_SHUTDOWN(elm_mod_shutdown);
