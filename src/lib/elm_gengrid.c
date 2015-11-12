#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_SELECTION_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED
#define ELM_WIDGET_ITEM_PROTECTED

#include <Elementary.h>
#include <Elementary_Cursor.h>
#include "elm_priv.h"
#include "elm_widget_gengrid.h"
#include "elm_interface_scrollable.h"

#define MY_PAN_CLASS ELM_GENGRID_PAN_CLASS

#define MY_PAN_CLASS_NAME "Elm_Gengrid_Pan"
#define MY_PAN_CLASS_NAME_LEGACY "elm_gengrid_pan"

#define MY_CLASS ELM_GENGRID_CLASS

#define MY_CLASS_NAME "Elm_Gengrid"
#define MY_CLASS_NAME_LEGACY "elm_gengrid"

// internally allocated
#define CLASS_ALLOCATED     0x3a70f00f

/* --
 * TODO:
 * Handle non-homogeneous objects too.
 */

#define PRELOAD             1
#define REORDER_EFFECT_TIME 0.5

#define CACHE_MAX 40

EAPI const char ELM_GENGRID_SMART_NAME[] = "elm_gengrid";
EAPI const char ELM_GENGRID_PAN_SMART_NAME[] = "elm_gengrid_pan";

#define ELM_PRIV_GENGRID_SIGNALS(cmd) \
   cmd(SIG_ACTIVATED, "activated", "") \
   cmd(SIG_CLICKED_DOUBLE, "clicked,double", "") \
   cmd(SIG_CLICKED_RIGHT, "clicked,right", "") \
   cmd(SIG_LONGPRESSED, "longpressed", "") \
   cmd(SIG_SELECTED, "selected", "") \
   cmd(SIG_UNSELECTED, "unselected", "") \
   cmd(SIG_REALIZED, "realized", "") \
   cmd(SIG_UNREALIZED, "unrealized", "") \
   cmd(SIG_CHANGED, "changed", "") \
   cmd(SIG_DRAG_START_UP, "drag,start,up", "") \
   cmd(SIG_DRAG_START_DOWN, "drag,start,down", "") \
   cmd(SIG_DRAG_START_LEFT, "drag,start,left", "") \
   cmd(SIG_DRAG_START_RIGHT, "drag,start,right", "") \
   cmd(SIG_DRAG_STOP, "drag,stop", "") \
   cmd(SIG_DRAG, "drag", "") \
   cmd(SIG_SCROLL, "scroll", "") \
   cmd(SIG_SCROLL_ANIM_START, "scroll,anim,start", "") \
   cmd(SIG_SCROLL_ANIM_STOP, "scroll,anim,stop", "") \
   cmd(SIG_SCROLL_DRAG_START, "scroll,drag,start", "") \
   cmd(SIG_SCROLL_DRAG_STOP, "scroll,drag,stop", "") \
   cmd(SIG_SCROLL_PAGE_CHANGE, "scroll,page,changed", "") \
   cmd(SIG_EDGE_TOP, "edge,top", "") \
   cmd(SIG_EDGE_BOTTOM, "edge,bottom", "") \
   cmd(SIG_EDGE_LEFT, "edge,left", "") \
   cmd(SIG_EDGE_RIGHT, "edge,right", "") \
   cmd(SIG_MOVED, "moved", "") \
   cmd(SIG_INDEX_UPDATE, "index,update", "") \
   cmd(SIG_HIGHLIGHTED, "highlighted", "") \
   cmd(SIG_UNHIGHLIGHTED, "unhighlighted", "") \
   cmd(SIG_ITEM_FOCUSED, "item,focused", "") \
   cmd(SIG_ITEM_UNFOCUSED, "item,unfocused", "") \
   cmd(SIG_PRESSED, "pressed", "") \
   cmd(SIG_RELEASED, "released", "") \
   cmd(SIG_ITEM_REORDER_START, "item,reorder,anim,start", "") \
   cmd(SIG_ITEM_REORDER_STOP, "item,reorder,anim,stop", "")

ELM_PRIV_GENGRID_SIGNALS(ELM_PRIV_STATIC_VARIABLE_DECLARE);

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   ELM_PRIV_GENGRID_SIGNALS(ELM_PRIV_SMART_CALLBACKS_DESC)
   {SIG_WIDGET_LANG_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_ACCESS_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_LAYOUT_FOCUSED, ""}, /**< handled by elm_layout */
   {SIG_LAYOUT_UNFOCUSED, ""}, /**< handled by elm_layout */
   {SIG_ITEM_FOCUSED, ""},
   {SIG_ITEM_UNFOCUSED, ""},

   {NULL, NULL}
};
#undef ELM_PRIV_GENGRID_SIGNALS

static Eina_Bool _key_action_move(Evas_Object *obj, const char *params);
static Eina_Bool _key_action_select(Evas_Object *obj, const char *params);
static Eina_Bool _key_action_escape(Evas_Object *obj, const char *params);
static void _item_position_update(Eina_Inlist *list, int idx);
static void _item_mouse_callbacks_add(Elm_Gen_Item *it, Evas_Object *view);
static void _item_mouse_callbacks_del(Elm_Gen_Item *it, Evas_Object *view);


static const Elm_Action key_actions[] = {
   {"move", _key_action_move},
   {"select", _key_action_select},
   {"escape", _key_action_escape},
   {NULL, NULL}
};


//-- item cache handle routine --//
// push item cache into caches
static Eina_Bool
_item_cache_push(Elm_Gengrid_Data *sd, Item_Cache *itc)
{
   if (!itc || (sd->item_cache_max <= 0))
     return EINA_FALSE;

   sd->item_cache_count++;
   sd->item_cache =
     eina_inlist_prepend(sd->item_cache, EINA_INLIST_GET(itc));

   return EINA_TRUE;
}

// pop item cache from caches
static Item_Cache *
_item_cache_pop(Elm_Gengrid_Data *sd, Item_Cache *itc)
{
   if ((!itc) || (!sd->item_cache) ||
       (sd->item_cache_count <= 0))
     return NULL;

   sd->item_cache =
     eina_inlist_remove (sd->item_cache, EINA_INLIST_GET(itc));
   sd->item_cache_count--;

   return itc;
}

// free one item cache from caches
static void
_item_cache_free(Item_Cache *itc)
{
   if (!itc) return;

   evas_object_del(itc->spacer);
   evas_object_del(itc->base_view);
   eina_stringshare_del(itc->item_style);
   ELM_SAFE_FREE(itc ,free);
}

// clean up item cache by removing overflowed caches
static void
_item_cache_clean(Elm_Gengrid_Data *sd)
{
   evas_event_freeze(evas_object_evas_get(sd->obj));

   while ((sd->item_cache) && (sd->item_cache_count > sd->item_cache_max))
     {
        Item_Cache *itc =
           EINA_INLIST_CONTAINER_GET(sd->item_cache->last, Item_Cache);
        _item_cache_free(_item_cache_pop(sd, itc));
     }
   evas_event_thaw(evas_object_evas_get(sd->obj));
   evas_event_thaw_eval(evas_object_evas_get(sd->obj));
}

// empty all item caches
static void
_item_cache_zero(Elm_Gengrid_Data *sd)
{
   int pmax = sd->item_cache_max;

   sd->item_cache_max = 0;
   _item_cache_clean(sd);
   sd->item_cache_max = pmax;
}

// add an item to item cache
static Eina_Bool
_item_cache_add(Elm_Gen_Item *it)
{
   if (it->item->nocache_once || it->item->nocache) return EINA_FALSE;

   Item_Cache *itc = NULL;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   Evas_Object *obj = sd->obj;

   evas_event_freeze(evas_object_evas_get(obj));
   if (sd->item_cache_max > 0)
     itc = ELM_NEW(Item_Cache);
   if (!_item_cache_push(sd, itc))
     {
        if (itc) ELM_SAFE_FREE(itc, free);

        evas_event_thaw(evas_object_evas_get(obj));
        evas_event_thaw_eval(evas_object_evas_get(obj));
        return EINA_FALSE;
     }

   itc->spacer = it->spacer;
   itc->base_view = VIEW(it);
   itc->item_style = eina_stringshare_add(it->itc->item_style);

   if (!it->group)
     {
        Eina_Bool tmp;
        if (it->selected)
          edje_object_signal_emit(itc->base_view, "elm,state,unselected", "elm");
        if (eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()))
          edje_object_signal_emit(itc->base_view, "elm,state,enabled", "elm");
        if (it == (Elm_Gen_Item *)sd->focused_item &&
            (elm_widget_focus_highlight_enabled_get(obj) || _elm_config->win_auto_focus_enable))
          edje_object_signal_emit(itc->base_view, "elm,state,unfocused", "elm");

        ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
        // FIXME: other callbacks?
        _item_mouse_callbacks_del(it, itc->base_view);
     }

   edje_object_mirrored_set(VIEW(it),
                            elm_widget_mirrored_get(WIDGET(it)));
   edje_object_scale_set(VIEW(it),
                         elm_widget_scale_get(WIDGET(it))
                         * elm_config_scale_get());

   evas_object_hide(itc->base_view);
   evas_object_move(itc->base_view, -9999, -9999);
   it->spacer = NULL;
   VIEW(it) = NULL;

   _item_cache_clean(sd);

   evas_event_thaw(evas_object_evas_get(obj));
   evas_event_thaw_eval(evas_object_evas_get(obj));

   return EINA_TRUE;
}

// find an item from item cache and remove it from the cache
static Eina_Bool
_item_cache_find(Elm_Gen_Item *it)
{
   if (it->item->nocache_once || it->item->nocache) return EINA_FALSE;

   Item_Cache *itc = NULL;
   Eina_Inlist *l;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   EINA_INLIST_FOREACH_SAFE(sd->item_cache, l, itc)
     {
        if (((!it->itc->item_style) && (!itc->item_style)) ||
            (it->itc->item_style && itc->item_style &&
             (!strcmp(it->itc->item_style, itc->item_style))))
          {
             itc = _item_cache_pop(sd, itc);
             if (!itc) continue;

             it->spacer = itc->spacer;
             VIEW(it) = itc->base_view;
             itc->spacer = NULL;
             itc->base_view = NULL;

             _item_cache_free(itc);
             return EINA_TRUE;
          }
     }
   return EINA_FALSE;
}

static Eina_Bool
_is_no_select(Elm_Gen_Item *it)
{
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   if ((sd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (it->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY))
     return EINA_TRUE;
   return EINA_FALSE;
}

EOLIAN static Elm_Object_Item *
_elm_gengrid_search_by_text_item_get(Eo *obj EINA_UNUSED,
                                     Elm_Gengrid_Data *sd,
                                     Elm_Object_Item *eo_item_to_search_from,
                                     const char *part_name,
                                     const char *pattern,
                                     Elm_Glob_Match_Flags flags)
{
   Elm_Gen_Item *it = NULL;
   char *str = NULL;
   Eina_Inlist *start = NULL;
   int fnflags = 0;

   if (!pattern) return NULL;
   if (!sd->items) return NULL;

   if (flags & ELM_GLOB_MATCH_NO_ESCAPE) fnflags |= FNM_NOESCAPE;
   if (flags & ELM_GLOB_MATCH_PATH) fnflags |= FNM_PATHNAME;
   if (flags & ELM_GLOB_MATCH_PERIOD) fnflags |= FNM_PERIOD;
#ifdef FNM_CASEFOLD
   if (flags & ELM_GLOB_MATCH_NOCASE) fnflags |= FNM_CASEFOLD;
#endif

   ELM_GENGRID_ITEM_DATA_GET(eo_item_to_search_from, item_to_search_from);
   start = (item_to_search_from) ?
     EINA_INLIST_GET(item_to_search_from) :
     sd->items;
   EINA_INLIST_FOREACH(start, it)
     {
        if (!it->itc->func.text_get) continue;
        str = it->itc->func.text_get((void *)WIDGET_ITEM_DATA_GET(EO_OBJ(it)),
                                     WIDGET(it), part_name);
        if (!str) continue;
        if (!fnmatch(pattern, str, fnflags))
          {
             free(str);
             return EO_OBJ(it);
          }
        free(str);
     }
   return NULL;
}

static void
_item_show_region(void *data)
{
   Elm_Gengrid_Data *sd = data;
   Evas_Coord cvw, cvh, it_xpos = 0, it_ypos = 0, col = 0, row = 0, minx = 0, miny = 0;
   Evas_Coord vw = 0, vh = 0;
   Elm_Object_Item *eo_it = NULL;
   evas_object_geometry_get(sd->pan_obj, NULL, NULL, &cvw, &cvh);

   if ((cvw != 0) && (cvh != 0))
       {
          int x = 0, y = 0;
          if (sd->show_region)
            eo_it = sd->show_it;
          else if (sd->bring_in)
            eo_it = sd->bring_in_it;

          if (!eo_it) return;
          ELM_GENGRID_ITEM_DATA_GET(eo_it, it);

          eo_do(sd->pan_obj, elm_obj_pan_pos_min_get(&minx, &miny));
          if (sd->horizontal && (sd->item_height > 0))
            {
               row = cvh / sd->item_height;
               if (row <= 0) row = 1;
               x = (it->position - 1) / row;
               if (elm_widget_mirrored_get(sd->obj))
                 {
                    col = sd->item_count / row;
                    if (sd->item_count % row == 0)
                      col--;
                    x = col - x;
                 }

               y = (it->position - 1) % row;
               if (x >= 1)
                 it_xpos = ((x - GG_IT(it)->prev_group) * sd->item_width)
                    + (GG_IT(it)->prev_group * sd->group_item_width)
                    + minx;
               else it_xpos = minx;
               miny = miny + ((cvh - (sd->item_height * row))
                    * sd->align_y);
               it_ypos = y * sd->item_height + miny;
               it->x = x;
               it->y = y;
            }
          else if (sd->item_width > 0)
            {
               col = cvw / sd->item_width;
               if (col <= 0) col = 1;
               y = (it->position - 1) / col;
               x = (it->position - 1) % col;
               it_xpos = x * sd->item_width + minx;
               if (y >= 1)
                 it_ypos = ((y - GG_IT(it)->prev_group) * sd->item_height)
                    + (GG_IT(it)->prev_group * sd->group_item_height)
                    + miny;
               else it_ypos = miny;
               minx = minx + ((cvw - (sd->item_width * col))
                    * sd->align_x);
               it->x = x;
               it->y = y;
            }

          switch (sd->scroll_to_type)
            {
               case ELM_GENGRID_ITEM_SCROLLTO_TOP:
                  eo_do(WIDGET(it), elm_interface_scrollable_content_viewport_geometry_get
                        (NULL, NULL, &vw, &vh));
                  break;
               case ELM_GENGRID_ITEM_SCROLLTO_MIDDLE:
                  eo_do(WIDGET(it), elm_interface_scrollable_content_viewport_geometry_get
                        (NULL, NULL, &vw, &vh));
                  it_xpos = it_xpos - ((vw - sd->item_width) / 2);
                  it_ypos = it_ypos - ((vh - sd->item_height) / 2);
                  break;
               default:
                  vw = sd->item_width;
                  vh = sd->item_height;
                  break;
            }

          if (sd->show_region)
            {
               eo_do(WIDGET(it), elm_interface_scrollable_content_region_show(
                                                     it_xpos, it_ypos, vw, vh));
               sd->show_region = EINA_FALSE;
            }
          if (sd->bring_in)
            {
               eo_do(WIDGET(it), elm_interface_scrollable_region_bring_in(
                                                 it_xpos, it_ypos, vw, vh));
               sd->bring_in = EINA_FALSE;
            }
       }
}

static void
_calc_job(void *data)
{
   ELM_GENGRID_DATA_GET(data, sd);
   Evas_Coord minw = 0, minh = 0, nmax = 0, cvw, cvh;
   Elm_Gen_Item *it, *group_item = NULL;
   int count_group = 0;
   long count = 0;

   sd->items_lost = 0;

   evas_object_geometry_get(sd->pan_obj, NULL, NULL, &cvw, &cvh);

   if ((cvw != 0) || (cvh != 0))
     {
        if ((sd->horizontal) && (sd->item_height > 0))
          nmax = cvh / sd->item_height;
        else if (sd->item_width > 0)
          nmax = cvw / sd->item_width;

        if (nmax < 1)
          nmax = 1;

        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (GG_IT(it)->prev_group != count_group)
               GG_IT(it)->prev_group = count_group;
             if (it->group)
               {
                  count = count % nmax;
                  if (count)
                    sd->items_lost += nmax - count;
                  count_group++;
                  if (count) count = 0;
                  group_item = it;
               }
             else
               {
                  if (it->parent != group_item)
                    it->parent = group_item;
                  count++;
               }
          }
        count = sd->item_count + sd->items_lost - count_group;
        if (sd->horizontal)
          {
             minw = (ceil(count / (float)nmax) * sd->item_width) +
               (count_group * sd->group_item_width);
             minh = nmax * sd->item_height;
          }
        else
          {
             minw = nmax * sd->item_width;
             minh = (ceil(count / (float)nmax) * sd->item_height) +
               (count_group * sd->group_item_height);
          }

        if ((minw != sd->minw) || (minh != sd->minh))
          {
             sd->minh = minh;
             sd->minw = minw;
             eo_do(sd->pan_obj, eo_event_callback_call
                   (ELM_PAN_EVENT_CHANGED, NULL));
          }

        sd->nmax = nmax;
        evas_object_smart_changed(sd->pan_obj);

        if (sd->show_region || sd->bring_in)
          _item_show_region(sd);
     }
   sd->calc_job = NULL;
}

EOLIAN static void
_elm_gengrid_pan_eo_base_destructor(Eo *obj, Elm_Gengrid_Pan_Data *psd)
{
   eo_data_unref(psd->wobj, psd->wsd);
   eo_do_super(obj, MY_PAN_CLASS, eo_destructor());
}

EOLIAN static void
_elm_gengrid_pan_evas_object_smart_move(Eo *obj EINA_UNUSED, Elm_Gengrid_Pan_Data *psd, Evas_Coord _gen_param2 EINA_UNUSED, Evas_Coord _gen_param3 EINA_UNUSED)
{
   ecore_job_del(psd->wsd->calc_job);
   psd->wsd->calc_job = ecore_job_add(_calc_job, psd->wobj);
}

EOLIAN static void
_elm_gengrid_pan_evas_object_smart_resize(Eo *obj, Elm_Gengrid_Pan_Data *psd, Evas_Coord w, Evas_Coord h)
{
   Evas_Coord ow, oh;

   evas_object_geometry_get(obj, NULL, NULL, &ow, &oh);
   if ((ow == w) && (oh == h)) return;
   ecore_job_del(psd->wsd->calc_job);
   psd->wsd->calc_job = ecore_job_add(_calc_job, psd->wobj);
}

static void
_item_unselect(Elm_Gen_Item *it)
{
   Elm_Gen_Item_Type *item = GG_IT(it);
   Elm_Gengrid_Data *sd = item->wsd;
   Elm_Object_Item *eo_it = EO_OBJ(it);

   if ((it->generation < sd->generation) || (!it->selected))
     return;

   if (it->selected)
     {
        it->selected = EINA_FALSE;
        sd->selected = eina_list_remove(sd->selected, eo_it);
        eo_do(WIDGET(it), eo_event_callback_call
          (EVAS_SELECTABLE_INTERFACE_EVENT_UNSELECTED, eo_it));
     }
}

static void
_item_mouse_in_cb(void *data,
                  Evas *evas EINA_UNUSED,
                  Evas_Object *obj EINA_UNUSED,
                  void *event_info EINA_UNUSED)
{
   Elm_Gen_Item *it = data;
   if (!elm_object_item_disabled_get(EO_OBJ(it)) &&
       (_elm_config->focus_move_policy == ELM_FOCUS_MOVE_POLICY_IN))
     elm_object_item_focus_set(EO_OBJ(it), EINA_TRUE);
}

static void
_item_mouse_move_cb(void *data,
                    Evas *evas EINA_UNUSED,
                    Evas_Object *obj,
                    void *event_info)
{
   Elm_Gen_Item *it = data;
   Evas_Event_Mouse_Move *ev = event_info;
   Evas_Coord ox, oy, ow, oh, it_scrl_x, it_scrl_y;
   Evas_Coord minw = 0, minh = 0, x, y, w, h, dx, dy, adx, ady;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   Elm_Object_Item *eo_it = EO_OBJ(it);

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     {
        if (!sd->on_hold)
          {
             sd->on_hold = EINA_TRUE;
             if (!sd->was_selected)
               {
                  it->unhighlight_cb(it);
                  it->unsel_cb(it);
               }
          }
     }
  else if (ELM_RECTS_POINT_OUT(x, y, w, h, ev->cur.canvas.x, ev->cur.canvas.y) &&
           !sd->reorder_it )
    {
       ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
       if (!sd->was_selected)
         {
            it->unhighlight_cb(it);
            it->unsel_cb(it);
         }
        it->base->still_in = EINA_FALSE;
    }

   if ((it->dragging) && (it->down))
     {
        ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
        eo_do(WIDGET(it), eo_event_callback_call(EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG, eo_it));
        return;
     }

   if ((!it->down) || (sd->longpressed))
     {
        ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
        if ((sd->reorder_mode) && (sd->reorder_it))
          {
             evas_object_geometry_get
               (sd->pan_obj, &ox, &oy, &ow, &oh);

             it_scrl_x = ev->cur.canvas.x - sd->reorder_it->dx;
             it_scrl_y = ev->cur.canvas.y - sd->reorder_it->dy;

             if (it_scrl_x < ox) sd->reorder_item_x = ox;
             else if (it_scrl_x + sd->item_width > ox + ow)
               sd->reorder_item_x = ox + ow - sd->item_width;
             else sd->reorder_item_x = it_scrl_x;

             if (it_scrl_y < oy) sd->reorder_item_y = oy;
             else if (it_scrl_y + sd->item_height > oy + oh)
               sd->reorder_item_y = oy + oh - sd->item_height;
             else sd->reorder_item_y = it_scrl_y;

             ecore_job_del(sd->calc_job);
             sd->calc_job = ecore_job_add(_calc_job, sd->obj);
          }
        return;
     }

   if (it->select_mode != ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY)
     elm_coords_finger_size_adjust(1, &minw, 1, &minh);

   x = ev->cur.canvas.x - x;
   y = ev->cur.canvas.y - y;
   dx = x - it->dx;
   adx = dx;

   if (adx < 0) adx = -dx;
   dy = y - it->dy;
   ady = dy;

   if (ady < 0) ady = -dy;
   minw /= 2;
   minh /= 2;

   if ((adx > minw) || (ady > minh))
     {
        const Eo_Event_Description *left_drag, *right_drag;

        if (!elm_widget_mirrored_get(WIDGET(it)))
          {
             left_drag = EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG_START_LEFT;
             right_drag = EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG_START_RIGHT;
          }
        else
          {
             left_drag = EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG_START_RIGHT;
             right_drag = EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG_START_LEFT;
          }

        it->dragging = 1;
        ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
        if (!sd->was_selected)
          {
             it->unhighlight_cb(it);
             it->unsel_cb(it);
          }

        if (dy < 0)
          {
             if (ady > adx)
               eo_do(WIDGET(it), eo_event_callback_call
                     (EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG_START_UP, eo_it));
             else
               {
                  if (dx < 0)
                    eo_do(WIDGET(it), eo_event_callback_call(left_drag, eo_it));
               }
          }
        else
          {
             if (ady > adx)
               eo_do(WIDGET(it), eo_event_callback_call
                 (EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG_START_DOWN, eo_it));
             else
               {
                  if (dx < 0)
                    eo_do(WIDGET(it), eo_event_callback_call(left_drag, eo_it));
                  else
                    eo_do(WIDGET(it), eo_event_callback_call(right_drag, eo_it));
               }
          }
     }
}

