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

#include <iostream>

static void
_clicked_double(::elm_panes obj, Eo_Event_Description const& desc, void* info)
{
  static double size = 0.0;
  double tmp_size = 0.0;

  tmp_size = obj.content_left_size_get();
  if (tmp_size > 0)
    {
      obj.content_left_size_set(0.0);
      printf("Double clicked, hidding.\n");
    }
  else
    {
      obj.content_left_size_set(size);
      std::cout << "Double clicked, restoring size." << std::endl;
    }
   size = tmp_size;
}

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("panes", "Panes Example"));
  win.autohide_set(true);
  win.focus_highlight_enabled_set(true);

  ::elm_panes panes(efl::eo::parent = win);
  panes.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(panes);
  panes.visibility_set(true);

  ::elm_button bt(efl::eo::parent = win);
  bt.text_set(nullptr, "Left");
  bt.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt.visibility_set(true);
  panes.content_set("left", bt);

  ::elm_panes panes_h(efl::eo::parent = win);
  panes_h.horizontal_set(true);
  panes_h.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  panes_h.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  panes_h.visibility_set(true);
  panes.content_set("right", panes_h);

  ::elm_button bt2(efl::eo::parent = win);
  bt2.text_set(nullptr, "Up");
  bt2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt2.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt2.visibility_set(true);
  panes_h.content_set("left", bt2);
  
  ::elm_button bt3(efl::eo::parent = win);
  bt3.text_set(nullptr, "Down");
  bt3.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt3.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt3.visibility_set(true);
  panes_h.content_set("right", bt3);

  auto clicked = std::bind([] () { std::cout << "Clicked" << std::endl; });
  panes.callback_clicked_add(clicked);

  auto clicked_double = std::bind([&] () { static double size = 0.0;
					   double tmp_size = 0.0;
					  
					   tmp_size = panes.content_left_size_get();
					   if (tmp_size > 0)
					     {
					       panes.content_left_size_set(0.0);
					       std::cout << "Double clicked, hidding." << std::endl;
					     }
					   else
					     {
					       panes.content_left_size_set(size);
					       std::cout << "Double clicked, restoring size." << std::endl;
					     }
					  size = tmp_size; });
  
  panes.callback_clicked_double_add(clicked_double);
  
  auto press = std::bind([] () { std::cout << "Pressed" << std::endl; });
  panes.callback_press_add(press);

  auto unpress = std::bind([&] () { std::cout << "Unpressed, size : " << panes.content_left_size_get() << std::endl; });
  panes.callback_unpress_add(unpress);

  win.size_set(320, 400);
  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()
