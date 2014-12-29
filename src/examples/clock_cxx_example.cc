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

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  int digedit;
  
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
 
  ::elm_win win(elm_win_util_standard_add("clock", "Clock Example"));
  win.autohide_set(true);

  ::elm_box bx(efl::eo::parent = win);
  bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(bx);
  bx.visibility_set(true);

  ::elm_clock ck(efl::eo::parent = win);
  bx.pack_end(ck);
  ck.visibility_set(true);

  ::elm_clock ck2(efl::eo::parent = win);
  ck2.show_am_pm_set(true);
  bx.pack_end(ck2);
  ck2.visibility_set(true);

  ::elm_clock ck3(efl::eo::parent = win);
  ck3.show_seconds_set(true);
  ck3.time_set(10, 11, 12);
  bx.pack_end(ck3);
  ck3.visibility_set(true);

  ::elm_clock ck4(efl::eo::parent = win);
  ck4.edit_set(true);
  ck4.show_seconds_set(true);
  ck4.show_am_pm_set(true);
  ck4.time_set(10, 11, 12);
  bx.pack_end(ck4);
  ck4.visibility_set(true);

  ::elm_clock ck5(efl::eo::parent = win);
  ck5.show_seconds_set(true);
  ck5.edit_set(true);
  digedit = ELM_CLOCK_EDIT_HOUR_UNIT | ELM_CLOCK_EDIT_MIN_UNIT | ELM_CLOCK_EDIT_SEC_UNIT;
  ck5.edit_mode_set(static_cast<Elm_Clock_Edit_Mode>(digedit));
  bx.pack_end(ck5);
  ck5.visibility_set(true);

  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()
