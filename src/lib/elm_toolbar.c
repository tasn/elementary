#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_SELECTION_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED
#define ELM_WIDGET_ITEM_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "elm_widget_toolbar.h"
#include "els_box.h"

#define MY_CLASS ELM_TOOLBAR_CLASS

#define MY_CLASS_NAME "Elm_Toolbar"
#define MY_CLASS_NAME_LEGACY "elm_toolbar"

#define ELM_TOOLBAR_ITEM_FROM_INLIST(item) \
  ((item) ? EINA_INLIST_CONTAINER_GET(item, Elm_Toolbar_Item_Data) : NULL)

static const char SIG_SCROLL[] = "scroll";
static const char SIG_SCROLL_ANIM_START[] = "scroll,anim,start";
static const char SIG_SCROLL_ANIM_STOP[] = "scroll,anim,stop";
static const char SIG_SCROLL_DRAG_START[] = "scroll,drag,start";
static const char SIG_SCROLL_DRAG_STOP[] = "scroll,drag,stop";
static const char SIG_CLICKED[] = "clicked";
static const char SIG_SELECTED[] = "selected";
static const char SIG_UNSELECTED[] = "unselected";
static const char SIG_LONGPRESSED[] = "longpressed";
static const char SIG_CLICKED_DOUBLE[] = "clicked,double";
static const char SIG_ITEM_FOCUSED[] = "item,focused";
static const char SIG_ITEM_UNFOCUSED[] = "item,unfocused";
static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_SCROLL, ""},
   {SIG_SCROLL_ANIM_START, ""},
   {SIG_SCROLL_ANIM_STOP, ""},
   {SIG_SCROLL_DRAG_START, ""},
   {SIG_SCROLL_DRAG_STOP, ""},
   {SIG_CLICKED, ""},
   {SIG_SELECTED, ""},
   {SIG_UNSELECTED, ""},
   {SIG_LONGPRESSED, ""},
   {SIG_CLICKED_DOUBLE, ""},
   {SIG_ITEM_FOCUSED, ""},
   {SIG_ITEM_UNFOCUSED, ""},
   {SIG_WIDGET_LANG_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_ACCESS_CHANGED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_FOCUSED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_UNFOCUSED, ""}, /**< handled by elm_widget */
   {NULL, NULL}
};

static Eina_Bool _key_action_select(Evas_Object *obj, const char *params);
static Eina_Bool _key_action_move(Evas_Object *obj, const char *params);
static void _sizing_eval(Evas_Object *obj);

static const Elm_Action key_actions[] = {
   {"select", _key_action_select},
   {"move", _key_action_move},
   {NULL, NULL}
};

static void _item_select(Elm_Toolbar_Item_Data *it);

static int
_toolbar_item_prio_compare_cb(const void *i1,
                              const void *i2)
{
   const Elm_Toolbar_Item_Data *eti1 = i1;
   const Elm_Toolbar_Item_Data *eti2 = i2;

   if (!eti2) return 1;
   if (!eti1) return -1;

   if (eti2->prio.priority == eti1->prio.priority)
     return -1;

   return eti2->prio.priority - eti1->prio.priority;
}

static void
_items_visibility_fix(Elm_Toolbar_Data *sd,
                      Evas_Coord *iw,
                      Evas_Coord vw,
                      Eina_Bool *more)
{
   Elm_Toolbar_Item_Data *it, *prev;
   Evas_Coord ciw = 0, cih = 0;
   Eina_List *sorted = NULL;
   int count = 0, i = 0;

   *more = EINA_FALSE;

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->separator)
          {
             prev = ELM_TOOLBAR_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->prev);
             if (prev) it->prio.priority = prev->prio.priority;
          }
     }

   EINA_INLIST_FOREACH(sd->items, it)
     {
        sorted = eina_list_sorted_insert
            (sorted, _toolbar_item_prio_compare_cb, it);
     }

   if (sd->more_item)
     {
        evas_object_geometry_get(sd->VIEW(more_item), NULL, NULL, &ciw, &cih);
        if (sd->vertical) *iw += cih;
        else *iw += ciw;
     }

   EINA_LIST_FREE(sorted, it)
     {
        if (it->prio.priority > sd->standard_priority)
          {
             evas_object_geometry_get(VIEW(it), NULL, NULL, &ciw, &cih);
             if (sd->vertical) *iw += cih;
             else *iw += ciw;
             it->prio.visible = (*iw <= vw);
             it->in_box = sd->bx;
             if (!it->separator) count++;
          }
        else
          {
             it->prio.visible = EINA_FALSE;
             if (!it->separator) i++;
             if (i <= (count + 1))
               it->in_box = sd->bx_more;
             else
               it->in_box = sd->bx_more2;
             *more = EINA_TRUE;
          }
     }
}

static void
_item_menu_destroy(Elm_Toolbar_Item_Data *item)
{
   ELM_SAFE_FREE(item->o_menu, evas_object_del);
   item->menu = EINA_FALSE;
}

static void
_item_unselect(Elm_Toolbar_Item_Data *item)
{
   if ((!item) || (!item->selected)) return;

   ELM_TOOLBAR_DATA_GET(WIDGET(item), sd);

   item->selected = EINA_FALSE;
   sd->selected_item = NULL;
   elm_layout_signal_emit(VIEW(item), "elm,state,unselected", "elm");
   if (item->icon)
     elm_widget_signal_emit(item->icon, "elm,state,unselected", "elm");
   eo_do(WIDGET(item), eo_event_callback_call(EVAS_SELECTABLE_INTERFACE_EVENT_UNSELECTED, EO_OBJ(item)));
}

static void
_menu_hide(void *data,
           Evas *e EINA_UNUSED,
           Evas_Object *obj EINA_UNUSED,
           void *event_info EINA_UNUSED)
{
   Elm_Object_Item *eo_selected;
   Elm_Toolbar_Item_Data *it = data;

   eo_selected = elm_toolbar_selected_item_get(WIDGET(it));
   ELM_TOOLBAR_ITEM_DATA_GET(eo_selected, selected);
   _item_unselect(selected);
}

static void
_menu_del(void *data,
          Evas *e EINA_UNUSED,
          Evas_Object *obj,
          void *event_info EINA_UNUSED)
{
   // avoid hide being emitted during object deletion
   evas_object_event_callback_del_full
     (obj, EVAS_CALLBACK_HIDE, _menu_hide, data);
}

static void
_item_menu_create(Elm_Toolbar_Data *sd,
                  Elm_Toolbar_Item_Data *item)
{
   item->o_menu = elm_menu_add(elm_widget_parent_get(WIDGET(item)));
   item->menu = EINA_TRUE;

   if (sd->menu_parent)
     elm_menu_parent_set(item->o_menu, sd->menu_parent);

   evas_object_event_callback_add
     (item->o_menu, EVAS_CALLBACK_HIDE, _menu_hide, item);
   evas_object_event_callback_add
     (item->o_menu, EVAS_CALLBACK_DEL, _menu_del, item);
}

static void
_elm_toolbar_item_menu_cb(void *data,
                          Evas_Object *obj EINA_UNUSED,
                          void *event_info EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it = data;

   if (it->func) it->func((void *)(WIDGET_ITEM_DATA_GET(EO_OBJ(it))), WIDGET(it), EO_OBJ(it));
}

static void
_item_show(Elm_Toolbar_Item_Data *it)
{
   Evas_Coord x, y, w, h, bx, by;

   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   evas_object_geometry_get(sd->bx, &bx, &by, NULL, NULL);
   evas_object_geometry_get(VIEW(it), &x, &y, &w, &h);
   eo_do(WIDGET(it), elm_interface_scrollable_content_region_show
         (x - bx, y - by, w, h));
}

static void
_item_mirrored_set(Evas_Object *obj EINA_UNUSED,
                   Elm_Toolbar_Item_Data *it,
                   Eina_Bool mirrored)
{
   elm_widget_mirrored_set(VIEW(it), mirrored);
   if (it->o_menu) elm_widget_mirrored_set(it->o_menu, mirrored);
}

static void
_mirrored_set(Evas_Object *obj,
              Eina_Bool mirrored)
{
   Elm_Toolbar_Item_Data *it;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   EINA_INLIST_FOREACH(sd->items, it)
     _item_mirrored_set(obj, it, mirrored);
   if (sd->more_item)
     _item_mirrored_set(obj, sd->more_item, mirrored);
}

static void
_items_size_fit(Evas_Object *obj, Evas_Coord *bl, Evas_Coord view)
{
   Elm_Toolbar_Item_Data *it, *prev;
   Eina_Bool full = EINA_FALSE, more = EINA_FALSE;
   Evas_Coord min, mw, mh;
   int sumf = 0, sumb = 0, prev_min = 0;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   EINA_INLIST_FOREACH(sd->items, it)
     {
        mw = mh = -1;
        if (it->in_box && it->in_box == sd->bx)
          {
             if (!it->separator && !it->object)
               elm_coords_finger_size_adjust(1, &mw, 1, &mh);
             edje_object_size_min_restricted_calc(elm_layout_edje_get(VIEW(it)), &mw, &mh, mw, mh);
          }
        else if (!more)
          {
             more = EINA_TRUE;
             elm_coords_finger_size_adjust(1, &mw, 1, &mh);
             edje_object_size_min_restricted_calc(elm_layout_edje_get(sd->VIEW(more_item)), &mw, &mh, mw, mh);
          }

        if (mw != -1 || mh != -1)
          {
             if (sd->vertical) min = mh;
             else min = mw;

             if ((!full) && ((sumf + min) > view))
               {
                  prev = ELM_TOOLBAR_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->prev);
                  if (prev && prev->separator)
                    {
                       sumf -= prev_min;
                       sumb += prev_min;
                    }
                  full = EINA_TRUE;
               }

             if (!full) sumf += min;
             else sumb += min;
             prev_min = min;
          }
     }
   if (sumf != 0) *bl = (Evas_Coord)(((sumf + sumb) * view) / sumf);
}

static Eina_Bool
_elm_toolbar_item_coordinates_calc(Elm_Toolbar_Item_Data *item,
                                   Elm_Toolbar_Item_Scrollto_Type type,
                                   Evas_Coord *x,
                                   Evas_Coord *y,
                                   Evas_Coord *w,
                                   Evas_Coord *h)
{
   Evas_Coord ix, iy, iw, ih, bx, by, vw = 0, vh = 0;

   ELM_TOOLBAR_DATA_GET(WIDGET(item), sd);

   eo_do(WIDGET(item),
         elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &vw, &vh));
   evas_object_geometry_get(sd->bx, &bx, &by, NULL, NULL);
   evas_object_geometry_get(VIEW(item), &ix, &iy, &iw, &ih);

   switch (type)
     {
      case ELM_TOOLBAR_ITEM_SCROLLTO_IN:
         *x = ix - bx;
         *y = iy - by;
         *w = iw;
         *h = ih;
         break;

      case ELM_TOOLBAR_ITEM_SCROLLTO_FIRST:
         *x = ix - bx;
         *y = iy - by;
         *w = vw;
         *h = vh;
         break;

      case ELM_TOOLBAR_ITEM_SCROLLTO_MIDDLE:
         *x = ix - bx + (iw / 2) - (vw / 2);
         *y = iy - by + (ih / 2) - (vh / 2);
         *w = vw;
         *h = vh;
         break;

      case ELM_TOOLBAR_ITEM_SCROLLTO_LAST:
         *x = ix - bx + iw - vw;
         *y = iy - by + ih - vh;
         *w = vw;
         *h = vh;
         break;

      default:
         return EINA_FALSE;
     }

   return EINA_TRUE;
}

static void
_resize_job(void *data)
{
   Evas_Object *obj = (Evas_Object *)data;
   Evas_Coord mw, mh, vw = 0, vh = 0, w = 0, h = 0;
   Elm_Toolbar_Item_Data *it;
   Eina_List *list;
   Eina_Bool more;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   sd->resize_job = NULL;
   eo_do(obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &vw, &vh));
   evas_object_size_hint_min_get(sd->bx, &mw, &mh);
   evas_object_geometry_get(sd->bx, NULL, NULL, &w, &h);

   if (sd->shrink_mode == ELM_TOOLBAR_SHRINK_MENU)
     {
        Evas_Coord iw = 0, ih = 0, more_w = 0, more_h = 0;

        if (sd->vertical)
          {
             h = vh;
             _items_visibility_fix(sd, &ih, vh, &more);
          }
        else
          {
             w = vw;
             _items_visibility_fix(sd, &iw, vw, &more);
          }
        evas_object_geometry_get
          (sd->VIEW(more_item), NULL, NULL, &more_w, &more_h);

        if (sd->vertical)
          {
             if ((ih - more_h) <= vh) ih -= more_h;
          }
        else
          {
             if ((iw - more_w) <= vw) iw -= more_w;
          }

        /* All items are removed from the box object, since removing
         * individual items won't trigger a resize. Items are be
         * readded below. */
        evas_object_box_remove_all(sd->bx, EINA_FALSE);
        if (((sd->vertical) && (ih > vh)) ||
            ((!sd->vertical) && (iw > vw)) || more)
          {
             Evas_Object *menu;

             _item_menu_destroy(sd->more_item);
             _item_menu_create(sd, sd->more_item);
             menu =
               elm_toolbar_item_menu_get(EO_OBJ(sd->more_item));
             EINA_INLIST_FOREACH(sd->items, it)
               {
                  if (!it->prio.visible)
                    {
                       if (it->separator)
                         elm_menu_item_separator_add(menu, NULL);
                       else
                         {
                            Elm_Object_Item *menu_it;
                            Eina_Bool tmp;

                            menu_it = elm_menu_item_add
                                (menu, NULL, it->icon_str, it->label,
                                _elm_toolbar_item_menu_cb, it);
                            Eina_Bool disabled = eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get());
                            eo_do(menu_it, elm_wdg_item_disabled_set(disabled));
                            if (it->o_menu)
                              elm_menu_clone(it->o_menu, menu, menu_it);
                         }
                       evas_object_hide(VIEW(it));
                    }
                  else
                    {
                       evas_object_box_append(sd->bx, VIEW(it));
                       evas_object_show(VIEW(it));
                    }
               }
             evas_object_box_append(sd->bx, sd->VIEW(more_item));
             evas_object_show(sd->VIEW(more_item));
          }
        else
          {
             /* All items are visible, show them all (except for the
              * "More" button, of course). */
             EINA_INLIST_FOREACH(sd->items, it)
               {
                  evas_object_show(VIEW(it));
                  evas_object_box_append(sd->bx, VIEW(it));
               }
             evas_object_hide(sd->VIEW(more_item));
          }
     }
   else if (sd->shrink_mode == ELM_TOOLBAR_SHRINK_HIDE)
     {
        Evas_Coord iw = 0, ih = 0;

        if (sd->vertical)
          {
             h = vh;
             _items_visibility_fix(sd, &ih, vh, &more);
          }
        else
          {
             w = vw;
             _items_visibility_fix(sd, &iw, vw, &more);
          }
        evas_object_box_remove_all(sd->bx, EINA_FALSE);
        if (((sd->vertical) && (ih > vh)) ||
            ((!sd->vertical) && (iw > vw)) || more)
          {
             EINA_INLIST_FOREACH(sd->items, it)
               {
                  if (!it->prio.visible)
                    evas_object_hide(VIEW(it));
                  else
                    {
                       evas_object_box_append(sd->bx, VIEW(it));
                       evas_object_show(VIEW(it));
                    }
               }
          }
        else
          {
             /* All items are visible, show them all */
             EINA_INLIST_FOREACH(sd->items, it)
               {
                  evas_object_show(VIEW(it));
                  evas_object_box_append(sd->bx, VIEW(it));
               }
          }
     }
   else if (sd->shrink_mode == ELM_TOOLBAR_SHRINK_EXPAND)
     {
        Evas_Coord iw = 0, ih = 0;

        if (sd->vertical)
          h = (vh >= mh) ? vh : mh;
        else
          w = (vw >= mw) ? vw : mw;

        if (sd->vertical)
          _items_visibility_fix(sd, &ih, vh, &more);
        else
          _items_visibility_fix(sd, &iw, vw, &more);

        evas_object_box_remove_all(sd->bx, EINA_FALSE);
        evas_object_box_remove_all(sd->bx_more, EINA_FALSE);
        evas_object_box_remove_all(sd->bx_more2, EINA_FALSE);

        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (it->in_box)
               {
                  evas_object_box_append(it->in_box, VIEW(it));
                  evas_object_show(VIEW(it));
               }
          }
        if (more)
          {
             evas_object_box_append(sd->bx, sd->VIEW(more_item));
             evas_object_show(sd->VIEW(more_item));
          }
        else
          evas_object_hide(sd->VIEW(more_item));

        if (sd->vertical)
          {
             if (h > vh) _items_size_fit(obj, &h, vh);
             if (sd->item_count - sd->separator_count > 0)
               eo_do(obj, elm_interface_scrollable_paging_set
                     (0.0, 0.0, 0, (h / (sd->item_count - sd->separator_count))));
          }
        else
          {
             if (w > vw) _items_size_fit(obj, &w, vw);
             if (sd->item_count - sd->separator_count > 0)
               eo_do(obj, elm_interface_scrollable_paging_set
                     (0.0, 0.0, (w / (sd->item_count - sd->separator_count)), 0));
          }
     }
   else
     {
        if (sd->vertical)
          h = (vh >= mh) ? vh : mh;
        else
          w = (vw >= mw) ? vw : mw;
        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (it->selected)
               _item_show(it);
             evas_object_show(VIEW(it));
          }
     }

   if (sd->transverse_expanded)
     {
        if (sd->vertical)
          w = vw;
        else
          h = vh;
     }

   evas_object_resize(sd->bx, w, h);

