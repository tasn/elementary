#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Elementary.h>

#include <elm_priv.h>
#include "elm_widget_layout.h"

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
  Evas_Object *list; //the genlist
  Evas_Object *layout;
  Eina_Bool visible_hidden; //the menu is there but hidden
} Menu;

typedef struct
{
  Evas_Object *apply; //< apply button
  Evas_Object *reset; //< reset button
  Evas_Object *layout; //< The layout where everything is displayed
  Eina_Bool hidden; //< true if it is realized but hidden
} Panel;

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
      Evas_Object *display, *grid, *obj;
      Panel *p;
      Ecore_Thread *run;
      const char* keyword;
   } search;
   Evas_Object *title_box, *search_label, *search_results, *unsaved_opened;
   Evas_Object *table_panel;
   Evas_Object *table_menu;
   Elm_Settingspane_Item *menu_hidden;
   Elm_Settingspane_Item *menu_visible;
   Elm_Settingspane_Item *panel_visible;
} Elm_Settingspane_Data;

typedef struct
{
   Elm_Settingspane_Item *par;

   Eina_List *childs; //list of Elm_Settingspane_Item*

   const char *key_word; //< the original string which was given
   Eina_List *key_words; //< list of strdup´ed strings to match searching

   const char *name;
   const char *description;
   void *data;

   Evas_Object *sw; //< widget where this is created
   Eina_Bool changed;

   Elm_Widget_Item *it;
   Evas_Object *icon;

   struct
   {
      Evas_Smart_Cb apply;
      Evas_Smart_Cb reset;
      Content_Get_Cb content_get;
   } event;
   Panel *panel; //< if this item is realized its not NULL
   Menu *menu;
} Elm_Settingspane_Item_Data;

typedef struct _Search_Run
{
   Eina_List *results;
   Elm_Settingspane_Data *data;
   const char *keyword;
} Search_Run;


static void _item_menu_refresh(Eo *obj, Elm_Settingspane_Item_Data *pd);

static void _unsaved_changes_add(Evas_Object *w, Elm_Object_Item *item);
static void _unsaved_changes_remove(Evas_Object *w, Elm_Object_Item *item);

static void _history_stack_push(Evas_Object *w, Elm_Object_Item *it);
static Elm_Object_Item* _history_stack_pop(Evas_Object *w);
static Elm_Object_Item* _history_stack_current(Evas_Object *w);
static Eina_Bool _history_stack_end(Evas_Object *w);
static void _history_stack_remove(Evas_Object *w, Elm_Object_Item *it);

static void _item_sel_cb(void *data, Evas_Object *obj, void *event_info);
static void _search_panel_stop(Evas_Object *w);

#define HAS_PANEL(id) (id->event.content_get)
/* Data Keys in the layouts, DK = Data Key */

#define DK_MAIN_MENU_SHOWED "__menu_layout_show"
#define DK_MAIN_MENU_HIDDEN "__menu_layout_hidden"
#define DK_MAIN_MENU_BEHIND "__menu_layout_behind"

#define DK_MENU_GENLIST "__menu_genlist"
#define DK_MENU_ITEM_SHOWN "__showed_item"

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

static Panel* _item_panel_realize(Eo *obj, Evas_Object *w, Evas_Object *content);

static const Elm_Layout_Part_Alias_Description _content_aliases[] =
{
   {"options", "elm.swallow.options"},
   {NULL, NULL}
};

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
 * =================================
 *        Titlebar stuff
 * =================================
 */
static void
_title_bar_refresh(Evas_Object *w)
{
   Elm_Object_Item *it, *walker;
   Elm_Settingspane_Item_Data *id = NULL;
   Evas_Object *bt;

   C_DATA(w);

   it = _history_stack_current(w);

   elm_box_clear(wd->title_box);

   do {
        walker = id ? id->par : it;
        id = IC_DATA_L(walker);

        bt = elm_button_add(wd->title_box);
        elm_object_style_set(bt, "navlink");
        elm_object_text_set(bt, id->name);
        evas_object_show(bt);

        elm_box_pack_start(wd->title_box, bt);
   } while(id->par);
}