static Eina_Bool
_long_press_cb(void *data)
{
   Eina_Bool tmp;
   Elm_Gen_Item *it = data;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   it->long_timer = NULL;
   if (eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()) || (it->dragging))
     return ECORE_CALLBACK_CANCEL;
   sd->longpressed = EINA_TRUE;
   eo_do(WIDGET(it), eo_event_callback_call
     (EVAS_CLICKABLE_INTERFACE_EVENT_LONGPRESSED, EO_OBJ(it)));

   if (sd->reorder_mode)
     {
        sd->reorder_it = it;
        evas_object_raise(VIEW(it));
        eo_do(WIDGET(it), elm_interface_scrollable_hold_set(EINA_TRUE));
        eo_do(WIDGET(it), elm_interface_scrollable_bounce_allow_get(
          &(sd->old_h_bounce),
          &(sd->old_v_bounce)));

        eo_do(WIDGET(it), elm_interface_scrollable_bounce_allow_set(EINA_FALSE, EINA_FALSE));
        edje_object_signal_emit(VIEW(it), "elm,state,reorder,enabled", "elm");
     }

   return ECORE_CALLBACK_CANCEL;
}

static void
_item_highlight(Elm_Gen_Item *it)
{
   const char *selectraise = NULL;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   if (_is_no_select(it) ||
       (!sd->highlight) || (it->highlighted) ||
       (it->generation < sd->generation))
     return;

   edje_object_signal_emit(VIEW(it), "elm,state,selected", "elm");
   eo_do(WIDGET(it), eo_event_callback_call
     (ELM_GENGRID_EVENT_HIGHLIGHTED, EO_OBJ(it)));

   selectraise = edje_object_data_get(VIEW(it), "selectraise");
   if ((selectraise) && (!strcmp(selectraise, "on")))
     evas_object_stack_above(VIEW(it), sd->stack);

   it->highlighted = EINA_TRUE;
}

static void
_item_unhighlight(Elm_Gen_Item *it)
{
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   Elm_Object_Item *eo_it = EO_OBJ(it);

   if (!it->highlighted ||
       (it->generation < sd->generation))
     return;

   edje_object_signal_emit(VIEW(it), "elm,state,unselected", "elm");
   eo_do(WIDGET(it), eo_event_callback_call(
            ELM_GENGRID_EVENT_UNHIGHLIGHTED, eo_it));

   evas_object_stack_below(VIEW(it), sd->stack);

   it->highlighted = EINA_FALSE;
}

static void
_item_mouse_down_cb(void *data,
                    Evas *evas EINA_UNUSED,
                    Evas_Object *obj,
                    void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   Elm_Gen_Item *it = data;
   Evas_Coord x, y;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   if (ev->button == 3)
     {
        evas_object_geometry_get(obj, &x, &y, NULL, NULL);
        it->dx = ev->canvas.x - x;
        it->dy = ev->canvas.y - y;
        return;
     }

   if (ev->button != 1) return;

   it->down = 1;
   sd->mouse_down = EINA_TRUE;
   it->dragging = 0;
   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   it->dx = ev->canvas.x - x;
   it->dy = ev->canvas.y - y;
   sd->longpressed = EINA_FALSE;
   it->base->still_in = EINA_TRUE;

   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     sd->on_hold = EINA_TRUE;
   else sd->on_hold = EINA_FALSE;

   if (sd->on_hold) return;

   sd->was_selected = it->selected;
   it->highlight_cb(it);
   if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
     {
        eo_do(WIDGET(it),
              eo_event_callback_call(EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED_DOUBLE, EO_OBJ(it)),
              eo_event_callback_call(ELM_GENGRID_EVENT_ACTIVATED, EO_OBJ(it)));
     }

   eo_do(WIDGET(it), eo_event_callback_call(EVAS_CLICKABLE_INTERFACE_EVENT_PRESSED, EO_OBJ(it)));
   ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
   if (it->realized)
     it->long_timer = ecore_timer_add
         (_elm_config->longpress_timeout, _long_press_cb, it);
   else
     it->long_timer = NULL;
}

static void
_item_text_realize(Elm_Gen_Item *it,
                   Evas_Object *target,
                   Eina_List **source,
                   const char *parts)
{
   const Eina_List *l;
   const char *key;
   char *s;

   if (!it->itc->func.text_get) return;

   if (!(*source))
     *source = elm_widget_stringlist_get
        (edje_object_data_get(target, "texts"));
   EINA_LIST_FOREACH(*source, l, key)
     {
        if (parts && fnmatch(parts, key, FNM_PERIOD)) continue;

        s = it->itc->func.text_get
           ((void *)WIDGET_ITEM_DATA_GET(EO_OBJ(it)), WIDGET(it), key);
        if (s)
          {
             edje_object_part_text_escaped_set(target, key, s);
             free(s);
          }
        else
          {
             edje_object_part_text_set(target, key, "");
          }
        if (_elm_config->atspi_mode)
          elm_interface_atspi_accessible_name_changed_signal_emit(EO_OBJ(it));
     }
}

static void
_item_content_realize(Elm_Gen_Item *it,
                      Evas_Object *target,
                      Eina_List **contents,
                      const char *src,
                      const char *parts)
{
   Eina_Bool tmp;
   Evas_Object *content;

   if (!parts)
     {
        EINA_LIST_FREE(*contents, content)
          evas_object_del(content);
     }
   if (it->itc->func.content_get)
     {
        Eina_List *source;
        const char *key;

        source = elm_widget_stringlist_get(edje_object_data_get(target, src));

        EINA_LIST_FREE(source, key)
          {
             if (parts && fnmatch(parts, key, FNM_PERIOD))
               continue;

             Evas_Object *old = edje_object_part_swallow_get(target, key);
             if (old)
               {
                  *contents = eina_list_remove(*contents, old);
                  evas_object_del(old);
               }
             content = NULL;
             if (it->itc->func.content_get)
               content = it->itc->func.content_get
                  ((void *)WIDGET_ITEM_DATA_GET(EO_OBJ(it)), WIDGET(it), key);
             if (!content) continue;
             *contents = eina_list_append(*contents, content);
             if (!edje_object_part_swallow(target, key, content))
               {
                  ERR("%s (%p) can not be swallowed into %s",
                      evas_object_type_get(content), content, key);
                  evas_object_hide(content);
                  continue;
               }
             elm_widget_sub_object_add(WIDGET(it), content);
             if (eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()))
               elm_widget_disabled_set(content, EINA_TRUE);

             if (_elm_config->atspi_mode && eo_isa(content, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN))
               eo_do(content, elm_interface_atspi_accessible_parent_set(EO_OBJ(it)));
          }
     }
}

static void
_item_state_realize(Elm_Gen_Item *it, Evas_Object *target, const char *parts)
{
   Eina_List *src;
   const char *key;
   char buf[4096];

   if (!it->itc->func.state_get) return;

   src = elm_widget_stringlist_get(edje_object_data_get(target, "states"));
   EINA_LIST_FREE(src, key)
     {
        if (parts && fnmatch(parts, key, FNM_PERIOD)) continue;

        Eina_Bool on = it->itc->func.state_get
           ((void *)WIDGET_ITEM_DATA_GET(EO_OBJ(it)), WIDGET(it), key);

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
   edje_object_message_signal_process(target);
}

static void
_view_inflate(Evas_Object *view, Elm_Gen_Item *it, Eina_List **sources,
              Eina_List **contents)
{
   if (!view) return;
   if (sources) _item_text_realize(it, view, sources, NULL);
   if (contents) _item_content_realize(it, view, contents, "contents", NULL);
   _item_state_realize(it, view, NULL);
}

/**
 * Apply the right style for the created item view.
 */
static void
_view_style_update(Elm_Gen_Item *it, Evas_Object *view, const char *style)
{
   char buf[1024];
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   snprintf(buf, sizeof(buf), "item/%s", style ? style : "default");

   if (!elm_widget_theme_object_set(WIDGET(it), view,
                                    "gengrid", buf,
                                    elm_widget_style_get(WIDGET(it))))
     {
        ERR("%s is not a valid gengrid item style. "
            "Automatically falls back into default style.",
            style);
        elm_widget_theme_object_set
          (WIDGET(it), view, "gengrid", "item/default", "default");
     }

   edje_object_mirrored_set(view, elm_widget_mirrored_get(WIDGET(it)));
   edje_object_scale_set(view, elm_widget_scale_get(WIDGET(it)) *
                         elm_config_scale_get());
   evas_object_stack_below(view, sd->stack);
}

/**
 * Create a VIEW(it) during _item_realize()
 */
static Evas_Object *
_view_create(Elm_Gen_Item *it, const char *style)
{
   Evas_Object *view = edje_object_add(evas_object_evas_get(WIDGET(it)));
   evas_object_smart_member_add(view, GG_IT(it)->wsd->pan_obj);
   elm_widget_sub_object_add(WIDGET(it), view);
   edje_object_scale_set(view, elm_widget_scale_get(WIDGET(it)) *
                         elm_config_scale_get());

   _view_style_update(it, view, style);
   return view;
}

static void
_view_clear(Evas_Object *view, Eina_List **texts, Eina_List **contents)
{
   const char *part;
   Evas_Object *c;
   const Eina_List *l;

   EINA_LIST_FOREACH(*texts, l, part)
     edje_object_part_text_set(view, part, NULL);
   ELM_SAFE_FREE(*texts, elm_widget_stringlist_free);

   EINA_LIST_FREE(*contents, c)
     evas_object_del(c);
}

static void
_elm_gengrid_item_unrealize(Elm_Gen_Item *it,
                            Eina_Bool calc)
{
   if (!it->realized) return;
   if (GG_IT(it)->wsd->reorder_it == it) return;

   evas_event_freeze(evas_object_evas_get(WIDGET(it)));
   if (!calc)
     eo_do(WIDGET(it), eo_event_callback_call(ELM_GENGRID_EVENT_UNREALIZED, EO_OBJ(it)));
   ELM_SAFE_FREE(it->long_timer, ecore_timer_del);

   _view_clear(VIEW(it), &(it->texts), &(it->contents));

   ELM_SAFE_FREE(it->states, elm_widget_stringlist_free);
   eo_do(EO_OBJ(it), elm_wdg_item_track_cancel());

   it->unrealize_cb(it);

   it->realized = EINA_FALSE;
   it->want_unrealize = EINA_FALSE;

   evas_event_thaw(evas_object_evas_get(WIDGET(it)));
   evas_event_thaw_eval(evas_object_evas_get(WIDGET(it)));
}

static void
_item_mouse_up_cb(void *data,
                  Evas *evas EINA_UNUSED,
                  Evas_Object *obj EINA_UNUSED,
                  void *event_info)
{
   Eina_Bool tmp;
   Evas_Event_Mouse_Up *ev = event_info;
   Eina_Bool dragged = EINA_FALSE;
   Elm_Gen_Item *it = data;
   Elm_Object_Item *eo_it = EO_OBJ(it);
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   Evas_Coord x, y, dx, dy;

   if ((ev->button == 3) && (!it->dragging))
     {
        evas_object_geometry_get(obj, &x, &y, NULL, NULL);
        dx = it->dx - (ev->canvas.x - x);
        dy = it->dy - (ev->canvas.y - y);
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;
        if ((dx < 5) && (dy < 5))
          eo_do(WIDGET(it), eo_event_callback_call
            (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED_RIGHT, EO_OBJ(it)));
        return;
     }

   if (ev->button != 1) return;

   it->down = EINA_FALSE;
   sd->mouse_down = EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     sd->on_hold = EINA_TRUE;
   else sd->on_hold = EINA_FALSE;

   eo_do(WIDGET(it), eo_event_callback_call
     (ELM_GENGRID_EVENT_RELEASED, eo_it));
   ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
   if (it->dragging)
     {
        it->dragging = EINA_FALSE;
        eo_do(WIDGET(it), eo_event_callback_call
          (EVAS_DRAGGABLE_INTERFACE_EVENT_DRAG_STOP, eo_it));
        dragged = EINA_TRUE;
     }

   if ((sd->reorder_mode) &&
       (sd->reorder_it))
     {
        eo_do (WIDGET(it), eo_event_callback_call
          (ELM_WIDGET_EVENT_MOVED, EO_OBJ(sd->reorder_it)));
        sd->reorder_it = NULL;
        sd->move_effect_enabled = EINA_FALSE;
        ecore_job_del(sd->calc_job);
        sd->calc_job =
          ecore_job_add(_calc_job, sd->obj);

        eo_do(WIDGET(it), elm_interface_scrollable_hold_set(EINA_FALSE));
        eo_do(WIDGET(it), elm_interface_scrollable_bounce_allow_set(
              sd->old_h_bounce, sd->old_v_bounce));

        edje_object_signal_emit(VIEW(it), "elm,state,reorder,disabled", "elm");
     }
   if (sd->longpressed)
     {
        sd->longpressed = EINA_FALSE;
        if (!sd->was_selected)
          {
             it->unhighlight_cb(it);
             it->unsel_cb(it);
          }
        sd->was_selected = EINA_FALSE;
        return;
     }
   if (dragged)
     {
        if (it->want_unrealize)
          _elm_gengrid_item_unrealize(it, EINA_FALSE);
     }

   if (eo_do_ret(eo_it, tmp, elm_wdg_item_disabled_get())) return;

   if (sd->on_hold || !it->base->still_in)
     {
        sd->longpressed = EINA_FALSE;
        sd->on_hold = EINA_FALSE;
        return;
     }

   if (sd->focused_item != eo_it)
     elm_object_item_focus_set(eo_it, EINA_TRUE);

   if (sd->multi &&
       ((sd->multi_select_mode != ELM_OBJECT_MULTI_SELECT_MODE_WITH_CONTROL) ||
        (evas_key_modifier_is_set(ev->modifiers, "Control"))))
     {
        if (!it->selected)
          {
             it->highlight_cb(it);
             it->sel_cb(it);
          }
        else
          {
             it->unhighlight_cb(it);
             it->unsel_cb(it);
          }
     }
   else
     {
        if (!it->selected)
          {
             while (sd->selected)
               {
                  Elm_Object_Item *eo_sel = sd->selected->data;
                  Elm_Gen_Item *sel = eo_data_scope_get(eo_sel, ELM_GENGRID_ITEM_CLASS);
                  it->unhighlight_cb(sel);
                  it->unsel_cb(sel);
               }
          }
        else
          {
             const Eina_List *l, *l_next;
             Elm_Object_Item *eo_item2;

             EINA_LIST_FOREACH_SAFE(sd->selected, l, l_next, eo_item2)
               {
                  ELM_GENGRID_ITEM_DATA_GET(eo_item2, item2);
                  if (item2 != it)
                    {
                       it->unhighlight_cb(item2);
                       it->unsel_cb(item2);
                    }
               }
          }
        it->highlight_cb(it);
        it->sel_cb(it);
     }
}

static void
_item_mouse_callbacks_add(Elm_Gen_Item *it,
                          Evas_Object *view)
{
   evas_object_event_callback_add
     (view, EVAS_CALLBACK_MOUSE_IN, _item_mouse_in_cb, it);
   evas_object_event_callback_add
     (view, EVAS_CALLBACK_MOUSE_MOVE, _item_mouse_move_cb, it);
   evas_object_event_callback_add
     (view, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, it);
   evas_object_event_callback_add
     (view, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, it);
}

static void
_item_mouse_callbacks_del(Elm_Gen_Item *it,
                          Evas_Object *view)
{
   evas_object_event_callback_del_full
     (view, EVAS_CALLBACK_MOUSE_IN, _item_mouse_in_cb, it);
   evas_object_event_callback_del_full
     (view, EVAS_CALLBACK_MOUSE_MOVE, _item_mouse_move_cb, it);
   evas_object_event_callback_del_full
     (view, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, it);
   evas_object_event_callback_del_full
     (view, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, it);
}



static void
_elm_gengrid_item_index_update(Elm_Gen_Item *it)
{
   if (it->position_update)
     {
        eo_do(WIDGET(it), eo_event_callback_call
          (ELM_GENGRID_EVENT_INDEX_UPDATE, EO_OBJ(it)));
        it->position_update = EINA_FALSE;
     }
}

static void
_item_unrealize_cb(Elm_Gen_Item *it)
{
   if (!_item_cache_add(it))
     {
        ELM_SAFE_FREE(VIEW(it), evas_object_del);
        ELM_SAFE_FREE(it->spacer, evas_object_del);
     }
}

static char *
_access_info_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)data;
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, NULL);

   if (it->itc->func.text_get)
     {
        const Eina_List *l;
        const char *key;

        if (!(it->texts)) it->texts =
          elm_widget_stringlist_get(edje_object_data_get(VIEW(it), "texts"));

        EINA_LIST_FOREACH(it->texts, l, key)
          {
             char *s = it->itc->func.text_get
                ((void *)WIDGET_ITEM_DATA_GET(EO_OBJ(it)), WIDGET(it), key);
             return s;
          }
     }

   return NULL;
}

static char *
_access_state_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   Elm_Gen_Item *it = (Elm_Gen_Item *)data;
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, NULL);

   if (it->base->disabled)
     return strdup(E_("State: Disabled"));

   return NULL;
}

static void
_access_on_highlight_cb(void *data)
{
   Evas_Coord x, y, w, h;
   Evas_Coord sx, sy, sw, sh;
   Elm_Gen_Item *it = (Elm_Gen_Item *)data;
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it);

   evas_object_geometry_get(it->base->view, &x, &y, &w, &h);

   evas_object_geometry_get(it->base->widget, &sx, &sy, &sw, &sh);
   if ((x < sx) || (y < sy) || ((x + w) > (sx + sw)) || ((y + h) > (sy + sh)))
     elm_gengrid_item_bring_in(EO_OBJ(it),
                               ELM_GENGRID_ITEM_SCROLLTO_IN);
}

static void
_access_widget_item_register(Elm_Gen_Item *it)
{
   Elm_Access_Info *ai;

   _elm_access_widget_item_register(it->base);

   ai = _elm_access_info_get(it->base->access_obj);

   _elm_access_text_set(ai, ELM_ACCESS_TYPE, E_("Gengrid Item"));
   _elm_access_callback_set(ai, ELM_ACCESS_INFO, _access_info_cb, it);
   _elm_access_callback_set(ai, ELM_ACCESS_STATE, _access_state_cb, it);
   _elm_access_on_highlight_hook_set(ai, _access_on_highlight_cb, it);
}

static void
_elm_gengrid_item_focus_update(Elm_Gen_Item *it)
{
   const char *focus_raise;
   Evas_Object *obj = WIDGET(it);
   ELM_GENGRID_DATA_GET(obj, sd);

   if (elm_widget_focus_highlight_enabled_get(obj) || _elm_config->win_auto_focus_enable)
     {
        edje_object_signal_emit
           (VIEW(it), "elm,state,focused", "elm");
     }

   focus_raise = edje_object_data_get(VIEW(it), "focusraise");
   if ((focus_raise) && (!strcmp(focus_raise, "on")))
     {
        Elm_Gen_Item *it1;
        Eina_List *l;

        evas_object_raise(VIEW(it));
        EINA_LIST_FOREACH(sd->group_items, l, it1)
          {
             if (GG_IT(it1)->group_realized)
                evas_object_raise(VIEW(it1));
          }
     }
}

static void
_item_realize(Elm_Gen_Item *it)
{
   Eina_Bool tmp;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   Elm_Object_Item *eo_it = EO_OBJ(it);

   if ((it->realized) ||
       (it->generation < sd->generation))
     return;

   if (!_item_cache_find(it))
     {
        VIEW(it) = _view_create(it, it->itc->item_style);
        if (it->item->nocache_once)
          it->item->nocache_once = EINA_FALSE;
     }

   if (it->spacer && edje_object_part_exists(VIEW(it), "elm.swallow.pad"))
     {
        it->spacer =
           evas_object_rectangle_add(evas_object_evas_get(WIDGET(it)));
        evas_object_color_set(it->spacer, 0, 0, 0, 0);
        elm_widget_sub_object_add(WIDGET(it), it->spacer);
        evas_object_size_hint_min_set(it->spacer, 2 * elm_config_scale_get(), 1);
        edje_object_part_swallow(VIEW(it), "elm.swallow.pad", it->spacer);
     }

   /* access */
   if (_elm_config->access_mode) _access_widget_item_register(it);

   /* infate texts, contents and states of view object */
   _view_inflate(VIEW(it), it, &it->texts, &it->contents);

   if (it->group)
     {
        if ((!sd->group_item_width)
            && (!sd->group_item_height))
          {
             edje_object_size_min_restricted_calc
               (VIEW(it), &sd->group_item_width,
               &sd->group_item_height,
               sd->group_item_width,
               sd->group_item_height);
          }
     }
   else
     {
        if ((!sd->item_width)
            && (!sd->item_height))
          {
             edje_object_size_min_restricted_calc
               (VIEW(it), &sd->item_width,
               &sd->item_height,
               sd->item_width,
               sd->item_height);
             elm_coords_finger_size_adjust
               (1, &sd->item_width, 1,
               &sd->item_height);
          }

        _item_mouse_callbacks_add(it, VIEW(it));
        _elm_gengrid_item_index_update(it);

        if (it->selected)
          edje_object_signal_emit(VIEW(it), "elm,state,selected", "elm");
        if (eo_do_ret(eo_it, tmp, elm_wdg_item_disabled_get()))
          edje_object_signal_emit(VIEW(it), "elm,state,disabled", "elm");
     }
   edje_object_message_signal_process(VIEW(it));
   evas_object_show(VIEW(it));

   if (it->tooltip.content_cb)
     {
        eo_do(eo_it,
              elm_wdg_item_tooltip_content_cb_set(
                 it->tooltip.content_cb, it->tooltip.data, NULL),
              elm_wdg_item_tooltip_style_set(it->tooltip.style),
              elm_wdg_item_tooltip_window_mode_set(it->tooltip.free_size));
     }

   if (it->mouse_cursor)
     eo_do(eo_it, elm_wdg_item_cursor_set(it->mouse_cursor));

   if (it->cursor_engine_only)
     eo_do(eo_it, elm_wdg_item_cursor_engine_only_set(it->cursor_engine_only));

   if (eo_it == sd->focused_item)
     {
        _elm_gengrid_item_focus_update(it);
        _elm_widget_item_highlight_in_theme(WIDGET(it), EO_OBJ(it));
        _elm_widget_highlight_in_theme_update(WIDGET(it));
        _elm_widget_focus_highlight_start(WIDGET(it));
     }

   it->realized = EINA_TRUE;
   it->want_unrealize = EINA_FALSE;
}

