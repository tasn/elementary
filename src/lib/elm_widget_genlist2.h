#ifndef ELM_WIDGET_GENLIST_H
#define ELM_WIDGET_GENLIST_H

#include "elm_gen_common.h"
#include "elm_interface_scrollable.h"
#include "elm_widget_layout.h"

// FIXME: Difference from upstream
#define GENLIST_ENTRY_SUPPORT 1
#define GENLIST_FX_SUPPORT 1
#define MAX_ITEMS_PER_VIEWPORT 12

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
 * @def ELM_GENLIST_CLASS
 *
 * Use this macro to cast whichever subclass of
 * #Elm_Genlist_Smart_Class into it, so to access its fields.
 *
 * @ingroup Widget
 */
#define ELM_GENLIST_CLASS(x) ((Elm_Genlist_Smart_Class *)x)

/**
 * @def ELM_GENLIST_DATA
 *
 * Use this macro to cast whichever subdata of
 * #Elm_Genlist_Smart_Data into it, so to access its fields.
 *
 * @ingroup Widget
 */
#define ELM_GENLIST_DATA(x)  ((Elm_Genlist_Smart_Data *)x)

/**
 * @def ELM_GENLIST_SMART_CLASS_VERSION
 *
 * Current version for Elementary genlist @b base smart class, a value
 * which goes to _Elm_Genlist_Smart_Class::version.
 *
 * @ingroup Widget
 */
#define ELM_GENLIST_SMART_CLASS_VERSION 1

/**
 * @def ELM_GENLIST_SMART_CLASS_INIT
 *
 * Initializer for a whole #Elm_Genlist_Smart_Class structure, with
 * @c NULL values on its specific fields.
 *
 * @param smart_class_init initializer to use for the "base" field
 * (#Evas_Smart_Class).
 *
 * @see EVAS_SMART_CLASS_INIT_NULL
 * @see EVAS_SMART_CLASS_INIT_NAME_VERSION
 * @see ELM_GENLIST_SMART_CLASS_INIT_NULL
 * @see ELM_GENLIST_SMART_CLASS_INIT_NAME_VERSION
 *
 * @ingroup Widget
 */
#define ELM_GENLIST_SMART_CLASS_INIT(smart_class_init) \
  {smart_class_init, ELM_GENLIST_SMART_CLASS_VERSION}

/**
 * @def ELM_GENLIST_SMART_CLASS_INIT_NULL
 *
 * Initializer to zero out a whole #Elm_Genlist_Smart_Class structure.
 *
 * @see ELM_GENLIST_SMART_CLASS_INIT_NAME_VERSION
 * @see ELM_GENLIST_SMART_CLASS_INIT
 *
 * @ingroup Widget
 */
#define ELM_GENLIST_SMART_CLASS_INIT_NULL \
  ELM_GENLIST_SMART_CLASS_INIT(EVAS_SMART_CLASS_INIT_NULL)

/**
 * @def ELM_GENLIST_SMART_CLASS_INIT_NAME_VERSION
 *
 * Initializer to zero out a whole #Elm_Genlist_Smart_Class structure and
 * set its name and version.
 *
 * This is similar to #ELM_GENLIST_SMART_CLASS_INIT_NULL, but it will
 * also set the version field of #Elm_Genlist_Smart_Class (base field)
 * to the latest #ELM_GENLIST_SMART_CLASS_VERSION and name it to the
 * specific value.
 *
 * It will keep a reference to the name field as a <c>"const char *"</c>,
 * i.e., the name must be available while the structure is
 * used (hint: static or global variable!) and must not be modified.
 *
 * @see ELM_GENLIST_SMART_CLASS_INIT_NULL
 * @see ELM_GENLIST_SMART_CLASS_INIT
 *
 * @ingroup Widget
 */
#define ELM_GENLIST_SMART_CLASS_INIT_NAME_VERSION(name) \
  ELM_GENLIST_SMART_CLASS_INIT                          \
    (ELM_LAYOUT_SMART_CLASS_INIT_NAME_VERSION(name))

/**
 * Elementary genlist base smart class. This inherits directly from
 * #Elm_Layout_Smart_Class and is meant to build widgets extending the
 * behavior of a genlist.
 *
 * All of the functions listed on @ref Genlist namespace will work for
 * objects deriving from #Elm_Genlist_Smart_Class.
 */
typedef struct _Elm_Genlist_Smart_Class
{
   Elm_Layout_Smart_Class base;

   int                    version;    /**< Version of this smart class definition */
} Elm_Genlist_Smart_Class;

/**
 * Base widget smart data extended with genlist instance data.
 */
