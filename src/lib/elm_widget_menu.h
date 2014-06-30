#ifndef ELM_WIDGET_MENU_H
#define ELM_WIDGET_MENU_H

#include "Elementary.h"

#include "elm_object_item_migration_temp.h"
#include "elm_menu_item.eo.h"

/**
 * @addtogroup Widget
 * @{
 *
 * @section elm-menu-class The Elementary Menu Class
 *
 * Elementary, besides having the @ref Menu widget, exposes its
 * foundation -- the Elementary Menu Class -- in order to create
 * other widgets which are a menu with some more logic on top.
 */

/**
 * Base widget smart data extended with menu instance data.
 */
typedef struct _Elm_Menu_Data Elm_Menu_Data;
struct _Elm_Menu_Data
{
   Evas_Object          *hv, *bx, *location, *parent;

   Eina_List            *items;
   Evas_Coord            xloc, yloc;
   Elm_DBus_Menu        *dbus_menu;

   Eina_Bool             menu_bar : 1;
};

typedef struct _Elm_Menu_Item_Data       Elm_Menu_Item_Data;
struct _Elm_Menu_Item_Data
{
   Elm_Widget_Item_Data *base;

   Elm_Widobj_Item *parent;
   Evas_Object   *content;
   const char    *icon_str;
   const char    *label;
   Evas_Smart_Cb  func;
   unsigned int   idx;
   Elm_DBus_Menu *dbus_menu;
   int            dbus_idx;

   struct
   {
      Evas_Object *hv, *bx, *location;
      Eina_List   *items;
      Eina_Bool    open : 1;
   } submenu;

   Eina_Bool      separator : 1;
   Eina_Bool      selected : 1;
   Eina_Bool      object_item : 1;
   Eina_Bool      was_enabled : 1;
   Eina_Bool      blocked : 1;
};

/**
 * @}
 */

#define ELM_MENU_DATA_GET(o, sd) \
  Elm_Menu_Data * sd = eo_data_scope_get(o, ELM_MENU_CLASS)

#define ELM_MENU_DATA_GET_OR_RETURN(o, ptr)          \
  ELM_MENU_DATA_GET(o, ptr);                         \
  if (EINA_UNLIKELY(!ptr))                           \
    {                                                \
       CRI("No widget data for object %p (%s)",      \
           o, evas_object_type_get(o));              \
       return;                                       \
    }

#define ELM_MENU_DATA_GET_OR_RETURN_VAL(o, ptr, val) \
  ELM_MENU_DATA_GET(o, ptr);                         \
  if (EINA_UNLIKELY(!ptr))                           \
    {                                                \
       CRI("No widget data for object %p (%s)",      \
           o, evas_object_type_get(o));              \
       return val;                                   \
    }

#define ELM_MENU_ITEM_DATA_GET(o, sd) \
  Elm_Menu_Item_Data * sd = eo_data_scope_get(o, ELM_MENU_ITEM_CLASS)

#define ELM_MENU_CHECK(obj)                              \
  if (EINA_UNLIKELY(!eo_isa((obj), ELM_MENU_CLASS))) \
    return

#define ELM_MENU_ITEM_CHECK(obj)                            \
  if (EINA_UNLIKELY(!eo_isa((obj), ELM_MENU_ITEM_CLASS))) \
    return

#define ELM_MENU_ITEM_CHECK_OR_RETURN(obj, ...)             \
  if (EINA_UNLIKELY(!eo_isa((obj), ELM_MENU_ITEM_CLASS))) \
    return __VA_ARGS__;


#endif