static Eina_Bool
_reorder_item_move_animator_cb(void *data)
{
   Elm_Gen_Item *it = data;
   Evas_Coord dx, dy;
   double tt, t;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   tt = REORDER_EFFECT_TIME;
   t = ((0.0 > (t = ecore_loop_time_get() -
                  GG_IT(it)->moving_effect_start_time)) ? 0.0 : t);
   dx = ((GG_IT(it)->tx - GG_IT(it)->ox) / 10)
     * elm_config_scale_get();
   dy = ((GG_IT(it)->ty - GG_IT(it)->oy) / 10)
     * elm_config_scale_get();

   if (t <= tt)
     {
        GG_IT(it)->rx += (1 * sin((t / tt) * (M_PI / 2)) * dx);
        GG_IT(it)->ry += (1 * sin((t / tt) * (M_PI / 2)) * dy);
     }
   else
     {
        GG_IT(it)->rx += dx;
        GG_IT(it)->ry += dy;
     }

   if (((dx > 0) && (GG_IT(it)->rx >= GG_IT(it)->tx)) ||
        ((dx <= 0) && (GG_IT(it)->rx <= GG_IT(it)->tx))
       || ((dy > 0) && (GG_IT(it)->ry >= GG_IT(it)->ty)) ||
           ((dy <= 0) && (GG_IT(it)->ry <= GG_IT(it)->ty)))
     {
        evas_object_move(VIEW(it), GG_IT(it)->tx, GG_IT(it)->ty);
        if (it->group)
          {
             Evas_Coord vw, vh;

             evas_object_geometry_get
               (sd->pan_obj, NULL, NULL, &vw, &vh);
             if (sd->horizontal)
               evas_object_resize
                 (VIEW(it), sd->group_item_width, vh);
             else
               evas_object_resize
                 (VIEW(it), vw, sd->group_item_height);
          }
        else
          evas_object_resize(VIEW(it), sd->item_width, sd->item_height);
        GG_IT(it)->moving = EINA_FALSE;
        GG_IT(it)->item_reorder_move_animator = NULL;

        return ECORE_CALLBACK_CANCEL;
     }

   evas_object_move(VIEW(it), GG_IT(it)->rx, GG_IT(it)->ry);
   if (it->group)
     {
        Evas_Coord vw, vh;

        evas_object_geometry_get(sd->pan_obj, NULL, NULL, &vw, &vh);
        if (sd->horizontal)
          evas_object_resize(VIEW(it), sd->group_item_width, vh);
        else
          evas_object_resize(VIEW(it), vw, sd->group_item_height);
     }
   else
     evas_object_resize(VIEW(it), sd->item_width, sd->item_height);

   return ECORE_CALLBACK_RENEW;
}

static void
_item_place(Elm_Gen_Item *it,
            Evas_Coord cx,
            Evas_Coord cy)
{
   Evas_Coord x, y, ox, oy, cvx, cvy, cvw, cvh, iw, ih, ww;
   Evas_Coord tch, tcw, alignw = 0, alignh = 0, vw, vh;
   Eina_Bool reorder_item_move_forward = EINA_FALSE;
   Eina_Bool was_realized;
   Elm_Gen_Item_Type *item;
   long items_count;
   int item_pos;

   item = GG_IT(it);
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, wsd);

   it->x = cx;
   it->y = cy;
   evas_object_geometry_get(wsd->pan_obj, &ox, &oy, &vw, &vh);

   /* Preload rows/columns at each side of the Gengrid */
   cvx = ox - PRELOAD * wsd->item_width;
   cvy = oy - PRELOAD * wsd->item_height;
   cvw = vw + 2 * PRELOAD * wsd->item_width;
   cvh = vh + 2 * PRELOAD * wsd->item_height;

   items_count = wsd->item_count -
     eina_list_count(wsd->group_items) + wsd->items_lost;
   if (wsd->horizontal)
     {
        int columns, items_visible = 0, items_row;

        if (wsd->item_height > 0)
          items_visible = vh / wsd->item_height;
        if (items_visible < 1)
          items_visible = 1;

        columns = items_count / items_visible;
        if (items_count % items_visible)
          columns++;

        tcw = (wsd->item_width * columns) + (wsd->group_item_width *
                                             eina_list_count(wsd->group_items));
        alignw = (vw - tcw) * wsd->align_x;

        items_row = items_visible;
        if ((unsigned int)items_row > wsd->item_count)
          items_row = wsd->item_count;
        if (wsd->filled && (unsigned int)wsd->nmax
            > (unsigned int)wsd->item_count)
          tch = wsd->nmax * wsd->item_height;
        else
          tch = items_row * wsd->item_height;
        alignh = (vh - tch) * wsd->align_y;
        item_pos = items_row * cx + cy + 1;
        if (item_pos != it->position && !it->position_update)
          {
             it->position = item_pos;
             it->position_update = EINA_TRUE;
          }
     }
   else
     {
        unsigned int rows, items_visible = 0, items_col;

        if (wsd->item_width > 0)
          items_visible = vw / wsd->item_width;
        if (items_visible < 1)
          items_visible = 1;

        rows = items_count / items_visible;
        if (items_count % items_visible)
          rows++;

        tch = (wsd->item_height * rows) + (wsd->group_item_height *
                                           eina_list_count(wsd->group_items));
        alignh = (vh - tch) * wsd->align_y;

        items_col = items_visible;
        if (items_col > wsd->item_count)
          items_col = wsd->item_count;
        if (wsd->filled && (unsigned int)wsd->nmax
            > (unsigned int)wsd->item_count)
          tcw = wsd->nmax * wsd->item_width;
        else
          tcw = items_col * wsd->item_width;
        alignw = (vw - tcw) * wsd->align_x;
        item_pos = cx + items_col * cy + 1;
        if (item_pos != it->position && !it->position_update)
          {
             it->position = item_pos;
             it->position_update = EINA_TRUE;
          }
     }

   if (it->group)
     {
        if (wsd->horizontal)
          {
             x = (((cx - item->prev_group) * wsd->item_width)
                  + (item->prev_group * wsd->group_item_width)) -
               wsd->pan_x + ox + alignw;
             y = oy;
             iw = wsd->group_item_width;
             ih = vh;
          }
        else
          {
             x = ox;
             y = (((cy - item->prev_group) * wsd->item_height)
                  + (item->prev_group * wsd->group_item_height))
               - wsd->pan_y + oy + alignh;
             iw = vw;
             ih = wsd->group_item_height;
          }
        item->gx = x;
        item->gy = y;
     }
   else
     {
        if (wsd->horizontal)
          {
             x = (((cx - item->prev_group) * wsd->item_width)
                  + (item->prev_group * wsd->group_item_width)) -
               wsd->pan_x + ox + alignw;
             y = (cy * wsd->item_height) - wsd->pan_y + oy + alignh;
          }
        else
          {
             x = (cx * wsd->item_width) - wsd->pan_x + ox + alignw;
             y = (((cy - item->prev_group)
                   * wsd->item_height) + (item->prev_group *
                                          wsd->group_item_height)) -
               wsd->pan_y + oy + alignh;
          }
        if (elm_widget_mirrored_get(WIDGET(it))) /* Switch items side
                                                  * and componsate for
                                                  * pan_x when in RTL
                                                  * mode */
          {
             evas_object_geometry_get(WIDGET(it), NULL, NULL, &ww, NULL);
             x = ww - x - wsd->item_width - wsd->pan_x - wsd->pan_x + ox + ox;
          }
        iw = wsd->item_width;
        ih = wsd->item_height;
     }

   was_realized = it->realized;
   if (ELM_RECTS_INTERSECT(x, y, iw, ih, cvx, cvy, cvw, cvh))
     {
        _item_realize(it);
        if (!was_realized)
          {
             _elm_gengrid_item_index_update(it);
             eo_do(WIDGET(it), eo_event_callback_call
               (ELM_GENGRID_EVENT_REALIZED, EO_OBJ(it)));
          }
        if (it->parent)
          {
             if (wsd->horizontal)
               {
                  if (it->parent->item->gx < ox)
                    {
                       it->parent->item->gx = x + wsd->item_width -
                         wsd->group_item_width;
                       if (it->parent->item->gx > ox)
                         it->parent->item->gx = ox;
                    }
                  it->parent->item->group_realized = EINA_TRUE;
               }
             else
               {
                  if (it->parent->item->gy < oy)
                    {
                       it->parent->item->gy = y + wsd->item_height -
                         wsd->group_item_height;
                       if (it->parent->item->gy > oy)
                         it->parent->item->gy = oy;
                    }
                  it->parent->item->group_realized = EINA_TRUE;
               }
          }
        if (wsd->reorder_mode)
          {
             if (wsd->reorder_it)
               {
                  if (item->moving) return;

                  if (!wsd->move_effect_enabled)
                    {
                       item->ox = x;
                       item->oy = y;
                    }
                  if (wsd->reorder_it == it)
                    {
                       evas_object_move(VIEW(it), wsd->reorder_item_x,
                                        wsd->reorder_item_y);
                       evas_object_resize(VIEW(it), iw, ih);
                       return;
                    }
                  else
                    {
                       Evas_Coord nx, ny, nw, nh;

                       if (wsd->move_effect_enabled)
                         {
                            if ((item->ox != x) || (item->oy != y))
                              if (((wsd->old_pan_x == wsd->pan_x)
                                   && (wsd->old_pan_y == wsd->pan_y))
                                  || ((wsd->old_pan_x != wsd->pan_x) &&
                                      !(item->ox - wsd->pan_x
                                        + wsd->old_pan_x == x)) ||
                                  ((wsd->old_pan_y != wsd->pan_y) &&
                                   !(item->oy - wsd->pan_y +
                                     wsd->old_pan_y == y)))
                                {
                                   item->tx = x;
                                   item->ty = y;
                                   item->rx = item->ox;
                                   item->ry = item->oy;
                                   item->moving = EINA_TRUE;
                                   item->moving_effect_start_time =
                                     ecore_loop_time_get();
                                   item->item_reorder_move_animator =
                                     ecore_animator_add
                                       (_reorder_item_move_animator_cb, it);
                                   return;
                                }
                         }

                       /* need fix here */
                       if (it->group)
                         {
                            if (wsd->horizontal)
                              {
                                 nx = x + (wsd->group_item_width / 2);
                                 ny = y;
                                 nw = 1;
                                 nh = vh;
                              }
                            else
                              {
                                 nx = x;
                                 ny = y + (wsd->group_item_height / 2);
                                 nw = vw;
                                 nh = 1;
                              }
                         }
                       else
                         {
                            nx = x + (wsd->item_width / 2);
                            ny = y + (wsd->item_height / 2);
                            nw = 1;
                            nh = 1;
                         }

                       if (ELM_RECTS_INTERSECT
                             (wsd->reorder_item_x, wsd->reorder_item_y,
                             wsd->item_width, wsd->item_height,
                             nx, ny, nw, nh))
                         {
                            if (wsd->horizontal)
                              {
                                 if ((wsd->nmax * wsd->reorder_it->x +
                                      wsd->reorder_it->y) >
                                     (wsd->nmax * it->x + it->y))
                                   reorder_item_move_forward = EINA_TRUE;
                              }
                            else
                              {
                                 if ((wsd->nmax * wsd->reorder_it->y +
                                      wsd->reorder_it->x) >
                                     (wsd->nmax * it->y + it->x))
                                   reorder_item_move_forward = EINA_TRUE;
                              }

                            wsd->items = eina_inlist_remove
                                (wsd->items,
                                EINA_INLIST_GET(wsd->reorder_it));
                            if (reorder_item_move_forward)
                              wsd->items = eina_inlist_prepend_relative
                                  (wsd->items,
                                  EINA_INLIST_GET(wsd->reorder_it),
                                  EINA_INLIST_GET(it));
                            else
                              wsd->items = eina_inlist_append_relative
                                  (wsd->items,
                                  EINA_INLIST_GET(wsd->reorder_it),
                                  EINA_INLIST_GET(it));

                            wsd->reorder_item_changed = EINA_TRUE;
                            wsd->move_effect_enabled = EINA_TRUE;
                            ecore_job_del(wsd->calc_job);
                            wsd->calc_job =
                              ecore_job_add(_calc_job, wsd->obj);

                            return;
                         }
                    }
               }
             else if (item->item_reorder_move_animator)
               {
                  ELM_SAFE_FREE(item->item_reorder_move_animator,
                                ecore_animator_del);
                  item->moving = EINA_FALSE;
               }
          }
        if (!it->group)
          {
             evas_object_move(VIEW(it), x, y);
             evas_object_resize(VIEW(it), iw, ih);
          }
        else
          item->group_realized = EINA_TRUE;
     }
   else
     {
        if (!it->group)
          _elm_gengrid_item_unrealize(it, EINA_FALSE);
        else
          item->group_realized = EINA_FALSE;
     }
}

static void
_group_item_place(Elm_Gengrid_Pan_Data *psd)
{
   Evas_Coord iw, ih, vw, vh;
   Eina_Bool was_realized;
   Elm_Gen_Item *it;
   Eina_List *l;

   evas_object_geometry_get(psd->wsd->pan_obj, NULL, NULL, &vw, &vh);
   if (psd->wsd->horizontal)
     {
        iw = psd->wsd->group_item_width;
        ih = vh;
     }
   else
     {
        iw = vw;
        ih = psd->wsd->group_item_height;
     }

   EINA_LIST_FOREACH(psd->wsd->group_items, l, it)
     {
        was_realized = it->realized;
        if (GG_IT(it)->group_realized)
          {
             _item_realize(it);
             if (!was_realized)
               {
                  _elm_gengrid_item_index_update(it);
                  eo_do(WIDGET(it), eo_event_callback_call
                    (ELM_GENGRID_EVENT_REALIZED, EO_OBJ(it)));
               }
             evas_object_move
               (VIEW(it), GG_IT(it)->gx,
               GG_IT(it)->gy);
             evas_object_resize(VIEW(it), iw, ih);
             evas_object_raise(VIEW(it));
          }
        else
          _elm_gengrid_item_unrealize(it, EINA_FALSE);
     }
}

EOLIAN static void
_elm_gengrid_pan_evas_object_smart_calculate(Eo *obj EINA_UNUSED, Elm_Gengrid_Pan_Data *psd)
{
   Evas_Coord cx = 0, cy = 0;
   Elm_Gen_Item *it;

   Elm_Gengrid_Data *sd = psd->wsd;

   if (!sd->nmax) return;

   sd->reorder_item_changed = EINA_FALSE;

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->group)
          {
             if (sd->horizontal)
               {
                  if (cy)
                    {
                       cx++;
                       cy = 0;
                    }
               }
             else
               {
                  if (cx)
                    {
                       cx = 0;
                       cy++;
                    }
               }
          }

        _item_place(it, cx, cy);
        if (sd->reorder_item_changed) return;
        if (it->group)
          {
             if (sd->horizontal)
               {
                  cx++;
                  cy = 0;
               }
             else
               {
                  cx = 0;
                  cy++;
               }
          }
        else
          {
             if (sd->horizontal)
               {
                  cy = (cy + 1) % sd->nmax;
                  if (!cy) cx++;
               }
             else
               {
                  cx = (cx + 1) % sd->nmax;
                  if (!cx) cy++;
               }
          }
     }
   _group_item_place(psd);

   if ((sd->reorder_mode) && (sd->reorder_it))
     {
        if (!sd->reorder_item_changed)
          {
             sd->old_pan_x = sd->pan_x;
             sd->old_pan_y = sd->pan_y;
          }
        sd->move_effect_enabled = EINA_FALSE;
     }

   eo_do(psd->wobj, eo_event_callback_call
     (ELM_INTERFACE_SCROLLABLE_EVENT_CHANGED, NULL));

   if (sd->focused_item)
     _elm_widget_focus_highlight_start(psd->wobj);
}

EOLIAN static void
_elm_gengrid_pan_elm_pan_pos_set(Eo *obj, Elm_Gengrid_Pan_Data *psd, Evas_Coord x, Evas_Coord y)
{
   if ((x == psd->wsd->pan_x) && (y == psd->wsd->pan_y)) return;
   psd->wsd->pan_x = x;
   psd->wsd->pan_y = y;
   evas_object_smart_changed(obj);
}

EOLIAN static void
_elm_gengrid_pan_elm_pan_pos_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Pan_Data *psd, Evas_Coord *x, Evas_Coord *y)
{
   if (x) *x = psd->wsd->pan_x;
   if (y) *y = psd->wsd->pan_y;
}

EOLIAN static void
_elm_gengrid_pan_elm_pan_content_size_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Pan_Data *psd, Evas_Coord *w, Evas_Coord *h)
{
   if (w) *w = psd->wsd->minw;
   if (h) *h = psd->wsd->minh;
}

EOLIAN static void
_elm_gengrid_pan_elm_pan_pos_max_get(Eo *obj, Elm_Gengrid_Pan_Data *psd, Evas_Coord *x, Evas_Coord *y)
{
   Evas_Coord ow, oh;

   evas_object_geometry_get(obj, NULL, NULL, &ow, &oh);
   if (x)
     *x = (ow < psd->wsd->minw) ? psd->wsd->minw - ow : 0;
   if (y)
     *y = (oh < psd->wsd->minh) ? psd->wsd->minh - oh : 0;
}

EOLIAN static void
_elm_gengrid_pan_elm_pan_pos_min_get(Eo *obj, Elm_Gengrid_Pan_Data *psd, Evas_Coord *x, Evas_Coord *y)
{
   Evas_Coord mx = 0, my = 0;
   Eina_Bool mirrored = elm_widget_mirrored_get(psd->wsd->obj);

   eo_do(obj, elm_obj_pan_pos_max_get(&mx, &my));
   if (x)
     *x = -mx * (mirrored ? 1 - psd->wsd->align_x : psd->wsd->align_x);
   if (y)
     *y = -my * psd->wsd->align_y;
}

EOLIAN static void
_elm_gengrid_pan_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_PAN_CLASS_NAME_LEGACY, klass);
}

#include "elm_gengrid_pan.eo.c"

static void
_elm_gengrid_item_focused(Elm_Object_Item *eo_it)
{
   Eina_Bool tmp;
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);
   Evas_Object *obj = WIDGET(it);
   ELM_GENGRID_DATA_GET(obj, sd);

   if (it->generation < sd->generation)
     return;

   if (_is_no_select(it) ||
       (eo_it == sd->focused_item) ||
       (eo_do_ret(eo_it, tmp, elm_wdg_item_disabled_get())))
     return;

   switch (_elm_config->focus_autoscroll_mode)
     {
      case ELM_FOCUS_AUTOSCROLL_MODE_SHOW:
         elm_gengrid_item_show
            (eo_it, ELM_GENGRID_ITEM_SCROLLTO_IN);
         break;
      case ELM_FOCUS_AUTOSCROLL_MODE_BRING_IN:
         elm_gengrid_item_bring_in
            (eo_it, ELM_GENGRID_ITEM_SCROLLTO_IN);
         break;
      default:
         break;
     }

   sd->focused_item = eo_it;

   /* If item is not realized state, widget couldn't get focus_highlight data. */
   if (it->realized)
     {
        _elm_gengrid_item_focus_update(it);
        _elm_widget_item_highlight_in_theme(obj, eo_it);
        _elm_widget_highlight_in_theme_update(obj);
        _elm_widget_focus_highlight_start(obj);
     }

   eo_do(obj, eo_event_callback_call(ELM_GENGRID_EVENT_ITEM_FOCUSED, eo_it));
   if (_elm_config->atspi_mode)
     elm_interface_atspi_accessible_state_changed_signal_emit(eo_it, ELM_ATSPI_STATE_FOCUSED, EINA_TRUE);
}

static void
_elm_gengrid_item_unfocused(Elm_Object_Item *eo_it)
{
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);
   Evas_Object *obj = WIDGET(it);
   ELM_GENGRID_DATA_GET(obj, sd);

   if (it->generation < sd->generation)
     return;

   if (_is_no_select(it))
     return;

   if ((!sd->focused_item) ||
       (eo_it != sd->focused_item))
     return;

   if (elm_widget_focus_highlight_enabled_get(obj) || _elm_config->win_auto_focus_enable)
     {
        ELM_GENGRID_ITEM_DATA_GET(sd->focused_item, focus_it);
        edje_object_signal_emit
           (VIEW(focus_it), "elm,state,unfocused", "elm");
     }

   sd->focused_item = NULL;
   eo_do(obj, eo_event_callback_call(ELM_GENGRID_EVENT_ITEM_UNFOCUSED, eo_it));
   if (_elm_config->atspi_mode)
     elm_interface_atspi_accessible_state_changed_signal_emit(eo_it, ELM_ATSPI_STATE_FOCUSED, EINA_FALSE);
}

