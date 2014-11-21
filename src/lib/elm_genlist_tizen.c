#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define TIZEN_ONLY 0

#include <fnmatch.h>

#include <Elementary.h>
#include <Elementary_Cursor.h>

#include "elm_priv.h"
#include "elm_widget_genlist.h"
#include "elm_interface_scrollable.h"

EAPI Eo_Op ELM_OBJ_GENLIST_PAN_BASE_ID = EO_NOOP;

#define MY_PAN_CLASS ELM_OBJ_GENLIST_PAN_CLASS

#define MY_PAN_CLASS_NAME "Elm_Genlist_Pan"
#define MY_PAN_CLASS_NAME_LEGACY "elm_genlist_pan"

EAPI Eo_Op ELM_OBJ_GENLIST_BASE_ID = EO_NOOP;

#define MY_CLASS ELM_OBJ_GENLIST_CLASS

#define MY_CLASS_NAME "Elm_Genlist"
#define MY_CLASS_NAME_LEGACY "elm_genlist"

// internally allocated
#define CLASS_ALLOCATED     0x3a70f11f

#define MAX_ITEMS_PER_BLOCK 32
#define REORDERED_ITEM_OFFSET (8 * elm_config_scale_get())
#define REORDER_FASTER 5
#define REORDER_ANIM_OFFSET (8 * elm_config_scale_get())
#define PINCH_ZOOM_TOLERANCE 0.4
#define ITEM_HIGHLIGHT_TIMER 0.1
#define ITEM_SELECT_TIMER 0.01  // This is needed for highlight effect when item is unhighlighted right after item is highlighted and selected

#define ERR_ABORT(_msg)                         \
   do {                                         \
        ERR(_msg);                              \
        if (getenv("ELM_ERROR_ABORT")) abort(); \
   } while (0)

#define ELM_PRIV_GENLIST_SIGNALS(cmd) \
    cmd(SIG_ACTIVATED, "activated", "") \
    cmd(SIG_CLICKED_DOUBLE, "clicked,double", "") \
    cmd(SIG_SELECTED, "selected", "") \
    cmd(SIG_UNSELECTED, "unselected", "") \
    cmd(SIG_EXPANDED, "expanded", "") \
    cmd(SIG_CONTRACTED, "contracted", "") \
    cmd(SIG_EXPAND_REQUEST, "expand,request", "") \
    cmd(SIG_CONTRACT_REQUEST, "contract,request", "") \
    cmd(SIG_REALIZED, "realized", "") \
    cmd(SIG_UNREALIZED, "unrealized", "") \
    cmd(SIG_DRAG_START_UP, "drag,start,up", "") \
    cmd(SIG_DRAG_START_DOWN, "drag,start,down", "") \
    cmd(SIG_DRAG_START_LEFT, "drag,start,left", "") \
    cmd(SIG_DRAG_START_RIGHT, "drag,start,right", "") \
    cmd(SIG_DRAG_STOP, "drag,stop", "") \
    cmd(SIG_DRAG, "drag", "") \
    cmd(SIG_LONGPRESSED, "longpressed", "") \
    cmd(SIG_SCROLL, "scroll", "") \
    cmd(SIG_SCROLL_ANIM_START, "scroll,anim,start", "") \
    cmd(SIG_SCROLL_ANIM_STOP, "scroll,anim,stop", "") \
    cmd(SIG_SCROLL_DRAG_START, "scroll,drag,start", "") \
    cmd(SIG_SCROLL_DRAG_STOP, "scroll,drag,stop", "") \
    cmd(SIG_EDGE_TOP, "edge,top", "") \
    cmd(SIG_EDGE_BOTTOM, "edge,bottom", "") \
    cmd(SIG_EDGE_LEFT, "edge,left", "") \
    cmd(SIG_EDGE_RIGHT, "edge,right", "") \
    cmd(SIG_VBAR_DRAG, "vbar,drag", "") \
    cmd(SIG_VBAR_PRESS, "vbar,press", "") \
    cmd(SIG_VBAR_UNPRESS, "vbar,unpress", "") \
    cmd(SIG_HBAR_DRAG, "hbar,drag", "") \
    cmd(SIG_HBAR_PRESS, "hbar,press", "") \
    cmd(SIG_HBAR_UNPRESS, "hbar,unpress", "") \
    cmd(SIG_MULTI_SWIPE_LEFT, "multi,swipe,left", "") \
    cmd(SIG_MULTI_SWIPE_RIGHT, "multi,swipe,right", "") \
    cmd(SIG_MULTI_SWIPE_UP, "multi,swipe,up", "") \
    cmd(SIG_MULTI_SWIPE_DOWN, "multi,swipe,down", "") \
    cmd(SIG_MULTI_PINCH_OUT, "multi,pinch,out", "") \
    cmd(SIG_MULTI_PINCH_IN, "multi,pinch,in", "") \
    cmd(SIG_SWIPE, "swipe", "") \
    cmd(SIG_MOVED, "moved", "") \
    cmd(SIG_MOVED_AFTER, "moved,after", "") \
    cmd(SIG_MOVED_BEFORE, "moved,before", "") \
    cmd(SIG_INDEX_UPDATE, "index,update", "") \
    cmd(SIG_TREE_EFFECT_FINISHED , "tree,effect,finished", "") \
    cmd(SIG_HIGHLIGHTED, "highlighted", "") \
    cmd(SIG_UNHIGHLIGHTED, "unhighlighted", "") \
    cmd(SIG_LANG_CHANGED, "language,changed", "") \
    cmd(SIG_PRESSED, "pressed", "") \
    cmd(SIG_RELEASED, "released", "") \
    cmd(SIG_LOADED, "loaded", "")

ELM_PRIV_GENLIST_SIGNALS(ELM_PRIV_STATIC_VARIABLE_DECLARE);

// ****  edje interface signals *** //
static const char SIGNAL_ENABLED[] = "elm,state,enabled";
static const char SIGNAL_DISABLED[] = "elm,state,disabled";
static const char SIGNAL_HIGHLIGHTED[] = "elm,state,selected";     // actually highlighted
static const char SIGNAL_UNHIGHLIGHTED[] = "elm,state,unselected"; // actually unhighlighted
static const char SIGNAL_CLICKED[] = "elm,state,clicked";
static const char SIGNAL_EXPANDED[] = "elm,state,expanded";
static const char SIGNAL_CONTRACTED[] = "elm,state,contracted";
static const char SIGNAL_FLIP_ENABLED[] = "elm,state,flip,enabled";
static const char SIGNAL_FLIP_DISABLED[] = "elm,state,flip,disabled";
static const char SIGNAL_DECORATE_ENABLED[] = "elm,state,decorate,enabled";
static const char SIGNAL_DECORATE_ENABLED_EFFECT[] = "elm,state,decorate,enabled,effect";
static const char SIGNAL_DECORATE_DISABLED[] = "elm,state,decorate,disabled";
static const char SIGNAL_REORDER_ENABLED[] = "elm,state,reorder,enabled";
static const char SIGNAL_REORDER_DISABLED[] = "elm,state,reorder,disabled";
static const char SIGNAL_REORDER_MODE_SET[] = "elm,state,reorder,mode_set";
static const char SIGNAL_REORDER_MODE_UNSET[] = "elm,state,reorder,mode_unset";
static const char SIGNAL_DEFAULT[] = "elm,state,default";
static const char SIGNAL_FOCUSED[] = "elm,action,focus_highlight,show";
static const char SIGNAL_UNFOCUSED[] = "elm,action,focus_highlight,hide";

#if TIZEN_ONLY
static const char SIGNAL_BG_CHANGE[] = "bg_color_change";
static const char SIG_ITEM_FOCUSED[] = "item,focused";
static const char SIG_ITEM_UNFOCUSED[] = "item,unfocused";
static const char SIG_ACCESS_CHANGED[] = "access,changed";
#endif

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   ELM_PRIV_GENLIST_SIGNALS(ELM_PRIV_SMART_CALLBACKS_DESC)
   {"focused", ""}, /**< handled by elm_widget */
   {"unfocused", ""}, /**< handled by elm_widget */

   {NULL, NULL}
};
#undef ELM_PRIV_GENLIST_SIGNALS

static void _item_mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _item_mouse_move_cb(void *data, Evas *evas EINA_UNUSED, Evas_Object *obj, void *event_info);
static void _item_mouse_up_cb(void *data, Evas *evas, Evas_Object *obj EINA_UNUSED, void *event_info);
static Eina_Bool _long_press_cb(void *data);

static void _item_edje_callbacks_add(Elm_Gen_Item *it);
static void _item_edje_callbacks_del(Elm_Gen_Item *it);

static void _item_block_calc(Item_Block *);
static void _item_min_calc(Elm_Gen_Item *it);
static void _item_calc(Elm_Gen_Item *it);
static void _item_mouse_callbacks_add(Elm_Gen_Item *, Evas_Object *);
static void _item_mouse_callbacks_del(Elm_Gen_Item *, Evas_Object *);
static void _item_select(Elm_Gen_Item *it);

static void _expand_toggle_signal_cb(void *data, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED);
static void _expand_signal_cb(void *data, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED);
static void _contract_signal_cb(void *data, Evas_Object *obj EINA_UNUSED, const char *emission EINA_UNUSED, const char *source EINA_UNUSED);
static void _decorate_item_unrealize(Elm_Gen_Item *it);
static void _decorate_item_realize(Elm_Gen_Item *it);
static void _decorate_all_item_unrealize(Elm_Gen_Item *it);
static void _item_queue(Elm_Gen_Item *it, Eina_Compare_Cb cb);
static Eina_Bool _queue_idle_enter(void *data);
static void _dummy_job(void *data);

typedef enum
{
   FOCUS_DIR_UP = 0,
   FOCUS_DIR_DOWN,
   FOCUS_DIR_LEFT,
   FOCUS_DIR_RIGHT
} Focus_Dir;

typedef struct _Size_Cache {
     Evas_Coord minw;
     Evas_Coord minh;
} Size_Cache;

static void
_item_cache_all_free(Elm_Genlist_Smart_Data *sd)
{
   // It would be better not to use
   // EINA_INLIST_FOREACH or EINA_INLIST_FOREACH_SAFE
   while (sd->item_cache)
     {
        Item_Cache *ic = EINA_INLIST_CONTAINER_GET(sd->item_cache->last, Item_Cache);
        if (ic->base_view) evas_object_del(ic->base_view);
        if (ic->item_style) eina_stringshare_del(ic->item_style);
        sd->item_cache = eina_inlist_remove(sd->item_cache, EINA_INLIST_GET(ic));
        // Free should be performed after inlist is poped
        free(ic);
     }
   sd->item_cache = NULL;
   sd->item_cache_count = 0;
}

static void
_item_cache_push(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   Item_Cache *ic = NULL;

   if (sd->no_cache) return;

   if (sd->item_cache_count >= sd->item_cache_max)
    {
        ic = EINA_INLIST_CONTAINER_GET(sd->item_cache->last, Item_Cache);
        if (ic->base_view) evas_object_del(ic->base_view);
        eina_stringshare_del(ic->item_style);
        sd->item_cache = eina_inlist_remove(sd->item_cache,
                                            sd->item_cache->last);
        sd->item_cache_count--;
        // Free should be performed after inlist is poped
        free(ic);
     }

   ic = ELM_NEW(Item_Cache);
   if (!ic)
     {
        if (VIEW(it)) evas_object_del(VIEW(it));
        VIEW(it) = NULL;
        return;
     }
   // set item's state as default before pushing item into cache.
   edje_object_signal_emit(VIEW(it), SIGNAL_DEFAULT, "elm");

   edje_object_mirrored_set(VIEW(it),
                            elm_widget_mirrored_get(WIDGET(it)));
   edje_object_scale_set(VIEW(it),
                         elm_widget_scale_get(WIDGET(it))
                         * elm_config_scale_get());

   ic->base_view = VIEW(it);
   ic->multiline = GL_IT(it)->multiline;
   ic->item_style = eina_stringshare_add(it->itc->item_style);
   evas_object_hide(ic->base_view);

   sd->item_cache = eina_inlist_prepend(sd->item_cache, EINA_INLIST_GET(ic));
   sd->item_cache_count++;

   VIEW(it) = NULL;
}

static Eina_Bool
_item_cache_pop(Elm_Gen_Item *it)
{
   Item_Cache *ic = NULL;
   Eina_Inlist *l;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if (!it->itc->item_style) return EINA_FALSE;

   EINA_INLIST_FOREACH_SAFE(sd->item_cache, l, ic)
     {
        if (ic->item_style &&
            (!strcmp(it->itc->item_style, ic->item_style)))
          {
             sd->item_cache =
                eina_inlist_remove(sd->item_cache, EINA_INLIST_GET(ic));
             sd->item_cache_count--;

             VIEW(it) = ic->base_view;
             GL_IT(it)->multiline = ic->multiline;
             eina_stringshare_del(ic->item_style);

             free(ic);
             return EINA_TRUE;
          }
     }
   return EINA_FALSE;
}

static void
_item_text_realize(Elm_Gen_Item *it,
                   Evas_Object *target,
                   const char *parts)
{
   if (it->itc->func.text_get)
     {
        Eina_List *source;
        const char *key;

        source = elm_widget_stringlist_get
           (edje_object_data_get(target, "texts"));
        EINA_LIST_FREE(source, key)
          {
             if (parts && fnmatch(parts, key, FNM_PERIOD))
               continue;

             char *s = it->itc->func.text_get
                 ((void *)it->base.data, WIDGET(it), key);

             if (s)
               {
                  edje_object_part_text_escaped_set(target, key, s);
                  free(s);
               }
             else
               {
                  edje_object_part_text_set(target, key, "");
               }
          }
     }
}

static void
_changed_size_hints(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Elm_Gen_Item *it = data;
   GL_IT(it)->calc_done = EINA_FALSE;
   GL_IT(it)->block->calc_done = EINA_FALSE;
   GL_IT(it)->wsd->calc_done = EINA_FALSE;
   evas_object_smart_changed(GL_IT(it)->wsd->pan_obj);
}

// FIXME: There are applications which do not use elm_win as top widget.
// This is workaround! Those could not use focus!
static Eina_Bool _focus_enabled(Evas_Object *obj)
{
   if (!elm_widget_focus_get(obj)) return EINA_FALSE;

   const Evas_Object *win = elm_widget_top_get(obj);
   const char *type = evas_object_type_get(win);

   if (type && !strcmp(type, "elm_win"))
     {
        return elm_win_focus_highlight_enabled_get(win);
     }
   return EINA_FALSE;
}

static Eina_List *
_item_content_realize(Elm_Gen_Item *it,
                      Evas_Object *target,
                      Eina_List *contents,
                      const char *src,
                      const char *parts)
{
   if (!parts)
     {
        Evas_Object *c;
        EINA_LIST_FREE(contents, c)
          {
             // FIXME: For animation, befor del, processing edc.
             if (evas_object_smart_type_check(c, "elm_layout"))
               edje_object_message_signal_process(elm_layout_edje_get(c));
             // FIXME: If parent-child relationship was broken before 'ic'
             // is deleted, freeze_pop will not be called. ex) elm_slider
             // If layout is used instead of edje, this problme can be
             // solved.
             if (0 != elm_widget_scroll_freeze_get(c))
               elm_widget_scroll_freeze_pop(c);
             evas_object_del(c);
          }
     }
   if (it->itc->func.content_get)
     {
        const char *key;
        Evas_Object *ic = NULL;
        Eina_List *source;

        source = elm_widget_stringlist_get
           (edje_object_data_get(target, src));

        EINA_LIST_FREE(source, key)
          {
             if (parts && fnmatch(parts, key, FNM_PERIOD))
               continue;

             Evas_Object *old = edje_object_part_swallow_get(target, key);
             if (old)
               {
                  contents = eina_list_remove(contents, old);
                  // FIXME: For animation, befor del, processing edc.
                  if (evas_object_smart_type_check(old, "elm_layout"))
                    edje_object_message_signal_process(elm_layout_edje_get(old));
                  // FIXME: If parent-child relationship was broken before 'ic'
                  // is deleted, freeze_pop will not be called. ex) elm_slider
                  // If layout is used instead of edje, this problme can be
                  // solved.
                  if (0 != elm_widget_scroll_freeze_get(old))
                    elm_widget_scroll_freeze_pop(old);
                  evas_object_del(old);
               }

             if (it->itc->func.content_get)
               ic = it->itc->func.content_get
                   ((void *)it->base.data, WIDGET(it), key);
             if (ic)
               {
                  if (!edje_object_part_swallow(target, key, ic))
                    {
                      WRN("%s (%p) can not be swallowed into %s",
                           evas_object_type_get(ic), ic, key);
                       continue;
                    }
                  if (elm_widget_item_disabled_get(it))
                    elm_widget_disabled_set(ic, EINA_TRUE);
                  evas_object_show(ic);
                  contents = eina_list_append(contents, ic);

                  if (GL_IT(it)->wsd->realization_mode)
                    {
                       evas_object_event_callback_add
                          (ic, EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                           _changed_size_hints, it);
                    }
               }
          }
     }

   return contents;
}

static void
_item_state_realize(Elm_Gen_Item *it,
                    Evas_Object *target,
                    const char *parts)
{
   if (it->itc->func.state_get)
     {
        Eina_List *src;
        const char *key;
        char buf[4096];

        src = elm_widget_stringlist_get
           (edje_object_data_get(target, "states"));
        EINA_LIST_FREE(src, key)
          {
             if (parts && fnmatch(parts, key, FNM_PERIOD))
               continue;

             Eina_Bool on = it->itc->func.state_get
                 ((void *)it->base.data, WIDGET(it), key);

             if (on)
               {
                  snprintf(buf, sizeof(buf), "elm,state,%s,active", key);
                  edje_object_signal_emit(target, buf, "elm");
               }
             else
               {
                  snprintf(buf, sizeof(buf), "elm,state,%s,passive", key);
                  edje_object_signal_emit(target, buf, "elm");
               }
          }
     }
}

static void
_view_theme_update(Elm_Gen_Item *it, Evas_Object *view, const char *style)
{
   char buf[1024];
   const char *key;
   Eina_List *l;

   snprintf(buf, sizeof(buf), "item/%s", style ? : "default");
   elm_widget_theme_object_set(WIDGET(it), view, "genlist", buf,
                               elm_widget_style_get(WIDGET(it)));
   edje_object_mirrored_set(view, elm_widget_mirrored_get(WIDGET(it)));
   edje_object_scale_set(view, elm_widget_scale_get(WIDGET(it))
                         * elm_config_scale_get());

   GL_IT(it)->multiline = EINA_FALSE;
   Eina_List *txts = elm_widget_stringlist_get
      (edje_object_data_get(view, "texts"));
   EINA_LIST_FOREACH(txts, l, key)
     {
        const Evas_Object *txt_obj = NULL;
        const char *type = NULL;
        txt_obj = edje_object_part_object_get(view, key);
        if (txt_obj) type =  evas_object_type_get(txt_obj);
        if (!type) continue;
        if (type && strcmp(type, "textblock")) continue;

        const Evas_Textblock_Style *tb_style =
           evas_object_textblock_style_get(txt_obj);
        if (tb_style)
          {
             const char *str = evas_textblock_style_get(tb_style);
             if (str)
               {
                  if (strstr(str, "wrap="))
                    {
                       GL_IT(it)->multiline = EINA_TRUE;
                       break;
                    }
               }
          }
     }
   eina_list_free(txts);
}

static Evas_Object *
_view_create(Elm_Gen_Item *it, const char *style)
{
   Evas_Object *view = edje_object_add(evas_object_evas_get(WIDGET(it)));
   evas_object_smart_member_add(view, GL_IT(it)->wsd->pan_obj);
   elm_widget_sub_object_add(WIDGET(it), view);
   edje_object_scale_set(view, elm_widget_scale_get(WIDGET(it)) *
                         elm_config_scale_get());

   _view_theme_update(it, view, style);
   return view;
}

static void
_view_clear(Evas_Object *view, Eina_List **contents)
{
   if (!view) return;
   const char *part;
   Evas_Object *c;
   Eina_List *texts  = elm_widget_stringlist_get
     (edje_object_data_get(view, "texts"));
   EINA_LIST_FREE(texts, part)
     edje_object_part_text_set(view, part, NULL);
   EINA_LIST_FREE(*contents, c)
     {
        // FIXME: For animation, befor del, processing edc.
        if (evas_object_smart_type_check(c, "elm_layout"))
          edje_object_message_signal_process(elm_layout_edje_get(c));
        // FIXME: If parent-child relationship was broken before 'ic'
        // is deleted, freeze_pop will not be called. ex) elm_slider
        // If layout is used instead of edje, this problme can be
        // solved.
        if (0 != elm_widget_scroll_freeze_get(c))
          elm_widget_scroll_freeze_pop(c);
        evas_object_del(c);
     }
}

static void
_view_inflate(Evas_Object *view, Elm_Gen_Item *it, Eina_List **contents)
{
   if (!view) return;
   _item_text_realize(it, view, NULL);
   *contents = _item_content_realize(it, view, *contents,
                                     "contents", NULL);
   _item_state_realize(it, view, NULL);
}

static void
_elm_genlist_item_index_update(Elm_Gen_Item *it)
{
   if (it->position_update || GL_IT(it)->block->position_update)
     {
        evas_object_smart_callback_call(WIDGET(it), SIG_INDEX_UPDATE, it);
        it->position_update = EINA_FALSE;
     }
}

static char *
_access_info_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   char *ret;
   Eina_Strbuf *buf;

   Elm_Gen_Item *it = (Elm_Gen_Item *)data;
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, NULL);

   buf = eina_strbuf_new();

   if (it->itc->func.text_get)
     {
        Eina_List *texts;
        const char *key;

        texts =
           elm_widget_stringlist_get(edje_object_data_get(VIEW(it), "texts"));

        EINA_LIST_FREE(texts, key)
          {
             char *s = it->itc->func.text_get
                ((void *)it->base.data, WIDGET(it), key);

             s = _elm_util_mkup_to_text(s);

             if (s)
               {
                  if (eina_strbuf_length_get(buf) > 0) eina_strbuf_append(buf, ", ");
                  eina_strbuf_append(buf, s);
                  free(s);
               }
          }
     }

   ret = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ret;
}

static char *
_access_state_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)data;
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, NULL);

   if (it->base.disabled)
     return strdup(E_("State: Disabled"));

   return NULL;
}

static Eina_Bool
_elm_util_freeze_events_get(const Evas_Object *obj)
{
   Evas_Object *parent = (Evas_Object *) obj;
   while (parent)
     {
        if (evas_object_freeze_events_get(parent)) return EINA_TRUE;
        parent = evas_object_smart_parent_get(parent);
     }
   return EINA_FALSE;
}

static void
_access_activate_cb(void *data,
                    Evas_Object *part_obj EINA_UNUSED,
                    Elm_Object_Item *item EINA_UNUSED)
{
   Elm_Gen_Item *it = data;

   if (!_elm_util_freeze_events_get(WIDGET(it)))
     _item_select(it);
}

static void
_access_widget_item_register(Elm_Gen_Item *it)
{
   Elm_Access_Info *ai;
   Eina_List *orders = NULL;
   Eina_List *l = NULL;
   Evas_Object *c, *child;
   Eina_List *listn, *ln;
   Eina_List *texts, *contents;

   if (!_elm_config->access_mode) return;

   /* access: unregister item which have no text and content */
   texts  = elm_widget_stringlist_get(edje_object_data_get(VIEW(it), "texts"));
   contents  = elm_widget_stringlist_get(edje_object_data_get(VIEW(it),
                                                              "contents"));
   if (!texts && !contents) return;
   eina_list_free(texts);
   eina_list_free(contents);

   _elm_access_widget_item_unregister((Elm_Widget_Item *)it);
   // FIXME: if item->view is not it->item->deco_all_view!!
   if (it->deco_all_view)
     {
        Evas_Object *acc = elm_access_object_register
           (it->deco_all_view, WIDGET(it));
        it->base.access_obj = acc;
     }
   else
     {
        _elm_access_widget_item_register((Elm_Widget_Item *)it);
     }

   ai = _elm_access_info_get(it->base.access_obj);
   _elm_access_callback_set(ai, ELM_ACCESS_INFO, _access_info_cb, it);
   _elm_access_callback_set(ai, ELM_ACCESS_STATE, _access_state_cb, it);
   _elm_access_activate_callback_set(ai, _access_activate_cb, it);

   elm_object_item_access_order_unset((Elm_Object_Item *)it);
   if (it->flipped)
     {
        EINA_LIST_FOREACH(GL_IT(it)->flip_content_objs, l, c)
          {
             if (elm_widget_can_focus_get(c))
               {
                  orders = eina_list_append(orders, c);
               }
          }
     }
   else
     {
        if (it->deco_all_view)
          {
             EINA_LIST_FOREACH(GL_IT(it)->deco_all_contents, l, c)
               {
                  if (elm_widget_can_focus_get(c))
                    {
                       orders = eina_list_append(orders, c);
                    }
               }
          }
        EINA_LIST_FOREACH(it->content_objs, l, c)
          {
             if (elm_widget_can_focus_get(c))
               {
                  /*FIXME The below change is done to push datetime's access objects
                    like date field and time field in to genlist focus list to get focus.*/
                  listn = elm_widget_can_focus_child_list_get(c);
                  if (listn)
                    {
                       EINA_LIST_FOREACH(listn, ln, child)
                         {
                            orders = eina_list_append(orders, child);
                         }
                    }
                  else
                    {
                       orders = eina_list_append(orders, c);
                    }
               }
          }
     }
   if (orders)
      elm_object_item_access_order_set((Elm_Object_Item *)it, orders);
}

static void
_item_unrealize(Elm_Gen_Item *it,
                Eina_Bool calc)
{
   Evas_Object *content;

   if (!it->realized) return;

   if (GL_IT(it)->wsd->reorder.it == it)
     {
        WRN("reordering item should not be unrealized");
     }

   if (!calc)
     {
        evas_object_smart_callback_call(WIDGET(it), SIG_UNREALIZED, it);
        _elm_access_widget_item_unregister((Elm_Widget_Item *)it);
        elm_object_item_access_order_unset((Elm_Object_Item *)it);
     }

   Eina_List *l;
   Elm_Widget_Item_Signal_Data *wisd;
   EINA_LIST_FOREACH(((Elm_Widget_Item *)it)->signals, l, wisd)
     {
        _elm_widget_item_signal_callback_del
           ((Elm_Widget_Item *)it, wisd->emission, wisd->source, wisd->func);
     }

   _decorate_item_unrealize(it);
   _decorate_all_item_unrealize(it);

   _item_edje_callbacks_del(it);
   _item_mouse_callbacks_del(it, VIEW(it));

   EINA_LIST_FREE(GL_IT(it)->flip_content_objs, content)
    evas_object_del(content);
   if (it->spacer) evas_object_del(it->spacer);
   _view_clear(VIEW(it), &(it->content_objs));

   if (GL_IT(it)->highlight_timer)
     {
        ecore_timer_del(GL_IT(it)->highlight_timer);
        GL_IT(it)->highlight_timer = NULL;
     }
   if (it->long_timer)
     {
        ecore_timer_del(it->long_timer);
        it->long_timer = NULL;
     }
   /* trackt */
   elm_widget_item_track_cancel(it);
   _item_cache_push(it);
   it->realized = EINA_FALSE;
}

static void
_item_block_unrealize(Item_Block *itb)
{
   Elm_Gen_Item *it;
   const Eina_List *l;

   // Do not check itb->realized
   // because it->item->block can be changed

   EINA_LIST_FOREACH(itb->items, l, it)
     {
        if (!((GL_IT(it)->wsd->reorder_mode) && (GL_IT(it)->wsd->reorder.it == it))
               && !it->flipped)
          _item_unrealize(it, EINA_FALSE);
     }
   itb->realized = EINA_FALSE;
}

