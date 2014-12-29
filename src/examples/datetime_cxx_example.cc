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

#include <Eina.hh>

#include <elm_win.eo.hh>
#include <elm_box.eo.hh>
#include <elm_datetime.eo.hh>

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("Datetime", "Datetime"));
  win.autohide_set(true);

  ::elm_box bx(efl::eo::parent = win);
  bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(bx);
  bx.horizontal_set(false);
  bx.visibility_set(true);

  ::elm_datetime datetime(efl::eo::parent = bx);
  datetime.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  datetime.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  datetime.field_visible_set(ELM_DATETIME_HOUR, false);
  datetime.field_visible_set(ELM_DATETIME_MINUTE, false);
  datetime.field_visible_set(ELM_DATETIME_AMPM, false);
  bx.pack_end(datetime);
  datetime.visibility_set(true);

  ::elm_datetime datetime2(efl::eo::parent = bx);
  datetime2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  datetime2.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  datetime2.field_visible_set(ELM_DATETIME_YEAR, false);
  datetime2.field_visible_set(ELM_DATETIME_MONTH, false);
  datetime2.field_visible_set(ELM_DATETIME_DATE, false);
  bx.pack_end(datetime2);
  datetime2.visibility_set(true);

  ::elm_datetime datetime3(efl::eo::parent = bx);
  datetime3.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  datetime3.size_hint_align_set(EVAS_HINT_FILL, 0.5);
  bx.pack_end(datetime3);
  datetime3.visibility_set(true);

  win.visibility_set(true);

  elm_run();
  
  return 0;
}
ELM_MAIN()