static Eina_Bool
_item_focus_up(Elm_Gengrid_Data *sd)
{
   unsigned int i;
   Elm_Gen_Item *prev = NULL;

   if (!sd->focused_item)
     {
        prev = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);
        while ((prev) && (prev->generation < sd->generation))
          prev = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(prev)->prev);
        elm_object_item_focus_set(EO_OBJ(prev), EINA_TRUE);
        return EINA_TRUE;
     }
   else
     {
        Elm_Object_Item *eo_prev = elm_gengrid_item_prev_get(sd->focused_item);
        if (!eo_prev) return EINA_FALSE;
        prev = eo_data_scope_get(eo_prev, ELM_GENGRID_ITEM_CLASS);
        if (eo_prev == sd->focused_item) return EINA_FALSE;
     }

   for (i = 1; i < sd->nmax; i++)
     {
        Elm_Object_Item *eo_tmp =
          elm_gengrid_item_prev_get(EO_OBJ(prev));
        if (!eo_tmp) return EINA_FALSE;
        prev = eo_data_scope_get(eo_tmp, ELM_GENGRID_ITEM_CLASS);
     }

   elm_object_item_focus_set(EO_OBJ(prev), EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_item_focus_down(Elm_Gengrid_Data *sd)
{
   unsigned int i, idx;
   Elm_Gen_Item *next = NULL;
   Elm_Object_Item *eo_tmp = NULL;

   if (!sd->focused_item)
     {
        next = ELM_GEN_ITEM_FROM_INLIST(sd->items);
        while ((next) && (next->generation < sd->generation))
          next = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(next)->next);
     }
   else
     {

        idx = elm_gengrid_item_index_get(sd->focused_item);

        if (idx > sd->item_count -
            ((sd->item_count % sd->nmax) == 0 ?
             sd->nmax : (sd->item_count % sd->nmax)))
          return EINA_FALSE;
        if (idx > sd->item_count - sd->nmax)
          {
             eo_tmp = elm_gengrid_last_item_get(sd->obj);
             next = eo_data_scope_get(eo_tmp, ELM_GENGRID_ITEM_CLASS);
          }
        else
          {
             next = eo_data_scope_get(sd->focused_item, ELM_GENGRID_ITEM_CLASS);
             for (i = 0; i < sd->nmax; i++)
               {
                  eo_tmp = elm_gengrid_item_next_get(EO_OBJ(next));
                  if (!eo_tmp) return EINA_FALSE;
                  next = eo_data_scope_get(eo_tmp, ELM_GENGRID_ITEM_CLASS);
               }
          }
     }

   elm_object_item_focus_set(EO_OBJ(next), EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_item_focus_left(Elm_Gengrid_Data *sd)
{
   Elm_Gen_Item *prev;

   if (!sd->focused_item)
     {
        prev = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);
        while ((prev) && (prev->generation < sd->generation))
          prev = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(prev)->prev);
     }
   else
     {
        Elm_Object_Item *eo_prev = elm_gengrid_item_prev_get(sd->focused_item);
        if (!eo_prev) return EINA_FALSE;
        prev = eo_data_scope_get(eo_prev, ELM_GENGRID_ITEM_CLASS);
        if (eo_prev == sd->focused_item) return EINA_FALSE;
     }

   elm_object_item_focus_set(EO_OBJ(prev), EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_item_focus_right(Elm_Gengrid_Data *sd)
{
   Elm_Gen_Item *next;

   if (!sd->focused_item)
     {
        next = ELM_GEN_ITEM_FROM_INLIST(sd->items);
        while ((next) && (next->generation < sd->generation))
          next = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(next)->next);
     }
   else
     {
        Elm_Object_Item *eo_next = elm_gengrid_item_next_get(sd->focused_item);
        if (!eo_next) return EINA_FALSE;
        next = eo_data_scope_get(eo_next, ELM_GENGRID_ITEM_CLASS);
        if (eo_next == sd->focused_item) return EINA_FALSE;
     }

   elm_object_item_focus_set(EO_OBJ(next), EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_item_multi_select_left(Elm_Gengrid_Data *sd)
{
   Elm_Object_Item *eo_prev;

   if (!sd->selected) return EINA_FALSE;

   eo_prev = elm_gengrid_item_prev_get(sd->last_selected_item);
   if (!eo_prev) return EINA_TRUE;

   if (elm_gengrid_item_selected_get(eo_prev))
     {
        elm_gengrid_item_selected_set(sd->last_selected_item, EINA_FALSE);
        sd->last_selected_item = eo_prev;
        elm_gengrid_item_show
          (eo_prev, ELM_GENGRID_ITEM_SCROLLTO_IN);
     }
   else
     {
        elm_gengrid_item_selected_set(eo_prev, EINA_TRUE);
        elm_gengrid_item_show(eo_prev, ELM_GENGRID_ITEM_SCROLLTO_IN);
     }

   return EINA_TRUE;
}

static Eina_Bool
_item_multi_select_right(Elm_Gengrid_Data *sd)
{
   Elm_Object_Item *eo_next;

   if (!sd->selected) return EINA_FALSE;

   eo_next = elm_gengrid_item_next_get(sd->last_selected_item);
   if (!eo_next) return EINA_TRUE;

   if (elm_gengrid_item_selected_get(eo_next))
     {
        elm_gengrid_item_selected_set(sd->last_selected_item, EINA_FALSE);
        sd->last_selected_item = eo_next;
        elm_gengrid_item_show
          (eo_next, ELM_GENGRID_ITEM_SCROLLTO_IN);
     }
   else
     {
        elm_gengrid_item_selected_set(eo_next, EINA_TRUE);
        elm_gengrid_item_show(eo_next, ELM_GENGRID_ITEM_SCROLLTO_IN);
     }

   return EINA_TRUE;
}

static Eina_Bool
_item_multi_select_up(Elm_Gengrid_Data *sd)
{
   unsigned int i;
   Eina_Bool r = EINA_TRUE;

   if (!sd->selected) return EINA_FALSE;

   for (i = 0; (r) && (i < sd->nmax); i++)
     r &= _item_multi_select_left(sd);

   return r;
}

static Eina_Bool
_item_multi_select_down(Elm_Gengrid_Data *sd)
{
   unsigned int i;
   Eina_Bool r = EINA_TRUE;

   if (!sd->selected) return EINA_FALSE;

   for (i = 0; (r) && (i < sd->nmax); i++)
     r &= _item_multi_select_right(sd);

   return r;
}

static Eina_Bool
_all_items_deselect(Elm_Gengrid_Data *sd)
{
   if (!sd->selected) return EINA_FALSE;

   while (sd->selected)
     {
        Elm_Object_Item *eo_it = sd->selected->data;
        elm_gengrid_item_selected_set(eo_it, EINA_FALSE);
     }

   return EINA_TRUE;
}

static Eina_Bool
_item_single_select_up(Elm_Gengrid_Data *sd)
{
   unsigned int i;
   Elm_Object_Item *eo_prev;

   if (!sd->selected)
     eo_prev = EO_OBJ(ELM_GEN_ITEM_FROM_INLIST(sd->items->last));
   else
     eo_prev = sd->last_selected_item;

   while (eo_prev)
     {
        for (i = 0; i < sd->nmax; i++)
          {
             eo_prev = elm_gengrid_item_prev_get(eo_prev);
             if (!eo_prev) return EINA_FALSE;
          }

        if (!elm_object_item_disabled_get(eo_prev)) break;
     }

   if (!eo_prev) return EINA_FALSE;

   _all_items_deselect(sd);
   elm_gengrid_item_selected_set(eo_prev, EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_item_single_select_down(Elm_Gengrid_Data *sd)
{
   unsigned int i;
   unsigned int idx;
   Elm_Object_Item *eo_next, *eo_orig;

   if (!sd->selected)
     eo_next = EO_OBJ(ELM_GEN_ITEM_FROM_INLIST(sd->items));
   else
     eo_next = sd->last_selected_item;
   eo_orig = eo_next;

   while (eo_next)
     {
        for (i = 0; i < sd->nmax; i++)
          {
             eo_next = elm_gengrid_item_next_get(eo_next);
             if (!eo_next) break;
          }

        if (eo_next && !elm_object_item_disabled_get(eo_next)) break;
     }

   if (!eo_next)
     {
        idx = elm_gengrid_item_index_get(eo_orig);
        if (idx > sd->item_count -
            ((sd->item_count % sd->nmax) == 0 ?
             sd->nmax : (sd->item_count % sd->nmax)))
           return EINA_FALSE;
        else
           eo_next = elm_gengrid_last_item_get(sd->obj);
     }

   _all_items_deselect(sd);
   elm_gengrid_item_selected_set(eo_next, EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_item_single_select_left(Elm_Gengrid_Data *sd)
{
   Elm_Gen_Item *prev;

   if (!sd->selected)
     {
        prev = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);
        while ((prev) && (prev->generation < sd->generation)
               && elm_object_item_disabled_get(EO_OBJ(prev)))
          prev = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(prev)->prev);
     }
   else
     {
        Elm_Object_Item *eo_prev =
           elm_gengrid_item_prev_get(sd->last_selected_item);
        while (eo_prev)
          {
             if (!elm_object_item_disabled_get(eo_prev))
               break;
             eo_prev = elm_gengrid_item_prev_get(eo_prev);
          }
        prev = eo_data_scope_get(eo_prev, ELM_GENGRID_ITEM_CLASS);
     }

   if (!prev) return EINA_FALSE;

   _all_items_deselect(sd);

   elm_gengrid_item_selected_set(EO_OBJ(prev), EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_item_single_select_right(Elm_Gengrid_Data *sd)
{
   Elm_Gen_Item *next;

   if (!sd->selected)
     {
        next = ELM_GEN_ITEM_FROM_INLIST(sd->items);
        while ((next) && (next->generation < sd->generation)
               && elm_object_item_disabled_get(EO_OBJ(next)))
          next = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(next)->next);
     }
   else
     {
        Elm_Object_Item *eo_next =
           elm_gengrid_item_next_get(sd->last_selected_item);
        while (eo_next)
          {
             if (!elm_object_item_disabled_get(eo_next))
               break;
             eo_next = elm_gengrid_item_next_get(eo_next);
          }
        next = eo_data_scope_get(eo_next, ELM_GENGRID_ITEM_CLASS);
     }

   if (!next) return EINA_FALSE;

   _all_items_deselect(sd);

   elm_gengrid_item_selected_set(EO_OBJ(next), EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_elm_gengrid_item_edge_check(Elm_Object_Item *eo_it,
                             Elm_Focus_Direction dir)
{
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, EINA_FALSE);
   ELM_GENGRID_DATA_GET(WIDGET(it), sd);
   Evas_Coord ix = 0, iy = 0; //item's geometry
   Evas_Coord cx = 0, cy = 0; //prev or next item's geometry
   Elm_Object_Item *eo_item = NULL;
   Elm_Gen_Item *tmp = it;
   Eina_Bool mirrored = elm_widget_mirrored_get(WIDGET(it));

   evas_object_geometry_get(VIEW(it), &ix, &iy, NULL, NULL);

   if (((sd->horizontal) && (dir == ELM_FOCUS_UP)) ||
       ((!sd->horizontal) && (dir == ELM_FOCUS_LEFT)))
     {
        eo_item = elm_gengrid_item_prev_get(EO_OBJ(it));
        while (eo_item)
          {
             if (sd->reorder_mode || !elm_object_item_disabled_get(eo_item)) break;
             eo_item = elm_gengrid_item_prev_get(eo_item);
          }
        if (eo_item)
          {
             ELM_GENGRID_ITEM_DATA_GET(eo_item, item);
             evas_object_geometry_get(VIEW(item), &cx, &cy, NULL, NULL);
             if ((sd->horizontal) && (ix == cx) && (iy > cy))
               return EINA_FALSE;
             else if ((!sd->horizontal) && (iy == cy))
               {
                  if ((!mirrored && (ix > cx)) || (mirrored && (ix < cx)))
                    return EINA_FALSE;
                  else
                    return EINA_TRUE;
               }
             else
               return EINA_TRUE;
          }
        if ((dir == ELM_FOCUS_UP) || (!eo_item && sd->reorder_mode))
          return EINA_TRUE;
     }
   else if (((sd->horizontal) && (dir == ELM_FOCUS_DOWN)) ||
            ((!sd->horizontal) && (dir == ELM_FOCUS_RIGHT)))
     {
        eo_item = elm_gengrid_item_next_get(EO_OBJ(it));
        while (eo_item)
          {
             if (sd->reorder_mode || !elm_object_item_disabled_get(eo_item)) break;
             eo_item = elm_gengrid_item_next_get(eo_item);
          }
        if (eo_item)
          {
             ELM_GENGRID_ITEM_DATA_GET(eo_item, item);
             evas_object_geometry_get(VIEW(item), &cx, &cy, NULL, NULL);
             if ((sd->horizontal) && (ix == cx) && (iy < cy))
               return EINA_FALSE;
             else if ((!sd->horizontal) && (iy == cy))
               {
                  if ((!mirrored && (ix < cx)) || (mirrored && (ix > cx)))
                    return EINA_FALSE;
                  else
                    return EINA_TRUE;
               }
             else
               return EINA_TRUE;
          }
        if ((dir == ELM_FOCUS_DOWN) || (!eo_item && sd->reorder_mode))
          return EINA_TRUE;
     }
   else if (((!sd->horizontal) && (dir == ELM_FOCUS_UP)) ||
            ((sd->horizontal) && (dir == ELM_FOCUS_LEFT)))
     {
        Evas_Coord col, row, cvw, cvh;

        evas_object_geometry_get(sd->pan_obj, NULL, NULL, &cvw, &cvh);
        if (sd->horizontal && sd->item_height > 0)
          {
             row = cvh / sd->item_height;
             if (row <= 0) row = 1;
             col = tmp->position / row;
             if (col == 0)
               return EINA_TRUE;
          }
        else if (sd->item_width > 0)
          {
             col = cvw / sd->item_width;
             if (col <= 0) col = 1;
             row = tmp->position / col;
             if (row == 0)
               return EINA_TRUE;
          }
     }
   else if (((!sd->horizontal) && (dir == ELM_FOCUS_DOWN)) ||
            ((sd->horizontal) && (dir == ELM_FOCUS_RIGHT)))
     {
        Evas_Coord col = 0, row = 0, cvw, cvh;
        int x = 0;

        evas_object_geometry_get(sd->pan_obj, NULL, NULL, &cvw, &cvh);
        if (sd->horizontal && sd->item_height > 0)
          {
             row = cvh / sd->item_height;
             if (row <= 0) row = 1;
             col = sd->item_count / row;
             x = sd->item_count % row;
             if (x == 0)
               {
                  if ((tmp->position <= (row * col)) &&
                      (tmp->position > (row * (col - 1))))
                    return EINA_TRUE;
               }
             else
               {
                  if ((tmp->position <= ((col * row) + x)) &&
                      (tmp->position > ((col - 1) * row) + x))
                    return EINA_TRUE;
               }
          }
        else if (sd->item_width > 0)
          {
             col = cvw / sd->item_width;
             if (col <= 0) col = 1;
             row = sd->item_count / col;
             x = sd->item_count % col;
             if (x == 0)
               {
                  if ((tmp->position <= (col * row)) &&
                      (tmp->position > (col * (row - 1))))
                       return EINA_TRUE;
               }
             else
               {
                  if ((tmp->position <= ((col * row) + x)) &&
                      (tmp->position > (((col * (row - 1)) + x))))
                    return EINA_TRUE;
               }
          }
     }

   return EINA_FALSE;
}

static Elm_Object_Item *
get_up_item(Elm_Gengrid_Data *sd, Elm_Object_Item *eo_it)
{
   Elm_Object_Item *eo_prev = NULL;
   unsigned int i;

   eo_prev = elm_gengrid_item_prev_get(eo_it);
   if (!eo_prev)
     return NULL;

   for (i = 1; i < sd->nmax; i++)
     {
        Elm_Object_Item *eo_tmp = elm_gengrid_item_prev_get(eo_prev);
        if (!eo_tmp) return eo_prev;
        eo_prev = eo_tmp;
     }

   return eo_prev;
}

static Elm_Object_Item *
get_down_item(Elm_Gengrid_Data *sd, Elm_Object_Item *eo_it)
{
   Elm_Object_Item *eo_next = NULL;
   unsigned int i;

   eo_next = elm_gengrid_item_next_get(eo_it);
   if (!eo_next)
     return NULL;

   for (i = 1; i < sd->nmax; i++)
     {
        Elm_Object_Item *eo_tmp = elm_gengrid_item_next_get(eo_next);
        if (!eo_tmp) return eo_next;
        eo_next = eo_tmp;
     }

   return eo_next;
}

typedef struct _Item_Info
{
   Elm_Gen_Item *it;
   Evas_Coord x, y;
} Item_Info;

typedef struct _Reorder_Normal_Data
{
   Item_Info *items;
   Elm_Gen_Item *corner_item;
   int no;
} Reorder_Normal_Data;

static void
_free_reorder_normal_data(Reorder_Normal_Data *rnd)
{
   free(rnd->items);
   free(rnd);
}

static void
_anim_end(Elm_Gengrid_Data *sd)
{
   Eina_Inlist *tmp = NULL;
   Elm_Gen_Item *it1_prev = NULL, *it2_prev = NULL;
   it1_prev = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(sd->reorder.it1)->prev);
   it2_prev = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(sd->reorder.it2)->prev);

   if ((sd->reorder.type == ELM_GENGRID_REORDER_TYPE_NORMAL) &&
       ((!sd->horizontal && ((sd->reorder.dir == ELM_FOCUS_UP) || (sd->reorder.dir == ELM_FOCUS_DOWN))) ||
       (sd->horizontal && ((sd->reorder.dir == ELM_FOCUS_LEFT) || (sd->reorder.dir == ELM_FOCUS_RIGHT)))))
     {
        sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it1));
        if ((sd->reorder.dir == ELM_FOCUS_UP) ||
            (sd->reorder.dir == ELM_FOCUS_LEFT))
          {
             if (it2_prev)
               {
                  tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it2_prev));
                  sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it1),
                                                          tmp);
               }
             else
               sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(sd->reorder.it1));
          }
        else if ((sd->reorder.dir == ELM_FOCUS_DOWN) ||
                 (sd->reorder.dir == ELM_FOCUS_RIGHT))
          {
             tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(sd->reorder.it2));
             sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it1),
                                                          tmp);
          }
     }
   else if ((sd->reorder.type == ELM_GENGRID_REORDER_TYPE_SWAP) &&
            ((!sd->horizontal && ((sd->reorder.dir == ELM_FOCUS_UP) || (sd->reorder.dir == ELM_FOCUS_DOWN))) ||
       (sd->horizontal && ((sd->reorder.dir == ELM_FOCUS_LEFT) || (sd->reorder.dir == ELM_FOCUS_RIGHT)))))
     {
        sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it1));
        sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it2));

        if (it1_prev)
          {
             tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it1_prev));
             sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it2),
                                                     tmp);
          }
        else
          sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(sd->reorder.it2));
        if (it2_prev)
          {
             tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it2_prev));
             sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it1),
                                                     tmp);
          }
        else
          sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(sd->reorder.it1));
     }
   else if ((!sd->horizontal && (sd->reorder.dir == ELM_FOCUS_LEFT)) ||
            (sd->horizontal && (sd->reorder.dir == ELM_FOCUS_UP)))
     {
         if (!(sd->reorder.it2 == it1_prev))
           {
              sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it1));
              sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it2));
           
              if (it1_prev)
                {
                   tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it1_prev));
                   sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it2),
                                                           tmp);
                }
              else
                sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(sd->reorder.it2));
           
              if (it2_prev)
                {
                   tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it2_prev));
                   sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it1),
                                                           tmp);
                }
              else
                sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(sd->reorder.it1));
           }
         else
           {
              sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it1));
              sd->items = eina_inlist_prepend_relative(sd->items, EINA_INLIST_GET(sd->reorder.it1),
                                                       EINA_INLIST_GET(sd->reorder.it2));
           }
     }
   else if ((!sd->horizontal && (sd->reorder.dir == ELM_FOCUS_RIGHT)) ||
            (sd->horizontal && (sd->reorder.dir == ELM_FOCUS_DOWN)))
     {
        if (!(sd->reorder.it1 == it2_prev))
          {
             sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it1));
             sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it2));
          
             if (it1_prev)
               {  
                  tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it1_prev));
                  sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it2),
                                                          tmp);
               }
             else
               sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(sd->reorder.it2));
        
             if (it2_prev)
               {
                  tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it2_prev));
                  sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it1),
                                                          tmp);
               }
             else
               sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(sd->reorder.it1));
          }
        else
          {
             sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(sd->reorder.it1));
             sd->items = eina_inlist_append_relative(sd->items, EINA_INLIST_GET(sd->reorder.it1),
                                                     EINA_INLIST_GET(sd->reorder.it2));
          }
     }
   _item_position_update(sd->items, 0);

   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, sd->obj);
}

static Eina_Bool
_item_move_cb(void *data, double pos)
{
   Elm_Gengrid_Data *sd = data;
   double frame = pos;
   Evas_Coord xx1, yy1, xx2, yy2;
   double dx, dy;

   switch (sd->reorder.tween_mode)
     {
      case ECORE_POS_MAP_LINEAR:
         frame = ecore_animator_pos_map(frame, sd->reorder.tween_mode, 0, 0);
         break;
      case ECORE_POS_MAP_ACCELERATE:
      case ECORE_POS_MAP_DECELERATE:
      case ECORE_POS_MAP_SINUSOIDAL:
         frame = ecore_animator_pos_map(frame, sd->reorder.tween_mode, 1.0, 0);
         break;
      case ECORE_POS_MAP_DIVISOR_INTERP:
      case ECORE_POS_MAP_BOUNCE:
      case ECORE_POS_MAP_SPRING:
         frame = ecore_animator_pos_map(frame, sd->reorder.tween_mode, 1.0, 1.0);
         break;
      default:
         frame = ecore_animator_pos_map(frame, sd->reorder.tween_mode, 0, 0);
         break;
     }

   dx = sd->reorder.x2 - sd->reorder.x1;
   dy = sd->reorder.y2 - sd->reorder.y1;
   xx1 = sd->reorder.x1 + (dx * frame);
   yy1 = sd->reorder.y1 + (dy * frame);

   if (sd->reorder.type == ELM_GENGRID_REORDER_TYPE_NORMAL)
     {
        int i = 0;
        Reorder_Normal_Data *rnd = sd->reorder.data;

        for (;i < rnd->no; i++)
          {
             dx = rnd->items[i + 1].x - rnd->items[i].x;
             dy = rnd->items[i + 1].y - rnd->items[i].y;
             xx2 = rnd->items[i].x + (frame * dx);
             yy2 = rnd->items[i].y + (frame * dy);
             evas_object_move(VIEW(rnd->items[i].it), xx2, yy2);
          }
     }
   else if (sd->reorder.type == ELM_GENGRID_REORDER_TYPE_SWAP)
     {
        xx2 = sd->reorder.x2 - (dx * frame);
        yy2 = sd->reorder.y2 - (dy * frame);
        evas_object_move(VIEW(sd->reorder.it2), xx2, yy2);
     }

   evas_object_move(VIEW(sd->reorder.it1), xx1, yy1);

   if (pos == 1.0)
     {
        _anim_end(sd);
        if (sd->reorder.type == ELM_GENGRID_REORDER_TYPE_NORMAL)
          _free_reorder_normal_data(sd->reorder.data);
        elm_gengrid_item_show(EO_OBJ(sd->reorder.it1),
                              ELM_GENGRID_ITEM_SCROLLTO_IN);
        eo_do(sd->obj, eo_event_callback_call
          (ELM_GENGRID_EVENT_ITEM_REORDER_ANIM_STOP, EO_OBJ(sd->reorder.it1)));
        eo_do(sd->obj, eo_event_callback_call
          (ELM_WIDGET_EVENT_MOVED, EO_OBJ(sd->reorder.it1)));
        sd->reorder.running = EINA_FALSE;
     }
   _elm_widget_focus_highlight_start(sd->obj);

   return EINA_TRUE;
}