static void
_item_update(Elm_Gen_Item *it)
{
   Evas_Object *c;
   if (!it->realized) return;

   _view_clear(VIEW(it), &(it->content_objs));
   EINA_LIST_FREE(GL_IT(it)->flip_content_objs, c)
     evas_object_del(c);
   _view_clear(GL_IT(it)->deco_it_view, &(GL_IT(it)->deco_it_contents));
   _view_clear(it->deco_all_view, &(GL_IT(it)->deco_all_contents));

   _view_inflate(VIEW(it), it, &(it->content_objs));
   if (it->flipped)
     {
        GL_IT(it)->flip_content_objs =
           _item_content_realize(it, VIEW(it), GL_IT(it)->flip_content_objs,
                                 "flips", NULL);
        edje_object_signal_emit(VIEW(it), SIGNAL_FLIP_ENABLED, "elm");
     }
   if (GL_IT(it)->wsd->decorate_all_mode)
     _view_inflate(it->deco_all_view, it, &(GL_IT(it)->deco_all_contents));
   else if (GL_IT(it)->deco_it_view)
     _view_inflate(GL_IT(it)->deco_it_view, it, &(GL_IT(it)->deco_it_contents));

   if (it->selected)
      evas_object_smart_callback_call(WIDGET(it), SIG_HIGHLIGHTED, it);
}

static void
_item_block_calc(Item_Block *itb)
{
   const Eina_List *l;
   Elm_Gen_Item *it;
   Evas_Coord minw = 9999999, minh = 0;

   if (itb->calc_done) return;

   EINA_LIST_FOREACH(itb->items, l, it)
     {
        if (it->item->hide) continue;
        if (GL_IT(it)->updateme)
          {
             if (it->realized)
               {
                  _item_update(it);
               }
             GL_IT(it)->calc_done = EINA_FALSE;
             GL_IT(it)->updateme = EINA_FALSE;
          }
        if (!GL_IT(it)->queued) _item_calc(it);

        it->x = 0;
        it->y = minh;
        if (minw > GL_IT(it)->minw) minw = GL_IT(it)->minw;
        minh += GL_IT(it)->minh;
        if (GL_IT(it)->is_prepend)
          {
             itb->sd->comp_y += GL_IT(it)->minh;
             GL_IT(it)->is_prepend = EINA_FALSE;
          }
     }
   itb->minw = minw;
   itb->minh = minh;
   itb->calc_done = EINA_TRUE;
   itb->position_update = EINA_FALSE;
}

static void
_calc(void *data)
{
   Elm_Genlist_Smart_Data *sd = data;
   Item_Block *itb;
   Evas_Coord minw = 99999999, minh = 0, vw, vh;

   if (sd->calc_done) return;

   EINA_INLIST_FOREACH(sd->blocks, itb)
     {
        _item_block_calc(itb);
        itb->x = 0;
        itb->y = minh;
        if (minw > itb->minw) minw = itb->minw;
        minh += itb->minh;
     }

   eo_do(sd->obj, elm_scrollable_interface_content_viewport_size_get(&vw, &vh));
   if ((sd->mode == ELM_LIST_COMPRESS) && (minw > vw))
      minw = vw;
   else if (minw < vw) minw = vw;

   if ((minw != sd->minw) || (minh != sd->minh))
     {
        if (!sd->queue && (minh != sd->minh))
          evas_object_smart_callback_call(sd->obj, SIG_LOADED, NULL);

        sd->minw = minw;
        sd->minh = minh;
        sd->processed_sizes = 0;
        elm_layout_sizing_eval(sd->obj);
        evas_object_smart_callback_call(sd->pan_obj, "changed", NULL);
     }
   sd->dir = 0;
   sd->calc_done = EINA_TRUE;
}

static void
_item_position(Elm_Gen_Item *it,
               Evas_Object *view,
               Evas_Coord it_x,
               Evas_Coord it_y)
{
   if (!it) return;
   if (!view) return;
   evas_object_resize(view, GL_IT(it)->w, GL_IT(it)->h);
   evas_object_move(view, it_x, it_y);
   evas_object_show(view);
}

static void
_item_all_position(Elm_Gen_Item *it,
                   Evas_Coord it_x,
                   Evas_Coord it_y)
{
   if (!it) return;

   if (it->deco_all_view)
     _item_position(it, it->deco_all_view, it_x, it_y);
   else if (GL_IT(it)->deco_it_view)
     _item_position(it, GL_IT(it)->deco_it_view, it_x, it_y);
   else
     _item_position(it, VIEW(it), it_x, it_y);
}

static void
_item_sub_items_clear(Elm_Gen_Item *it)
{
   Eina_List *l, *ll;
   Elm_Object_Item *it2;

   EINA_LIST_FOREACH_SAFE(GL_IT(it)->items, l, ll, it2)
     elm_widget_item_del(it2);
}

static void
_decorate_item_unrealize(Elm_Gen_Item *it)
{
   if (!GL_IT(it)->deco_it_view) return;

   edje_object_part_unswallow(GL_IT(it)->deco_it_view, VIEW(it));
   _view_clear(GL_IT(it)->deco_it_view, &(GL_IT(it)->deco_it_contents));
   evas_object_del(GL_IT(it)->deco_it_view);
   GL_IT(it)->deco_it_view = NULL;

   _item_mouse_callbacks_add(it, VIEW(it));
   evas_object_smart_member_add(VIEW(it), GL_IT(it)->wsd->pan_obj);
}

static void
_decorate_all_item_realize(Elm_Gen_Item *it, Eina_Bool effect)
{
   if (it->deco_all_view || !it->itc->decorate_all_item_style) return;

   // Before adding & swallowing, delete it from smart member
   evas_object_smart_member_del(VIEW(it));

   it->deco_all_view = _view_create(it, it->itc->decorate_all_item_style);
   _view_inflate(it->deco_all_view, it, &(GL_IT(it)->deco_all_contents));
   edje_object_part_swallow
      (it->deco_all_view, "elm.swallow.decorate.content", VIEW(it));
   evas_object_show(it->deco_all_view);

   if (elm_widget_item_disabled_get(it))
     edje_object_signal_emit(it->deco_all_view, SIGNAL_DISABLED, "elm");
   if (it->selected)
     edje_object_signal_emit(it->deco_all_view, SIGNAL_HIGHLIGHTED, "elm");
   if (GL_IT(it)->wsd->reorder_mode)
     edje_object_signal_emit
       (it->deco_all_view, SIGNAL_REORDER_MODE_SET, "elm");

   if (effect)
     {
        edje_object_signal_emit(VIEW(it),
                                SIGNAL_DECORATE_ENABLED_EFFECT, "elm");
        edje_object_signal_emit(it->deco_all_view,
                                SIGNAL_DECORATE_ENABLED_EFFECT, "elm");
     }
   else
     {
        edje_object_signal_emit(VIEW(it), SIGNAL_DECORATE_ENABLED, "elm");
        edje_object_signal_emit(it->deco_all_view,
                                SIGNAL_DECORATE_ENABLED,"elm");
     }
   if (it->flipped)
     edje_object_signal_emit(it->deco_all_view, SIGNAL_FLIP_ENABLED, "elm");

   _item_mouse_callbacks_add(it, it->deco_all_view);
   _item_mouse_callbacks_del(it, VIEW(it));
}

static void
_expand_toggle_signal_cb(void *data,
                         Evas_Object *obj EINA_UNUSED,
                         const char *emission EINA_UNUSED,
                         const char *source EINA_UNUSED)
{
   Elm_Gen_Item *it = data;

   if (GL_IT(it)->expanded)
     evas_object_smart_callback_call(WIDGET(it), SIG_CONTRACT_REQUEST, it);
   else
     evas_object_smart_callback_call(WIDGET(it), SIG_EXPAND_REQUEST, it);
}

static void
_expand_signal_cb(void *data,
                  Evas_Object *obj EINA_UNUSED,
                  const char *emission EINA_UNUSED,
                  const char *source EINA_UNUSED)
{
   Elm_Gen_Item *it = data;

   if (!GL_IT(it)->expanded)
     evas_object_smart_callback_call(WIDGET(it), SIG_EXPAND_REQUEST, it);
}

static void
_contract_signal_cb(void *data,
                    Evas_Object *obj EINA_UNUSED,
                    const char *emission EINA_UNUSED,
                    const char *source EINA_UNUSED)
{
   Elm_Gen_Item *it = data;

   if (GL_IT(it)->expanded)
     evas_object_smart_callback_call(WIDGET(it), SIG_CONTRACT_REQUEST, it);
}

static void
_item_mouse_callbacks_add(Elm_Gen_Item *it,
                          Evas_Object *view)
{
   evas_object_event_callback_add
     (view, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, it);
   evas_object_event_callback_add
     (view, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, it);
   evas_object_event_callback_add
     (view, EVAS_CALLBACK_MOUSE_MOVE, _item_mouse_move_cb, it);
}

static void
_item_mouse_callbacks_del(Elm_Gen_Item *it,
                          Evas_Object *view)
{
   evas_object_event_callback_del_full
     (view, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, it);
   evas_object_event_callback_del_full
     (view, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, it);
   evas_object_event_callback_del_full
     (view, EVAS_CALLBACK_MOUSE_MOVE, _item_mouse_move_cb, it);

}

static void
_item_edje_callbacks_add(Elm_Gen_Item *it)
{
   edje_object_signal_callback_add
      (VIEW(it), "elm,action,expand,toggle", "elm",
       _expand_toggle_signal_cb, it);
   edje_object_signal_callback_add
      (VIEW(it), "elm,action,expand", "elm", _expand_signal_cb, it);
   edje_object_signal_callback_add
      (VIEW(it), "elm,action,contract", "elm", _contract_signal_cb, it);
}

static void
_item_edje_callbacks_del(Elm_Gen_Item *it)
{
   edje_object_signal_callback_del_full(VIEW(it), "elm,action,expand,toggle",
                                        "elm", _expand_toggle_signal_cb, it);
   edje_object_signal_callback_del_full(VIEW(it), "elm,action,expand", "elm",
                                        _expand_signal_cb, it);
   edje_object_signal_callback_del_full(VIEW(it), "elm,action,contract", "elm",
                                        _contract_signal_cb, it);
}


static void
_item_realize(Elm_Gen_Item *it,
              Eina_Bool calc)
{
   const char *treesize;

   if (it->realized) return;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if (!_item_cache_pop(it))
     {
        VIEW(it) = _view_create(it, it->itc->item_style);
     }
   treesize = edje_object_data_get(VIEW(it), "treesize");
   if (treesize)
     {
        int tsize = atoi(treesize);
        if ((tsize > 0) &&
            it->parent &&
            (GL_IT(it->parent)->type == ELM_GENLIST_ITEM_TREE))
          {
             it->spacer =
                evas_object_rectangle_add(evas_object_evas_get(WIDGET(it)));
             evas_object_color_set(it->spacer, 0, 0, 0, 0);
             evas_object_size_hint_min_set
                (it->spacer, (GL_IT(it)->expanded_depth * tsize) *
                 elm_config_scale_get(), 1);
             edje_object_part_swallow(VIEW(it), "elm.swallow.pad", it->spacer);
          }
     }
   _view_inflate(VIEW(it), it, &it->content_objs);
   _item_mouse_callbacks_add(it, VIEW(it));
   // Expand can be supported by only VIEW(it)
   _item_edje_callbacks_add(it);

   if (elm_widget_item_disabled_get(it))
     edje_object_signal_emit(VIEW(it), SIGNAL_DISABLED, "elm");
   if (it->selected)
     {
        edje_object_signal_emit(VIEW(it), SIGNAL_HIGHLIGHTED, "elm");
        evas_object_smart_callback_call(WIDGET(it), SIG_HIGHLIGHTED, it);
     }
   if (GL_IT(it)->expanded)
     edje_object_signal_emit(VIEW(it), SIGNAL_EXPANDED, "elm");
   if (sd->reorder_mode)
     edje_object_signal_emit(VIEW(it), SIGNAL_REORDER_MODE_SET, "elm");
#if TIZEN_ONLY
   if (GL_IT(it)->expanded_depth > 0)
     edje_object_signal_emit(VIEW(it), SIGNAL_BG_CHANGE, "elm");
#endif
   if (it->flipped)
     {
        GL_IT(it)->flip_content_objs =
           _item_content_realize(it, VIEW(it), GL_IT(it)->flip_content_objs,
                                 "flips", NULL);
        edje_object_signal_emit(VIEW(it), SIGNAL_FLIP_ENABLED, "elm");
     }

   if (_focus_enabled(WIDGET(it)) && (it == sd->focused_item))
     {
        edje_object_signal_emit(VIEW(it), SIGNAL_FOCUSED, "elm");
     }
   if (sd->decorate_all_mode && it->itc->decorate_all_item_style)
     {
        _decorate_all_item_realize(it, EINA_FALSE);
     }
   else if (it->decorate_it_set && it->itc->decorate_item_style &&
            (sd->mode_item == it))
     {
        _decorate_item_realize(it);
     }

   it->realized = EINA_TRUE;
   if (!calc)
     {
        _elm_genlist_item_index_update(it);

        if (it->tooltip.content_cb)
          {
             elm_widget_item_tooltip_content_cb_set
                (it, it->tooltip.content_cb, it->tooltip.data, NULL);
             elm_widget_item_tooltip_style_set(it, it->tooltip.style);
             elm_widget_item_tooltip_window_mode_set(it, it->tooltip.free_size);
          }
        if (it->mouse_cursor)
           elm_widget_item_cursor_set(it, it->mouse_cursor);

        // Register accessibility before realized callback
        // because user can customize accessibility.
        _access_widget_item_register(it);
        evas_object_smart_callback_call(WIDGET(it), SIG_REALIZED, it);
     }
   edje_object_message_signal_process(VIEW(it));
   if (it->deco_all_view)
      edje_object_message_signal_process(it->deco_all_view);
}

static Eina_Bool
_reorder_anim(void *data)
{
   Eina_List *l;
   Elm_Gen_Item *it;
   Elm_Genlist_Smart_Data *sd = data;
   Evas_Coord oy, r_y, r_y_scrl, r_y_scrl_max;
   evas_object_geometry_get(sd->pan_obj, NULL, &oy, NULL, NULL);

   r_y = sd->reorder.it->y + sd->reorder.it->item->block->y;
   r_y_scrl = sd->reorder.it->item->scrl_y;
   r_y_scrl_max = r_y_scrl + sd->reorder.it->item->h;

   EINA_LIST_FOREACH(sd->reorder.move_items, l, it)
     {
        Evas_Coord it_y, it_y_scrl_center;
        it_y = it->y + GL_IT(it)->block->y;
        it_y_scrl_center = GL_IT(it)->scrl_y + (GL_IT(it)->h / 2);

        if ((sd->reorder.dir == -1) && (r_y_scrl <= it_y_scrl_center))
           GL_IT(it)->reorder_offset += REORDER_ANIM_OFFSET;
        else if ((sd->reorder.dir == 1) && (r_y_scrl_max >= it_y_scrl_center))
           GL_IT(it)->reorder_offset -= REORDER_ANIM_OFFSET;

        if (r_y > it_y)
          {
             if (sd->reorder.it->item->h < GL_IT(it)->reorder_offset)
               GL_IT(it)->reorder_offset = sd->reorder.it->item->h;
             else if (0 > GL_IT(it)->reorder_offset)
               GL_IT(it)->reorder_offset = 0;
          }
        else if (r_y < it_y)
          {
             if (-(sd->reorder.it->item->h) > GL_IT(it)->reorder_offset)
               GL_IT(it)->reorder_offset = -(sd->reorder.it->item->h);
             else if (0 < GL_IT(it)->reorder_offset)
               GL_IT(it)->reorder_offset = 0;
          }
     }
   if (!sd->reorder.move_items)
     {
        sd->reorder.anim = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   evas_object_smart_changed(sd->pan_obj);
   return ECORE_CALLBACK_RENEW;
}

static int
_reorder_space_get(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   Evas_Coord oy, r_y, it_y, r_y_scrl, r_y_scrl_max, it_y_center;
   evas_object_geometry_get(sd->pan_obj, NULL, &oy, NULL, NULL);

   if (GL_IT(it)->type == ELM_GENLIST_ITEM_GROUP) return 0;
   if (sd->reorder.it->parent != it->parent) return 0;

   r_y = sd->reorder.it->y + sd->reorder.it->item->block->y;
   it_y = it->y + GL_IT(it)->block->y;

   r_y_scrl = sd->reorder.it->item->scrl_y;
   r_y_scrl_max = r_y_scrl + sd->reorder.it->item->h;
   it_y_center = it_y + (GL_IT(it)->h / 2) - sd->pan_y + oy;

   if ((r_y > it_y) && (r_y_scrl <= it_y_center))
     {
        if (!eina_list_data_find(sd->reorder.move_items, it))
           sd->reorder.move_items = eina_list_append(sd->reorder.move_items, it);
        if (!sd->reorder.anim)
           sd->reorder.anim = ecore_animator_add(_reorder_anim,
                                                 GL_IT(it)->wsd);
     }
   else if ((r_y < it_y) && (r_y_scrl_max >= it_y_center))
     {
        if (!eina_list_data_find(sd->reorder.move_items, it))
           sd->reorder.move_items = eina_list_append(sd->reorder.move_items, it);
        if (!sd->reorder.anim)
           sd->reorder.anim = ecore_animator_add(_reorder_anim,
                                                 GL_IT(it)->wsd);
     }
   return GL_IT(it)->reorder_offset;
}

static void
_reorder_calc(Elm_Gen_Item *it, Elm_Genlist_Smart_Data *sd)
{
   Elm_Gen_Item *git = NULL;
   Elm_Gen_Item *ngit = NULL;

   Evas_Coord ox, oy, ow, oh;
   evas_object_geometry_get(sd->pan_obj, &ox, &oy, &ow, &oh);

   // Do not cross the parent group items and
   // Do not exceeds viewport
   git = it->parent;
   ngit = eina_list_data_get(eina_list_next
                             (eina_list_data_find_list
                              (sd->group_items, git)));
   if (git && (GL_IT(it)->scrl_y <= (GL_IT(git)->scrl_y + GL_IT(git)->h)))
     {
        GL_IT(it)->scrl_y = GL_IT(git)->scrl_y + GL_IT(git)->h;
     }
   else if (ngit && (GL_IT(it)->scrl_y >= (GL_IT(ngit)->scrl_y - GL_IT(it)->h)))
     {
        GL_IT(it)->scrl_y = GL_IT(ngit)->scrl_y - GL_IT(it)->h;
     }

   if (GL_IT(it)->scrl_y < oy)
     {
        GL_IT(it)->scrl_y = oy;
        // Move faster when reaching edges
        eo_do(sd->obj,
              elm_scrollable_interface_content_region_show(sd->pan_x,
                                                           sd->pan_y - REORDER_FASTER,
                                                           ow, oh));
     }
   else if (GL_IT(it)->scrl_y + GL_IT(it)->h > oy + oh)
     {
        GL_IT(it)->scrl_y = oy + oh - GL_IT(it)->h;
        // Move faster when reaching edges
        eo_do(sd->obj,
              elm_scrollable_interface_content_region_show(sd->pan_x,
                                                           sd->pan_y + REORDER_FASTER,
                                                           ow, oh));
     }

   _item_all_position(it, GL_IT(it)->scrl_x - REORDERED_ITEM_OFFSET,
                      GL_IT(it)->scrl_y - REORDERED_ITEM_OFFSET);
   if (it->deco_all_view)
      evas_object_raise(it->deco_all_view);
   else if (GL_IT(it)->deco_it_view)
      evas_object_raise(GL_IT(it)->deco_it_view);
   else
      evas_object_raise(VIEW(it));
}


static void
_item_block_realize(Item_Block *itb, Eina_Bool force)
{
   Elm_Genlist_Smart_Data *sd = itb->sd;
   Elm_Gen_Item *it;
   const Eina_List *l;
   Evas_Coord ox, oy, ow, oh, cvx, cvy, cvw, cvh;

   // Do not check itb->realized
   // because it->item->block can be changed

   evas_object_geometry_get(sd->pan_obj, &ox, &oy, &ow, &oh);
   evas_output_viewport_get(evas_object_evas_get(sd->obj),
                            &cvx, &cvy, &cvw, &cvh);

   if (_elm_config->access_mode)
     {
        // If access is on, realize more items (3 times)
        cvy = cvy - cvh;
        cvh = cvh * 3;
     }

   if (sd->reorder.it)  _reorder_calc(sd->reorder.it, sd);
   EINA_LIST_FOREACH(itb->items, l, it)
     {
        if (sd->reorder.it == it) continue;
        if (it->item->hide)
          {
             if (it->realized) evas_object_hide(VIEW(it));
             continue;
          }
        GL_IT(it)->scrl_x = it->x + itb->x - sd->pan_x + ox;
        GL_IT(it)->scrl_y = it->y + itb->y - sd->pan_y + oy;
        GL_IT(it)->w = sd->minw;
        GL_IT(it)->h = GL_IT(it)->minh;
        if (force || sd->realization_mode ||
            it->flipped ||
           (ELM_RECTS_INTERSECT(GL_IT(it)->scrl_x, GL_IT(it)->scrl_y,
                                 GL_IT(it)->w, GL_IT(it)->h,
                                 cvx, cvy, cvw, cvh)))
          {
             if (!it->realized) _item_realize(it, EINA_FALSE);
             if (sd->reorder.it)
                GL_IT(it)->scrl_y += _reorder_space_get(it);

             _item_all_position(it, GL_IT(it)->scrl_x, GL_IT(it)->scrl_y);
          }
        else if (it->realized)
          {
             _item_unrealize(it, EINA_FALSE);
          }
     }
   itb->realized = EINA_TRUE;
}

static void
_elm_genlist_pan_smart_pos_set(Eo *obj, void *_pd, va_list *list)
{
   Evas_Coord x = va_arg(*list, Evas_Coord);
   Evas_Coord y = va_arg(*list, Evas_Coord);

   Elm_Genlist_Pan_Smart_Data *psd = _pd;
   if ((x == psd->wsd->pan_x) && (y == psd->wsd->pan_y)) return;

   if (y > psd->wsd->pan_y) psd->wsd->dir = 1;
   else psd->wsd->dir = -1;
   psd->wsd->pan_x = x;
   psd->wsd->pan_y = y;

   evas_object_smart_changed(obj);
}

static void
_elm_genlist_pan_smart_pos_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Evas_Coord *x = va_arg(*list, Evas_Coord *);
   Evas_Coord *y = va_arg(*list, Evas_Coord *);
   Elm_Genlist_Pan_Smart_Data *psd = _pd;

   if (x) *x = psd->wsd->pan_x;
   if (y) *y = psd->wsd->pan_y;
}

static void
_elm_genlist_pan_smart_pos_max_get(Eo *obj, void *_pd, va_list *list)
{
   Evas_Coord ow, oh;
   Evas_Coord *x = va_arg(*list, Evas_Coord *);
   Evas_Coord *y = va_arg(*list, Evas_Coord *);

   Elm_Genlist_Pan_Smart_Data *psd = _pd;

   evas_object_geometry_get(obj, NULL, NULL, &ow, &oh);
   ow = psd->wsd->minw - ow;
   if (ow < 0) ow = 0;
   oh = psd->wsd->minh - oh;
   if (oh < 0) oh = 0;
   if (x) *x = ow;
   if (y) *y = oh;
}

static void
_elm_genlist_pan_smart_pos_min_get(Eo *obj EINA_UNUSED, void *_pd EINA_UNUSED, va_list *list)
{
   Evas_Coord *x = va_arg(*list, Evas_Coord *);
   Evas_Coord *y = va_arg(*list, Evas_Coord *);
   if (x) *x = 0;
   if (y) *y = 0;
}

static void
_elm_genlist_pan_smart_content_size_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Evas_Coord *w = va_arg(*list, Evas_Coord *);
   Evas_Coord *h = va_arg(*list, Evas_Coord *);
   Elm_Genlist_Pan_Smart_Data *psd = _pd;

   if (w) *w = psd->wsd->minw;
   if (h) *h = psd->wsd->minh;
}

static void
_elm_genlist_pan_smart_del(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Elm_Genlist_Pan_Smart_Data *psd = _pd;

   if (psd->resize_job)
     {
        ecore_job_del(psd->resize_job);
        psd->resize_job = NULL;
     }

   eo_do_super(obj, MY_PAN_CLASS, evas_obj_smart_del());
}

static void
_elm_genlist_pan_smart_move(Eo *obj, void *_pd, va_list *list)
{
   Elm_Genlist_Pan_Smart_Data *psd = _pd;
   va_arg(*list, Evas_Coord);
   va_arg(*list, Evas_Coord);

   psd->wsd->dir = 0;
   evas_object_smart_changed(obj);
}

static void
_elm_genlist_pan_smart_resize(Eo *obj, void *_pd, va_list *list)
{
   Evas_Coord w = va_arg(*list, Evas_Coord);
   Evas_Coord h = va_arg(*list, Evas_Coord);

   Elm_Genlist_Pan_Smart_Data *psd = _pd;

   if ((w > 1 || h > 1))
     {
        if (psd->wsd->queue && !psd->wsd->queue_idle_enterer)
          {
             psd->wsd->queue_idle_enterer = ecore_idle_enterer_add(_queue_idle_enter, psd->wsd);
          }
     }
   if (psd->wsd->mode == ELM_LIST_COMPRESS &&
       psd->wsd->prev_viewport_w != w)
     {
        Item_Block *itb;
        EINA_INLIST_FOREACH(psd->wsd->blocks, itb)
          {
             Eina_List *l;
             Elm_Gen_Item *it;
             EINA_LIST_FOREACH(itb->items, l, it)
               {
                  if (!GL_IT(it)->multiline) continue;
                  if (GL_IT(it)->wsd->realization_mode)
                    {
                       GL_IT(it)->calc_done = EINA_FALSE;
                       GL_IT(it)->block->calc_done = EINA_FALSE;
                    }
                  else _item_queue(it, NULL);
               }
          }
        psd->wsd->prev_viewport_w = w;
     }
   psd->wsd->calc_done = EINA_FALSE;
   evas_object_smart_changed(obj);
}

// FIXME: Remove below macro after opensource is patched
#define EINA_INLIST_REVERSE_FOREACH_FROM(list, it)                                \
   for (it = NULL, it = (list ? _EINA_INLIST_CONTAINER(it, list) : NULL); \
        it; it = (EINA_INLIST_GET(it)->prev ? _EINA_INLIST_CONTAINER(it, EINA_INLIST_GET(it)->prev) : NULL))

