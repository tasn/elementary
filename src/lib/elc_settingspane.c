#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Elementary.h>

#include <elm_priv.h>

/**
 * This widget has a internal stack for displaying panels.
 *
 * - Each item is a Stack_Item
 *
 * - A new item on the stack will result from a call to item_focus
 *
 * - If just a entry in a menu is selected the current field of this item
 *   will be adjusted.
 *
 * - If the "back" arrived at the end of a item, the item under this
 *   will be displayed again
 *
 * - If there is no lower item on the stack the "back" buton should be disabled
 *
 */

typedef struct
{
   Elm_Settingspane_Item *zero;

   // list of all Elm_Item_Object
   Eina_List *history_stack;
   Eina_List *unsaved_changes;

   Elm_Genlist_Item_Class *itcc; // , *itci;

   struct
   {
      Eina_Bool on;
      Evas_Object *display;
      Ecore_Thread *run;
      const char* keyword;
   } search;
} Elm_Settingspane_Data;

typedef struct
{
   Elm_Settingspane_Item *par;

   Eina_List *childs; //list of Elm_Settingspane_Item*

   Eina_List *key_words; //< list of strdup´ed strings to match searching

   const char *name;
   const char *description;
   void *data;

   Evas_Object *sw;
   Eina_Bool changed;

   Elm_Widget_Item *it;
   Evas_Object *icon;

   struct
   {
      Evas_Object *tmp_content; //obejct of a hiden content will be held here.

      Evas_Smart_Cb apply;
      Evas_Smart_Cb reset;
      Content_Get_Cb content_get;
   } panel;
} Elm_Settingspane_Item_Data;

typedef struct _Search_Run
{
   Eina_List *results;
   Elm_Settingspane_Data *data;
   const char *keyword;
} Search_Run;

static void _unsaved_changes_add(Evas_Object *w, Elm_Object_Item *item);
static void _unsaved_changes_remove(Evas_Object *w, Elm_Object_Item *item);

static void _history_stack_push(Evas_Object *w, Elm_Object_Item *it);
static Elm_Object_Item* _history_stack_pop(Evas_Object *w);
static Elm_Object_Item* _history_stack_current(Evas_Object *w);
static Eina_Bool _history_stack_end(Evas_Object *w);
static void _history_stack_remove(Evas_Object *w, Elm_Object_Item *it);

static void _item_sel_cb(void *data, Evas_Object *obj, void *event_info);
static void _search_panel_stop(Evas_Object *w);

#define HAS_PANEL(id) (id->panel.content_get)
/* Data Keys in the layouts, DK = Data Key */

#define DK_MAIN_MENU_TABLE "__menu_table"
#define DK_MAIN_MENU_SHOWED "__menu_layout_show"
#define DK_MAIN_MENU_HIDDEN "__menu_layout_hidden"
#define DK_MAIN_MENU_BEHIND "__menu_layout_behind"

#define DK_MAIN_PANEL_TABLE "__panel_table"
#define DK_MAIN_PANEL_SHOWED "__panel_layout_show"
#define DK_MAIN_PANEL_HIDDEN "__panel_layout_hidden"

#define DK_MAIN_TITLE_BOX "__panel_title"

#define DK_MAIN_SEARCH_OBJECTS "__panel_search"
#define DK_PANEL_APPLY_BUTTON "__apply_btn"
#define DK_PANEL_RESET_BUTTON "__reset_btn"
#define DK_PANEL_ITEM_SHOWN "__showed_item"

#define DK_MENU_GENLIST "__menu_genlist"
#define DK_MENU_ITEM_SHOWN "__showed_item"

#define DK_SEARCH_LABEL "__search_label"
#define DK_SEARCH_GRID "__search_results"
#define DK_UNSAVED_OPENED "__unsaved_opened"

/* Layout positions */

#define POS_PANEL_PAD "elm.rect.pad"
#define POS_PANEL_CONTROLS "elm.swallow.controls"
#define POS_PANEL_CONTENT "elm.swallow.content"

#define POS_MENU_CONTENT "elm.swallow.content"

#define POS_MAIN_TITLE "elm.swallow.title"
#define POS_MAIN_PANEL "elm.swallow.panel"
#define POS_MAIN_MENU "elm.swallow.menu"
#define POS_MAIN_SEARCH "elm.swallow.search"

/* Signals */

#define EMITTER "elm"

#define EMIT_BACK_ENABLE "elm,state,back,enabled", EMITTER
#define EMIT_BACK_DISABLE "elm,state,back,disabled", EMITTER
#define EMIT_UNSAVED_ENABLE "elm,state,unsaved,enabled", EMITTER
#define EMIT_UNSAVED_DISABLE "elm,state,unsaved,disabled", EMITTER

#define EMIT_MENU_DEFAULT "elm,state,default", EMITTER
#define EMIT_MENU_VISIBLE "elm,state,visible", EMITTER
#define EMIT_MENU_VISIBLE_HIDDEN "elm,state,visible,hidden", EMITTER
#define EMIT_MENU_HIDDEN "elm,state,hidden", EMITTER
#define EMIT_MENU_SEARCHMODE_ENABLE "elm,searchmode,enable", EMITTER
#define EMIT_MENU_SEARCHMODE_DISABLE "elm,searchmode,disable", EMITTER

#define EMIT_CONTENT_DEFAULT "elm,state,default", EMITTER
#define EMIT_CONTENT_VISIBLE "elm,state,visible", EMITTER
#define EMIT_CONTENT_HIGHLIGHT "elm,state,highlight", EMITTER
#define EMIT_CONTENT_CONTROLS_SHOW "elm,state,controls,enable", EMITTER
#define EMIT_CONTENT_CONTROLS_HIDE "elm,state,controls,disable", EMITTER


#define IC_DATA(v) Elm_Settingspane_Item_Data *id = eo_data_scope_get(v, ELM_SETTINGSPANE_ITEM_CLASS)
#define IC_DATA_L(v) eo_data_scope_get(v, ELM_SETTINGSPANE_ITEM_CLASS)
#define C_DATA(v) Elm_Settingspane_Data *wd = eo_data_scope_get(v, ELM_SETTINGSPANE_CLASS)

static void
conf_menu_back_cb(void *data EINA_UNUSED, Evas_Object *obj, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
   /*
    *
    * FIXME: For menu cb, we need a new stack method here which pop to the next menu entry!!!
    *
    */
   C_DATA(obj);
   if (wd->search.on)
     _search_panel_stop(obj);
   else
     _history_stack_pop(obj);
}

