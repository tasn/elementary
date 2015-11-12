#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "elm_widget_glview.h"

#define MY_CLASS ELM_GLVIEW_CLASS

#define MY_CLASS_NAME "Elm_Glview"
#define MY_CLASS_NAME_LEGACY "elm_glview"

static const char SIG_FOCUSED[] = "focused";
static const char SIG_UNFOCUSED[] = "unfocused";

/* smart callbacks coming from elm glview objects: */
static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_FOCUSED, ""},
   {SIG_UNFOCUSED, ""},
   {SIG_WIDGET_LANG_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_ACCESS_CHANGED, ""}, /**< handled by elm_widget */
   {NULL, NULL}
};

EOLIAN static Eina_Bool
_elm_glview_elm_widget_on_focus(Eo *obj, Elm_Glview_Data *_pd EINA_UNUSED, Elm_Object_Item *item EINA_UNUSED)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);
   Eina_Bool int_ret = EINA_FALSE;

   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_on_focus(NULL));
   if (!int_ret) return EINA_FALSE;

   if (elm_widget_focus_get(obj))
     evas_object_focus_set(wd->resize_obj, EINA_TRUE);
   else
     evas_object_focus_set(wd->resize_obj, EINA_FALSE);

   return EINA_TRUE;
}

static void
_glview_update_surface(Evas_Object *obj)
{
   Evas_Native_Surface ns = { 0 };
   Evas_GL_Options_Bits opt;

   ELM_GLVIEW_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   if (!sd) return;

   evas_gl_make_current(sd->evasgl, NULL, NULL);

   if (sd->surface)
     {
        evas_object_image_native_surface_set(wd->resize_obj, NULL);
        evas_gl_surface_destroy(sd->evasgl, sd->surface);
     }

   evas_object_image_size_set(wd->resize_obj, sd->w, sd->h);

   opt = sd->config->options_bits;
   if ((opt & EVAS_GL_OPTIONS_DIRECT) &&
       (sd->render_policy != ELM_GLVIEW_RENDER_POLICY_ON_DEMAND))
     {
        if (!sd->warned_about_dr)
          {
             WRN("App requested direct rendering but render policy is not ON_DEMAND. "
                 "Disabling direct rendering...");
             sd->warned_about_dr = EINA_TRUE;
          }
        sd->config->options_bits &= ~(EVAS_GL_OPTIONS_DIRECT);
     }
   sd->surface = evas_gl_surface_create(sd->evasgl, sd->config, sd->w, sd->h);
   sd->config->options_bits = opt;
   evas_gl_native_surface_get(sd->evasgl, sd->surface, &ns);
   evas_object_image_native_surface_set(wd->resize_obj, &ns);
   elm_glview_changed_set(obj);

   // fake a resize event so that clients can reconfigure their viewport
   sd->resized = EINA_TRUE;
}

EOLIAN static void
_elm_glview_evas_object_smart_resize(Eo *obj, Elm_Glview_Data *sd, Evas_Coord w, Evas_Coord h)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_resize(w, h));

   sd->resized = EINA_TRUE;

   if (sd->scale_policy == ELM_GLVIEW_RESIZE_POLICY_RECREATE)
     {
        if ((w == 0) || (h == 0))
          {
             w = 64;
             h = 64;
          }

        if ((sd->w == w) && (sd->h == h)) return;

        sd->w = w;
        sd->h = h;

        _glview_update_surface(obj);
     }
}