static void
_elm_genlist_pan_smart_calculate(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Evas_Coord ox, oy, ow, oh, cvx, cvy, cvw, cvh;
   Item_Block *itb;

   Elm_Genlist_Pan_Smart_Data *psd = _pd;

   _calc(psd->wsd);

   evas_object_geometry_get(obj, &ox, &oy, &ow, &oh);
   evas_output_viewport_get(evas_object_evas_get(obj), &cvx, &cvy, &cvw, &cvh);

   if (_elm_config->access_mode)
     {
        // If access is on, realize more items (3 times)
        cvy = cvy - cvh;
        cvh = cvh * 3;
     }

   // Belows are tweaks for performance
   // Block is not needed anymore because of below algorithm..
   // later, we should remove dirty block algorithm
   Eina_Inlist *start = NULL;
   Eina_List *realized_new = NULL;
   Eina_Bool flag = EINA_FALSE;
   if ((psd->wsd->blocks_realized) && (psd->wsd->dir == -1) &&
       (!_elm_config->access_mode))
     {
        start = EINA_INLIST_GET((Item_Block *)eina_list_data_get
                               (eina_list_last(psd->wsd->blocks_realized)));
        EINA_INLIST_REVERSE_FOREACH_FROM(start, itb)
          {
             Evas_Coord itb_x, itb_y, itb_w, itb_h;
             itb_x = itb->x - psd->wsd->pan_x + ox;
             itb_y = itb->y - psd->wsd->pan_y + oy;
             itb_w = psd->wsd->minw;
             itb_h = itb->minh;
             if (psd->wsd->realization_mode ||
                 ELM_RECTS_INTERSECT(itb_x, itb_y, itb_w, itb_h,
                                     cvx, cvy, cvw, cvh))
               {
                  flag = EINA_TRUE;
                  realized_new = eina_list_prepend(realized_new, itb);
                  _item_block_realize(itb, EINA_FALSE);
               }
             else
               {
                  _item_block_unrealize(itb);
                  if (flag) break;
               }
          }
     }
   else if ((psd->wsd->blocks_realized) && (psd->wsd->dir == 1) &&
            (!_elm_config->access_mode))
     {
        start = EINA_INLIST_GET((Item_Block *)eina_list_data_get
                                (psd->wsd->blocks_realized));
        EINA_INLIST_FOREACH(start, itb)
          {
             Evas_Coord itb_x, itb_y, itb_w, itb_h;
             itb_x = itb->x - psd->wsd->pan_x + ox;
             itb_y = itb->y - psd->wsd->pan_y + oy;
             itb_w = psd->wsd->minw;
             itb_h = itb->minh;
             if (psd->wsd->realization_mode ||
                 ELM_RECTS_INTERSECT(itb_x, itb_y, itb_w, itb_h,
                                     cvx, cvy, cvw, cvh))
               {
                  flag = EINA_TRUE;
                  realized_new = eina_list_append(realized_new, itb);
                  _item_block_realize(itb, EINA_FALSE);
               }
             else
               {
                  _item_block_unrealize(itb);
                  if (flag) break;
               }
          }
     }
   else
     {
        EINA_INLIST_FOREACH(psd->wsd->blocks, itb)
          {
             Evas_Coord itb_x, itb_y, itb_w, itb_h;
             itb_x = itb->x - psd->wsd->pan_x + ox;
             itb_y = itb->y - psd->wsd->pan_y + oy;
             itb_w = psd->wsd->minw;
             itb_h = itb->minh;
             if (psd->wsd->realization_mode ||
                 ELM_RECTS_INTERSECT(itb_x, itb_y, itb_w, itb_h,
                                     cvx, cvy, cvw, cvh))
               {
                  realized_new = eina_list_append(realized_new, itb);
                  _item_block_realize(itb, EINA_FALSE);
               }
             else
               {
                  _item_block_unrealize(itb);
               }
          }
     }
   eina_list_free(psd->wsd->blocks_realized);
   psd->wsd->blocks_realized = realized_new;

   if (psd->wsd->comp_y)
     {
        eo_do(obj, elm_scrollable_interface_content_region_show
              (psd->wsd->pan_x, psd->wsd->pan_y + psd->wsd->comp_y, ow, oh));
        psd->wsd->comp_y = 0;
     }
   else if (psd->wsd->expanded_item &&
            GL_IT(psd->wsd->expanded_item)->calc_done)
     {
        Eina_List *l;
        Evas_Coord size = GL_IT(psd->wsd->expanded_item)->minh;
        Elm_Gen_Item *tmp;
        EINA_LIST_FOREACH(psd->wsd->expanded_item->item->items, l, tmp)
          {
             size += GL_IT(tmp)->minh;
          }
        if (size >= oh)
           elm_genlist_item_bring_in
             ((Elm_Object_Item *)psd->wsd->expanded_item,
              ELM_GENLIST_ITEM_SCROLLTO_TOP);
        else
          {
             tmp = eina_list_data_get
                (eina_list_last(psd->wsd->expanded_item->item->items));
             elm_genlist_item_bring_in((Elm_Object_Item *)tmp,
                                       ELM_GENLIST_ITEM_SCROLLTO_IN);
          }
        psd->wsd->expanded_item = NULL;
     }
   else if (psd->wsd->show_item &&
            psd->wsd->show_item->item->calc_done &&
            psd->wsd->show_item->item->block->calc_done &&
            psd->wsd->calc_done)
     {
        Evas_Coord x, y;
        Elm_Gen_Item *it = psd->wsd->show_item;
        psd->wsd->show_item = NULL;
        x = it->x + GL_IT(it)->block->x;
        y = it->y + GL_IT(it)->block->y;

        switch (psd->wsd->scroll_to_type)
          {
           case ELM_GENLIST_ITEM_SCROLLTO_IN:
              if ((y >= psd->wsd->pan_y) &&
                  ((y + GL_IT(it)->minh) <= (psd->wsd->pan_y + oh)))
                {
                   y = psd->wsd->pan_y;
                }
              else if (psd->wsd->pan_y <= y)
                 y -= (oh - GL_IT(it)->minh);
              break;
           case ELM_GENLIST_ITEM_SCROLLTO_MIDDLE:
              y = y - (oh / 2) + (GL_IT(it)->h / 2);
              break;
           default:
              break;
          }
        if (psd->wsd->bring_in)
           eo_do(WIDGET(it), elm_scrollable_interface_region_bring_in(x, y, ow, oh));
        else
           eo_do(WIDGET(it), elm_scrollable_interface_content_region_show(x, y, ow, oh));
     }
}

static void
_elm_genlist_pan_destructor(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Elm_Genlist_Pan_Smart_Data *psd = _pd;
   eo_data_unref(psd->wobj, psd->wsd);
   eo_do_super(obj, MY_PAN_CLASS, eo_destructor());
}

static void
_genlist_pan_class_constructor(Eo_Class *klass)
{
      const Eo_Op_Func_Description func_desc[] = {
           EO_OP_FUNC(EO_BASE_ID(EO_BASE_SUB_ID_DESTRUCTOR), _elm_genlist_pan_destructor),

           EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_DEL), _elm_genlist_pan_smart_del),
           EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_RESIZE), _elm_genlist_pan_smart_resize),
           EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_MOVE), _elm_genlist_pan_smart_move),
           EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_CALCULATE), _elm_genlist_pan_smart_calculate),
           EO_OP_FUNC(ELM_OBJ_PAN_ID(ELM_OBJ_PAN_SUB_ID_POS_SET), _elm_genlist_pan_smart_pos_set),
           EO_OP_FUNC(ELM_OBJ_PAN_ID(ELM_OBJ_PAN_SUB_ID_POS_GET), _elm_genlist_pan_smart_pos_get),
           EO_OP_FUNC(ELM_OBJ_PAN_ID(ELM_OBJ_PAN_SUB_ID_POS_MAX_GET), _elm_genlist_pan_smart_pos_max_get),
           EO_OP_FUNC(ELM_OBJ_PAN_ID(ELM_OBJ_PAN_SUB_ID_POS_MIN_GET), _elm_genlist_pan_smart_pos_min_get),
           EO_OP_FUNC(ELM_OBJ_PAN_ID(ELM_OBJ_PAN_SUB_ID_CONTENT_SIZE_GET), _elm_genlist_pan_smart_content_size_get),
           EO_OP_FUNC_SENTINEL
      };
      eo_class_funcs_set(klass, func_desc);

      evas_smart_legacy_type_register(MY_PAN_CLASS_NAME_LEGACY, klass);
}

static const Eo_Class_Description _genlist_pan_class_desc = {
     EO_VERSION,
     MY_PAN_CLASS_NAME,
     EO_CLASS_TYPE_REGULAR,
     EO_CLASS_DESCRIPTION_OPS(NULL, NULL, 0),
     NULL,
     sizeof(Elm_Genlist_Pan_Smart_Data),
     _genlist_pan_class_constructor,
     NULL
};

EO_DEFINE_CLASS(elm_obj_genlist_pan_class_get, &_genlist_pan_class_desc, ELM_OBJ_PAN_CLASS, NULL);

static void _item_unfocused(Elm_Gen_Item *it)
{
   if (!it) return;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   if (!sd->focused_item) return;
   Evas_Object *content = sd->focused_content;

   if (content)
     {
        elm_object_focus_set(sd->obj, EINA_FALSE);
        elm_object_focus_set(sd->obj, EINA_TRUE);
        sd->focused_content = NULL;
     }
   edje_object_signal_emit
      (VIEW(sd->focused_item), SIGNAL_UNFOCUSED, "elm");
   if (sd->focused_item->deco_all_view)
      edje_object_signal_emit
         (sd->focused_item->deco_all_view, SIGNAL_UNFOCUSED, "elm");
   if (it == sd->focused_item) sd->focused_item = NULL;
#if TIZEN_ONLY
   evas_object_smart_callback_call(WIDGET(it), SIG_ITEM_UNFOCUSED, it);a
#endif
}

static void _item_focused(Elm_Gen_Item *it, Elm_Genlist_Item_Scrollto_Type type)
{
   if (!it) return;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if (sd->focused_item != it) _item_unfocused(sd->focused_item);

   if (_focus_enabled(sd->obj))
     {
        Evas_Coord x, y, w, h, sx, sy, sw, sh;
        evas_object_geometry_get(VIEW(it), &x, &y, &w, &h);
        evas_object_geometry_get(sd->obj, &sx, &sy, &sw, &sh);
        if ((x < sx) || (y < sy) || ((x + w) > (sx + sw)) || ((y + h) > (sy + sh)))
          {
             elm_genlist_item_bring_in((Elm_Object_Item *)it, type);
          }

        if (it->deco_all_view)
          edje_object_signal_emit
            (it->deco_all_view, SIGNAL_FOCUSED, "elm");
        else
          edje_object_signal_emit
            (VIEW(it), SIGNAL_FOCUSED, "elm");
     }
   sd->focused_item = it;
#if TIZEN_ONLY
   evas_object_smart_callback_call(WIDGET(it), SIG_ITEM_FOCUSED, it);
#endif
}

static void
_item_highlight(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if (!sd->highlight) return;
   if (elm_widget_item_disabled_get(it)) return;
   if ((sd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY ) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY ))
     return;
   if (it->highlighted) return;

   it->highlighted = EINA_TRUE;

   if (it->deco_all_view)
     {
        edje_object_signal_emit(it->deco_all_view, SIGNAL_HIGHLIGHTED, "elm");
        edje_object_message_signal_process(it->deco_all_view);
     }
   edje_object_signal_emit(VIEW(it), SIGNAL_HIGHLIGHTED, "elm");
   edje_object_message_signal_process(VIEW(it));

//***************** TIZEN Only
// If check is used on genlist item, highlight it also.
   Eina_List *l;
   Evas_Object *content;
   if (sd->decorate_all_mode)
     {
        EINA_LIST_FOREACH(GL_IT(it)->deco_all_contents, l, content)
          {
             const char *type = elm_widget_type_get(content);
             if (type && !strcmp(type, "elm_check"))
                elm_object_signal_emit(content, "elm,state,mouse,down", "elm");
          }
     }
   else
     {
        EINA_LIST_FOREACH(it->content_objs, l, content)
          {
             const char *type = elm_widget_type_get(content);
             if (type && !strcmp(type, "elm_check"))
                elm_object_signal_emit(content, "elm,state,mouse,down", "elm");
          }
     }
//****************************
   evas_object_smart_callback_call(WIDGET(it), SIG_HIGHLIGHTED, it);
}

static void
_item_unhighlight(Elm_Gen_Item *it)
{
   if (!it->highlighted) return;

   it->highlighted = EINA_FALSE;

   if (it->deco_all_view)
     edje_object_signal_emit(it->deco_all_view, SIGNAL_UNHIGHLIGHTED, "elm");
   edje_object_signal_emit(VIEW(it), SIGNAL_UNHIGHLIGHTED, "elm");
//******************** TIZEN Only
// If check is used on genlist item, unhighlight it also.
   Eina_List *l;
   Evas_Object *content;
   if (GL_IT(it)->wsd->decorate_all_mode)
     {
        EINA_LIST_FOREACH(GL_IT(it)->deco_all_contents, l, content)
          {
             const char *type = elm_widget_type_get(content);
             if (type && !strcmp(type, "elm_check"))
                elm_object_signal_emit(content, "elm,state,mouse,up", "elm");
          }
     }
   else
     {
        EINA_LIST_FOREACH(it->content_objs, l, content)
          {
             const char *type = elm_widget_type_get(content);
             if (type && !strcmp(type, "elm_check"))
                elm_object_signal_emit(content, "elm,state,mouse,up", "elm");
          }
     }
   //*******************************
   evas_object_smart_callback_call(WIDGET(it), SIG_UNHIGHLIGHTED, it);
}

static void
_item_unselect(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if (!it->selected) return;

   it->selected = EINA_FALSE;
   sd->selected = eina_list_remove(sd->selected, it);
   _item_unhighlight(it);
   evas_object_smart_callback_call(WIDGET(it), SIG_UNSELECTED, it);
}

static void
_item_select(Elm_Gen_Item *it)
{
   Evas_Object *obj = WIDGET(it);
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   if (elm_widget_item_disabled_get(it)) return;
   if ((sd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY))
     return;
   if (sd->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS &&
       it->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS &&
       it->selected)
      return;
   if (!sd->multi)
     {
        const Eina_List *l, *ll;
        Elm_Gen_Item *it2;
        EINA_LIST_FOREACH_SAFE(sd->selected, l, ll, it2)
          if (it2 != it) _item_unselect(it2);
     }

   if (!it->selected)
     {
        sd->selected = eina_list_append(sd->selected, it);
        it->selected = EINA_TRUE;
     }
   sd->last_selected_item = (Elm_Object_Item *)it;
   _item_highlight(it);
   _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_IN);

   // FIXME: after evas_object_raise, mouse event callbacks(ex, UP, DOWN)
   // can be called again eventhough already received it.
   const char *selectraise = edje_object_data_get(VIEW(it), "selectraise");
   if ((selectraise) && (!strcmp(selectraise, "on")))
     {
        if (it->deco_all_view) evas_object_raise(it->deco_all_view);
        else evas_object_raise(VIEW(it));
        if ((GL_IT(it)->group_item) && (GL_IT(it)->group_item->realized))
          evas_object_raise(GL_IT(it)->VIEW(group_item));
     }

   edje_object_signal_emit
	   (VIEW(sd->focused_item), SIGNAL_CLICKED, "elm");
   evas_object_ref(obj);
   if (it->func.func) it->func.func((void *)it->func.data, obj, it);
   if (EINA_MAGIC_CHECK((Elm_Widget_Item *)it, ELM_WIDGET_ITEM_MAGIC))
      evas_object_smart_callback_call(obj, SIG_SELECTED, it);
   evas_object_unref(obj);
}

static Eina_Bool
_highlight_timer(void *data)
{
   Elm_Gen_Item *it = data;
   GL_IT(it)->highlight_timer = NULL;
   _item_highlight(it);
   return EINA_FALSE;
}

static Eina_Bool
_select_timer(void *data)
{
   Elm_Gen_Item *it = data;
   GL_IT(it)->highlight_timer = NULL;

   if ((GL_IT(it)->wsd->select_mode == ELM_OBJECT_SELECT_MODE_ALWAYS) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_ALWAYS))
      _item_select(it);
   else
     {
        if (!it->selected) _item_select(it);
        else _item_unselect(it);
     }

   return EINA_FALSE;
}

static Evas_Object
*_item_focusable_content_search(Evas_Object *obj, Eina_List *l, int dir)
{
   if ((dir != 1) && (dir != -1)) return NULL;

   Evas_Object *next = NULL;

   while (l)
     {
        next = eina_list_data_get(l);
        if (next && (elm_widget_can_focus_get(next) &&
                   (evas_object_visible_get(next))))
           break;
        else if (next && (elm_widget_child_can_focus_get(next)))
          {
             if ((dir == 1) && (elm_widget_focus_next_get
                               (next, ELM_FOCUS_RIGHT, &next)))
                break;
             else if ((dir == -1) && (elm_widget_focus_next_get
                                     (next, ELM_FOCUS_LEFT, &next)))
                break;
          }

        next = NULL;
        if (dir == 1) l = eina_list_next(l);
        else l = eina_list_prev(l);
     }

     if (!next) next = obj;

   return next;
}

static Eina_Bool _item_focusable_search(Elm_Gen_Item **it, int dir)
{
   if (!(*it)) return EINA_FALSE;
   if ((dir != 1) && (dir != -1)) return EINA_FALSE;

   Elm_Gen_Item *tmp = *it;

   while (tmp)
     {
        if (!elm_object_item_disabled_get((Elm_Object_Item *)tmp) &&
           (!tmp->item->hide))
          {
             if ((tmp->select_mode == ELM_OBJECT_SELECT_MODE_DEFAULT) ||
                 (tmp->select_mode == ELM_OBJECT_SELECT_MODE_ALWAYS))
               {
                  *it = tmp;
                  return EINA_TRUE;
               }

             if ((tmp->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY) ||
                 (tmp->flipped))
               {
                  Evas_Object *obj = NULL;
                  Eina_List *contents = NULL;
                  if (tmp->flipped)
                    contents = GL_IT(tmp)->flip_content_objs;
                  else
                    {
                       contents = tmp->content_objs;
                       if (tmp->deco_all_view)
                         {
                            if (contents)
                              contents = GL_IT(tmp)->deco_all_contents;
                            else
                              contents = eina_list_merge(contents,
                                            GL_IT(tmp)->deco_all_contents);
                         }
                    }

                  if (dir == 1)
                    obj = _item_focusable_content_search(NULL, contents, 1);
                  else
                    {
                       contents = eina_list_last(contents);
                       obj = _item_focusable_content_search(NULL, contents, -1);
                    }

                  if (obj)
                    {
                       GL_IT(tmp)->wsd->focused_item = tmp;
                       GL_IT(tmp)->wsd->focused_content = obj;
                       elm_object_focus_set(obj, EINA_TRUE);
                       *it = tmp;
                       return EINA_FALSE;
                    }
               }
          }
        if (dir == 1)
          tmp = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(tmp)->next);
        else
          tmp = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(tmp)->prev);
     }

   *it = NULL;
   return EINA_FALSE;
}

static Eina_Bool _item_focus_next(Elm_Genlist_Smart_Data *sd, Focus_Dir dir)
{
   Elm_Gen_Item *it = NULL;
   Elm_Gen_Item *old_focused = sd->focused_item;
   Evas_Object *old_content = sd->focused_content;

   if (dir == FOCUS_DIR_DOWN || dir == FOCUS_DIR_UP)
     {
        Eina_Bool find_item;

        if (dir == FOCUS_DIR_DOWN)
          {
             if (sd->focused_item)
               {
                  it = ELM_GEN_ITEM_FROM_INLIST
                     (EINA_INLIST_GET(sd->focused_item)->next);
                  _item_unfocused(sd->focused_item);
               }
             else it = ELM_GEN_ITEM_FROM_INLIST(sd->items);
             find_item = _item_focusable_search(&it, 1);
          }
        else if (dir == FOCUS_DIR_UP)
          {
             if (sd->focused_item)
               {
                  it = ELM_GEN_ITEM_FROM_INLIST
                     (EINA_INLIST_GET(sd->focused_item)->prev);
                  _item_unfocused(sd->focused_item);
               }
             else it = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);
             find_item = _item_focusable_search(&it, -1);
          }

        if (!it)
          {
             if (old_focused)
               {
                  sd->focused_item = old_focused;
                  if (old_content)
                    {
                       sd->focused_content = old_content;
                       elm_object_focus_set(old_content, EINA_TRUE);
                    }
                  else
                    _item_focused(old_focused, ELM_GENLIST_ITEM_SCROLLTO_IN);
               }
             return EINA_FALSE;
          }
        else if (!find_item)
          return EINA_TRUE;

        _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
     }
   else if (dir == FOCUS_DIR_LEFT || dir == FOCUS_DIR_RIGHT)
     {
        Evas_Object *obj = NULL;
        Eina_List *contents = NULL;
        Eina_List *l = NULL;

        if (!sd->focused_item) return EINA_FALSE;
        if (sd->focused_item->flipped)
          contents = GL_IT(sd->focused_item)->flip_content_objs;
        else
          {
             contents = sd->focused_item->content_objs;
             if (sd->focused_item->deco_all_view)
               {
                  if (contents)
                    contents = GL_IT(sd->focused_item)->deco_all_contents;
                  else
                    contents = eina_list_merge(contents,
                                  GL_IT(sd->focused_item)->deco_all_contents);
               }
          }

        if (sd->focused_content)
          {
             l = eina_list_data_find_list(contents,
                                          sd->focused_content);
             obj = sd->focused_content;
          }

        if (dir == FOCUS_DIR_LEFT)
          {
             if ((l) && (eina_list_prev(l)))
               {
                  l = eina_list_prev(l);
                  obj = _item_focusable_content_search(obj, l, -1);
                  if (!obj) obj = sd->focused_content;
               }
             else if (!l)
               {
                  //search focused content is child of content
                  if (sd->focused_content)
                    l = eina_list_data_find_list(contents,
                                                 elm_widget_parent_get
                                                 (sd->focused_content));
                  if (!l) l = eina_list_last(contents);
                  obj = _item_focusable_content_search(obj, l, -1);
               }
             else obj = sd->focused_content;
          }
        else if (dir == FOCUS_DIR_RIGHT)
          {
             if ((l) && (eina_list_next(l)))
               {
                  l = eina_list_next(l);
                  obj = _item_focusable_content_search(obj, l, 1);
                  if (!obj) obj = sd->focused_content;
               }
             else if (!l)
               {
                  //search focused content is child of content
                  if (sd->focused_content)
                    l = eina_list_data_find_list(contents,
                                                 elm_widget_parent_get
                                                 (sd->focused_content));
                  if (!l) l = contents;
                  obj = _item_focusable_content_search(obj, l, 1);
               }
             else obj = sd->focused_content;
          }

        if (obj)
          {
             sd->focused_content = obj;
             elm_object_focus_set(obj, EINA_TRUE);
          }
        else
          {
             sd->focused_content = NULL;
             return EINA_FALSE;
          }
     }
   else return EINA_FALSE;

   return EINA_TRUE;
}

static Eina_Bool
_item_multi_select_up(Elm_Genlist_Smart_Data *sd)
{
   Elm_Object_Item *prev;

   if (!sd->selected) return EINA_FALSE;
   if (!sd->multi) return EINA_FALSE;

   prev = elm_genlist_item_prev_get(sd->last_selected_item);
   if (!prev) return EINA_TRUE;

   if (elm_genlist_item_selected_get(prev))
     {
        elm_genlist_item_selected_set(sd->last_selected_item, EINA_FALSE);
        sd->last_selected_item = prev;
        elm_genlist_item_show
          (sd->last_selected_item, ELM_GENLIST_ITEM_SCROLLTO_IN);
     }
   else
     {
        elm_genlist_item_selected_set(prev, EINA_TRUE);
        elm_genlist_item_show(prev, ELM_GENLIST_ITEM_SCROLLTO_IN);
     }
   return EINA_TRUE;
}

static Eina_Bool
_item_multi_select_down(Elm_Genlist_Smart_Data *sd)
{
   Elm_Object_Item *next;

   if (!sd->selected) return EINA_FALSE;
   if (!sd->multi) return EINA_FALSE;

   next = elm_genlist_item_next_get(sd->last_selected_item);
   if (!next) return EINA_TRUE;

   if (elm_genlist_item_selected_get(next))
     {
        elm_genlist_item_selected_set(sd->last_selected_item, EINA_FALSE);
        sd->last_selected_item = next;
        elm_genlist_item_show
          (sd->last_selected_item, ELM_GENLIST_ITEM_SCROLLTO_IN);
     }
   else
     {
        elm_genlist_item_selected_set(next, EINA_TRUE);
        elm_genlist_item_show(next, ELM_GENLIST_ITEM_SCROLLTO_IN);
     }

   return EINA_TRUE;
}

static Eina_Bool
_all_items_deselect(Elm_Genlist_Smart_Data *sd)
{
   if (!sd->selected) return EINA_FALSE;

   while (sd->selected)
     elm_genlist_item_selected_set(sd->selected->data, EINA_FALSE);

   return EINA_TRUE;
}

static Eina_Bool
_item_single_select_up(Elm_Genlist_Smart_Data *sd)
{
   Elm_Gen_Item *prev;

   if (!sd->selected)
     {
        prev = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);
        while (prev)
          prev = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(prev)->prev);
     }
   else
     prev = (Elm_Gen_Item *)elm_genlist_item_prev_get
         (sd->last_selected_item);

   if (!prev) return EINA_FALSE;

   _all_items_deselect(sd);

   elm_genlist_item_selected_set((Elm_Object_Item *)prev, EINA_TRUE);
   elm_genlist_item_show((Elm_Object_Item *)prev, ELM_GENLIST_ITEM_SCROLLTO_IN);
   return EINA_TRUE;
}

static Eina_Bool
_item_single_select_down(Elm_Genlist_Smart_Data *sd)
{
   Elm_Gen_Item *next;

   if (!sd->selected)
     {
        next = ELM_GEN_ITEM_FROM_INLIST(sd->items);
        while (next)
          next = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(next)->next);
     }
   else
     next = (Elm_Gen_Item *)elm_genlist_item_next_get
        (sd->last_selected_item);

   if (!next) return EINA_FALSE;

   _all_items_deselect(sd);

   elm_genlist_item_selected_set((Elm_Object_Item *)next, EINA_TRUE);
   elm_genlist_item_show
     ((Elm_Object_Item *)next, ELM_GENLIST_ITEM_SCROLLTO_IN);

   return EINA_TRUE;
}