// Remove the first or last separator since it is not necessary
   list = evas_object_box_children_get(sd->bx_more);
   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->separator &&
            ((VIEW(it) == eina_list_data_get(list)) ||
             (VIEW(it) == eina_list_nth(list, eina_list_count(list) - 1))))
          {
             evas_object_box_remove(sd->bx_more, VIEW(it));
             evas_object_move(VIEW(it), -9999, -9999);
             evas_object_hide(VIEW(it));
          }
     }
   eina_list_free(list);
   list = evas_object_box_children_get(sd->bx_more2);
   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->separator &&
            ((VIEW(it) == eina_list_data_get(list)) ||
             (VIEW(it) == eina_list_nth(list, eina_list_count(list) - 1))))
          {
             evas_object_box_remove(sd->bx_more2, VIEW(it));
             evas_object_move(VIEW(it), -9999, -9999);
             evas_object_hide(VIEW(it));
          }
     }
   eina_list_free(list);

   _mirrored_set(obj, elm_widget_mirrored_get(obj));
}

static void
_elm_toolbar_item_focused(Elm_Object_Item *eo_it)
{
   ELM_TOOLBAR_ITEM_DATA_GET(eo_it, it);
   Evas_Object *obj = WIDGET(it);
   ELM_TOOLBAR_DATA_GET(obj, sd);
   const char *focus_raise;

   if ((!sd) || (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY) ||
       (eo_it == sd->focused_item))
     return;

   sd->focused_item = eo_it;
   switch (_elm_config->focus_autoscroll_mode)
     {
      case ELM_FOCUS_AUTOSCROLL_MODE_SHOW:
         elm_toolbar_item_show(EO_OBJ(it),
                               ELM_TOOLBAR_ITEM_SCROLLTO_IN);
         break;
      case ELM_FOCUS_AUTOSCROLL_MODE_BRING_IN:
         elm_toolbar_item_bring_in(EO_OBJ(it),
                                   ELM_TOOLBAR_ITEM_SCROLLTO_IN);
         break;
      default:
         break;
     }

   if (elm_widget_focus_highlight_enabled_get(obj))
     {
        elm_layout_signal_emit
           (VIEW(it), "elm,state,focused", "elm");
     }
   elm_layout_signal_emit
      (VIEW(it), "elm,highlight,on", "elm");
   focus_raise = elm_layout_data_get(VIEW(it), "focusraise");
   if ((focus_raise) && (!strcmp(focus_raise, "on")))
     evas_object_raise(VIEW(it));
   eo_do(obj, eo_event_callback_call
     (ELM_TOOLBAR_EVENT_ITEM_FOCUSED, EO_OBJ(it)));
   if (_elm_config->atspi_mode)
     elm_interface_atspi_accessible_state_changed_signal_emit(EO_OBJ(it), ELM_ATSPI_STATE_FOCUSED, EINA_TRUE);
}

static void
_elm_toolbar_item_unfocused(Elm_Object_Item *eo_it)
{
   ELM_TOOLBAR_ITEM_DATA_GET(eo_it, it);
   Evas_Object *obj = WIDGET(it);
   ELM_TOOLBAR_DATA_GET(obj, sd);

   if ((!sd) || !sd->focused_item ||
       (eo_it != sd->focused_item))
     return;
   if (sd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY)
     return;
   if (elm_widget_focus_highlight_enabled_get(obj))
     {
        ELM_TOOLBAR_ITEM_DATA_GET(sd->focused_item, focus_it);
        elm_layout_signal_emit
           (VIEW(focus_it), "elm,state,unfocused", "elm");
     }
   elm_layout_signal_emit
      (VIEW(it), "elm,highlight,off", "elm");
   sd->focused_item = NULL;
   eo_do(obj, eo_event_callback_call
     (ELM_TOOLBAR_EVENT_ITEM_UNFOCUSED, eo_it));
   if (_elm_config->atspi_mode)
     elm_interface_atspi_accessible_state_changed_signal_emit(eo_it, ELM_ATSPI_STATE_FOCUSED, EINA_TRUE);
}

/*
 * This function searches the nearest visible item based on the given item.
 * If the given item is in the toolbar viewport, this returns the given item.
 * Or this searches other items and checks the nearest fully visible item
 * according to the given item's position.
 */
static Elm_Object_Item *
_elm_toolbar_nearest_visible_item_get(Evas_Object *obj, Elm_Object_Item *eo_it)
{
   Evas_Coord vx = 0, vy = 0, vw = 0, vh = 0; // toolbar viewport geometry
   Evas_Coord ix = 0, iy = 0, iw = 0, ih = 0; // given item geometry
   Evas_Coord cx = 0, cy = 0, cw = 0, ch = 0; // candidate item geometry
   Eina_List *item_list = NULL;
   Elm_Toolbar_Item_Data *item = NULL;
   ELM_TOOLBAR_DATA_GET(obj, sd);
   Eina_Bool search_next = EINA_FALSE;
   Evas_Object *it_obj;

   if (!eo_it) return NULL;
   ELM_TOOLBAR_ITEM_DATA_GET(eo_it, it);

   evas_object_geometry_get(obj, &vx, &vy, &vw, &vh);
   evas_object_geometry_get(VIEW(it), &ix, &iy, &iw, &ih);

   if (ELM_RECTS_INCLUDE(vx, vy, vw, vh, ix, iy, iw, ih))
     {
        if (!elm_object_item_disabled_get(eo_it))
          return eo_it;
        else
          search_next = EINA_TRUE;
     }

   item_list = evas_object_box_children_get(sd->bx);

   if ((sd->vertical && (iy < vy)) ||
       (!sd->vertical && (iw < vw)) ||
       search_next)
     {
        while ((item_list = eina_list_next(item_list)))
          {
             it_obj = eina_list_data_get(item_list);
             if (it_obj)
               item = evas_object_data_get(it_obj, "item");
             if (!item)
               break;
             evas_object_geometry_get(VIEW(item), &cx, &cy, &cw, &ch);
             if (ELM_RECTS_INCLUDE(vx, vy, vw, vh, cx, cy, cw, ch) &&
                 !elm_object_item_disabled_get(EO_OBJ(item)))
               {
                  eina_list_free(item_list);
                  return EO_OBJ(item);
               }
          }
     }
   else
     {
        while ((item_list = eina_list_prev(item_list)))
          {
             it_obj = eina_list_data_get(item_list);
             if (it_obj)
               item = evas_object_data_get(it_obj, "item");
             if (!item)
               break;
             evas_object_geometry_get(VIEW(item), &cx, &cy, &cw, &ch);
             if (ELM_RECTS_INCLUDE(vx, vy, vw, vh, cx, cy, cw, ch) &&
                 !elm_object_item_disabled_get(EO_OBJ(item)))
               {
                  eina_list_free(item_list);
                  return EO_OBJ(item);
               }
          }
     }
   eina_list_free(item_list);
   return eo_it;
}

EOLIAN static Eina_Bool
_elm_toolbar_elm_widget_on_focus(Eo *obj, Elm_Toolbar_Data *sd, Elm_Object_Item *item EINA_UNUSED)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);
   Eina_Bool int_ret = EINA_FALSE;
   Elm_Object_Item *eo_it = NULL;

   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_on_focus(NULL));
   if (!int_ret) return EINA_FALSE;
   if (!sd->items) return EINA_FALSE;

   if (elm_widget_focus_get(obj))
     {
        evas_object_focus_set(wd->resize_obj, EINA_TRUE);
        if (sd->mouse_down) return EINA_TRUE;

        if (sd->last_focused_item)
          eo_it = sd->last_focused_item;
        else if (_elm_config->first_item_focus_on_first_focus_in &&
                 elm_widget_focus_highlight_enabled_get(obj))
          {
             eo_it = elm_toolbar_first_item_get(obj);
          }

        if (eo_it)
          {
             eo_it = _elm_toolbar_nearest_visible_item_get(obj, eo_it);
             if (eo_it) _elm_toolbar_item_focused(eo_it);
          }
     }
   else
     {
        sd->last_focused_item = sd->focused_item;
        if (sd->focused_item)
          _elm_toolbar_item_unfocused(sd->focused_item);
        evas_object_focus_set(wd->resize_obj, EINA_FALSE);
     }
   return EINA_TRUE;
}

static Elm_Toolbar_Item_Data *
_focus_next_item_get(Evas_Object *obj, Eina_Bool reverse)
{
   ELM_TOOLBAR_DATA_GET(obj, sd);
   Eina_List *list = NULL, *children_list;
   Elm_Toolbar_Item_Data *it = NULL;
   Evas_Object *it_obj = NULL;

   children_list = evas_object_box_children_get(sd->bx);
   if (reverse)
     children_list = eina_list_reverse(children_list);
   list = children_list;
   if (sd->focused_item)
     {
        ELM_TOOLBAR_ITEM_DATA_GET(sd->focused_item, focus_it);
        list = eina_list_data_find_list(list, VIEW(focus_it));
        if (list) list = eina_list_next(list);
     }
   it_obj = eina_list_data_get(list);
   if (it_obj) it = evas_object_data_get(it_obj, "item");
   else it = NULL;

   while (it &&
          (it->separator ||
           elm_object_item_disabled_get(EO_OBJ(it))))
     {
        if (list) list = eina_list_next(list);
        if (!list)
          {
             it = NULL;
             break;
          }
        it_obj = eina_list_data_get(list);
        if (it_obj) it = evas_object_data_get(it_obj, "item");
        else it = NULL;
     }
   eina_list_free(children_list);

   return it;
}

EOLIAN static void
_elm_toolbar_item_elm_widget_item_focus_set(Eo *eo_it, Elm_Toolbar_Item_Data *it, Eina_Bool focused)
{
   ELM_TOOLBAR_ITEM_CHECK(it);
   Evas_Object *obj = WIDGET(it);
   ELM_TOOLBAR_DATA_GET(obj, sd);

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
               _elm_toolbar_item_unfocused(sd->focused_item);
             _elm_toolbar_item_focused(eo_it);
          }
     }
   else
     {
        if (!elm_widget_focus_get(obj))
          return;
        if (eo_it)
          _elm_toolbar_item_unfocused(eo_it);
     }

   _elm_widget_item_highlight_in_theme(obj, EO_OBJ(it));
   _elm_widget_highlight_in_theme_update(obj);
   _elm_widget_focus_highlight_start(obj);
}

EOLIAN static Eina_Bool
_elm_toolbar_item_elm_widget_item_focus_get(Eo *eo_it, Elm_Toolbar_Item_Data *it)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(it, EINA_FALSE);
   Evas_Object *obj = WIDGET(it);
   ELM_TOOLBAR_CHECK(obj) EINA_FALSE;
   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (eo_it == sd->focused_item)
     return EINA_TRUE;
   return EINA_FALSE;
}

static Eina_Bool
_item_focused_next( Evas_Object *obj,
                   Eina_Bool reverse,
                   Elm_Focus_Direction dir)
{
   ELM_TOOLBAR_DATA_GET(obj, sd);
   Elm_Toolbar_Item_Data *next_focused_item;

   next_focused_item = _focus_next_item_get(obj, reverse);
   if (!next_focused_item)
     return EINA_FALSE;

   if ((!sd->vertical && (dir == ELM_FOCUS_LEFT || dir == ELM_FOCUS_RIGHT))
         || (sd->vertical && (dir == ELM_FOCUS_UP || dir == ELM_FOCUS_DOWN)))
   {
      elm_object_item_focus_set(EO_OBJ(next_focused_item), EINA_TRUE);
      return EINA_TRUE;
   }
   _elm_widget_focus_highlight_start(obj);
   return EINA_FALSE;
}

static Eina_Bool
_key_action_select(Evas_Object *obj, const char *params EINA_UNUSED)
{
   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (sd->focused_item)
     {
        ELM_TOOLBAR_ITEM_DATA_GET(sd->focused_item, focus_it);
        _item_select(focus_it);
     }

   return EINA_TRUE;
}

static Eina_Bool
_key_action_move(Evas_Object *obj, const char *params)
{
   const char *dir = params;

   _elm_widget_focus_auto_show(obj);
   if (!strcmp(dir, "left"))
     {
        if (!_item_focused_next(obj, EINA_TRUE, ELM_FOCUS_LEFT))
          return EINA_FALSE;
     }
   else if (!strcmp(dir, "right"))
     {
        if (!_item_focused_next(obj, EINA_FALSE, ELM_FOCUS_RIGHT))
          return EINA_FALSE;
     }
   else if (!strcmp(dir, "up"))
     {
        if (!_item_focused_next(obj, EINA_TRUE, ELM_FOCUS_UP))
          return EINA_FALSE;
     }
   else if (!strcmp(dir, "down"))
     {
        if (!_item_focused_next(obj, EINA_FALSE, ELM_FOCUS_DOWN))
          return EINA_FALSE;
     }
   else return EINA_FALSE;

   return EINA_TRUE;

}