static void
_store_nearby_items(Elm_Gengrid_Data *sd)
{
   Reorder_Normal_Data *rnd = sd->reorder.data;
   Eina_Inlist *itr;
   Evas_Coord x, y, cvw, cvh, col = 0, row = 0;
   int i = 0;
   int corner_item_pos = 0;
   Eina_Inlist *it1_list, *it2_list;

   evas_object_geometry_get(sd->pan_obj, NULL, NULL, &cvw, &cvh);

   rnd->no = abs(sd->reorder.it2->position - sd->reorder.it1->position);
   rnd->items = malloc(sizeof(Item_Info) * (rnd->no + 1));
   rnd->corner_item = NULL;

  if (sd->horizontal && sd->item_height > 0)
    {
       row = cvh / sd->item_height;
       if (row <= 0) row = 1;
       if (sd->reorder.dir == ELM_FOCUS_RIGHT)
         {
            corner_item_pos = (sd->reorder.it2->position + 1) / row;
            corner_item_pos = corner_item_pos * row;
            corner_item_pos = sd->reorder.it2->position - corner_item_pos;
         }
       else if (sd->reorder.dir == ELM_FOCUS_LEFT)
         {
            corner_item_pos = (sd->reorder.it2->position + 1) / row;
            corner_item_pos = (corner_item_pos + 1) * row;
            corner_item_pos = corner_item_pos - sd->reorder.it2->position;
         }
    }
  else if (sd->item_width > 0)
    {
       col = cvw / sd->item_width;
       if (col <= 0) col = 1;
       if (sd->reorder.dir == ELM_FOCUS_DOWN)
         {
            corner_item_pos = (sd->reorder.it2->position + 1) / col;
            corner_item_pos = corner_item_pos * col;
            corner_item_pos = sd->reorder.it2->position - corner_item_pos;
         }
       else if (sd->reorder.dir == ELM_FOCUS_UP)
         {
            corner_item_pos = (sd->reorder.it2->position + 1) / col;
            corner_item_pos = (corner_item_pos + 1) * col;
            corner_item_pos = corner_item_pos - sd->reorder.it2->position;
         }
    }

  it1_list = eina_inlist_find(sd->items, EINA_INLIST_GET(sd->reorder.it1));
  it2_list = eina_inlist_find(sd->items, EINA_INLIST_GET(sd->reorder.it2));

  if ((sd->reorder.it1->position) < (sd->reorder.it2)->position)
    {
       for (itr = it2_list; itr != it1_list; itr = itr->prev)
         {
            Elm_Gen_Item *cur = EINA_INLIST_CONTAINER_GET(itr, Elm_Gen_Item);
            evas_object_geometry_get(VIEW(cur), &x, &y, NULL, NULL);
            rnd->items[i].it = cur;
            rnd->items[i].x = x;
            rnd->items[i].y = y;
            if (i == (corner_item_pos - 1))
              rnd->corner_item = cur;
            i++;
         }
       rnd->items[i].it = sd->reorder.it1;
       rnd->items[i].x = sd->reorder.x1;
       rnd->items[i].y = sd->reorder.y1;
    }
  else if (sd->reorder.it1->position > sd->reorder.it2->position)
    {
       for (itr = it2_list; itr != it1_list; itr = itr->next)
         {
            Elm_Gen_Item *cur = EINA_INLIST_CONTAINER_GET(itr, Elm_Gen_Item);
            evas_object_geometry_get(VIEW(cur), &x, &y, NULL, NULL);
            rnd->items[i].it = cur;
            rnd->items[i].x = x;
            rnd->items[i].y = y;
            if (i == (corner_item_pos - 1))
              rnd->corner_item = cur;
            i++;
         }
       rnd->items[i].it = sd->reorder.it1;
       rnd->items[i].x = sd->reorder.x1;
       rnd->items[i].y = sd->reorder.y1;
    }
}

static void
_swap_items(Elm_Object_Item *eo_it1, Elm_Object_Item *eo_it2, Elm_Focus_Direction dir)
{
   ELM_GENGRID_ITEM_DATA_GET(eo_it1, it1);
   ELM_GENGRID_ITEM_DATA_GET(eo_it2, it2);
   ELM_GENGRID_DATA_GET(WIDGET(it1), sd);
   Evas_Coord xx1, yy1, xx2, yy2;

   sd->reorder.running = EINA_TRUE;
   sd->reorder.dir = dir;
   sd->reorder.it1 = it1;
   sd->reorder.it2 = it2;

   evas_object_geometry_get(VIEW(it1), &xx1, &yy1, NULL, NULL);
   evas_object_geometry_get(VIEW(it2), &xx2, &yy2, NULL, NULL);
   sd->reorder.x1 = xx1;
   sd->reorder.y1 = yy1;
   sd->reorder.x2 = xx2;
   sd->reorder.y2 = yy2;

   if (sd->reorder.type == ELM_GENGRID_REORDER_TYPE_NORMAL)
     {
        Reorder_Normal_Data *rnd = malloc(sizeof(Reorder_Normal_Data));
        sd->reorder.data = rnd;
        _store_nearby_items(sd);
        if (rnd->corner_item)
          evas_object_raise(VIEW(rnd->corner_item));
     }
   evas_object_raise(VIEW(it1));
   eo_do(sd->obj, eo_event_callback_call
     (ELM_GENGRID_EVENT_ITEM_REORDER_ANIM_START, EO_OBJ(sd->reorder.it1)));
   //TODO: Add elm config for time
   ecore_animator_timeline_add(0.3, _item_move_cb, sd);
}

static Eina_Bool
_item_horizontal_loop(Evas_Object *obj, Elm_Focus_Direction dir)
{
   ELM_GENGRID_DATA_GET(obj, sd);
   Elm_Object_Item *eo_item = sd->focused_item;
   unsigned int counter, i;

   ELM_GENGRID_ITEM_DATA_GET(eo_item, item);
   if (sd->horizontal)
     {
        if (dir == ELM_FOCUS_UP)
          {
             counter = 0;
             while (!_elm_gengrid_item_edge_check(EO_OBJ(item), dir))
               {
                  eo_item = elm_gengrid_item_prev_get(eo_item);
                  item = eo_data_scope_get(eo_item, ELM_GENGRID_ITEM_CLASS);
                  counter++;
               }
             eo_item = elm_gengrid_first_item_get(obj);
             for (i = 0; i < counter; i++)
               eo_item = elm_gengrid_item_next_get(eo_item);
          }
        else
          {
             while (1)
               {
                  if (_elm_gengrid_item_edge_check(EO_OBJ(item), dir))
                    break;
                  else
                    {
                       for (i = 0; i < sd->nmax; i++)
                         eo_item = elm_gengrid_item_next_get(eo_item);
                       item = eo_data_scope_get(eo_item, ELM_GENGRID_ITEM_CLASS);
                    }
               }
          }
     }
   else
     {
        if (dir == ELM_FOCUS_RIGHT)
          {
             while (!_elm_gengrid_item_edge_check(EO_OBJ(item), dir))
               {
                  if (!elm_gengrid_item_next_get(eo_item))
                    break;
                  eo_item = elm_gengrid_item_next_get(eo_item);
                  item = eo_data_scope_get(eo_item, ELM_GENGRID_ITEM_CLASS);
               }
          }
        else if (dir == ELM_FOCUS_LEFT)
          {
             while (!_elm_gengrid_item_edge_check(EO_OBJ(item), dir))
               {
                  if (!elm_gengrid_item_prev_get(EO_OBJ(item)))
                    break;
                  eo_item = elm_gengrid_item_prev_get(eo_item);
                  item = eo_data_scope_get(eo_item, ELM_GENGRID_ITEM_CLASS);
               }
          }
        else
          return EINA_FALSE;
     }
   if (!eo_item) return EINA_FALSE;
   if (sd->reorder_mode && !(sd->focused_item == eo_item))
     {
        _swap_items(sd->focused_item, eo_item, ELM_FOCUS_RIGHT);
        return EINA_TRUE;
     }
   else
     {
        if (_elm_config->item_select_on_focus_disable)
          elm_object_item_focus_set(eo_item, EINA_TRUE);
        else
          elm_gengrid_item_selected_set(eo_item, EINA_TRUE);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eina_Bool
_key_action_move(Evas_Object *obj, const char *params)
{
   ELM_GENGRID_DATA_GET(obj, sd);
   const char *dir = params;

   Evas_Coord x = 0;
   Evas_Coord y = 0;
   Evas_Coord v_w = 0;
   Evas_Coord v_h = 0;
   Evas_Coord step_x = 0;
   Evas_Coord step_y = 0;
   Evas_Coord page_x = 0;
   Evas_Coord page_y = 0;
   Elm_Object_Item *it = NULL;
   Eina_Bool mirrored = elm_widget_mirrored_get(obj);

   eo_do(obj,
         elm_interface_scrollable_content_pos_get(&x, &y),
         elm_interface_scrollable_step_size_get(&step_x, &step_y),
         elm_interface_scrollable_page_size_get(&page_x, &page_y),
         elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &v_w, &v_h));

   if (sd->reorder_mode && sd->reorder.running) return EINA_TRUE;
   _elm_widget_focus_auto_show(obj);
   if ((!strcmp(dir, "left") && !mirrored) ||
       (!strcmp(dir, "right") && mirrored))
     {
        if (sd->reorder_mode)
          {
             Elm_Object_Item *eo_left;

             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_LEFT))
               {
                  if (sd->item_loop_enable)
                    return EINA_TRUE;
                  return EINA_FALSE;
               }
             if (!sd->horizontal)
               eo_left = elm_gengrid_item_prev_get(sd->focused_item);
             else
               eo_left = get_up_item(sd, sd->focused_item);
             if (!eo_left)
               return EINA_TRUE;
             _swap_items(sd->focused_item, eo_left, ELM_FOCUS_LEFT);

             return EINA_TRUE;
          }
        else
          {
             Evas_Object *next = NULL;
             Elm_Object_Item *next_item = NULL;
             next_item = elm_object_item_focus_next_item_get(sd->focused_item,
                                                             ELM_FOCUS_LEFT);
             if (next_item)
               {
                  elm_object_item_focus_set(next_item, EINA_TRUE);
                  return EINA_TRUE;
               }
             next = elm_object_item_focus_next_object_get(sd->focused_item,
                                                          ELM_FOCUS_LEFT);
             if (next)
               {
                  elm_object_focus_set(next, EINA_TRUE);
                  return EINA_TRUE;
               }
          }
        if (sd->horizontal)
          {
             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_LEFT))
               {
                  if (sd->item_loop_enable)
                    {
                       if (_item_horizontal_loop(obj, ELM_FOCUS_RIGHT))
                         return EINA_TRUE;
                    }
                  return EINA_FALSE;
               }
             if (!_elm_config->item_select_on_focus_disable)
               {
                  return _item_single_select_up(sd);
               }
             else
               return _item_focus_up(sd);
          }
        else
          {
             if (sd->focused_item && (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_LEFT) ||
                 !elm_gengrid_item_prev_get(sd->focused_item)))
               {
                  if (sd->item_loop_enable)
                    {
                       if (_item_horizontal_loop(obj, ELM_FOCUS_RIGHT))
                         return EINA_TRUE;
                    }
                  return EINA_FALSE;
               }
             if (!_elm_config->item_select_on_focus_disable)
               {
                  return _item_single_select_left(sd);
               }
             else
               return _item_focus_left(sd);
          }
     }
   else if ((!strcmp(dir, "left_multi") && !mirrored) ||
            (!strcmp(dir, "right_multi") && mirrored))
     {
        if (sd->horizontal)
          {
             if (_item_multi_select_up(sd)) return EINA_TRUE;
             else if (_item_single_select_up(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
        else
          {
             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_LEFT))
               return EINA_FALSE;
             if (_item_multi_select_left(sd)) return EINA_TRUE;
             else if (_item_single_select_left(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
     }
   else if ((!strcmp(dir, "right") && !mirrored) ||
            (!strcmp(dir, "left") && mirrored))
     {
        if (sd->reorder_mode)
          {
             Elm_Object_Item *eo_right;

             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_RIGHT))
               {
                  if (sd->item_loop_enable)
                    return EINA_TRUE;
                  return EINA_FALSE;
               }
             if (!sd->horizontal)
               eo_right = elm_gengrid_item_next_get(sd->focused_item);
             else
               eo_right = get_down_item(sd, sd->focused_item);
             if (!eo_right)
               return EINA_TRUE;
             _swap_items(sd->focused_item, eo_right, ELM_FOCUS_RIGHT);

             return EINA_TRUE;
          }
        else
          {
             Evas_Object *next = NULL;
             Elm_Object_Item *next_item = NULL;
             next_item = elm_object_item_focus_next_item_get(sd->focused_item,
                                                             ELM_FOCUS_RIGHT);
             if (next_item)
               {
                  elm_object_item_focus_set(next_item, EINA_TRUE);
                  return EINA_TRUE;
               }
             next = elm_object_item_focus_next_object_get(sd->focused_item,
                                                          ELM_FOCUS_RIGHT);
             if (next)
               {
                  elm_object_focus_set(next, EINA_TRUE);
                  return EINA_TRUE;
               }
          }
        if (sd->horizontal)
          {
             if (sd->focused_item && _elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_RIGHT))
               {
                  if (sd->item_loop_enable)
                    {
                       if (_item_horizontal_loop(obj, ELM_FOCUS_UP))
                         return EINA_TRUE;
                    }
               }
             if (!_elm_config->item_select_on_focus_disable)
               {
                  return _item_single_select_down(sd);
               }
             else
               return _item_focus_down(sd);
          }
        else
          {
             if (sd->focused_item && (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_RIGHT) ||
                 !elm_gengrid_item_next_get(sd->focused_item)))
               {
                  if (sd->item_loop_enable)
                    {
                       if (_item_horizontal_loop(obj, ELM_FOCUS_LEFT))
                         return EINA_TRUE;
                    }
                  return EINA_FALSE;
               }
             if (!_elm_config->item_select_on_focus_disable)
               {
                  return _item_single_select_right(sd);
               }
             else
               return _item_focus_right(sd);
          }
     }
   else if ((!strcmp(dir, "right_multi") && !mirrored) ||
            (!strcmp(dir, "left_multi") && mirrored))
     {
        if (sd->horizontal)
          {
             if (_item_multi_select_down(sd)) return EINA_TRUE;
             else if (_item_single_select_down(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
        else
          {
             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_RIGHT))
               return EINA_FALSE;
             if (_item_multi_select_right(sd)) return EINA_TRUE;
             else if (_item_single_select_right(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
     }
   else if (!strcmp(dir, "up"))
     {
        if (sd->reorder_mode)
          {
             Elm_Object_Item *eo_up;

             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_UP))
               return EINA_FALSE;
             if (!sd->horizontal)
               eo_up = get_up_item(sd, sd->focused_item);
             else
               eo_up = elm_gengrid_item_prev_get(sd->focused_item);
             if (!eo_up)
               return EINA_TRUE;
             _swap_items(sd->focused_item, eo_up, ELM_FOCUS_UP);

             return EINA_TRUE;
          }
        else
          {
             Evas_Object *next = NULL;
             Elm_Object_Item *next_item = NULL;
             next_item = elm_object_item_focus_next_item_get(sd->focused_item,
                                                             ELM_FOCUS_UP);
             if (next_item)
               {
                  elm_object_item_focus_set(next_item, EINA_TRUE);
                  return EINA_TRUE;
               }
             next = elm_object_item_focus_next_object_get(sd->focused_item,
                                                          ELM_FOCUS_UP);
             if (next)
               {
                  elm_object_focus_set(next, EINA_TRUE);
                  return EINA_TRUE;
               }
          }
        if (sd->horizontal)
          {
             if (sd->focused_item && _elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_UP))
               return EINA_FALSE;
             if (!_elm_config->item_select_on_focus_disable)
               {
                  if (_item_single_select_left(sd)) return EINA_TRUE;
                  else return EINA_FALSE;
               }
             else
               return _item_focus_left(sd);
          }
        else
          {
             if (!_elm_config->item_select_on_focus_disable)
               {
                  return _item_single_select_up(sd);
               }
             else
               return _item_focus_up(sd);
          }
     }
   else if (!strcmp(dir, "up_multi"))
     {
        if (sd->horizontal)
          {
             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_UP))
               return EINA_FALSE;
             if (_item_multi_select_left(sd)) return EINA_TRUE;
             else if (_item_single_select_left(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
        else
          {
             if (_item_multi_select_up(sd)) return EINA_TRUE;
             else if (_item_single_select_up(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
     }
   else if (!strcmp(dir, "down"))
     {
        if (sd->reorder_mode)
          {
             Elm_Object_Item *eo_down;

             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_DOWN))
               return EINA_FALSE;
             if (!sd->horizontal)
               eo_down = get_down_item(sd, sd->focused_item);
             else
               eo_down = elm_gengrid_item_next_get(sd->focused_item);
             if (!eo_down)
               return EINA_TRUE;
             _swap_items(sd->focused_item, eo_down, ELM_FOCUS_DOWN);

             return EINA_TRUE;
          }
        else
          {
             Evas_Object *next = NULL;
             Elm_Object_Item *next_item = NULL;
             next_item = elm_object_item_focus_next_item_get(sd->focused_item,
                                                             ELM_FOCUS_DOWN);
             if (next_item)
               {
                  elm_object_item_focus_set(next_item, EINA_TRUE);
                  return EINA_TRUE;
               }
             next = elm_object_item_focus_next_object_get(sd->focused_item,
                                                          ELM_FOCUS_DOWN);
             if (next)
               {
                  elm_object_focus_set(next, EINA_TRUE);
                  return EINA_TRUE;
               }
          }
        if (sd->horizontal)
          {
             if (sd->focused_item && _elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_DOWN))
               return EINA_FALSE;
             if (!_elm_config->item_select_on_focus_disable)
               {
                  return _item_single_select_right(sd);
               }
             else
               return _item_focus_right(sd);
          }
        else
          {
             if (!_elm_config->item_select_on_focus_disable)
               {
                  return _item_single_select_down(sd);
               }
             else
               return _item_focus_down(sd);
          }
     }
   else if (!strcmp(dir, "down_multi"))
     {
        if (sd->horizontal)
          {
             if (_elm_gengrid_item_edge_check(sd->focused_item, ELM_FOCUS_DOWN))
               return EINA_FALSE;
             if (_item_multi_select_right(sd)) return EINA_TRUE;
             else if (_item_single_select_right(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
        else
          {
             if (_item_multi_select_down(sd)) return EINA_TRUE;
             else if (_item_single_select_down(sd)) return EINA_TRUE;
             else return EINA_FALSE;
          }
     }
   else if (!strcmp(dir, "first"))
     {
        it = elm_gengrid_first_item_get(obj);
        if (!_elm_config->item_select_on_focus_disable)
          elm_gengrid_item_selected_set(it, EINA_TRUE);
        else
          elm_object_item_focus_set(it, EINA_TRUE);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "last"))
     {
        it = elm_gengrid_last_item_get(obj);
        if (!_elm_config->item_select_on_focus_disable)
          elm_gengrid_item_selected_set(it, EINA_TRUE);
        else
          elm_object_item_focus_set(it, EINA_TRUE);
        return EINA_TRUE;
     }
   else if (!strcmp(dir, "prior"))
     {
        if (sd->horizontal)
          {
             if (page_x < 0)
               x -= -(page_x * v_w) / 100;
             else
               x -= page_x;
          }
        else
          {
             if (page_y < 0)
               y -= -(page_y * v_h) / 100;
             else
               y -= page_y;
          }
     }
   else if (!strcmp(dir, "next"))
     {
        if (sd->horizontal)
          {
             if (page_x < 0)
               x += -(page_x * v_w) / 100;
             else
               x += page_x;
          }
        else
          {
             if (page_y < 0)
               y += -(page_y * v_h) / 100;
             else
               y += page_y;
          }
     }
   else return EINA_FALSE;

   eo_do(obj, elm_interface_scrollable_content_pos_set(x, y, EINA_TRUE));
   return EINA_TRUE;
}

static Eina_Bool
_key_action_select(Evas_Object *obj, const char *params)
{
   ELM_GENGRID_DATA_GET(obj, sd);
   Elm_Object_Item *eo_it = elm_object_focused_item_get(obj);
   if (!eo_it) return EINA_TRUE;
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);

   if (sd->multi &&
       ((sd->multi_select_mode != ELM_OBJECT_MULTI_SELECT_MODE_WITH_CONTROL) ||
        (!strcmp(params, "multi"))))
     {
        if (!it->selected)
          {
             it->highlight_cb(it);
             it->sel_cb(it);
          }
        else it->unsel_cb(it);
     }
   else
     {
        if (!it->selected)
          {
             while (sd->selected)
               {
                  Elm_Object_Item *eo_sel = sd->selected->data;
                  Elm_Gen_Item *sel = eo_data_scope_get(eo_sel, ELM_GENGRID_ITEM_CLASS);
                  it->unsel_cb(sel);
               }
          }
        else
          {
             const Eina_List *l, *l_next;
             Elm_Object_Item *eo_item2;

             EINA_LIST_FOREACH_SAFE(sd->selected, l, l_next, eo_item2)
               {
                  ELM_GENGRID_ITEM_DATA_GET(eo_item2, item2);
                  if (item2 != it) it->unsel_cb(item2);
               }
          }
        it->highlight_cb(it);
        it->sel_cb(it);
     }

   if (!sd->multi)
     eo_do(WIDGET(it), eo_event_callback_call(ELM_GENGRID_EVENT_ACTIVATED, eo_it));

   return EINA_TRUE;
}

static Eina_Bool
_key_action_escape(Evas_Object *obj, const char *params EINA_UNUSED)
{
   ELM_GENGRID_DATA_GET(obj, sd);

   if (!_all_items_deselect(sd)) return EINA_FALSE;
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_event(Eo *obj, Elm_Gengrid_Data *sd, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   (void) src;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;
   if (!sd->items) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

/*
 * This function searches the nearest visible item based on the given item.
 * If the given item is in the gengrid viewport, this returns the given item.
 * Or this searches the realized items and checks the nearest fully visible item
 * according to the given item's position.
 */
static Elm_Object_Item *
_elm_gengrid_nearest_visible_item_get(Evas_Object *obj, Elm_Object_Item *eo_it)
{
   Evas_Coord vx = 0, vy = 0, vw = 0, vh = 0; // gengrid viewport geometry
   Evas_Coord ix = 0, iy = 0, iw = 0, ih = 0; // given item geometry
   Evas_Coord cx = 0, cy = 0, cw = 0, ch = 0; // candidate item geometry
   Eina_List *item_list = NULL, *l = NULL;
   Elm_Object_Item *eo_item = NULL;
   ELM_GENGRID_DATA_GET(obj, sd);
   Eina_Bool search_next = EINA_FALSE;

   if (!eo_it) return NULL;
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);

   evas_object_geometry_get(sd->pan_obj, &vx, &vy, &vw, &vh);
   evas_object_geometry_get(VIEW(it), &ix, &iy, &iw, &ih); // FIXME: check if the item is realized or not

   if (ELM_RECTS_INCLUDE(vx, vy, vw, vh, ix, iy, iw, ih))
     {
        if (!elm_object_item_disabled_get(eo_it))
          return eo_it;
        else
          search_next = EINA_TRUE;
     }

   item_list = elm_gengrid_realized_items_get(obj);

   if ((iy < vy) || search_next)
     {
        EINA_LIST_FOREACH(item_list, l, eo_item)
          {
             ELM_GENGRID_ITEM_DATA_GET(eo_item, item);
             evas_object_geometry_get(VIEW(item), &cx, &cy, &cw, &ch);
             if (ELM_RECTS_INCLUDE(vx, vy, vw, vh, cx, cy, cw, ch) &&
                 !elm_object_item_disabled_get(eo_item))
               {
                  eina_list_free(item_list);
                  return eo_item;
               }
          }
     }
   else
     {
        EINA_LIST_REVERSE_FOREACH(item_list, l, eo_item)
          {
             ELM_GENGRID_ITEM_DATA_GET(eo_item, item);
             evas_object_geometry_get(VIEW(item), &cx, &cy, &cw, &ch);
             if (ELM_RECTS_INCLUDE(vx, vy, vw, vh, cx, cy, cw, ch) &&
                 !elm_object_item_disabled_get(eo_item))
               {
                  eina_list_free(item_list);
                  return eo_item;
               }
          }
     }
   eina_list_free(item_list);

   return eo_it;
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_on_focus(Eo *obj, Elm_Gengrid_Data *sd, Elm_Object_Item *item)
{
   Eina_Bool int_ret = EINA_FALSE;
   Elm_Object_Item *eo_it = NULL;

   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_on_focus(NULL));
   if (!int_ret) return EINA_FALSE;

   if (elm_widget_focus_get(obj) && (sd->selected) &&
       (!sd->last_selected_item))
     {
        Elm_Object_Item *sel = eina_list_data_get(sd->selected);
        sd->last_selected_item = eo_data_scope_get(sel, ELM_GENGRID_ITEM_CLASS);
     }

   if (elm_widget_focus_get(obj) && !sd->mouse_down)
     {
        if (item) eo_it = item;
        else
          {
             if (sd->last_focused_item)
               eo_it = sd->last_focused_item;
             else if (sd->last_selected_item)
               eo_it = sd->last_selected_item;
             else if (_elm_config->first_item_focus_on_first_focus_in)
               eo_it = elm_gengrid_first_item_get(obj);
          }
        if (eo_it)
          {
             eo_it = _elm_gengrid_nearest_visible_item_get(obj, eo_it);
             if (eo_it)
               {
                  if (!_elm_config->item_select_on_focus_disable &&
                      eo_it != sd->last_selected_item)
                    elm_gengrid_item_selected_set(eo_it, EINA_TRUE);
                  else
                    elm_object_item_focus_set(eo_it, EINA_TRUE);
                  _elm_widget_focus_highlight_start(obj);
               }
          }
     }
   else
     {
        if (sd->focused_item)
          {
             sd->last_focused_item = sd->focused_item;
             _elm_gengrid_item_unfocused(sd->focused_item);
          }
     }

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_on_focus_region(Eo *obj, Elm_Gengrid_Data *sd, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
   if (!sd->focused_item) return EINA_FALSE;
   if (elm_object_focus_region_show_mode_get(obj) == ELM_FOCUS_REGION_SHOW_ITEM)
     {
        Evas_Coord vx, vy;
        ELM_GENGRID_ITEM_DATA_GET(sd->focused_item, focus_it);
        evas_object_geometry_get(VIEW(focus_it), x, y, w, h);
        evas_object_geometry_get(obj, &vx, &vy, NULL, NULL);

        *x -= vx;
        *y -= vy;

        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eina_Bool _elm_gengrid_smart_focus_next_enable = EINA_FALSE;

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_focus_next_manager_is(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *_pd EINA_UNUSED)
{
   return _elm_gengrid_smart_focus_next_enable;
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_focus_next(Eo *obj, Elm_Gengrid_Data *sd, Elm_Focus_Direction dir, Evas_Object **next, Elm_Object_Item **next_item)
{
   Eina_List *items = NULL;
   Elm_Gen_Item *it;

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->realized)
          items = eina_list_append(items, it->base->access_obj);
     }

   return elm_widget_focus_list_next_get
            (obj, items, eina_list_data_get, dir, next, next_item);
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_focus_direction_manager_is(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *_pd EINA_UNUSED)
{
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_focus_direction(Eo *obj, Elm_Gengrid_Data *sd EINA_UNUSED, const Evas_Object *base, double degree, Evas_Object **direction, Elm_Object_Item **direction_item, double *weight)
{
   Eina_List *items = NULL, *l = NULL;
   Elm_Object_Item *eo_item = NULL;
   Eina_Bool ret = EINA_FALSE;
   double c_weight = 0.0;

   items = elm_gengrid_realized_items_get(obj);
   eo_item = elm_object_focused_item_get(base);
   if (eo_item)
     {
        ELM_GENGRID_ITEM_DATA_GET(eo_item, base_item);
        base = VIEW(base_item);
     }

   EINA_LIST_FOREACH(items, l, eo_item)
     {
        ELM_GENGRID_ITEM_DATA_GET(eo_item, item);

        c_weight = _elm_widget_focus_direction_weight_get(base, VIEW(item), degree);
        if ((c_weight == -1.0) ||
            ((c_weight != 0.0) && (*weight != -1.0) &&
             ((int)(*weight * 100000000) < (int)(c_weight * 100000000))))
          {
             *direction = (Evas_Object *)obj;
             *direction_item = eo_item;
             *weight = c_weight;
             ret = EINA_TRUE;
          }
     }
   eina_list_free(items);

   return ret;
}

static void
_mirrored_set(Evas_Object *obj,
              Eina_Bool rtl)
{
   Elm_Gen_Item *it;

   ELM_GENGRID_DATA_GET(obj, sd);

   _item_cache_zero(sd);
   eo_do_super(obj, MY_CLASS, elm_interface_scrollable_mirrored_set(rtl));

   if (!sd->items) return;
   it = ELM_GEN_ITEM_FROM_INLIST(sd->items);

   while (it)
     {
        edje_object_mirrored_set(VIEW(it), rtl);
        elm_gengrid_item_update(EO_OBJ(it));
        it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next);
     }
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_theme_apply(Eo *obj, Elm_Gengrid_Data *sd EINA_UNUSED)
{
   Eina_Bool int_ret = EINA_FALSE;
   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_theme_apply());
   if (!int_ret) return EINA_FALSE;

   _mirrored_set(obj, elm_widget_mirrored_get(obj));

   return EINA_TRUE;
}

static void
_item_position_update(Eina_Inlist *list,
                      int idx)
{
   Elm_Gen_Item *it;

   EINA_INLIST_FOREACH(list, it)
     {
        it->position = idx++;
        it->position_update = EINA_TRUE;
     }
}

static void
_elm_gengrid_item_del_not_serious(Elm_Gen_Item *it)
{
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   Elm_Object_Item *eo_it = EO_OBJ(it);

   eo_do(eo_it, elm_wdg_item_pre_notify_del());
   it->generation = sd->generation - 1; /* This means that the item is deleted */

   if (it->walking > 0) return;

   if (it->selected)
     sd->selected = eina_list_remove(sd->selected, eo_it);
   if (sd->last_selected_item == eo_it)
     sd->last_selected_item = NULL;
   if (sd->focused_item == eo_it)
     sd->focused_item = NULL;
   if (sd->last_focused_item == eo_it)
     sd->last_focused_item = NULL;

   if (it->itc->func.del)
     it->itc->func.del((void *)WIDGET_ITEM_DATA_GET(EO_OBJ(it)), WIDGET(it));
}

static void
_elm_gengrid_item_del_serious(Elm_Gen_Item *it)
{
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   sd->item_count--;
   _elm_gengrid_item_del_not_serious(it);
   sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(it));
   if (it->tooltip.del_cb)
     it->tooltip.del_cb((void *)it->tooltip.data, WIDGET(it), it);
   sd->walking -= it->walking;
   ELM_SAFE_FREE(it->long_timer, ecore_timer_del);
   if (it->group)
     sd->group_items = eina_list_remove(sd->group_items, it);

   ELM_SAFE_FREE(sd->state, eina_inlist_sorted_state_free);
   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(sd->calc_cb, sd->obj);

   ELM_SAFE_FREE(it->item, free);
}

static void
_item_del(Elm_Gen_Item *it)
{
   Evas_Object *obj = WIDGET(it);
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   evas_event_freeze(evas_object_evas_get(obj));
   sd->selected = eina_list_remove(sd->selected, it);
   if (it->realized) _elm_gengrid_item_unrealize(it, EINA_FALSE);
   _elm_gengrid_item_del_serious(it);
   elm_gengrid_item_class_unref((Elm_Gengrid_Item_Class *)it->itc);
   evas_event_thaw(evas_object_evas_get(obj));
   evas_event_thaw_eval(evas_object_evas_get(obj));
}

static void
_scroll_animate_start_cb(Evas_Object *obj,
                         void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_ANIM_START, NULL));
}

static void
_scroll_animate_stop_cb(Evas_Object *obj,
                        void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_ANIM_STOP, NULL));
}

static void
_scroll_drag_start_cb(Evas_Object *obj,
                      void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_DRAG_START, NULL));
}