static void
_elm_genlist_smart_event(Eo *obj, void *_pd, va_list *list)
{
   Evas_Object *src = va_arg(*list, Evas_Object *);
   Evas_Callback_Type type = va_arg(*list, Evas_Callback_Type);
   Evas_Event_Key_Down *ev = va_arg(*list, void *);
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = EINA_FALSE;
   (void) src;

   Evas_Coord x = 0;
   Evas_Coord y = 0;
   Evas_Coord v_w = 0;
   Evas_Coord v_h = 0;
   Evas_Coord step_x = 0;
   Evas_Coord step_y = 0;
   Evas_Coord page_x = 0;
   Evas_Coord page_y = 0;
   Evas_Coord pan_max_x = 0, pan_max_y = 0;

   if (elm_widget_disabled_get(obj)) return;
   if ((type != EVAS_CALLBACK_KEY_DOWN) && (type != EVAS_CALLBACK_KEY_UP))
   if (!sd->items) return;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return;
   if (elm_widget_disabled_get(obj)) return;
   if (!_focus_enabled(obj)) return;

   eo_do(obj,
         elm_scrollable_interface_content_pos_get(&x, &y),
         elm_scrollable_interface_step_size_get(&step_x, &step_y),
         elm_scrollable_interface_page_size_get(&page_x, &page_y),
         elm_scrollable_interface_content_viewport_size_get(&v_w, &v_h));

   if ((!strcmp(ev->key, "Left")) ||
       ((!strcmp(ev->key, "KP_Left")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        if (sd->select_on_focus_enabled) x -= step_x;
        else
          {
             if (_item_focus_next(sd, FOCUS_DIR_LEFT))
               {
                  ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
                  if (ret) *ret = EINA_TRUE;
                  return;
               }
             else return;
          }
     }
   else if ((!strcmp(ev->key, "Right")) ||
            ((!strcmp(ev->key, "KP_Right")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        if (sd->select_on_focus_enabled) x += step_x;
        else
          {
             if (_item_focus_next(sd, FOCUS_DIR_RIGHT))
               {
                  ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
                  if (ret) *ret = EINA_TRUE;
                  return;
               }
             else return;
          }
     }
   else if ((!strcmp(ev->key, "Up")) ||
            ((!strcmp(ev->key, "KP_Up")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;;
        if (sd->select_on_focus_enabled)
          {
             if (((evas_key_modifier_is_set(ev->modifiers, "Shift")) &&
                  (_item_multi_select_up(sd))) || (_item_single_select_up(sd)))
               {
                  ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
                  if (ret) *ret = EINA_TRUE;
                  return;
               }
          }
        else
          {
             if (_item_focus_next(sd, FOCUS_DIR_UP))
               {
                  ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
                  if (ret) *ret = EINA_TRUE;
                  return;
               }
             else
               return;
          }
     }
   else if ((!strcmp(ev->key, "Down")) ||
            ((!strcmp(ev->key, "KP_Down")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        if (sd->select_on_focus_enabled)
          {
             if (((evas_key_modifier_is_set(ev->modifiers, "Shift")) &&
                  (_item_multi_select_down(sd))) ||
                 (_item_single_select_down(sd)))
               {
                  ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
                  if (ret) *ret = EINA_TRUE;
                  return;
               }
          }
        else
          {
             if (_item_focus_next(sd, FOCUS_DIR_DOWN))
               {
                  ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
                  if (ret) *ret = EINA_TRUE;
                  return;
               }
             else return;
          }
     }
   else if ((!strcmp(ev->key, "Home")) ||
            ((!strcmp(ev->key, "KP_Home")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        if (sd->select_on_focus_enabled)
          {
             Elm_Object_Item *it = elm_genlist_first_item_get(obj);
             elm_genlist_item_bring_in(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
             elm_genlist_item_selected_set(it, EINA_TRUE);
             ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
          }
        else
          {
             _item_unfocused(sd->focused_item);
             _item_focus_next(sd, FOCUS_DIR_DOWN);
             ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
          }
        if (ret) *ret = EINA_TRUE;
        return;
     }
   else if ((!strcmp(ev->key, "End")) ||
            ((!strcmp(ev->key, "KP_End")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        if (sd->select_on_focus_enabled)
          {
             Elm_Object_Item *it = elm_genlist_last_item_get(obj);
             elm_genlist_item_bring_in(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
             elm_genlist_item_selected_set(it, EINA_TRUE);
          }
        else
          {
             _item_unfocused(sd->focused_item);
             sd->focused_item = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);
             _item_focus_next(sd, FOCUS_DIR_UP);
          }
        ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
        if (ret) *ret = EINA_TRUE;
        return;
     }
   else if ((!strcmp(ev->key, "Prior")) ||
            ((!strcmp(ev->key, "KP_Prior")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        Elm_Gen_Item *it = sd->focused_item;

        while (page_y <= v_h)
          {
              if (elm_genlist_item_prev_get((Elm_Object_Item *) it))
                it = (Elm_Gen_Item *) elm_genlist_item_prev_get((Elm_Object_Item *) it);
              else break;
              page_y += GL_IT(it)->minh;
          }
        if (_item_focusable_search(&it, -1))
           _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_TOP);
        else if (!it)
          {
             _item_focusable_search(&it, 1);
             _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_TOP);
          }

        ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
        if (ret) *ret = EINA_TRUE;
        return;
     }
   else if ((!strcmp(ev->key, "Next")) ||
            ((!strcmp(ev->key, "KP_Next")) && (!ev->string)))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        Elm_Gen_Item *it = sd->focused_item;

        page_y = GL_IT(it)->minh;

        while (page_y <= v_h)
          {
             if (elm_genlist_item_next_get((Elm_Object_Item *) it))
                it = (Elm_Gen_Item *) elm_genlist_item_next_get((Elm_Object_Item *) it);
             else break;
             page_y += GL_IT(it)->minh;
          }
        if (_item_focusable_search(&it, 1))
           _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_TOP);
        else if(!it)
          {
             _item_focusable_search(&it, -1);
             _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_TOP);
          }

        _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_TOP);

        ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
        if (ret) *ret = EINA_TRUE;
        return;
     }
   else if (!strcmp(ev->key, "Escape"))
     {
        if (type != EVAS_CALLBACK_KEY_DOWN) return;
        if (!_all_items_deselect(sd)) return;
        ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
        if (ret) *ret = EINA_TRUE;
        return;
     }
   else if (((!strcmp(ev->key, "Return")) ||
             (!strcmp(ev->key, "KP_Enter")) ||
             (!strcmp(ev->key, "space")))
            && (!sd->multi) && (sd->selected))
     {
        if (type == EVAS_CALLBACK_KEY_DOWN && !sd->key_down_item)
          {
             if (sd->focused_item)
               {
                  sd->key_down_item = sd->focused_item;

                  edje_object_signal_emit
                     (VIEW(sd->focused_item), SIGNAL_UNFOCUSED, "elm");
                  if (sd->focused_item->deco_all_view)
                    edje_object_signal_emit
                       (sd->focused_item->deco_all_view, SIGNAL_UNFOCUSED, "elm");

                  _item_highlight(sd->key_down_item);
                  if (sd->key_down_item->long_timer)
                    ecore_timer_del(sd->key_down_item->long_timer);
                  sd->key_down_item->long_timer = ecore_timer_add
                                             (sd->longpress_timeout,
                                             _long_press_cb, sd->key_down_item);
                  ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
                  evas_object_smart_callback_call(obj, SIG_ACTIVATED, sd->key_down_item);
                  if (ret) *ret = EINA_TRUE;
                  return;
               }
             else return;
          }
        else if (type == EVAS_CALLBACK_KEY_UP && sd->key_down_item)
          {
             edje_object_signal_emit
                (VIEW(sd->key_down_item), SIGNAL_FOCUSED, "elm");
             if (sd->key_down_item->deco_all_view)
                 edje_object_signal_emit
                    (sd->key_down_item->deco_all_view, SIGNAL_FOCUSED, "elm");

             if (sd->key_down_item->long_timer)
               ecore_timer_del(sd->key_down_item->long_timer);
             sd->key_down_item->long_timer = NULL;
             if (GL_IT(sd->key_down_item)->highlight_timer)
               ecore_timer_del(GL_IT(sd->key_down_item)->highlight_timer);
             GL_IT(sd->key_down_item)->highlight_timer = ecore_timer_add
                  (ITEM_SELECT_TIMER, _select_timer, sd->key_down_item);
             sd->key_down_item = NULL;
             ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
             if (ret) *ret = EINA_TRUE;
             return;
          }
        else return;
     }
   else return;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   eo_do(sd->pan_obj, elm_obj_pan_pos_max_get(&pan_max_x, &pan_max_y));
   if (x < 0) x = 0;
   if (x > pan_max_x) x = pan_max_x;
   if (y < 0) y = 0;
   if (y > pan_max_y) y = pan_max_y;

   eo_do(obj, elm_scrollable_interface_content_pos_set(x, y, EINA_TRUE));

   if (ret) *ret = EINA_TRUE;
}

/* This function disables the specific code of the layout sub object add.
 * Only the widget sub_object_add is called.
 */
static void
_elm_genlist_smart_layout_sub_object_add_enable(Eo *obj EINA_UNUSED, void *_pd EINA_UNUSED, va_list *list)
{
   Eina_Bool *enable = va_arg(*list, Eina_Bool *);
   *enable = EINA_FALSE;
}

static void
_elm_genlist_smart_sub_object_add(Eo *obj, void *_pd EINA_UNUSED, va_list *list)
{
   Evas_Object *sobj = va_arg(*list, Evas_Object *);
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   if (ret) *ret = EINA_FALSE;
   Eina_Bool int_ret = EINA_FALSE;

   /* skipping layout's code, which registers size hint changing
    * callback on sub objects. this is here because items'
    * content_get() routines may change hints on the objects after
    * creation, thus issuing TOO MANY sizing_eval()'s here. they are
    * not needed at here anyway, so let's skip listening to those
    * hints changes */
   eo_do_super(obj, MY_CLASS, elm_wdg_sub_object_add(sobj, &int_ret));
   if (!int_ret) return;

   if (ret) *ret = EINA_TRUE;
}

static void
_elm_genlist_smart_sub_object_del(Eo *obj, void *_pd, va_list *list)
{
   Evas_Object *sobj = va_arg(*list, Evas_Object *);
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   if (ret) *ret = EINA_FALSE;
   Eina_Bool int_ret = EINA_FALSE;

   Elm_Genlist_Smart_Data *sd = _pd;

   /* XXX: hack -- also skipping sizing recalculation on
    * sub-object-del. genlist's crazy code paths (like groups and
    * such) seem to issue a whole lot of deletions and Evas bitches
    * about too many recalculations */
   sd->on_sub_del = EINA_TRUE;

   eo_do_super(obj, MY_CLASS, elm_wdg_sub_object_del(sobj, &int_ret));
   if (!int_ret) return;

   sd->on_sub_del = EINA_FALSE;

   if (ret) *ret = EINA_TRUE;
}

static void
_elm_genlist_focus_highlight_show(void *data EINA_UNUSED,
                                  Evas_Object *obj,
                                  const char *emission EINA_UNUSED,
                                  const char *src EINA_UNUSED)
{
   ELM_GENLIST_DATA_GET(obj, sd);

   if (sd->focused_item)
     {
        if (!sd->focused_content)
          {
             Eina_Bool found = EINA_FALSE;
             Elm_Gen_Item *it = sd->focused_item;
             found = _item_focusable_search(&it, 1);
             if (found)
                _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
          }
        else elm_object_focus_set(sd->focused_content, EINA_TRUE);
     }
}

static void
_elm_genlist_focus_highlight_hide(void *data EINA_UNUSED,
                                  Evas_Object *obj,
                                  const char *emission EINA_UNUSED,
                                  const char *src EINA_UNUSED)
{
   ELM_GENLIST_DATA_GET(obj, sd);
   if (sd->focused_item)
     {
        // Do not use _item_unfocused because focus should be remained
        edje_object_signal_emit
           (VIEW(sd->focused_item), SIGNAL_UNFOCUSED, "elm");
        if (sd->focused_item->deco_all_view)
           edje_object_signal_emit
              (sd->focused_item->deco_all_view, SIGNAL_UNFOCUSED, "elm");
     }
}

static void
_elm_genlist_smart_on_focus(Eo *obj, void *_pd EINA_UNUSED, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   if (ret) *ret = EINA_FALSE;
   Eina_Bool int_ret = EINA_FALSE;
   Elm_Genlist_Smart_Data *sd = _pd;

   eo_do_super(obj, MY_CLASS, elm_wdg_on_focus(&int_ret));
   if (!int_ret) return;

   if ((sd->items) && (sd->selected) && (!sd->last_selected_item))
     sd->last_selected_item = eina_list_data_get(sd->selected);

   if (sd->select_on_focus_enabled)
     {
        if (ret) *ret = EINA_TRUE;
        return;
     }
   if (elm_widget_focus_get(obj))
     {
        if (_focus_enabled(obj))
          {
             if (sd->focused_item)
               {
                  if (!sd->focused_content)
                    {
                       Eina_Bool found = EINA_FALSE;
                       Elm_Gen_Item *it = sd->focused_item;
                       found = _item_focusable_search(&it, 1);
                       if (found)
                         _item_focused(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
                    }
               }
             else
               _item_focus_next(sd, FOCUS_DIR_DOWN);
          }
     }
   else
     {
        // when key down and not called key up
        // and focus is not on genlist, call select_timer forcely
        if (sd->key_down_item)
          {
             _select_timer(sd->key_down_item);
             sd->key_down_item = NULL;
          }

        if (sd->focused_item)
          {
             if (sd->focused_content)
               {
                  //FIXME: when genlist contents loose their focus,
                  //       focus unset should automatically work.
                  elm_object_focus_set(sd->focused_content, EINA_FALSE);
               }
             // Do not use _item_unfocused because focus should be remained
             edje_object_signal_emit
                (VIEW(sd->focused_item), SIGNAL_UNFOCUSED, "elm");
              if (sd->focused_item->deco_all_view)
                edje_object_signal_emit
                   (sd->focused_item->deco_all_view, SIGNAL_UNFOCUSED, "elm");
          }
        return;
     }
   if (ret) *ret = EINA_TRUE;
}

static Eina_Bool _elm_genlist_smart_focus_next_enable = EINA_FALSE;

static void
_elm_genlist_smart_focus_next_manager_is(Eo *obj EINA_UNUSED, void *_pd EINA_UNUSED, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   if (ret) *ret = _elm_genlist_smart_focus_next_enable;
}

static void
_elm_genlist_smart_focus_direction_manager_is(Eo *obj EINA_UNUSED, void *_pd EINA_UNUSED, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   if (ret) *ret = EINA_FALSE;
}

static void
_elm_genlist_smart_focus_next(Eo *obj, void *_pd EINA_UNUSED, va_list *list)
{
   Elm_Focus_Direction dir = va_arg(*list, Elm_Focus_Direction);
   Evas_Object **next = va_arg(*list, Evas_Object **);
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   if (ret) *ret = EINA_FALSE;
   Eina_Bool int_ret = EINA_FALSE;

   Eina_List *items = NULL;
   Item_Block *itb = NULL;
   Elm_Object_Item *it = NULL;

   Elm_Genlist_Smart_Data *sd = _pd;

   if (!_elm_config->access_mode) return;

   if (!elm_widget_highlight_get(obj))
     {
        if (ELM_FOCUS_PREVIOUS == dir)
          {
             it = elm_genlist_last_item_get(obj);
             itb = EINA_INLIST_CONTAINER_GET(sd->blocks->last, Item_Block);
          }
        else if (ELM_FOCUS_NEXT == dir)
          {
             it = elm_genlist_first_item_get(obj);
             itb = EINA_INLIST_CONTAINER_GET(sd->blocks, Item_Block);
          }
        else return;

        if (it && itb && itb->calc_done)
          {
             _item_block_realize(itb, EINA_TRUE);
             elm_genlist_item_show(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
          }
        else return;
     }

   // FIXME: do not use realized items get
   // because of above forcing realization case.
   EINA_INLIST_FOREACH(sd->blocks, itb)
     {
        Eina_List *l;
        if (!itb->realized) continue;

        EINA_LIST_FOREACH(itb->items, l, it)
          {
             Eina_List *ll;
             Evas_Object *c;
             Eina_List *orders;
             if (!((Elm_Gen_Item *)it)->realized) continue;

             if (it->base.access_obj)
                items = eina_list_append(items, it->base.access_obj);
             else
                items = eina_list_append(items, VIEW(it));

             orders = (Eina_List *)elm_object_item_access_order_get(it);
             EINA_LIST_FOREACH(orders, ll, c)
               {
                  items = eina_list_append(items, c);
               }
          }
     }

   int_ret = elm_widget_focus_list_next_get
            (obj, items, eina_list_data_get, dir, next);

   if (ret) *ret = int_ret;
}

static void
_elm_genlist_smart_theme(Eo *obj, void *_pd, va_list *list)
{
   Item_Block *itb;
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   if (ret) *ret = EINA_FALSE;
   Eina_Bool int_ret = EINA_FALSE;

   Elm_Genlist_Smart_Data *sd = _pd;

   eo_do_super(obj, MY_CLASS, elm_wdg_theme(&int_ret));
   if (!int_ret) return;

   EINA_INLIST_FOREACH(sd->blocks, itb)
     {
        Eina_List *l;
        Elm_Gen_Item *it;
        EINA_LIST_FOREACH(itb->items, l, it)
          {
             _item_queue(it, NULL);
          }
     }
   sd->calc_done = EINA_FALSE;
   _item_cache_all_free(sd);
   eina_hash_free_buckets(sd->size_caches);
   elm_layout_sizing_eval(obj);
   evas_object_smart_changed(sd->pan_obj);

   if (ret) *ret = EINA_TRUE;
}

/* FIXME: take off later. maybe this show region coords belong in the
 * interface (new api functions, set/get)? */
static void
_show_region_hook(void *data EINA_UNUSED,
                  Evas_Object *obj)
{
   Evas_Coord x, y, w, h;

   ELM_GENLIST_DATA_GET_OR_RETURN(obj, sd);

   elm_widget_show_region_get(obj, &x, &y, &w, &h);
   //x & y are screen coordinates, Add with pan coordinates
   x += sd->pan_x;
   y += sd->pan_y;
   eo_do(obj, elm_scrollable_interface_content_region_show(x, y, w, h));
}

static void
_elm_genlist_smart_translate(Eo *obj, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Item_Block *itb;
   Elm_Genlist_Smart_Data *sd = _pd;

   // Before calling text_get, inform user first.
   evas_object_smart_callback_call(obj, SIG_LANG_CHANGED, NULL);

   // FIXME: We should change item's height if lang is changed??
   EINA_INLIST_FOREACH(sd->blocks, itb)
     {
        Eina_List *l;
        Elm_Gen_Item *it;
        EINA_LIST_FOREACH(itb->items, l, it)
          {
             if (it->realized)
               {
                  elm_genlist_item_fields_update((Elm_Object_Item *)it,
                                                 NULL,
                                                 ELM_GENLIST_ITEM_FIELD_TEXT);
               }
          }
     }
   if (ret) *ret = EINA_TRUE;
}

static void
_item_block_position_update(Eina_Inlist *list,
                            int idx)
{
   Item_Block *tmp;

   EINA_INLIST_FOREACH(list, tmp)
     {
        tmp->position = idx++;
        tmp->position_update = EINA_TRUE;
     }
}

static void
_item_position_update(Eina_List *list,
                      int idx)
{
   Elm_Gen_Item *it;
   Eina_List *l;

   EINA_LIST_FOREACH(list, l, it)
     {
        it->position = idx++;
        it->position_update = EINA_TRUE;
     }
}

static void
_item_block_merge(Item_Block *left,
                  Item_Block *right)
{
   Eina_List *l;
   Elm_Gen_Item *it2;

   EINA_LIST_FOREACH(right->items, l, it2)
     {
        it2->item->block = left;
        left->count++;
        left->calc_done = EINA_FALSE;
     }
   left->items = eina_list_merge(left->items, right->items);
}

static void
_item_block_del(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   Eina_Inlist *il;
   Item_Block *itb = GL_IT(it)->block;
   Eina_Bool block_changed = EINA_FALSE;

   itb->items = eina_list_remove(itb->items, it);
   itb->count--;
   itb->calc_done = EINA_FALSE;
   sd->calc_done = EINA_FALSE;
   evas_object_smart_changed(sd->pan_obj);
   if (itb->count < 1)
     {
        Item_Block *itbn;

        il = EINA_INLIST_GET(itb);
        itbn = (Item_Block *)(il->next);
        if (it->parent)
          it->parent->item->items =
            eina_list_remove(it->parent->item->items, it);
        else
          {
             _item_block_position_update(il->next, itb->position);
          }
        GL_IT(it)->wsd->blocks =
          eina_inlist_remove(GL_IT(it)->wsd->blocks, il);
        GL_IT(it)->wsd->blocks_realized = eina_list_remove
           (GL_IT(it)->wsd->blocks_realized, itb);
        free(itb);
        itb = NULL;
        if (itbn) itbn->calc_done = EINA_FALSE;
     }
   else
     {
        if (itb->count < (itb->sd->max_items_per_block / 2))
          {
             Item_Block *itbp;
             Item_Block *itbn;

             il = EINA_INLIST_GET(itb);
             itbp = (Item_Block *)(il->prev);
             itbn = (Item_Block *)(il->next);

             /* merge block with previous */
             if ((itbp) &&
                 ((itbp->count + itb->count) <
                  (itb->sd->max_items_per_block +
                   (itb->sd->max_items_per_block / 2))))
               {
                  _item_block_merge(itbp, itb);
                  _item_block_position_update
                    (EINA_INLIST_GET(itb)->next, itb->position);
                  GL_IT(it)->wsd->blocks = eina_inlist_remove
                      (GL_IT(it)->wsd->blocks, EINA_INLIST_GET(itb));
                  GL_IT(it)->wsd->blocks_realized = eina_list_remove
                     (GL_IT(it)->wsd->blocks_realized, itb);
                  free(itb);
                  block_changed = EINA_TRUE;
               }
             /* merge block with next */
             else if ((itbn) &&
                      ((itbn->count + itb->count) <
                       (itb->sd->max_items_per_block +
                        (itb->sd->max_items_per_block / 2))))
               {
                  _item_block_merge(itb, itbn);
                  _item_block_position_update
                    (EINA_INLIST_GET(itbn)->next, itbn->position);
                  GL_IT(it)->wsd->blocks =
                    eina_inlist_remove(GL_IT(it)->wsd->blocks,
                                       EINA_INLIST_GET(itbn));
                  GL_IT(it)->wsd->blocks_realized = eina_list_remove
                     (GL_IT(it)->wsd->blocks_realized, itbn);
                  free(itbn);
                  block_changed = EINA_TRUE;
               }
          }
     }

   if (block_changed)
     {
        evas_object_smart_changed(sd->pan_obj);
     }

   GL_IT(it)->block = NULL;
}

static Item_Block *
_item_block_new(Elm_Genlist_Smart_Data *sd,
                Eina_Bool prepend)
{
   Item_Block *itb;

   itb = calloc(1, sizeof(Item_Block));
   if (!itb) return NULL;
   itb->sd = sd;
   if (prepend)
     {
        sd->blocks = eina_inlist_prepend(sd->blocks, EINA_INLIST_GET(itb));
        _item_block_position_update(sd->blocks, 0);
     }
   else
     {
        sd->blocks = eina_inlist_append(sd->blocks, EINA_INLIST_GET(itb));
        itb->position_update = EINA_TRUE;
        if (sd->blocks != EINA_INLIST_GET(itb))
          {
             itb->position =
               ((Item_Block *)(EINA_INLIST_GET(itb)->prev))->position + 1;
          }
        else
          {
             itb->position = 0;
          }
     }

   return itb;
}

static Eina_Bool
_item_block_add(Elm_Genlist_Smart_Data *sd,
                Elm_Gen_Item *it)
{
   Item_Block *itb = NULL;
   if (GL_IT(it)->block) return EINA_TRUE;

   if (!GL_IT(it)->rel)
     {
newblock:
        if (GL_IT(it)->rel)
          {
             itb = calloc(1, sizeof(Item_Block));
             if (!itb) return EINA_FALSE;
             itb->sd = sd;
             if (!GL_IT(it)->rel->item->block)
               {
                  sd->blocks =
                    eina_inlist_append(sd->blocks, EINA_INLIST_GET(itb));
                  itb->items = eina_list_append(itb->items, it);
                  itb->position_update = EINA_TRUE;
                  it->position = eina_list_count(itb->items);
                  it->position_update = EINA_TRUE;

                  if (sd->blocks != EINA_INLIST_GET(itb))
                    {
                       itb->position =
                         ((Item_Block *)
                          (EINA_INLIST_GET(itb)->prev))->position + 1;
                    }
                  else
                    {
                       itb->position = 0;
                    }
               }
             else
               {
                  Eina_List *tmp;

                  tmp = eina_list_data_find_list(itb->items, GL_IT(it)->rel);
                  if (GL_IT(it)->before)
                    {
                       sd->blocks = eina_inlist_prepend_relative
                           (sd->blocks, EINA_INLIST_GET(itb),
                           EINA_INLIST_GET(GL_IT(it)->rel->item->block));
                       itb->items =
                         eina_list_prepend_relative_list(itb->items, it, tmp);

                       /* Update index from where we prepended */
                       _item_position_update
                         (eina_list_prev(tmp), GL_IT(it)->rel->position);
                       _item_block_position_update
                         (EINA_INLIST_GET(itb),
                         GL_IT(it)->rel->item->block->position);
                    }
                  else
                    {
                       sd->blocks = eina_inlist_append_relative
                           (sd->blocks, EINA_INLIST_GET(itb),
                           EINA_INLIST_GET(GL_IT(it)->rel->item->block));
                       itb->items =
                         eina_list_append_relative_list(itb->items, it, tmp);

                       /* Update block index from where we appended */
                       _item_position_update
                         (eina_list_next(tmp), GL_IT(it)->rel->position + 1);
                       _item_block_position_update
                         (EINA_INLIST_GET(itb),
                         GL_IT(it)->rel->item->block->position + 1);
                    }
               }
          }
        else
          {
             if (GL_IT(it)->before)
               {
                  if (sd->blocks)
                    {
                       itb = (Item_Block *)(sd->blocks);
                       if (itb->count >= sd->max_items_per_block)
                         {
                            itb = _item_block_new(sd, EINA_TRUE);
                            if (!itb) return EINA_FALSE;
                         }
                    }
                  else
                    {
                       itb = _item_block_new(sd, EINA_TRUE);
                       if (!itb) return EINA_FALSE;
                    }
                  itb->items = eina_list_prepend(itb->items, it);
                  _item_position_update(itb->items, 0);
               }
             else
               {
                  if (sd->blocks)
                    {
                       itb = (Item_Block *)(sd->blocks->last);
                       if (itb->count >= sd->max_items_per_block)
                         {
                            itb = _item_block_new(sd, EINA_FALSE);
                            if (!itb) return EINA_FALSE;
                         }
                    }
                  else
                    {
                       itb = _item_block_new(sd, EINA_FALSE);
                       if (!itb) return EINA_FALSE;
                    }
                  itb->items = eina_list_append(itb->items, it);
                  it->position = eina_list_count(itb->items);
               }
          }
     }
   else
     {
        Eina_List *tmp;

#if 0
        if ((!GL_IT(it)->wsd->sorting) && (GL_IT(it)->rel->item->queued))
          {
             /* NOTE: for a strange reason eina_list and eina_inlist
                don't have the same property on sorted insertion
                order, so the queue is not always ordered like the
                item list.  This lead to issue where we depend on an
                item that is not yet created. As a quick work around,
                we reschedule the calc of the item and stop reordering
                the list to prevent any nasty issue to show up here.
              */
             sd->queue = eina_list_append(sd->queue, it);
             sd->requeued = EINA_TRUE;
             GL_IT(it)->queued = EINA_TRUE;

             return EINA_FALSE;
          }
#endif
        itb = GL_IT(it)->rel->item->block;
        if (!itb) goto newblock;
        tmp = eina_list_data_find_list(itb->items, GL_IT(it)->rel);
        if (GL_IT(it)->before)
          {
             itb->items = eina_list_prepend_relative_list(itb->items, it, tmp);
             _item_position_update
               (eina_list_prev(tmp), GL_IT(it)->rel->position);
          }
        else
          {
             itb->items = eina_list_append_relative_list(itb->items, it, tmp);
             _item_position_update
               (eina_list_next(tmp), GL_IT(it)->rel->position + 1);
          }
     }

   itb->count++;
   itb->calc_done = EINA_FALSE;
   sd->calc_done = EINA_FALSE;
   GL_IT(it)->block = itb;
   evas_object_smart_changed(itb->sd->pan_obj);

   if (itb->count > itb->sd->max_items_per_block)
     {
        int newc;
        Item_Block *itb2;
        Elm_Gen_Item *it2;
        Eina_Bool done = EINA_FALSE;

        newc = itb->count / 2;

        if (EINA_INLIST_GET(itb)->prev)
          {
             Item_Block *itbp = (Item_Block *)(EINA_INLIST_GET(itb)->prev);

             if (itbp->count + newc < sd->max_items_per_block / 2)
               {
                  /* moving items to previous block */
                  while ((itb->count > newc) && (itb->items))
                    {
                       it2 = eina_list_data_get(itb->items);
                       itb->items = eina_list_remove_list
                           (itb->items, itb->items);
                       itb->count--;

                       itbp->items = eina_list_append(itbp->items, it2);
                       it2->item->block = itbp;
                       itbp->count++;
                    }

                  done = EINA_TRUE;
               }
          }

        if (!done && EINA_INLIST_GET(itb)->next)
          {
             Item_Block *itbn = (Item_Block *)(EINA_INLIST_GET(itb)->next);

             if (itbn->count + newc < sd->max_items_per_block / 2)
               {
                  /* moving items to next block */
                  while ((itb->count > newc) && (itb->items))
                    {
                       Eina_List *l;

                       l = eina_list_last(itb->items);
                       it2 = eina_list_data_get(l);
                       itb->items = eina_list_remove_list(itb->items, l);
                       itb->count--;

                       itbn->items = eina_list_prepend(itbn->items, it2);
                       it2->item->block = itbn;
                       itbn->count++;
                    }

                  done = EINA_TRUE;
               }
          }

        if (!done)
          {
             /* moving items to new block */
             itb2 = calloc(1, sizeof(Item_Block));
             if (!itb2) return EINA_FALSE;
             itb2->sd = sd;
             sd->blocks =
               eina_inlist_append_relative(sd->blocks, EINA_INLIST_GET(itb2),
                                           EINA_INLIST_GET(itb));
             itb2->calc_done = EINA_FALSE;
             while ((itb->count > newc) && (itb->items))
               {
                  Eina_List *l;

                  l = eina_list_last(itb->items);
                  it2 = l->data;
                  itb->items = eina_list_remove_list(itb->items, l);
                  itb->count--;

                  itb2->items = eina_list_prepend(itb2->items, it2);
                  it2->item->block = itb2;
                  itb2->count++;
               }
          }
     }

   return EINA_TRUE;
}

static void
_decorate_all_item_unrealize(Elm_Gen_Item *it)
{
   if (!it->deco_all_view) return;

   edje_object_part_unswallow(it->deco_all_view, VIEW(it));
   _view_clear(it->deco_all_view, &(GL_IT(it)->deco_all_contents));
   evas_object_del(it->deco_all_view);
   it->deco_all_view = NULL;

   edje_object_signal_emit(VIEW(it), SIGNAL_DECORATE_DISABLED, "elm");

   _item_mouse_callbacks_add(it, VIEW(it));
   evas_object_smart_member_add(VIEW(it), GL_IT(it)->wsd->pan_obj);
}

static void
_item_mouse_move_cb(void *data,
                    Evas *evas EINA_UNUSED,
                    Evas_Object *obj,
                    void *event_info)
{
   Evas_Event_Mouse_Move *ev = event_info;
   Elm_Gen_Item *it = data;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if ((sd->reorder_mode) && (sd->reorder.it == it) &&
       (!it->item->hide))
     {
        Evas_Coord ox;
        evas_object_geometry_get(sd->pan_obj, &ox, NULL, NULL, NULL);

        if ((ev->cur.canvas.y - it->dy) > GL_IT(it)->scrl_y)
           sd->reorder.dir = 1;
        else if ((ev->cur.canvas.y - it->dy) < GL_IT(it)->scrl_y)
           sd->reorder.dir = -1;

        GL_IT(it)->scrl_x = it->x + GL_IT(it)->block->x - sd->pan_x + ox;
        GL_IT(it)->scrl_y = ev->cur.canvas.y - it->dy;
        GL_IT(it)->w = sd->minw;
        GL_IT(it)->h = GL_IT(it)->minh;
        evas_object_smart_changed(sd->pan_obj);
     }

   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     {
        Evas_Coord minw = 0, minh = 0, x, y, dx, dy, adx, ady;
        if (GL_IT(it)->highlight_timer)
          {
             ecore_timer_del(GL_IT(it)->highlight_timer);
             GL_IT(it)->highlight_timer = NULL;
          }
        else if (!it->selected) _item_unhighlight(it);

        if (it->long_timer)
          {
             ecore_timer_del(it->long_timer);
             it->long_timer = NULL;
          }

        evas_object_ref(obj);

        if (!it->dragging)
          {
             minw = GL_IT(it)->wsd->finger_minw;
             minh = GL_IT(it)->wsd->finger_minh;

             evas_object_geometry_get(obj, &x, &y, NULL, NULL);
             dx = ev->cur.canvas.x - x;
             dy = ev->cur.canvas.y - y;
             dx = dx - it->dx;
             dy = dy - it->dy;
             adx = dx;
             ady = dy;
             if (adx < 0) adx = -dx;
             if (ady < 0) ady = -dy;
             if ((adx > minw) || (ady > minh))
               {
                  it->dragging = EINA_TRUE;
                  if (adx > ady)
                    {
                       if (dx < 0)
                          evas_object_smart_callback_call
                             (WIDGET(it), SIG_DRAG_START_LEFT, it);
                       else
                          evas_object_smart_callback_call
                             (WIDGET(it), SIG_DRAG_START_RIGHT, it);
                    }
                  else
                    {
                       if (dy < 0)
                          evas_object_smart_callback_call
                             (WIDGET(it), SIG_DRAG_START_UP, it);
                       else
                          evas_object_smart_callback_call
                             (WIDGET(it), SIG_DRAG_START_DOWN, it);
                    }
               }
          }

        /* If item magic value is changed, do not call smart callback*/
        if (EINA_MAGIC_CHECK((Elm_Widget_Item *)it, ELM_WIDGET_ITEM_MAGIC))
          {
             if (it->dragging)
               evas_object_smart_callback_call(WIDGET(it), SIG_DRAG, it);
          }

        evas_object_unref(obj);
     }
}

static Eina_Bool
_long_press_cb(void *data)
{
   Elm_Gen_Item *it = data, *it2;
   Elm_Genlist_Smart_Data *sd;
   Eina_List *l, *ll;

   sd = GL_IT(it)->wsd;
   it->long_timer = NULL;

   if (elm_widget_item_disabled_get(it) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY) ||
       (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY))
     return ECORE_CALLBACK_CANCEL;

   if ((sd->reorder_mode) &&
       (GL_IT(it)->type != ELM_GENLIST_ITEM_GROUP) &&
       (!sd->key_down_item))
     {
        if (elm_genlist_item_expanded_get((Elm_Object_Item *)it))
          {
             elm_genlist_item_expanded_set((Elm_Object_Item *)it, EINA_FALSE);
          }
        EINA_LIST_FOREACH_SAFE(sd->selected, l, ll, it2)
          {
             _item_unselect(it2);
          }
        eo_do(sd->obj, elm_scrollable_interface_hold_set(EINA_TRUE),
              elm_scrollable_interface_bounce_allow_set(EINA_FALSE, EINA_FALSE));
        if (sd->decorate_all_mode)
          edje_object_signal_emit(VIEW(it), SIGNAL_REORDER_ENABLED, "elm");

        sd->reorder.it = it;
        evas_object_smart_changed(sd->pan_obj);
     }
   evas_object_smart_callback_call(WIDGET(it), SIG_LONGPRESSED, it);

   return ECORE_CALLBACK_CANCEL;
}

void
_gesture_do(void *data)
{
   Elm_Genlist_Smart_Data *sd = data;

   if ((sd->g_item) && (sd->g_type))
     {
        if (!strcmp(sd->g_type, SIG_MULTI_SWIPE_LEFT))
               evas_object_smart_callback_call
             (WIDGET(sd->g_item), SIG_MULTI_SWIPE_LEFT, sd->g_item);
        else if (!strcmp(sd->g_type, SIG_MULTI_SWIPE_RIGHT))
               evas_object_smart_callback_call
             (WIDGET(sd->g_item), SIG_MULTI_SWIPE_RIGHT, sd->g_item);
        else if (!strcmp(sd->g_type, SIG_MULTI_SWIPE_UP))
               evas_object_smart_callback_call
             (WIDGET(sd->g_item), SIG_MULTI_SWIPE_UP, sd->g_item);
        else if (!strcmp(sd->g_type, SIG_MULTI_SWIPE_DOWN))
               evas_object_smart_callback_call
             (WIDGET(sd->g_item), SIG_MULTI_SWIPE_DOWN, sd->g_item);
        else if (!strcmp(sd->g_type, SIG_MULTI_PINCH_OUT))
          evas_object_smart_callback_call
             (WIDGET(sd->g_item), SIG_MULTI_PINCH_OUT, sd->g_item);
        else if (!strcmp(sd->g_type, SIG_MULTI_PINCH_IN))
          evas_object_smart_callback_call
             (WIDGET(sd->g_item), SIG_MULTI_PINCH_IN, sd->g_item);
        else if (!strcmp(sd->g_type, SIG_SWIPE))
          evas_object_smart_callback_call
             (WIDGET(sd->g_item), SIG_SWIPE, sd->g_item);

        sd->g_item = NULL;
        sd->g_type = NULL;
     }
}

static void
_item_mouse_down_cb(void *data,
                    Evas *evas,
                    Evas_Object *obj,
                    void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   Elm_Gen_Item *it = data;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   Evas_Coord x, y;

   // FIXME: To prevent duplicated callback call
   //if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return;

   if (ev->button != 1) return;
   // mouse down is activate only one finger
   if (evas_event_down_count_get(evas) != 1) return;

   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   it->dx = ev->canvas.x - x;
   it->dy = ev->canvas.y - y;

   GL_IT(it)->wsd->was_selected = it->selected;
   if (GL_IT(it)->highlight_timer) ecore_timer_del(GL_IT(it)->highlight_timer);
   GL_IT(it)->highlight_timer = ecore_timer_add(ITEM_HIGHLIGHT_TIMER,
                                               _highlight_timer, it);
   if (it->long_timer) ecore_timer_del(it->long_timer);
   it->long_timer = ecore_timer_add(sd->longpress_timeout, _long_press_cb, it);

   if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
     {
        evas_object_smart_callback_call(WIDGET(it), SIG_CLICKED_DOUBLE, it);
        evas_object_smart_callback_call(WIDGET(it), SIG_ACTIVATED, it);
     }
   evas_object_smart_callback_call(WIDGET(it), SIG_PRESSED, it);
}

static void
_item_min_calc(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   Evas_Coord mw = 0, mh = 0;

   Evas_Coord vw = 0;
   eo_do(sd->obj, elm_scrollable_interface_content_viewport_size_get(&vw, NULL));
   if ((it->select_mode != ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY) &&
       (sd->select_mode != ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY))
     {
        mw = sd->finger_minw;
        mh = sd->finger_minh;
     }
   if ((sd->mode == ELM_LIST_COMPRESS) && vw && (mw < vw))
      mw = vw;
   edje_object_size_min_restricted_calc(VIEW(it), &mw, &mh, mw, mh);

   if (sd->homogeneous)
     {
        Size_Cache *size, *tmp;
        tmp = eina_hash_find(sd->size_caches, it->itc->item_style);
        if (tmp)
           eina_hash_del_by_key(sd->size_caches, it->itc->item_style);

        size = ELM_NEW(Size_Cache);
        size->minw = mw;
        size->minh = mh;
        eina_hash_add(sd->size_caches, it->itc->item_style, size);
     }

   GL_IT(it)->w = GL_IT(it)->minw = mw;
   GL_IT(it)->h = GL_IT(it)->minh = mh;

   // FIXME: This is workaround for entry size issue.
   if (sd->realization_mode)
     {
        if (it->deco_all_view)
          evas_object_resize(it->deco_all_view, GL_IT(it)->w, GL_IT(it)->h);
        else if (GL_IT(it)->deco_it_view)
          evas_object_resize(GL_IT(it)->deco_it_view, GL_IT(it)->w, GL_IT(it)->h);
        else
          evas_object_resize(VIEW(it), GL_IT(it)->w, GL_IT(it)->h);
     }
   //
}

static void
_item_calc(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   Size_Cache *size = NULL;
   Evas_Coord p_minw, p_minh;

   if (GL_IT(it)->calc_done) return;

   p_minw = GL_IT(it)->minw;
   p_minh = GL_IT(it)->minh;

   if (sd->homogeneous)
     size = eina_hash_find(sd->size_caches, it->itc->item_style);
   if (size)
     {
        GL_IT(it)->minw = size->minw;
        GL_IT(it)->minh = size->minh;
     }
   else
     {
        if (!it->realized)
          {
             if (sd->realization_mode)
               {
                  _item_realize(it, EINA_FALSE);
                  _item_min_calc(it);
               }
             else
               {
                  _item_realize(it, EINA_TRUE);
                  _item_min_calc(it);
                  _item_unrealize(it, EINA_TRUE);
               }
          }
        else
           _item_min_calc(it);
     }

   GL_IT(it)->calc_done = EINA_TRUE;
   if ((p_minw != GL_IT(it)->minw) || (p_minh != GL_IT(it)->minh))
     {
        GL_IT(it)->block->calc_done = EINA_FALSE;
        sd->calc_done = EINA_FALSE;
     }
}

static Eina_Bool
_item_process(Elm_Genlist_Smart_Data *sd,
              Elm_Gen_Item *it)
{
   if (!_item_block_add(sd, it)) return EINA_FALSE;

   GL_IT(it)->calc_done = EINA_FALSE;
   _item_calc(it);

   return EINA_TRUE;
}

static void
_dummy_job(void *data)
{
   Elm_Genlist_Smart_Data *sd = data;
   sd->dummy_job = NULL;
   return;
}

static Eina_Bool
_queue_idle_enter(void *data)
{
   Elm_Genlist_Smart_Data *sd = data;
   Evas_Coord vw = 0;
   int n;
   double ft, t0;

   eo_do(sd->obj, elm_scrollable_interface_content_viewport_size_get(&vw, NULL));
   if (vw == 0)
     {
        if (sd->dummy_job)
          {
             ecore_job_del(sd->dummy_job);
             sd->dummy_job = NULL;
          }
        sd->queue_idle_enterer = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   ft = ecore_animator_frametime_get()/2;
   t0 = ecore_time_get();
   for (n = 0; (sd->queue) && (n < 128); n++)
     {
        Elm_Gen_Item *it;
        double t;

        it = eina_list_data_get(sd->queue);
        sd->queue = eina_list_remove_list(sd->queue, sd->queue);
        GL_IT(it)->queued = EINA_FALSE;
        if (!_item_process(sd, it)) continue;
        t = ecore_time_get();

        /* same as eina_inlist_count > 1 */
        if (sd->blocks && sd->blocks->next)
          {
             if ((t - t0) > ft) break;
          }
     }

   evas_object_smart_changed(sd->pan_obj);
   if (!sd->queue)
     {
        if (sd->dummy_job)
          {
             ecore_job_del(sd->dummy_job);
             sd->dummy_job = NULL;
          }
        sd->queue_idle_enterer = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   // Do not use smart_changed
   // Instead make any events (job, idler, etc.) to call idle enterer
   if (sd->dummy_job) ecore_job_del(sd->dummy_job);
   sd->dummy_job = ecore_job_add(_dummy_job, sd);

   return ECORE_CALLBACK_RENEW;
}

static void
_item_queue(Elm_Gen_Item *it,
            Eina_Compare_Cb cb)
{
   if (GL_IT(it)->queued) return;

   GL_IT(it)->queued = EINA_TRUE;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   if (cb && !sd->requeued)
     sd->queue = eina_list_sorted_insert(sd->queue, cb, it);
   else
     sd->queue = eina_list_append(sd->queue, it);

   if (sd->queue_idle_enterer)
      ecore_idle_enterer_del(sd->queue_idle_enterer);
   sd->queue_idle_enterer = ecore_idle_enterer_add(_queue_idle_enter, sd);
}

static void
_item_queue_direct(Elm_Gen_Item *it,
                   Eina_Compare_Cb cb)
{
   Evas_Coord vw, vh;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   eo_do(sd->obj, elm_scrollable_interface_content_viewport_size_get(&vw, &vh));
   // Processing items within viewport if items already exist.
   // This can prevent flickering issue when content size is changed.
   // This can fix the flickering issue when expanded item have subitems whose total height > vh
   if (!sd->queue && vw
       && ((sd->processed_sizes <= vh) || (GL_IT(it)->expanded_depth > 0)))
     {
        _item_process(sd, it);
        sd->processed_sizes += GL_IT(it)->minh;
        evas_object_smart_changed(sd->pan_obj);
        return;
     }
   _item_queue(it, cb);
}

/* If the application wants to know the relative item, use
 * elm_genlist_item_next_get(it)*/
static void
_item_move_before(Elm_Gen_Item *it,
                  Elm_Gen_Item *before)
{
   if (!it) return;
   if (!before) return;
   if (it == before) return;
   ELM_GENLIST_DATA_GET_FROM_ITEM(it, sd);

   sd->items =
     eina_inlist_remove(sd->items, EINA_INLIST_GET(it));
   if (it->item->block) _item_block_del(it);
   sd->items = eina_inlist_prepend_relative
       (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(before));
   if (it->item->rel)
      it->item->rel->item->rel_revs =
         eina_list_remove(it->item->rel->item->rel_revs, it);
   it->item->rel = before;
   before->item->rel_revs = eina_list_append(before->item->rel_revs, it);
   it->item->before = EINA_TRUE;
   if (before->item->group_item)
     it->item->group_item = before->item->group_item;
   _item_queue(it, NULL);

   evas_object_smart_callback_call(WIDGET(it), SIG_MOVED_BEFORE, it);
}

static void
_item_move_after(Elm_Gen_Item *it,
                 Elm_Gen_Item *after)
{
   if (!it) return;
   if (!after) return;
   if (it == after) return;

   GL_IT(it)->wsd->items =
     eina_inlist_remove(GL_IT(it)->wsd->items, EINA_INLIST_GET(it));
   if (GL_IT(it)->block) _item_block_del(it);

   GL_IT(it)->wsd->items = eina_inlist_append_relative
      (GL_IT(it)->wsd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(after));

   if (GL_IT(it)->rel)
     GL_IT(it)->rel->item->rel_revs =
        eina_list_remove(GL_IT(it)->rel->item->rel_revs, it);
   GL_IT(it)->rel = after;
   after->item->rel_revs = eina_list_append(after->item->rel_revs, it);
   GL_IT(it)->before = EINA_FALSE;
   if (after->item->group_item) GL_IT(it)->group_item = after->item->group_item;
   _item_queue_direct(it, NULL);

   evas_object_smart_callback_call(WIDGET(it), SIG_MOVED_AFTER, it);
}

static void
_item_mouse_up_cb(void *data,
                  Evas *evas,
                  Evas_Object *obj EINA_UNUSED,
                  void *event_info)
{
   Evas_Event_Mouse_Up *ev = event_info;
   Elm_Gen_Item *it = data;
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if (ev->button != 1) return;

   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     {
        if (it->dragging)
          {
             it ->dragging = EINA_FALSE;
             evas_object_smart_callback_call(WIDGET(it), SIG_DRAG_STOP, it);
          }
     }
   _gesture_do(sd);

   if (evas_event_down_count_get(evas) != 0)
     {
        if (!GL_IT(it)->highlight_timer) _item_unhighlight(it);
     }

   if (it->long_timer)
     {
        ecore_timer_del(it->long_timer);
        it->long_timer = NULL;
     }
   if (GL_IT(it)->highlight_timer)
     {
        ecore_timer_del(GL_IT(it)->highlight_timer);
        GL_IT(it)->highlight_timer = NULL;
        // Because naviframe can drop the all evevents.
        // highlight it before select timer is called.
        if (evas_event_down_count_get(evas) == 0) _item_highlight(it);
     }

   if (!sd->reorder.it && (evas_event_down_count_get(evas) == 0))
     {
        // FIXME: if highlight mode is not used, mouse move cannot disable
        // _item_select
        if ((sd->highlight && it->highlighted) || !sd->highlight)
          {
             if (sd->was_selected == it->selected)
                GL_IT(it)->highlight_timer =
                   ecore_timer_add(ITEM_SELECT_TIMER, _select_timer, it);
          }
     }
   else if (sd->reorder.it == it)
     {
        Elm_Gen_Item *it2, *it_max = NULL, *it_min = NULL;
        Evas_Coord r_y_scrl, it_y_max = 0, it_y_min = 99999999;

        if (!it->selected) _item_unhighlight(it);
        r_y_scrl = GL_IT(it)->scrl_y;
        EINA_LIST_FREE(sd->reorder.move_items, it2)
          {
             if (sd->reorder.it->parent != it2->parent)
               {
                  it2->item->reorder_offset = 0;
                  continue;
               }
             Evas_Coord it_y = it2->item->scrl_y +
                it2->item->reorder_offset + (it2->item->h / 2) +
                it2->item->block->y;

             if ((it_y < r_y_scrl) &&
                 (it_y_max < it_y))
               {
                  it_max = it2;
                  it_y_max = it_y;
               }
             else if ((it_y > r_y_scrl) &&
                      (it_y_min > it_y))
               {
                  it_min = it2;
                  it_y_min = it_y;
               }
             it2->item->reorder_offset = 0;
          }
        if (it_max)
          {
             _item_move_after(it, it_max);
             evas_object_smart_callback_call(WIDGET(it), SIG_MOVED, it);
          }
        else if (it_min)
          {
             _item_move_before(it, it_min);
             evas_object_smart_callback_call(WIDGET(it), SIG_MOVED, it);
          }
        sd->reorder.it = NULL;
        sd->reorder.dir = 0;
        if (sd->reorder.anim)
          {
             ecore_animator_del(sd->reorder.anim);
             sd->reorder.anim = NULL;
          }

        eo_do(sd->obj, elm_scrollable_interface_hold_set(EINA_FALSE),
              elm_scrollable_interface_bounce_allow_set(sd->h_bounce,
                                                        sd->v_bounce));
        edje_object_signal_emit(VIEW(it), SIGNAL_REORDER_DISABLED, "elm");
        evas_object_smart_changed(sd->pan_obj);
     }
   evas_object_smart_callback_call(WIDGET(it), SIG_RELEASED, it);
}

static Eina_Bool
_scroll_hold_timer_cb(void *data)
{
   ELM_GENLIST_DATA_GET(data, sd);

   if (!data) return ECORE_CALLBACK_CANCEL;

   eo_do(sd->obj, elm_scrollable_interface_hold_set(EINA_FALSE));
   sd->scr_hold_timer = NULL;

   return ECORE_CALLBACK_CANCEL;
}

static void
_decorate_item_finished_signal_cb(void *data,
                                  Evas_Object *obj,
                                  const char *emission EINA_UNUSED,
                                  const char *source EINA_UNUSED)
{
   Elm_Gen_Item *it = data;

   if (!data || !obj) return;
   if ((!it->realized)) return;

   _decorate_item_unrealize(it);
}

static Eina_Bool
_scroll_timeout_cb(void *data)
{
   Elm_Genlist_Smart_Data *sd = data;

   sd->scr_timer = NULL;
   if (sd->queue && !sd->queue_idle_enterer)
     {
        sd->queue_idle_enterer = ecore_idle_enterer_add(_queue_idle_enter, sd);
        if (sd->dummy_job) ecore_job_del(sd->dummy_job);
        sd->dummy_job = ecore_job_add(_dummy_job, sd);
     }
   return ECORE_CALLBACK_CANCEL;
}

static void
_scroll_cb(Evas_Object *obj,
           void *data EINA_UNUSED)
{
   ELM_GENLIST_DATA_GET(obj, sd);

   if (sd->queue_idle_enterer)
     {
        ecore_idle_enterer_del(sd->queue_idle_enterer);
        sd->queue_idle_enterer = NULL;
        if (sd->dummy_job)
          {
             ecore_job_del(sd->dummy_job);
             sd->dummy_job = NULL;
          }
     }
   if (sd->scr_timer) ecore_timer_del(sd->scr_timer);
   sd->scr_timer = ecore_timer_add(0.25, _scroll_timeout_cb, sd);

   evas_object_smart_callback_call(obj, SIG_SCROLL, NULL);
}

static void
_scroll_animate_start_cb(Evas_Object *obj,
                         void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_SCROLL_ANIM_START, NULL);
}

static void
_scroll_animate_stop_cb(Evas_Object *obj,
                        void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_SCROLL_ANIM_STOP, NULL);
}

static void
_scroll_drag_start_cb(Evas_Object *obj,
                      void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_SCROLL_DRAG_START, NULL);
}

static void
_scroll_drag_stop_cb(Evas_Object *obj,
                     void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_SCROLL_DRAG_STOP, NULL);
}

static void
_edge_left_cb(Evas_Object *obj,
              void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_EDGE_LEFT, NULL);
}

static void
_edge_right_cb(Evas_Object *obj,
               void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_EDGE_RIGHT, NULL);
}

static void
_edge_top_cb(Evas_Object *obj,
             void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_EDGE_TOP, NULL);
}

static void
_edge_bottom_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_EDGE_BOTTOM, NULL);
}

static void
_vbar_drag_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_VBAR_DRAG, NULL);
}

static void
_vbar_press_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_VBAR_PRESS, NULL);
}

