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

#include <iostream>

#include "canvas/evas_canvas.eo.hh"
#include "canvas/evas_rectangle.eo.hh"

namespace efl { namespace evas {
    using ::evas::canvas;
    using ::evas::rectangle;
  } }

static void _print_items(void *data, Evas_Object *obj, void *event_info);
static void _free(void *data, Evas_Object *obj, void *event_info);
//static void _add_item(void *data, Evas_Object *obj, void *event_info);

static void
_clear_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
  if (!data) return;
   static_cast<::elm_hoversel*>(data)->clear();
}

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  efl::eina::eina_init eina_init;

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("hoversel", "Hoversel"));
  win.autohide_set(true);

  efl::evas::rectangle rect(efl::eo::parent = win);
  rect.color_set( 255, 0, 0, 255);
  rect.visibility_set(true);

  ::elm_hoversel hoversel(efl::eo::parent = win);
  hoversel.horizontal_set(false);
  hoversel.text_set("elm.text", "Add an item to Hoversel");
  hoversel.content_set("icon", rect);

  hoversel.item_add("Print items", "", ELM_ICON_NONE, &_print_items, NULL);
  hoversel.item_add( "Option 2", "home", ELM_ICON_STANDARD, NULL,NULL);

  auto add_item = std::bind([] (::elm_hoversel obj) 
    {
      static int num = 0;
      char *str = new char[sizeof(char) * 10];
      Elm_Object_Item *hoversel_it;
      
      snprintf(str, 10, "item %d", ++num);
      
      hoversel_it = obj.item_add(str, "", ELM_ICON_NONE, NULL, str);
      elm_object_item_del_cb_set(hoversel_it, &_free);
    }, std::placeholders::_1);

  hoversel.callback_clicked_add(add_item);

  hoversel.size_set(180, 30);
  hoversel.position_set( 10, 10);
  hoversel.visibility_set(true);

  ::elm_button btn(efl::eo::parent = win);		
  btn.text_set("elm.text", "Clear all Items");

  auto clear_btn_clicked = std::bind([&] () { hoversel.clear();  });
    
  btn.callback_clicked_add(clear_btn_clicked);
  btn.size_set(180, 30);
  btn.position_set(10, 50);
  btn.visibility_set(true);

  win.size_set(200, 300);
  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()

static void
_print_items(void *data, Evas_Object *obj, void *event_info)
{
  efl::eina::range_list<const elm_hoversel_item> items(elm_hoversel_items_get(obj));

  for (auto i : items)
    try
      {
	std::cout << i.part_text_get("elm.text") << std::endl;
      }
    catch (std::logic_error const&) {}
}

static void
_free(void *data, Evas_Object *obj, void *event_info)
{
  delete[] static_cast<char *>(data);
}
