#ifdef HAVE_CONFIG_H
# include <elementary_config.h>
#endif

extern "C"
{
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
_delay_changed_cb(::elm_spinner obj, Eo_Event_Description const& desc, void* info)
{
  std::cout << "Value delay changed to " << obj.value_get() << std::endl;
}

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("spinner", "Spinner Example"));
  win.autohide_set(true);

  ::elm_box bx(efl::eo::parent = win);
  bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(bx);
  bx.visibility_set(true);

  ::elm_spinner sp(efl::eo::parent = win);
  sp.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  sp.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx.pack_end(sp);
  sp.visibility_set(true);

  ::elm_spinner sp2(efl::eo::parent = win);
  sp2.label_format_set("Percentage %%%1.2f something");
  sp2.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  sp2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx.pack_end(sp2);
  sp2.visibility_set(true);

  ::elm_spinner sp3(efl::eo::parent = win);
  sp3.label_format_set("%1.1f units");
  sp3.step_set(1.5);
  sp3.wrap_set(true);
  sp3.min_max_set(-50.0, 250.0);
  sp3.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  sp3.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx.pack_end(sp3);
  sp3.visibility_set(true);

  ::elm_spinner sp4(efl::eo::parent = win);
  sp4.style_set("vertical");
  sp4.interval_set(0.2);
  sp4.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  sp4.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx.pack_end(sp4);
  sp4.visibility_set(true);

  ::elm_spinner sp5(efl::eo::parent = win);
  sp5.editable_set(false);
  sp5.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  sp5.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx.pack_end(sp5);
  sp5.visibility_set(true);

  ::elm_spinner sp6(efl::eo::parent = win);
  sp6.editable_set(false);
  sp6.min_max_set(1, 12);
  sp6.special_value_add(1, "January");
  sp6.special_value_add(2, "February");
  sp6.special_value_add(3, "March");
  sp6.special_value_add(4, "April");
  sp6.special_value_add(5, "May");
  sp6.special_value_add(6, "June");
  sp6.special_value_add(7, "July");
  sp6.special_value_add(8, "August");
  sp6.special_value_add(9, "September");
  sp6.special_value_add(10, "October");
  sp6.special_value_add(11, "November");
  sp6.special_value_add(12, "December");
  sp6.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  sp6.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx.pack_end(sp6);
  sp6.visibility_set(true);

  auto changed = std::bind([] (::elm_spinner obj) { std::cout << "Value changed to " << obj.value_get() << std::endl; } , std::placeholders::_1 );
  auto delay = std::bind([] (::elm_spinner obj) {std::cout << "Value delay changed to " << obj.value_get() << std::endl;} , std::placeholders::_1 );

  ::elm_spinner sp7(efl::eo::parent = win);
  sp7.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  sp7.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx.pack_end(sp7);
  sp7.visibility_set(true);
  sp7.editable_set(true);
  sp7.callback_changed_add(changed);
  sp7.callback_delay_changed_add(delay);

  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()