EOLIAN static Eina_Bool
_elm_toolbar_elm_widget_event(Eo *obj, Elm_Toolbar_Data *sd, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
   (void) src;
   (void) type;
   Evas_Event_Key_Down *ev = event_info;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;
   if (!sd->items) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

static void
_resizing_eval(Evas_Object *obj)
{
   Evas_Coord x, y, h;
   ELM_TOOLBAR_DATA_GET(obj, sd);

   evas_object_geometry_get(obj, &x, &y, NULL, &h);
   evas_object_move(sd->more, x, y + h);
   if (sd->resize_job) ecore_job_del(sd->resize_job);
   // fit and fix the visibility
   sd->resize_job = ecore_job_add(_resize_job, obj);
}

static void
_resizing_eval_item(Elm_Toolbar_Item_Data *it)
{
   Evas_Coord x, y, h;
   Evas_Coord mw = -1, mh = -1;
   Evas_Object *obj = WIDGET(it);
   ELM_TOOLBAR_DATA_GET(obj, sd);

   evas_object_geometry_get(obj, &x, &y, NULL, &h);
   evas_object_move(sd->more, x, y + h);
   //calculate the size of item
   edje_object_size_min_restricted_calc(elm_layout_edje_get(VIEW(it)), &mw, &mh, mw, mh);
   if (!it->separator && !it->object)
     elm_coords_finger_size_adjust(1, &mw, 1, &mh);
   evas_object_size_hint_min_set(VIEW(it), mw, mh);
   evas_object_size_hint_max_set(VIEW(it), -1, -1);
}

static void
_resize_cb(void *data,
           Evas *e EINA_UNUSED,
           Evas_Object *obj EINA_UNUSED,
           void *event_info EINA_UNUSED)
{
   _resizing_eval(data);
}

EOLIAN static void
_elm_toolbar_item_elm_widget_item_disable(Eo *eo_toolbar, Elm_Toolbar_Item_Data *toolbar_it)
{
   Eina_Bool tmp;
   const char* emission;

   if (eo_do_ret(eo_toolbar, tmp, elm_wdg_item_disabled_get()))
     emission = "elm,state,disabled";
   else
     emission = "elm,state,enabled";

   elm_layout_signal_emit(VIEW(toolbar_it), emission, "elm");
   if (toolbar_it->icon)
     elm_widget_signal_emit(toolbar_it->icon, emission, "elm");

   _resize_cb(WIDGET(toolbar_it), NULL, NULL, NULL);
}

static Eina_Bool
_item_icon_set(Evas_Object *icon_obj,
               const char *type,
               const char *icon)
{
   char icon_str[512];

   if ((!type) || (!*type)) goto end;
   if ((!icon) || (!*icon)) return EINA_FALSE;
   if ((snprintf(icon_str, sizeof(icon_str), "%s%s", type, icon) > 0)
       && (elm_icon_standard_set(icon_obj, icon_str)))
     return EINA_TRUE;
end:
   if (elm_icon_standard_set(icon_obj, icon))
     return EINA_TRUE;

   WRN("couldn't find icon definition for '%s'", icon);
   return EINA_FALSE;
}

static int
_internal_elm_toolbar_icon_size_get(Evas_Object *obj)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, 0);
   const char *icon_size = edje_object_data_get
       (wd->resize_obj, "icon_size");

   if (icon_size)
     return (int)(atoi(icon_size) * elm_widget_scale_get(obj) * elm_config_scale_get()
            / edje_object_base_scale_get(wd->resize_obj));

   return _elm_config->icon_size;
}

static void
_menu_move_resize_cb(void *data,
                     Evas *e EINA_UNUSED,
                     Evas_Object *obj EINA_UNUSED,
                     void *event_info EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it = data;
   Evas_Coord x, y, h;

   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   if (!sd->menu_parent) return;
   evas_object_geometry_get(VIEW(it), &x, &y, NULL, &h);
   elm_menu_move(it->o_menu, x, y + h);
}

static void
_item_select(Elm_Toolbar_Item_Data *it)
{
   Evas_Object *obj;
   Eina_Bool sel;
   Eina_Bool tmp;
   Eina_List *list;

   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   if (eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()) || (it->separator) || (it->object))
     return;
   if ((sd->shrink_mode == ELM_TOOLBAR_SHRINK_EXPAND) && (!sd->more_item))
     return;
   sel = it->selected;

   if ((sd->select_mode != ELM_OBJECT_SELECT_MODE_NONE) &&
       (sd->select_mode != ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY))
     {
        if (sel)
          {
             if (sd->shrink_mode == ELM_TOOLBAR_SHRINK_EXPAND)
               {
                  if (sd->more_item == it)
                    {
                       elm_layout_signal_emit
                         (sd->more, "elm,state,close", "elm");
                       _item_unselect(it);
                    }
               }
             if (sd->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS)
               _item_unselect(it);
          }
        else
          {
             Elm_Object_Item *eo_it2 = elm_toolbar_selected_item_get(WIDGET(it));
             ELM_TOOLBAR_ITEM_DATA_GET(eo_it2, it2);
             _item_unselect(it2);

             it->selected = EINA_TRUE;
             sd->selected_item = EO_OBJ(it);
             if (sd->shrink_mode == ELM_TOOLBAR_SHRINK_EXPAND)
               {
                  if (sd->more_item == it)
                    {
                       list = evas_object_box_children_get(sd->bx_more2);
                       if (!list)
                         elm_layout_signal_emit
                           (sd->more, "elm,state,open", "elm");
                       else
                         elm_layout_signal_emit
                           (sd->more, "elm,state,open2", "elm");
                       eina_list_free(list);
                    }
                  else
                    {
                       if (it->in_box != sd->bx)
                         {
                            elm_layout_signal_emit
                              (sd->VIEW(more_item), "elm,state,selected",
                              "elm");
                            elm_widget_signal_emit
                              (sd->more_item->icon, "elm,state,selected",
                              "elm");
                         }
                       else
                         {
                            elm_layout_signal_emit
                              (sd->VIEW(more_item), "elm,state,unselected",
                              "elm");
                            elm_widget_signal_emit
                              (sd->more_item->icon, "elm,state,unselected",
                              "elm");
                         }
                       elm_layout_signal_emit
                         (sd->more, "elm,state,close", "elm");
                    }
               }
             elm_layout_signal_emit(VIEW(it), "elm,state,selected", "elm");
             if (it->icon)
               elm_widget_signal_emit(it->icon, "elm,state,selected", "elm");
             _item_show(it);
          }
     }

   obj = WIDGET(it);
   if (it->menu && (!sel))
     {
        evas_object_show(it->o_menu);
        evas_object_event_callback_add
          (VIEW(it), EVAS_CALLBACK_RESIZE, _menu_move_resize_cb, it);
        evas_object_event_callback_add
          (VIEW(it), EVAS_CALLBACK_MOVE, _menu_move_resize_cb, it);

        _menu_move_resize_cb(it, NULL, NULL, NULL);
     }

   if ((!sel) || (sd->select_mode == ELM_OBJECT_SELECT_MODE_ALWAYS))
     {
        if (it->func) it->func((void *)(WIDGET_ITEM_DATA_GET(EO_OBJ(it))), WIDGET(it), EO_OBJ(it));
     }
   eo_do(obj, eo_event_callback_call(EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, EO_OBJ(it)));
   eo_do(obj, eo_event_callback_call(EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, EO_OBJ(it)));
}

static void
_item_del(Elm_Toolbar_Item_Data *it)
{
   Elm_Toolbar_Item_State *it_state;
   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   _item_unselect(it);

   EINA_LIST_FREE(it->states, it_state)
     {
        if (it->icon == it_state->icon)
          it->icon = NULL;
        eina_stringshare_del(it_state->label);
        eina_stringshare_del(it_state->icon_str);
        if (it_state->icon) evas_object_del(it_state->icon);
        free(it_state);
     }

   eina_stringshare_del(it->label);
   if (it->label)
     elm_layout_signal_emit(VIEW(it), "elm,state,text,hidden", "elm");
   eina_stringshare_del(it->icon_str);

   if (it->icon)
     {
        elm_layout_signal_emit(VIEW(it), "elm,state,icon,hidden", "elm");
        evas_object_del(it->icon);
     }

   if (sd->focused_item == EO_OBJ(it))
     sd->focused_item = NULL;
   if (sd->last_focused_item == EO_OBJ(it))
     sd->last_focused_item = NULL;

   evas_object_del(it->object);
   //TODO: See if checking for sd->menu_parent is necessary before
   //deleting menu
   evas_object_del(it->o_menu);
}

static void
_item_theme_hook(Evas_Object *obj,
                 Elm_Toolbar_Item_Data *it,
                 double scale,
                 int icon_size)
{
   Evas_Coord mw = -1, mh = -1;
   Evas_Object *view = VIEW(it);
   const char *style;
   Eina_Bool tmp;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   style = elm_widget_style_get(obj);

   _item_mirrored_set(obj, it, elm_widget_mirrored_get(obj));
   edje_object_scale_set(elm_layout_edje_get(view), scale);

   if (!it->separator && !it->object)
     {
        if (!elm_layout_theme_set(view, "toolbar", "item", style))
          CRI("Failed to set layout!");
        if (it->selected)
          {
             elm_layout_signal_emit(view, "elm,state,selected", "elm");
             if (it->icon)
               elm_widget_signal_emit(it->icon, "elm,state,selected", "elm");
          }
        if (eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()))
          {
             elm_layout_signal_emit(view, "elm,state,disabled", "elm");
             if (it->icon)
               elm_widget_signal_emit(it->icon, "elm,state,disabled", "elm");
          }
        if (it->icon)
          {
             evas_object_size_hint_min_set(it->icon, icon_size, icon_size);
             evas_object_size_hint_max_set(it->icon, icon_size, icon_size);
             elm_layout_content_set(view, "elm.swallow.icon", it->icon);
             elm_layout_signal_emit
               (view, "elm,state,icon,visible", "elm");
          }
        if (it->label)
          {
             elm_layout_text_set(view, "elm.text", it->label);
             elm_layout_signal_emit(view, "elm,state,text,visible", "elm");
          }
     }
   else
     {
        if (!it->object)
          {
             if (!elm_layout_theme_set(view, "toolbar", "separator", style))
               CRI("Failed to set layout!");
          }
        else
          {
             if (!elm_layout_theme_set(view, "toolbar", "object", style))
               CRI("Failed to set layout!");
             elm_layout_content_set(view, "elm.swallow.object", it->object);
          }
     }

   if (sd->vertical)
     elm_layout_signal_emit(view, "elm,orient,vertical", "elm");
   else
     elm_layout_signal_emit(view, "elm,orient,horizontal", "elm");

    edje_object_message_signal_process(elm_layout_edje_get(view));
    if (!it->separator && !it->object)
      elm_coords_finger_size_adjust(1, &mw, 1, &mh);
    if (sd->shrink_mode != ELM_TOOLBAR_SHRINK_EXPAND)
      {
         if (sd->vertical)
           {
              evas_object_size_hint_weight_set(view, EVAS_HINT_EXPAND, -1.0);
              evas_object_size_hint_align_set
                (view, EVAS_HINT_FILL, EVAS_HINT_FILL);
           }
         else
           {
              evas_object_size_hint_weight_set(VIEW(it), -1.0, EVAS_HINT_EXPAND);
              evas_object_size_hint_align_set
                (view, EVAS_HINT_FILL, EVAS_HINT_FILL);
           }
      }
    else
      {
         evas_object_size_hint_weight_set
           (VIEW(it), EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
         evas_object_size_hint_align_set
           (VIEW(it), EVAS_HINT_FILL, EVAS_HINT_FILL);
      }
    _resizing_eval_item(it);
    evas_object_smart_need_recalculate_set(obj, EINA_TRUE);
}

static void
_inform_item_number(Evas_Object *obj)
{
   ELM_TOOLBAR_DATA_GET(obj, sd);
   Elm_Toolbar_Item_Data *it;
   char buf[sizeof("elm,number,item,") + 4];
   static int scount = 0;
   int count = 0;
   Evas_Coord mw, mh;

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (!it->separator) count++;
     }
   if (scount != count)
     {
        scount = count;
        if (snprintf(buf, sizeof(buf), "elm,number,item,%d", count) >= (int)sizeof(buf))
          ERR("Too many items to fit signal buffer (%d)", count);

        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (!it->separator && !it->object)
               {
                  elm_layout_signal_emit(VIEW(it), buf, "elm");
                  edje_object_message_signal_process(elm_layout_edje_get(VIEW(it)));

                  mw = mh = -1;
                  elm_coords_finger_size_adjust(1, &mw, 1, &mh);

                  edje_object_size_min_restricted_calc(elm_layout_edje_get(VIEW(it)), &mw, &mh, mw, mh);
                  evas_object_size_hint_min_set(VIEW(it), mw, mh);
               }
          }
     }
}

static void
_sizing_eval(Evas_Object *obj)
{
   Evas_Coord minw = -1, minh = -1, minw_bx = -1, minh_bx = -1;
   Evas_Coord vw = 0, vh = 0;
   Evas_Coord w, h;

   ELM_TOOLBAR_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   evas_object_smart_need_recalculate_set(sd->bx, EINA_TRUE);
   evas_object_smart_calculate(sd->bx);
   edje_object_size_min_calc(wd->resize_obj, &minw, &minh);
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);

   if (w < minw) w = minw;
   if (h < minh) h = minh;

   evas_object_resize(wd->resize_obj, w, h);

   evas_object_size_hint_min_get(sd->bx, &minw_bx, &minh_bx);
   eo_do(obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &vw, &vh));

   if (sd->shrink_mode == ELM_TOOLBAR_SHRINK_NONE)
     {
        minw = minw_bx + (w - vw);
        minh = minh_bx + (h - vh);
     }
   else if (sd->shrink_mode == ELM_TOOLBAR_SHRINK_EXPAND)
     {
        if (sd->vertical)
          {
             minw = minw_bx + (w - vw);
             if (minh_bx <= vh) minh_bx = vh;
             else _items_size_fit(obj, &minh_bx, vh);
          }
        else
          {
             minh = minh_bx + (h - vh);
             if (minw_bx <= vw) minw_bx = vw;
             else _items_size_fit(obj, &minw_bx, vw);
          }
     }
   else
     {
        if (sd->vertical)
          {
             minw = minw_bx + (w - vw);
             minh = h - vh;
          }
        else
          {
             minw = w - vw;
             minh = minh_bx + (h - vh);
          }
     }

   if (sd->transverse_expanded)
     {
        if (sd->vertical)
          minw_bx = vw;
        else
          minh_bx = vh;
     }

   evas_object_resize(sd->bx, minw_bx, minh_bx);
   evas_object_resize(sd->more, minw_bx, minh_bx);
   evas_object_size_hint_min_set(obj, minw, minh);
   evas_object_size_hint_max_set(obj, -1, -1);

   _inform_item_number(obj);
}

static void
_elm_toolbar_highlight_in_theme(Evas_Object *obj)
{
   const char *fh;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   fh = edje_object_data_get
       (wd->resize_obj, "focus_highlight");
   if ((fh) && (!strcmp(fh, "on")))
     elm_widget_highlight_in_theme_set(obj, EINA_TRUE);
   else
     elm_widget_highlight_in_theme_set(obj, EINA_FALSE);
}

EOLIAN static Eina_Bool
_elm_toolbar_elm_widget_theme_apply(Eo *obj, Elm_Toolbar_Data *sd)
{
   Elm_Toolbar_Item_Data *it;
   double scale = 0;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);

   if (sd->delete_me) return EINA_TRUE;

   Eina_Bool int_ret = EINA_FALSE;
   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_theme_apply());
   if (!int_ret) return EINA_FALSE;

   elm_widget_theme_object_set
     (obj, wd->resize_obj, "toolbar", "base",
     elm_widget_style_get(obj));
   if (sd->vertical)
     edje_object_signal_emit(wd->resize_obj, "elm,orient,vertical", "elm");
   else
     edje_object_signal_emit(wd->resize_obj, "elm,orient,horizontal", "elm");

   if (!elm_layout_theme_set
       (sd->more, "toolbar", "more", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");
   if (sd->vertical)
     elm_object_signal_emit(sd->more, "elm,orient,vertical", "elm");
   else
     elm_object_signal_emit(sd->more, "elm,orient,horizontal", "elm");

   _mirrored_set(obj, elm_widget_mirrored_get(obj));

   sd->theme_icon_size = _internal_elm_toolbar_icon_size_get(obj);
   if (sd->priv_icon_size) sd->icon_size = sd->priv_icon_size;
   else sd->icon_size = sd->theme_icon_size;

   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());
   EINA_INLIST_FOREACH(sd->items, it)
     _item_theme_hook(obj, it, scale, sd->icon_size);

   if (sd->more_item)
     _item_theme_hook(obj, sd->more_item, scale, sd->icon_size);

   _elm_toolbar_highlight_in_theme(obj);
   evas_object_smart_need_recalculate_set(obj, EINA_TRUE);

   return EINA_TRUE;
}

