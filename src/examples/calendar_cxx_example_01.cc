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

#include <Eina.hh>

#include <elm_win.eo.hh>
#include <elm_calendar.eo.hh>


EAPI_MAIN int
elm_main (int argc, char *argv[])
{

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
 
  ::elm_win win(elm_win_util_standard_add("calendar", "Calendar Creation Example"));
  win.autohide_set(true);

  ::elm_calendar cal(efl::eo::parent = win);
  cal.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(cal);
  cal.visibility_set(true);
  
  win.visibility_set(true);

  elm_run();
  
  return 0;
}
ELM_MAIN()