static void
_scroll_drag_stop_cb(Evas_Object *obj,
                     void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_DRAG_STOP, NULL));
}

static void
_edge_left_cb(Evas_Object *obj,
              void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(ELM_GENGRID_EVENT_EDGE_LEFT, NULL));
}

static void
_edge_right_cb(Evas_Object *obj,
               void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(ELM_GENGRID_EVENT_EDGE_RIGHT, NULL));
}

static void
_edge_top_cb(Evas_Object *obj,
             void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(ELM_GENGRID_EVENT_EDGE_TOP, NULL));
}

static void
_edge_bottom_cb(Evas_Object *obj,
                void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(ELM_GENGRID_EVENT_EDGE_BOTTOM, NULL));
}

static void
_scroll_page_change_cb(Evas_Object *obj,
                     void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(ELM_GENGRID_EVENT_SCROLL_PAGE_CHANGED, NULL));
}

static void
_scroll_cb(Evas_Object *obj,
           void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL, NULL));
}

static int
_elm_gengrid_item_compare(const void *data,
                          const void *data1)
{
   Elm_Gen_Item *it, *item1;
   Eina_Compare_Cb cb = NULL;
   ptrdiff_t d;

   it = ELM_GEN_ITEM_FROM_INLIST(data);
   item1 = ELM_GEN_ITEM_FROM_INLIST(data1);
   if (it && GG_IT(it)->wsd->item_compare_cb)
     cb = GG_IT(it)->wsd->item_compare_cb;
   else if (item1 && GG_IT(item1)->wsd->item_compare_cb)
     cb = GG_IT(item1)->wsd->item_compare_cb;
   if (cb && it && item1) return cb(EO_OBJ(it), EO_OBJ(item1));
   d = (char *)data - (char *)data1;
   if (d < 0) return -1;
   if (!d) return 0;
   return 1;
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_disable(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   Eina_Bool tmp;
   if (it->generation < GG_IT(it)->wsd->generation) return;

   if (it->realized)
     {
        if (eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()))
          edje_object_signal_emit(VIEW(it), "elm,state,disabled", "elm");
        else
          edje_object_signal_emit(VIEW(it), "elm,state,enabled", "elm");
     }
}

EOLIAN static Eina_Bool
_elm_gengrid_item_elm_widget_item_del_pre(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   if (it->walking > 0)
     {
        _elm_gengrid_item_del_not_serious(it);
        return EINA_FALSE;
     }

   _item_del(it);
   return EINA_TRUE;
}

EOLIAN static Evas_Object *
_elm_gengrid_item_elm_widget_item_part_content_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it, const char * part)
{
   return edje_object_part_swallow_get(VIEW(it), part);
}

EOLIAN static const char *
_elm_gengrid_item_elm_widget_item_part_text_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it, const char * part)
{
   if (!it->itc->func.text_get) return NULL;
   return edje_object_part_text_get(VIEW(it), part);
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_signal_emit(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it, const char *emission, const char *source)
{
   edje_object_signal_emit(VIEW(it), emission, source);
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_focus_set(Eo *eo_it, Elm_Gen_Item *it, Eina_Bool focused)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it);
   Evas_Object *obj = WIDGET(it);
   ELM_GENGRID_DATA_GET(obj, sd);

   if (focused)
     {
        sd->last_focused_item = eo_it;
        if (!elm_object_focus_get(obj))
          elm_object_focus_set(obj, EINA_TRUE);

        if (!elm_widget_focus_get(obj))
          return;

        if (eo_it != sd->focused_item)
          {
             if (sd->focused_item)
               _elm_gengrid_item_unfocused(sd->focused_item);
             _elm_gengrid_item_focused(eo_it);
          }
     }
   else
     {
        if (!elm_widget_focus_get(obj))
          return;
        _elm_gengrid_item_unfocused(eo_it);
     }
   elm_widget_focus_region_show(obj);
}

EOLIAN static Eina_Bool
_elm_gengrid_item_elm_widget_item_focus_get(Eo *eo_it, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, EINA_FALSE);
   Evas_Object *obj = WIDGET(it);
   ELM_GENGRID_DATA_GET(obj, sd);

   if (eo_it == sd->focused_item)
     return EINA_TRUE;

   return EINA_FALSE;
}

static void
_internal_elm_gengrid_clear(Evas_Object *obj,
                   Eina_Bool standby)
{
   Eina_Inlist *next, *l;

   ELM_GENGRID_DATA_GET(obj, sd);
   if (!sd->items) return;

   if (!standby) sd->generation++;

   ELM_SAFE_FREE(sd->state, eina_inlist_sorted_state_free);

   if (sd->walking > 0)
     {
        sd->clear_me = EINA_TRUE;
        return;
     }
   evas_event_freeze(evas_object_evas_get(obj));
   for (l = sd->items, next = l ? l->next : NULL;
        l;
        l = next, next = next ? next->next : NULL)
     {
        Elm_Gen_Item *it = ELM_GEN_ITEM_FROM_INLIST(l);

        if (it->generation < sd->generation)
          {
             Elm_Gen_Item *itn = NULL;

             if (next) itn = ELM_GEN_ITEM_FROM_INLIST(next);
             if (itn) itn->walking++;  /* prevent early death of subitem */
             it->del_cb(it);
             eo_del(EO_OBJ(it));
             if (itn) itn->walking--;
          }
     }
   sd->clear_me = EINA_FALSE;
   sd->pan_changed = EINA_TRUE;
   ELM_SAFE_FREE(sd->calc_job, ecore_job_del);
   sd->selected = eina_list_free(sd->selected);
   if (sd->clear_cb) sd->clear_cb(sd);
   sd->pan_x = 0;
   sd->pan_y = 0;
   sd->minw = 0;
   sd->minh = 0;

   if (sd->pan_obj)
     {
        evas_object_size_hint_min_set(sd->pan_obj, sd->minw, sd->minh);
        eo_do(sd->pan_obj, eo_event_callback_call
          (ELM_PAN_EVENT_CHANGED, NULL));
     }
   eo_do(obj, elm_interface_scrollable_content_region_show(0, 0, 0, 0));
   evas_event_thaw(evas_object_evas_get(obj));
   evas_event_thaw_eval(evas_object_evas_get(obj));
}

static void
_item_select(Elm_Gen_Item *it)
{
   Evas_Object *obj = WIDGET(it);
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   Elm_Object_Item *eo_it = EO_OBJ(it);

   if (_is_no_select(it) ||
       (it->generation < sd->generation) ||
       (it->decorate_it_set))
     return;

   if (!it->selected)
     {
        it->selected = EINA_TRUE;
        sd->selected = eina_list_append(sd->selected, eo_it);
     }
   else if ((sd->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS) &&
            (it->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS))
     return;

   evas_object_ref(obj);
   it->walking++;
   sd->walking++;
   if (it->func.func) it->func.func((void *)it->func.data, WIDGET(it), eo_it);
   if (it->generation == sd->generation)
     {
        eo_do(WIDGET(it), eo_event_callback_call(EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, eo_it));
        elm_object_item_focus_set(eo_it, EINA_TRUE);
     }

   it->walking--;
   sd->walking--;
   if ((sd->clear_me) && (!sd->walking))
     _internal_elm_gengrid_clear(WIDGET(it), EINA_TRUE);
   else
     {
        if ((!it->walking) && (it->generation < sd->generation))
          {
             it->del_cb(it);
             eo_del(eo_it);
          }
        else
          sd->last_selected_item = eo_it;
     }
   evas_object_unref(obj);
}

EOLIAN static Eo *
_elm_gengrid_item_eo_base_constructor(Eo *eo_it, Elm_Gen_Item *it)
{
   eo_it = eo_do_super_ret(eo_it, ELM_GENGRID_ITEM_CLASS, eo_it, eo_constructor());
   it->base = eo_data_scope_get(eo_it, ELM_WIDGET_ITEM_CLASS);
   eo_do(eo_it, elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_LIST_ITEM));

   return eo_it;
}

static Elm_Gen_Item *
_elm_gengrid_item_new(Elm_Gengrid_Data *sd,
                      const Elm_Gen_Item_Class *itc,
                      const void *data,
                      Evas_Smart_Cb func,
                      const void *func_data)
{
   if (!itc) return NULL;

   Eo *eo_it = eo_add(ELM_GENGRID_ITEM_CLASS, sd->obj);
   if (!eo_it) return NULL;
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);

   it->generation = sd->generation;
   it->itc = itc;
   elm_gengrid_item_class_ref((Elm_Gengrid_Item_Class *)itc);

   WIDGET_ITEM_DATA_SET(EO_OBJ(it), data);
   it->parent = NULL;
   it->func.func = func;
   it->func.data = func_data;

   it->del_cb = (Ecore_Cb)_item_del;
   it->highlight_cb = (Ecore_Cb)_item_highlight;
   it->unhighlight_cb = (Ecore_Cb)_item_unhighlight;
   it->sel_cb = (Ecore_Cb)_item_select;
   it->unsel_cb = (Ecore_Cb)_item_unselect;
   it->unrealize_cb = (Ecore_Cb)_item_unrealize_cb;

   GG_IT(it) = ELM_NEW(Elm_Gen_Item_Type);
   GG_IT(it)->wsd = sd;

   it->group = it->itc->item_style &&
     (!strcmp(it->itc->item_style, "group_index"));
   sd->item_count++;

  return it;
}

/* common layout sizing won't apply here */
EOLIAN static void
_elm_gengrid_elm_layout_sizing_eval(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *_pd EINA_UNUSED)
{
   return;  /* no-op */
}

EOLIAN static void
_elm_gengrid_evas_object_smart_add(Eo *obj, Elm_Gengrid_Data *priv)
{
   Eina_Bool bounce = _elm_config->thumbscroll_bounce_enable;
   Elm_Gengrid_Pan_Data *pan_data;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

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

   priv->calc_cb = (Ecore_Cb)_calc_job;

   priv->generation = 1;

   if (!elm_layout_theme_set(obj, "gengrid", "base",
                             elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   eo_do(obj, elm_interface_scrollable_objects_set(wd->resize_obj, priv->hit_rect));

   priv->old_h_bounce = bounce;
   priv->old_v_bounce = bounce;

   eo_do(obj, elm_interface_scrollable_bounce_allow_set(bounce, bounce));

   eo_do(obj,
         elm_interface_scrollable_animate_start_cb_set
         (_scroll_animate_start_cb),
         elm_interface_scrollable_animate_stop_cb_set(_scroll_animate_stop_cb),
         elm_interface_scrollable_drag_start_cb_set(_scroll_drag_start_cb),
         elm_interface_scrollable_drag_stop_cb_set(_scroll_drag_stop_cb),
         elm_interface_scrollable_edge_left_cb_set(_edge_left_cb),
         elm_interface_scrollable_edge_right_cb_set(_edge_right_cb),
         elm_interface_scrollable_edge_top_cb_set(_edge_top_cb),
         elm_interface_scrollable_edge_bottom_cb_set(_edge_bottom_cb),
         elm_interface_scrollable_scroll_cb_set(_scroll_cb),
         elm_interface_scrollable_page_change_cb_set(_scroll_page_change_cb));

   priv->align_x = 0.5;
   priv->align_y = 0.5;
   priv->highlight = EINA_TRUE;
   priv->item_cache_max = CACHE_MAX;

   priv->pan_obj = eo_add(MY_PAN_CLASS, evas_object_evas_get(obj));
   pan_data = eo_data_scope_get(priv->pan_obj, MY_PAN_CLASS);
   eo_data_ref(obj, NULL);
   pan_data->wobj = obj;
   pan_data->wsd = priv;

   priv->stack = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_smart_member_add(priv->stack, priv->pan_obj);
   evas_object_raise(priv->stack);

   eo_do(obj, elm_interface_scrollable_extern_pan_set(priv->pan_obj));
}

EOLIAN static void
_elm_gengrid_evas_object_smart_del(Eo *obj, Elm_Gengrid_Data *sd)
{
   elm_gengrid_clear(obj);
   ELM_SAFE_FREE(sd->pan_obj, evas_object_del);
   ELM_SAFE_FREE(sd->stack, evas_object_del);

   _item_cache_zero(sd);
   ecore_job_del(sd->calc_job);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

EOLIAN static void
_elm_gengrid_evas_object_smart_move(Eo *obj, Elm_Gengrid_Data *sd, Evas_Coord x, Evas_Coord y)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_move(x, y));

   evas_object_move(sd->hit_rect, x, y);
}

EOLIAN static void
_elm_gengrid_evas_object_smart_resize(Eo *obj, Elm_Gengrid_Data *sd, Evas_Coord w, Evas_Coord h)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_resize(w, h));

   evas_object_resize(sd->hit_rect, w, h);
}

