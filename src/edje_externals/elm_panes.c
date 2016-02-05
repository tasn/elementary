#include "private.h"
#include <assert.h>

typedef struct _Elm_Params_Panes Elm_Params_Panes;

struct _Elm_Params_Panes {
   Elm_Params base;
   Evas_Object *content_left;
   Evas_Object *content_right;
   Eina_Bool is_horizontal;
   Eina_Bool horizontal;
   Eina_Bool is_left_size;
   double left_size;
   Eina_Bool is_fixed;
   Eina_Bool fixed;
};

static void external_panes_state_set(void *data EINA_UNUSED,
      Evas_Object *obj, const void *from_params,
      const void *to_params, float pos EINA_UNUSED)
{
   const Elm_Params_Panes *p;

   if (to_params) p = to_params;
   else if (from_params) p = from_params;
   else return;

   if (p->content_left)
     elm_object_part_content_set(obj, "left", p->content_left);

   if (p->content_right)
     elm_object_part_content_set(obj, "right", p->content_right);

   if (p->is_left_size)
     elm_panes_content_left_size_set(obj, p->left_size);

   if (p->is_horizontal)
     elm_panes_horizontal_set(obj, p->horizontal);

   if (p->is_fixed)
     elm_panes_fixed_set(obj, p->fixed);
}

static Eina_Bool external_panes_param_set(void *data EINA_UNUSED,
      Evas_Object *obj, const Edje_External_Param *param)
{
   if ((!strcmp(param->name, "content left"))
       && (param->type == EDJE_EXTERNAL_PARAM_TYPE_STRING))
   {
      Evas_Object *content = external_common_param_elm_layout_get(obj, param);
      if ((strcmp(param->s, "")) && (!content))
         return EINA_FALSE;
      elm_object_part_content_set(obj, "left", content);
      return EINA_TRUE;
   }
   else if ((!strcmp(param->name, "content right"))
            && (param->type == EDJE_EXTERNAL_PARAM_TYPE_STRING))
   {
      Evas_Object *content = external_common_param_elm_layout_get(obj, param);
      if ((strcmp(param->s, "")) && (!content))
        return EINA_FALSE;
      elm_object_part_content_set(obj, "right", content);
      return EINA_TRUE;
   }
   else if ((!strcmp(param->name, "horizontal"))
            && (param->type == EDJE_EXTERNAL_PARAM_TYPE_BOOL))
   {
      elm_panes_horizontal_set(obj, param->i);
      return EINA_TRUE;
   }
   else if ((!strcmp(param->name, "left size"))
            && (param->type == EDJE_EXTERNAL_PARAM_TYPE_DOUBLE))
   {
      elm_panes_content_left_size_set(obj, param->d);
      return EINA_TRUE;
   }
   else if ((!strcmp(param->name, "fixed"))
            && (param->type == EDJE_EXTERNAL_PARAM_TYPE_BOOL))
   {
      elm_panes_fixed_set(obj, param->i);
      return EINA_TRUE;
   }

   ERR("unknown parameter '%s' of type '%s'",
         param->name, edje_external_param_type_str(param->type));

   return EINA_FALSE;
}

static Eina_Bool
external_panes_param_get(void *data EINA_UNUSED, const Evas_Object *obj,
                         Edje_External_Param *param)
{
   if (!strcmp(param->name, "content left"))
     {
        /* not easy to get content name back from live object */
        return EINA_FALSE;
     }
   else if (!strcmp(param->name, "content right"))
     {
        /* not easy to get content name back from live object */
        return EINA_FALSE;
     }
   else if ((!strcmp(param->name, "horizontal"))
            && (param->type == EDJE_EXTERNAL_PARAM_TYPE_BOOL))
     {
        param->i = elm_panes_horizontal_get(obj);
        return EINA_TRUE;
     }
   else if ((!strcmp(param->name, "left size"))
            && (param->type == EDJE_EXTERNAL_PARAM_TYPE_DOUBLE))
     {
        param->d = elm_panes_content_left_size_get(obj);
        return EINA_TRUE;
     }
   else if ((!strcmp(param->name, "fixed"))
            && (param->type == EDJE_EXTERNAL_PARAM_TYPE_BOOL))
     {
        param->i = elm_panes_fixed_get(obj);
        return EINA_TRUE;
     }

   ERR("unknown parameter '%s' of type '%s'",
       param->name, edje_external_param_type_str(param->type));

   return EINA_FALSE;
}

static void *
external_panes_params_parse(void *data EINA_UNUSED, Evas_Object *obj,
                            const Eina_List *params)
{
   Elm_Params_Panes *mem;
   Edje_External_Param *param;
   const Eina_List *l;

   mem = calloc(1, sizeof(Elm_Params_Panes));
   if (!mem)
     return NULL;

   EINA_LIST_FOREACH(params, l, param)
     {
        if (!strcmp(param->name, "content left"))
          mem->content_left = external_common_param_elm_layout_get(obj, param);
        else if (!strcmp(param->name, "content right"))
          mem->content_right = external_common_param_elm_layout_get(obj, param);
        else if (!strcmp(param->name, "horizontal"))
          {
             mem->is_horizontal = EINA_TRUE;
             mem->horizontal = param->i;
          }
        else if (!strcmp(param->name, "left size"))
          {
             mem->is_left_size = EINA_TRUE;
             mem->left_size = param->d;
          }
        else if (!strcmp(param->name, "fixed"))
          {
             mem->is_fixed = EINA_TRUE;
             mem->fixed = param->i;
          }
     }

   return mem;
}

static Evas_Object *
external_panes_content_get(void *data EINA_UNUSED, const Evas_Object *obj,
                           const char *content)
{
   if (!strcmp(content, "left"))
     return elm_object_part_content_get(obj, "left");
   else if (!strcmp(content, "right"))
     return elm_object_part_content_get(obj, "right");

   ERR("unknown content '%s'", content);

   return NULL;
}

static void external_panes_params_free(void *params)
{
   free(params);
}

static Edje_External_Param_Info external_panes_params[] = {
   DEFINE_EXTERNAL_COMMON_PARAMS,
   EDJE_EXTERNAL_PARAM_INFO_STRING("content left"),
   EDJE_EXTERNAL_PARAM_INFO_STRING("content right"),
   EDJE_EXTERNAL_PARAM_INFO_BOOL("horizontal"),
   EDJE_EXTERNAL_PARAM_INFO_DOUBLE("left size"),
   EDJE_EXTERNAL_PARAM_INFO_BOOL("fixed"),
   EDJE_EXTERNAL_PARAM_INFO_SENTINEL
};

DEFINE_EXTERNAL_ICON_ADD(panes, "panes");
DEFINE_EXTERNAL_TYPE_SIMPLE(panes, "Panes");