/**
 * =============================
 *   Events of a item
 * =============================
 */

static void
_item_content_reset(Elm_Settingspane_Item *it)
{
   IC_DATA(it);

   if (id->event.reset)
     id->event.reset(id->data, id->sw, it);
}

static void
_item_content_apply(Elm_Settingspane_Item *it)
{
   IC_DATA(it);

   if (id->event.apply)
     id->event.apply(id->data, id->sw, it);
}

static Evas_Object*
_item_content_get(Elm_Settingspane_Item *it, Evas_Object *obj)
{
   IC_DATA(it);

   if (id->event.content_get)
     return id->event.content_get(obj, it, id->data);
   else
     return NULL;
}


/*
 * =====================================
 *     apply and reset of the items
 * =====================================
 */

static void
_content_layout_apply_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event EINA_UNUSED)
{
   _item_content_apply(data);
}

static void
_content_layout_reset_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event EINA_UNUSED)
{
   _item_content_reset(data);
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
_history_stack_push(Evas_Object *w, Elm_Settingspane *it)
{
   C_DATA(w);

   /* we just want one item to be on the stack */
   _history_stack_remove(w, it);

   wd->history_stack = eina_list_append(wd->history_stack, it);
   /* check if back is still valid */
   _back_check(w);
   /* refresh tiltlebar */
   _title_bar_refresh(w);
}

static Elm_Object_Item*
_history_stack_pop(Evas_Object *w)
{
   C_DATA(w);
   Elm_Object_Item *it;

   if (_history_stack_end(w))
     return NULL;

   it = _history_stack_current(w);

   eo_do(it, elm_obj_settingspane_item_unrealize());

   /* pop the item */
   wd->history_stack = eina_list_remove(wd->history_stack, it);

   /* new top item */
   it = _history_stack_current(w);
   eo_do(it, elm_obj_settingspane_item_realize());

   /* check if back is still valid */
   _back_check(w);
   /* refresh tiltlebar */
   _title_bar_refresh(w);
   return it;
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
     _item_menu_refresh(root, id);
}

static void
_item_del(Elm_Settingspane_Item *del, Eina_Bool full)
{
   IC_DATA(del);
   Elm_Settingspane_Item_Data *id_par = NULL;
   Eina_List *node, *nnode;
   Elm_Settingspane_Item *ci;

   eo_do(del, elm_obj_settingspane_item_unrealize());

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
        if (!full)
          _item_menu_refresh(id->par, id_par);
     }

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
   wd->search_label = l;
   elm_box_pack_end(o, l);
   evas_object_show(l);

   g = elm_gengrid_add(w);
   elm_gengrid_item_size_set(g, 150, 150);
   evas_object_size_hint_weight_set(g, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(g, EVAS_HINT_FILL, EVAS_HINT_FILL);
   wd->search.grid = g;
   elm_box_pack_end(o, g);
   evas_object_show(g);

   wd->search.obj = o;
   Panel *p = _item_panel_realize(NULL, w, o);
   wd->search.p = p;
   elm_table_pack(wd->table_panel, p->layout, 0, 0, 1, 1);

   elm_layout_signal_emit(w, EMIT_MENU_SEARCHMODE_ENABLE);
   elm_layout_signal_emit(p->layout, EMIT_CONTENT_VISIBLE);
   evas_object_raise(p->layout);
}

static void
_search_panel_hide(Evas_Object *w)
{
   C_DATA(w);

   elm_layout_signal_emit(wd->search.p->layout, EMIT_CONTENT_DEFAULT);
   elm_layout_signal_emit(w, EMIT_MENU_SEARCHMODE_DISABLE);
   free(wd->search.p);
   wd->search.p = NULL;
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

   elm_gengrid_clear(sr->data->search.grid);

   gic = elm_gengrid_item_class_new();
   gic->item_style = "thumb";
   gic->func.text_get = _search_grid_label_get;
   gic->func.content_get = _search_grid_content_get;
   gic->func.del = _search_grid_del;

   EINA_LIST_FOREACH(sr->results, node, it)
     {
        elm_gengrid_item_append(sr->data->search.grid, gic, it, _search_grid_item_sel, it);
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

   sr = calloc(1, sizeof(Search_Run));
   sr->keyword = word;
   sr->data = wd;

   snprintf(buf, sizeof(buf), "Search results for \"%s\": ", word);
   elm_object_text_set(wd->search_label, buf);

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
   C_DATA(id->sw);

   wd->unsaved_opened = NULL;

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
   C_DATA(data);

   wd->unsaved_opened = NULL;
}

static void
_conf_unsaved_menu_show(Evas_Object *par, int x, int y, char *style )
{
   Elm_Object_Item *it;
   Eina_List *node;
   Evas_Object *ctx;
   C_DATA(par);

   if (eina_list_count(wd->unsaved_changes) == 0 || wd->unsaved_opened)
     return;

   ctx = elm_ctxpopup_add(par);
   wd->unsaved_opened = ctx;
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
        elm_ctxpopup_item_append(ctx, id->name, NULL, _history_menu_item_sel_cb, it);
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


EOLIAN static const Elm_Layout_Part_Alias_Description*
_elm_settingspane_elm_layout_content_aliases_get(Eo *obj EINA_UNUSED, Elm_Settingspane_Data *_pd EINA_UNUSED)
{
   return _content_aliases;
}


EOLIAN static Elm_Settingspane_Item*
_elm_settingspane_focused_get(Eo *obj, Elm_Settingspane_Data *pd EINA_UNUSED)
{
   Elm_Settingspane_Item *current = _history_stack_current(obj);

   if (current == pd->zero) /* we dont want to return our internal item */
     return NULL;
   else
     return current;
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
   pd->title_box = bx;
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
   pd->table_panel = tb1;

   tb2 = elm_table_add(obj);
   evas_object_size_hint_weight_set(tb2, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb2, 0.0, EVAS_HINT_FILL);
   elm_object_part_content_set(obj, POS_MAIN_MENU, tb2);
   evas_object_show(tb2);
   pd->table_menu = tb2;

   pd->zero = _item_new(obj, NULL, NULL, eina_stringshare_add("root"), eina_stringshare_add(""), NULL);
 }

EOLIAN static void
_elm_settingspane_evas_object_smart_show(Eo *obj, Elm_Settingspane_Data *pd)
{
   Elm_Settingspane_Item *it;

   it = _history_stack_current(obj);
   if (!it)
     eo_do(pd->zero, elm_obj_settingspane_item_focus());

   _title_bar_refresh(obj);
   eo_do_super(obj, ELM_SETTINGSPANE_CLASS, evas_obj_smart_show());
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

static Evas_Object*
_elm_settingspane_icon_gen(Evas_Object *par, const char *file, const char *group)
{
  Evas_Object *ic;

  ic = elm_icon_add(par);

  if (group && file)
    {
       //we have a edje file
       elm_image_file_set(ic, file, group);
    }
  else if (!group && file)
    {
      //we have a normal file
      elm_image_file_set(ic, file, group);
    }
  else if (group && !file)
    {
      //we have a standart icon
      elm_icon_standard_set(ic, group);
    }
  else
    {
      evas_object_del(ic);
      return NULL;
    }
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
  return ic;
}

static Elm_Settingspane_Item *
_elm_settingspane_item_append_full(Eo *obj, Elm_Settingspane_Data *pd, void *data,
                                   const char *name, const char *description,
                                   const char *file, const char *group,
                                   Elm_Settingspane_Item *par, Elm_Settingspane_Item *rel)
{
   Elm_Settingspane_Item *new_child = NULL;
   Elm_Settingspane_Item_Data *data_par;
   Evas_Object *icon;

   icon = _elm_settingspane_icon_gen(obj, file, group);

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
_elm_settingspane_item_append(Eo *obj, Elm_Settingspane_Data *pd, void *data, const char *name, const char *description, const char *file, const char *group, Elm_Settingspane_Item *par)
{
   return _elm_settingspane_item_append_full(obj, pd, data, name, description, file, group, par, NULL);
}

EOLIAN static Elm_Settingspane_Item *
_elm_settingspane_item_append_relative(Eo *obj, Elm_Settingspane_Data *pd, void *data, const char *name, const char *description, const char *file, const char *group, Elm_Settingspane_Item *rel)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(rel, NULL);
   Elm_Settingspane_Item_Data *id = eo_data_scope_get(rel, ELM_SETTINGSPANE_ITEM_CLASS);
   return _elm_settingspane_item_append_full(obj, pd, data, name, description, file, group, id->par, rel);
}

/* Item implement */
EOLIAN static void
_elm_settingspane_item_focus(Eo *obj, Elm_Settingspane_Item_Data *pd)
{
   eo_do(obj, elm_obj_settingspane_item_realize());
   _history_stack_push(pd->sw, obj);
}

static void
_default_reached_cb(void *data, Evas_Object *obj, const char *emission EINA_UNUSED, const char *source EINA_UNUSED)
{
   Panel *p;
   if (!data)
     {
        evas_object_del(obj);
        return;
     }

   IC_DATA(data);
   p = id->panel;

   if (id->changed)
     {
        p->hidden = EINA_TRUE;
        evas_object_hide(p->layout);
        DBG("keep item %p", data);
     }
   else
     {
        evas_object_del(p->layout);
        free(p);
        id->panel = NULL;
        DBG("delete item %p", data);
     }
}

static Panel*
_item_panel_realize(Eo *obj, Evas_Object *w, Evas_Object *content)
{
   Panel *p;
   Evas_Object *bx, *sc;

   p = calloc(1, sizeof(Panel));

   p->layout = elm_layout_add(w);
   evas_object_size_hint_weight_set(p->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(p->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_layout_theme_set(p->layout, "settingspane", "panel", "default");
   elm_layout_signal_callback_add(p->layout, "elm,state,default,reached", EMITTER, _default_reached_cb, obj);

   bx = elm_box_add(w);
   evas_object_size_hint_weight_set(bx, 0, 0);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_horizontal_set(bx, EINA_TRUE);
   elm_object_part_content_set(p->layout, POS_PANEL_CONTROLS, bx);

   p->apply = elm_button_add(w);
   evas_object_size_hint_weight_set(p->apply, 0.0, 0.0);
   evas_object_size_hint_align_set(p->apply, 0.0, 0.0);
   evas_object_smart_callback_add(p->apply, "clicked", _content_layout_apply_cb, obj);
   elm_object_text_set(p->apply, "Apply");
   elm_box_pack_end(bx, p->apply);
   evas_object_show(p->apply);

   p->reset = elm_button_add(w);
   evas_object_size_hint_weight_set(p->reset, 0, 0);
   evas_object_size_hint_align_set(p->reset, 0.0, 0.0);
   elm_object_text_set(p->reset, "Reset");
   evas_object_smart_callback_add(p->reset, "clicked", _content_layout_reset_cb, obj);
   elm_box_pack_end(bx, p->reset);
   evas_object_show(p->reset);

   sc = elm_scroller_add(w);
   evas_object_size_hint_weight_set(sc,
                       EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_object_content_set(sc, content);
   evas_object_show(sc);

   elm_object_part_content_set(p->layout, POS_PANEL_CONTENT, sc);
   return p;
}

static inline void
_item_panel_show(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   Panel *p = pd->panel;
   elm_layout_signal_emit(p->layout, EMIT_CONTENT_VISIBLE);
   evas_object_raise(p->layout);
   evas_object_show(p->layout);
}

static void
_item_menu_realize(Eo *obj, Elm_Settingspane_Item_Data *pd, Elm_Settingspane_Data *wd)
{
   Menu *m;
   Evas_Object *o, *list;

   m = calloc(1, sizeof(Menu));

   m->layout = o = elm_layout_add(pd->sw);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_layout_theme_set(o, "settingspane", "menu", "default");

   m->list = list = elm_genlist_add(pd->sw);
   elm_scroller_bounce_set(list, EINA_FALSE, EINA_FALSE);
   elm_object_style_set(list, "flatedge");
   evas_object_size_hint_weight_set(list, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_genlist_mode_set(list, ELM_LIST_LIMIT);
   evas_object_data_set(o, DK_MENU_GENLIST, list);
   elm_object_part_content_set(o, POS_MENU_CONTENT, list);
   evas_object_show(list);

   pd->menu = m;
   elm_table_pack(wd->table_menu, m->layout, 0, 0, 1, 1);

   _item_menu_refresh(obj, pd);
}
static inline void
_item_menu_show(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd, Eina_Bool hidden)
{
   Menu *m;

   m = pd->menu;
   if (hidden)
     elm_layout_signal_emit(m->layout, EMIT_MENU_VISIBLE_HIDDEN);
   else
     elm_layout_signal_emit(m->layout, EMIT_MENU_VISIBLE);
   evas_object_show(m->layout);
}

static inline void
_item_menu_visible_hide(Eo *obj)
{
   IC_DATA(obj);
   Menu *m;

   m = id->menu;
   if (!m) return;

   elm_layout_signal_emit(m->layout, EMIT_MENU_HIDDEN);
   evas_object_raise(m->layout);
}


EOLIAN static void
_elm_settingspane_item_realize(Eo *obj, Elm_Settingspane_Item_Data *pd)
{
  C_DATA(pd->sw);
  DBG("realize %p", obj);
  if (pd->event.content_get) //this is a panel
    {
       if (!pd->panel)
         {
            Panel *p;

            p =_item_panel_realize(obj, pd->sw, _item_content_get(obj, pd->sw));
            elm_table_pack(wd->table_panel, p->layout, 0, 0, 1, 1);
            pd->panel = p;
         }
       /* show its menu */
       if (pd->par)
         eo_do(pd->par, elm_obj_settingspane_item_realize());
       /* show this panel */
       _item_panel_show(obj, pd);
       /* if another one is open close it */
       if (wd->panel_visible)
         eo_do(wd->panel_visible, elm_obj_settingspane_item_unrealize());
       /* set us as visible */
       wd->panel_visible = obj;
    }
  else
    {
       /* dont do the work if this is the current menu */
       if (wd->menu_visible == obj)
         return;
       /* if we are realizing a menu and a panel is open, close it*/
       if (wd->panel_visible)
         eo_do(wd->panel_visible, elm_obj_settingspane_item_unrealize());
       /* if this is the hidden object */
       if (wd->menu_hidden == obj)
         {
            if (wd->menu_visible)
              eo_do(wd->menu_visible, elm_obj_settingspane_item_unrealize());
            wd->menu_hidden = NULL;
            if (pd->par)
              {
                 Elm_Settingspane_Item_Data *ido;
                 ido = IC_DATA_L(pd->par);
                 _item_menu_realize(pd->par, ido, wd);
                 wd->menu_hidden = pd->par;
                _item_menu_visible_hide(wd->menu_hidden);
              }
            _item_menu_show(obj, pd, (!!wd->menu_hidden));
         }
       else
         {
            /* if this is not realized realize the object */
            if (!pd->menu)
              _item_menu_realize(obj, pd, wd);

            if (wd->menu_hidden)
              eo_do(wd->menu_hidden, elm_obj_settingspane_item_unrealize());
            if (wd->menu_visible)
              _item_menu_visible_hide(wd->menu_visible);

            wd->menu_hidden = wd->menu_visible;
            wd->menu_visible = obj;       /* show the menu */
            _item_menu_show(obj, pd, (!!wd->menu_hidden));
         }
       DBG("Hidden : %p visible: %p", wd->menu_hidden, wd->menu_visible);
    }
}

static void
_item_panel_unrealize(Eo *obj  EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   Panel *p = pd->panel;
   elm_layout_signal_emit(p->layout, EMIT_CONTENT_DEFAULT);
}

static void
_item_menu_unrealize(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   Menu *m;

   m = pd->menu;
   elm_layout_signal_emit(m->layout, EMIT_CONTENT_DEFAULT);
   evas_object_del(m->layout);
   free(m);
   pd->menu = NULL;
}

static void
_item_menu_refresh(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   Eina_List *node;
   Elm_Settingspane_Item *child;
   C_DATA(pd->sw);

   if (!pd->menu) return;

   elm_genlist_clear(pd->menu->list);

   EINA_LIST_FOREACH(pd->childs, node, child)
     {
        Elm_Settingspane_Item_Data *cid = IC_DATA_L(child);
        Elm_Object_Item *it;

        it = elm_genlist_item_append(pd->menu->list, wd->itcc, child,
                                NULL, ELM_GENLIST_ITEM_NONE,
                                _item_sel_cb, child);
        elm_genlist_item_select_mode_set(it, ELM_OBJECT_SELECT_MODE_ALWAYS);
        elm_genlist_item_tooltip_text_set(it, cid->description);
     }
}

EOLIAN static void
_elm_settingspane_item_unrealize(Eo *obj, Elm_Settingspane_Item_Data *pd)
{
  C_DATA(pd->sw);
  DBG("unrealize %p", obj);
  if (!pd->panel && !pd->menu)
    return;

  if (pd->event.content_get) //this is a panel
    {
       _item_panel_unrealize(obj, pd);
       if (wd->panel_visible == obj)
         wd->panel_visible = NULL;
    }
  else
    {
       _item_menu_unrealize(obj, pd);
       if (wd->menu_visible == obj)
         wd->menu_visible = NULL;
       if (wd->menu_hidden == obj)
         wd->menu_hidden = NULL;
    }
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

   if (!pd->panel) return;

    if (!pd->changed)
      elm_layout_signal_emit(pd->panel->layout, EMIT_CONTENT_CONTROLS_HIDE);
    else
      elm_layout_signal_emit(pd->panel->layout, EMIT_CONTENT_CONTROLS_SHOW);

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
   pd->event.content_get = content_get;
   pd->event.reset = reset;
   pd->event.apply = apply;
}

EOLIAN static void
_elm_settingspane_item_eo_base_destructor(Eo *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
   if (pd->panel)
     eo_unref(pd->panel->layout);
   //unref the icon,
   eo_unref(pd->icon);
   eo_do_super(obj, ELM_SETTINGSPANE_ITEM_CLASS, eo_destructor());
}

EOLIAN static void
_elm_settingspane_item_keywords_set(Elm_Settingspane_Item *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd, char *v)
{
  char **splits;
  char *split;
  const char *word, *str;
  int i = 0;

  /* save the original string */
  str = eina_stringshare_add(v);
  if (pd->key_word)
    eina_stringshare_del(pd->key_word);

  pd->key_word = str;

  /* free the existing list */
  EINA_LIST_FREE(pd->key_words, word)
    {
       eina_stringshare_del(word);
    }
  /* parse out the single words */
  splits = eina_str_split(pd->key_word, ",", -1);

  if (!splits) return;

  for(i = 0; splits[i]; i++)
    {
       split = splits[i];
       pd->key_words = eina_list_append(pd->key_words, eina_stringshare_add(split));
    }

  pd->key_words = eina_list_append(pd->key_words, str);
}

EOLIAN static char *
_elm_settingspane_item_keywords_get(Elm_Settingspane_Item *obj EINA_UNUSED, Elm_Settingspane_Item_Data *pd)
{
  return ((char*)pd->key_word);
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
