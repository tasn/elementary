#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#include <Elementary.h>
#include "elm_priv.h"
#include "elm_widget_combobox.h"

#define MY_CLASS ELM_COMBOBOX_CLASS

#define MY_CLASS_NAME "Elm_Combobox"
#define MY_CLASS_NAME_LEGACY "elm_combobox"

static const char SIG_SELECTED[] = "selected";
static const char SIG_DISMISSED[] = "dismissed";
static const char SIG_EXPANDED[] = "expanded";
static const char SIG_ITEM_FOCUSED[] = "item,focused";
static const char SIG_ITEM_UNFOCUSED[] = "item,unfocused";

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_SELECTED, ""},
   {SIG_DISMISSED, ""},
   {SIG_EXPANDED, ""},
   {SIG_ITEM_FOCUSED, ""},
   {SIG_ITEM_UNFOCUSED, ""},
   {"clicked", ""}, /**< handled by parent button class */
   {SIG_WIDGET_LANG_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_ACCESS_CHANGED, ""}, /**< handled by elm_widget */
   {NULL, NULL}
};

static Eina_Bool _key_action_move(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"move", _key_action_move},
   {NULL, NULL}
};

EOLIAN static Eina_Bool
_elm_combobox_elm_widget_translate(Eo *obj EINA_UNUSED, Elm_Combobox_Data *sd)
{
   eo_do_super(obj, MY_CLASS, elm_obj_widget_translate());
   eo_do(sd->genlist, elm_obj_widget_translate());
   eo_do(sd->entry, elm_obj_widget_translate());

   if (sd->hover)
     eo_do(sd->hover, elm_obj_widget_translate());

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_combobox_elm_widget_theme_apply(Eo *obj, Elm_Combobox_Data *sd)
{
   const char *style;
   Eina_Bool int_ret = EINA_FALSE;
   Eina_Bool mirrored;
   char buf[4096];

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);

   style = elm_widget_style_get(obj);

   /* if (sd->horizontal) */
   /*   snprintf(buf, sizeof(buf), "combobox_horizontal/%s", style); */
   /* else */
   snprintf(buf, sizeof(buf), "combobox_vertical/%s", style);

   /* combobox's style has no extra bit for orientation but could have... */
   eina_stringshare_replace(&(wd->style), buf);

   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_theme_apply());
   if (!int_ret) return EINA_FALSE;

   mirrored = elm_widget_mirrored_get(obj);

   if (sd->hover)
     elm_widget_mirrored_set(sd->hover, mirrored);

   elm_widget_mirrored_set(sd->genlist, mirrored);
   elm_widget_mirrored_set(sd->entry, mirrored);

   elm_combobox_hover_end(obj);

   return EINA_TRUE;
}

static void
_on_hover_clicked(void *data,
                  Evas_Object *obj EINA_UNUSED,
                  void *event_info EINA_UNUSED)
{
   elm_combobox_hover_end(data);
}

static void
_activate(Evas_Object *obj)
{
   char buf[4096];

   ELM_COMBOBOX_DATA_GET(obj, sd);

   if (sd->expanded)
     {
        elm_combobox_hover_end(obj);
        return;
     }
   if (elm_widget_disabled_get(obj)) return;

   sd->expanded = EINA_TRUE;

   if (sd->hover)
     {
        evas_object_show(sd->hover);
        return;
     }

   sd->hover = elm_hover_add(sd->hover_parent);
   elm_widget_sub_object_add(obj, sd->hover);
   elm_widget_mirrored_automatic_set(sd->hover, EINA_FALSE);

   /* if (sd->horizontal) */
   /*   snprintf(buf, sizeof(buf), "combobox_horizontal/%s", */
   /*            elm_widget_style_get(obj)); */
   /* else */
   snprintf(buf, sizeof(buf), "combobox_vertical/%s",
            elm_widget_style_get(obj));

   elm_object_style_set(sd->hover, buf);

   evas_object_smart_callback_add
     (sd->hover, "clicked", _on_hover_clicked, obj);
   elm_hover_target_set(sd->hover, obj);

   /* hover's content */
   /* if (sd->horizontal) */
   /*   elm_object_part_content_set(sd->hover, elm_hover_best_content_location_get */
   /*                                 (sd->hover, ELM_HOVER_AXIS_HORIZONTAL), bx); */
   /* else */
   elm_object_part_content_set(sd->hover, elm_hover_best_content_location_get
                               (sd->hover, ELM_HOVER_AXIS_VERTICAL), sd->genlist);

   evas_object_smart_callback_call(obj, SIG_EXPANDED, NULL);
   evas_object_show(sd->hover);
}