static void
_vbar_unpress_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_VBAR_UNPRESS, NULL);
}

static void
_hbar_drag_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_HBAR_DRAG, NULL);
}

static void
_hbar_press_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_HBAR_PRESS, NULL);
}

static void
_hbar_unpress_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   evas_object_smart_callback_call(obj, SIG_HBAR_UNPRESS, NULL);
}

static Evas_Event_Flags
_pinch_zoom_start_cb(void *data, void *event_info)
{
   Elm_Genlist_Smart_Data *sd = data;
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;

   sd->g_item = (Elm_Gen_Item*)elm_genlist_at_xy_item_get(sd->obj,
                                                          p->x, p->y, NULL);
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
_pinch_zoom_cb(void *data, void *event_info)
{
   Elm_Genlist_Smart_Data *sd = data;
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;

   if (p->zoom > 1.0 + PINCH_ZOOM_TOLERANCE)
     sd->g_type = SIG_MULTI_PINCH_OUT;
   else if (p->zoom < 1.0 - PINCH_ZOOM_TOLERANCE)
     sd->g_type = SIG_MULTI_PINCH_IN;

   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
_gesture_n_lines_start_cb(void *data , void *event_info)
{
   Elm_Genlist_Smart_Data *sd = data;
   Elm_Gesture_Line_Info *p = (Elm_Gesture_Line_Info *) event_info;
   Evas_Coord x,y;

   x = (p->momentum.x1 + p->momentum.x2) / 2;
   y = (p->momentum.y1 + p->momentum.y2) / 2;

   sd->g_item = (Elm_Gen_Item*)elm_genlist_at_xy_item_get(sd->obj, x, y, NULL);
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
_gesture_n_lines_cb(void *data , void *event_info)
{
   Elm_Genlist_Smart_Data *sd = data;
   Elm_Gesture_Line_Info *p = (Elm_Gesture_Line_Info *) event_info;

   if (p->momentum.n < 2)
     return EVAS_EVENT_FLAG_NONE;

   Evas_Coord minw = 0, minh = 0;
   Evas_Coord x, y, off_x, off_y;
   Evas_Coord cur_x, cur_y, prev_x, prev_y;
   Elm_Gen_Item *down_it;

   minw = sd->finger_minw;
   minh = sd->finger_minh;

   prev_x = prev_y = 0;

   cur_x = p->momentum.x1;
   cur_y = p->momentum.y1;

   down_it = (Elm_Gen_Item *)elm_genlist_at_xy_item_get(sd->obj, cur_x, cur_y, 
                                                        NULL);
   if (down_it)
     {
        evas_object_geometry_get(VIEW(down_it), &x, &y, NULL, NULL);
        prev_x = down_it->dx + x;
        prev_y = down_it->dy + y;

        off_x = abs(cur_x - prev_x);
        off_y = abs(cur_y - prev_y);

        if ((off_x > minw) || (off_y > minh))
          {
             if (off_x > off_y)
               {
                  if (cur_x > prev_x)
                    sd->g_type = SIG_MULTI_SWIPE_RIGHT;
                  else
                    sd->g_type = SIG_MULTI_SWIPE_LEFT;
               }
             else
               {
                  if (cur_y > prev_y)
                    sd->g_type = SIG_MULTI_SWIPE_DOWN;
                  else
                    sd->g_type = SIG_MULTI_SWIPE_UP;
               }
          }
     }
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
_gesture_n_flicks_cb(void *data , void *event_info)
{
   Elm_Genlist_Smart_Data *sd = data;
   Elm_Gesture_Line_Info *p = (Elm_Gesture_Line_Info *) event_info;

   if (p->momentum.n == 1)
      sd->g_type = SIG_SWIPE;

   return EVAS_EVENT_FLAG_NONE;
}

static void
_decorate_item_realize(Elm_Gen_Item *it)
{
   char buf[1024];
   if (GL_IT(it)->deco_it_view || !it->itc->decorate_item_style) return;

   // Before adding & swallowing, delete it from smart member
   evas_object_smart_member_del(VIEW(it));

   GL_IT(it)->deco_it_view = _view_create(it, it->itc->decorate_item_style);
   edje_object_part_swallow
      (GL_IT(it)->deco_it_view,
       edje_object_data_get(GL_IT(it)->deco_it_view, "mode_part"), VIEW(it));
   _view_inflate(GL_IT(it)->deco_it_view, it, &(GL_IT(it)->deco_it_contents));

   snprintf(buf, sizeof(buf), "elm,state,%s,active",
            GL_IT(it)->wsd->decorate_it_type);
   edje_object_signal_emit(GL_IT(it)->deco_it_view, buf, "elm");
   edje_object_signal_emit(VIEW(it), buf, "elm");

   _item_mouse_callbacks_add(it, GL_IT(it)->deco_it_view);
   _item_mouse_callbacks_del(it, VIEW(it));
}

static void
_size_cache_free(void *data)
{
   if (data) free(data);
}

static void
_elm_genlist_smart_add(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Elm_Genlist_Smart_Data *priv = _pd;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   Elm_Genlist_Pan_Smart_Data *pan_data;
   Evas_Coord minw, minh;

   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());
   elm_widget_sub_object_parent_add(obj);

   priv->hit_rect = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_smart_member_add(priv->hit_rect, obj);
   elm_widget_sub_object_add(obj, priv->hit_rect);

   /* common scroller hit rectangle setup */
   evas_object_color_set(priv->hit_rect, 0, 0, 0, 0);
   evas_object_show(priv->hit_rect);
   evas_object_repeat_events_set(priv->hit_rect, EINA_TRUE);

   elm_widget_can_focus_set(obj, EINA_TRUE);
   elm_widget_on_show_region_hook_set(obj, _show_region_hook, NULL);

   if (!elm_layout_theme_set
       (obj, "genlist", "base", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   eo_do(obj, elm_scrollable_interface_objects_set(wd->resize_obj, priv->hit_rect));

   eo_do(obj, elm_scrollable_interface_bounce_allow_set
         (EINA_FALSE, _elm_config->thumbscroll_bounce_enable));
   priv->v_bounce = _elm_config->thumbscroll_bounce_enable;

   eo_do(obj,
         elm_scrollable_interface_scroll_cb_set(_scroll_cb),
         elm_scrollable_interface_animate_start_cb_set(_scroll_animate_start_cb),
         elm_scrollable_interface_animate_stop_cb_set(_scroll_animate_stop_cb),
         elm_scrollable_interface_drag_start_cb_set(_scroll_drag_start_cb),
         elm_scrollable_interface_drag_stop_cb_set(_scroll_drag_stop_cb),
         elm_scrollable_interface_edge_left_cb_set(_edge_left_cb),
         elm_scrollable_interface_edge_right_cb_set(_edge_right_cb),
         elm_scrollable_interface_edge_top_cb_set(_edge_top_cb),
         elm_scrollable_interface_edge_bottom_cb_set(_edge_bottom_cb),
         elm_scrollable_interface_vbar_drag_cb_set(_vbar_drag_cb),
         elm_scrollable_interface_vbar_press_cb_set(_vbar_press_cb),
         elm_scrollable_interface_vbar_unpress_cb_set(_vbar_unpress_cb),
         elm_scrollable_interface_hbar_drag_cb_set(_hbar_drag_cb),
         elm_scrollable_interface_hbar_press_cb_set(_hbar_press_cb),
         elm_scrollable_interface_hbar_unpress_cb_set(_hbar_unpress_cb));

   priv->mode = ELM_LIST_SCROLL;
   priv->max_items_per_block = MAX_ITEMS_PER_BLOCK;
   priv->item_cache_max = priv->max_items_per_block * 2;
   priv->longpress_timeout = _elm_config->longpress_timeout;
   priv->highlight = EINA_TRUE;
   priv->finger_minw = 0;
   priv->finger_minh = 0;
   elm_coords_finger_size_adjust(1, &priv->finger_minw, 1, &priv->finger_minh);

   priv->pan_obj = eo_add(MY_PAN_CLASS, evas_object_evas_get(obj));
   pan_data = eo_data_scope_get(priv->pan_obj, MY_PAN_CLASS);
   eo_data_ref(obj, NULL);
   pan_data->wobj = obj;
   pan_data->wsd = priv;

   eo_do(obj, elm_scrollable_interface_extern_pan_set(priv->pan_obj));

   edje_object_size_min_calc(wd->resize_obj, &minw, &minh);
   evas_object_size_hint_min_set(obj, minw, minh);

   priv->size_caches = eina_hash_string_small_new(_size_cache_free);
   _item_cache_all_free(priv);

   eo_do(obj, elm_scrollable_interface_mirrored_set(elm_widget_mirrored_get(obj)));

   const char *str = edje_object_data_get(wd->resize_obj, "focus_highlight");
   if ((str) && (!strcmp(str, "on")))
      elm_widget_highlight_in_theme_set(obj, EINA_TRUE);
   else
      elm_widget_highlight_in_theme_set(obj, EINA_FALSE);
   priv->select_on_focus_enabled = EINA_FALSE;
   elm_widget_signal_callback_add(obj, "elm,action,focus_highlight,hide", "elm", _elm_genlist_focus_highlight_hide, obj);
   elm_widget_signal_callback_add(obj, "elm,action,focus_highlight,show", "elm", _elm_genlist_focus_highlight_show, obj);

   priv->g_layer = elm_gesture_layer_add(obj);
   if (!priv->g_layer) ERR("elm_gesture_layer_add() failed");
   elm_gesture_layer_attach(priv->g_layer, priv->hit_rect);
   elm_gesture_layer_cb_set
      (priv->g_layer, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_START,
       _pinch_zoom_start_cb, priv);
   elm_gesture_layer_cb_set
      (priv->g_layer, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_MOVE,
       _pinch_zoom_cb, priv);

   elm_gesture_layer_cb_set
      (priv->g_layer, ELM_GESTURE_N_LINES, ELM_GESTURE_STATE_START,
       _gesture_n_lines_start_cb, priv);
   elm_gesture_layer_cb_set
      (priv->g_layer, ELM_GESTURE_N_LINES, ELM_GESTURE_STATE_MOVE,
       _gesture_n_lines_cb, priv);

   elm_gesture_layer_cb_set
      (priv->g_layer, ELM_GESTURE_N_FLICKS, ELM_GESTURE_STATE_START,
       _gesture_n_lines_start_cb, priv);
   elm_gesture_layer_cb_set
      (priv->g_layer, ELM_GESTURE_N_FLICKS, ELM_GESTURE_STATE_MOVE,
       _gesture_n_flicks_cb, priv);

   elm_layout_sizing_eval(obj);
}

static void
_elm_genlist_smart_del(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Elm_Genlist_Smart_Data *sd = _pd;

   sd->no_cache = EINA_TRUE;
   elm_genlist_clear(obj);
   if (sd->size_caches) eina_hash_free(sd->size_caches);
   if (sd->decorate_it_type) eina_stringshare_del(sd->decorate_it_type);
   eo_unref(sd->pan_obj);
   ELM_SAFE_FREE(sd->pan_obj, evas_object_del);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

static void
_elm_genlist_smart_move(Eo *obj, void *_pd, va_list *list)
{
   Evas_Coord x = va_arg(*list, Evas_Coord);
   Evas_Coord y = va_arg(*list, Evas_Coord);
   Elm_Genlist_Smart_Data *sd = _pd;

   eo_do_super(obj, MY_CLASS, evas_obj_smart_move(x, y));

   evas_object_move(sd->hit_rect, x, y);
}

static void
_elm_genlist_smart_resize(Eo *obj, void *_pd, va_list *list)
{
   Evas_Coord w = va_arg(*list, Evas_Coord);
   Evas_Coord h = va_arg(*list, Evas_Coord);
   Elm_Genlist_Smart_Data *sd = _pd;

   eo_do_super(obj, MY_CLASS, evas_obj_smart_resize(w, h));

   evas_object_resize(sd->hit_rect, w, h);
}

static void
_elm_genlist_smart_member_add(Eo *obj, void *_pd, va_list *list)
{
   Evas_Object *member = va_arg(*list, Evas_Object *);
   Elm_Genlist_Smart_Data *sd = _pd;

   eo_do_super(obj, MY_CLASS, evas_obj_smart_member_add(member));

   if (sd->hit_rect)
     evas_object_raise(sd->hit_rect);
}

static void
_elm_genlist_smart_access(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Genlist_Smart_Data *sd = _pd;
   _elm_genlist_smart_focus_next_enable = va_arg(*list, int);

   Eina_List *l;
   Elm_Gen_Item *it;
   l = elm_genlist_realized_items_get(obj);
   EINA_LIST_FREE(l, it)
     {
        _item_unrealize(it, EINA_FALSE);
     }

   evas_object_smart_changed(sd->pan_obj);
#if TIZEN_ONLY
   evas_object_smart_callback_call(obj, SIG_ACCESS_CHANGED, NULL);a
#endif
}

static void
_elm_genlist_smart_sizing_eval(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Evas_Coord minw = 0, minh = 0, maxw = -1, maxh = -1;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   Elm_Genlist_Smart_Data *sd = _pd;

   /* parent class' early call */
   if (sd->on_sub_del) return;;

   evas_object_size_hint_min_get(obj, &minw, NULL);
   evas_object_size_hint_max_get(obj, &maxw, &maxh);
   edje_object_size_min_calc(wd->resize_obj, &minw, &minh);

   if (sd->mode == ELM_LIST_LIMIT)
     {
        maxw = -1;
        minw = minw + sd->minw;
     }

   evas_object_size_hint_min_set(obj, minw, minh);
   evas_object_size_hint_max_set(obj, maxw, maxh);
}

EAPI Evas_Object *
elm_genlist_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   eo_unref(obj);
   return obj;
}

static void
_constructor(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Elm_Genlist_Smart_Data *sd = _pd;
   sd->obj = obj;

   eo_do_super(obj, MY_CLASS, eo_constructor());
   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks, NULL));
}

static Evas_Object *
_item_content_get_hook(Elm_Gen_Item *it,
                       const char *part)
{
   Evas_Object *ret = NULL;
   if (it->deco_all_view)
     ret = edje_object_part_swallow_get(it->deco_all_view, part);
   else if (it->decorate_it_set)
     ret = edje_object_part_swallow_get(it->item->deco_it_view, part);
   if (!ret)
     {
        if (part)
          ret = edje_object_part_swallow_get(VIEW(it), part);
        else
          ret = edje_object_part_swallow_get(VIEW(it), "elm.swallow.icon");
     }
   return ret;
}

static const char *
_item_text_get_hook(Elm_Gen_Item *it,
                    const char *part)
{
   if (!it->itc->func.text_get) return NULL;
   const char *ret = NULL;
   if (it->deco_all_view)
     ret = edje_object_part_text_get(it->deco_all_view, part);
   else if (it->decorate_it_set)
     ret = edje_object_part_text_get(it->item->deco_it_view, part);
   if (!ret)
     {
        if (part)
          ret = edje_object_part_text_get(VIEW(it), part);
        else
          ret = edje_object_part_text_get(VIEW(it), "elm.text");
     }
   return ret;
}

static void
_item_disable_hook(Elm_Object_Item *item)
{
   Eina_List *l;
   Evas_Object *obj;
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   _item_unselect(it);
   if (it == GL_IT(it)->wsd->focused_item) _item_unfocused(it);
   if (GL_IT(it)->highlight_timer)
     {
        ecore_timer_del(GL_IT(it)->highlight_timer);
        GL_IT(it)->highlight_timer = NULL;
     }
   if (it->long_timer)
     {
        ecore_timer_del(it->long_timer);
        it->long_timer = NULL;
     }

   if (it->realized)
     {
        if (elm_widget_item_disabled_get(it))
          {
             edje_object_signal_emit(VIEW(it), SIGNAL_DISABLED, "elm");
             if (it->deco_all_view)
               edje_object_signal_emit
                 (it->deco_all_view, SIGNAL_DISABLED, "elm");
          }
        else
          {
             edje_object_signal_emit(VIEW(it), SIGNAL_ENABLED, "elm");
             if (it->deco_all_view)
               edje_object_signal_emit
                 (it->deco_all_view, SIGNAL_ENABLED, "elm");
          }
        EINA_LIST_FOREACH(it->content_objs, l, obj)
          elm_widget_disabled_set(obj, elm_widget_item_disabled_get(it));
     }
}

static void
_item_free_common(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   sd->processed_sizes -= GL_IT(it)->minh;
   if (sd->processed_sizes < 0) sd->processed_sizes = 0;

   elm_widget_item_pre_notify_del(it);
   if (it->itc->func.del)
     it->itc->func.del((void *)it->base.data, WIDGET(it));
   if (it->tooltip.del_cb)
     it->tooltip.del_cb((void *)it->tooltip.data, WIDGET(it), it);

   if (GL_IT(it)->block) _item_block_del(it);
   if (it->parent)
     it->parent->item->items =
        eina_list_remove(it->parent->item->items, it);
   if (GL_IT(it)->queued)
     {
        GL_IT(it)->queued = EINA_FALSE;
        sd->queue = eina_list_remove(sd->queue, it);
     }
   if (GL_IT(it)->type == ELM_GENLIST_ITEM_GROUP)
      sd->group_items = eina_list_remove(sd->group_items, it);

   // FIXME: relative will be better to be fixed. it is too harsh.
   if (GL_IT(it)->rel)
     GL_IT(it)->rel->item->rel_revs =
        eina_list_remove(GL_IT(it)->rel->item->rel_revs, it);
   if (GL_IT(it)->rel_revs)
     {
        Elm_Gen_Item *tmp;
        EINA_LIST_FREE(GL_IT(it)->rel_revs, tmp) tmp->item->rel = NULL;
     }
   _item_sub_items_clear(it);

   sd->reorder.move_items = eina_list_remove(sd->reorder.move_items, it);
   if (it->selected)
      {
         sd->selected = eina_list_remove(sd->selected, it);
         it->selected = EINA_FALSE;
      }
   if (sd->show_item == it) sd->show_item = NULL;

   if ((sd->g_item) && (sd->g_item == it)) sd->g_item = NULL;
   if (sd->expanded_item == it) sd->expanded_item = NULL;
   if (sd->state) eina_inlist_sorted_state_free(sd->state);

   if (sd->last_selected_item == (Elm_Object_Item *)it)
     sd->last_selected_item = NULL;

   if (sd->realization_mode)
     {
        Eina_List *l;
        Evas_Object *c;
        EINA_LIST_FOREACH(GL_IT(it)->flip_content_objs, l, c)
          {
             evas_object_event_callback_del_full(c,
                                                 EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                                                 _changed_size_hints, it);
          }
        EINA_LIST_FOREACH(GL_IT(it)->deco_all_contents, l, c)
          {
             evas_object_event_callback_del_full(c,
                                                 EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                                                 _changed_size_hints, it);
          }
        EINA_LIST_FOREACH(it->content_objs, l, c)
          {
             evas_object_event_callback_del_full(c,
                                                 EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                                                 _changed_size_hints, it);
          }
     }

   if (sd->mode_item) sd->mode_item = NULL;
   if (it->selected) _item_unselect(it);
   if (it == sd->focused_item) _item_unfocused(it);
   if (it == sd->key_down_item) sd->key_down_item = NULL;

   _item_mouse_callbacks_del(it, VIEW(it));
   _item_edje_callbacks_del(it);
   if (GL_IT(it)->highlight_timer)
     {
        ecore_timer_del(GL_IT(it)->highlight_timer);
        GL_IT(it)->highlight_timer = NULL;
     }
   if (it->long_timer)
     {
        ecore_timer_del(it->long_timer);
        it->long_timer = NULL;
     }

   sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(it));
   sd->item_count--;
}

static void
_item_free(Elm_Gen_Item *it)
{
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   _item_free_common(it);
   _item_unrealize(it, EINA_FALSE);
   elm_genlist_item_class_unref((Elm_Genlist_Item_Class *)it->itc);
   free(GL_IT(it));
   GL_IT(it) = NULL;

   evas_object_smart_changed(sd->pan_obj);
}

static Eina_Bool
_item_del_pre_hook(Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   _item_free(it);
   return EINA_TRUE;
}

static void
_item_signal_emit_hook(Elm_Object_Item *it,
                       const char *emission,
                       const char *source)
{
   Elm_Gen_Item *item = (Elm_Gen_Item *)it;
   if (!item->realized)
     {
        WRN("item is not realized yet");
        return;
     }
   edje_object_signal_emit
      (VIEW(it), emission, source);
   if (item->deco_all_view)
     edje_object_signal_emit
       (item->deco_all_view, emission, source);
}

static Elm_Gen_Item *
_elm_genlist_item_new(Elm_Genlist_Smart_Data *sd,
                      const Elm_Genlist_Item_Class *itc,
                      const void *data,
                      Elm_Gen_Item *parent,
                      Elm_Genlist_Item_Type type,
                      Evas_Smart_Cb func,
                      const void *func_data)
{
   Elm_Gen_Item *it, *it2;
   int depth = 0;

   if (!itc) return NULL;

   it = elm_widget_item_new(sd->obj, Elm_Gen_Item);
   if (!it) return NULL;

   it->itc = itc;
   elm_genlist_item_class_ref((Elm_Genlist_Item_Class *)itc);

   it->base.data = data;
   it->parent = parent;
   it->func.func = func;
   it->func.data = func_data;

   elm_widget_item_content_get_hook_set(it, _item_content_get_hook);
   elm_widget_item_text_get_hook_set(it, _item_text_get_hook);

   elm_widget_item_disable_hook_set(it, _item_disable_hook);
   elm_widget_item_del_pre_hook_set(it, _item_del_pre_hook);
   elm_widget_item_signal_emit_hook_set(it, _item_signal_emit_hook);

   GL_IT(it) = ELM_NEW(Elm_Gen_Item_Type);
   GL_IT(it)->wsd = sd;
   GL_IT(it)->type = type;

   if (it->parent)
     {
        if (GL_IT(it->parent)->type == ELM_GENLIST_ITEM_GROUP)
          GL_IT(it)->group_item = parent;
        else if (GL_IT(it->parent)->group_item)
          GL_IT(it)->group_item = GL_IT(it->parent)->group_item;
     }
   for (it2 = it, depth = 0; it2->parent; it2 = it2->parent)
     {
        if (GL_IT(it2->parent)->type == ELM_GENLIST_ITEM_TREE) depth += 1;
     }
   GL_IT(it)->expanded_depth = depth;
   sd->item_count++;

   return it;
}

static int
_elm_genlist_item_compare(const void *data,
                          const void *data1)
{
   const Elm_Gen_Item *it, *item1;

   it = ELM_GEN_ITEM_FROM_INLIST(data);
   item1 = ELM_GEN_ITEM_FROM_INLIST(data1);
   return GL_IT(it)->wsd->item_compare_cb(it, item1);
}

static int
_elm_genlist_item_list_compare(const void *data,
                               const void *data1)
{
   const Elm_Gen_Item *it = data;
   const Elm_Gen_Item *item1 = data1;

   return GL_IT(it)->wsd->item_compare_cb(it, item1);
}

EAPI unsigned int
elm_genlist_items_count(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) 0;
   unsigned int ret = 0;
   eo_do((Eo *) obj, elm_obj_genlist_items_count(&ret));
   return ret;
}

static void
_items_count(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   unsigned int *ret = va_arg(*list, unsigned int *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->item_count;
}

EAPI Elm_Object_Item *
elm_genlist_item_append(Evas_Object *obj,
                        const Elm_Genlist_Item_Class *itc,
                        const void *data,
                        Elm_Object_Item *parent,
                        Elm_Genlist_Item_Type type,
                        Evas_Smart_Cb func,
                        const void *func_data)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do(obj, elm_obj_genlist_item_append(itc, data, parent, type, func, func_data, &ret));
   return ret;
}

static void
_item_append(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Gen_Item *it;

   const Elm_Genlist_Item_Class *itc = va_arg(*list, const Elm_Genlist_Item_Class *);
   const void *data = va_arg(*list, const void *);
   Elm_Object_Item *parent = va_arg(*list, Elm_Object_Item *);
   Elm_Genlist_Item_Type type = va_arg(*list, Elm_Genlist_Item_Type);
   Evas_Smart_Cb func = va_arg(*list, Evas_Smart_Cb);
   const void *func_data = va_arg(*list, const void *);
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   Elm_Genlist_Smart_Data *sd = _pd;
   if (ret) *ret = NULL;

   EINA_SAFETY_ON_NULL_RETURN(itc);
   if (parent)
     {
        ELM_GENLIST_ITEM_CHECK_OR_RETURN(parent);
        EINA_SAFETY_ON_FALSE_RETURN(obj == WIDGET(parent));
     }
   it = _elm_genlist_item_new
       (sd, itc, data, (Elm_Gen_Item *)parent, type, func, func_data);
   if (!it) return;

   if (!it->parent)
     {
        if (GL_IT(it)->type == ELM_GENLIST_ITEM_GROUP)
          sd->group_items = eina_list_append(sd->group_items, it);
        sd->items = eina_inlist_append(sd->items, EINA_INLIST_GET(it));
        GL_IT(it)->rel = NULL;
     }
   else
     {
        Elm_Gen_Item *it2 = NULL;
        Eina_List *ll = eina_list_last(it->parent->item->items);

        if (ll) it2 = ll->data;
        it->parent->item->items =
          eina_list_append(it->parent->item->items, it);
        if (!it2) it2 = it->parent;
        sd->items = eina_inlist_append_relative
          (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(it2));
        GL_IT(it)->rel = it2;
        it2->item->rel_revs = eina_list_append(it2->item->rel_revs, it);
     }
   GL_IT(it)->before = EINA_FALSE;
   _item_queue_direct(it, NULL);

   if (ret) *ret = (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_item_prepend(Evas_Object *obj,
                         const Elm_Genlist_Item_Class *itc,
                         const void *data,
                         Elm_Object_Item *parent,
                         Elm_Genlist_Item_Type type,
                         Evas_Smart_Cb func,
                         const void *func_data)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do(obj, elm_obj_genlist_item_prepend(itc, data, parent, type, func, func_data, &ret));
   return ret;
}

static void
_item_prepend(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Gen_Item *it;

   const Elm_Genlist_Item_Class *itc = va_arg(*list, const Elm_Genlist_Item_Class *);
   const void *data = va_arg(*list, const void *);
   Elm_Object_Item *parent = va_arg(*list, Elm_Object_Item *);
   Elm_Genlist_Item_Type type = va_arg(*list, Elm_Genlist_Item_Type);
   Evas_Smart_Cb func = va_arg(*list, Evas_Smart_Cb);
   const void *func_data = va_arg(*list, const void *);
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   Elm_Genlist_Smart_Data *sd = _pd;
   if (ret) *ret = NULL;

   EINA_SAFETY_ON_NULL_RETURN(itc);
   if (parent)
     {
        ELM_GENLIST_ITEM_CHECK_OR_RETURN(parent);
        EINA_SAFETY_ON_FALSE_RETURN(obj == WIDGET(parent));
     }

   it = _elm_genlist_item_new
       (sd, itc, data, (Elm_Gen_Item *)parent, type, func, func_data);
   if (!it) return;

   if (sd->items) GL_IT(it)->is_prepend = EINA_TRUE;
   if (!it->parent)
     {
        if (GL_IT(it)->type == ELM_GENLIST_ITEM_GROUP)
          sd->group_items = eina_list_prepend(sd->group_items, it);
        sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(it));
        GL_IT(it)->rel = NULL;
     }
   else
     {
        Elm_Gen_Item *it2 = NULL;
        Eina_List *ll = it->parent->item->items;

        if (ll) it2 = ll->data;
        it->parent->item->items =
          eina_list_prepend(it->parent->item->items, it);
        if (!it2) it2 = it->parent;
        sd->items = eina_inlist_prepend_relative
            (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(it2));
        GL_IT(it)->rel = it2;
        it2->item->rel_revs = eina_list_append(it2->item->rel_revs, it);
     }
   GL_IT(it)->before = EINA_TRUE;
   _item_queue_direct(it, NULL);

   if (ret) *ret = (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_item_insert_after(Evas_Object *obj,
                              const Elm_Genlist_Item_Class *itc,
                              const void *data,
                              Elm_Object_Item *parent,
                              Elm_Object_Item *after_it,
                              Elm_Genlist_Item_Type type,
                              Evas_Smart_Cb func,
                              const void *func_data)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do(obj, elm_obj_genlist_item_insert_after(itc, data, parent, after_it, type, func, func_data, &ret));
   return ret;
}

static void
_item_insert_after(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   const Elm_Genlist_Item_Class *itc = va_arg(*list, const Elm_Genlist_Item_Class *);
   const void *data = va_arg(*list, const void *);
   Elm_Object_Item *parent = va_arg(*list, Elm_Object_Item *);
   Elm_Object_Item *after_it = va_arg(*list, Elm_Object_Item *);
   Elm_Genlist_Item_Type type = va_arg(*list, Elm_Genlist_Item_Type);
   Evas_Smart_Cb func = va_arg(*list, Evas_Smart_Cb);
   const void *func_data = va_arg(*list, const void *);
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);

   if (ret) *ret = NULL;

   Elm_Gen_Item *after = (Elm_Gen_Item *)after_it;
   Elm_Gen_Item *it;

   ELM_GENLIST_ITEM_CHECK(after_it);
   Elm_Genlist_Smart_Data *sd = _pd;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(after_it);
   EINA_SAFETY_ON_FALSE_RETURN(obj == WIDGET(after_it));
   if (parent)
     {
        ELM_GENLIST_ITEM_CHECK_OR_RETURN(parent);
        EINA_SAFETY_ON_FALSE_RETURN(obj == WIDGET(parent));
     }

   /* It makes no sense to insert after in an empty list with after !=
    * NULL, something really bad is happening in your app. */
   EINA_SAFETY_ON_NULL_RETURN(sd->items);

   it = _elm_genlist_item_new
       (sd, itc, data, (Elm_Gen_Item *)parent, type, func, func_data);
   if (!it) return;

   if (!it->parent)
     {
        if ((GL_IT(it)->type == ELM_GENLIST_ITEM_GROUP) &&
            (GL_IT(after)->type == ELM_GENLIST_ITEM_GROUP))
          sd->group_items = eina_list_append_relative
              (sd->group_items, it, after);
     }
   else
     {
        it->parent->item->items =
          eina_list_append_relative(it->parent->item->items, it, after);
     }
   sd->items = eina_inlist_append_relative
       (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(after));

   GL_IT(it)->rel = after;
   after->item->rel_revs = eina_list_append(after->item->rel_revs, it);
   GL_IT(it)->before = EINA_FALSE;
   _item_queue_direct(it, NULL);

   if (ret) *ret = (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_item_insert_before(Evas_Object *obj,
                               const Elm_Genlist_Item_Class *itc,
                               const void *data,
                               Elm_Object_Item *parent,
                               Elm_Object_Item *before_it,
                               Elm_Genlist_Item_Type type,
                               Evas_Smart_Cb func,
                               const void *func_data)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do(obj, elm_obj_genlist_item_insert_before(itc, data, parent, before_it, type, func, func_data, &ret));
   return ret;
}

static void
_item_insert_before(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   const Elm_Genlist_Item_Class *itc = va_arg(*list, const Elm_Genlist_Item_Class *);
   const void *data = va_arg(*list, const void *);
   Elm_Object_Item *parent = va_arg(*list, Elm_Object_Item *);
   Elm_Object_Item *before_it = va_arg(*list, Elm_Object_Item *);
   Elm_Genlist_Item_Type type = va_arg(*list, Elm_Genlist_Item_Type);
   Evas_Smart_Cb func = va_arg(*list, Evas_Smart_Cb);
   const void *func_data = va_arg(*list, const void *);
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   Elm_Gen_Item *before = (Elm_Gen_Item *)before_it;
   Elm_Gen_Item *it;

   if (ret) *ret = NULL;

   Elm_Genlist_Smart_Data *sd = _pd;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(before_it);
   EINA_SAFETY_ON_FALSE_RETURN(obj == WIDGET(before_it));
   if (parent)
     {
        ELM_GENLIST_ITEM_CHECK_OR_RETURN(parent);
        EINA_SAFETY_ON_FALSE_RETURN(obj == WIDGET(parent));
     }

   /* It makes no sense to insert before in an empty list with before
    * != NULL, something really bad is happening in your app. */
   EINA_SAFETY_ON_NULL_RETURN(sd->items);

   it = _elm_genlist_item_new
       (sd, itc, data, (Elm_Gen_Item *)parent, type, func, func_data);
   if (!it) return;

   if (!it->parent)
     {
        if ((GL_IT(it)->type == ELM_GENLIST_ITEM_GROUP) &&
            (GL_IT(before)->type == ELM_GENLIST_ITEM_GROUP))
          sd->group_items =
            eina_list_prepend_relative(sd->group_items, it, before);
     }
   else
     {
        it->parent->item->items =
          eina_list_prepend_relative(it->parent->item->items, it, before);
     }
   sd->items = eina_inlist_prepend_relative
       (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(before));

   GL_IT(it)->rel = before;
   before->item->rel_revs = eina_list_append(before->item->rel_revs, it);
   GL_IT(it)->before = EINA_TRUE;
   _item_queue_direct(it, NULL);

   if (*ret) *ret = (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_item_sorted_insert(Evas_Object *obj,
                               const Elm_Genlist_Item_Class *itc,
                               const void *data,
                               Elm_Object_Item *parent,
                               Elm_Genlist_Item_Type type,
                               Eina_Compare_Cb comp,
                               Evas_Smart_Cb func,
                               const void *func_data)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do(obj, elm_obj_genlist_item_sorted_insert(itc, data, parent, type, comp, func, func_data, &ret));
   return ret;
}

static void
_item_sorted_insert(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Gen_Item *rel = NULL;
   Elm_Gen_Item *it;

   const Elm_Genlist_Item_Class *itc = va_arg(*list, const Elm_Genlist_Item_Class *);
   const void *data = va_arg(*list, const void *);
   Elm_Object_Item *parent = va_arg(*list, Elm_Object_Item *);
   Elm_Genlist_Item_Type type = va_arg(*list, Elm_Genlist_Item_Type);
   Eina_Compare_Cb comp = va_arg(*list, Eina_Compare_Cb);
   Evas_Smart_Cb func = va_arg(*list, Evas_Smart_Cb);
   const void *func_data = va_arg(*list, const void *);
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   if (ret) *ret = NULL;

   Elm_Genlist_Smart_Data *sd = _pd;

   if (parent)
     {
        ELM_GENLIST_ITEM_CHECK_OR_RETURN(parent);
        EINA_SAFETY_ON_FALSE_RETURN(obj == WIDGET(parent));
     }

   it = _elm_genlist_item_new
       (sd, itc, data, (Elm_Gen_Item *)parent, type, func, func_data);
   if (!it) return;

   sd->item_compare_cb = comp;

   if (it->parent)
     {
        Eina_List *l;
        int cmp_result;

        l = eina_list_search_sorted_near_list
            (it->parent->item->items, _elm_genlist_item_list_compare, it,
            &cmp_result);

        if (l)
          rel = eina_list_data_get(l);
        else
          rel = it->parent;

        if (cmp_result >= 0)
          {
             it->parent->item->items = eina_list_prepend_relative_list
                 (it->parent->item->items, it, l);
             sd->items = eina_inlist_prepend_relative
                 (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(rel));
             it->item->before = EINA_FALSE;
          }
        else if (cmp_result < 0)
          {
             it->parent->item->items = eina_list_append_relative_list
                 (it->parent->item->items, it, l);
             sd->items = eina_inlist_append_relative
                 (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(rel));
             it->item->before = EINA_TRUE;
          }
     }
   else
     {
        if (!sd->state)
          {
             sd->state = eina_inlist_sorted_state_new();
             eina_inlist_sorted_state_init(sd->state, sd->items);
             sd->requeued = EINA_FALSE;
          }

        if (it->group)
          sd->group_items = eina_list_append(sd->group_items, it);

        sd->items = eina_inlist_sorted_state_insert
            (sd->items, EINA_INLIST_GET(it), _elm_genlist_item_compare,
            sd->state);

        if (EINA_INLIST_GET(it)->next)
          {
             rel = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next);
             it->item->before = EINA_TRUE;
          }
        else if (EINA_INLIST_GET(it)->prev)
          {
             rel = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->prev);
             it->item->before = EINA_FALSE;
          }
     }

   if (rel)
     {
        it->item->rel = rel;
        rel->item->rel_revs = eina_list_append(rel->item->rel_revs, it);
     }

   _item_queue(it, _elm_genlist_item_list_compare);

   if (ret) *ret = (Elm_Object_Item *)it;
}

EAPI void
elm_genlist_clear(Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_clear());
}

