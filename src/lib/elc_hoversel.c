#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#define ELM_WIDGET_ITEM_PROTECTED
#include <Elementary.h>
#include "elm_priv.h"
#include "elm_widget_hoversel.h"

#define MY_CLASS ELM_HOVERSEL_CLASS

#define MY_CLASS_NAME "Elm_Hoversel"
#define MY_CLASS_NAME_LEGACY "elm_hoversel"

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
static Eina_Bool _key_action_activate(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"move", _key_action_move},
   {"activate", _key_action_activate},
   {NULL, NULL}
};

EOLIAN static Eina_Bool
_elm_hoversel_elm_widget_translate(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd)
{
   Eo *it;
   Eina_List *l;

   EINA_LIST_FOREACH(sd->items, l, it)
     eo_do(it, elm_wdg_item_translate());

   eo_do_super(obj, MY_CLASS, elm_obj_widget_translate());

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_hoversel_elm_widget_theme_apply(Eo *obj, Elm_Hoversel_Data *sd)
{
   Eina_Bool int_ret = EINA_FALSE;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);

   char buf[4096];
   const char *style;

   style = eina_stringshare_add(elm_widget_style_get(obj));

   if (sd->horizontal)
     snprintf(buf, sizeof(buf), "hoversel_horizontal/%s", style);
   else
     snprintf(buf, sizeof(buf), "hoversel_vertical/%s", style);

   /* hoversel's style has an extra bit: orientation */
   eina_stringshare_replace(&(wd->style), buf);

   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_theme_apply());
   if (!int_ret) return EINA_FALSE;

   eina_stringshare_replace(&(wd->style), style);

   eina_stringshare_del(style);

   if (sd->hover)
     elm_widget_mirrored_set(sd->hover, elm_widget_mirrored_get(obj));

   elm_hoversel_hover_end(obj);

   return EINA_TRUE;
}

static Eina_Bool
_on_hover_clicked(void *data EINA_UNUSED,
                     Eo *obj, const Eo_Event_Description *desc EINA_UNUSED,
                     void *event_info EINA_UNUSED)
{
   const char *dismissstr;

   dismissstr = edje_object_data_get(elm_layout_edje_get(obj), "dismiss");

   if (!dismissstr || strcmp(dismissstr, "on"))
     elm_hoversel_hover_end(data); // for backward compatibility

   return EINA_TRUE;
}

static Eina_Bool
_on_item_clicked(void *data EINA_UNUSED,
                     Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                     void *event_info EINA_UNUSED)
{
   Elm_Hoversel_Item_Data *item = data;
   Evas_Object *obj2 = WIDGET(item);
   Elm_Object_Item *eo_it = EO_OBJ(item);

   ELM_HOVERSEL_DATA_GET(obj2, sd);

   if (item->func) item->func((void *)WIDGET_ITEM_DATA_GET(eo_it), obj2, eo_it);
   eo_do(obj2, eo_event_callback_call(EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, eo_it));

   if (sd->auto_update)
     {
        Evas_Object *ic;

        ic = elm_object_part_content_unset(obj2, "icon");
        ELM_SAFE_FREE(ic, evas_object_del);

        if (item->icon_file)
          {
             ic = elm_icon_add(obj2);
             elm_image_resizable_set(ic, EINA_FALSE, EINA_TRUE);
             if (item->icon_type == ELM_ICON_FILE)
               elm_image_file_set(ic, item->icon_file, item->icon_group);
             else if (item->icon_type == ELM_ICON_STANDARD)
               elm_icon_standard_set(ic, item->icon_file);
             elm_object_part_content_set(obj2, "icon", ic);
          }

        if(item->label)
          elm_object_text_set(obj2, item->label);
     }

   elm_hoversel_hover_end(obj2);

   return EINA_TRUE;
}

static Eina_Bool
_item_focused_cb(void *data EINA_UNUSED,
                     Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                     void *event_info EINA_UNUSED)
{
   Elm_Hoversel_Item_Data *it = data;

   eo_do(WIDGET(it), eo_event_callback_call(ELM_HOVERSEL_EVENT_ITEM_FOCUSED, EO_OBJ(it)));

   return EINA_TRUE;
}

