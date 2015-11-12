#ifndef ELM_WIDGET_GENLIST_H
#define ELM_WIDGET_GENLIST_H

#include "elm_gen_common.h"
#include "Elementary.h"

/* DO NOT USE THIS HEADER UNLESS YOU ARE PREPARED FOR BREAKING OF YOUR
 * CODE. THIS IS ELEMENTARY'S INTERNAL WIDGET API (for now) AND IS NOT
 * FINAL. CALL elm_widget_api_check(ELM_INTERNAL_API_VERSION) TO CHECK
 * IT AT RUNTIME.
 */

/**
 * @addtogroup Widget
 * @{
 *
 * @section elm-genlist-class The Elementary Genlist Class
 *
 * Elementary, besides having the @ref Genlist widget, exposes its
 * foundation -- the Elementary Genlist Class -- in order to create
 * other widgets which are a genlist with some more logic on top.
 */

/**
 * Base widget smart data extended with genlist instance data.
 */
typedef struct _Elm_Genlist_Data Elm_Genlist_Data;

typedef enum
{
   ELM_GENLIST_TREE_EFFECT_NONE = 0,
   ELM_GENLIST_TREE_EFFECT_EXPAND = 1,
   ELM_GENLIST_TREE_EFFECT_CONTRACT = 2
} Elm_Genlist_Item_Move_Effect_Mode;

struct _Elm_Genlist_Data
{
   Eina_Inlist_Sorted_State             *state;
   Evas_Object                          *hit_rect;
   Evas_Object                          *pan_obj;
   Evas_Object                          *stack[2]; // stacking markers in pan
   Evas_Object                          *obj; // the object itself

   Eina_List                            *selected; /* a list of
                                                    * selected
                                                    * items */
   Eina_List                            *deselecting; /* a list of items currently being deselected */
   Eina_List                            *group_items; /* a list of
                                                       * groups index
                                                       * items */
   Eina_Inlist                          *items; /* an inlist of all items */
   Elm_Gen_Item                         *reorder_it; /* item currently
                                                     * being
                                                     * repositioned */
   Elm_Object_Item                      *last_selected_item;
   Elm_Object_Item                      *focused_item; /**< a focused item by keypad arrow or mouse. This is set to NULL if widget looses focus. */
   Elm_Object_Item                      *last_focused_item; /**< This records the last focused item when widget looses focus. This is required to set the focus on last focused item when widgets gets focus. */
   Ecore_Job                            *calc_job;
   int                                   walking;
   int                                   item_width, item_height;
   int                                   group_item_width, group_item_height;
   int                                   minw, minh;
   Eina_Bool                             scr_minw : 1; /* a flag for determining
                                                        * minimum width to limit
                                                        * as their content size */
   Eina_Bool                             scr_minh : 1; /* a flag for determining
                                                        * minimum height to limit
                                                        * as their content size */
   unsigned int                          item_count;
   Evas_Coord                            pan_x, pan_y;
   Elm_Object_Select_Mode                select_mode;
   Elm_Object_Multi_Select_Mode          multi_select_mode; /**< select mode for multiple selection */

   Eina_Inlist                          *blocks; /* an inlist of all
                                                  * blocks. a block
                                                  * consists of a
                                                  * certain number of
                                                  * items. maximum
                                                  * number of items in
                                                  * a block is
                                                  * 'max_items_per_block'. */
   Evas_Coord                            reorder_old_pan_y, w, h, realminw;
   Evas_Coord                            prev_viewport_w; /* previous scrollable
                                                           * interface's
                                                           * viewport size.
                                                           * This is used only
                                                           * when genlist is in
                                                           * a compress mode. */
   Ecore_Job                            *update_job;
   Ecore_Idle_Enterer                   *queue_idle_enterer;
   Ecore_Idler                          *must_recalc_idler;
   Eina_List                            *queue;
   Elm_Gen_Item                         *show_item, *anchor_item, *mode_item,
                                        *reorder_rel, *expanded_item;
   Eina_Inlist                          *item_cache; /* an inlist of
                                                      * edje object it
                                                      * cache. */
   Evas_Coord                            anchor_y;
   Evas_Coord                            reorder_start_y; /* reorder
                                                           * it's
                                                           * initial y
                                                           * coordinate
                                                           * in the
                                                           * pan. */
   Elm_List_Mode                         mode;
   Ecore_Timer                          *multi_timer, *scr_hold_timer;
   Ecore_Animator                       *reorder_move_animator;
   const char                           *decorate_it_type;
   double                                start_time;
   Evas_Coord                            prev_x, prev_y, prev_mx, prev_my;
   Evas_Coord                            cur_x, cur_y, cur_mx, cur_my;

   struct
   {
      Evas_Coord x, y;
   } history[SWIPE_MOVES];