static void
_clear_eo(Eo *obj, void *_pd, va_list *list EINA_UNUSED)
{
   Elm_Genlist_Smart_Data *sd = _pd;
   Elm_Gen_Item *it;

   eina_hash_free_buckets(sd->size_caches);
   _item_unfocused(sd->focused_item);
   if (sd->key_down_item) sd->key_down_item = NULL;
   if (sd->mode_item) sd->mode_item = NULL;

   if (sd->state)
     {
        eina_inlist_sorted_state_free(sd->state);
        sd->state = NULL;
     }

   // Do not use EINA_INLIST_FOREACH or EINA_INLIST_FOREACH_SAFE
   // because sd->items can be modified inside elm_widget_item_del()
   while (sd->items)
     {
        it = EINA_INLIST_CONTAINER_GET(sd->items->last, Elm_Gen_Item);
        elm_widget_item_del(it);
     }
   sd->reorder.it = NULL;
   sd->reorder.dir = 0;
   eina_list_free(sd->reorder.move_items);

   sd->items = NULL;
   sd->blocks = NULL;
   _item_cache_all_free(sd);

   if (sd->selected)
     {
        sd->selected = eina_list_free(sd->selected);
        sd->selected = NULL;
     }

   sd->show_item = NULL;

   sd->pan_x = 0;
   sd->pan_y = 0;
   sd->minw = 0;
   sd->minh = 0;

   if (sd->pan_obj)
     {
        evas_object_size_hint_min_set(sd->pan_obj, sd->minw, sd->minh);
        evas_object_smart_callback_call(sd->pan_obj, "changed", NULL);
     }
   elm_layout_sizing_eval(sd->obj);
   eo_do(obj, elm_scrollable_interface_content_region_show(0, 0, 0, 0));

   if (sd->dummy_job)
     {
        ecore_job_del(sd->dummy_job);
        sd->dummy_job = NULL;
     }
   if (sd->queue_idle_enterer)
     {
        ecore_idle_enterer_del(sd->queue_idle_enterer);
        sd->queue_idle_enterer = NULL;
     }
   if (sd->scr_hold_timer)
     {
        ecore_timer_del(sd->scr_hold_timer);
        sd->scr_hold_timer = NULL;
     }
   if (sd->reorder.anim)
     {
        ecore_animator_del(sd->reorder.anim);
        sd->reorder.anim = NULL;
     }
   if (sd->scr_timer)
     {
        ecore_timer_del(sd->scr_timer);
        sd->scr_timer = NULL;
     }

   if (sd->queue) sd->queue = eina_list_free(sd->queue);
   if (sd->g_item) sd->g_item = NULL;
   if (sd->g_type) sd->g_type = NULL;
}