static void
_elm_toolbar_item_label_update(Elm_Toolbar_Item_Data *item)
{
   elm_layout_text_set(VIEW(item), "elm.text", item->label);
   if (item->label)
     elm_layout_signal_emit(VIEW(item), "elm,state,text,visible", "elm");
   else
     elm_layout_signal_emit(VIEW(item), "elm,state,text,hidden", "elm");
}

static void
_elm_toolbar_item_label_set_cb(void *data,
                               Evas_Object *obj,
                               const char *emission,
                               const char *source)
{
   Elm_Toolbar_Item_Data *item = data;

   _elm_toolbar_item_label_update(item);
   elm_layout_signal_callback_del
     (obj, emission, source, _elm_toolbar_item_label_set_cb);
   elm_layout_signal_emit(VIEW(item), "elm,state,label,reset", "elm");
}

static void
_item_label_set(Elm_Toolbar_Item_Data *item,
                const char *label,
                const char *sig)
{
   const char *s;

   if ((label) && (item->label) && (!strcmp(label, item->label))) return;

   eina_stringshare_replace(&item->label, label);
   s = elm_layout_data_get(VIEW(item), "transition_animation_on");
   if ((s) && (atoi(s)))
     {
        elm_layout_text_set
          (VIEW(item), "elm.text_new", item->label);
        elm_layout_signal_emit(VIEW(item), sig, "elm");
        elm_layout_signal_callback_add
          (VIEW(item), "elm,state,label_set,done", "elm",
          _elm_toolbar_item_label_set_cb, item);
     }
   else
     _elm_toolbar_item_label_update(item);

   _resizing_eval_item(item);
}

EOLIAN static void
_elm_toolbar_item_elm_widget_item_part_text_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                    const char *part,
                    const char *label)
{
   char buf[256];

   if ((!part) || (!strcmp(part, "default")) ||
       (!strcmp(part, "elm.text")))
     {
        _item_label_set(item, label, "elm,state,label_set");
     }
   else
     {
        if (label)
          {
             snprintf(buf, sizeof(buf), "elm,state,%s,visible", part);
             elm_layout_signal_emit(VIEW(item), buf, "elm");
          }
        else
          {
             snprintf(buf, sizeof(buf), "elm,state,%s,hidden", part);
             elm_layout_signal_emit(VIEW(item), buf, "elm");
          }
        elm_layout_text_set(VIEW(item), part, label);
     }
}

EOLIAN static const char *
_elm_toolbar_item_elm_widget_item_part_text_get(Eo *eo_it EINA_UNUSED, Elm_Toolbar_Item_Data *it,
                    const char *part)
{
   char buf[256];

   if (!part || !strcmp(part, "default"))
     snprintf(buf, sizeof(buf), "elm.text");
   else
     snprintf(buf, sizeof(buf), "%s", part);

   return elm_layout_text_get(VIEW(it), buf);
}

EOLIAN static void
_elm_toolbar_item_elm_widget_item_part_content_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                       const char *part,
                       Evas_Object *content)
{
   Evas_Object *obj = WIDGET(item);
   double scale;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (part && strcmp(part, "object") && strcmp(part, "elm.swallow.object"))
     {
        eo_do(VIEW(item), elm_obj_container_content_set(part, content));
        return;
     }
   if (item->object == content) return;

   evas_object_del(item->object);

   item->object = content;
   if (item->object)
     elm_widget_sub_object_add(obj, item->object);

   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());
   _item_theme_hook(obj, item, scale, sd->icon_size);
}

EOLIAN static Evas_Object *
_elm_toolbar_item_elm_widget_item_part_content_get(Eo *eo_it EINA_UNUSED, Elm_Toolbar_Item_Data *it,
                       const char *part)
{
   Evas_Object *content;

   if (part && strcmp(part, "object") && strcmp(part, "elm.swallow.object"))
     {
        eo_do(VIEW(it), content = elm_obj_container_content_get(part));
        if (content) return content;
        else return NULL;
     }
   return it->object;
}

EOLIAN static Evas_Object *
_elm_toolbar_item_elm_widget_item_part_content_unset(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                         const char *part)
{
   Evas_Object *obj = WIDGET(item);
   Evas_Object *o;
   double scale;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (part && strcmp(part, "object") && strcmp(part, "elm.swallow.object"))
     {
        eo_do(VIEW(item), o = elm_obj_container_content_unset(part));
        return o;
     }

   elm_layout_content_unset(VIEW(item), "elm.swallow.object");
   elm_widget_sub_object_del(obj, item->object);
   o = item->object;
   item->object = NULL;
   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());
   _item_theme_hook(obj, item, scale, sd->icon_size);

   return o;
}

EOLIAN static Eina_Bool
_elm_toolbar_elm_widget_translate(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   Elm_Toolbar_Item_Data *it;

   EINA_INLIST_FOREACH(sd->items, it)
     eo_do(EO_OBJ(it), elm_wdg_item_translate());

   eo_do_super(obj, MY_CLASS, elm_obj_widget_translate());

   return EINA_TRUE;
}

static void
_item_resize(void *data,
             Evas *e EINA_UNUSED,
             Evas_Object *obj EINA_UNUSED,
             void *event_info EINA_UNUSED)
{
   evas_object_smart_need_recalculate_set(data, EINA_TRUE);
   _resizing_eval(data);
}

static void
_move_cb(void *data,
         Evas *e EINA_UNUSED,
         Evas_Object *obj EINA_UNUSED,
         void *event_info EINA_UNUSED)
{
   Evas_Coord x, y, h;

   ELM_TOOLBAR_DATA_GET(data, sd);
   evas_object_geometry_get(data, &x, &y, NULL, &h);
   evas_object_move(sd->more, x, y + h);
}

static void
_select_filter_cb(Elm_Toolbar_Item_Data *it,
                  Evas_Object *obj EINA_UNUSED,
                  const char *emission,
                  const char *source EINA_UNUSED)
{
   int button;
   char buf[sizeof("elm,action,click,") + 1];

   button = atoi(emission + sizeof("mouse,clicked,") - 1);
   if (button == 1) return;  /* regular left click event */
   snprintf(buf, sizeof(buf), "elm,action,click,%d", button);
   elm_layout_signal_emit(VIEW(it), buf, "elm");
}

static void
_select_cb(void *data,
           Evas_Object *obj EINA_UNUSED,
           const char *emission EINA_UNUSED,
           const char *source EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it = data;

   if ((_elm_config->access_mode == ELM_ACCESS_MODE_OFF) ||
       (_elm_access_2nd_click_timeout(VIEW(it))))
     {
        if (_elm_config->access_mode != ELM_ACCESS_MODE_OFF)
          _elm_access_say(E_("Selected"));
        _item_select(it);
     }
}

static void
_item_move_cb(void *data,
         Evas *e EINA_UNUSED,
         Evas_Object *obj EINA_UNUSED,
         void *event_info EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *item = data;

   item->on_move = EINA_FALSE;

   evas_object_event_callback_del_full
     (VIEW(item), EVAS_CALLBACK_MOVE, _item_move_cb, data);
}

static void
_items_change(Elm_Toolbar_Item_Data *reorder_from, Elm_Toolbar_Item_Data *reorder_to)
{
   Elm_Toolbar_Item_Data *prev = NULL, *next = NULL;
   int tmp;

   if (!reorder_from) return;
   ELM_TOOLBAR_DATA_GET(WIDGET(reorder_from), sd);
   if (reorder_from == reorder_to) return;

   if ((reorder_to) &&
       (!reorder_from->separator) && (!reorder_to->separator))
     {
        prev = ELM_TOOLBAR_ITEM_FROM_INLIST
            (EINA_INLIST_GET(reorder_from)->prev);
        if (prev == reorder_to)
          prev = reorder_from;
        if (!prev)
          next = ELM_TOOLBAR_ITEM_FROM_INLIST
              (EINA_INLIST_GET(reorder_from)->next);
        if (next == reorder_to)
          next = NULL;

        sd->items = eina_inlist_remove
            (sd->items, EINA_INLIST_GET(reorder_from));
        sd->items = eina_inlist_append_relative
            (sd->items, EINA_INLIST_GET(reorder_from),
            EINA_INLIST_GET(reorder_to));

        sd->items = eina_inlist_remove
            (sd->items, EINA_INLIST_GET(reorder_to));
        if (prev)
          sd->items = eina_inlist_append_relative
              (sd->items, EINA_INLIST_GET(reorder_to),
              EINA_INLIST_GET(prev));
        else if (next)
          sd->items = eina_inlist_prepend_relative
              (sd->items, EINA_INLIST_GET(reorder_to),
              EINA_INLIST_GET(next));
        else
          sd->items = eina_inlist_prepend
             (sd->items, EINA_INLIST_GET(reorder_to));

        evas_object_box_remove(sd->bx, VIEW(reorder_from));
        evas_object_box_insert_after(sd->bx, VIEW(reorder_from),
                                     VIEW(reorder_to));
        evas_object_box_remove(sd->bx, VIEW(reorder_to));
        if (prev)
          evas_object_box_insert_after(sd->bx, VIEW(reorder_to),
                                       VIEW(prev));
        else if (next)
          evas_object_box_insert_before(sd->bx, VIEW(reorder_to),
                                        VIEW(next));
        else
          evas_object_box_prepend(sd->bx, VIEW(reorder_to));

        tmp = reorder_from->prio.priority;
        reorder_from->prio.priority = reorder_to->prio.priority;
        reorder_to->prio.priority = tmp;

        reorder_from->on_move = EINA_TRUE;
        reorder_to->on_move = EINA_TRUE;

        evas_object_event_callback_add
           (VIEW(reorder_from), EVAS_CALLBACK_MOVE,
           _item_move_cb, reorder_from);
        evas_object_event_callback_add
           (VIEW(reorder_to), EVAS_CALLBACK_MOVE,
           _item_move_cb, reorder_to);
     }

   _resizing_eval(WIDGET(reorder_from));
}

static void
_transit_del_cb(void *data, Elm_Transit *transit EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it, *item = data;
   ELM_TOOLBAR_DATA_GET(WIDGET(item), sd);

   if (item->reorder_to)
     {
        if (item->reorder_to == sd->reorder_empty)
          sd->reorder_empty = item;
        else if (item == sd->reorder_empty)
          sd->reorder_empty = item->reorder_to;

        _items_change(item->reorder_to, item);

        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (it != item)
               {
                  if (it->reorder_to == item)
                    it->reorder_to = item->reorder_to;
                  else if (it->reorder_to == item->reorder_to)
                    it->reorder_to = item;
               }
          }
     }
   if (item->proxy)
     {
        evas_object_image_source_visible_set(elm_image_object_get(item->proxy), EINA_TRUE);
        ELM_SAFE_FREE(item->proxy, evas_object_del);
     }
   item->trans = NULL;

   if (item->reorder_to)
     {
        EINA_INLIST_FOREACH(sd->items, it)
           if (it->trans) break;

        if (!it) sd->reorder_empty = sd->reorder_item;
     }
   item->reorder_to = NULL;
}

static void
_item_transition_start
(Elm_Toolbar_Item_Data *it, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
   Evas_Coord tx, ty;
   Evas_Object *obj = WIDGET(it), *img = NULL;
   ELM_TOOLBAR_DATA_GET(obj, sd);

   it->proxy = elm_image_add(obj);
   img = elm_image_object_get(it->proxy);
   elm_image_aspect_fixed_set(it->proxy, EINA_FALSE);
   evas_object_image_source_set(img, VIEW(it));
   evas_object_image_source_visible_set(img, EINA_FALSE);
   evas_object_image_source_clip_set(img, EINA_FALSE);

   it->trans = elm_transit_add();
   elm_transit_object_add(it->trans, it->proxy);
   evas_object_geometry_get(VIEW(sd->reorder_empty), &tx, &ty, NULL, NULL);
   evas_object_move(it->proxy, x, y);
   evas_object_resize(it->proxy, w, h);
   evas_object_show(it->proxy);

   elm_transit_effect_translation_add(it->trans, 0, 0, tx - x, 0);
   elm_transit_duration_set(it->trans, 0.2);
   elm_transit_del_cb_set(it->trans, _transit_del_cb, it);
   elm_transit_go(it->trans);

   it->reorder_to = sd->reorder_empty;
}

static void
_animate_missed_items(Elm_Toolbar_Item_Data *prev, Elm_Toolbar_Item_Data *next)
{
   ELM_TOOLBAR_DATA_GET(WIDGET(prev), sd);
   Elm_Toolbar_Item_Data *it, *it2;
   Eina_List *list, *l;
   Evas_Object *o;
   Eina_Bool reverse = EINA_FALSE;
   Evas_Coord fx, fy, fw, fh;

   list = evas_object_box_children_get(sd->bx);

   EINA_LIST_FOREACH(list, l, o)
     {
        if (o == VIEW(prev))
          break;
        else if (o == VIEW(next))
          reverse = EINA_TRUE;
     }

   if (!reverse)
     l = eina_list_next(l);
   else
     l = eina_list_prev(l);

   while (VIEW(next) != eina_list_data_get(l))
     {
        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (VIEW(it) == eina_list_data_get(l))
               {
                  if (!it->trans && it != sd->reorder_item)
                    {
                       evas_object_geometry_get(VIEW(sd->reorder_empty), &fx, &fy, &fw, &fh);
                       _item_transition_start(it, fx, fy, fw, fh);
                       sd->reorder_empty = it;
                    }
                  EINA_INLIST_FOREACH(sd->items, it2)
                    {
                       if (it == it2->reorder_to) break;
                    }
                  if (it2)
                    {
                       it2->reorder_to = NULL;
                       evas_object_geometry_get(it2->proxy, &fx, &fy, &fw, &fh);
                       if (it2->trans) elm_transit_del(it2->trans);
                       _item_transition_start(it2, fx, fy, fw, fh);
                       sd->reorder_empty = it;
                    }
               }
          }
        if (!reverse)
          l = eina_list_next(l);
        else
          l = eina_list_prev(l);
     }
   eina_list_free(list);
}

static void
_mouse_move_reorder(Elm_Toolbar_Item_Data *item,
                    Evas *evas EINA_UNUSED,
                    Evas_Object *obj EINA_UNUSED,
                    Evas_Event_Mouse_Move *ev)
{
   Evas_Coord x, y, w, h;
   Evas_Coord fx, fy, fw, fh;
   Elm_Toolbar_Item_Data *it, *it2;

   ELM_TOOLBAR_DATA_GET(WIDGET(item), sd);

   evas_object_geometry_get(VIEW(item), &x, &y, &w, &h);
   if (sd->vertical)
     evas_object_move(item->proxy, x, ev->cur.canvas.y - (h / 2));
   else
     evas_object_move(item->proxy, ev->cur.canvas.x - (w / 2), y);
   evas_object_show(item->proxy);

   if (sd->reorder_empty->on_move) return;

   evas_object_geometry_get(sd->VIEW(reorder_empty), &x, &y, &w, &h);
   if (ev->cur.canvas.x < x || ev->cur.canvas.x > x + w)
     {
        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (it->on_move) continue;
             evas_object_geometry_get(VIEW(it), &x, &y, &w, &h);
             if (ev->cur.canvas.x > x && ev->cur.canvas.x < x + w) break;
          }
        if (it && (it != sd->reorder_empty))
          {
             _animate_missed_items(sd->reorder_empty, it);
             if (!it->trans && it != item)
               {
                  evas_object_geometry_get(VIEW(it), &fx, &fy, &fw, &fh);
                  _item_transition_start(it, fx, fy, fw, fh);
                  sd->reorder_empty = it;
               }
             EINA_INLIST_FOREACH(sd->items, it2)
               {
                  if (it == it2->reorder_to) break;
               }
             if (it2)
               {
                  it2->reorder_to = NULL;
                  evas_object_geometry_get(it2->proxy, &fx, &fy, &fw, &fh);
                  if (it2->trans) elm_transit_del(it2->trans);
                  _item_transition_start(it2, fx, fy, fw, fh);
                  sd->reorder_empty = it;
               }
          }
     }
}