   int                                   multi_device;
   int                                   item_cache_count;

   /* maximum number of cached items. (max_items_per_block * 2) */
   int                                   item_cache_max;
   int                                   movements;

   /* maximum number of items per block */
   int                                   max_items_per_block;

   /* longpress timeout. this value comes from _elm_config by
    * default. this can be changed by
    * elm_genlist_longpress_timeout_set() */
   double                                longpress_timeout;
   Eina_Compare_Cb                       item_compare_cb;
   Eina_Compare_Cb                       item_compare_data_cb;

   /* a scrollto type which remembers where to scroll ex) in, top,
    * middle */
   Elm_Genlist_Item_Scrollto_Type        scroll_to_type;
   Evas_Object                          *event_block_rect; /**< This object blocks the event in some cases. For example, when the tree effect is running and not finished, this object blocks events to the genlist. */
   Eina_List                            *move_items; /* items move for
                                                      * tree effect */
   Elm_Gen_Item                         *expanded_next_item;
   Ecore_Animator                       *tree_effect_animator;
   Elm_Genlist_Item_Move_Effect_Mode     move_effect_mode;
   int                                   reorder_fast;

   Eina_List                            *filter_queue;
   Eina_List                            *filtered_list;
   void                                 *filter_data;
   unsigned int                          processed_count;
   unsigned int                          filtered_count;
   Ecore_Idle_Enterer                   *queue_filter_enterer;
   Eina_Bool                             filter;

   Eina_Bool                             focus_on_selection_enabled : 1;
   Eina_Bool                             tree_effect_enabled : 1;
   Eina_Bool                             auto_scroll_enabled : 1;
   Eina_Bool                             decorate_all_mode : 1;
   Eina_Bool                             height_for_width : 1;
   Eina_Bool                             reorder_pan_move : 1;
   Eina_Bool                             multi_timeout : 1;
   Eina_Bool                             multi_touched : 1;
   Eina_Bool                             reorder_mode : 1; /* a flag
                                                            * for
                                                            * reorder
                                                            * mode
                                                            * enable/disable */
   /* this flag means genlist is supposed to be scrolled. if this flag
    * is set to @c EINA_TRUE, genlist checks whether it's ok to scroll
    * genlist now or not. */
   Eina_Bool                             check_scroll : 1;
   Eina_Bool                             pan_changed : 1;
   Eina_Bool                             wasselected : 1;
   Eina_Bool                             homogeneous : 1;
   Eina_Bool                             longpressed : 1;
   /* a flag for items can be highlighted or not. by default this flag
    * is true. */
   Eina_Bool                             mouse_down : 1;
   Eina_Bool                             multi_down : 1;
   Eina_Bool                             on_sub_del : 1;

   Eina_Bool                             highlight : 1;
   Eina_Bool                             h_bounce : 1;
   Eina_Bool                             v_bounce : 1;
   Eina_Bool                             bring_in : 1; /* a flag to
                                                        * describe the
                                                        * scroll
                                                        * animation. (show,
                                                        * bring in) */

   /* this is set to @c EINA_TRUE when the item is re-queued. this
    * happens when the item is un-queued but the rel item is still in
    * the queue. this item will be processed later. */
   Eina_Bool                             requeued : 1;
   Eina_Bool                             on_hold : 1;
   Eina_Bool                             multi : 1; /* a flag for item
                                                     * multi
                                                     * selection */

   Eina_Bool                             swipe : 1;
   /**< value whether item loop feature is enabled or not. */
   Eina_Bool                             item_loop_enable : 1;
   Eina_Bool                             item_looping_on : 1;
};

typedef struct _Item_Block Item_Block;
typedef struct _Item_Cache Item_Cache;

struct Elm_Gen_Item_Type
{
   Elm_Gen_Item           *it;

   Elm_Genlist_Data       *wsd;

   Item_Block             *block;
   Eina_List              *items;
   Evas_Coord              w, h, minw, minh;
   Elm_Gen_Item           *group_item;
   Elm_Genlist_Item_Type   type;
   Eina_List              *deco_it_texts, *deco_it_contents;
   Eina_List              *deco_all_texts, *deco_all_contents;
   Eina_List              *flip_contents;
   Ecore_Timer            *swipe_timer;
   Evas_Coord              scrl_x, scrl_y, old_scrl_y;

   Elm_Gen_Item           *rel;
   Eina_List              *rel_revs; // FIXME: find better way not to use this
   Evas_Object            *deco_it_view;
   int                     expanded_depth;
   int                     order_num_in;

   Eina_Bool               decorate_all_item_realized : 1;
   Eina_Bool               tree_effect_finished : 1; /* tree effect */
   Eina_Bool               move_effect_enabled : 1;
   Eina_Bool               tree_effect_hide_me : 1; /* item hide for
                                                    * tree effect */