EAPI void
elm_genlist_multi_select_set(Evas_Object *obj,
                             Eina_Bool multi)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_multi_select_set(multi));
}

static void
_multi_select_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool multi = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;

   sd->multi = !!multi;
   if (!sd->multi && sd->selected)
     {
        Eina_List *l, *ll;
        Elm_Gen_Item *it;
        Elm_Gen_Item *last = sd->selected->data;
        EINA_LIST_FOREACH_SAFE(sd->selected, l, ll, it)
           if (last != it) _item_unselect(it);
     }
}

EAPI Eina_Bool
elm_genlist_multi_select_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, elm_obj_genlist_multi_select_get(&ret));
   return ret;
}

static void
_multi_select_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->multi;
}

EAPI void
elm_genlist_multi_select_mode_set(Evas_Object *obj,
                                  Elm_Object_Multi_Select_Mode mode)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_multi_select_mode_set(mode));
}

static void
_multi_select_mode_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Object_Multi_Select_Mode mode = va_arg(*list, Elm_Object_Multi_Select_Mode);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (mode >= ELM_OBJECT_MULTI_SELECT_MODE_MAX)
     return;

   if (sd->multi_select_mode != mode)
     sd->multi_select_mode = mode;
}

EAPI Elm_Object_Multi_Select_Mode
elm_genlist_multi_select_mode_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) ELM_OBJECT_MULTI_SELECT_MODE_MAX;
   Elm_Object_Multi_Select_Mode ret = ELM_OBJECT_MULTI_SELECT_MODE_MAX;
   eo_do((Eo *)obj, elm_obj_genlist_multi_select_mode_get(&ret));
   return ret;
}

static void
_multi_select_mode_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Object_Multi_Select_Mode *ret = va_arg(*list, Elm_Object_Multi_Select_Mode *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->multi_select_mode;
}

EAPI Elm_Object_Item *
elm_genlist_selected_item_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do((Eo *) obj, elm_obj_genlist_selected_item_get(&ret));
   return ret;
}

static void
_selected_item_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (sd->selected)
      *ret = sd->selected->data;
   else
      *ret = NULL;
}

EAPI const Eina_List *
elm_genlist_selected_items_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) NULL;
   const Eina_List *ret = NULL;
   eo_do((Eo *) obj, elm_obj_genlist_selected_items_get(&ret));
   return ret;
}

static void
_selected_items_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   const Eina_List **ret = va_arg(*list, const Eina_List **);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->selected;
}

EAPI Eina_List *
elm_genlist_realized_items_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Eina_List *ret = NULL;
   eo_do((Eo *) obj, elm_obj_genlist_realized_items_get(&ret));
   return ret;
}

static void
_realized_items_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Item_Block *itb;
   Eina_Bool done = EINA_FALSE;

   Eina_List **ret = va_arg(*list, Eina_List **);
   Elm_Genlist_Smart_Data *sd = _pd;
   if (ret) *ret = NULL;

   EINA_INLIST_FOREACH(sd->blocks, itb)
     {
        if (itb->realized)
          {
             Eina_List *l;
             Elm_Gen_Item *it;

             done = EINA_TRUE;
             EINA_LIST_FOREACH(itb->items, l, it)
               {
                  if (it->realized && ret) *ret = eina_list_append(*ret, it);
               }
          }
        else
          {
             if (done) break;
          }
     }
}

EAPI Elm_Object_Item *
elm_genlist_at_xy_item_get(const Evas_Object *obj,
                           Evas_Coord x,
                           Evas_Coord y,
                           int *posret)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do((Eo *) obj, elm_obj_genlist_at_xy_item_get(x, y, posret, &ret));
   return ret;
}

static void
_at_xy_item_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Evas_Coord ox, oy, ow, oh;
   Evas_Coord lasty;
   Item_Block *itb;

   Evas_Coord x = va_arg(*list, Evas_Coord);
   Evas_Coord y = va_arg(*list, Evas_Coord);
   int *posret = va_arg(*list, int *);
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   Elm_Genlist_Smart_Data *sd = _pd;

   evas_object_geometry_get(sd->pan_obj, &ox, &oy, &ow, &oh);
   lasty = oy;
   EINA_INLIST_FOREACH(sd->blocks, itb)
     {
        Eina_List *l;
        Elm_Gen_Item *it;
        if (!ELM_RECTS_INTERSECT(ox + itb->x - itb->sd->pan_x,
                                 oy + itb->y - itb->sd->pan_y,
                                 sd->minw, itb->minh, x, y, 1, 1))
          continue;
        EINA_LIST_FOREACH(itb->items, l, it)
          {
             Evas_Coord itx, ity;

             itx = ox + itb->x + it->x - itb->sd->pan_x;
             ity = oy + itb->y + it->y - itb->sd->pan_y;
             if (ELM_RECTS_INTERSECT
                   (itx, ity, GL_IT(it)->w, GL_IT(it)->h, x, y, 1, 1))
               {
                  if (posret)
                    {
                       if (y <= (ity + (GL_IT(it)->h / 4))) *posret = -1;
                       else if (y >= (ity + GL_IT(it)->h - (GL_IT(it)->h / 4)))
                         *posret = 1;
                       else *posret = 0;
                    }

                  *ret = (Elm_Object_Item *)it;
                  return;
               }
             lasty = ity + GL_IT(it)->h;
          }
     }
   if (posret)
     {
        if (y > lasty) *posret = 1;
        else *posret = -1;
     }


   if (ret) *ret = NULL;
}

EAPI Elm_Object_Item *
elm_genlist_first_item_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do((Eo *) obj, elm_obj_genlist_first_item_get(&ret));
   return ret;
}

static void
_first_item_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Gen_Item *tit, *it = NULL;

   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   Elm_Genlist_Smart_Data *sd = _pd;
   if (ret) *ret = NULL;

   if (!sd->items) return;

   EINA_INLIST_REVERSE_FOREACH(sd->items, tit) it = tit;

   if (ret) *ret = (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_last_item_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do((Eo *) obj, elm_obj_genlist_last_item_get(&ret));
   return ret;
}

static void
_last_item_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Gen_Item *it;

   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   if (ret) *ret = NULL;

   Elm_Genlist_Smart_Data *sd = _pd;

   if (!sd->items) return;

   it = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);

   if (ret) *ret = (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_item_next_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *it = NULL;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, NULL);

   it = (Elm_Gen_Item *)item;
   while (it)
     {
        it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next);
        if (it) break;
     }

   return (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_item_prev_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *it = NULL;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, NULL);

   it = (Elm_Gen_Item *)item;
   while (it)
     {
        it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->prev);
        if (it) break;
     }

   return (Elm_Object_Item *)it;
}

EAPI Elm_Object_Item *
elm_genlist_item_parent_get(const Elm_Object_Item *it)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, NULL);

   return (Elm_Object_Item *)((Elm_Gen_Item *)it)->parent;
}

EAPI void
elm_genlist_item_subitems_clear(Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   _item_sub_items_clear(it);
}

EAPI void
elm_genlist_item_selected_set(Elm_Object_Item *item,
                              Eina_Bool selected)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   if (elm_widget_item_disabled_get(it)) return;
   // FIXME: This API has highlight/unhighlith feature also..
   if (selected) _item_highlight(it);
   else _item_unhighlight(it);

   if (it->selected == selected) return;

   if (selected) _item_select(it);
   else _item_unselect(it);
}

EAPI Eina_Bool
elm_genlist_item_selected_get(const Elm_Object_Item *it)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, EINA_FALSE);

   return ((Elm_Gen_Item *)it)->selected;
}

EAPI void
elm_genlist_item_expanded_set(Elm_Object_Item *item,
                              Eina_Bool expanded)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   expanded = !!expanded;
   if (GL_IT(it)->expanded == expanded) return;
   if (GL_IT(it)->type != ELM_GENLIST_ITEM_TREE) return;
   GL_IT(it)->expanded = expanded;
   GL_IT(it)->wsd->expanded_item = it;

   if (GL_IT(it)->expanded)
     {
        if (it->realized)
          edje_object_signal_emit(VIEW(it), SIGNAL_EXPANDED, "elm");
        evas_object_smart_callback_call(WIDGET(it), SIG_EXPANDED, it);
     }
   else
     {
        if (it->realized)
          edje_object_signal_emit(VIEW(it), SIGNAL_CONTRACTED, "elm");
        evas_object_smart_callback_call(WIDGET(it), SIG_CONTRACTED, it);
     }}

EAPI Eina_Bool
elm_genlist_item_expanded_get(const Elm_Object_Item *it)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, EINA_FALSE);

   return ((Elm_Gen_Item *)it)->item->expanded;
}

EAPI int
elm_genlist_item_expanded_depth_get(const Elm_Object_Item *it)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, 0);

   return ((Elm_Gen_Item *)it)->item->expanded_depth;
}

EAPI void
elm_genlist_item_promote(Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   _item_move_before
     (it, (Elm_Gen_Item *)elm_genlist_first_item_get(WIDGET(it)));
}

EAPI void
elm_genlist_item_demote(Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   _item_move_after(it, (Elm_Gen_Item *)elm_genlist_last_item_get(WIDGET(it)));
}

EAPI void
elm_genlist_item_show(Elm_Object_Item *item,
                      Elm_Genlist_Item_Scrollto_Type type)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   sd->show_item = it;
   sd->bring_in = EINA_FALSE;
   sd->scroll_to_type = type;
   evas_object_smart_changed(sd->pan_obj);
}

EAPI void
elm_genlist_item_bring_in(Elm_Object_Item *item,
                          Elm_Genlist_Item_Scrollto_Type type)
{

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;
   sd->show_item = it;
   sd->bring_in = EINA_TRUE;
   sd->scroll_to_type = type;
   evas_object_smart_changed(sd->pan_obj);
}

EAPI void
elm_genlist_item_all_contents_unset(Elm_Object_Item *item,
                                    Eina_List **l)
{
   Evas_Object *content;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   EINA_LIST_FREE (it->content_objs, content)
     {
        evas_object_smart_member_del(content);
        evas_object_hide(content);
        if (l) *l = eina_list_append(*l, content);
     }
}

EAPI void
elm_genlist_item_update(Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   if (!GL_IT(it)->block) return;

   GL_IT(it)->updateme = EINA_TRUE;
   GL_IT(it)->block->calc_done = EINA_FALSE;
   GL_IT(it)->wsd->calc_done = EINA_FALSE;
   evas_object_smart_changed(GL_IT(it)->wsd->pan_obj);
}

EAPI void
elm_genlist_item_fields_update(Elm_Object_Item *item,
                               const char *parts,
                               Elm_Genlist_Item_Field_Type itf)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   if (!GL_IT(it)->block) return;

   if ((!itf) || (itf & ELM_GENLIST_ITEM_FIELD_TEXT))
     {
        _item_text_realize(it, VIEW(it), parts);
     }
   if ((!itf) || (itf & ELM_GENLIST_ITEM_FIELD_CONTENT))
     {
        it->content_objs = _item_content_realize
           (it, VIEW(it), it->content_objs, "contents", parts);
        if (it->flipped)
          {
             GL_IT(it)->flip_content_objs =
               _item_content_realize(it, VIEW(it), GL_IT(it)->flip_content_objs,
                                     "flips", parts);
          }
        if (GL_IT(it)->deco_it_view)
          {
             GL_IT(it)->deco_it_contents =
               _item_content_realize(it, GL_IT(it)->deco_it_view,
                                     GL_IT(it)->deco_it_contents,
                                     "contents", parts);
          }
        if (GL_IT(it)->wsd->decorate_all_mode)
          {
             GL_IT(it)->deco_all_contents =
               _item_content_realize(it, it->deco_all_view,
                                     GL_IT(it)->deco_all_contents,
                                     "contents", parts);
          }
     }

   if ((!itf) || (itf & ELM_GENLIST_ITEM_FIELD_STATE))
     _item_state_realize(it, VIEW(it), parts);

   GL_IT(it)->calc_done = EINA_FALSE;
   GL_IT(it)->block->calc_done = EINA_FALSE;
   GL_IT(it)->wsd->calc_done = EINA_FALSE;
    evas_object_smart_changed(GL_IT(it)->wsd->pan_obj);}

EAPI void
elm_genlist_item_item_class_update(Elm_Object_Item *item,
                                   const Elm_Genlist_Item_Class *itc)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   EINA_SAFETY_ON_NULL_RETURN(itc);
   it->itc = itc;

   if (!GL_IT(it)->block) return;

   _view_theme_update(it, VIEW(it), it->itc->item_style);
   elm_genlist_item_update(item);
}

EAPI const Elm_Genlist_Item_Class *
elm_genlist_item_item_class_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, NULL);

   return it->itc;
}

static Evas_Object *
_elm_genlist_item_label_create(void *data,
                               Evas_Object *obj EINA_UNUSED,
                               Evas_Object *tooltip,
                               void *it EINA_UNUSED)
{
   Evas_Object *label = elm_label_add(tooltip);

   if (!label)
     return NULL;

   elm_object_style_set(label, "tooltip");
   elm_object_text_set(label, data);

   return label;
}

static void
_elm_genlist_item_label_del_cb(void *data,
                               Evas_Object *obj EINA_UNUSED,
                               void *event_info EINA_UNUSED)
{
   eina_stringshare_del(data);
}

EAPI void
elm_genlist_item_tooltip_text_set(Elm_Object_Item *it,
                                  const char *text)
{
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it);

   text = eina_stringshare_add(text);
   elm_genlist_item_tooltip_content_cb_set
     (it, _elm_genlist_item_label_create, text,
     _elm_genlist_item_label_del_cb);
}

EAPI void
elm_genlist_item_tooltip_content_cb_set(Elm_Object_Item *item,
                                        Elm_Tooltip_Item_Content_Cb func,
                                        const void *data,
                                        Evas_Smart_Cb del_cb)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_GOTO(item, error);

   if ((it->tooltip.content_cb == func) && (it->tooltip.data == data))
     return;

   if (it->tooltip.del_cb)
     it->tooltip.del_cb((void *)it->tooltip.data, WIDGET(it), it);

   it->tooltip.content_cb = func;
   it->tooltip.data = data;
   it->tooltip.del_cb = del_cb;

   if (VIEW(it))
     {
        elm_widget_item_tooltip_content_cb_set
          (it, it->tooltip.content_cb, it->tooltip.data, NULL);
        elm_widget_item_tooltip_style_set(it, it->tooltip.style);
        elm_widget_item_tooltip_window_mode_set(it, it->tooltip.free_size);
     }

   return;

error:
   if (del_cb) del_cb((void *)data, NULL, NULL);
}

EAPI void
elm_genlist_item_tooltip_unset(Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   if ((VIEW(it)) && (it->tooltip.content_cb))
     elm_widget_item_tooltip_unset(it);

   if (it->tooltip.del_cb)
     it->tooltip.del_cb((void *)it->tooltip.data, WIDGET(it), it);
   it->tooltip.del_cb = NULL;
   it->tooltip.content_cb = NULL;
   it->tooltip.data = NULL;
   it->tooltip.free_size = EINA_FALSE;
   if (it->tooltip.style)
     elm_genlist_item_tooltip_style_set(item, NULL);
}

EAPI void
elm_genlist_item_tooltip_style_set(Elm_Object_Item *item,
                                   const char *style)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   eina_stringshare_replace(&it->tooltip.style, style);
   if (VIEW(it)) elm_widget_item_tooltip_style_set(it, style);
}

EAPI const char *
elm_genlist_item_tooltip_style_get(const Elm_Object_Item *it)
{
   return elm_object_item_tooltip_style_get(it);
}

EAPI Eina_Bool
elm_genlist_item_tooltip_window_mode_set(Elm_Object_Item *item,
                                         Eina_Bool disable)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, EINA_FALSE);

   it->tooltip.free_size = disable;
   if (VIEW(it)) return elm_widget_item_tooltip_window_mode_set(it, disable);

   return EINA_TRUE;
}

EAPI Eina_Bool
elm_genlist_item_tooltip_window_mode_get(const Elm_Object_Item *it)
{
   return elm_object_tooltip_window_mode_get(VIEW(it));
}

EAPI void
elm_genlist_item_cursor_set(Elm_Object_Item *item,
                            const char *cursor)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   eina_stringshare_replace(&it->mouse_cursor, cursor);
   if (VIEW(it)) elm_widget_item_cursor_set(it, cursor);
}

EAPI const char *
elm_genlist_item_cursor_get(const Elm_Object_Item *it)
{
   return elm_widget_item_cursor_get(it);
}

EAPI void
elm_genlist_item_cursor_unset(Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   if (!it->mouse_cursor) return;

   if (VIEW(it)) elm_widget_item_cursor_unset(it);

   ELM_SAFE_FREE(it->mouse_cursor, eina_stringshare_del);
}

EAPI void
elm_genlist_item_cursor_style_set(Elm_Object_Item *it,
                                  const char *style)
{
   elm_widget_item_cursor_style_set(it, style);
}

EAPI const char *
elm_genlist_item_cursor_style_get(const Elm_Object_Item *it)
{
   return elm_widget_item_cursor_style_get(it);
}

EAPI void
elm_genlist_item_cursor_engine_only_set(Elm_Object_Item *it,
                                        Eina_Bool engine_only)
{
   elm_widget_item_cursor_engine_only_set(it, engine_only);
}

EAPI Eina_Bool
elm_genlist_item_cursor_engine_only_get(const Elm_Object_Item *it)
{
   return elm_widget_item_cursor_engine_only_get(it);
}

EAPI int
elm_genlist_item_index_get(const Elm_Object_Item *item)
{
   int cnt = 0;
   Elm_Gen_Item *tmp;
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, -1);

   EINA_INLIST_FOREACH(GL_IT(it)->wsd->items, tmp)
     {
        if (tmp == it) break;
        cnt++;
     }
   return cnt;
}

EAPI void
elm_genlist_mode_set(Evas_Object *obj,
                     Elm_List_Mode mode)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_mode_set(mode));
}

static void
_mode_set(Eo *obj, void *_pd, va_list *list)
{
   Elm_List_Mode mode = va_arg(*list, Elm_List_Mode);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (sd->mode == mode) return;
   sd->mode = mode;
   elm_layout_sizing_eval(obj);
}

EAPI Elm_List_Mode
elm_genlist_mode_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) ELM_LIST_LAST;
   Elm_List_Mode ret = ELM_LIST_LAST;
   eo_do((Eo *) obj, elm_obj_genlist_mode_get(&ret));
   return ret;
}

static void
_mode_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_List_Mode *ret = va_arg(*list, Elm_List_Mode *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->mode;
}

EAPI void
elm_genlist_bounce_set(Evas_Object *obj,
                       Eina_Bool h_bounce,
                       Eina_Bool v_bounce)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_scrollable_interface_bounce_allow_set(h_bounce, v_bounce));
}

static void
_bounce_set(Eo *obj, void *_pd, va_list *list)
{
   Eina_Bool h_bounce = va_arg(*list, int);
   Eina_Bool v_bounce = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;

   sd->h_bounce = !!h_bounce;
   sd->v_bounce = !!v_bounce;
   eo_do_super(obj, MY_CLASS, elm_scrollable_interface_bounce_allow_set
         (sd->h_bounce, sd->v_bounce));
}

EAPI void
elm_genlist_bounce_get(const Evas_Object *obj,
                       Eina_Bool *h_bounce,
                       Eina_Bool *v_bounce)
{
   ELM_GENLIST_CHECK(obj);
   eo_do((Eo *) obj, elm_scrollable_interface_bounce_allow_get
         (h_bounce, v_bounce));
}

