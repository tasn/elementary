extern "C"
{
#ifdef HAVE_CONFIG_H
# include <elementary_config.h>
#endif

#define ELM_INTERNAL_API_ARGESFSDFEFC
#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_COMPONENT_PROTECTED
#define ELM_INTERFACE_ATSPI_ACTION_PROTECTED
#define ELM_INTERFACE_ATSPI_VALUE_PROTECTED
#define ELM_INTERFACE_ATSPI_EDITABLE_TEXT_PROTECTED
#define ELM_INTERFACE_ATSPI_TEXT_PROTECTED
#define ELM_INTERFACE_ATSPI_SELECTION_PROTECTED
#define ELM_INTERFACE_ATSPI_IMAGE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#include <Eo.h>
#include <Evas.h>
#include <Elementary.h>
#include <elm_widget.h>

#include "elm_interface_atspi_accessible.h"
#include "elm_interface_atspi_widget_action.h"
#include "elm_interface_atspi_text.h"
#include "elm_interface_scrollable.h"
}

#include <Elementary.hh>
#include <Eina.hh>

#include <Evas.hh>

#include "canvas/evas_canvas.eo.hh"
#include "canvas/evas_rectangle.eo.hh"

namespace efl { namespace evas {
    using ::evas::canvas;
    using ::evas::rectangle;
  } }

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  efl::eina::eina_init eina_init;
  Elm_Object_Item *menu_it, *menu_it1;
  
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
  
  ::elm_win win(elm_win_util_standard_add("menu", "Menu"));
  win.autohide_set(true);

  efl::evas::rectangle rect(efl::eo::parent = win);
  win.resize_object_add(rect);
  rect.size_hint_min_set(0, 0);
  rect.color_set( 0, 0, 0, 0);
  rect.visibility_set(true);

  ::elm_menu menu(efl::eo::parent = win);
  menu.item_add(NULL, "", "first item", NULL, NULL);
  menu_it = menu.item_add(NULL, "mail-reply-all", "second item", NULL, NULL);

  menu.item_add(menu_it, "object-rotate-left", "menu 1", NULL, NULL);
  ::elm_button button(efl::eo::parent = win);
  button.text_set("elm.text", "button - delete items");
  menu_it1 = menu.item_add(menu_it, "", "", NULL, NULL);
  elm_object_item_content_set(menu_it1, eo_ref(button._eo_ptr()));      // YYYY

  auto del_it = std::bind([&] ()
    {  
      const Eina_List *l = elm_menu_item_subitems_get(menu_it);
      elm_object_item_del(static_cast<Elm_Widget_Item*>(eina_list_data_get(l)));
      std::cout << "TEST" << std::endl;
    });

  button.callback_clicked_add(del_it);
  menu.item_separator_add(menu_it);
  menu.item_add(menu_it, "", "third item", NULL, NULL);
  menu.item_add(menu_it, "", "fourth item", NULL, NULL);
  menu.item_add(menu_it, "window-new", "sub menu", NULL, NULL);

  menu_it = menu.item_add(NULL, "", "third item", NULL, NULL);
  elm_object_item_disabled_set(menu_it, EINA_TRUE);

  auto show = std::bind([&] (void *event_info) 
  			{
  			  Evas_Event_Mouse_Down *ev = static_cast<Evas_Event_Mouse_Down*>(event_info);
  			  menu.move(ev->canvas.x, ev->canvas.y);
  			  menu.visibility_set(true);
  			}, std::placeholders::_3);

  rect.callback_mouse_down_add(show);
  menu.visibility_set(true);

  win.size_set(250, 350);
  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()