static void
_mouse_up_reorder(Elm_Toolbar_Item_Data *it,
                  Evas *evas EINA_UNUSED,
                  Evas_Object *obj,
                  Evas_Event_Mouse_Up *ev EINA_UNUSED)
{
   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   evas_object_event_callback_del_full
     (obj, EVAS_CALLBACK_MOUSE_MOVE,
     (Evas_Object_Event_Cb)_mouse_move_reorder, it);
   evas_object_event_callback_del_full
     (sd->more, EVAS_CALLBACK_MOUSE_MOVE,
     (Evas_Object_Event_Cb)_mouse_move_reorder, it);
   evas_object_event_callback_del_full
     (VIEW(it), EVAS_CALLBACK_MOUSE_MOVE,
     (Evas_Object_Event_Cb)_mouse_move_reorder, it);
   evas_object_event_callback_del_full
     (obj, EVAS_CALLBACK_MOUSE_UP,
     (Evas_Object_Event_Cb)_mouse_up_reorder, it);
   evas_object_event_callback_del_full
     (sd->more, EVAS_CALLBACK_MOUSE_UP,
     (Evas_Object_Event_Cb)_mouse_up_reorder, it);

   if (it->proxy)
     {
        evas_object_image_source_visible_set(elm_image_object_get(it->proxy), EINA_TRUE);
        ELM_SAFE_FREE(it->proxy, evas_object_del);
     }

   eo_do(obj, elm_interface_scrollable_hold_set(EINA_FALSE));
}

static void
_item_reorder_start(Elm_Toolbar_Item_Data *item)
{
   Evas_Object *obj = WIDGET(item), *img = NULL;
   Evas_Coord x, y, w, h;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   sd->reorder_empty = sd->reorder_item = item;

   item->proxy = elm_image_add(obj);
   img = elm_image_object_get(item->proxy);
   elm_image_aspect_fixed_set(item->proxy, EINA_FALSE);
   evas_object_image_source_set(img, VIEW(item));
   evas_object_image_source_visible_set(img, EINA_FALSE);
   evas_object_image_source_clip_set(img, EINA_FALSE);

   evas_object_layer_set(item->proxy, 100);
   elm_layout_signal_emit(VIEW(item), "elm,state,moving", "elm");

   evas_object_event_callback_add
     (obj, EVAS_CALLBACK_MOUSE_MOVE,
     (Evas_Object_Event_Cb)_mouse_move_reorder, item);

   evas_object_event_callback_add
     (sd->more, EVAS_CALLBACK_MOUSE_MOVE,
     (Evas_Object_Event_Cb)_mouse_move_reorder, item);

   evas_object_event_callback_add
     (item->proxy, EVAS_CALLBACK_MOUSE_MOVE,
     (Evas_Object_Event_Cb)_mouse_move_reorder, item);

   evas_object_event_callback_add
     (obj, EVAS_CALLBACK_MOUSE_UP,
     (Evas_Object_Event_Cb)_mouse_up_reorder, item);

   evas_object_event_callback_add
     (sd->more, EVAS_CALLBACK_MOUSE_UP,
     (Evas_Object_Event_Cb)_mouse_up_reorder, item);

   evas_object_geometry_get(VIEW(item), &x, &y, &w, &h);
   evas_object_resize(item->proxy, w, h);
   evas_object_move(item->proxy, x, y);
   evas_object_show(item->proxy);

   eo_do(WIDGET(item), elm_interface_scrollable_hold_set(EINA_TRUE));
}

static Eina_Bool
_long_press_cb(void *data)
{
   Elm_Toolbar_Item_Data *it = data;
   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   sd->long_timer = NULL;
   sd->long_press = EINA_TRUE;

   if (sd->reorder_mode)
     _item_reorder_start(it);

   eo_do(WIDGET(it), eo_event_callback_call(EVAS_CLICKABLE_INTERFACE_EVENT_LONGPRESSED, EO_OBJ(it)));

   return ECORE_CALLBACK_CANCEL;
}

static void
_mouse_move_cb(Elm_Toolbar_Item_Data *it,
               Evas *evas EINA_UNUSED,
               Evas_Object *obj EINA_UNUSED,
               Evas_Event_Mouse_Move *ev)
{
   Evas_Coord x, y, w, h;

   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);
   evas_object_geometry_get(VIEW(it), &x, &y, &w, &h);

   if ((x > ev->cur.canvas.x) || (ev->cur.canvas.x > x + w) ||
       (y > ev->cur.canvas.y) || (ev->cur.canvas.y > y + h))
     {
        ELM_SAFE_FREE(sd->long_timer, ecore_timer_del);
     }
}

static void
_mouse_down_cb(Elm_Toolbar_Item_Data *it,
               Evas *evas EINA_UNUSED,
               Evas_Object *obj EINA_UNUSED,
               Evas_Event_Mouse_Down *ev)
{
   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   if (ev->button != 1) return;
   if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
     eo_do(WIDGET(it), eo_event_callback_call(EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED_DOUBLE, EO_OBJ(it)));
   sd->mouse_down = EINA_TRUE;
   sd->long_press = EINA_FALSE;
   if (sd->long_timer)
     ecore_timer_interval_set
       (sd->long_timer, _elm_config->longpress_timeout);
   else
     sd->long_timer = ecore_timer_add
         (_elm_config->longpress_timeout, _long_press_cb, it);

   evas_object_event_callback_add(VIEW(it), EVAS_CALLBACK_MOUSE_MOVE,
                                  (Evas_Object_Event_Cb)_mouse_move_cb, it);
}

static void
_mouse_up_cb(Elm_Toolbar_Item_Data *it,
             Evas *evas EINA_UNUSED,
             Evas_Object *obj EINA_UNUSED,
             Evas_Event_Mouse_Up *ev)
{
   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   if (ev->button != 1) return;
   sd->mouse_down = EINA_FALSE;
   ELM_SAFE_FREE(sd->long_timer, ecore_timer_del);
   if ((!elm_object_item_disabled_get(EO_OBJ(it))) &&
       (sd->focused_item != EO_OBJ(it)))
     elm_object_item_focus_set(EO_OBJ(it), EINA_TRUE);
   evas_object_event_callback_del_full
     (VIEW(it), EVAS_CALLBACK_MOUSE_MOVE,
     (Evas_Object_Event_Cb)_mouse_move_cb, it);
}

static void
_mouse_in_cb(void *data,
             Evas_Object *obj EINA_UNUSED,
             const char *emission EINA_UNUSED,
             const char *source EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it = data;

   elm_layout_signal_emit(VIEW(it), "elm,state,highlighted", "elm");
   if (it->icon)
     elm_widget_signal_emit(it->icon, "elm,state,highlighted", "elm");

   if (!elm_object_item_disabled_get(EO_OBJ(it)) &&
       (_elm_config->focus_move_policy == ELM_FOCUS_MOVE_POLICY_IN))
     elm_object_item_focus_set(EO_OBJ(it), EINA_TRUE);
}

static void
_mouse_out_cb(void *data,
              Evas_Object *obj EINA_UNUSED,
              const char *emission EINA_UNUSED,
              const char *source EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it = data;

   elm_layout_signal_emit(VIEW(it), "elm,state,unhighlighted", "elm");
   if (it->icon)
     elm_widget_signal_emit(it->icon, "elm,state,unhighlighted", "elm");
}

static void
_scroll_cb(Evas_Object *obj,
           void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL, NULL));
}

static void
_scroll_anim_start_cb(Evas_Object *obj,
                      void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_ANIM_START, NULL));
}

static void
_scroll_anim_stop_cb(Evas_Object *obj,
                     void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_ANIM_STOP, NULL));
}

static void
_scroll_drag_start_cb(Evas_Object *obj,
                      void *data EINA_UNUSED)
{
   ELM_TOOLBAR_DATA_GET(obj, sd);
   ELM_SAFE_FREE(sd->long_timer, ecore_timer_del);

   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_DRAG_START, NULL));
}

static void
_scroll_drag_stop_cb(Evas_Object *obj,
                     void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call(EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_DRAG_STOP, NULL));
}

static void
_layout(Evas_Object *o,
        Evas_Object_Box_Data *priv,
        void *data)
{
   Evas_Object *obj = (Evas_Object *)data;

   ELM_TOOLBAR_DATA_GET(obj, sd);
   _els_box_layout
     (o, priv, !sd->vertical, sd->homogeneous, elm_widget_mirrored_get(obj));
}

static char *
_access_info_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it = (Elm_Toolbar_Item_Data *)data;
   const char *txt = (it->base)->access_info;

   if (!txt) txt = it->label;
   if (txt) return strdup(txt);

   return NULL;
}

static char *
_access_state_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   Elm_Toolbar_Item_Data *it = (Elm_Toolbar_Item_Data *)data;
   Eina_Bool tmp;

   if (it->separator)
     return strdup(E_("Separator"));
   else if (eo_do_ret(EO_OBJ(it), tmp, elm_wdg_item_disabled_get()))
     return strdup(E_("State: Disabled"));
   else if (it->selected)
     return strdup(E_("State: Selected"));
   else if (it->menu)
     return strdup(E_("Has menu"));

   return NULL;
}

EOLIAN static void
_elm_toolbar_item_eo_base_destructor(Eo *eo_item, Elm_Toolbar_Item_Data *item)
{
   Elm_Toolbar_Item_Data *next = NULL;
   Evas_Object *obj;

   ELM_TOOLBAR_DATA_GET(WIDGET(item), sd);

   obj = WIDGET(item);

   if (item != sd->more_item) /* more item does not get in the list */
     {
        if (!sd->delete_me)
          next = ELM_TOOLBAR_ITEM_FROM_INLIST(EINA_INLIST_GET(item)->next);
        sd->items = eina_inlist_remove(sd->items, EINA_INLIST_GET(item));
        sd->item_count--;
        if (!sd->delete_me)
          {
             if (!next) next = ELM_TOOLBAR_ITEM_FROM_INLIST(sd->items);
             if ((sd->select_mode == ELM_OBJECT_SELECT_MODE_ALWAYS) &&
                 item->selected && next) _item_select(next);
          }
     }

   _item_del(item);

   if (item != sd->more_item)
      eo_do(obj, elm_obj_widget_theme_apply());

   eo_do_super(eo_item, ELM_TOOLBAR_ITEM_CLASS, eo_destructor());
}

static void
_access_activate_cb(void *data EINA_UNUSED,
                    Evas_Object *part_obj EINA_UNUSED,
                    Elm_Object_Item *item)
{
   ELM_TOOLBAR_ITEM_DATA_GET(item, it);
   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);
   Eina_Bool tmp;

   if (eo_do_ret(item, tmp, elm_wdg_item_disabled_get())) return;

   if (it->selected && (sd->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS))
     {
        _elm_access_say(E_("Unselected"));
        _item_unselect(it);
     }
   else
     {
        _elm_access_say(E_("Selected"));
        _item_select(it);
     }
}

static void
_access_widget_item_register(Elm_Toolbar_Item_Data *it)
{
   Elm_Access_Info *ai;
   _elm_access_widget_item_register(it->base);
   ai = _elm_access_info_get(it->base->access_obj);

   _elm_access_text_set(ai, ELM_ACCESS_TYPE, E_("Toolbar Item"));
   _elm_access_callback_set(ai, ELM_ACCESS_INFO, _access_info_cb, it);
   _elm_access_callback_set(ai, ELM_ACCESS_STATE, _access_state_cb, it);
   _elm_access_activate_callback_set(ai, _access_activate_cb, NULL);
}

EOLIAN static Eo *
_elm_toolbar_item_eo_base_constructor(Eo *eo_it, Elm_Toolbar_Item_Data *it)
{
   eo_it = eo_do_super_ret(eo_it, ELM_TOOLBAR_ITEM_CLASS, eo_it, eo_constructor());
   it->base = eo_data_scope_get(eo_it, ELM_WIDGET_ITEM_CLASS);
   eo_do(eo_it, elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_MENU_ITEM));

   return eo_it;
}