static Eina_Bool
_render_cb(void *obj)
{
   ELM_GLVIEW_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);

   evas_object_render_op_set(wd->resize_obj, evas_object_render_op_get(obj));

   // Do a make current
   if (!evas_gl_make_current(sd->evasgl, sd->surface, sd->context))
     {
        sd->render_idle_enterer = NULL;
        ERR("Failed doing make current.\n");
        return EINA_FALSE;
     }

   // Call the init function if it hasn't been called already
   if (!sd->initialized)
     {
        if (sd->init_func) sd->init_func(obj);
        sd->initialized = EINA_TRUE;
     }

   if (sd->resized)
     {
        if (sd->resize_func) sd->resize_func(obj);
        sd->resized = EINA_FALSE;
     }

   if (sd->render_policy == ELM_GLVIEW_RENDER_POLICY_ALWAYS)
     evas_sync(evas_object_evas_get(obj));
   // Call the render function
   if (sd->render_func) sd->render_func(obj);

   // Depending on the policy return true or false
   if (sd->render_policy == ELM_GLVIEW_RENDER_POLICY_ON_DEMAND)
     return EINA_TRUE;
   else if (sd->render_policy == ELM_GLVIEW_RENDER_POLICY_ALWAYS)
     {
        // Return false so it only runs once
        sd->render_idle_enterer = NULL;
        return EINA_FALSE;
     }
   else
     {
        ERR("Invalid Render Policy.\n");
        sd->render_idle_enterer = NULL;
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

static void
_set_render_policy_callback(Evas_Object *obj)
{
   ELM_GLVIEW_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   switch (sd->render_policy)
     {
      case ELM_GLVIEW_RENDER_POLICY_ON_DEMAND:
         if (sd->render_idle_enterer)
              evas_object_image_pixels_dirty_set(wd->resize_obj, EINA_TRUE);
         // Delete idle_enterer if it for some reason is around
         ELM_SAFE_FREE(sd->render_idle_enterer, ecore_idle_enterer_del);

        // Set pixel getter callback
        evas_object_image_pixels_get_callback_set
          (wd->resize_obj,
          (Evas_Object_Image_Pixels_Get_Cb)_render_cb,
          obj);
        break;

      case ELM_GLVIEW_RENDER_POLICY_ALWAYS:
        if (evas_object_image_pixels_dirty_get(wd->resize_obj))
          sd->render_idle_enterer = ecore_idle_enterer_before_add((Ecore_Task_Cb)_render_cb, obj);
        // Unset the pixel getter callback if set already
        evas_object_image_pixels_get_callback_set
          (wd->resize_obj, NULL, NULL);

        break;

      default:
        ERR("Invalid Render Policy.\n");
        return;
     }
}

EOLIAN static void
_elm_glview_evas_object_smart_add(Eo *obj, Elm_Glview_Data *priv EINA_UNUSED)
{
   Evas_Object *img;

   elm_widget_sub_object_parent_add(obj);

   // Create image to render Evas_GL Surface
   img = evas_object_image_filled_add(evas_object_evas_get(obj));
   elm_widget_resize_object_set(obj, img, EINA_TRUE);
   evas_object_image_size_set(img, 1, 1);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());
}

static void
_elm_glview_constructor(Eo *obj, Elm_Glview_Data *priv)
{
   // Evas_GL
   priv->evasgl = evas_gl_new(evas_object_evas_get(obj));
   if (!priv->evasgl)
     {
        ERR("Failed Creating an Evas GL Object.\n");

        return;
     }

   // Create a default config
   priv->config = evas_gl_config_new();
   if (!priv->config)
     {
        ERR("Failed Creating a Config Object.\n");

        evas_gl_free(priv->evasgl);
        priv->evasgl = NULL;
        return;
     }
   priv->config->color_format = EVAS_GL_RGB_888;

   // Initialize variables
   priv->scale_policy = ELM_GLVIEW_RESIZE_POLICY_RECREATE;
   priv->render_policy = ELM_GLVIEW_RENDER_POLICY_ON_DEMAND;

   // Initialize it to (64,64)  (It's an arbitrary value)
   priv->w = 64;
   priv->h = 64;

   // Set context version
   if (!priv->gles_version)
     priv->gles_version = EVAS_GL_GLES_2_X;
   priv->config->gles_version = priv->gles_version;

   // Create Context
   if (priv->gles_version == EVAS_GL_GLES_2_X)
     priv->context = evas_gl_context_create(priv->evasgl, NULL);
   else
     priv->context = evas_gl_context_version_create(priv->evasgl, NULL, priv->gles_version);
   if (!priv->context)
     {
        ERR("Error Creating an Evas_GL Context.\n");

        ELM_SAFE_FREE(priv->config, evas_gl_config_free);
        ELM_SAFE_FREE(priv->evasgl, evas_gl_free);
        return;
     }
}

EOLIAN static void
_elm_glview_evas_object_smart_del(Eo *obj, Elm_Glview_Data *sd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   // Call delete func if it's registered
   if (sd->del_func)
     {
        evas_gl_make_current(sd->evasgl, sd->surface, sd->context);
        sd->del_func(obj);
     }

   ecore_idle_enterer_del(sd->render_idle_enterer);
   evas_gl_make_current(sd->evasgl, NULL, NULL);

   if (sd->surface)
     {
        evas_object_image_native_surface_set(wd->resize_obj, NULL);
        evas_gl_surface_destroy(sd->evasgl, sd->surface);
     }
   if (sd->context) evas_gl_context_destroy(sd->evasgl, sd->context);
   if (sd->config) evas_gl_config_free(sd->config);
   if (sd->evasgl) evas_gl_free(sd->evasgl);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

EAPI Evas_Object *
elm_glview_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent,
                             elm_obj_glview_version_constructor(EVAS_GL_GLES_2_X));
   return obj;
}