typedef struct _Elm_Genlist_Smart_Data Elm_Genlist_Smart_Data;
typedef struct _Item_Block Item_Block;
typedef struct _Item_Cache Item_Cache;


struct _Elm_Genlist_Smart_Data
{
   Elm_Layout_Smart_Data                 base; /* base widget smart data as
                                                * first member obligatory, as
                                                * we're inheriting from it */

   const Elm_Scrollable_Smart_Interface *s_iface;

   Eina_Inlist_Sorted_State             *state;
   Evas_Object                          *hit_rect;
   Evas_Object                          *pan_obj;

   Evas_Object                          *focused_content;
   Elm_Gen_Item                         *focused_item;
   Eina_List                            *selected; /* a list of
                                                    * selected
                                                    * items */
   Eina_List                            *group_items; /* a list of
                                                       * groups index
                                                       * items */
   Eina_Inlist                          *items; /* an inlist of all items */
   struct
     {
        Elm_Gen_Item *it;
        int dir;
        Eina_List *move_items;
        Ecore_Animator *anim;
     } reorder;
   Elm_Object_Item                      *last_selected_item;
   int                                   minw, minh;
   Eina_Bool                             scr_minw : 1;
   Eina_Bool                             scr_minh : 1;
   unsigned int                          item_count;
   Evas_Coord                            pan_x, pan_y;
   Elm_Object_Select_Mode                select_mode;

   Evas_Coord                            comp_y;
   int                                   dir;
   Eina_List                            *blocks_realized;
   Eina_Inlist                          *blocks; /* an inlist of all
                                                  * blocks. a block
                                                  * consists of a
                                                  * certain number of
                                                  * items. maximum
                                                  * number of items in
                                                  * a block is
                                                  * 'max_items_per_block'. */
   Evas_Coord                            prev_viewport_w;
   Ecore_Idle_Enterer                   *queue_idle_enterer;
   Eina_List                            *queue;
   Elm_Gen_Item                         *show_item, *mode_item, *expanded_item;
   Eina_Inlist                          *item_cache; /* an inlist of
                                                      * edje object it
                                                      * cache. */
   Elm_List_Mode                         mode;
   Ecore_Timer                          *scr_hold_timer;
   const char                           *decorate_it_type;
   double                                start_time;
   Evas_Coord                            prev_x, prev_y, prev_mx, prev_my;
   Evas_Coord                            cur_x, cur_y, cur_mx, cur_my;

   int                                   item_cache_count;
   int                                   item_cache_max; /* maximum
                                                         * number of
                                                         * cached
                                                         * items */
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

   Evas_Object                          *g_layer;
   Elm_Gen_Item                         *g_item;
   const char                           *g_type;
   Eina_Bool                             select_on_focus_enabled : 1;
   Eina_Bool                             decorate_all_mode : 1;
   Eina_Bool                             reorder_mode : 1; /* a flag
                                                            * for
                                                            * reorder
                                                            * mode
                                                            * enable/disable */
   Eina_Bool                             reorder_force : 1; /* a flag
                                                            * for
                                                            * reorder
                                                            * by force
   /* this flag means genlist is supposed to be scrolled. if this flag
    * is set to EINA_TRUE, genlist checks whether it's ok to scroll
    * genlist now or not. */
   Eina_Bool                             homogeneous : 1;
   Eina_Bool                             realization_mode : 1;
   Eina_Hash                             *size_caches;

   Eina_Bool                             on_sub_del : 1;

   /* a flag for items can be highlighted or not. by default this flag
    * is true. */
   Eina_Bool                             highlight : 1;
   Eina_Bool                             h_bounce : 1;
   Eina_Bool                             v_bounce : 1;
   Eina_Bool                             bring_in : 1; /* a flag to
                                                        * describe the
                                                        * scroll
                                                        * animation. (show,
                                                        * bring in) */

   /* this is set to EINA_TRUE when the item is re-queued. this
    * happens when the item is un-queued but the rel item is still in
    * the queue. this item will be processed later. */
   Eina_Bool                             requeued : 1;
   Eina_Bool                             multi : 1; /* a flag for item
                                                     * multi
                                                     * selection */
   Eina_Bool                             sorting : 1;
   Evas_Coord                            finger_minw, finger_minh;
   int                                   processed_sizes;
   Eina_Bool                             calc_done : 1;
   Eina_Bool                             was_selected : 1;
   Ecore_Job                             *dummy_job;
   Ecore_Timer                           *scr_timer;
   Eina_Bool                             no_cache : 1;
   Elm_Gen_Item                          *key_down_item;
   Eina_Bool                             on_hold : 1; /* a flag for check
                                                       * item selected or
                                                       * not */