static Eina_Bool
_item_unfocused_cb(void *data EINA_UNUSED,
                   Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                   void *event_info EINA_UNUSED)
{
   Elm_Hoversel_Item_Data *it = data;

   eo_do(WIDGET(it), eo_event_callback_call(ELM_HOVERSEL_EVENT_ITEM_UNFOCUSED, EO_OBJ(it)));

   return EINA_TRUE;
}

static void
_create_scroller(Evas_Object *obj, Elm_Hoversel_Data *sd)
{
   //table
   sd->tbl = elm_table_add(obj);

   //spacer
   sd->spacer = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_color_set(sd->spacer, 0, 0, 0, 0);
   elm_table_pack(sd->tbl, sd->spacer, 0, 0, 1, 1);

   //Scroller
   sd->scr = elm_scroller_add(sd->tbl);
   elm_object_style_set(sd->scr, "popup/no_inset_shadow");
   evas_object_size_hint_weight_set(sd->scr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(sd->scr, EVAS_HINT_FILL, EVAS_HINT_FILL);
   if (sd->horizontal)
     {
        elm_scroller_policy_set(sd->scr, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
        elm_scroller_content_min_limit(sd->scr, EINA_FALSE, EINA_TRUE);
        elm_scroller_bounce_set(sd->scr, EINA_TRUE, EINA_FALSE);
     }
   else
     {
        elm_scroller_policy_set(sd->scr, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
        elm_scroller_content_min_limit(sd->scr, EINA_TRUE, EINA_FALSE);
        elm_scroller_bounce_set(sd->scr, EINA_FALSE, EINA_TRUE);
     }
   elm_table_pack(sd->tbl, sd->scr, 0, 0, 1, 1);
   evas_object_show(sd->scr);
}

static void
_resizing_eval(Evas_Object *obj, Elm_Hoversel_Data *sd)
{
   Evas_Object *bx = NULL;
   const char *max_size_str;
   int max_size = 0;
   char buf[128];
   Evas_Coord box_w = -1, box_h = -1;
   Evas_Coord x, y, w, h, xx, yy, ww, hh, vw = 0, vh = 0;
   double align_x;
   Eina_List *l;
   Evas_Object *it;
   Evas_Coord obj_x, obj_y, obj_w, obj_h, it_w, it_h;

   if (sd->scr)
     bx = elm_object_content_get(sd->scr);

   if ((!sd->expanded) || (!bx)) return;

   edje_object_size_min_calc(elm_layout_edje_get(sd->scr), &vw, &vh);
   evas_object_geometry_get(obj, &obj_x, &obj_y, &obj_w, &obj_h);

   evas_object_size_hint_align_get(obj, &align_x, NULL);
   if (!sd->horizontal && align_x == EVAS_HINT_FILL)
     {
        l = elm_box_children_get(bx);
        EINA_LIST_FREE(l, it)
          {
             edje_object_size_min_calc(elm_layout_edje_get(it), &it_w, &it_h);
             if ((obj_w - vw) > it_w)
               evas_object_size_hint_min_set(it, (obj_w - vw), it_h);
             else
               evas_object_size_hint_min_set(it, it_w, it_h);
          }
     }

   elm_box_recalculate(bx);
   evas_object_size_hint_min_get(bx, &box_w, &box_h);

   box_w += vw;
   box_h += vh;

   max_size_str = edje_object_data_get(elm_layout_edje_get(sd->hover), "max_size");
   if (max_size_str)
     max_size = (int)(atoi(max_size_str)
                      * elm_config_scale_get()
                      * elm_object_scale_get(obj))
                      / edje_object_base_scale_get(elm_layout_edje_get(sd->hover));

   if (sd->horizontal)
     {
        ww = (max_size > 0) ? MIN(box_w, max_size) : box_w ;
        hh = box_h;

        evas_object_size_hint_min_set(sd->spacer, ww, hh);

        if (!sd->last_location)
          sd->last_location = elm_hover_best_content_location_get(sd->hover, ELM_HOVER_AXIS_HORIZONTAL);
     }
   else
     {
        ww = box_w;
        hh = (max_size > 0) ? MIN(box_h, max_size) : box_h ;

        evas_object_size_hint_min_set(sd->spacer, ww, hh);

        if (!sd->last_location)
          sd->last_location = elm_hover_best_content_location_get(sd->hover, ELM_HOVER_AXIS_VERTICAL);
     }

   evas_object_geometry_get(sd->hover_parent, &x, &y, &w, &h);
   if (eo_isa(sd->hover_parent, ELM_WIN_CLASS))
     {
        x = 0;
        y = 0;
     }

   snprintf(buf, sizeof(buf), "elm.swallow.slot.%s", sd->last_location);
   edje_object_part_geometry_get(elm_layout_edje_get(sd->hover), buf, &xx, &yy, NULL, NULL);
   xx += x;
   yy += y;

   if (sd->horizontal)
     {
        if (xx < obj_x)
          {
             xx = x;
             if ((xx + ww) > obj_x)
               ww = obj_x - xx;
          }
        else
          {
             if ((xx + ww) > (x + w))
               ww = (x + w) - xx;
          }

        if (yy < 0) yy = y;
        if ((yy + hh) > (y + h))
          hh = (y + h) - yy;
     }
   else
     {
        if (yy < obj_y)
          {
             yy = y;
             if ((yy + hh) > obj_y)
               hh = obj_y - yy;
          }
        else
          {
             if ((yy + hh) > (y + h))
               hh = (y + h) - yy;
          }

        if (xx < 0) xx = x;
        if ((xx + ww) > (x + w))
          ww = (x + w) - xx;
     }

   evas_object_size_hint_min_set(sd->spacer, ww, hh);
}

static void
_hover_end_finished(void *data,
                    Evas_Object *obj EINA_UNUSED,
                    const char *emission EINA_UNUSED,
                    const char *source EINA_UNUSED)
{
   Elm_Object_Item *eo_item;
   Eina_List *l;
   const char *dismissstr;

   ELM_HOVERSEL_DATA_GET(data, sd);

   dismissstr = edje_object_data_get(elm_layout_edje_get(sd->hover), "dismiss");

   if (dismissstr && !strcmp(dismissstr, "on"))
     {
        sd->expanded = EINA_FALSE;

        EINA_LIST_FOREACH(sd->items, l, eo_item)
          {
             ELM_HOVERSEL_ITEM_DATA_GET(eo_item, it);
             VIEW(it) = NULL;
          }
        ELM_SAFE_FREE(sd->hover, evas_object_del);
        sd->scr = NULL;
        sd->last_location = NULL;

        eo_do(obj, eo_event_callback_call(ELM_HOVERSEL_EVENT_DISMISSED, NULL));
     }
}

static void
_activate(Evas_Object *obj)
{
   Elm_Object_Item *eo_item;
   Evas_Object *bt, *bx, *ic;
   const Eina_List *l;
   char buf[4096];

   ELM_HOVERSEL_DATA_GET(obj, sd);

   if (sd->expanded)
     {
        elm_hoversel_hover_end(obj);
        return;
     }
   sd->expanded = EINA_TRUE;

   if (elm_widget_disabled_get(obj)) return;

   sd->hover = elm_hover_add(sd->hover_parent);
   elm_widget_sub_object_add(obj, sd->hover);

   if (sd->horizontal)
     snprintf(buf, sizeof(buf), "hoversel_horizontal/%s",
              elm_widget_style_get(obj));
   else
     snprintf(buf, sizeof(buf), "hoversel_vertical/%s",
              elm_widget_style_get(obj));

   elm_object_style_set(sd->hover, buf);

   eo_do(sd->hover, eo_event_callback_add
     (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, _on_hover_clicked, obj));
   elm_layout_signal_callback_add
     (sd->hover, "elm,action,hide,finished", "elm", _hover_end_finished, obj);
   elm_hover_target_set(sd->hover, obj);

   /* hover's content */
   bx = elm_box_add(sd->hover);
   elm_box_homogeneous_set(bx, EINA_TRUE);
   elm_box_horizontal_set(bx, sd->horizontal);

   if (sd->horizontal)
     snprintf(buf, sizeof(buf), "hoversel_horizontal_entry/%s",
              elm_widget_style_get(obj));
   else
     snprintf(buf, sizeof(buf), "hoversel_vertical_entry/%s",
              elm_widget_style_get(obj));

   EINA_LIST_FOREACH(sd->items, l, eo_item)
     {
        ELM_HOVERSEL_ITEM_DATA_GET(eo_item, item);
        VIEW(item) = bt = elm_button_add(bx);
        elm_widget_mirrored_set(bt, elm_widget_mirrored_get(obj));
        elm_object_style_set(bt, buf);
        elm_object_text_set(bt, item->label);

        if (item->icon_file)
          {
             ic = elm_icon_add(bt);
             elm_image_resizable_set(ic, EINA_FALSE, EINA_TRUE);
             if (item->icon_type == ELM_ICON_FILE)
               elm_image_file_set(ic, item->icon_file, item->icon_group);
             else if (item->icon_type == ELM_ICON_STANDARD)
               elm_icon_standard_set(ic, item->icon_file);
             elm_object_part_content_set(bt, "icon", ic);
          }

        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_box_pack_end(bx, bt);
        eo_do(bt, eo_event_callback_add
          (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, _on_item_clicked, item));
        evas_object_show(bt);
        eo_do(bt,
              eo_event_callback_add(ELM_WIDGET_EVENT_FOCUSED, _item_focused_cb, item),
              eo_event_callback_add(ELM_WIDGET_EVENT_UNFOCUSED, _item_unfocused_cb, item));
     }

   _create_scroller(obj, sd);
   elm_object_content_set(sd->scr, bx);

   _resizing_eval(obj, sd);
   elm_object_part_content_set(sd->hover, sd->last_location, sd->tbl);

   eo_do(obj, eo_event_callback_call(ELM_HOVERSEL_EVENT_EXPANDED, NULL));
   evas_object_show(sd->hover);
}

static Eina_Bool
_on_clicked(void *data EINA_UNUSED,
            Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
            void *event_info EINA_UNUSED)
{
   _activate(data);

   return EINA_TRUE;
}

static void
_on_parent_del(void *data,
               Evas *e EINA_UNUSED,
               Evas_Object *obj EINA_UNUSED,
               void *event_info EINA_UNUSED)
{
   elm_hoversel_hover_parent_set(data, NULL);
}

static void
_elm_hoversel_item_elm_widget_item_part_text_set(Eo *eo_it EINA_UNUSED,
                                            Elm_Hoversel_Item_Data *it,
                                            const char *part,
                                            const char *label)
{
   if (part && strcmp(part, "default")) return;
   eina_stringshare_replace(&it->label, label);

   if (VIEW(it))
     elm_object_text_set(VIEW(it), label);
}

static const char *
_elm_hoversel_item_elm_widget_item_part_text_get(Eo *eo_it EINA_UNUSED,
                                            Elm_Hoversel_Item_Data *it,
                                            const char *part)
{
   if (part && strcmp(part, "default")) return NULL;
   return it->label;
}

EOLIAN static void
_elm_hoversel_item_elm_widget_item_signal_emit(Eo *eo_it EINA_UNUSED,
                                         Elm_Hoversel_Item_Data *it,
                                         const char *emission,
                                         const char *source)
{
   elm_object_signal_emit(VIEW(it), emission, source);
}

EOLIAN static void
_elm_hoversel_item_elm_widget_item_style_set(Eo *eo_it EINA_UNUSED,
                                             Elm_Hoversel_Item_Data *it,
                                             const char *style)
{
   elm_object_style_set(VIEW(it), style);
}

EOLIAN static const char *
_elm_hoversel_item_elm_widget_item_style_get(Eo *eo_it EINA_UNUSED,
                                             Elm_Hoversel_Item_Data *it)
{
   return elm_object_style_get(VIEW(it));
}

EOLIAN static void
_elm_hoversel_item_elm_widget_item_focus_set(Eo *eo_it EINA_UNUSED,
                                             Elm_Hoversel_Item_Data *it,
                                             Eina_Bool focused)
{
   elm_object_focus_set(VIEW(it), focused);
}

EOLIAN static Eina_Bool
_elm_hoversel_item_elm_widget_item_focus_get(Eo *eo_it EINA_UNUSED,
                                             Elm_Hoversel_Item_Data *it)
{
   return elm_widget_focus_get(VIEW(it));
}

EOLIAN static void
_elm_hoversel_item_eo_base_destructor(Eo *eo_item, Elm_Hoversel_Item_Data *item)
{
   ELM_HOVERSEL_DATA_GET_OR_RETURN(WIDGET(item), sd);

   elm_hoversel_hover_end(WIDGET(item));
   sd->items = eina_list_remove(sd->items, eo_item);
   eina_stringshare_del(item->label);
   eina_stringshare_del(item->icon_file);
   eina_stringshare_del(item->icon_group);

   eo_do_super(eo_item, ELM_HOVERSEL_ITEM_CLASS, eo_destructor());
}

static void
_on_move_resize(void * data,
           Evas *e EINA_UNUSED,
           Evas_Object *obj,
           void *event_info EINA_UNUSED)
{
   Elm_Hoversel_Data *sd = data;

   _resizing_eval(obj, sd);
}

EOLIAN static void
_elm_hoversel_evas_object_smart_add(Eo *obj, Elm_Hoversel_Data *priv)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());
   elm_widget_sub_object_parent_add(obj);

   eo_do(obj, eo_event_callback_add(
         EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, _on_clicked, obj));

   //What are you doing here?
   eo_do(obj, elm_obj_widget_theme_apply());

   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOVE, _on_move_resize, priv);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_RESIZE, _on_move_resize, priv);
}

