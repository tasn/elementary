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

#include <stdio.h>

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
   
  ::elm_win win(elm_win_util_standard_add("calendar", "Calendar Getters Example"));
  win.autohide_set(true);
      
  ::elm_calendar cal(efl::eo::parent = win);
  cal.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(cal);

  auto print_cal_info = std::bind([] (::elm_calendar obj)
    {
      int year_min, year_max;
      bool sel_enabled;
      const char **wds;
      struct tm sel_time;
      double interval;

      if (!obj.selected_time_get(&sel_time))
	return;

      interval = obj.interval_get();
      obj.min_max_year_get(&year_min, &year_max);
      sel_enabled = (obj.select_mode_get() != ELM_CALENDAR_SELECT_MODE_NONE);
      wds = obj.weekdays_names_get();

      std::cout << "Day: " << sel_time.tm_mday << ", Mon: " << sel_time.tm_mon << ", Year " << sel_time.tm_year + 1900 << ", WeekDay: " << sel_time.tm_wday << "<br>" << std::endl;
      std::cout << "Interval: " << interval << ", Year_Min: " << year_min << ", Year_Max " << year_max << ", Sel Enabled: " << sel_enabled << "<br>" << std::endl;
      std::cout << "Weekdays: " << wds[0] << ", " << wds[1] << ", " << wds[2] << ", " << wds[3] <<  ", " <<  wds[4] << ", " <<  wds[5] << ", " << wds[6] << "<br>" << std::endl << std::endl;
    } , std::placeholders::_1 ) ;

  cal.callback_changed_add(print_cal_info);
  cal.visibility_set(true);

  win.visibility_set(true);

  elm_run();
  
  return 0;
}
ELM_MAIN()