static void
_on_clicked(void *data,
            Evas_Object *obj EINA_UNUSED,
            void *event_info EINA_UNUSED)
{
   _activate(data);
}

static void
_on_parent_del(void *data,
               Evas *e EINA_UNUSED,
               Evas_Object *obj EINA_UNUSED,
               void *event_info EINA_UNUSED)
{
   elm_combobox_hover_parent_set(data, NULL);
}

EOLIAN static void
_elm_combobox_evas_object_smart_add(Eo *obj, Elm_Combobox_Data *sd EINA_UNUSED)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());
   elm_widget_sub_object_parent_add(obj);

   elm_widget_mirrored_automatic_set(obj, EINA_FALSE);

   evas_object_smart_callback_add(obj, "clicked", _on_clicked, obj);

   //What are you doing here?
   eo_do(obj, elm_obj_widget_theme_apply());
}

EOLIAN static void
_elm_combobox_evas_object_smart_del(Eo *obj, Elm_Combobox_Data *sd EINA_UNUSED)
{
   elm_combobox_hover_parent_set(obj, NULL);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

EOLIAN static void
_elm_combobox_evas_object_smart_show(Eo *obj, Elm_Combobox_Data *sd)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_show());
   if (sd->expanded) evas_object_show(sd->hover);
}

EOLIAN static void
_elm_combobox_evas_object_smart_hide(Eo *obj, Elm_Combobox_Data *sd)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_hide());
   if (sd->hover) evas_object_hide(sd->hover);
}

EOLIAN static void
_elm_combobox_elm_widget_parent_set(Eo *obj, Elm_Combobox_Data *sd EINA_UNUSED, Evas_Object *parent)
{
   elm_combobox_hover_parent_set(obj, parent);
}

EOLIAN static Eina_Bool
_elm_combobox_elm_button_admits_autorepeat_get(Eo *obj EINA_UNUSED, Elm_Combobox_Data *sd EINA_UNUSED)
{
   return EINA_FALSE;
}

EAPI Evas_Object *
elm_combobox_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

EOLIAN static void
_elm_combobox_eo_base_constructor(Eo *obj, Elm_Combobox_Data *sd)
{
   Evas_Object *gl;
   Evas_Object *entry;
   char buf[4096];

   eo_do_super(obj, MY_CLASS, eo_constructor());

   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_GLASS_PANE));

   /* if (sd->horizontal) */
   /*   snprintf(buf, sizeof(buf), "combobox_horizontal_entry/%s", */
   /*            elm_widget_style_get(obj)); */
   /* else */
   snprintf(buf, sizeof(buf), "combobox_vertical_entry/%s",
            elm_widget_style_get(obj));

   // This is the genlist object that will take over the genlist call
   sd->genlist = gl = eo_add(ELM_GENLIST_CLASS, obj);
   elm_widget_mirrored_automatic_set(gl, EINA_FALSE);
   elm_widget_mirrored_set(gl, elm_widget_mirrored_get(obj));
   elm_object_style_set(gl, buf);
   evas_object_hide(gl);

   // This is the entry object that will take over the entry call
   sd->entry = entry = eo_add(ELM_ENTRY_CLASS, obj);
   elm_widget_mirrored_automatic_set(entry, EINA_FALSE);
   elm_widget_mirrored_set(entry, elm_widget_mirrored_get(obj));
   elm_scroller_policy_set(entry, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_entry_scrollable_set(entry, EINA_TRUE);
   elm_entry_single_line_set(entry, EINA_TRUE);
   elm_object_style_set(entry, buf);
   evas_object_show(entry);

   eo_do(obj,
         eo_composite_attach(gl),
         eo_composite_attach(entry));

   elm_object_part_content_set(obj, "elm.swallow.content", entry);
}