static void
_bounce_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *h_bounce = va_arg(*list, Eina_Bool *);
   Eina_Bool *v_bounce = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (h_bounce) *h_bounce = sd->h_bounce;
   if (v_bounce) *v_bounce = sd->v_bounce;
}

EAPI void
elm_genlist_homogeneous_set(Evas_Object *obj,
                            Eina_Bool homogeneous)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_homogeneous_set(homogeneous));
}

static void
_homogeneous_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool homogeneous = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;

   sd->homogeneous = !!homogeneous;
}

EAPI Eina_Bool
elm_genlist_homogeneous_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, elm_obj_genlist_homogeneous_get(&ret));
   return ret;
}

static void
_homogeneous_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->homogeneous;
}

EAPI void
elm_genlist_block_count_set(Evas_Object *obj,
                            int count)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_block_count_set(count));
}

static void
_block_count_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   int count = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;
   EINA_SAFETY_ON_TRUE_RETURN(count < 1);

   sd->max_items_per_block = count;
   sd->item_cache_max = sd->max_items_per_block * 2;
   _item_cache_all_free(sd);
}

EAPI int
elm_genlist_block_count_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) 0;
   int ret = 0;
   eo_do((Eo *) obj, elm_obj_genlist_block_count_get(&ret));
   return ret;
}

static void
_block_count_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   int *ret = va_arg(*list, int *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->max_items_per_block;
}

EAPI void
elm_genlist_longpress_timeout_set(Evas_Object *obj,
                                  double timeout)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_longpress_timeout_set(timeout));
}

static void
_longpress_timeout_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   double timeout = va_arg(*list, double);
   Elm_Genlist_Smart_Data *sd = _pd;

   sd->longpress_timeout = timeout;
}

EAPI double
elm_genlist_longpress_timeout_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) 0;
   double ret = 0;
   eo_do((Eo *) obj, elm_obj_genlist_longpress_timeout_get(&ret));
   return ret;
}

static void
_longpress_timeout_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   double *ret = va_arg(*list, double *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->longpress_timeout;
}

EAPI void
elm_genlist_scroller_policy_set(Evas_Object *obj,
                                Elm_Scroller_Policy policy_h,
                                Elm_Scroller_Policy policy_v)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_scrollable_interface_policy_set(policy_h, policy_v));
}

static void
_scroller_policy_set(Eo *obj, void *_pd EINA_UNUSED, va_list *list)
{
   Elm_Scroller_Policy policy_h = va_arg(*list, Elm_Scroller_Policy);
   Elm_Scroller_Policy policy_v = va_arg(*list, Elm_Scroller_Policy);

   if ((policy_h >= ELM_SCROLLER_POLICY_LAST) ||
       (policy_v >= ELM_SCROLLER_POLICY_LAST))
     return;

   eo_do_super(obj, MY_CLASS, elm_scrollable_interface_policy_set(policy_h, policy_v));
}

EAPI void
elm_genlist_scroller_policy_get(const Evas_Object *obj,
                                Elm_Scroller_Policy *policy_h,
                                Elm_Scroller_Policy *policy_v)
{
   ELM_GENLIST_CHECK(obj);
   eo_do((Eo *) obj, elm_scrollable_interface_policy_get(policy_h, policy_v));
}

static void
_scroller_policy_get(Eo *obj, void *_pd EINA_UNUSED, va_list *list)
{
   Elm_Scroller_Policy s_policy_h, s_policy_v;

   Elm_Scroller_Policy *policy_h = va_arg(*list, Elm_Scroller_Policy *);
   Elm_Scroller_Policy *policy_v = va_arg(*list, Elm_Scroller_Policy *);

   eo_do_super((Eo *)obj, MY_CLASS, elm_scrollable_interface_policy_get
         (&s_policy_h, &s_policy_v));
   if (policy_h) *policy_h = (Elm_Scroller_Policy)s_policy_h;
   if (policy_v) *policy_v = (Elm_Scroller_Policy)s_policy_v;
}

EAPI void
elm_genlist_realized_items_update(Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_realized_items_update());
}

static void
_realized_items_update(Eo *obj, void *_pd EINA_UNUSED, va_list *list_unused EINA_UNUSED)
{
   Eina_List *list;
   Elm_Object_Item *it;

   list = elm_genlist_realized_items_get(obj);
   EINA_LIST_FREE(list, it)
     elm_genlist_item_update(it);
}

EAPI void
elm_genlist_item_decorate_mode_set(Elm_Object_Item *item,
                                   const char *decorate_it_type,
                                   Eina_Bool decorate_it_set)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;
   Elm_Genlist_Smart_Data *sd;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   sd = GL_IT(it)->wsd;

   if (elm_widget_item_disabled_get(it)) return;
   if (!decorate_it_type) return;
   if (sd->decorate_all_mode) return;

   if ((sd->mode_item == it) &&
       (!strcmp(decorate_it_type, sd->decorate_it_type)) &&
       (decorate_it_set))
     return;
   if (!it->itc->decorate_item_style) return;
   it->decorate_it_set = decorate_it_set;

   _item_unselect(it);
   if (((sd->decorate_it_type)
        && (strcmp(decorate_it_type, sd->decorate_it_type))) ||
       (decorate_it_set) || ((it == sd->mode_item) && (!decorate_it_set)))
     {
        char buf[1024], buf2[1024];
        eo_do(sd->obj, elm_scrollable_interface_hold_set(EINA_TRUE));
        if (sd->scr_hold_timer) ecore_timer_del(sd->scr_hold_timer);
        sd->scr_hold_timer = ecore_timer_add(0.1, _scroll_hold_timer_cb, sd);

        snprintf(buf, sizeof(buf), "elm,state,%s,passive", sd->decorate_it_type);
        edje_object_signal_emit(GL_IT(it)->deco_it_view, buf, "elm");
        edje_object_signal_emit(VIEW(it), buf, "elm");

        snprintf(buf2, sizeof(buf2), "elm,state,%s,passive,finished",
                 sd->decorate_it_type);
        edje_object_signal_callback_add(GL_IT(it)->deco_it_view, buf2,
                                        "elm",
                                        _decorate_item_finished_signal_cb, it);
        sd->mode_item = NULL;
     }

   eina_stringshare_replace(&sd->decorate_it_type, decorate_it_type);
   if (decorate_it_set)
     {
        sd->mode_item = it;
        eo_do(sd->obj, elm_scrollable_interface_hold_set(EINA_TRUE));
        if (sd->scr_hold_timer) ecore_timer_del(sd->scr_hold_timer);
        sd->scr_hold_timer = ecore_timer_add(0.1, _scroll_hold_timer_cb, sd);

        _decorate_item_realize(it);
     }
}

EAPI const char *
elm_genlist_item_decorate_mode_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *i = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, NULL);
   return GL_IT(i)->wsd->decorate_it_type;
}

EAPI Elm_Object_Item *
elm_genlist_decorated_item_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) NULL;
   Elm_Object_Item *ret = NULL;
   eo_do((Eo *) obj, elm_obj_genlist_decorated_item_get(&ret));
   return ret;
}

static void
_decorated_item_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Object_Item **ret = va_arg(*list, Elm_Object_Item **);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = (Elm_Object_Item *)sd->mode_item;
}

EAPI Eina_Bool
elm_genlist_decorate_mode_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, elm_obj_genlist_decorate_mode_get(&ret));
   return ret;
}

static void
_decorate_mode_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->decorate_all_mode;
}

EAPI void
elm_genlist_decorate_mode_set(Evas_Object *obj,
                              Eina_Bool decorated)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_decorate_mode_set(decorated));
}

static void
_decorate_mode_set(Eo *obj, void *_pd, va_list *valist)
{
   Elm_Gen_Item *it;
   Eina_List *list;

   Eina_Bool decorated = va_arg(*valist, int);
   Elm_Genlist_Smart_Data *sd = _pd;

   decorated = !!decorated;
   if (sd->decorate_all_mode == decorated) return;
   // decorate_all_mode should be set first
   // because content_get func. will be called after this
   // and user can check whether deocrate_all_mode is enabeld.
   sd->decorate_all_mode = decorated;

   list = elm_genlist_realized_items_get(obj);
   EINA_LIST_FREE(list, it)
     {
        if (!sd->decorate_all_mode)
          {
             _decorate_all_item_unrealize(it);
          }
        else if (it->itc->decorate_all_item_style)
          {
             _decorate_all_item_realize(it, EINA_TRUE);
          }
        _access_widget_item_register(it);
     }
   evas_object_smart_changed(sd->pan_obj);
}

EAPI void
elm_genlist_reorder_mode_set(Evas_Object *obj,
                             Eina_Bool reorder_mode)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_reorder_mode_set(reorder_mode));
}

static void
_reorder_mode_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_List *realized;
   Elm_Gen_Item *it;
   Eina_Bool reorder_mode = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (sd->reorder_mode == !!reorder_mode) return;
   sd->reorder_mode = !!reorder_mode;

   realized = elm_genlist_realized_items_get(obj);
   EINA_LIST_FREE(realized, it)
     {
        if (sd->reorder_mode)
          {
             edje_object_signal_emit
                (VIEW(it), SIGNAL_REORDER_MODE_SET, "elm");
             if (it->deco_all_view)
                edje_object_signal_emit
                   (it->deco_all_view, SIGNAL_REORDER_MODE_SET, "elm");
          }
        else
          {
             edje_object_signal_emit
                (VIEW(it), SIGNAL_REORDER_MODE_UNSET, "elm");
             if (it->deco_all_view)
                edje_object_signal_emit
                   (it->deco_all_view, SIGNAL_REORDER_MODE_UNSET, "elm");
          }
     }
}

EAPI Eina_Bool
elm_genlist_reorder_mode_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, elm_obj_genlist_reorder_mode_get(&ret));
   return ret;
}

static void
_reorder_mode_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->reorder_mode;
}

EAPI Elm_Genlist_Item_Type
elm_genlist_item_type_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, ELM_GENLIST_ITEM_MAX);

   return it->item->type;
}

EAPI Elm_Genlist_Item_Class *
elm_genlist_item_class_new(void)
{
   Elm_Genlist_Item_Class *itc;

   itc = calloc(1, sizeof(Elm_Genlist_Item_Class));
   if (!itc)
     return NULL;

   itc->version = CLASS_ALLOCATED;
   itc->refcount = 1;
   itc->delete_me = EINA_FALSE;

   return itc;
}

EAPI void
elm_genlist_item_class_free(Elm_Genlist_Item_Class *itc)
{
   if (itc && (itc->version == CLASS_ALLOCATED))
     {
        if (!itc->delete_me) itc->delete_me = EINA_TRUE;
        if (itc->refcount > 0) elm_genlist_item_class_unref(itc);
        else
          {
             itc->version = 0;
             free(itc);
          }
     }
}

EAPI void
elm_genlist_item_class_ref(Elm_Genlist_Item_Class *itc)
{
   if (itc && (itc->version == CLASS_ALLOCATED))
     {
        itc->refcount++;
        if (itc->refcount == 0) itc->refcount--;
     }
}

EAPI void
elm_genlist_item_class_unref(Elm_Genlist_Item_Class *itc)
{
   if (itc && (itc->version == CLASS_ALLOCATED))
     {
        if (itc->refcount > 0) itc->refcount--;
        if (itc->delete_me && (!itc->refcount))
          elm_genlist_item_class_free(itc);
     }
}

EAPI void
elm_genlist_item_flip_set(Elm_Object_Item *item,
                          Eina_Bool flip)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   flip = !!flip;
   if (it->flipped == flip) return;
   it->flipped = flip;

   if (it->flipped)
     {
        GL_IT(it)->flip_content_objs =
           _item_content_realize(it, VIEW(it), GL_IT(it)->flip_content_objs,
                                 "flips", NULL);

        edje_object_signal_emit(VIEW(it), SIGNAL_FLIP_ENABLED, "elm");
        if (GL_IT(it)->wsd->decorate_all_mode)
           edje_object_signal_emit
              (it->deco_all_view, SIGNAL_FLIP_ENABLED, "elm");
     }
   else
     {
        Evas_Object *c;
        edje_object_signal_emit(VIEW(it), SIGNAL_FLIP_DISABLED, "elm");
        if (GL_IT(it)->wsd->decorate_all_mode)
           edje_object_signal_emit
              (it->deco_all_view, SIGNAL_FLIP_DISABLED, "elm");

        EINA_LIST_FREE(GL_IT(it)->flip_content_objs, c)
           evas_object_del(c);

        // FIXME: update texts should be done by app?
        _item_text_realize(it, VIEW(it), NULL);
     }
   _access_widget_item_register(it);
}

EAPI Eina_Bool
elm_genlist_item_flip_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, EINA_FALSE);

   return it->flipped;
}

EAPI void
elm_genlist_select_mode_set(Evas_Object *obj,
                            Elm_Object_Select_Mode mode)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_select_mode_set(mode));
}

static void
_select_mode_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Object_Select_Mode mode = va_arg(*list, Elm_Object_Select_Mode);
   Elm_Genlist_Smart_Data *sd = _pd;

   if ((mode >= ELM_OBJECT_SELECT_MODE_MAX) || (sd->select_mode == mode))
     return;

   sd->select_mode = mode;

   if ((sd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY ))
     {
        Eina_List *l, *ll;
        Elm_Gen_Item *it;
        EINA_LIST_FOREACH_SAFE(sd->selected, l, ll, it)
          _item_unselect(it);
     }
   if (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY)
     {
        Elm_Gen_Item *it;
        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (!GL_IT(it)->block) continue;
             GL_IT(it)->calc_done = EINA_FALSE;
             GL_IT(it)->block->calc_done = EINA_FALSE;
             sd->calc_done = EINA_FALSE;
             evas_object_smart_changed(sd->pan_obj);
          }
     }
}

EAPI Elm_Object_Select_Mode
elm_genlist_select_mode_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) ELM_OBJECT_SELECT_MODE_MAX;
   Elm_Object_Select_Mode ret = ELM_OBJECT_SELECT_MODE_MAX;
   eo_do((Eo *) obj, elm_obj_genlist_select_mode_get(&ret));
   return ret;
}

static void
_select_mode_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Elm_Object_Select_Mode *ret = va_arg(*list, Elm_Object_Select_Mode *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->select_mode;
}

EAPI void
elm_genlist_highlight_mode_set(Evas_Object *obj,
                               Eina_Bool highlight)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_highlight_mode_set(highlight));
}

static void
_highlight_mode_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool highlight = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;

   sd->highlight = !!highlight;
}

EAPI Eina_Bool
elm_genlist_highlight_mode_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, elm_obj_genlist_highlight_mode_get(&ret));
   return ret;
}

static void
_highlight_mode_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->highlight;
}

EAPI void
elm_genlist_item_select_mode_set(Elm_Object_Item *item,
                                 Elm_Object_Select_Mode mode)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;

   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);

   if ((mode >= ELM_OBJECT_SELECT_MODE_MAX) || (it->select_mode == mode))
     return;

   it->select_mode = mode;

   if ((it->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY ))
      _item_unselect(it);

   if (GL_IT(it)->block && it->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY)
     {
        GL_IT(it)->calc_done = EINA_FALSE;
        GL_IT(it)->block->calc_done = EINA_FALSE;
        GL_IT(it)->wsd->calc_done = EINA_FALSE;
        evas_object_smart_changed(GL_IT(it)->wsd->pan_obj);
     }
}

EAPI Elm_Object_Select_Mode
elm_genlist_item_select_mode_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item, ELM_OBJECT_SELECT_MODE_MAX);

   return it->select_mode;
}

EAPI void
elm_genlist_tree_effect_enabled_set(Evas_Object *obj,
                                    Eina_Bool enabled)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_tree_effect_enabled_set(enabled));
}

static void
_tree_effect_enabled_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool enabled = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;

   sd->tree_effect_enabled = !!enabled;
}

EAPI Eina_Bool
elm_genlist_tree_effect_enabled_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, elm_obj_genlist_tree_effect_enabled_get(&ret));
   return ret;
}

static void
_tree_effect_enabled_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;

   if (ret) *ret = sd->tree_effect_enabled;
}

EAPI void
elm_genlist_focus_on_selection_set(Evas_Object *obj,
                                    Eina_Bool enabled)
{
   ELM_GENLIST_CHECK(obj);
   eo_do(obj, elm_obj_genlist_focus_on_selection_set(enabled));
}

static void
_focus_on_selection_set(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool enabled = va_arg(*list, int);
   Elm_Genlist_Smart_Data *sd = _pd;
   sd->focus_on_selection_enabled = !!enabled;
}

EAPI Eina_Bool
elm_genlist_focus_on_selection_get(const Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, elm_obj_genlist_focus_on_selection_get(&ret));
   return ret;
}

static void
_focus_on_selection_get(Eo *obj EINA_UNUSED, void *_pd, va_list *list)
{
   Eina_Bool *ret = va_arg(*list, Eina_Bool *);
   Elm_Genlist_Smart_Data *sd = _pd;
   if (ret) *ret = sd->focus_on_selection_enabled;
}

EAPI Elm_Object_Item *
elm_genlist_nth_item_get(const Evas_Object *obj, unsigned int nth)
{
   Elm_Gen_Item *it = NULL;
   Eina_Accessor *a;
   void *data;

   ELM_GENLIST_CHECK(obj) NULL;
   ELM_GENLIST_DATA_GET(obj, sd);

   if (!sd->items) return NULL;

   a = eina_inlist_accessor_new(sd->items);
   if (!a) return NULL;
   if (eina_accessor_data_get(a, nth, &data))
     it = ELM_GEN_ITEM_FROM_INLIST(data);
   eina_accessor_free(a);
   return (Elm_Object_Item *)it;
}

EAPI void
elm_genlist_item_hide_set(const Elm_Object_Item *item, Eina_Bool hide)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(item);
   Elm_Genlist_Smart_Data *sd = GL_IT(it)->wsd;

   if (it->item->hide == !!hide) return;
   it->item->hide = !!hide;

   if (GL_IT(it)->block)
     GL_IT(it)->block->calc_done = EINA_FALSE;
   sd->calc_done = EINA_FALSE;

   evas_object_smart_changed(sd->pan_obj);
}

EAPI Eina_Bool
elm_genlist_item_hide_get(const Elm_Object_Item *item)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)item;
   ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, EINA_FALSE)

   return it->item->hide;
}

EAPI void
elm_genlist_realization_mode_set(Evas_Object *obj, Eina_Bool realization_mode)
{
   ELM_GENLIST_CHECK(obj);
   ELM_GENLIST_DATA_GET(obj, sd);

   if (sd->realization_mode == realization_mode) return;
   sd->realization_mode = realization_mode;
}

EAPI Eina_Bool
elm_genlist_realization_mode_get(Evas_Object *obj)
{
   ELM_GENLIST_CHECK(obj) EINA_FALSE;
   ELM_GENLIST_DATA_GET(obj, sd);

   return sd->realization_mode;
}

static void
_class_constructor(Eo_Class *klass)
{
   const Eo_Op_Func_Description func_desc[] = {
        EO_OP_FUNC(EO_BASE_ID(EO_BASE_SUB_ID_CONSTRUCTOR), _constructor),

        EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_MEMBER_ADD), _elm_genlist_smart_member_add),
        EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_ADD), _elm_genlist_smart_add),
        EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_DEL), _elm_genlist_smart_del),
        EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_RESIZE), _elm_genlist_smart_resize),
        EO_OP_FUNC(EVAS_OBJ_SMART_ID(EVAS_OBJ_SMART_SUB_ID_MOVE), _elm_genlist_smart_move),

        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_THEME), _elm_genlist_smart_theme),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_TRANSLATE), _elm_genlist_smart_translate),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_EVENT), _elm_genlist_smart_event),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_ON_FOCUS), _elm_genlist_smart_on_focus),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_SUB_OBJECT_ADD), _elm_genlist_smart_sub_object_add),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_SUB_OBJECT_DEL), _elm_genlist_smart_sub_object_del),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_FOCUS_NEXT_MANAGER_IS), _elm_genlist_smart_focus_next_manager_is),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_FOCUS_DIRECTION_MANAGER_IS), _elm_genlist_smart_focus_direction_manager_is),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_FOCUS_NEXT), _elm_genlist_smart_focus_next),
        EO_OP_FUNC(ELM_WIDGET_ID(ELM_WIDGET_SUB_ID_ACCESS), _elm_genlist_smart_access),

        EO_OP_FUNC(ELM_OBJ_LAYOUT_ID(ELM_OBJ_LAYOUT_SUB_ID_SIZING_EVAL), _elm_genlist_smart_sizing_eval),
        EO_OP_FUNC(ELM_OBJ_LAYOUT_ID(ELM_OBJ_LAYOUT_SUB_ID_SUB_OBJECT_ADD_ENABLE), _elm_genlist_smart_layout_sub_object_add_enable),

        EO_OP_FUNC(ELM_SCROLLABLE_INTERFACE_ID(ELM_SCROLLABLE_INTERFACE_SUB_ID_BOUNCE_ALLOW_SET), _bounce_set),
        EO_OP_FUNC(ELM_SCROLLABLE_INTERFACE_ID(ELM_SCROLLABLE_INTERFACE_SUB_ID_BOUNCE_ALLOW_GET), _bounce_get),
        EO_OP_FUNC(ELM_SCROLLABLE_INTERFACE_ID(ELM_SCROLLABLE_INTERFACE_SUB_ID_POLICY_SET), _scroller_policy_set),
        EO_OP_FUNC(ELM_SCROLLABLE_INTERFACE_ID(ELM_SCROLLABLE_INTERFACE_SUB_ID_POLICY_GET), _scroller_policy_get),

        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_ITEMS_COUNT), _items_count),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_ITEM_APPEND), _item_append),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_ITEM_PREPEND), _item_prepend),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_ITEM_INSERT_AFTER), _item_insert_after),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_ITEM_INSERT_BEFORE), _item_insert_before),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_ITEM_SORTED_INSERT), _item_sorted_insert),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_CLEAR), _clear_eo),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_SET), _multi_select_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_GET), _multi_select_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_MODE_SET), _multi_select_mode_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_MODE_GET), _multi_select_mode_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_SELECTED_ITEM_GET), _selected_item_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_SELECTED_ITEMS_GET), _selected_items_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_REALIZED_ITEMS_GET), _realized_items_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_AT_XY_ITEM_GET), _at_xy_item_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_FIRST_ITEM_GET), _first_item_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_LAST_ITEM_GET), _last_item_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_MODE_SET), _mode_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_MODE_GET), _mode_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_HOMOGENEOUS_SET), _homogeneous_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_HOMOGENEOUS_GET), _homogeneous_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_BLOCK_COUNT_SET), _block_count_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_BLOCK_COUNT_GET), _block_count_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_LONGPRESS_TIMEOUT_SET), _longpress_timeout_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_LONGPRESS_TIMEOUT_GET), _longpress_timeout_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_REALIZED_ITEMS_UPDATE), _realized_items_update),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_DECORATED_ITEM_GET), _decorated_item_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_DECORATE_MODE_GET), _decorate_mode_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_DECORATE_MODE_SET), _decorate_mode_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_REORDER_MODE_SET), _reorder_mode_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_REORDER_MODE_GET), _reorder_mode_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_SELECT_MODE_SET), _select_mode_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_SELECT_MODE_GET), _select_mode_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_HIGHLIGHT_MODE_SET), _highlight_mode_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_HIGHLIGHT_MODE_GET), _highlight_mode_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_TREE_EFFECT_ENABLED_SET), _tree_effect_enabled_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_TREE_EFFECT_ENABLED_GET), _tree_effect_enabled_get),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_FOCUS_ON_SELECTION_SET), _focus_on_selection_set),
        EO_OP_FUNC(ELM_OBJ_GENLIST_ID(ELM_OBJ_GENLIST_SUB_ID_FOCUS_ON_SELECTION_GET), _focus_on_selection_get),
        EO_OP_FUNC_SENTINEL
   };
   eo_class_funcs_set(klass, func_desc);

   if (_elm_config->access_mode)
      _elm_genlist_smart_focus_next_enable = EINA_TRUE;

   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

static const Eo_Op_Description op_desc[] = {
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_ITEMS_COUNT, "Return how many items are currently in a list."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_ITEM_APPEND, "Append a new item in a given genlist widget."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_ITEM_PREPEND, "Prepend a new item in a given genlist widget."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_ITEM_INSERT_AFTER, "Insert an item after another in a genlist widget."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_ITEM_INSERT_BEFORE, "Insert an item before another in a genlist widget."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_ITEM_SORTED_INSERT, "Insert a new item into the sorted genlist object."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_CLEAR, "Remove all items from a given genlist widget."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_SET, "Enable or disable multi-selection in the genlist."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_GET, "Get if multi-selection in genlist is enabled or disabled."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_MODE_SET, "Set the genlist multi select mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_MULTI_SELECT_MODE_GET, "Get the genlist multi select mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_SELECTED_ITEM_GET, "Get the selected item in the genlist."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_SELECTED_ITEMS_GET, "Get a list of selected items in the genlist."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_REALIZED_ITEMS_GET, "Get a list of realized items in genlist."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_AT_XY_ITEM_GET, "Get the item that is at the x, y canvas coords."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_FIRST_ITEM_GET, "Get the first item in the genlist."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_LAST_ITEM_GET, "Get the last item in the genlist."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_MODE_SET, "This sets the horizontal stretching mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_MODE_GET, "Get the horizontal stretching mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_HOMOGENEOUS_SET, "Enable/disable homogeneous mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_HOMOGENEOUS_GET, "Get whether the homogeneous mode is enabled."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_BLOCK_COUNT_SET, "Set the maximum number of items within an item block."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_BLOCK_COUNT_GET, "Get the maximum number of items within an item block."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_LONGPRESS_TIMEOUT_SET, "Set the timeout in seconds for the longpress event."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_LONGPRESS_TIMEOUT_GET, "Get the timeout in seconds for the longpress event."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_REALIZED_ITEMS_UPDATE, "Update the contents of all realized items."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_DECORATED_ITEM_GET, "Get active genlist mode item."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_DECORATE_MODE_GET, "Get Genlist decorate mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_DECORATE_MODE_SET, "Set Genlist decorate mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_REORDER_MODE_SET, "Set reorder mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_REORDER_MODE_GET, "Get the reorder mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_SELECT_MODE_SET, "Set the genlist select mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_SELECT_MODE_GET, "Get the genlist select mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_HIGHLIGHT_MODE_SET, "Set whether the genlist items' should be highlighted when item selected."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_HIGHLIGHT_MODE_GET, "Get whether the genlist items' should be highlighted when item selected."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_TREE_EFFECT_ENABLED_SET, "Set Genlist tree effect."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_TREE_EFFECT_ENABLED_GET, "Get Genlist tree effect."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_FOCUS_ON_SELECTION_SET, "Set focus upon item's selection mode."),
     EO_OP_DESCRIPTION(ELM_OBJ_GENLIST_SUB_ID_FOCUS_ON_SELECTION_GET, "Get focus upon item's selection mode."),
     EO_OP_DESCRIPTION_SENTINEL
};

static const Eo_Class_Description class_desc = {
     EO_VERSION,
     MY_CLASS_NAME,
     EO_CLASS_TYPE_REGULAR,
     EO_CLASS_DESCRIPTION_OPS(&ELM_OBJ_GENLIST_BASE_ID, op_desc, ELM_OBJ_GENLIST_SUB_ID_LAST),
     NULL,
     sizeof(Elm_Genlist_Smart_Data),
     _class_constructor,
     NULL
};

EO_DEFINE_CLASS(elm_obj_genlist_class_get, &class_desc, ELM_OBJ_LAYOUT_CLASS, ELM_SCROLLABLE_INTERFACE, EVAS_SMART_CLICKABLE_INTERFACE, NULL);
