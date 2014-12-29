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
#include "elm_interface_atspi_text.h"
#include "elm_interface_atspi_widget_action.h"
#include "elm_interface_scrollable.h"
}

#include <Elementary.hh>
#include <Eina.hh>

#include <iostream>

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  static int val = 1;

  ::elm_win win(elm_win_util_standard_add("radio", "Radio"));
  win.autohide_set(true);

  ::elm_box bx(efl::eo::parent = win);
  bx.horizontal_set(true);
  bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(bx);
  bx.visibility_set(true);

  ::elm_radio radio(efl::eo::parent = win);
  ::elm_radio group(efl::eo::parent = win);
  group = radio;
  radio.text_set("elm.text", "Radio 1");
  radio.state_value_set(1);
  radio.value_pointer_set(&val);
   	
  auto cb = std::bind([] () { std::cout << "val is now: " << val << std::endl; });

  ::elm_icon ic(efl::eo::parent = win);
  ic.standard_set("home");
  radio.content_set("icon", ic);
  bx.pack_end(radio);
  radio.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  radio.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  radio.visibility_set(true);
  radio.callback_changed_add(cb);
   	
  ::elm_radio radio2(efl::eo::parent = win);
  radio2.text_set("elm.text", "Radio 2");
  radio2.state_value_set(2);
  radio2.value_pointer_set(&val);
  radio2.group_add(group);
  ::elm_icon ic2(efl::eo::parent = win);
  ic2.standard_set("file");
  radio2.content_set("icon", ic2);
  radio2.content_set("icon", ic2);
  bx.pack_end(radio2);
  radio2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  radio2.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  radio2.visibility_set(true);
  radio2.callback_changed_add(cb);

  ::elm_radio radio3(efl::eo::parent = win);
  radio3.text_set("elm.text", "Radio 3");
  radio3.state_value_set(3);
  radio3.value_pointer_set(&val);
  radio3.group_add(group);
  bx.pack_end(radio3);
  radio3.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  radio3.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  radio3.visibility_set(true);
  radio3.callback_changed_add(cb);

  ::elm_radio radio4(efl::eo::parent = win);
  radio4.text_set("elm.text", "Radio 4");
  radio4.state_value_set(4);
  radio4.value_pointer_set(&val);
  radio4.group_add(group);
  bx.pack_end(radio4);
  radio4.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  radio4.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  radio4.visibility_set(true);
  radio4.callback_changed_add(cb);

  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()

// static void _cb(::elm_radio obj, Eo_Event_Description const& desc, void* info)
// {
//   std::cout << "val is now: " << val << std::endl;
// }

