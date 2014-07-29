#ifndef ELM_WIDGET_SEGMENT_CONTROL_H
#define ELM_WIDGET_SEGMENT_CONTROL_H

#include "Elementary.h"
#include "elm_segment_control_item.eo.h"

#include "elm_object_item_migration_temp.h"

/**
 * @addtogroup Widget
 * @{
 *
 * @section elm-segment-control-class The Elementary Segment control Class
 *
 * Elementary, besides having the @ref SegmentControl widget, exposes
 * its foundation -- the Elementary Segment control Class -- in order
 * to create other widgets which are a segment control with some more
 * logic on top.
 */
/**
 * Base layout smart data extended with segment control instance data.
 */
typedef struct _Elm_Segment_Control_Data       Elm_Segment_Control_Data;
typedef struct _Elm_Segment_Control_Item_Data          Elm_Segment_Control_Item_Data;

struct _Elm_Segment_Control_Data
{
   Evas_Object          *obj;
   Eina_List            *items;
   Elm_Widobj_Item      *selected_item;

   int                   item_width;
};

struct _Elm_Segment_Control_Item_Data
{
   Elm_Widget_Item_Data *base;

   Evas_Object *icon;
   const char  *label;
   int          seg_index;
};

/**
 * @}
 */

#define ELM_SEGMENT_CONTROL_DATA_GET(o, sd) \
  Elm_Segment_Control_Data * sd = eo_data_scope_get(o, ELM_SEGMENT_CONTROL_CLASS)

#define ELM_SEGMENT_CONTROL_DATA_GET_OR_RETURN(o, ptr) \
  ELM_SEGMENT_CONTROL_DATA_GET(o, ptr);                \
  if (EINA_UNLIKELY(!ptr))                             \
    {                                                  \
       CRI("No widget data for object %p (%s)",        \
           o, evas_object_type_get(o));                \
       return;                                         \
    }

#define ELM_SEGMENT_CONTROL_DATA_GET_OR_RETURN_VAL(o, ptr, val) \
  ELM_SEGMENT_CONTROL_DATA_GET(o, ptr);                         \
  if (EINA_UNLIKELY(!ptr))                                      \
    {                                                           \
       CRI("No widget data for object %p (%s)",                 \
           o, evas_object_type_get(o));                         \
       return val;                                              \
    }

#define ELM_SEGMENT_CONTROL_CHECK(obj)                          \
  if (EINA_UNLIKELY(!eo_isa((obj), ELM_SEGMENT_CONTROL_CLASS))) \
    return

#define ELM_SEGMENT_ITEM_DATA_GET(o, sd) \
  Elm_Segment_Control_Item_Data * sd = eo_data_scope_get(o, ELM_SEGMENT_CONTROL_ITEM_CLASS)

#define ELM_SEGMENT_CONTROL_ITEM_CHECK(obj)                  \
 if (EINA_UNLIKELY(!eo_isa((obj), ELM_SEGMENT_CONTROL_ITEM_CLASS)))   \
    return

#define ELM_SEGMENT_CONTROL_ITEM_CHECK_OR_RETURN(obj, ...)    \
 if (EINA_UNLIKELY(!eo_isa((obj), ELM_SEGMENT_CONTROL_ITEM_CLASS)))    \
    return __VA_ARGS__;

#endif