/*
 * Titlebar stuff
 */
static void
_title_bar_refresh(Evas_Object *w)
{
   Elm_Object_Item *it, *walker;
   Elm_Settingspane_Item_Data *id = NULL;
   Evas_Object *bt, *bx;
   it = _history_stack_current(w);
   bx = evas_object_data_get(w, DK_MAIN_TITLE_BOX);

   elm_box_clear(bx);

   do {
        walker = id ? id->par : it;
        id = IC_DATA_L(walker);

        bt = elm_button_add(bx);
        elm_object_style_set(bt, "navlink");
        elm_object_text_set(bt, id->name);
        evas_object_show(bt);

        elm_box_pack_start(bx, bt);
   } while(id->par);
}

/*
 * Elm_Settingspane_Item event funktions
 */
static void
_item_content_reset(Elm_Settingspane_Item *it)
{
   IC_DATA(it);

   if (id->panel.reset)
     id->panel.reset(id->data, id->sw, it);
}

static void
_item_content_apply(Elm_Settingspane_Item *it)
{
   IC_DATA(it);

   if (id->panel.apply)
     id->panel.apply(id->data, id->sw, it);
}

static void
_item_content_del(Elm_Settingspane_Item *it EINA_UNUSED)
{
/*   Elm_Settingspane_Item_Data *id = eo_data_scope_get(it,
                                     ELM_SETTINGSPANE_ITEM_CLASS);
   if (id->class->content_del)
     id->class->content_del(id->data);*/
}

static Evas_Object*
_item_content_get(Elm_Settingspane_Item *it, Evas_Object *obj)
{
   IC_DATA(it);

   if (id->panel.content_get)
     return id->panel.content_get(obj, it, id->data);
   else
     return NULL;
}


/*
 * content_layout section
 */

static void
_content_layout_apply_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event EINA_UNUSED)
{
   Evas_Object *l = data;
   Elm_Settingspane_Item *displayed = evas_object_data_get(l, DK_PANEL_ITEM_SHOWN);

   if (!displayed) return;

   _item_content_apply(displayed);
}

static void
_content_layout_reset_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event EINA_UNUSED)
{
   Evas_Object *l = data;
   Elm_Settingspane_Item *displayed = evas_object_data_get(l, DK_PANEL_ITEM_SHOWN);

   if (!displayed) return;

   _item_content_reset(displayed);
}