static Elm_Toolbar_Item_Data *
_item_new(Evas_Object *obj,
          const char *icon,
          const char *label,
          Evas_Smart_Cb func,
          const void *data)
{
   Evas_Object *icon_obj;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   icon_obj = elm_icon_add(obj);
   elm_icon_order_lookup_set(icon_obj, sd->lookup_order);
   if (!icon_obj) return NULL;

   Eo *eo_it = eo_add(ELM_TOOLBAR_ITEM_CLASS, obj);
   if (!eo_it)
     {
        evas_object_del(icon_obj);
        return NULL;
     }
   ELM_TOOLBAR_ITEM_DATA_GET(eo_it, it);

   it->label = eina_stringshare_add(label);
   it->prio.visible = 1;
   it->prio.priority = 0;
   it->func = func;
   it->separator = EINA_FALSE;
   it->object = NULL;
   WIDGET_ITEM_DATA_SET(EO_OBJ(it), data);

   VIEW(it) = elm_layout_add(obj);
   evas_object_data_set(VIEW(it), "item", it);

   if (_elm_config->access_mode == ELM_ACCESS_MODE_ON)
     _access_widget_item_register(it);

   eo_do(icon_obj, elm_interface_atspi_accessible_parent_set(VIEW(it)));
   eo_do(VIEW(it), elm_interface_atspi_accessible_parent_set(eo_it));

   if (_item_icon_set(icon_obj, "toolbar/", icon))
     {
        it->icon = icon_obj;
        it->icon_str = eina_stringshare_add(icon);
     }
   else
     {
        it->icon = NULL;
        it->icon_str = NULL;
        evas_object_del(icon_obj);
     }

   if (!elm_layout_theme_set
       (VIEW(it), "toolbar", "item", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");
   elm_layout_signal_callback_add
     (VIEW(it), "elm,action,click", "elm", _select_cb, it);
   elm_layout_signal_callback_add
     (VIEW(it), "mouse,clicked,*", "*", (Edje_Signal_Cb)_select_filter_cb, it);
   elm_layout_signal_callback_add
     (VIEW(it), "elm,mouse,in", "elm", _mouse_in_cb, it);
   elm_layout_signal_callback_add
     (VIEW(it), "elm,mouse,out", "elm", _mouse_out_cb, it);
   evas_object_event_callback_add
     (VIEW(it), EVAS_CALLBACK_MOUSE_DOWN, (Evas_Object_Event_Cb)_mouse_down_cb,
     it);
   evas_object_event_callback_add
     (VIEW(it), EVAS_CALLBACK_MOUSE_UP, (Evas_Object_Event_Cb)_mouse_up_cb, it);
   elm_widget_sub_object_add(obj, VIEW(it));

   if (it->icon)
     {
        evas_object_size_hint_min_set(it->icon, sd->icon_size, sd->icon_size);
        evas_object_size_hint_max_set(it->icon, sd->icon_size, sd->icon_size);
        elm_layout_content_set(VIEW(it), "elm.swallow.icon", it->icon);
        elm_layout_signal_emit(VIEW(it), "elm,state,icon,visible", "elm");
        elm_layout_signal_emit(VIEW(it), "elm,icon,visible", "elm");
        evas_object_show(it->icon);
        elm_widget_sub_object_add(obj, it->icon);
     }
   else
     {
        elm_layout_signal_emit(VIEW(it), "elm,state,icon,hidden", "elm");
        elm_layout_signal_emit(VIEW(it), "elm,icon,hidden", "elm");
     }

   if (it->label)
     {
        elm_layout_text_set(VIEW(it), "elm.text", it->label);
        elm_layout_signal_emit(VIEW(it), "elm,state,text,visible", "elm");
        elm_layout_signal_emit(VIEW(it), "elm,text,visible", "elm");
     }
   else
     {
        elm_layout_signal_emit(VIEW(it), "elm,state,text,hidden", "elm");
        elm_layout_signal_emit(VIEW(it), "elm,text,hidden", "elm");
     }

   edje_object_message_signal_process(elm_layout_edje_get(VIEW(it)));

   evas_object_event_callback_add
     (VIEW(it), EVAS_CALLBACK_RESIZE, _item_resize, obj);
   if ((!sd->items) && (sd->select_mode == ELM_OBJECT_SELECT_MODE_ALWAYS))
     _item_select(it);

   if (_elm_config->atspi_mode)
        elm_interface_atspi_accessible_added(eo_it);

   return it;
}

static void
_elm_toolbar_item_icon_update(Elm_Toolbar_Item_Data *item)
{
   Elm_Toolbar_Item_State *it_state;
   Evas_Object *old_icon =
     elm_layout_content_get(VIEW(item), "elm.swallow.icon");
   Eina_List *l;

   elm_widget_sub_object_del(WIDGET(item), old_icon);
   elm_layout_content_set(VIEW(item), "elm.swallow.icon", item->icon);
   if (item->icon)
       elm_layout_signal_emit(VIEW(item), "elm,state,icon,visible", "elm");
   else
       elm_layout_signal_emit(VIEW(item), "elm,state,icon,hidden", "elm");
   evas_object_hide(old_icon);

   EINA_LIST_FOREACH(item->states, l, it_state)
     {
        if (it_state->icon == old_icon) return;
     }
   evas_object_del(old_icon);
}

static void
_elm_toolbar_item_icon_set_cb(void *data,
                              Evas_Object *obj,
                              const char *emission,
                              const char *source)
{
   Elm_Toolbar_Item_Data *item = data;

   elm_layout_content_unset(VIEW(item), "elm.swallow.icon");
   _elm_toolbar_item_icon_update(item);
   elm_layout_signal_callback_del
     (obj, emission, source, _elm_toolbar_item_icon_set_cb);
   elm_layout_signal_emit(VIEW(item), "elm,state,icon,reset", "elm");
}

static void
_elm_toolbar_item_icon_obj_set(Evas_Object *obj,
                               Elm_Toolbar_Item_Data *item,
                               Evas_Object *icon_obj,
                               const char *icon_str,
                               double icon_size,
                               const char *sig)
{
   Evas_Object *old_icon;
   const char *s;
   int ms = 0;

   if (icon_str)
     eina_stringshare_replace(&item->icon_str, icon_str);
   else
     {
        eina_stringshare_del(item->icon_str);
        item->icon_str = NULL;
     }
   item->icon = icon_obj;
   if (icon_obj)
     {
        ms = (icon_size * elm_config_scale_get());
        evas_object_size_hint_min_set(item->icon, ms, ms);
        evas_object_size_hint_max_set(item->icon, ms, ms);
        evas_object_show(item->icon);
        elm_widget_sub_object_add(obj, item->icon);
     }
   s = elm_layout_data_get(VIEW(item), "transition_animation_on");
   if ((s) && (atoi(s)))
     {
        old_icon = elm_layout_content_get
            (VIEW(item), "elm.swallow.icon_new");
        if (old_icon)
          {
             elm_widget_sub_object_del(WIDGET(item), old_icon);
             evas_object_hide(old_icon);
          }
        elm_layout_content_set
          (VIEW(item), "elm.swallow.icon_new", item->icon);
        elm_layout_signal_emit(VIEW(item), sig, "elm");
        elm_layout_signal_callback_add
          (VIEW(item), "elm,state,icon_set,done", "elm",
          _elm_toolbar_item_icon_set_cb, item);
     }
   else
     _elm_toolbar_item_icon_update(item);
   _resizing_eval_item(item);
}

static void
_elm_toolbar_item_state_cb(void *data EINA_UNUSED,
                           Evas_Object *obj EINA_UNUSED,
                           void *event_info)
{
   Elm_Object_Item *eo_it = event_info;
   ELM_TOOLBAR_ITEM_DATA_GET(eo_it, it);
   Elm_Toolbar_Item_State *it_state;

   it_state = eina_list_data_get(it->current_state);
   if (it_state->func)
     it_state->func((void *)it_state->data, obj, event_info);
}

static Elm_Toolbar_Item_State *
_item_state_new(const char *label,
                const char *icon_str,
                Evas_Object *icon,
                Evas_Smart_Cb func,
                const void *data)
{
   Elm_Toolbar_Item_State *it_state;

   it_state = ELM_NEW(Elm_Toolbar_Item_State);
   it_state->label = eina_stringshare_add(label);
   it_state->icon_str = eina_stringshare_add(icon_str);
   it_state->icon = icon;
   it_state->func = func;
   it_state->data = data;

   return it_state;
}

static void
_elm_toolbar_action_left_cb(void *data,
                            Evas_Object *o EINA_UNUSED,
                            const char *sig EINA_UNUSED,
                            const char *src EINA_UNUSED)
{
   Evas_Object *obj = data;
   Elm_Toolbar_Item_Data *it, *it2;
   Eina_Bool done = EINA_FALSE;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->selected)
          {
             Eina_Bool found = EINA_FALSE;

             EINA_INLIST_REVERSE_FOREACH(sd->items, it2)
               {
                  if (elm_object_item_disabled_get(EO_OBJ(it2)))
                    continue;
                  if (it2 == it)
                    {
                       found = EINA_TRUE;
                       continue;
                    }
                  if (!found) continue;
                  if (it2->separator) continue;
                  _item_unselect(it);
                  _item_select(it2);
                  break;
               }
             done = EINA_TRUE;
             break;
          }
     }
   if (!done)
     {
        EINA_INLIST_FOREACH(sd->items, it)
          {
             if (elm_object_item_disabled_get(EO_OBJ(it))) continue;
             if (it->separator) continue;
             _item_select(it);
             break;
          }
     }
}

static void
_elm_toolbar_action_right_cb(void *data,
                             Evas_Object *o EINA_UNUSED,
                             const char *sig EINA_UNUSED,
                             const char *src EINA_UNUSED)
{
   Evas_Object *obj = data;
   Elm_Toolbar_Item_Data *it, *it2;
   Eina_Bool done = EINA_FALSE;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->selected)
          {
             Eina_Bool found = EINA_FALSE;

             EINA_INLIST_FOREACH(sd->items, it2)
               {
                  if (elm_object_item_disabled_get(EO_OBJ(it2)))
                    continue;
                  if (it2 == it)
                    {
                       found = EINA_TRUE;
                       continue;
                    }
                  if (!found) continue;
                  if (it2->separator) continue;
                  _item_unselect(it);
                  _item_select(it2);
                  break;
               }
             done = EINA_TRUE;
             break;
          }
     }
   if (!done)
     {
        EINA_INLIST_REVERSE_FOREACH(sd->items, it)
          {
             if (elm_object_item_disabled_get(EO_OBJ(it))) continue;
             if (it->separator) continue;
             _item_select(it);
             break;
          }
     }
}

static void
_elm_toolbar_action_up_cb(void *data,
                          Evas_Object *o,
                          const char *sig,
                          const char *src)
{
   _elm_toolbar_action_left_cb(data, o, sig, src);
}

static void
_elm_toolbar_action_down_cb(void *data,
                            Evas_Object *o,
                            const char *sig,
                            const char *src)
{
   _elm_toolbar_action_right_cb(data, o, sig, src);
}

EOLIAN static void
_elm_toolbar_evas_object_smart_add(Eo *obj, Elm_Toolbar_Data *priv)
{
   Evas_Object *edje;

   elm_widget_sub_object_parent_add(obj);

   edje = edje_object_add(evas_object_evas_get(obj));
   elm_widget_resize_object_set(obj, edje, EINA_TRUE);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());

   elm_widget_theme_object_set
     (obj, edje, "toolbar", "base", elm_widget_style_get(obj));

   priv->hit_rect = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_smart_member_add(priv->hit_rect, obj);
   elm_widget_sub_object_add(obj, priv->hit_rect);

   /* common scroller hit rectangle setup */
   evas_object_color_set(priv->hit_rect, 0, 0, 0, 0);
   evas_object_show(priv->hit_rect);
   evas_object_repeat_events_set(priv->hit_rect, EINA_TRUE);

   elm_widget_can_focus_set(obj, EINA_TRUE);

   eo_do(obj, elm_interface_scrollable_objects_set(edje, priv->hit_rect));

   priv->standard_priority = -99999;

   eo_do(obj,
         elm_interface_scrollable_bounce_allow_set
         (_elm_config->thumbscroll_bounce_enable, EINA_FALSE),
         elm_interface_scrollable_policy_set
         (ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF),
         elm_interface_scrollable_scroll_cb_set(_scroll_cb),
         elm_interface_scrollable_animate_start_cb_set(_scroll_anim_start_cb),
         elm_interface_scrollable_animate_stop_cb_set(_scroll_anim_stop_cb),
         elm_interface_scrollable_drag_start_cb_set(_scroll_drag_start_cb),
         elm_interface_scrollable_drag_stop_cb_set(_scroll_drag_stop_cb));

   edje_object_signal_callback_add
     (edje, "elm,action,left", "elm", _elm_toolbar_action_left_cb, obj);
   edje_object_signal_callback_add
     (edje, "elm,action,right", "elm", _elm_toolbar_action_right_cb, obj);
   edje_object_signal_callback_add
     (edje, "elm,action,up", "elm", _elm_toolbar_action_up_cb, obj);
   edje_object_signal_callback_add
     (edje, "elm,action,down", "elm", _elm_toolbar_action_down_cb, obj);

   priv->shrink_mode = ELM_TOOLBAR_SHRINK_NONE;
   priv->theme_icon_size = _internal_elm_toolbar_icon_size_get(obj);
   priv->icon_size = priv->theme_icon_size;

   priv->homogeneous = EINA_TRUE;
   priv->align = 0.5;

   priv->bx = evas_object_box_add(evas_object_evas_get(obj));
   evas_object_box_align_set(priv->bx, priv->align, 0.5);
   evas_object_box_layout_set(priv->bx, _layout, obj, NULL);
   elm_widget_sub_object_add(obj, priv->bx);
   eo_do(obj, elm_interface_scrollable_content_set(priv->bx));
   evas_object_show(priv->bx);

   priv->more = elm_layout_add(obj);
   if (!elm_layout_theme_set(priv->more, "toolbar", "more", "default"))
     CRI("Failed to set layout!");
   if (priv->vertical)
     elm_object_signal_emit(priv->more, "elm,orient,vertical", "elm");
   else
     elm_object_signal_emit(priv->more, "elm,orient,horizontal", "elm");

   elm_widget_sub_object_add(obj, priv->more);
   evas_object_show(priv->more);

   priv->bx_more = evas_object_box_add(evas_object_evas_get(obj));
   evas_object_box_align_set(priv->bx_more, priv->align, 0.5);
   evas_object_box_layout_set(priv->bx_more, _layout, obj, NULL);
   elm_widget_sub_object_add(obj, priv->bx_more);
   elm_layout_content_set
     (priv->more, "elm.swallow.content", priv->bx_more);
   evas_object_show(priv->bx_more);

   priv->bx_more2 = evas_object_box_add(evas_object_evas_get(obj));
   evas_object_box_align_set(priv->bx_more2, priv->align, 0.5);
   evas_object_box_layout_set(priv->bx_more2, _layout, obj, NULL);
   elm_widget_sub_object_add(obj, priv->bx_more2);
   elm_layout_content_set
     (priv->more, "elm.swallow.content2", priv->bx_more2);
   evas_object_show(priv->bx_more2);

   elm_toolbar_shrink_mode_set(obj, _elm_config->toolbar_shrink_mode);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_RESIZE, _resize_cb, obj);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOVE, _move_cb, obj);
   evas_object_event_callback_add
     (priv->bx, EVAS_CALLBACK_RESIZE, _resize_cb, obj);
   elm_toolbar_icon_order_lookup_set(obj, ELM_ICON_LOOKUP_THEME_FDO);

   _elm_toolbar_highlight_in_theme(obj);
   _sizing_eval(obj);
}

EOLIAN static void
_elm_toolbar_evas_object_smart_del(Eo *obj, Elm_Toolbar_Data *sd)
{
   Elm_Toolbar_Item_Data *it, *next;

   sd->delete_me = EINA_TRUE;

   ecore_job_del(sd->resize_job);
   sd->resize_job = NULL;

   it = ELM_TOOLBAR_ITEM_FROM_INLIST(sd->items);
   while (it)
     {
        next = ELM_TOOLBAR_ITEM_FROM_INLIST(EINA_INLIST_GET(it)->next);
        eo_do(EO_OBJ(it), elm_wdg_item_del());
        it = next;
     }
   if (sd->more_item) eo_do(EO_OBJ(sd->more_item), elm_wdg_item_del());
   ecore_timer_del(sd->long_timer);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

EOLIAN static void
_elm_toolbar_evas_object_smart_move(Eo *obj, Elm_Toolbar_Data *sd, Evas_Coord x, Evas_Coord y)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_move(x, y));

   evas_object_move(sd->hit_rect, x, y);
}

EOLIAN static void
_elm_toolbar_evas_object_smart_resize(Eo *obj, Elm_Toolbar_Data *sd, Evas_Coord w, Evas_Coord h)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_resize(w, h));

   evas_object_resize(sd->hit_rect, w, h);
}

EOLIAN static void
_elm_toolbar_evas_object_smart_member_add(Eo *obj, Elm_Toolbar_Data *sd, Evas_Object *member)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_member_add(member));

   if (sd->hit_rect)
     evas_object_raise(sd->hit_rect);
}

static Eina_List *
_access_item_find_append(const Evas_Object *obj,
                         Evas_Object *bx,
                         Eina_List *items)
{
   Elm_Toolbar_Item_Data *it;
   Eina_List *list;

   ELM_TOOLBAR_DATA_GET(obj, sd);

   list = evas_object_box_children_get(bx);
   if (!list) return items;

   EINA_INLIST_FOREACH (sd->items, it)
     {
        if (it->separator) continue;
        if (eina_list_data_find(list, it->base->view))
          items = eina_list_append(items, it->base->access_obj);
     }
   eina_list_free(list);

   return items;
}

static Eina_Bool _elm_toolbar_smart_focus_next_enable = EINA_FALSE;

EOLIAN static Eina_Bool
_elm_toolbar_elm_widget_focus_next_manager_is(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *_pd EINA_UNUSED)
{
   return _elm_toolbar_smart_focus_next_enable;
}

EOLIAN static Eina_Bool
_elm_toolbar_elm_widget_focus_next(Eo *obj, Elm_Toolbar_Data *sd, Elm_Focus_Direction dir, Evas_Object **next, Elm_Object_Item **next_item)
{
   Eina_List *items = NULL;
   Eina_List *list;

   if (sd->more_item && sd->more_item->selected)
     {
        items = _access_item_find_append(obj, sd->bx_more, items);
        items = _access_item_find_append(obj, sd->bx_more2, items);
        items = eina_list_append(items, sd->more_item->base->access_obj);
     }
   else
     {
        items = _access_item_find_append(obj, sd->bx, items);
        list = evas_object_box_children_get(sd->bx);
        if (sd->more_item && eina_list_data_find(list, sd->more_item->base->view))
          items = eina_list_append(items, sd->more_item->base->access_obj);
        eina_list_free(list);
     }

   return elm_widget_focus_list_next_get
            (obj, items, eina_list_data_get, dir, next, next_item);
}