   Eina_Bool               stacking_even : 1;
   Eina_Bool               want_realize : 1;
   Eina_Bool               nocache_once : 1; /* do not use cache for
                                              * this item only once */
   Eina_Bool               nostacking : 1;
   Eina_Bool               expanded : 1;
   Eina_Bool               mincalcd : 1;
   Eina_Bool               updateme : 1;
   Eina_Bool               nocache : 1; /* do not use cache for this item */
   Eina_Bool               queued : 1;
   Eina_Bool               before : 1;
   Eina_Bool               show_me : 1;
};

struct _Item_Block
{
   EINA_INLIST;

   int                     count;
   int                     num;
   int                     reorder_offset;
   Elm_Genlist_Data       *sd;
   Eina_List              *items;
   Evas_Coord              x, y, w, h, minw, minh;
   int                     position;
   int                     item_position_stamp;

   Eina_Bool               position_update : 1;
   Eina_Bool               want_unrealize : 1;
   Eina_Bool               must_recalc : 1;
   Eina_Bool               realized : 1;
   Eina_Bool               updateme : 1;
   Eina_Bool               changed : 1;
   Eina_Bool               show_me : 1;
};

struct _Item_Cache
{
   EINA_INLIST;

   Evas_Object *base_view, *spacer;
   const char  *item_style; // it->itc->item_style
   Eina_Bool    tree : 1; // it->group
};

typedef struct _Elm_Genlist_Pan_Data Elm_Genlist_Pan_Data;
struct _Elm_Genlist_Pan_Data
{
   Evas_Object            *wobj;
   Elm_Genlist_Data       *wsd;
   Ecore_Job              *resize_job;
};

/**
 * Structure added to genlist for internal filter iterator implementation
 * Can be extended to genlist as a whole in future if needed.
 */
typedef struct _Elm_Genlist_Filter Elm_Genlist_Filter;
struct _Elm_Genlist_Filter
{
   Eina_Iterator iterator;
   const Eina_Inlist *head;
   const Eina_Inlist *current;
};

#define ELM_GENLIST_FILTER_ITERATOR_ITEM_GET(ptr,                 \
                                  type) ((type *)((char *)ptr - \
                                                  offsetof(type, __in_list)))


/**
 * @}
 */

#define GL_IT(_it) (_it->item)

#define ELM_GENLIST_DATA_GET(o, sd) \
  Elm_Genlist_Data * sd = eo_data_scope_get(o, ELM_GENLIST_CLASS)

#define ELM_GENLIST_DATA_GET_FROM_ITEM(it, sd) \
  Elm_Genlist_Data * sd = GL_IT(it)->wsd

#define ELM_GENLIST_PAN_DATA_GET(o, sd) \
  Elm_Genlist_Pan_Data * sd = eo_data_scope_get(o, ELM_GENLIST_PAN_CLASS)

#define ELM_GENLIST_DATA_GET_OR_RETURN(o, ptr)       \
  ELM_GENLIST_DATA_GET(o, ptr);                      \
  if (EINA_UNLIKELY(!ptr))                           \
    {                                                \
       CRI("No widget data for object %p (%s)",      \
           o, evas_object_type_get(o));              \
       return;                                       \
    }

#define ELM_GENLIST_DATA_GET_OR_RETURN_VAL(o, ptr, val) \
  ELM_GENLIST_DATA_GET(o, ptr);                         \
  if (EINA_UNLIKELY(!ptr))                              \
    {                                                   \
       CRI("No widget data for object %p (%s)",         \
           o, evas_object_type_get(o));                 \
       return val;                                      \
    }

#define ELM_GENLIST_CHECK(obj)                              \
  if (EINA_UNLIKELY(!eo_isa((obj), ELM_GENLIST_CLASS))) \
    return

#define ELM_GENLIST_ITEM_CHECK(it)                          \
  ELM_WIDGET_ITEM_CHECK_OR_RETURN(it->base, ); \
  ELM_GENLIST_CHECK(it->base->widget);

#define ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, ...)                      \
  ELM_WIDGET_ITEM_CHECK_OR_RETURN(it->base, __VA_ARGS__); \
  ELM_GENLIST_CHECK(it->base->widget) __VA_ARGS__;

#define ELM_GENLIST_ITEM_CHECK_OR_GOTO(it, label)              \
  ELM_WIDGET_ITEM_CHECK_OR_GOTO(it->base, label); \
  if (!it->base->widget || !eo_isa                              \
        ((it->base->widget), ELM_GENLIST_CLASS)) goto label;

#define ELM_GENLIST_ITEM_DATA_GET(o, sd) \
  Elm_Gen_Item* sd = eo_data_scope_get(o, ELM_GENLIST_ITEM_CLASS)

#endif