static void
_content_layout_content_init(Evas_Object *w)
{
   Evas_Object *o, *pad, *table, *bx, *apply, *reset;
   int i = 0;

   table = evas_object_data_get(w, DK_MAIN_PANEL_TABLE);

   for (i = 0; i < 2; i++)
     {
        o = elm_layout_add(w);
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_layout_theme_set(o, "settingspane", "panel", "default");
        evas_object_show(o);

        pad = evas_object_rectangle_add(evas_object_evas_get(w));
        evas_object_size_hint_weight_set(pad, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(pad, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_object_part_content_set(o, POS_PANEL_PAD, pad);
        evas_object_show(pad);

        elm_table_pack(table, o, 0, 0, 1, 1);

        bx = elm_box_add(w);
        evas_object_size_hint_weight_set(bx, 0, 0);
        evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_box_horizontal_set(bx, EINA_TRUE);
        elm_object_part_content_set(o, POS_PANEL_CONTROLS, bx);

        apply = elm_button_add(w);
        evas_object_size_hint_weight_set(apply, 0.0, 0.0);
        evas_object_size_hint_align_set(apply, 0.0, 0.0);
        evas_object_smart_callback_add(apply, "clicked", _content_layout_apply_cb, o);
        elm_object_text_set(apply, "Apply");
        evas_object_data_set(o, DK_PANEL_APPLY_BUTTON, apply);
        elm_box_pack_end(bx, apply);

        reset = elm_button_add(w);
        evas_object_size_hint_weight_set(reset, 0, 0);
        evas_object_size_hint_align_set(reset, 0.0, 0.0);
        elm_object_text_set(reset, "Reset");
        evas_object_smart_callback_add(reset, "clicked", _content_layout_reset_cb, o);
        evas_object_data_set(o, DK_PANEL_RESET_BUTTON, reset);
        elm_box_pack_end(bx, reset);

        evas_object_show(bx);
        evas_object_show(reset);
        evas_object_show(apply);

        if (i < 1)
          evas_object_data_set(w, DK_MAIN_PANEL_SHOWED, o);
        else
          evas_object_data_set(w, DK_MAIN_PANEL_HIDDEN, o);
     }
}

static void
_content_del(Elm_Settingspane_Item *it)
{
   Evas_Object *shown_content;
   IC_DATA(it);

   shown_content = id->panel.tmp_content;
   id->panel.tmp_content = NULL;
   eo_unref(shown_content);
   evas_object_del(shown_content);
   if (it)
     _item_content_del(it);

}

static void
_content_layout_content_hide(Evas_Object *w, Eina_Bool keep)
{
   Evas_Object *shown = evas_object_data_get(w, DK_MAIN_PANEL_SHOWED);
   Elm_Object_Item *item = evas_object_data_get(shown, DK_PANEL_ITEM_SHOWN);

   evas_object_data_del(shown, DK_PANEL_ITEM_SHOWN);
   elm_layout_signal_emit(shown, EMIT_CONTENT_DEFAULT);

   if (!keep && item)
     _content_del(item);
   else
     {
        Evas_Object *cnt;

        cnt = elm_object_part_content_unset(shown, POS_PANEL_CONTENT);
        evas_object_hide(cnt);
     }
}

static void
_content_layout_content_controls_refresh(Evas_Object *w, Elm_Settingspane_Item *it)
{
   IC_DATA(it);

   Evas_Object *shown = evas_object_data_get(w, DK_MAIN_PANEL_SHOWED);
   Evas_Object *apply_btn = evas_object_data_get(shown, DK_PANEL_APPLY_BUTTON);
   Evas_Object *reset_btn = evas_object_data_get(shown, DK_PANEL_RESET_BUTTON);

   if (!id->changed)
     elm_layout_signal_emit(shown, EMIT_CONTENT_CONTROLS_HIDE);
   else
     elm_layout_signal_emit(shown, EMIT_CONTENT_CONTROLS_SHOW);

   elm_object_disabled_set(apply_btn, !id->changed);
   elm_object_disabled_set(reset_btn, !id->changed);
}

static void
_content_layout_display(Evas_Object *w, Evas_Object *content)
{
   Evas_Object *shown = evas_object_data_get(w, DK_MAIN_PANEL_SHOWED);
   Evas_Object *hidden = evas_object_data_get(w, DK_MAIN_PANEL_HIDDEN);

   elm_layout_content_unset(hidden, POS_PANEL_CONTENT);
   elm_object_part_content_set(hidden, POS_PANEL_CONTENT, content);

   elm_layout_signal_emit(hidden, EMIT_CONTENT_VISIBLE);

   evas_object_data_set(w, DK_MAIN_PANEL_SHOWED, hidden);
   evas_object_raise(hidden);
   evas_object_data_set(w, DK_MAIN_PANEL_HIDDEN, shown);
}

static void
_content_layout_content_set(Evas_Object *w, Elm_Settingspane_Item *it)
{
   IC_DATA(it);

   Evas_Object *hidden = evas_object_data_get(w, DK_MAIN_PANEL_HIDDEN);

   if (!id->panel.tmp_content)
     {
        id->panel.tmp_content = elm_scroller_add(w);
        eo_ref(id->panel.tmp_content);
        evas_object_size_hint_weight_set(id->panel.tmp_content,
                            EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_show(id->panel.tmp_content);
        elm_object_content_set(id->panel.tmp_content, _item_content_get(it, w));

     }

   _content_layout_display(w, id->panel.tmp_content);
   evas_object_data_set(hidden, DK_PANEL_ITEM_SHOWN, it);

   _content_layout_content_controls_refresh(w, it);
}

/*
 * menu_layout section
 */
static void
_menu_layout_init(Evas_Object *w)
{
   Evas_Object *o, *table, *list;
   int i = 0;

   table = evas_object_data_get(w, DK_MAIN_MENU_TABLE);

   for (i = 0; i < 3; i++)
     {
        o = elm_layout_add(w);
        evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_layout_theme_set(o, "settingspane", "menu", "default");
//        elm_layout_signal_callback_add(w, "action,back", "conf", conf_menu_back_cb, w);
        evas_object_show(o);

        list = elm_genlist_add(w);
        elm_scroller_bounce_set(list, EINA_FALSE, EINA_FALSE);
        elm_object_style_set(list, "flatedge");
        evas_object_size_hint_weight_set(list, 0.0, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_genlist_mode_set(list, ELM_LIST_LIMIT);
        evas_object_data_set(o, DK_MENU_GENLIST, list);
        elm_object_part_content_set(o, POS_MENU_CONTENT, list);
        evas_object_show(list);
        elm_table_pack(table, o, 0, 0, 1, 1);

        if (i == 0)
          evas_object_data_set(w, DK_MAIN_MENU_HIDDEN, o);
        else if (i == 1)
          evas_object_data_set(w, DK_MAIN_MENU_SHOWED, o);
        else if (i == 2)
          evas_object_data_set(w, DK_MAIN_MENU_BEHIND, o);
     }
}

static void
_list_fill(Evas_Object *list, Elm_Settingspane_Item *item)
{
   IC_DATA(item);
   C_DATA(id->sw);

   Eina_List *node;
   Elm_Settingspane_Item *child;

   elm_genlist_clear(list);

   EINA_LIST_FOREACH(id->childs, node, child)
     {
        Elm_Settingspane_Item_Data *cid = IC_DATA_L(child);
        Elm_Object_Item *it;

        it = elm_genlist_item_append(list, wd->itcc, child,
                                NULL, ELM_GENLIST_ITEM_NONE,
                                _item_sel_cb, child);
        elm_genlist_item_select_mode_set(it, ELM_OBJECT_SELECT_MODE_ALWAYS);
         elm_genlist_item_tooltip_text_set(it, cid->description);
     }
}

static void
_menu_layout_show(Evas_Object *w, Elm_Settingspane_Item *item, Elm_Settingspane_Item *item_h)
{
   Evas_Object *shown = evas_object_data_get(w, DK_MAIN_MENU_SHOWED);
   Evas_Object *hidden = evas_object_data_get(w, DK_MAIN_MENU_HIDDEN);
   Evas_Object *behind = evas_object_data_get(w, DK_MAIN_MENU_BEHIND);
   Evas_Object *shownlist = evas_object_data_get(shown, DK_MENU_GENLIST);
   Evas_Object *behindlist = evas_object_data_get(behind, DK_MENU_GENLIST);
   Elm_Settingspane_Item *shown_h, *shown_l;

   shown_h = evas_object_data_get(hidden, DK_MENU_ITEM_SHOWN);
   shown_l = evas_object_data_get(shown, DK_MENU_ITEM_SHOWN);

   if (shown_l == item && shown_h == item_h)
     return;

   _list_fill(behindlist, item);
   evas_object_data_set(behind, DK_MENU_ITEM_SHOWN, item);

   //showed item has not changed
   if (!item_h)
     {
        elm_layout_signal_emit(shown, EMIT_MENU_DEFAULT);
        elm_layout_signal_emit(behind, EMIT_MENU_VISIBLE);
        evas_object_data_del(shown, DK_MENU_ITEM_SHOWN);
      }
   else
     {
        _list_fill(shownlist, item_h);
        elm_layout_signal_emit(shown, EMIT_MENU_HIDDEN);
        elm_layout_signal_emit(behind, EMIT_MENU_VISIBLE_HIDDEN);
        evas_object_data_set(shown, DK_MENU_ITEM_SHOWN, item_h);
     }
   evas_object_raise(shown);

   evas_object_stack_below(hidden, behind);
   elm_layout_signal_emit(hidden, EMIT_MENU_DEFAULT);
   evas_object_data_del(hidden, DK_MENU_ITEM_SHOWN);

   //reorder the use
   evas_object_data_set(w, DK_MAIN_MENU_SHOWED, behind);
   evas_object_data_set(w, DK_MAIN_MENU_HIDDEN, shown);
   evas_object_data_set(w, DK_MAIN_MENU_BEHIND, hidden);
}
static void
_menu_layout_refresh(Evas_Object *w)
{
   Evas_Object *layouts[3];
   int i = 0;

   layouts[0] = evas_object_data_get(w, DK_MAIN_MENU_SHOWED);
   layouts[1] = evas_object_data_get(w, DK_MAIN_MENU_HIDDEN);
   layouts[2] = evas_object_data_get(w, DK_MAIN_MENU_BEHIND);

   for (i = 0; i< 3; i++)
     {
        Elm_Settingspane_Item *it;
        Evas_Object *list;

        it = evas_object_data_get(layouts[i], DK_MENU_ITEM_SHOWN);
        if (!it)
          continue;

        list = evas_object_data_get(layouts[i], DK_MENU_GENLIST);

        _list_fill(list, it);
     }
}

static void
_menu_layout_fill(Elm_Settingspane_Item *it)
{
   IC_DATA(it);
   if (id->par)
     _menu_layout_show(id->sw, it, id->par);
   else
     _menu_layout_show(id->sw, it, NULL);
}

static void
_menu_layout_hide(Evas_Object *w)
{
   Evas_Object *shown = evas_object_data_get(w, DK_MAIN_MENU_SHOWED);
   Evas_Object *hidden = evas_object_data_get(w, DK_MAIN_MENU_HIDDEN);
   Evas_Object *behind = evas_object_data_get(w, DK_MAIN_MENU_BEHIND);

   elm_layout_signal_emit(hidden, EMIT_MENU_DEFAULT);
   evas_object_data_del(hidden, DK_MENU_ITEM_SHOWN);
   elm_layout_signal_emit(shown, EMIT_MENU_DEFAULT);
   evas_object_data_del(shown, DK_MENU_ITEM_SHOWN);
   elm_layout_signal_emit(behind, EMIT_MENU_DEFAULT);
   evas_object_data_del(behind, DK_MENU_ITEM_SHOWN);

   elm_layout_signal_emit(w, EMIT_MENU_SEARCHMODE_ENABLE);
}
static void
_menu_layout_restore(Evas_Object *w)
{
   elm_layout_signal_emit(w, EMIT_MENU_SEARCHMODE_DISABLE);
}

static void
_menu_layout_hidden_to_show(Evas_Object *w, Elm_Settingspane_Item *new, Elm_Settingspane_Item *lower_new)
{
   Evas_Object *shown = evas_object_data_get(w, DK_MAIN_MENU_SHOWED);
   Evas_Object *hidden = evas_object_data_get(w, DK_MAIN_MENU_HIDDEN);
   Evas_Object *behind = evas_object_data_get(w, DK_MAIN_MENU_BEHIND);
   Evas_Object *behindlist = evas_object_data_get(behind, DK_MENU_GENLIST);
   Evas_Object *hiddenlist = evas_object_data_get(hidden, DK_MENU_GENLIST);

   Elm_Settingspane_Item *item_shown = evas_object_data_get(shown, DK_MENU_ITEM_SHOWN);
   Elm_Settingspane_Item *item_hidden = evas_object_data_get(hidden, DK_MENU_ITEM_SHOWN);

   if ((item_hidden == lower_new) && (item_shown == new))
     return;


   if (lower_new)
     {
        elm_layout_signal_emit(behind, EMIT_MENU_DEFAULT);
        _list_fill(behindlist, lower_new);
        evas_object_raise(behind);
        elm_layout_signal_emit(behind, EMIT_MENU_HIDDEN);
     }

   //I guess in the hidden one the items should be the correct one
   _list_fill(hiddenlist, new);
   elm_layout_signal_emit(hidden, EMIT_MENU_VISIBLE);
   evas_object_stack_below(shown, hidden);

   elm_layout_signal_emit(shown, EMIT_MENU_DEFAULT);

   evas_object_data_set(w, DK_MAIN_MENU_SHOWED, hidden);
   evas_object_data_set(w, DK_MAIN_MENU_HIDDEN, behind);
   evas_object_data_set(w, DK_MAIN_MENU_BEHIND, shown);
}

/*
 * Callbacks for the items in the gengrid
 */

static char*
_elm_settingspane_ii_text_cb(void *data, Evas_Object *obj EINA_UNUSED, const char *part)
{
   Elm_Settingspane_Item *it = data;
   IC_DATA(it);

   if (!strcmp(part, "elm.text"))
     return strdup(id->name);
   else if (!strcmp(part, "elm.text.sub"))
     return strdup(id->description);
   return NULL;
}

static Evas_Object*
_elm_settingspane_ii_content_cb(void *data, Evas_Object *obj, const char *part)
{
   Elm_Settingspane_Item *it = data;
   IC_DATA(it);
   Evas_Object *res = NULL;

   if (!strcmp(part, "elm.swallow.icon"))
     {
          res = id->icon;
     }
   else
     {
        res = elm_icon_add(obj);
        elm_icon_standard_set(res, "arrow_right");
        elm_image_no_scale_set(res, EINA_TRUE);
     }
   return res;
}

static Elm_Settingspane_Item*
_item_new(Evas_Object *obj, Elm_Settingspane_Item *par, void *usr_data, const char *name, const char *description, Evas_Object *icon)
{
   Elm_Settingspane_Item *item = eo_add(ELM_SETTINGSPANE_ITEM_CLASS, obj);
   Elm_Settingspane_Item_Data *data = eo_data_scope_get(item, ELM_SETTINGSPANE_ITEM_CLASS);

   data->data = usr_data;
   data->sw = obj;
   data->par = par;
   data->name = name;
   data->description = description;
   data->icon = icon;
   data->key_words = NULL;
   data->key_words = eina_list_append(data->key_words, eina_stringshare_add(name));
   //we need to ref this item, so elm_genlist_clear will not delete it!
   eo_ref(icon);
   return item;
}

static Eina_Bool
_item_has_panel(Elm_Settingspane_Item *item)
{
   IC_DATA(item);
   return HAS_PANEL(id) ? EINA_TRUE : EINA_FALSE;
}

/*static void
_item_stack_dump(Elm_Settingspane_Data *wd)
{
   Eina_List *node;
   Elm_Settingspane_Item *it;

   EINA_LIST_FOREACH(wd->stack, node, it)
   {
      Elm_Settingspane_Item_Data *id = eo_data_scope_get(it, ELM_SETTINGSPANE_ITEM_CLASS);
      ERR("item %p:%s", id, id->class->style);
   }
}*/

static void
_back_check(Evas_Object *w)
{
   if (_history_stack_end(w))
     elm_layout_signal_emit(w, EMIT_BACK_DISABLE);
   else
     elm_layout_signal_emit(w, EMIT_BACK_ENABLE);
}

static void
_unsaved_check(Evas_Object *w)
{
   C_DATA(w);
   if (eina_list_count(wd->unsaved_changes) == 0)
     elm_layout_signal_emit(w, EMIT_UNSAVED_DISABLE);
   else
     elm_layout_signal_emit(w, EMIT_UNSAVED_ENABLE);
}

static void
_unsaved_changes_add(Evas_Object *w, Elm_Object_Item *item)
{
   Eina_List *node;
   Elm_Object_Item *it;
   C_DATA(w);
   EINA_LIST_FOREACH(wd->unsaved_changes, node, it)
     {
        if (it == item)
          return;
     }
   wd->unsaved_changes = eina_list_append(wd->unsaved_changes, item);
   _unsaved_check(w);
}

static void
_unsaved_changes_remove(Evas_Object *w, Elm_Object_Item *item)
{
  C_DATA(w);
  wd->unsaved_changes = eina_list_remove(wd->unsaved_changes, item);
  _unsaved_check(w);
}

static void
_history_stack_push(Evas_Object *w, Elm_Object_Item *it)
{
   C_DATA(w);
   IC_DATA(it);
   Elm_Object_Item *old;
   Elm_Settingspane_Item_Data *ido;

   /* we just want one item to be on the stack */
   _history_stack_remove(w, it);

   old = _history_stack_current(w);
   ido = IC_DATA_L(old);
   wd->history_stack = eina_list_append(wd->history_stack, it);

   if (old && HAS_PANEL(ido))
     {
        _content_layout_content_hide(w, ido->changed);
     }

   if (HAS_PANEL(id))
     {
        _content_layout_content_set(id->sw, it);
     }
   else
     _menu_layout_show(id->sw, it, old);

   /* check if back is still valid */
   _back_check(w);
   /* refresh tiltlebar */
   _title_bar_refresh(w);
}

static Elm_Object_Item*
_history_stack_pop(Evas_Object *w)
{
   C_DATA(w);
   Elm_Object_Item *current, *poped;
   Elm_Settingspane_Item_Data *poped_id;
   Eina_Bool keep = EINA_FALSE;

   if (_history_stack_end(w))
     return NULL;

   poped = _history_stack_current(w);
   poped_id = IC_DATA_L(poped);

   /* check for unsaved changes */
   if (poped_id->changed)
     keep = EINA_TRUE;
   /* pop the item */
   wd->history_stack = eina_list_remove(wd->history_stack, poped);
   /* new top item */
   current = _history_stack_current(w);

   /* make animations and display the new panels */
   if (_item_has_panel(current) && _item_has_panel(poped))
     {
        IC_DATA(current);
        _menu_layout_fill(id->par);
        _content_layout_content_hide(w, keep);
        _content_layout_content_set(w, current);
     }
   else if (_item_has_panel(current) && !_item_has_panel(poped))
     {
        IC_DATA(current);
        _menu_layout_fill(id->par);
        _content_layout_content_hide(w, keep);
        _content_layout_content_set(w, current);
     }
   else if (!_item_has_panel(current) && _item_has_panel(poped))
     {
        _content_layout_content_hide(w, keep);
        _menu_layout_fill(current);
     }
   else
     {
        IC_DATA(current);
        _menu_layout_hidden_to_show(w, current, id->par);
     }

   /* check if back is still valid */
   _back_check(w);
   /* refresh tiltlebar */
   _title_bar_refresh(w);
   return poped;
}

static Elm_Object_Item*
_history_stack_current(Evas_Object *w)
{
   C_DATA(w);
   return eina_list_data_get(eina_list_last(wd->history_stack));
}

static Eina_Bool
_history_stack_end(Evas_Object *w)
{
   C_DATA(w);
   return eina_list_count(wd->history_stack) == 1;
}

static void
_history_stack_remove(Evas_Object *w, Elm_Object_Item *rm_it)
{
   C_DATA(w);
   Eina_List *node, *node2;
   Elm_Object_Item *it;
   /* we need to iterate throuw it, eina_list_remove removes it just once */
   EINA_LIST_FOREACH_SAFE(wd->history_stack, node, node2, it)
     {
        if (it == rm_it)
          wd->history_stack = eina_list_remove(wd->history_stack, it);
     }
}

static void
_item_append(Elm_Settingspane_Item *root, Elm_Settingspane_Item *toadd, Elm_Settingspane_Item *rel)
{
   IC_DATA(root);
   id->childs = eina_list_append_relative(id->childs, toadd, rel);

   if (_history_stack_current(id->sw) == root)
     _menu_layout_refresh(id->sw);
}

static void
_item_del(Elm_Settingspane_Item *del, Eina_Bool full)
{
   IC_DATA(del);
   Elm_Settingspane_Item_Data *id_par = NULL;
   Eina_List *node, *nnode;
   Elm_Settingspane_Item *ci;

   if (id->changed)
     {
        ERR("This page has unsaved changes, you have lost all the data!");
     }
   if (!full)
     {
        Elm_Settingspane_Item *top = _history_stack_current(id->sw);
        if (top == del)
          {
             _history_stack_pop(id->sw);
          }
     }
   EINA_LIST_FOREACH_SAFE(id->childs, node, nnode, ci)
     {
        _item_del(ci, full);
     }

   if (id->par)
     {
        id_par = IC_DATA_L(id->par);
        id_par->childs = eina_list_remove(id_par->childs, del);
     }
   if (!full)
     _menu_layout_refresh(id->sw);
   //if we have the item somewhere in the stack, remove it!
   _history_stack_remove(id->sw, del);
   //unref the content, if there is one
   eo_del(del);
}

static void
_item_sel_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Elm_Settingspane_Item *curr, *selected = data;
   IC_DATA(selected);

   curr = _history_stack_current(id->sw);

   if (curr == selected)
     return;

   eo_do(data, elm_obj_settingspane_item_focus());
}
/*
 * Seachbar stuff
 */

static void
_search_panel_display(Evas_Object *w)
{
   Evas_Object *o, *l, *g;
   C_DATA(w);

   o = elm_box_add(w);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   wd->search.display = o;
   evas_object_show(o);

   l = elm_label_add(w);
   evas_object_size_hint_weight_set(l, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(l, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_data_set(o, DK_SEARCH_LABEL, l);
   elm_box_pack_end(o, l);
   evas_object_show(l);

   g = elm_gengrid_add(w);
   elm_gengrid_item_size_set(g, 150, 150);
   evas_object_size_hint_weight_set(g, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(g, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_data_set(o, DK_SEARCH_GRID, g);
   elm_box_pack_end(o, g);
   evas_object_show(g);

   evas_object_data_set(w, DK_MAIN_SEARCH_OBJECTS, o);
   _content_layout_content_hide(w, EINA_TRUE);
   _content_layout_display(w, o);
   _menu_layout_hide(w);
}

static void
_search_panel_hide(Evas_Object *w)
{
   Elm_Settingspane_Item *item;
   Elm_Settingspane_Item_Data *id;
   Evas_Object *o;

   item = _history_stack_current(w);
   id = IC_DATA_L(item);

   o = evas_object_data_get(w, DK_MAIN_SEARCH_OBJECTS);
   if (!o)
     return;

   if (_item_has_panel(item))
     {
        _content_layout_content_hide(w, EINA_TRUE);
        _content_layout_content_set(w, item);
        if (id->par)
          {
              _menu_layout_restore(w);
              _menu_layout_fill(id->par);
          }
     }
   else
     {
        _content_layout_content_hide(w, EINA_TRUE);
        _menu_layout_restore(w);
        _menu_layout_fill(item);
     }

   evas_object_del(o);
   evas_object_data_del(w, DK_MAIN_SEARCH_OBJECTS);
}

static void
_search_panel_stop(Evas_Object *w)
{
   C_DATA(w);
   Evas_Object *search;

   search = elm_object_part_content_get(w, POS_MAIN_SEARCH);
   _search_panel_hide(w);
   elm_object_text_set(search, "");
   wd->search.on = EINA_FALSE;
   ecore_thread_cancel(wd->search.run);
}

static char*
_search_grid_label_get(void *data, Evas_Object *obj EINA_UNUSED, const char *part EINA_UNUSED)
{
   IC_DATA(data);

   return strdup(id->name);
}


static Evas_Object*
_search_grid_content_get(void *data, Evas_Object *obj EINA_UNUSED, const char *part EINA_UNUSED)
{
   IC_DATA(data);

   if (!strcmp(part, "elm.swallow.icon"))
     return id->icon;
   else
     return NULL;
}

static void
_search_grid_item_sel(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   IC_DATA(data);

   _search_panel_stop(id->sw);
   eo_do(data, elm_obj_settingspane_item_focus());
}

static void
_search_grid_del(void *data, Evas_Object *obj EINA_UNUSED)
{
   IC_DATA(data);

   /* Also called when the widget is deleted, but then the item is allready freed.
    * This would result in a crash.
    */
   if (!id) return;
   evas_object_hide(id->icon);
}

static void *
_search_display_items(void *data)
{
   Search_Run *sr = data;
   Eina_List *node;
   Elm_Settingspane_Item *it;
   Elm_Gengrid_Item_Class *gic = NULL;
   Evas_Object *grid = evas_object_data_get(sr->data->search.display,
                                            DK_SEARCH_GRID);

   elm_gengrid_clear(grid);

   gic = elm_gengrid_item_class_new();
   gic->item_style = "thumb";
   gic->func.text_get = _search_grid_label_get;
   gic->func.content_get = _search_grid_content_get;
   gic->func.del = _search_grid_del;

   EINA_LIST_FOREACH(sr->results, node, it)
     {
        elm_gengrid_item_append(grid, gic, it, _search_grid_item_sel, it);
     }

   return NULL;
}

/*
 * !!!This code runs in a seperated thread!!!
 */


static Eina_Bool
_item_found(Elm_Settingspane_Item_Data *id, const char* keyword)
{
   Eina_List *node;
   Eina_Stringshare *val;
   int l = strlen(keyword);

   EINA_LIST_FOREACH(id->key_words, node, val)
     {
        if (!strncasecmp(val, keyword, l))
          {
             return EINA_TRUE;
          }
     }
   return EINA_FALSE;
}

static void
_search_tree(Elm_Settingspane_Item *it, Eina_List **results, const char *keyword)
{
   IC_DATA(it);
   Eina_List *node;
   Elm_Settingspane_Item *item;

   if (_item_found(id, keyword))
     {
        *results = eina_list_append(*results, it);
     }

   EINA_LIST_FOREACH(id->childs, node ,item)
     _search_tree(item, results, keyword);
}

static void
_search_cb(void *data, Ecore_Thread *th)
{
   Search_Run *sr = data;

   _search_tree(sr->data->zero, &(sr->results), sr->keyword);

   if (ecore_thread_check(th))
     return;

   ecore_main_loop_thread_safe_call_sync(_search_display_items, sr);
}
/*
 * END OF !!!This code runs in a seperated thread!!!
 */

static void
_search_cb_ended(void *data, Ecore_Thread *th EINA_UNUSED)
{
   Search_Run *sr = data;

   sr->data->search.run = NULL;
   free(sr);
}


static void
_search_panel_search_string_update(Evas_Object *w, const char *word)
{
   char buf[PATH_MAX];
   Search_Run *sr;
   C_DATA(w);
   Evas_Object *l = evas_object_data_get(wd->search.display, DK_SEARCH_LABEL);

   sr = calloc(1, sizeof(Search_Run));
   sr->keyword = word;
   sr->data = wd;

   snprintf(buf, sizeof(buf), "Search results for \"%s\": ", word);
   elm_object_text_set(l, buf);

   if (wd->search.run)
     ecore_thread_cancel(wd->search.run);
   wd->search.run = ecore_thread_run(_search_cb, _search_cb_ended, NULL, sr);
}

static void
_search_entry_changed(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Object *w = data;
   C_DATA(data);
   const char *edit;

   edit = elm_object_text_get(obj);

   if (!strcmp(edit, ""))
     {
        if (!wd->search.on)
          return;
       _search_panel_stop(w);
       return;
     }

   if (!wd->search.on)
     {
        wd->search.on = EINA_TRUE;
        _search_panel_display(w);
     }
   _search_panel_search_string_update(w, edit);
}

static void
_conf_unsaved_item_click_cb(void *data, Evas_Object *obj, void *event EINA_UNUSED)
{
   Elm_Object_Item *it = data;
   IC_DATA(it);

   evas_object_data_del(id->sw, DK_UNSAVED_OPENED);

   eo_do(it, elm_obj_settingspane_item_focus());
   evas_object_hide(obj);
}

static void
_conf_unsaved_menu_ctx_item_add(Evas_Object *ctx, Elm_Object_Item *it)
{
   IC_DATA(it);
   char buf[PATH_MAX];
   const char *group, *file;
   Evas_Object *ic;

   snprintf(buf, sizeof(buf), "*%s", id->name);

   ic = elm_icon_add(ctx);
   elm_image_file_get(id->icon, &file, &group);
   elm_image_file_set(ic, file, group);
   evas_object_show(ic);

   elm_ctxpopup_item_append(ctx, buf, ic, _conf_unsaved_item_click_cb, it);
}

static void
_conf_unsaved_dismissed_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event EINA_UNUSED)
{
   Evas_Object *par = data;
   evas_object_data_del(par, DK_UNSAVED_OPENED);
}

static void
_conf_unsaved_menu_show(Evas_Object *par, int x, int y, char *style )
{
   Elm_Object_Item *it;
   Eina_List *node;
   Evas_Object *ctx;
   C_DATA(par);

   if (eina_list_count(wd->unsaved_changes) == 0 || evas_object_data_get(par, DK_UNSAVED_OPENED))
     return;

   evas_object_data_set(par, DK_UNSAVED_OPENED, (void*)1);

   ctx = elm_ctxpopup_add(par);
   elm_object_style_set(ctx, style);
   evas_object_smart_callback_add(ctx, "dismissed", _conf_unsaved_dismissed_cb, par);

   elm_ctxpopup_direction_priority_set(ctx, ELM_CTXPOPUP_DIRECTION_DOWN,
                                             ELM_CTXPOPUP_DIRECTION_LEFT,
                                             ELM_CTXPOPUP_DIRECTION_RIGHT,
                                             ELM_CTXPOPUP_DIRECTION_UP);
   EINA_LIST_FOREACH(wd->unsaved_changes, node, it)
     {
        _conf_unsaved_menu_ctx_item_add(ctx, it);
     }
   evas_object_move(ctx, x, y);
   evas_object_show(ctx);
}

static void
_conf_unsaved_menu_cb(void *data EINA_UNUSED, Evas_Object *obj, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
   int x, y;

   evas_pointer_canvas_xy_get(evas_object_evas_get(obj), &x, &y);
   _conf_unsaved_menu_show(obj, x, y, "default");
}

static void
_history_menu_item_sel_cb(void *data, Evas_Object *obj, void *event EINA_UNUSED)
{
   Elm_Object_Item *it = data;
   eo_do(it, elm_obj_settingspane_item_focus());
   evas_object_hide(obj);
}

static void
_history_menu_show(Evas_Object *par, int x, int y)
{
   Evas_Object *ctx;
   C_DATA(par);
   Eina_List *node;
   Elm_Widget_Item *it;

   ctx = elm_ctxpopup_add(par);
   elm_ctxpopup_direction_priority_set(ctx, ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_LEFT,
                                            ELM_CTXPOPUP_DIRECTION_RIGHT,
                                            ELM_CTXPOPUP_DIRECTION_UP);

   EINA_LIST_REVERSE_FOREACH(wd->history_stack, node, it)
     {
        if (it == wd->zero) continue;
        IC_DATA(it);
        elm_ctxpopup_item_append(ctx, id->name, NULL, _history_menu_item_sel_cb, it); //< FIXME
     }

   evas_object_move(ctx, x, y);
   evas_object_show(ctx);
}

static void
_history_menu_show_cb(void *data EINA_UNUSED, Evas_Object *obj, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
   int x, y;

   evas_pointer_canvas_xy_get(evas_object_evas_get(obj), &x, &y);
   _history_menu_show(obj, x, y);
}

EOLIAN static void
_elm_settingspane_evas_object_smart_add(Eo *obj, Elm_Settingspane_Data *pd)
{
   Evas_Object *bx, *en, *tb1, *tb2;

   eo_do_super(obj, ELM_SETTINGSPANE_CLASS, evas_obj_smart_add());
   elm_widget_sub_object_parent_add(obj);
   elm_widget_can_focus_set(obj, EINA_TRUE);

   if (!elm_layout_theme_set(obj, "settingspane", "base", "default"))
     {
        CRI("Failed to set theme file");
     }
   elm_layout_signal_callback_add(obj, "elm,action,back", EMITTER, conf_menu_back_cb, obj);
   elm_layout_signal_callback_add(obj, "elm,unsaved,clicked", EMITTER, _conf_unsaved_menu_cb, obj);
   elm_layout_signal_callback_add(obj, "elm,action,back_menu", EMITTER, _history_menu_show_cb, obj);
   elm_layout_signal_emit(obj, EMIT_BACK_DISABLE);

   bx = elm_box_add(obj);
   elm_box_align_set(bx, 0.0, 0.5);
   elm_box_horizontal_set(bx, EINA_TRUE);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(obj, POS_MAIN_TITLE, bx);
   evas_object_data_set(obj, DK_MAIN_TITLE_BOX, bx);
   evas_object_show(bx);

   en = elm_entry_add(obj);
   elm_entry_single_line_set(en, EINA_TRUE);
   elm_entry_scrollable_set(en, EINA_TRUE);
   evas_object_smart_callback_add(en, "changed", _search_entry_changed, obj);
   elm_object_part_text_set(en, "guide", "Search");
   elm_object_part_content_set(obj, POS_MAIN_SEARCH, en);
   evas_object_show(en);

   tb1 = elm_table_add(obj);
   evas_object_size_hint_weight_set(tb1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb1, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(obj, POS_MAIN_PANEL, tb1);
   evas_object_show(tb1);
   evas_object_data_set(obj, DK_MAIN_PANEL_TABLE, tb1);

   tb2 = elm_table_add(obj);
   evas_object_size_hint_weight_set(tb2, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb2, 0.0, EVAS_HINT_FILL);
   elm_object_part_content_set(obj, POS_MAIN_MENU, tb2);
   evas_object_show(tb2);
   evas_object_data_set(obj, DK_MAIN_MENU_TABLE, tb2);


   _content_layout_content_init(obj);
   _menu_layout_init(obj);

   pd->zero = _item_new(obj, NULL, NULL, eina_stringshare_add("root"), eina_stringshare_add(""), NULL);
   _history_stack_push(obj, pd->zero);
   _title_bar_refresh(obj);
 }

EOLIAN static void
_elm_settingspane_eo_base_constructor(Eo *obj, Elm_Settingspane_Data *pd)
{
   eo_do_super(obj, ELM_SETTINGSPANE_CLASS, eo_constructor());

   pd->itcc = elm_genlist_item_class_new();
   pd->itcc->item_style = "double_label_unblock";
   pd->itcc->func.text_get = _elm_settingspane_ii_text_cb;
   pd->itcc->func.content_get = _elm_settingspane_ii_content_cb;
}

static Elm_Settingspane_Item *
_elm_settingspane_item_append_full(Eo *obj, Elm_Settingspane_Data *pd, void *data,
                                   const char *name, const char *description,
                                   Evas_Object *icon, Elm_Settingspane_Item *par,
                                   Elm_Settingspane_Item *rel)
{
   Elm_Settingspane_Item *new_child = NULL;
   Elm_Settingspane_Item_Data *data_par;

   /*
    * Append to the zero level
    */
   if (!par)
     {
        new_child = _item_new(obj, pd->zero, data, name, description, icon);
        _item_append(pd->zero, new_child, rel);
        return new_child;
     }

   data_par = IC_DATA_L(par);
   if (!data_par)
     return NULL;
   if (HAS_PANEL(data_par))
     {
        ERR("You cannot add a menu item to a item with a panel!");
        return NULL;
     }
   else
     {
        new_child = _item_new(obj, par, data, name, description, icon);
        _item_append(par, new_child, rel);
     }
   return new_child;
}

EOLIAN static Elm_Settingspane_Item *
_elm_settingspane_item_append(Eo *obj, Elm_Settingspane_Data *pd, void *data, const char *name, const char *description, Evas_Object *icon, Elm_Settingspane_Item *par)
{
   return _elm_settingspane_item_append_full(obj, pd, data, name, description, icon, par, NULL);
}

EOLIAN static Elm_Settingspane_Item *
_elm_settingspane_item_append_relative(Eo *obj, Elm_Settingspane_Data *pd, void *data, const char *name, const char *description, Evas_Object *icon, Elm_Settingspane_Item *rel)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(rel, NULL);
   Elm_Settingspane_Item_Data *id = eo_data_scope_get(rel, ELM_SETTINGSPANE_ITEM_CLASS);
   return _elm_settingspane_item_append_full(obj, pd, data, name, description, icon, id->par, rel);
}

/* Item implement */
EOLIAN static void
_elm_settingspane_item_focus(Eo *obj, Elm_Settingspane_Item_Data *pd)
{
   _history_stack_push(pd->sw, obj);
}

EOLIAN static void
_elm_settingspane_item_delete(Eo *obj, Elm_Settingspane_Item_Data *pd EINA_UNUSED)
{
   _item_del(obj, EINA_FALSE);
}

EOLIAN static const Elm_Settingspane_Item *
_elm_settingspane_item_parent_get(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   return pd->par;
}

EOLIAN static void
_elm_settingspane_item_changed_set(Eo *obj, Elm_Settingspane_Item_Data *pd, Eina_Bool changed)
{
   pd->changed = changed;
   if (changed)
     _unsaved_changes_add(pd->sw, obj);
   else
     _unsaved_changes_remove(pd->sw, obj);
   _content_layout_content_controls_refresh(pd->sw, obj);
}

EOLIAN static Eina_Bool
_elm_settingspane_item_changed_get(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   return pd->changed;
}

EOLIAN static const char *
_elm_settingspane_item_name_get(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   return pd->name;
}

EOLIAN static const char *
_elm_settingspane_item_description_get(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   return pd->description;
}

EOLIAN static void
_elm_settingspane_item_attach_panel(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd, Content_Get_Cb content_get, Evas_Smart_Cb reset, Evas_Smart_Cb apply)
{
   if (pd->childs)
     {
        ERR("You cannot add a panel to a item with childs!");
        return;
     }
   pd->panel.content_get = content_get;
   pd->panel.reset = reset;
   pd->panel.apply = apply;
}

EOLIAN static void
_elm_settingspane_item_eo_base_destructor(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   if (pd->panel.tmp_content)
     eo_unref(pd->panel.tmp_content);
   //unref the icon,
   eo_unref(pd->icon);
   eo_do_super(obj, ELM_SETTINGSPANE_ITEM_CLASS, eo_destructor());
}


EOLIAN static void
_elm_settingspane_item_keyword_add(Elm_Settingspane_Item *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd, Eina_Stringshare *str)
{
  eina_stringshare_ref(str);
  pd->key_words = eina_list_append(pd->key_words, str);
}

EOLIAN static void
_elm_settingspane_item_keyword_delete(Elm_Settingspane_Item *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd, Eina_Stringshare *str)
{
  eina_stringshare_del(str);
  pd->key_words = eina_list_remove(pd->key_words, str);
}

/* widget api calls */
EOLIAN static Eina_Bool
_elm_settingspane_save_quit(Eo *obj EINA_UNUSED, Elm_Settingspane_Data *pd)
{
    return (pd->unsaved_changes) ? EINA_FALSE : EINA_TRUE;
}

EOLIAN static void
_elm_settingspane_raise_unsaved(Eo *obj, Elm_Settingspane_Data *pd EINA_UNUSED)
{
   int x, y, w, h;
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   _conf_unsaved_menu_show(obj, w/2, h/2, "clean");
}

EOLIAN static void
_elm_settingspane_eo_base_destructor(Eo *obj, Elm_Settingspane_Data *pd)
{
   _item_del(pd->zero, EINA_TRUE);
   eo_do_super(obj, ELM_SETTINGSPANE_CLASS, eo_destructor());
}


static Elm_Settingspane_Item*
_item_search(Elm_Settingspane_Item *it, char **string, int lvl)
{
   IC_DATA(it);
   Elm_Settingspane_Item *item;
   Eina_List *node;

   EINA_LIST_FOREACH(id->childs, node, item)
   {
      Elm_Settingspane_Item_Data *iid = eo_data_scope_get(item, ELM_SETTINGSPANE_ITEM_CLASS);

      if (strcmp(iid->name, string[lvl]))
         continue;

      if (string[lvl +1])
        return _item_search(item, string, lvl +1);
      else
        return item;
   }
   return NULL;
}

EOLIAN static Elm_Settingspane_Item *
_elm_settingspane_item_get(Eo *obj EINA_UNUSED, Elm_Settingspane_Data *pd, const char *paths)
{
   char **string = eina_str_split(paths, "/", 0);
   return _item_search(pd->zero, string, 0);
}

/* Implement of a legacy function */

EAPI Evas_Object *
elm_settingspane_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(ELM_SETTINGSPANE_CLASS, parent);
   return obj;
}
#include "elm_settingspane_item.eo.c"
#include "elm_settingspane.eo.c"