EAPI Evas_Object *
elm_glview_version_add(Evas_Object *parent, Evas_GL_Context_Version version)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent,
                             elm_obj_glview_version_constructor(version));
   return obj;
}

EOLIAN static void
_elm_glview_version_constructor(Eo *obj, Elm_Glview_Data *sd,
                                Evas_GL_Context_Version version)
{
   sd->gles_version =
     ((version > 0) && (version <= 3)) ? version : EVAS_GL_GLES_2_X;
   _elm_glview_constructor(obj, sd);

   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_ANIMATION));
}

EOLIAN static Eo *
_elm_glview_eo_base_finalize(Eo *obj, Elm_Glview_Data *sd)
{
   if (!sd->evasgl)
     {
        ERR("Failed");
        return NULL;
     }

   return eo_do_super_ret(obj, MY_CLASS, obj, eo_finalize());
}

EOLIAN static Evas_GL_API*
_elm_glview_gl_api_get(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd)
{
   return evas_gl_context_api_get(sd->evasgl, sd->context);
}

EOLIAN static Eina_Bool
_elm_glview_mode_set(Eo *obj, Elm_Glview_Data *sd, Elm_GLView_Mode mode)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);

   // Set the configs
   if (mode & ELM_GLVIEW_ALPHA) sd->config->color_format = EVAS_GL_RGBA_8888;
   else sd->config->color_format = EVAS_GL_RGB_888;

   if (mode & ELM_GLVIEW_DEPTH)
     {
        const int mask = 7 << 6;
        if ((mode & mask) == (ELM_GLVIEW_DEPTH_8 & mask))
          sd->config->depth_bits = EVAS_GL_DEPTH_BIT_8;
        else if ((mode & mask) == (ELM_GLVIEW_DEPTH_16 & mask))
          sd->config->depth_bits = EVAS_GL_DEPTH_BIT_16;
        else if ((mode & mask) == (ELM_GLVIEW_DEPTH_24 & mask))
          sd->config->depth_bits = EVAS_GL_DEPTH_BIT_24;
        else if ((mode & mask) == (ELM_GLVIEW_DEPTH_32 & mask))
          sd->config->depth_bits = EVAS_GL_DEPTH_BIT_32;
        else
          sd->config->depth_bits = EVAS_GL_DEPTH_BIT_24;
     }
   else
     sd->config->depth_bits = EVAS_GL_DEPTH_NONE;

   if (mode & ELM_GLVIEW_STENCIL)
     {
        const int mask = 7 << 9;
        if ((mode & mask) == (ELM_GLVIEW_STENCIL_1 & mask))
          sd->config->stencil_bits = EVAS_GL_STENCIL_BIT_1;
        else if ((mode & mask) == (ELM_GLVIEW_STENCIL_1 & mask))
          sd->config->stencil_bits = EVAS_GL_STENCIL_BIT_2;
        else if ((mode & mask) == (ELM_GLVIEW_STENCIL_4 & mask))
          sd->config->stencil_bits = EVAS_GL_STENCIL_BIT_4;
        else if ((mode & mask) == (ELM_GLVIEW_STENCIL_8 & mask))
          sd->config->stencil_bits = EVAS_GL_STENCIL_BIT_8;
        else if ((mode & mask) == (ELM_GLVIEW_STENCIL_16 & mask))
          sd->config->stencil_bits = EVAS_GL_STENCIL_BIT_16;
        else
          sd->config->stencil_bits = EVAS_GL_STENCIL_BIT_8;
     }
   else
     sd->config->stencil_bits = EVAS_GL_STENCIL_NONE;

   if (mode & ELM_GLVIEW_MULTISAMPLE_HIGH)
     {
        if ((mode & ELM_GLVIEW_MULTISAMPLE_HIGH) == ELM_GLVIEW_MULTISAMPLE_LOW)
          sd->config->multisample_bits = EVAS_GL_MULTISAMPLE_LOW;
        else if ((mode & ELM_GLVIEW_MULTISAMPLE_HIGH) == ELM_GLVIEW_MULTISAMPLE_MED)
          sd->config->multisample_bits = EVAS_GL_MULTISAMPLE_MED;
        else
          sd->config->multisample_bits = EVAS_GL_MULTISAMPLE_HIGH;
     }
   else
     sd->config->multisample_bits = EVAS_GL_MULTISAMPLE_NONE;

   sd->config->options_bits = EVAS_GL_OPTIONS_NONE;
   if (mode & ELM_GLVIEW_DIRECT)
     sd->config->options_bits = EVAS_GL_OPTIONS_DIRECT;
   if (mode & ELM_GLVIEW_CLIENT_SIDE_ROTATION)
     sd->config->options_bits |= EVAS_GL_OPTIONS_CLIENT_SIDE_ROTATION;

   // Check for Alpha Channel and enable it
   if (mode & ELM_GLVIEW_ALPHA)
     evas_object_image_alpha_set(wd->resize_obj, EINA_TRUE);
   else
     evas_object_image_alpha_set(wd->resize_obj, EINA_FALSE);

   sd->mode = mode;
   sd->warned_about_dr = EINA_FALSE;

   _glview_update_surface(obj);
   if (!sd->surface)
     {
        ERR("Failed to create a surface with the requested configuration.");
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_glview_resize_policy_set(Eo *obj, Elm_Glview_Data *sd, Elm_GLView_Resize_Policy policy)
{
   if (policy == sd->scale_policy) return EINA_TRUE;
   switch (policy)
     {
      case ELM_GLVIEW_RESIZE_POLICY_RECREATE:
      case ELM_GLVIEW_RESIZE_POLICY_SCALE:
        sd->scale_policy = policy;
        _glview_update_surface(obj);
        elm_glview_changed_set(obj);
        return EINA_TRUE;

      default:
        ERR("Invalid Scale Policy.\n");
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_glview_render_policy_set(Eo *obj, Elm_Glview_Data *sd, Elm_GLView_Render_Policy policy)
{
   if ((policy != ELM_GLVIEW_RENDER_POLICY_ON_DEMAND) &&
       (policy != ELM_GLVIEW_RENDER_POLICY_ALWAYS))
     {
        ERR("Invalid Render Policy.\n");
        return EINA_FALSE;
     }

   if (sd->render_policy == policy) return EINA_TRUE;

   sd->warned_about_dr = EINA_FALSE;
   sd->render_policy = policy;
   _set_render_policy_callback(obj);

   return EINA_TRUE;
}

EOLIAN static void
_elm_glview_size_set(Eo *obj, Elm_Glview_Data *sd, int w, int h)
{
   if ((w == sd->w) && (h == sd->h)) return;

   sd->w = w;
   sd->h = h;

   _glview_update_surface(obj);
   elm_glview_changed_set(obj);
}

EOLIAN static void
_elm_glview_size_get(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd, int *w, int *h)
{
   if (w) *w = sd->w;
   if (h) *h = sd->h;
}

EOLIAN static void
_elm_glview_init_func_set(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd, Elm_GLView_Func_Cb func)
{
   sd->initialized = EINA_FALSE;
   sd->init_func = func;
}

EOLIAN static void
_elm_glview_del_func_set(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd, Elm_GLView_Func_Cb func)
{
   sd->del_func = func;
}

EOLIAN static void
_elm_glview_resize_func_set(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd, Elm_GLView_Func_Cb func)
{
   sd->resize_func = func;
}

EOLIAN static void
_elm_glview_render_func_set(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd, Elm_GLView_Func_Cb func)
{
   sd->render_func = func;
   _set_render_policy_callback(obj);
}

EOLIAN static void
_elm_glview_changed_set(Eo *obj, Elm_Glview_Data *sd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   evas_object_image_pixels_dirty_set
     (wd->resize_obj, EINA_TRUE);
   if (sd->render_policy == ELM_GLVIEW_RENDER_POLICY_ALWAYS &&
       !sd->render_idle_enterer)
     sd->render_idle_enterer =
       ecore_idle_enterer_before_add((Ecore_Task_Cb)_render_cb, obj);
}

EOLIAN static Evas_GL *
_elm_glview_evas_gl_get(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd)
{
   return sd->evasgl;
}

EOLIAN static int
_elm_glview_rotation_get(Eo *obj EINA_UNUSED, Elm_Glview_Data *sd)
{
   return evas_gl_rotation_get(sd->evasgl);
}

static void
_elm_glview_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

#include "elm_glview.eo.c"