EOLIAN static void
_elm_combobox_hover_parent_set(Eo *obj, Elm_Combobox_Data *sd, Evas_Object *parent)
{
   if (sd->hover_parent)
     evas_object_event_callback_del_full
       (sd->hover_parent, EVAS_CALLBACK_DEL, _on_parent_del, obj);

   sd->hover_parent = parent;
   if (sd->hover_parent)
     evas_object_event_callback_add
       (sd->hover_parent, EVAS_CALLBACK_DEL, _on_parent_del, obj);
}

EOLIAN static Evas_Object*
_elm_combobox_hover_parent_get(Eo *obj EINA_UNUSED, Elm_Combobox_Data *sd)
{
   return sd->hover_parent;
}

EOLIAN static void
_elm_combobox_hover_begin(Eo *obj, Elm_Combobox_Data *sd)
{
   if (sd->hover) return;

   _activate(obj);
}

EOLIAN static void
_elm_combobox_hover_end(Eo *obj, Elm_Combobox_Data *sd)
{
   if (!sd->hover) return;

   sd->expanded = EINA_FALSE;

   evas_object_hide(sd->hover);
   evas_object_smart_callback_call(obj, SIG_DISMISSED, NULL);
}

EOLIAN static Eina_Bool
_elm_combobox_expanded_get(Eo *obj EINA_UNUSED, Elm_Combobox_Data *sd)
{
   return sd->expanded;
}

static Eina_Bool
_key_action_move(Evas_Object *obj EINA_UNUSED, const char *params EINA_UNUSED)
{
#if 0
   ELM_COMBOBOX_DATA_GET(obj, sd);
   const char *dir = params;

   if (!strcmp(dir, "down"))
     {
        if (!sd->horizontal)
          {
            elm_object_focus_set(VIEW(fitem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_DOWN);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "up"))
     {
        if (!sd->horizontal)
          {
            elm_object_focus_set(VIEW(litem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_UP);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "left"))
     {
        if (sd->horizontal)
          {
            elm_object_focus_set(VIEW(litem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_LEFT);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "right"))
     {
        if (sd->horizontal)
          {
            elm_object_focus_set(VIEW(fitem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_RIGHT);
        return EINA_TRUE;
     }
   else
#endif
     return EINA_FALSE;
}

EOLIAN static Eina_Bool
_elm_combobox_elm_widget_event(Eo *obj, Elm_Combobox_Data *sd, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
   (void) src;
   Eina_Bool int_ret = EINA_FALSE;
   Evas_Event_Key_Down *ev = event_info;

   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_event(src, type, event_info));
   if (int_ret) return EINA_FALSE;

   if (!sd || !sd->hover) return EINA_FALSE;
   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

static void
_elm_combobox_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EOLIAN const Elm_Atspi_Action *
_elm_combobox_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Combobox_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "move,up", "move", "up", _key_action_move},
          { "move,down", "move", "down", _key_action_move},
          { "move,left", "move", "left", _key_action_move},
          { "move,right", "move", "right", _key_action_move},
          { NULL, NULL, NULL, NULL}
   };
   return &atspi_actions[0];
}

EOLIAN void
_elm_combobox_elm_widget_part_text_set(Eo *obj EINA_UNUSED, Elm_Combobox_Data *pd, const char * part, const char *label)
{
   elm_object_part_text_set(pd->entry, part, label);
}

EOLIAN const char *
_elm_combobox_elm_widget_part_text_get(Eo *obj EINA_UNUSED, Elm_Combobox_Data *pd, const char * part)
{
   return elm_object_part_text_get(pd->entry, part);
}


#include "elm_combobox.eo.c"