EOLIAN static void
_elm_hoversel_evas_object_smart_del(Eo *obj, Elm_Hoversel_Data *sd)
{
   Elm_Object_Item *eo_item;

   EINA_LIST_FREE(sd->items, eo_item)
     {
        eo_del(eo_item);
     }
   elm_hoversel_hover_parent_set(obj, NULL);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

EOLIAN static void
_elm_hoversel_evas_object_smart_show(Eo *obj, Elm_Hoversel_Data *sd)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_show());
   evas_object_show(sd->hover);
}

EOLIAN static void
_elm_hoversel_evas_object_smart_hide(Eo *obj, Elm_Hoversel_Data *sd)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_hide());
   evas_object_hide(sd->hover);
}

EOLIAN static void
_elm_hoversel_elm_widget_parent_set(Eo *obj, Elm_Hoversel_Data *_pd EINA_UNUSED, Evas_Object *parent)
{
   elm_hoversel_hover_parent_set(obj, parent);
}

EOLIAN static Eina_Bool
_elm_hoversel_elm_button_admits_autorepeat_get(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd EINA_UNUSED)
{
   return EINA_FALSE;
}

EAPI Evas_Object *
elm_hoversel_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

EOLIAN static Eo *
_elm_hoversel_eo_base_constructor(Eo *obj, Elm_Hoversel_Data *_pd EINA_UNUSED)
{
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());
   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_GLASS_PANE));

   return obj;
}