EOLIAN static void
_elm_gengrid_evas_object_smart_member_add(Eo *obj, Elm_Gengrid_Data *sd, Evas_Object *member)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_member_add(member));

   if (sd->hit_rect)
     evas_object_raise(sd->hit_rect);
}

static void
_access_obj_process(Elm_Gengrid_Data * sd, Eina_Bool is_access)
{
   Elm_Gen_Item *it;

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (!it->realized) continue;
        if (is_access) _access_widget_item_register(it);
        else
          _elm_access_widget_item_unregister(it->base);

     }
}

EOLIAN static void
_elm_gengrid_elm_widget_access(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Eina_Bool acs)
{
   _elm_gengrid_smart_focus_next_enable = acs;
   _access_obj_process(sd, _elm_gengrid_smart_focus_next_enable);
}

EAPI Evas_Object *
elm_gengrid_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

EOLIAN static Eo *
_elm_gengrid_eo_base_constructor(Eo *obj, Elm_Gengrid_Data *sd)
{
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());
   sd->obj = obj;

   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_TREE_TABLE));

   return obj;
}

EOLIAN static void
_elm_gengrid_item_size_set(Eo *obj, Elm_Gengrid_Data *sd, Evas_Coord w, Evas_Coord h)
{
   if ((sd->item_width == w) && (sd->item_height == h)) return;
   sd->item_width = w;
   sd->item_height = h;
   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);
}

EOLIAN static void
_elm_gengrid_item_size_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Evas_Coord *w, Evas_Coord *h)
{
   if (w) *w = sd->item_width;
   if (h) *h = sd->item_height;
}

EOLIAN static void
_elm_gengrid_group_item_size_set(Eo *obj, Elm_Gengrid_Data *sd, Evas_Coord w, Evas_Coord h)
{
   if ((sd->group_item_width == w) && (sd->group_item_height == h)) return;
   sd->group_item_width = w;
   sd->group_item_height = h;
   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);
}

EOLIAN static void
_elm_gengrid_group_item_size_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Evas_Coord *w, Evas_Coord *h)
{
   if (w) *w = sd->group_item_width;
   if (h) *h = sd->group_item_height;
}

EOLIAN static void
_elm_gengrid_align_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, double align_x, double align_y)
{
   double old_h, old_y;

   old_h = sd->align_x;
   old_y = sd->align_y;

   if (align_x > 1.0)
     align_x = 1.0;
   else if (align_x < 0.0)
     align_x = 0.0;
   sd->align_x = align_x;

   if (align_y > 1.0)
     align_y = 1.0;
   else if (align_y < 0.0)
     align_y = 0.0;
   sd->align_y = align_y;

   if ((old_h != sd->align_x) || (old_y != sd->align_y))
     evas_object_smart_calculate(sd->pan_obj);
}

EOLIAN static void
_elm_gengrid_align_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, double *align_x, double *align_y)
{
   if (align_x) *align_x = sd->align_x;
   if (align_y) *align_y = sd->align_y;
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_item_append(Eo *obj, Elm_Gengrid_Data *sd, const Elm_Gengrid_Item_Class *itc, const void *data, Evas_Smart_Cb func, const void *func_data)
{
   Elm_Gen_Item *it;

   it = _elm_gengrid_item_new(sd, itc, data, func, func_data);
   if (!it) return NULL;

   sd->items = eina_inlist_append(sd->items, EINA_INLIST_GET(it));
   it->position = sd->item_count;
   it->position_update = EINA_TRUE;

   if (it->group)
     sd->group_items = eina_list_prepend(sd->group_items, it);

   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);

   if (_elm_config->atspi_mode)
     {
        elm_interface_atspi_accessible_added(EO_OBJ(it));
        elm_interface_atspi_accessible_children_changed_added_signal_emit(sd->obj, EO_OBJ(it));
     }

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_item_prepend(Eo *obj, Elm_Gengrid_Data *sd, const Elm_Gengrid_Item_Class *itc, const void *data, Evas_Smart_Cb func, const void *func_data)
{
   Elm_Gen_Item *it;

   it = _elm_gengrid_item_new(sd, itc, data, func, func_data);
   if (!it) return NULL;

   sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(it));
   _item_position_update(sd->items, 1);

   if (it->group)
     sd->group_items = eina_list_append(sd->group_items, it);

   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);

   if (_elm_config->atspi_mode)
     {
        elm_interface_atspi_accessible_added(EO_OBJ(it));
        elm_interface_atspi_accessible_children_changed_added_signal_emit(sd->obj, EO_OBJ(it));
     }

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_item_insert_before(Eo *obj, Elm_Gengrid_Data *sd, const Elm_Gengrid_Item_Class *itc, const void *data, Elm_Object_Item *eo_relative, Evas_Smart_Cb func, const void *func_data)
{
   Elm_Gen_Item *it;
   Eina_Inlist *tmp;
   EINA_SAFETY_ON_NULL_RETURN_VAL(eo_relative, NULL);
   ELM_GENGRID_ITEM_DATA_GET(eo_relative, relative);

   ELM_GENGRID_ITEM_CHECK_OR_RETURN(relative, NULL);

   it = _elm_gengrid_item_new(sd, itc, data, func, func_data);
   if (!it) return NULL;
   sd->items = eina_inlist_prepend_relative
       (sd->items, EINA_INLIST_GET(it),
       EINA_INLIST_GET(relative));
   tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it));
   _item_position_update(tmp, relative->position);

   if (it->group)
     sd->group_items = eina_list_append_relative
         (sd->group_items, it, relative->parent);

   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_item_insert_after(Eo *obj, Elm_Gengrid_Data *sd, const Elm_Gengrid_Item_Class *itc, const void *data, Elm_Object_Item *eo_relative, Evas_Smart_Cb func, const void *func_data)
{
   Elm_Gen_Item *it;
   Eina_Inlist *tmp;
   EINA_SAFETY_ON_NULL_RETURN_VAL(eo_relative, NULL);
   ELM_GENGRID_ITEM_DATA_GET(eo_relative, relative);

   ELM_GENGRID_ITEM_CHECK_OR_RETURN(relative, NULL);

   it = _elm_gengrid_item_new(sd, itc, data, func, func_data);
   if (!it) return NULL;
   sd->items = eina_inlist_append_relative
       (sd->items, EINA_INLIST_GET(it),
       EINA_INLIST_GET(relative));
   tmp = eina_inlist_find(sd->items, EINA_INLIST_GET(it));
   _item_position_update(tmp, relative->position + 1);

   if (it->group)
     sd->group_items = eina_list_prepend_relative
         (sd->group_items, it, relative->parent);

   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_item_sorted_insert(Eo *obj, Elm_Gengrid_Data *sd, const Elm_Gengrid_Item_Class *itc, const void *data, Eina_Compare_Cb comp, Evas_Smart_Cb func, const void *func_data)
{
   Elm_Gen_Item *it;

   it = _elm_gengrid_item_new(sd, itc, data, func, func_data);
   if (!it) return NULL;

   if (!sd->state)
     {
        sd->state = eina_inlist_sorted_state_new();
        eina_inlist_sorted_state_init(sd->state, sd->items);
     }

   sd->item_compare_cb = comp;
   sd->items = eina_inlist_sorted_state_insert
       (sd->items, EINA_INLIST_GET(it), _elm_gengrid_item_compare, sd->state);
   _item_position_update(sd->items, 0);

   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);

   return EO_OBJ(it);
}

EOLIAN static void
_elm_gengrid_horizontal_set(Eo *obj, Elm_Gengrid_Data *sd, Eina_Bool horizontal)
{
   horizontal = !!horizontal;
   if (horizontal == sd->horizontal) return;
   sd->horizontal = horizontal;

   /* Update the items to conform to the new layout */
   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job, obj);
}

EOLIAN static Eina_Bool
_elm_gengrid_horizontal_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->horizontal;
}

EOLIAN static void
_elm_gengrid_clear(Eo *obj, Elm_Gengrid_Data *_pd EINA_UNUSED)
{
   _internal_elm_gengrid_clear(obj, EINA_FALSE);
}

EINA_DEPRECATED EAPI const Evas_Object *
elm_gengrid_item_object_get(const Elm_Object_Item *eo_it)
{
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, NULL);

   return VIEW(it);
}

EOLIAN static void
_elm_gengrid_item_update(Eo *eo_item EINA_UNUSED, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it);

   if (!it->realized) return;
   if (it->want_unrealize) return;

   _elm_gengrid_item_unrealize(it, EINA_TRUE);
   _item_realize(it);
   _item_place(it, it->x, it->y);

   _elm_gengrid_item_index_update(it);
}

EOLIAN static void
_elm_gengrid_item_fields_update(Eo *eo_item EINA_UNUSED, Elm_Gen_Item *it,
                                const char *parts,
                                Elm_Gengrid_Item_Field_Type itf)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it);

   if (!it->realized) return;
   if (it->want_unrealize) return;

   if ((!itf) || (itf & ELM_GENGRID_ITEM_FIELD_TEXT))
     _item_text_realize(it, VIEW(it), &it->texts, parts);

   if ((!itf) || (itf & ELM_GENGRID_ITEM_FIELD_CONTENT))
     _item_content_realize(it, VIEW(it), &it->contents, "contents", parts);

   if ((!itf) || (itf & ELM_GENGRID_ITEM_FIELD_STATE))
     _item_state_realize(it, VIEW(it), parts);

   _item_place(it, it->x, it->y);
}

EOLIAN static const Elm_Gengrid_Item_Class *
_elm_gengrid_item_item_class_get(Eo *eo_item EINA_UNUSED, Elm_Gen_Item *item)
{
   if (item->generation < GG_IT(item)->wsd->generation) return NULL;
   return item->itc;
}

EOLIAN static void
_elm_gengrid_item_item_class_update(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it,
                                   const Elm_Gengrid_Item_Class *itc)
{
   EINA_SAFETY_ON_NULL_RETURN(itc);

   if (it->generation < GG_IT(it)->wsd->generation) return;
   it->itc = itc;
   it->item->nocache_once = EINA_TRUE;

   elm_gengrid_item_update(EO_OBJ(it));
}

EOLIAN static void
_elm_gengrid_item_pos_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it,
                         unsigned int *x,
                         unsigned int *y)
{
   if (x) *x = it->x;
   if (y) *y = it->y;
}

EOLIAN static void
_elm_gengrid_multi_select_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Eina_Bool multi)
{
   sd->multi = !!multi;
}

EOLIAN static Eina_Bool
_elm_gengrid_multi_select_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->multi;
}

EOLIAN static void
_elm_gengrid_multi_select_mode_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Elm_Object_Multi_Select_Mode mode)
{
   if (mode >= ELM_OBJECT_MULTI_SELECT_MODE_MAX)
     return;

   if (sd->multi_select_mode != mode)
     sd->multi_select_mode = mode;
}

EOLIAN static Elm_Object_Multi_Select_Mode
_elm_gengrid_multi_select_mode_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->multi_select_mode;
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_selected_item_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   if (sd->selected) return sd->selected->data;
   return NULL;
}

EOLIAN static const Eina_List*
_elm_gengrid_selected_items_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->selected;
}

EOLIAN static void
_elm_gengrid_item_selected_set(Eo *eo_item EINA_UNUSED, Elm_Gen_Item *it,
      Eina_Bool selected)
{
   Eina_Bool tmp;
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);
   if ((it->generation < sd->generation) || eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()))
     return;
   selected = !!selected;
   if (it->selected == selected) return;

   if (selected)
     {
        if (!sd->multi)
          {
             while (sd->selected)
               {
                  Elm_Object_Item *eo_sel = sd->selected->data;
                  ELM_GENGRID_ITEM_DATA_GET(eo_sel, sel);
                  it->unhighlight_cb(sel);
                  it->unsel_cb(sel);
               }
          }
        it->highlight_cb(it);
        it->sel_cb(it);
        return;
     }
   it->unhighlight_cb(it);
   it->unsel_cb(it);
}

EOLIAN static Eina_Bool
_elm_gengrid_item_selected_get(Eo *eo_item EINA_UNUSED, Elm_Gen_Item *it)
{
   return it->selected;
}

EOLIAN static Eina_List*
_elm_gengrid_realized_items_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   Elm_Gen_Item *it;
   Eina_Bool done = EINA_FALSE;

   Eina_List *ret = NULL;

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->realized)
          {
             done = EINA_TRUE;
             ret = eina_list_append(ret, EO_OBJ(it));
          }
        else
          {
             if (done) break;
          }
     }

   return ret;
}

EOLIAN static void
_elm_gengrid_realized_items_update(Eo *obj, Elm_Gengrid_Data *_pd EINA_UNUSED)
{
   Eina_List *list;
   Elm_Object_Item *it;

   list = elm_gengrid_realized_items_get(obj);
   EINA_LIST_FREE(list, it)
     elm_gengrid_item_update(it);
}

static Evas_Object *
_elm_gengrid_item_label_create(void *data,
                               Evas_Object *obj EINA_UNUSED,
                               Evas_Object *tooltip,
                               void *it   EINA_UNUSED)
{
   Evas_Object *label = elm_label_add(tooltip);
   if (!label)
     return NULL;
   elm_object_style_set(label, "tooltip");
   elm_object_text_set(label, data);
   return label;
}

static void
_elm_gengrid_item_label_del_cb(void *data,
                               Evas_Object *obj EINA_UNUSED,
                               void *event_info EINA_UNUSED)
{
   eina_stringshare_del(data);
}

EAPI void
elm_gengrid_item_tooltip_text_set(Elm_Object_Item *it,
                                  const char *text)
{
   eo_do(it, elm_wdg_item_tooltip_text_set(text));
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_tooltip_text_set(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it, const char *text)
{
   text = eina_stringshare_add(text);
   elm_gengrid_item_tooltip_content_cb_set
     (EO_OBJ(it), _elm_gengrid_item_label_create, text,
     _elm_gengrid_item_label_del_cb);
}

EAPI void
elm_gengrid_item_tooltip_content_cb_set(Elm_Object_Item *item,
                                        Elm_Tooltip_Item_Content_Cb func,
                                        const void *data,
                                        Evas_Smart_Cb del_cb)
{
   eo_do(item, elm_wdg_item_tooltip_content_cb_set(func, data, del_cb));
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_tooltip_content_cb_set(Eo *eo_it, Elm_Gen_Item *it,
                                        Elm_Tooltip_Item_Content_Cb func,
                                        const void *data,
                                        Evas_Smart_Cb del_cb)
{
   ELM_GENGRID_ITEM_CHECK_OR_GOTO(it, error);

   if ((it->tooltip.content_cb != func) || (it->tooltip.data != data))
     {
        if (it->tooltip.del_cb)
           it->tooltip.del_cb((void *)it->tooltip.data, WIDGET(it), it);
        it->tooltip.content_cb = func;
        it->tooltip.data = data;
        it->tooltip.del_cb = del_cb;
     }
   if (VIEW(it))
     {
        eo_do_super(eo_it, ELM_GENGRID_ITEM_CLASS,
              elm_wdg_item_tooltip_content_cb_set
              (it->tooltip.content_cb, it->tooltip.data, NULL));
        eo_do(eo_it,
              elm_wdg_item_tooltip_style_set(it->tooltip.style),
              elm_wdg_item_tooltip_window_mode_set(it->tooltip.free_size));
     }

   return;

error:
   if (del_cb) del_cb((void *)data, NULL, NULL);
}

EAPI void
elm_gengrid_item_tooltip_unset(Elm_Object_Item *item)
{
   eo_do(item, elm_wdg_item_tooltip_unset());
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_tooltip_unset(Eo *eo_it, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it);

   if ((VIEW(it)) && (it->tooltip.content_cb))
     eo_do_super(eo_it, ELM_GENGRID_ITEM_CLASS,
           elm_wdg_item_tooltip_unset());

   if (it->tooltip.del_cb)
     it->tooltip.del_cb((void *)it->tooltip.data, WIDGET(it), it);
   it->tooltip.del_cb = NULL;
   it->tooltip.content_cb = NULL;
   it->tooltip.data = NULL;
   it->tooltip.free_size = EINA_FALSE;
   if (it->tooltip.style)
     eo_do(eo_it, elm_wdg_item_tooltip_style_set(NULL));
}

EAPI void
elm_gengrid_item_tooltip_style_set(Elm_Object_Item *it,
                                   const char *style)
{
   eo_do(it, elm_wdg_item_tooltip_style_set(style));
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_tooltip_style_set(Eo *eo_it, Elm_Gen_Item *it,
                                   const char *style)
{
   eina_stringshare_replace(&it->tooltip.style, style);
   if (VIEW(it)) eo_do_super(eo_it, ELM_GENGRID_ITEM_CLASS,
         elm_wdg_item_tooltip_style_set(style));
}

EAPI const char *
elm_gengrid_item_tooltip_style_get(const Elm_Object_Item *it)
{
   const char *ret;
   return eo_do_ret(it, ret, elm_wdg_item_tooltip_style_get());
}

EOLIAN static const char *
_elm_gengrid_item_elm_widget_item_tooltip_style_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   return it->tooltip.style;
}

EAPI Eina_Bool
elm_gengrid_item_tooltip_window_mode_set(Elm_Object_Item *it,
                                         Eina_Bool disable)
{
   Eina_Bool ret;
   return eo_do_ret(it, ret, elm_wdg_item_tooltip_window_mode_set(disable));
}

EOLIAN static Eina_Bool
_elm_gengrid_item_elm_widget_item_tooltip_window_mode_set(Eo *eo_it, Elm_Gen_Item *it,
                                   Eina_Bool disable)
{
   it->tooltip.free_size = disable;
   if (VIEW(it))
     {
        Eina_Bool ret;
        eo_do_super(eo_it, ELM_GENGRID_ITEM_CLASS,
            ret = elm_wdg_item_tooltip_window_mode_set(disable));
        return ret;
     }

   return EINA_TRUE;
}

EAPI Eina_Bool
elm_gengrid_item_tooltip_window_mode_get(const Elm_Object_Item *it)
{
   Eina_Bool ret;
   return eo_do_ret(it, ret, elm_wdg_item_tooltip_window_mode_get());
}

EOLIAN static Eina_Bool
_elm_gengrid_item_elm_widget_item_tooltip_window_mode_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, EINA_FALSE);

   return it->tooltip.free_size;
}

EAPI void
elm_gengrid_item_cursor_set(Elm_Object_Item *it,
                            const char *cursor)
{
   eo_do(it, elm_wdg_item_cursor_set(cursor));
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_cursor_set(Eo *eo_it, Elm_Gen_Item *it,
                            const char *cursor)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it);

   eina_stringshare_replace(&it->mouse_cursor, cursor);
   if (VIEW(it)) eo_do_super(eo_it, ELM_GENGRID_ITEM_CLASS,
         elm_wdg_item_cursor_set(cursor));
}

EAPI const char *
elm_gengrid_item_cursor_get(const Elm_Object_Item *it)
{
   const char *ret;
   return eo_do_ret(it, ret, elm_wdg_item_cursor_get());
}

EAPI void
elm_gengrid_item_cursor_unset(Elm_Object_Item *item)
{
   eo_do(item, elm_wdg_item_cursor_unset());
}

EOLIAN static void
_elm_gengrid_item_elm_widget_item_cursor_unset(Eo *eo_it, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it);
   if (!it->mouse_cursor)
     return;

   if (VIEW(it)) eo_do_super(eo_it, ELM_GENGRID_ITEM_CLASS,
         elm_wdg_item_cursor_unset());

   ELM_SAFE_FREE(it->mouse_cursor, eina_stringshare_del);
}

EAPI void
elm_gengrid_item_cursor_style_set(Elm_Object_Item *it,
                                  const char *style)
{
   eo_do(it, elm_wdg_item_cursor_style_set(style));
}

EAPI const char *
elm_gengrid_item_cursor_style_get(const Elm_Object_Item *it)
{
   const char *ret;
   return eo_do_ret(it, ret, elm_wdg_item_cursor_style_get());
}

EAPI void
elm_gengrid_item_cursor_engine_only_set(Elm_Object_Item *eo_it,
                                        Eina_Bool engine_only)
{
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);
   it->cursor_engine_only = engine_only;
   if (it->realized)
     eo_do(eo_it, elm_wdg_item_cursor_engine_only_set(engine_only));
}

EAPI Eina_Bool
elm_gengrid_item_cursor_engine_only_get(const Elm_Object_Item *eo_it)
{
   ELM_GENGRID_ITEM_DATA_GET(eo_it, it);
   Eina_Bool ret;
   if (it->realized)
     return eo_do_ret(eo_it, ret, elm_wdg_item_cursor_engine_only_get());
   else return it->cursor_engine_only;
}

EOLIAN static void
_elm_gengrid_reorder_mode_start(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Ecore_Pos_Map tween_mode)
{
   sd->reorder_mode = EINA_TRUE;
   sd->reorder.tween_mode = tween_mode;
   sd->reorder.type = ELM_GENGRID_REORDER_TYPE_NORMAL;
}

EOLIAN static void
_elm_gengrid_reorder_mode_stop(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   sd->reorder_mode = EINA_FALSE;
   sd->reorder.tween_mode = -1;
}

EOLIAN static void
_elm_gengrid_reorder_type_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Elm_Gengrid_Reorder_Type type)
{
   sd->reorder.type = type;
}

EOLIAN static void
_elm_gengrid_reorder_mode_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Eina_Bool reorder_mode)
{
   sd->reorder_mode = !!reorder_mode;
}

EOLIAN static Eina_Bool
_elm_gengrid_reorder_mode_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->reorder_mode;
}