static void
_access_obj_process(Elm_Toolbar_Data * sd, Eina_Bool is_access)
{
   Elm_Toolbar_Item_Data *it;

   EINA_INLIST_FOREACH (sd->items, it)
     {
        if (is_access) _access_widget_item_register(it);
        else _elm_access_widget_item_unregister(it->base);
     }
}

EOLIAN static void
_elm_toolbar_elm_widget_access(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd, Eina_Bool acs)
{
   _elm_toolbar_smart_focus_next_enable = acs;
   _access_obj_process(sd, _elm_toolbar_smart_focus_next_enable);
}

static void
_elm_toolbar_coordinates_adjust(Elm_Toolbar_Item_Data *it,
                                Evas_Coord *x,
                                Evas_Coord *y,
                                Evas_Coord *w,
                                Evas_Coord *h)
{
   ELM_TOOLBAR_DATA_GET(WIDGET(it), sd);

   Evas_Coord ix, iy, iw, ih, vx, vy, vw, vh;

   evas_object_geometry_get(sd->hit_rect, &vx, &vy, &vw, &vh);
   evas_object_geometry_get(VIEW(it), &ix, &iy, &iw, &ih);
   *x = ix;
   *y = iy;
   *w = iw;
   *h = ih;
   if (sd->vertical)
     {
        //TODO: Enhance it later.
        if ((ix < vx) || (ix + iw) > (vx + vw) || (iy + ih) > (vy + vh))
          *y = iy - ih;
        else if (iy < vy)
          *y = iy + ih;
     }
   else
     {
        //TODO: Enhance it later.
        if ((iy < vy) || (ix + iw) > (vx + vw) || (iy + ih) > (vy + vh))
          *x = ix - iw;
        else if (ix < vx)
          *x = ix + iw;
     }
}

EOLIAN static void
_elm_toolbar_elm_widget_focus_highlight_geometry_get(const Eo *obj, Elm_Toolbar_Data *sd, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
   if (sd->focused_item)
     {
        ELM_TOOLBAR_ITEM_DATA_GET(sd->focused_item, focus_it);
        _elm_toolbar_coordinates_adjust
           (focus_it, x, y, w, h);
        elm_widget_focus_highlight_focus_part_geometry_get
           (VIEW(focus_it), x, y, w, h);
     }
   else
     evas_object_geometry_get(obj, x, y, w, h);
}

EAPI Evas_Object *
elm_toolbar_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

EOLIAN static Eo *
_elm_toolbar_eo_base_constructor(Eo *obj, Elm_Toolbar_Data *_pd EINA_UNUSED)
{
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());
   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_TOOL_BAR));

   return obj;
}

EOLIAN static void
_elm_toolbar_icon_size_set(Eo *obj, Elm_Toolbar_Data *sd, int icon_size)
{
   if (sd->priv_icon_size == icon_size) return;
   sd->priv_icon_size = icon_size;

   if (sd->priv_icon_size) sd->icon_size = sd->priv_icon_size;
   else sd->icon_size = sd->theme_icon_size;

   eo_do(obj, elm_obj_widget_theme_apply());
}

EOLIAN static int
_elm_toolbar_icon_size_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->priv_icon_size;
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_item_append(Eo *obj, Elm_Toolbar_Data *sd, const char *icon, const char *label, Evas_Smart_Cb func, const void *data)
{
   Elm_Toolbar_Item_Data *it;
   double scale;

   it = _item_new(obj, icon, label, func, data);
   if (!it) return NULL;
   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());

   sd->items = eina_inlist_append(sd->items, EINA_INLIST_GET(it));
   evas_object_box_append(sd->bx, VIEW(it));
   evas_object_show(VIEW(it));

   _item_theme_hook(obj, it, scale, sd->icon_size);
   sd->item_count++;

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_item_prepend(Eo *obj, Elm_Toolbar_Data *sd, const char *icon, const char *label, Evas_Smart_Cb func, const void *data)
{
   Elm_Toolbar_Item_Data *it;
   double scale;

   it = _item_new(obj, icon, label, func, data);
   if (!it) return NULL;
   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());

   sd->items = eina_inlist_prepend(sd->items, EINA_INLIST_GET(it));
   evas_object_box_prepend(sd->bx, VIEW(it));
   evas_object_show(VIEW(it));
   _item_theme_hook(obj, it, scale, sd->icon_size);
   sd->item_count++;

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_item_insert_before(Eo *obj, Elm_Toolbar_Data *sd, Elm_Object_Item *eo_before, const char *icon, const char *label, Evas_Smart_Cb func, const void *data)
{
   Elm_Toolbar_Item_Data *it;
   double scale;

   EINA_SAFETY_ON_NULL_RETURN_VAL(eo_before, NULL);
   ELM_TOOLBAR_ITEM_DATA_GET(eo_before, _before);
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(_before,  NULL);

   it = _item_new(obj, icon, label, func, data);
   if (!it) return NULL;
   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());

   sd->items = eina_inlist_prepend_relative
       (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(_before));
   evas_object_box_insert_before(sd->bx, VIEW(it), VIEW(_before));
   _item_theme_hook(obj, it, scale, sd->icon_size);
   sd->item_count++;

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_item_insert_after(Eo *obj, Elm_Toolbar_Data *sd, Elm_Object_Item *eo_after, const char *icon, const char *label, Evas_Smart_Cb func, const void *data)
{
   Elm_Toolbar_Item_Data *it;
   double scale;

   EINA_SAFETY_ON_NULL_RETURN_VAL(eo_after, NULL);
   ELM_TOOLBAR_ITEM_DATA_GET(eo_after, _after);
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(_after, NULL);

   it = _item_new(obj, icon, label, func, data);
   if (!it) return NULL;
   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());

   sd->items = eina_inlist_append_relative
       (sd->items, EINA_INLIST_GET(it), EINA_INLIST_GET(_after));
   evas_object_box_insert_after(sd->bx, VIEW(it), VIEW(_after));
   _item_theme_hook(obj, it, scale, sd->icon_size);
   sd->item_count++;

   return EO_OBJ(it);
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_first_item_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   if (!sd->items) return NULL;
   Elm_Toolbar_Item_Data *it = ELM_TOOLBAR_ITEM_FROM_INLIST(sd->items);
   if (it) return EO_OBJ(it);
   return NULL;
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_last_item_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   if (!sd->items) return NULL;

   Elm_Toolbar_Item_Data *it = ELM_TOOLBAR_ITEM_FROM_INLIST(sd->items->last);
   if (it) return EO_OBJ(it);
   return NULL;
}

EOLIAN static Elm_Object_Item *
_elm_toolbar_item_next_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *it)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(it, NULL);

   Elm_Toolbar_Item_Data *ret_it = ELM_TOOLBAR_ITEM_FROM_INLIST(
            EINA_INLIST_GET(it)->next);
   if (ret_it) return EO_OBJ(ret_it);
   else return NULL;
}

EOLIAN static Elm_Object_Item *
_elm_toolbar_item_prev_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *it)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(it, NULL);

   Elm_Toolbar_Item_Data *ret_it = ELM_TOOLBAR_ITEM_FROM_INLIST(
            EINA_INLIST_GET(it)->prev);
   if (ret_it) return EO_OBJ(ret_it);
   else return NULL;
}

EOLIAN static void
_elm_toolbar_item_priority_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item, int priority)
{
   if (item->prio.priority == priority) return;
   item->prio.priority = priority;
   _resizing_eval(WIDGET(item));
}

EOLIAN static int
_elm_toolbar_item_priority_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   return item->prio.priority;
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_item_find_by_label(const Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd, const char *label)
{
   Elm_Toolbar_Item_Data *it;

   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (!strcmp(it->label, label)) return EO_OBJ(it);
     }

   return NULL;
}

EOLIAN static void
_elm_toolbar_item_selected_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item, Eina_Bool selected)
{
   if (item->selected == selected) return;
   if (selected) _item_select(item);
   else _item_unselect(item);
}

EOLIAN static Eina_Bool
_elm_toolbar_item_selected_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   return item->selected;
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_selected_item_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->selected_item;
}

EOLIAN static Elm_Object_Item*
_elm_toolbar_more_item_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return EO_OBJ(sd->more_item);
}

EOLIAN static void
_elm_toolbar_item_icon_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                          const char *icon)
{
   Evas_Object *obj;
   Evas_Object *icon_obj;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item);

   obj = WIDGET(item);
   ELM_TOOLBAR_DATA_GET(obj, sd);
   if ((icon) && (item->icon_str) && (!strcmp(icon, item->icon_str))) return;

   icon_obj = elm_icon_add(obj);
   if (!icon_obj) return;
   if (_item_icon_set(icon_obj, "toolbar/", icon))
     _elm_toolbar_item_icon_obj_set
       (obj, item, icon_obj, icon, sd->icon_size, "elm,state,icon_set");
   else
     {
        _elm_toolbar_item_icon_obj_set
          (obj, item, NULL, NULL, 0, "elm,state,icon_set");
        evas_object_del(icon_obj);
     }
}

EOLIAN static const char *
_elm_toolbar_item_icon_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *it)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(it, NULL);

   return it->icon_str;
}

EOLIAN static Evas_Object *
_elm_toolbar_item_object_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, NULL);

   return VIEW(item);
}

EOLIAN static Evas_Object *
_elm_toolbar_item_icon_object_get(Eo *eo_it EINA_UNUSED, Elm_Toolbar_Item_Data *it)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(it, NULL);

   return it->icon;
}

EOLIAN static Eina_Bool
_elm_toolbar_item_icon_memfile_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                                  const void *img,
                                  size_t size,
                                  const char *format,
                                  const char *key)
{
   Evas_Object *icon_obj;
   Evas_Object *obj;
   Eina_Bool ret;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, EINA_FALSE);

   obj = WIDGET(item);
   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (img && size)
     {
        icon_obj = elm_icon_add(obj);
        evas_object_repeat_events_set(icon_obj, EINA_TRUE);
        ret = elm_image_memfile_set(icon_obj, img, size, format, key);
        if (!ret)
          {
             evas_object_del(icon_obj);
             return EINA_FALSE;
          }
        _elm_toolbar_item_icon_obj_set
          (obj, item, icon_obj, NULL, sd->icon_size, "elm,state,icon_set");
     }
   else
     _elm_toolbar_item_icon_obj_set
       (obj, item, NULL, NULL, 0, "elm,state,icon_set");
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_toolbar_item_icon_file_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                                  const char *file,
                                  const char *key)
{
   Evas_Object *icon_obj;
   Evas_Object *obj;
   Eina_Bool ret;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, EINA_FALSE);

   obj = WIDGET(item);
   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (file)
     {
        icon_obj = elm_icon_add(obj);
        evas_object_repeat_events_set(icon_obj, EINA_TRUE);
        ret = elm_image_file_set(icon_obj, file, key);
        if (!ret)
          {
             evas_object_del(icon_obj);
             return EINA_FALSE;
          }
        _elm_toolbar_item_icon_obj_set
          (obj, item, icon_obj, NULL, sd->icon_size, "elm,state,icon_set");
     }
   else
     _elm_toolbar_item_icon_obj_set
       (obj, item, NULL, NULL, 0, "elm,state,icon_set");
   return EINA_TRUE;
}

EOLIAN static void
_elm_toolbar_item_separator_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                               Eina_Bool separator)
{
   Evas_Object *obj = WIDGET(item);
   double scale;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item);
   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (item->separator == separator) return;
   item->separator = separator;
   scale = (elm_widget_scale_get(obj) * elm_config_scale_get());
   _item_theme_hook(obj, item, scale, sd->icon_size);
   evas_object_size_hint_min_set(VIEW(item), -1, -1);
   if (separator) sd->separator_count++;
   else sd->separator_count--;
}

EOLIAN static Eina_Bool
_elm_toolbar_item_separator_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *it)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(it, EINA_FALSE);

   return it->separator;
}

EOLIAN static void
_elm_toolbar_shrink_mode_set(Eo *obj, Elm_Toolbar_Data *sd, Elm_Toolbar_Shrink_Mode shrink_mode)
{
   Eina_Bool bounce;

   if (sd->shrink_mode == shrink_mode) return;
   sd->shrink_mode = shrink_mode;
   bounce = (_elm_config->thumbscroll_bounce_enable) &&
     (shrink_mode == ELM_TOOLBAR_SHRINK_SCROLL);
   eo_do(obj, elm_interface_scrollable_bounce_allow_set(bounce, EINA_FALSE));

   if (sd->more_item)
     {
        eo_do(EO_OBJ(sd->more_item), elm_wdg_item_del());
        sd->more_item = NULL;
     }

   if (shrink_mode == ELM_TOOLBAR_SHRINK_MENU)
     {
        elm_toolbar_homogeneous_set(obj, EINA_FALSE);
        eo_do(obj, elm_interface_scrollable_policy_set
              (ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF));
        sd->more_item = _item_new(obj, "more_menu", "More", NULL, NULL);
        _resizing_eval_item(sd->more_item);
     }
   else if (shrink_mode == ELM_TOOLBAR_SHRINK_HIDE)
     {
        elm_toolbar_homogeneous_set(obj, EINA_FALSE);
        eo_do(obj, elm_interface_scrollable_policy_set
              (ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF));
     }
   else if (shrink_mode == ELM_TOOLBAR_SHRINK_EXPAND)
     {
        elm_toolbar_homogeneous_set(obj, EINA_FALSE);
        eo_do(obj, elm_interface_scrollable_policy_set
              (ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF));
        sd->more_item = _item_new(obj, "more_menu", "More", NULL, NULL);
        _resizing_eval_item(sd->more_item);
     }
   else
      eo_do(obj, elm_interface_scrollable_policy_set
            (ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF));

   evas_object_smart_need_recalculate_set(obj, EINA_TRUE);
}

EOLIAN static Elm_Toolbar_Shrink_Mode
_elm_toolbar_shrink_mode_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->shrink_mode;
}

EAPI void
elm_toolbar_transverse_expanded_set(Evas_Object *obj, Eina_Bool transverse_expanded)
{
   ELM_TOOLBAR_CHECK(obj);
   ELM_TOOLBAR_DATA_GET(obj, sd);

   if (sd->transverse_expanded == transverse_expanded) return;
   sd->transverse_expanded = transverse_expanded;

   _sizing_eval(obj);
}

EAPI Eina_Bool
elm_toolbar_transverse_expanded_get(const Evas_Object *obj)
{
   ELM_TOOLBAR_CHECK(obj) EINA_FALSE;
   ELM_TOOLBAR_DATA_GET(obj, sd);

   return sd->transverse_expanded;
}

EOLIAN static void
_elm_toolbar_homogeneous_set(Eo *obj, Elm_Toolbar_Data *sd, Eina_Bool homogeneous)
{
   homogeneous = !!homogeneous;
   if (homogeneous == sd->homogeneous) return;
   sd->homogeneous = homogeneous;
   if (homogeneous) elm_toolbar_shrink_mode_set(obj, ELM_TOOLBAR_SHRINK_NONE);
   evas_object_smart_calculate(sd->bx);
}

EOLIAN static Eina_Bool
_elm_toolbar_homogeneous_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->homogeneous;
}

EOLIAN static void
_elm_toolbar_menu_parent_set(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd, Evas_Object *parent)
{
   Elm_Toolbar_Item_Data *it;

   EINA_SAFETY_ON_NULL_RETURN(parent);

   sd->menu_parent = parent;
   EINA_INLIST_FOREACH(sd->items, it)
     {
        if (it->o_menu)
          elm_menu_parent_set(it->o_menu, sd->menu_parent);
     }
   if ((sd->more_item) && (sd->more_item->o_menu))
     elm_menu_parent_set(sd->more_item->o_menu, sd->menu_parent);
}