   /* this is genlist item Fx effect on Add/Remove */
   Eina_Bool                             fx_mode : 1;
   struct
     {
        Eina_List                        *items;
        Eina_List                        *pending_items;
        Ecore_Animator                   *anim;
        int                              cnt;
     } del_fx;
   struct
     {
        Eina_List                        *items;
        Ecore_Animator                   *anim;
        int                              cnt;
     } add_fx;

#ifndef ELM_FEATURE_WEARABLE
  /*for banded ux*/
   int                                   item_color[MAX_ITEMS_PER_VIEWPORT][3];
   Elm_Gen_Item                          *top_drawn_item;
   Eina_Bool                             color_calculated : 1;
#endif
};

struct Elm_Gen_Item_Type
{
   Elm_Gen_Item           *it;

   Elm_Genlist_Smart_Data *wsd;

   Item_Block             *block;
   Eina_List              *items;
   Evas_Coord              w, h, minw, minh;
   Elm_Gen_Item           *group_item;
   Elm_Genlist_Item_Type   type;
   Eina_List              *deco_it_contents;
   Eina_List              *deco_all_contents;
   Eina_List              *flip_content_objs;
   Evas_Coord              scrl_x, scrl_y;

   Elm_Gen_Item           *rel;
   Eina_List              *rel_revs; // FIXME: find better way not to use this
   Evas_Object            *deco_it_view;
   int                     expanded_depth;

   Eina_Bool               expanded : 1;
   Eina_Bool               calc_done : 1;
   Eina_Bool               updateme : 1;
   Eina_Bool               queued : 1;
   Eina_Bool               before : 1;
   Eina_Bool               is_prepend : 1;
   /* if item is realized once, it is not unrealized & realized again. */
   Eina_Bool               multiline;
   Ecore_Timer             *highlight_timer;
   Evas_Coord              reorder_offset;

   /* Redwood Only : this pointer save bg part address of item view */
   Evas_Object             *bg_part;
};

struct _Item_Block
{
   EINA_INLIST;

   int                     count;
   Elm_Genlist_Smart_Data *sd;
   Eina_List              *items;
   Evas_Coord              x, y;
   Evas_Coord              minw, minh;
   int                     position;
   int                     item_position_stamp;

   Eina_Bool               position_update : 1;
   Eina_Bool               realized : 1;
   Eina_Bool               calc_done : 1;
};

struct _Item_Cache
{
   EINA_INLIST;

   Evas_Object *base_view;
   const char  *item_style; // it->itc->item_style
   Eina_Bool    multiline : 1;
};

typedef struct _Elm_Genlist_Pan_Smart_Class
{
   Elm_Pan_Smart_Class base;
} Elm_Genlist_Pan_Smart_Class;

typedef struct _Elm_Genlist_Pan_Smart_Data Elm_Genlist_Pan_Smart_Data;
struct _Elm_Genlist_Pan_Smart_Data
{
   Elm_Pan_Smart_Data      base;
   Elm_Genlist_Smart_Data *wsd;
   Ecore_Job              *resize_job;
};

/**
 * @}
 */

EAPI extern const char ELM_GENLIST_SMART_NAME[];
EAPI const Elm_Genlist_Smart_Class *elm_genlist_smart_class_get(void);

#define ELM_GENLIST_DATA_GET(o, sd) \
  Elm_Genlist_Smart_Data *sd = evas_object_smart_data_get(o)

#define ELM_GENLIST_PAN_DATA_GET(o, sd) \
  Elm_Genlist_Pan_Smart_Data * sd = evas_object_smart_data_get(o)

#define ELM_GENLIST_CHECK_OR_RETURN(obj, ...)                         \
   do                                                                 \
     {                                                                \
        if (!obj ||                                                   \
            !elm_widget_type_check((obj),                             \
                                   ELM_GENLIST_SMART_NAME, __func__)) \
          {                                                           \
             ERR("obj (%p) is NULL or type is not correct", obj);     \
             return __VA_ARGS__;                                      \
          }                                                           \
     }                                                                \
   while (0)

#define ELM_GENLIST_ITEM_CHECK_OR_RETURN(it, ...)                        \
  ELM_WIDGET_ITEM_CHECK_OR_RETURN(((Elm_Widget_Item *)it), __VA_ARGS__); \
  ELM_WIDGET_ITEM_RETURN_IF_ONDEL(((Elm_Widget_Item *)it), __VA_ARGS__); \
  ELM_GENLIST_CHECK_OR_RETURN(it->base.widget, __VA_ARGS__);

#endif