EAPI void
elm_gengrid_bounce_set(Evas_Object *obj,
                       Eina_Bool h_bounce,
                       Eina_Bool v_bounce)
{
   ELM_GENGRID_CHECK(obj);
   eo_do(obj, elm_interface_scrollable_bounce_allow_set(h_bounce, v_bounce));
}

EOLIAN static void
_elm_gengrid_elm_interface_scrollable_bounce_allow_set(Eo *obj, Elm_Gengrid_Data *sd EINA_UNUSED, Eina_Bool h_bounce, Eina_Bool v_bounce)
{
   eo_do_super(obj, MY_CLASS, elm_interface_scrollable_bounce_allow_set(!!h_bounce, !!v_bounce));
}

EAPI void
elm_gengrid_bounce_get(const Evas_Object *obj,
                       Eina_Bool *h_bounce,
                       Eina_Bool *v_bounce)
{
   ELM_GENGRID_CHECK(obj);
   eo_do( obj, elm_interface_scrollable_bounce_allow_get(h_bounce, v_bounce));
}

EOLIAN static void
_elm_gengrid_page_relative_set(Eo *obj, Elm_Gengrid_Data *_pd EINA_UNUSED, double h_pagerel, double v_pagerel)
{
   Evas_Coord pagesize_h;
   Evas_Coord pagesize_v;

   eo_do( obj, elm_interface_scrollable_paging_get(NULL, NULL, &pagesize_h, &pagesize_v));
   eo_do( obj, elm_interface_scrollable_paging_set
     (h_pagerel, v_pagerel, pagesize_h, pagesize_v));
}

EOLIAN static void
_elm_gengrid_page_relative_get(Eo *obj, Elm_Gengrid_Data *_pd EINA_UNUSED, double *h_pagerel, double *v_pagerel)
{
   eo_do( obj, elm_interface_scrollable_paging_get(h_pagerel, v_pagerel, NULL, NULL));
}

EOLIAN static void
_elm_gengrid_page_size_set(Eo *obj, Elm_Gengrid_Data *_pd EINA_UNUSED, Evas_Coord h_pagesize, Evas_Coord v_pagesize)
{
   double pagerel_h;
   double pagerel_v;

   eo_do( obj, elm_interface_scrollable_paging_get(&pagerel_h, &pagerel_v, NULL, NULL));
   eo_do( obj, elm_interface_scrollable_paging_set
     (pagerel_h, pagerel_v, h_pagesize, v_pagesize));
}

EAPI void
elm_gengrid_current_page_get(const Evas_Object *obj,
                             int *h_pagenumber,
                             int *v_pagenumber)
{
   ELM_GENGRID_CHECK(obj);
   eo_do( obj, elm_interface_scrollable_current_page_get(h_pagenumber, v_pagenumber));
}

EAPI void
elm_gengrid_last_page_get(const Evas_Object *obj,
                          int *h_pagenumber,
                          int *v_pagenumber)
{
   ELM_GENGRID_CHECK(obj);
   eo_do( obj, elm_interface_scrollable_last_page_get(h_pagenumber, v_pagenumber));
}

EINA_DEPRECATED EAPI void
elm_gengrid_page_show(const Evas_Object *obj,
                      int h_pagenumber,
                      int v_pagenumber)
{
   ELM_GENGRID_CHECK(obj);
   eo_do( obj, elm_interface_scrollable_page_show(h_pagenumber, v_pagenumber));
}

EINA_DEPRECATED EAPI void
elm_gengrid_page_bring_in(const Evas_Object *obj,
                          int h_pagenumber,
                          int v_pagenumber)
{
   ELM_GENGRID_CHECK(obj);
   eo_do( obj, elm_interface_scrollable_page_bring_in(h_pagenumber, v_pagenumber));
}

EAPI void
elm_gengrid_scroller_policy_set(Evas_Object *obj,
                                Elm_Scroller_Policy policy_h,
                                Elm_Scroller_Policy policy_v)
{
   ELM_GENGRID_CHECK(obj);
   eo_do(obj, elm_interface_scrollable_policy_set(policy_h, policy_v));
}

EOLIAN static void
_elm_gengrid_elm_interface_scrollable_policy_set(Eo *obj, Elm_Gengrid_Data *sd EINA_UNUSED, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
{
   if ((policy_h >= ELM_SCROLLER_POLICY_LAST) ||
       (policy_v >= ELM_SCROLLER_POLICY_LAST))
     return;

   eo_do_super(obj, MY_CLASS, elm_interface_scrollable_policy_set(policy_h, policy_v));
}

EAPI void
elm_gengrid_scroller_policy_get(const Evas_Object *obj,
                                Elm_Scroller_Policy *policy_h,
                                Elm_Scroller_Policy *policy_v)
{
   ELM_GENGRID_CHECK(obj);
   eo_do( obj, elm_interface_scrollable_policy_get(policy_h, policy_v));
}

EOLIAN static void
_elm_gengrid_elm_interface_scrollable_policy_get(Eo *obj, Elm_Gengrid_Data *sd EINA_UNUSED, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v)
{
   Elm_Scroller_Policy s_policy_h, s_policy_v;

   eo_do_super(obj, MY_CLASS, elm_interface_scrollable_policy_get(&s_policy_h, &s_policy_v));
   if (policy_h) *policy_h = (Elm_Scroller_Policy)s_policy_h;
   if (policy_v) *policy_v = (Elm_Scroller_Policy)s_policy_v;
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_first_item_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   if (!sd->items) return NULL;

   Elm_Gen_Item *it = ELM_GEN_ITEM_FROM_INLIST(sd->items);

   while ((it) && (it->generation < sd->generation))
     it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next);

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_at_xy_item_get(const Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Evas_Coord x, Evas_Coord y, int *xposret, int *yposret)
{
   Elm_Gen_Item *it = ELM_GEN_ITEM_FROM_INLIST(sd->items);

   Evas_Coord l = 0, r = 0, t = 0, b = 0; /* left, right, top, bottom */
   Eina_Bool init = EINA_TRUE;

   while ((it) && (it->generation < sd->generation))
     it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next);

   if (it)
     do
       {
          Evas_Coord itx, ity;
          Evas_Coord itw, ith;
          evas_object_geometry_get(VIEW(it), &itx, &ity, &itw, &ith);

          /* Record leftmost, rightmost, top, bottom cords to set posret */
          if ((itw > 0) && (ith > 0) && (itx >= 0) && (ity >= 0))
            {  /* A scroller, ignore items in negative cords,or not rendered */
               if (init)
                 {
                    l = itx;
                    r = itx + itw;
                    t = ity;
                    b = ity + ith;
                    init = EINA_FALSE;
                 }
               else
                 {
                    if (itx < l)
                      l = itx;
                    if ((itx + itw) > r)
                      r = itx + itw;
                    if (ity < t)
                      t = ity;
                    if ((ity + ith) > b)
                      b = ity + ith;
                 }
            }

          if (ELM_RECTS_INTERSECT
                (itx, ity, itw, ith, x, y, 1, 1))
            {
               if (yposret)
                 {
                    if (y <= (ity + (ith / 4))) *yposret = -1;
                    else if (y >= (ity + ith - (ith / 4)))
                      *yposret = 1;
                    else *yposret = 0;
                 }

               if (xposret)
                 {
                    if (x <= (itx + (itw / 4))) *xposret = -1;
                    else if (x >= (itx + itw - (itw / 4)))
                      *xposret = 1;
                    else *xposret = 0;
                 }

               return EO_OBJ(it);
            }

       } while ((it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next)));

   /* No item found, tell the user if hit left/right/top/bottom of items */
   if (xposret)
     {
        *xposret = 0;
        if (x < l)
          *xposret = (-1);
        else if (x > r)
          *xposret = (1);
     }

   if (yposret)
     {
        *yposret = 0;
        if (y < t)
          *yposret = (-1);
        else if (y > b)
          *yposret = (1);
     }

   return NULL;
}

EOLIAN static Elm_Object_Item*
_elm_gengrid_last_item_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   if (!sd->items) return NULL;

   Elm_Gen_Item *it = ELM_GEN_ITEM_FROM_INLIST(sd->items->last);
   while ((it) && (it->generation < sd->generation))
     it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->prev);

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item *
_elm_gengrid_item_next_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, NULL);

   while (it)
     {
        it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next);
        if ((it) && (it->generation == GG_IT(it)->wsd->generation)) break;
     }

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item *
_elm_gengrid_item_prev_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, NULL);

   while (it)
     {
        it = ELM_GEN_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->prev);
        if ((it) && (it->generation == GG_IT(it)->wsd->generation)) break;
     }

   return EO_OBJ(it);
}

EOLIAN static void
_elm_gengrid_item_show(Eo *eo_it, Elm_Gen_Item *it, Elm_Gengrid_Item_Scrollto_Type type)
{
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   if ((it->generation < sd->generation)) return;

   sd->show_region = EINA_TRUE;
   sd->show_it = eo_it;
   sd->scroll_to_type = type;

   _item_show_region(sd);
}

EOLIAN static void
_elm_gengrid_item_bring_in(Eo *eo_it, Elm_Gen_Item *it, Elm_Gengrid_Item_Scrollto_Type type)
{
   ELM_GENGRID_DATA_GET_FROM_ITEM(it, sd);

   if (it->generation < sd->generation) return;

   sd->bring_in = EINA_TRUE;
   sd->bring_in_it = eo_it;
   sd->scroll_to_type = type;

   _item_show_region(sd);
}

EOLIAN static void
_elm_gengrid_filled_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Eina_Bool fill)
{
   fill = !!fill;
   if (sd->filled != fill)
     sd->filled = fill;
}

EOLIAN static Eina_Bool
_elm_gengrid_filled_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->filled;
}

EOLIAN static unsigned int
_elm_gengrid_items_count(const Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->item_count;
}

EAPI Elm_Gengrid_Item_Class *
elm_gengrid_item_class_new(void)
{
   Elm_Gengrid_Item_Class *itc;

   itc = calloc(1, sizeof(Elm_Gengrid_Item_Class));
   if (!itc)
     return NULL;
   itc->version = CLASS_ALLOCATED;
   itc->refcount = 1;
   itc->delete_me = EINA_FALSE;

   return itc;
}

EAPI void
elm_gengrid_item_class_free(Elm_Gengrid_Item_Class *itc)
{
   if (!itc || (itc->version != CLASS_ALLOCATED)) return;

   if (!itc->delete_me) itc->delete_me = EINA_TRUE;
   if (itc->refcount > 0) elm_gengrid_item_class_unref(itc);
   else
     {
        itc->version = 0;
        free(itc);
     }
}

EAPI void
elm_gengrid_item_class_ref(Elm_Gengrid_Item_Class *itc)
{
   if (itc && (itc->version == CLASS_ALLOCATED))
     {
        itc->refcount++;
        if (itc->refcount == 0) itc->refcount--;
     }
}

EAPI void
elm_gengrid_item_class_unref(Elm_Gengrid_Item_Class *itc)
{
   if (itc && (itc->version == CLASS_ALLOCATED))
     {
        if (itc->refcount > 0) itc->refcount--;
        if (itc->delete_me && (!itc->refcount))
          elm_gengrid_item_class_free(itc);
     }
}

EOLIAN static void
_elm_gengrid_select_mode_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Elm_Object_Select_Mode mode)
{
   if (mode >= ELM_OBJECT_SELECT_MODE_MAX)
     return;

   if (sd->select_mode != mode)
     sd->select_mode = mode;
}

EOLIAN static Elm_Object_Select_Mode
_elm_gengrid_select_mode_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->select_mode;
}

EOLIAN static void
_elm_gengrid_highlight_mode_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Eina_Bool highlight)
{
   sd->highlight = !!highlight;
}

EOLIAN static Eina_Bool
_elm_gengrid_highlight_mode_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->highlight;
}

EOLIAN static int
_elm_gengrid_item_index_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, -1);

   return it->position;
}

EOLIAN static void
_elm_gengrid_item_select_mode_set(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it,
                                 Elm_Object_Select_Mode mode)
{
   if (it->generation < GG_IT(it)->wsd->generation) return;
   if (mode >= ELM_OBJECT_SELECT_MODE_MAX)
     return;

   if (it->select_mode != mode)
     it->select_mode = mode;

   if (it->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY)
     elm_gengrid_item_update(EO_OBJ(it));
}

EOLIAN static Elm_Object_Select_Mode
_elm_gengrid_item_select_mode_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   ELM_GENGRID_ITEM_CHECK_OR_RETURN(it, ELM_OBJECT_SELECT_MODE_MAX);

   return it->select_mode;
}

EOLIAN Elm_Atspi_State_Set
_elm_gengrid_item_elm_interface_atspi_accessible_state_set_get(Eo *eo_it, Elm_Gen_Item *it EINA_UNUSED)
{
   Elm_Atspi_State_Set ret;
   Eina_Bool sel;

   eo_do_super(eo_it, ELM_GENGRID_ITEM_CLASS, ret = elm_interface_atspi_accessible_state_set_get());

   eo_do(eo_it, sel = elm_obj_gengrid_item_selected_get());

   STATE_TYPE_SET(ret, ELM_ATSPI_STATE_SELECTABLE);

   if (sel)
      STATE_TYPE_SET(ret, ELM_ATSPI_STATE_SELECTED);

   return ret;
}

EOLIAN char*
_elm_gengrid_item_elm_interface_atspi_accessible_name_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   char *ret;
   Eina_Strbuf *buf;

   buf = eina_strbuf_new();

   if (it->itc->func.text_get)
     {
        Eina_List *texts;
        const char *key;

        texts =
           elm_widget_stringlist_get(edje_object_data_get(VIEW(it), "texts"));

        EINA_LIST_FREE(texts, key)
          {
             char *str_markup = it->itc->func.text_get
                ((void *)WIDGET_ITEM_DATA_GET(EO_OBJ(it)), WIDGET(it), key);

             char *str_utf8 = _elm_util_mkup_to_text(str_markup);

             free(str_markup);

             if (str_utf8)
               {
                  if (eina_strbuf_length_get(buf) > 0)
                    eina_strbuf_append(buf, ", ");
                  eina_strbuf_append(buf, str_utf8);
                  free(str_utf8);
               }
          }
     }

   ret = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ret;
}

EOLIAN Eina_List*
_elm_gengrid_item_elm_interface_atspi_accessible_children_get(Eo *eo_it EINA_UNUSED, Elm_Gen_Item *it)
{
   Eina_List *ret = NULL;
   if (VIEW(it))
     {
        Eina_List *parts;
        const char *key;
        parts = elm_widget_stringlist_get(edje_object_data_get(VIEW(it), "contents"));

        EINA_LIST_FREE(parts, key)
          {
             Evas_Object *part;
             part = edje_object_part_swallow_get(VIEW(it), key);
             if (part && eo_isa(part, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN))
               ret = eina_list_append(ret, part);
          }
     }
   return ret;
}

EAPI Elm_Object_Item *
elm_gengrid_nth_item_get(const Evas_Object *obj, unsigned int nth)
{
   Elm_Gen_Item *it = NULL;
   Eina_Accessor *a;
   void *data;

   ELM_GENGRID_CHECK(obj) NULL;
   ELM_GENGRID_DATA_GET(obj, sd);

   if (!sd->items) return NULL;

   a = eina_inlist_accessor_new(sd->items);
   if (!a) return NULL;
   if (eina_accessor_data_get(a, nth, &data))
     it = ELM_GEN_ITEM_FROM_INLIST(data);
   eina_accessor_free(a);
   return EO_OBJ(it);
}

EOLIAN static void
_elm_gengrid_elm_widget_focus_highlight_geometry_get(const Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
   Evas_Coord ox, oy, oh, ow, item_x = 0, item_y = 0, item_w = 0, item_h = 0;

   evas_object_geometry_get(sd->pan_obj, &ox, &oy, &ow, &oh);

   if (sd->focused_item)
     {
        ELM_GENGRID_ITEM_DATA_GET(sd->focused_item, focus_it);
        evas_object_geometry_get(VIEW(focus_it), &item_x, &item_y, &item_w, &item_h);
        elm_widget_focus_highlight_focus_part_geometry_get(VIEW(focus_it), &item_x, &item_y, &item_w, &item_h);
     }

   *x = item_x;
   *y = item_y;
   *w = item_w;
   *h = item_h;

   if (sd->horizontal)
     {
        if (item_x < ox)
          {
             *x = ox;
          }
        else if (item_x > (ox + ow - item_w))
          {
             *x = ox + ow - item_w;
          }
     }
   else
     {
        if (item_y < oy)
          {
             *y = oy;
          }
        else if (item_y > (oy + oh - item_h))
          {
             *y = oy + oh - item_h;
          }
     }
}

EOLIAN static Elm_Object_Item *
_elm_gengrid_elm_widget_focused_item_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->focused_item;
}

EOLIAN static void
_elm_gengrid_elm_widget_item_loop_enabled_set(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd, Eina_Bool enable)
{
   sd->item_loop_enable = !!enable;
}

EOLIAN static Eina_Bool
_elm_gengrid_elm_widget_item_loop_enabled_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->item_loop_enable;
}

EOLIAN static void
_elm_gengrid_wheel_disabled_set(Eo *obj, Elm_Gengrid_Data *sd, Eina_Bool disabled)
{
   disabled = !!disabled;
   if (sd->wheel_disabled != disabled)
     eo_do(obj,
           elm_interface_scrollable_wheel_disabled_set(disabled));

   sd->wheel_disabled = disabled;
}

EOLIAN static Eina_Bool
_elm_gengrid_wheel_disabled_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   return sd->wheel_disabled;
}

EOLIAN static void
_elm_gengrid_class_constructor(Eo_Class *klass)
{
   if (_elm_config->access_mode)
      _elm_gengrid_smart_focus_next_enable = EINA_TRUE;

   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EOLIAN const Elm_Atspi_Action *
_elm_gengrid_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "move,prior", "move", "prior", _key_action_move},
          { "move,next", "move", "next", _key_action_move},
          { "move,left", "move", "left", _key_action_move},
          { "move,left,multi", "move", "left_multi", _key_action_move},
          { "move,right", "move", "right", _key_action_move},
          { "move,right,multi", "move", "right_multi", _key_action_move},
          { "move,up", "move", "up", _key_action_move},
          { "move,up,multi", "move", "up_multi", _key_action_move},
          { "move,down", "move", "down", _key_action_move},
          { "move,down,multi", "move", "down_multi", _key_action_move},
          { "move,first", "move", "first", _key_action_move},
          { "move,last", "move", "last", _key_action_move},
          { "select", "select", NULL, _key_action_select},
          { "select,multi", "select", "multi", _key_action_select},
          { "escape", "escape", NULL, _key_action_escape},
          { NULL, NULL, NULL, NULL }
   };
   return &atspi_actions[0];
}

EOLIAN Eina_List*
_elm_gengrid_elm_interface_atspi_accessible_children_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *sd)
{
   Eina_List *ret = NULL;
   Elm_Gen_Item *it;

   EINA_INLIST_FOREACH(sd->items, it)
      ret = eina_list_append(ret, EO_OBJ(it));

   return ret;
}

EOLIAN Elm_Atspi_State_Set
_elm_gengrid_elm_interface_atspi_accessible_state_set_get(Eo *obj, Elm_Gengrid_Data *sd EINA_UNUSED)
{
   Elm_Atspi_State_Set ret;

   eo_do_super(obj, ELM_GENGRID_CLASS, ret = elm_interface_atspi_accessible_state_set_get());

   STATE_TYPE_SET(ret, ELM_ATSPI_STATE_MANAGES_DESCENDANTS);

   return ret;
}

EOLIAN int
_elm_gengrid_elm_interface_atspi_selection_selected_children_count_get(Eo *objm EINA_UNUSED, Elm_Gengrid_Data *pd)
{
   return eina_list_count(pd->selected);
}

EOLIAN Eo*
_elm_gengrid_elm_interface_atspi_selection_selected_child_get(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *pd, int child_idx)
{
   return eina_list_nth(pd->selected, child_idx);
}

EOLIAN Eina_Bool
_elm_gengrid_elm_interface_atspi_selection_child_select(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *pd, int child_index)
{
   Elm_Gen_Item *item;
   if (pd->select_mode != ELM_OBJECT_SELECT_MODE_NONE)
     {
        EINA_INLIST_FOREACH(pd->items, item)
          {
             if (child_index-- == 0)
               {
                  elm_gengrid_item_selected_set(EO_OBJ(item), EINA_TRUE);
                  return EINA_TRUE;
               }
          }
     }
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_elm_gengrid_elm_interface_atspi_selection_selected_child_deselect(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *pd, int child_index)
{
   Eo *item;
   Eina_List *l;

   EINA_LIST_FOREACH(pd->selected, l, item)
     {
        if (child_index-- == 0)
          {
             elm_gengrid_item_selected_set(item, EINA_FALSE);
             return EINA_TRUE;
          }
     }
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_elm_gengrid_elm_interface_atspi_selection_is_child_selected(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *pd, int child_index)
{
   Elm_Gen_Item *item;

   EINA_INLIST_FOREACH(pd->items, item)
     {
        if (child_index-- == 0)
          {
             return elm_gengrid_item_selected_get(EO_OBJ(item));
          }
     }
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_elm_gengrid_elm_interface_atspi_selection_all_children_select(Eo *obj, Elm_Gengrid_Data *pd)
{
   Elm_Gen_Item *item;

   if (!elm_gengrid_multi_select_get(obj))
     return EINA_FALSE;

   EINA_INLIST_FOREACH(pd->items, item)
      elm_gengrid_item_selected_set(EO_OBJ(item), EINA_TRUE);

   return EINA_TRUE;
}

EOLIAN Eina_Bool
_elm_gengrid_elm_interface_atspi_selection_clear(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *pd)
{
   return _all_items_deselect(pd);
}

EOLIAN Eina_Bool
_elm_gengrid_elm_interface_atspi_selection_child_deselect(Eo *obj EINA_UNUSED, Elm_Gengrid_Data *pd, int child_index)
{
   Elm_Gen_Item *item;
   if (pd->select_mode != ELM_OBJECT_SELECT_MODE_NONE)
     {
        EINA_INLIST_FOREACH(pd->items, item)
          {
             if (child_index-- == 0)
               {
                  elm_gengrid_item_selected_set(EO_OBJ(item), EINA_FALSE);
                  return EINA_TRUE;
               }
          }
     }
   return EINA_FALSE;
}

#include "elm_gengrid.eo.c"
#include "elm_gengrid_item.eo.c"