EOLIAN static Evas_Object*
_elm_toolbar_menu_parent_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->menu_parent;
}

EOLIAN static void
_elm_toolbar_align_set(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd, double align)
{
   if (sd->vertical)
     {
        if (sd->align != align)
          evas_object_box_align_set(sd->bx, 0.5, align);
     }
   else
     {
        if (sd->align != align)
          evas_object_box_align_set(sd->bx, align, 0.5);
     }
   sd->align = align;
}

EOLIAN static double
_elm_toolbar_align_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->align;
}

EOLIAN static void
_elm_toolbar_item_menu_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                               Eina_Bool menu)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item);
   ELM_TOOLBAR_DATA_GET(WIDGET(item), sd);

   if (item->menu == menu) return;
   if (menu) _item_menu_create(sd, item);
   else _item_menu_destroy(item);
}

EOLIAN static Evas_Object *
_elm_toolbar_item_menu_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, NULL);

   if (!item->menu) return NULL;
   return item->o_menu;
}

EOLIAN static Elm_Toolbar_Item_State *
_elm_toolbar_item_state_add(Eo *eo_item, Elm_Toolbar_Item_Data *item,
                           const char *icon,
                           const char *label,
                           Evas_Smart_Cb func,
                           const void *data)
{
   Elm_Toolbar_Item_State *it_state;
   Evas_Object *icon_obj;
   Evas_Object *obj;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, NULL);

   obj = WIDGET(item);
   ELM_TOOLBAR_DATA_GET(WIDGET(item), sd);

   if (!item->states)
     {
        it_state = _item_state_new
            (item->label, item->icon_str, item->icon, item->func,
            WIDGET_ITEM_DATA_GET(EO_OBJ(item)));
        item->states = eina_list_append(item->states, it_state);
        item->current_state = item->states;
     }

   icon_obj = elm_icon_add(obj);
   elm_icon_order_lookup_set(icon_obj, sd->lookup_order);
   if (!icon_obj) goto error_state_add;

   if (!_item_icon_set(icon_obj, "toolbar/", icon))
     {
        ELM_SAFE_FREE(icon_obj, evas_object_del);
        icon = NULL;
     }

   it_state = _item_state_new(label, icon, icon_obj, func, data);
   item->states = eina_list_append(item->states, it_state);
   item->func = _elm_toolbar_item_state_cb;
   WIDGET_ITEM_DATA_SET(eo_item, NULL);

   return it_state;

error_state_add:
   if (item->states && !eina_list_next(item->states))
     {
        eina_stringshare_del(item->label);
        eina_stringshare_del(item->icon_str);
        free(eina_list_data_get(item->states));
        item->states = eina_list_free(item->states);
     }
   return NULL;
}

EOLIAN static Eina_Bool
_elm_toolbar_item_state_del(Eo *eo_item, Elm_Toolbar_Item_Data *item,
                           Elm_Toolbar_Item_State *state)
{
   Elm_Toolbar_Item_State *it_state;
   Eina_List *del_state;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, EINA_FALSE);

   if (!state) return EINA_FALSE;

   if (!item->states) return EINA_FALSE;

   del_state = eina_list_data_find_list(item->states, state);
   if (del_state == item->states) return EINA_FALSE;
   if (del_state == item->current_state)
     elm_toolbar_item_state_unset(EO_OBJ(item));

   eina_stringshare_del(state->label);
   eina_stringshare_del(state->icon_str);
   evas_object_del(state->icon);
   free(state);

   item->states = eina_list_remove_list(item->states, del_state);
   if (item->states && !eina_list_next(item->states))
     {
        it_state = eina_list_data_get(item->states);
        WIDGET_ITEM_DATA_SET(eo_item, it_state->data);
        item->func = it_state->func;
        eina_stringshare_del(it_state->label);
        eina_stringshare_del(it_state->icon_str);
        free(eina_list_data_get(item->states));
        item->states = eina_list_free(item->states);
     }

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_toolbar_item_state_set(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                           Elm_Toolbar_Item_State *state)
{
   Elm_Toolbar_Item_State *it_state;
   Eina_List *next_state;
   Evas_Object *obj;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, EINA_FALSE);

   obj = WIDGET(item);
   ELM_TOOLBAR_DATA_GET(obj, sd);
   if (!item->states) return EINA_FALSE;

   if (state)
     {
        next_state = eina_list_data_find_list(item->states, state);
        if (!next_state) return EINA_FALSE;
     }
   else
     next_state = item->states;

   if (next_state == item->current_state) return EINA_TRUE;

   it_state = eina_list_data_get(next_state);
   if (eina_list_data_find(item->current_state, state))
     {
        _item_label_set(item, it_state->label, "elm,state,label_set,forward");
        _elm_toolbar_item_icon_obj_set
          (obj, item, it_state->icon, it_state->icon_str,
          sd->icon_size, "elm,state,icon_set,forward");
     }
   else
     {
        _item_label_set(item, it_state->label, "elm,state,label_set,backward");
        _elm_toolbar_item_icon_obj_set
          (obj, item, it_state->icon, it_state->icon_str,
          sd->icon_size, "elm,state,icon_set,backward");
     }
   if (item->icon)
     {
        Eina_Bool tmp;
        if (eo_do_ret(EO_OBJ(item), tmp, elm_wdg_item_disabled_get()))
          elm_widget_signal_emit(item->icon, "elm,state,disabled", "elm");
        else
          elm_widget_signal_emit(item->icon, "elm,state,enabled", "elm");
     }

   item->current_state = next_state;

   return EINA_TRUE;
}

EAPI void
elm_toolbar_item_state_unset(Elm_Object_Item *it)
{
   eo_do(it, elm_obj_toolbar_item_state_set(NULL));
}

EOLIAN static Elm_Toolbar_Item_State *
_elm_toolbar_item_state_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, NULL);

   if ((!item->states) || (!item->current_state)) return NULL;
   if (item->current_state == item->states) return NULL;

   return eina_list_data_get(item->current_state);
}

EOLIAN static Elm_Toolbar_Item_State *
_elm_toolbar_item_state_next(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   Eina_List *next_state;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, NULL);

   if (!item->states) return NULL;

   next_state = eina_list_next(item->current_state);
   if (!next_state)
     next_state = eina_list_next(item->states);
   return eina_list_data_get(next_state);
}

EOLIAN static Elm_Toolbar_Item_State *
_elm_toolbar_item_state_prev(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   Eina_List *prev_state;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item, NULL);

   if (!item->states) return NULL;

   prev_state = eina_list_prev(item->current_state);
   if ((!prev_state) || (prev_state == item->states))
     prev_state = eina_list_last(item->states);
   return eina_list_data_get(prev_state);
}

EOLIAN static void
_elm_toolbar_icon_order_lookup_set(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd, Elm_Icon_Lookup_Order order)
{
   Elm_Toolbar_Item_Data *it;


   if (sd->lookup_order == order) return;
   sd->lookup_order = order;
   EINA_INLIST_FOREACH(sd->items, it)
     elm_icon_order_lookup_set(it->icon, order);
   if (sd->more_item)
     elm_icon_order_lookup_set(sd->more_item->icon, order);
}

EOLIAN static Elm_Icon_Lookup_Order
_elm_toolbar_icon_order_lookup_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->lookup_order;
}

EOLIAN static void
_elm_toolbar_horizontal_set(Eo *obj, Elm_Toolbar_Data *sd, Eina_Bool horizontal)
{
   horizontal = !!horizontal;
   if (horizontal != sd->vertical) return;
   sd->vertical = !horizontal;
   if (sd->vertical)
     evas_object_box_align_set(sd->bx, 0.5, sd->align);
   else
     evas_object_box_align_set(sd->bx, sd->align, 0.5);

   _sizing_eval(obj);
}

EOLIAN static Eina_Bool
_elm_toolbar_horizontal_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return !sd->vertical;
}

EOLIAN static unsigned int
_elm_toolbar_items_count(const Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->item_count;
}

EOLIAN static void
_elm_toolbar_standard_priority_set(Eo *obj, Elm_Toolbar_Data *sd, int priority)
{
   if (sd->standard_priority == priority) return;
   sd->standard_priority = priority;
   _resizing_eval(obj);
}

EOLIAN static int
_elm_toolbar_standard_priority_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->standard_priority;
}

EOLIAN static void
_elm_toolbar_select_mode_set(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd, Elm_Object_Select_Mode mode)
{
   if (mode >= ELM_OBJECT_SELECT_MODE_MAX)
     return;

   if (sd->select_mode == mode) return;
   sd->select_mode = mode;

   if ((mode == ELM_OBJECT_SELECT_MODE_ALWAYS) &&
       (sd->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS) &&
       sd->items)
     _item_select(ELM_TOOLBAR_ITEM_FROM_INLIST(sd->items));

}

EOLIAN static Elm_Object_Select_Mode
_elm_toolbar_select_mode_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->select_mode;
}

EAPI void
elm_toolbar_reorder_mode_set(Evas_Object *obj,
                             Eina_Bool    reorder_mode)
{
   ELM_TOOLBAR_CHECK(obj);
   ELM_TOOLBAR_DATA_GET(obj, sd);

   sd->reorder_mode = !!reorder_mode;
}

EAPI Eina_Bool
elm_toolbar_reorder_mode_get(const Evas_Object *obj)
{
   ELM_TOOLBAR_CHECK(obj) EINA_FALSE;
   ELM_TOOLBAR_DATA_GET(obj, sd);

   return sd->reorder_mode;
}

EOLIAN static void
_elm_toolbar_item_show(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                               Elm_Toolbar_Item_Scrollto_Type type)
{
   Evas_Coord x, y, w, h;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item);

   if (_elm_toolbar_item_coordinates_calc(item, type, &x, &y, &w, &h))
     eo_do(WIDGET(item), elm_interface_scrollable_content_region_show
     (x, y, w, h));
}

EOLIAN static void
_elm_toolbar_item_bring_in(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item,
                               Elm_Toolbar_Item_Scrollto_Type type)
{
   Evas_Coord x, y, w, h;

   ELM_TOOLBAR_ITEM_CHECK_OR_RETURN(item);

   if (_elm_toolbar_item_coordinates_calc(item, type, &x, &y, &w, &h))
     eo_do(WIDGET(item), elm_interface_scrollable_region_bring_in
     (x, y, w, h));
}

EOLIAN static char*
_elm_toolbar_item_elm_interface_atspi_accessible_name_get(Eo *eo_item EINA_UNUSED, Elm_Toolbar_Item_Data *item)
{
   return item->label ? strdup(item->label) : NULL;
}

EOLIAN static Elm_Atspi_State_Set
_elm_toolbar_item_elm_interface_atspi_accessible_state_set_get(Eo *eo_it, Elm_Toolbar_Item_Data *item EINA_UNUSED)
{
   Elm_Atspi_State_Set ret;
   Eina_Bool sel;

   eo_do_super(eo_it, ELM_TOOLBAR_ITEM_CLASS, ret = elm_interface_atspi_accessible_state_set_get());

   eo_do(eo_it, sel = elm_obj_toolbar_item_selected_get());

   STATE_TYPE_SET(ret, ELM_ATSPI_STATE_SELECTABLE);

   if (sel)
      STATE_TYPE_SET(ret, ELM_ATSPI_STATE_SELECTED);

   return ret;
}

EOLIAN static Elm_Object_Item *
_elm_toolbar_elm_widget_focused_item_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   return sd->focused_item;
}

EOLIAN static void
_elm_toolbar_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EOLIAN static const Elm_Atspi_Action*
_elm_toolbar_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "select", "select", NULL, _key_action_select},
          { "move,left", "move", "left", _key_action_move},
          { "move,right", "move", "right", _key_action_move},
          { "move,up", "move", "up", _key_action_move},
          { "move,down", "move", "down", _key_action_move},
          { NULL, NULL, NULL, NULL }
   };
   return &atspi_actions[0];
}

EOLIAN static Eina_List*
_elm_toolbar_elm_interface_atspi_accessible_children_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *sd)
{
   Eina_List *ret = NULL;
   Elm_Toolbar_Item_Data *it;

   EINA_INLIST_FOREACH(sd->items, it)
      ret = eina_list_append(ret, EO_OBJ(it));

   return ret;
}

EOLIAN static Elm_Atspi_State_Set
_elm_toolbar_elm_interface_atspi_accessible_state_set_get(Eo *obj, Elm_Toolbar_Data *sd EINA_UNUSED)
{
   Elm_Atspi_State_Set ret;

   eo_do_super(obj, ELM_TOOLBAR_CLASS, ret = elm_interface_atspi_accessible_state_set_get());

   STATE_TYPE_SET(ret, ELM_ATSPI_STATE_MANAGES_DESCENDANTS);

   return ret;
}

EOLIAN int
_elm_toolbar_elm_interface_atspi_selection_selected_children_count_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd)
{
   return pd->selected_item ? 1 : 0;
}

EOLIAN Eo*
_elm_toolbar_elm_interface_atspi_selection_selected_child_get(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd, int child_idx)
{
   if (child_idx != 0)
     return NULL;

   return pd->selected_item;
}

EOLIAN Eina_Bool
_elm_toolbar_elm_interface_atspi_selection_child_select(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd, int child_index)
{
   Elm_Toolbar_Item_Data *item;
   if (pd->select_mode != ELM_OBJECT_SELECT_MODE_NONE)
     {
        EINA_INLIST_FOREACH(pd->items, item)
          {
             if (child_index-- == 0)
               {
                  elm_toolbar_item_selected_set(EO_OBJ(item), EINA_TRUE);
                  return EINA_TRUE;
               }
          }
     }
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_elm_toolbar_elm_interface_atspi_selection_selected_child_deselect(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd, int child_index)
{
   if (child_index != 0)
     return EINA_FALSE;

   if (!pd->selected_item)
     return EINA_FALSE;

   elm_toolbar_item_selected_set(pd->selected_item, EINA_FALSE);

   return EINA_TRUE;
}

EOLIAN Eina_Bool
_elm_toolbar_elm_interface_atspi_selection_is_child_selected(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd, int child_index)
{
   Elm_Toolbar_Item_Data *item;

   EINA_INLIST_FOREACH(pd->items, item)
     {
        if (child_index-- == 0)
          {
             return elm_toolbar_item_selected_get(EO_OBJ(item));
          }
     }
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_elm_toolbar_elm_interface_atspi_selection_all_children_select(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd EINA_UNUSED)
{
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_elm_toolbar_elm_interface_atspi_selection_clear(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd)
{
   if (pd->selected_item)
     elm_toolbar_item_selected_set(pd->selected_item, EINA_FALSE);
   return EINA_TRUE;
}

EOLIAN Eina_Bool
_elm_toolbar_elm_interface_atspi_selection_child_deselect(Eo *obj EINA_UNUSED, Elm_Toolbar_Data *pd, int child_index)
{
   Elm_Toolbar_Item_Data *item;
   if (pd->select_mode != ELM_OBJECT_SELECT_MODE_NONE)
     {
        EINA_INLIST_FOREACH(pd->items, item)
          {
             if (child_index-- == 0)
               {
                  elm_toolbar_item_selected_set(EO_OBJ(item), EINA_FALSE);
                  return EINA_TRUE;
               }
          }
     }
   return EINA_FALSE;
}

EOLIAN void
_elm_toolbar_evas_object_smart_calculate(Eo *obj, Elm_Toolbar_Data *pd EINA_UNUSED)
{
   _sizing_eval(obj);
}


#include "elm_toolbar.eo.c"
#include "elm_toolbar_item.eo.c"