EOLIAN static void
_elm_hoversel_eo_base_destructor(Eo *obj, Elm_Hoversel_Data *_pd EINA_UNUSED)
{
   eo_do(obj, elm_obj_hoversel_clear());
   eo_do_super(obj, MY_CLASS, eo_destructor());
}

EOLIAN static void
_elm_hoversel_hover_parent_set(Eo *obj, Elm_Hoversel_Data *sd, Evas_Object *parent)
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
_elm_hoversel_hover_parent_get(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd)
{
   return sd->hover_parent;
}

EOLIAN static void
_elm_hoversel_horizontal_set(Eo *obj, Elm_Hoversel_Data *sd, Eina_Bool horizontal)
{
   sd->horizontal = !!horizontal;

   if (sd->scr)
     {
        if (sd->horizontal)
          {
             elm_scroller_policy_set(sd->scr, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
             elm_scroller_content_min_limit(sd->scr, EINA_FALSE, EINA_TRUE);
             elm_scroller_bounce_set(sd->scr, EINA_TRUE, EINA_FALSE);
          }
        else
          {
             elm_scroller_policy_set(sd->scr, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
             elm_scroller_content_min_limit(sd->scr, EINA_TRUE, EINA_FALSE);
             elm_scroller_bounce_set(sd->scr, EINA_FALSE, EINA_TRUE);
          }
     }

   eo_do(obj, elm_obj_widget_theme_apply());
}

EOLIAN static Eina_Bool
_elm_hoversel_horizontal_get(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd)
{
   return sd->horizontal;
}

EOLIAN static void
_elm_hoversel_hover_begin(Eo *obj, Elm_Hoversel_Data *sd)
{
   if (sd->hover) return;

   _activate(obj);
}

EOLIAN static void
_elm_hoversel_hover_end(Eo *obj, Elm_Hoversel_Data *sd)
{

   Elm_Object_Item *eo_item;
   Eina_List *l;
   const char *dismissstr;

   if (!sd->hover) return;

   dismissstr = edje_object_data_get(elm_layout_edje_get(sd->hover), "dismiss");

   if (dismissstr && !strcmp(dismissstr, "on"))
     {
        elm_hover_dismiss(sd->hover);



     }
   else

     {
        sd->expanded = EINA_FALSE;

        EINA_LIST_FOREACH(sd->items, l, eo_item)
          {
             ELM_HOVERSEL_ITEM_DATA_GET(eo_item, it);
             VIEW(it) = NULL;
          }
        ELM_SAFE_FREE(sd->hover, evas_object_del);
        sd->scr = NULL;
        sd->last_location = NULL;

        eo_do(obj, eo_event_callback_call(ELM_HOVERSEL_EVENT_DISMISSED, NULL));
     } // for backward compatibility
}

EOLIAN static Eina_Bool
_elm_hoversel_expanded_get(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd)
{
   return (sd->hover) ? EINA_TRUE : EINA_FALSE;
}

EOLIAN static void
_elm_hoversel_clear(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd)
{
   Elm_Object_Item *it;
   Eina_List *l, *ll;

   EINA_LIST_FOREACH_SAFE(sd->items, l, ll, it)
     {
        eo_do(it, elm_wdg_item_del());
     }
}

EOLIAN static const Eina_List*
_elm_hoversel_items_get(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd)
{
   return sd->items;
}

EOLIAN static Eo *
_elm_hoversel_item_eo_base_constructor(Eo *obj, Elm_Hoversel_Item_Data *it)
{
   obj = eo_do_super_ret(obj, ELM_HOVERSEL_ITEM_CLASS, obj, eo_constructor());
   it->base = eo_data_scope_get(obj, ELM_WIDGET_ITEM_CLASS);

   return obj;
}

EOLIAN static Elm_Object_Item*
_elm_hoversel_item_add(Eo *obj, Elm_Hoversel_Data *sd, const char *label, const char *icon_file, Elm_Icon_Type icon_type, Evas_Smart_Cb func, const void *data)
{
   Eo *eo_item = eo_add(ELM_HOVERSEL_ITEM_CLASS, obj);
   if (!eo_item) return NULL;

   ELM_HOVERSEL_ITEM_DATA_GET(eo_item, item);

   item->label = eina_stringshare_add(label);
   item->icon_file = eina_stringshare_add(icon_file);
   item->icon_type = icon_type;
   item->func = func;
   WIDGET_ITEM_DATA_SET(eo_item, data);

   sd->items = eina_list_append(sd->items, eo_item);

   return eo_item;
}

EOLIAN static void
_elm_hoversel_item_icon_set(Eo *eo_item EINA_UNUSED,
                            Elm_Hoversel_Item_Data *item,
                            const char *icon_file,
                            const char *icon_group,
                            Elm_Icon_Type icon_type)
{
   eina_stringshare_replace(&item->icon_file, icon_file);
   eina_stringshare_replace(&item->icon_group, icon_group);

   item->icon_type = icon_type;
}

EOLIAN static void
_elm_hoversel_item_icon_get(Eo *eo_item EINA_UNUSED,
                            Elm_Hoversel_Item_Data *item,
                            const char **icon_file,
                            const char **icon_group,
                            Elm_Icon_Type *icon_type)
{
   if (icon_file) *icon_file = item->icon_file;
   if (icon_group) *icon_group = item->icon_group;
   if (icon_type) *icon_type = item->icon_type;
}

static Elm_Object_Item *
item_focused_get(Elm_Hoversel_Data *sd)
{
   Elm_Object_Item *eo_item;
   Eina_List *l;

   EINA_LIST_FOREACH(sd->items, l, eo_item)
     {
        ELM_HOVERSEL_ITEM_DATA_GET(eo_item, item);
        if (elm_object_focus_get(VIEW(item)))
          return eo_item;
     }
   return NULL;
}

static Eina_Bool
_key_action_move(Evas_Object *obj, const char *params)
{
   ELM_HOVERSEL_DATA_GET(obj, sd);
   const char *dir = params;

   Elm_Object_Item  *eo_litem, *eo_fitem;
   eo_litem = eina_list_last_data_get(sd->items);
   eo_fitem = eina_list_data_get(sd->items);

   _elm_widget_focus_auto_show(obj);
   if (!strcmp(dir, "down"))
     {
        if ((!sd->horizontal) &&
            (item_focused_get(sd) == eo_litem))
          {
            ELM_HOVERSEL_ITEM_DATA_GET(eo_fitem, fitem);
            elm_object_focus_set(VIEW(fitem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_DOWN);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "up"))
     {
        if ((!sd->horizontal) &&
            (item_focused_get(sd) == eo_fitem))
          {
            ELM_HOVERSEL_ITEM_DATA_GET(eo_litem, litem);
            elm_object_focus_set(VIEW(litem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_UP);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "left"))
     {
        if (sd->horizontal &&
            (item_focused_get(sd) == eo_fitem))
          {
            ELM_HOVERSEL_ITEM_DATA_GET(eo_litem, litem);
            elm_object_focus_set(VIEW(litem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_LEFT);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "right"))
     {
        if (sd->horizontal &&
            (item_focused_get(sd) == eo_litem))
          {
            ELM_HOVERSEL_ITEM_DATA_GET(eo_fitem, fitem);
            elm_object_focus_set(VIEW(fitem), EINA_TRUE);
            return EINA_TRUE;
          }
        elm_widget_focus_cycle(sd->hover, ELM_FOCUS_RIGHT);
        return EINA_TRUE;
     }
   else return EINA_FALSE;
}

static Eina_Bool
_key_action_activate(Evas_Object *obj, const char *params EINA_UNUSED)
{
   _activate(obj);
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_hoversel_elm_widget_event(Eo *obj, Elm_Hoversel_Data *sd, Evas_Object *src, Evas_Callback_Type type, void *event_info)
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
_elm_hoversel_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EOLIAN const Elm_Atspi_Action *
_elm_hoversel_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "activate", "activate", NULL, _key_action_activate},
          { "move,up", "move", "up", _key_action_move},
          { "move,down", "move", "down", _key_action_move},
          { "move,left", "move", "left", _key_action_move},
          { "move,right", "move", "right", _key_action_move},
          { NULL, NULL, NULL, NULL}
   };
   return &atspi_actions[0];
}

EOLIAN void
_elm_hoversel_auto_update_set(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd, Eina_Bool auto_update)
{
   sd->auto_update = !!auto_update;
}

EOLIAN Eina_Bool
_elm_hoversel_auto_update_get(Eo *obj EINA_UNUSED, Elm_Hoversel_Data *sd)
{
   return sd->auto_update;
}

#include "elm_hoversel_item.eo.c"
#include "elm_hoversel.eo.c"
