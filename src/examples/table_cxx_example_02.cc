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
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("table", "Table"));
  win.autohide_set(true);
  
  ::elm_table table(efl::eo::parent = win);
  win.resize_object_add(table);
  table.visibility_set(true);
  table.homogeneous_set(false);
   	   	
  efl::evas::rectangle rect(efl::eo::parent = win);
  rect.color_set( 255, 0, 0, 255);
  rect.size_hint_min_set( 100, 50); 
  rect.visibility_set(true);
  table.pack(rect, 0, 0, 2, 1);
		
  efl::evas::rectangle rect2(efl::eo::parent = win);
  rect2.color_set( 0, 255, 0, 255);
  rect2.size_hint_min_set(50, 100); 
  rect2.visibility_set(true);
  table.pack(rect2, 0, 1, 1, 2);
	
  efl::evas::rectangle rect3(efl::eo::parent = win);
  rect3.color_set( 0, 0, 255, 255);
  rect3.size_hint_min_set(50, 50); 
  rect3.visibility_set(true);
  table.pack(rect3, 1, 1, 1, 1);
	
  efl::evas::rectangle rect4(efl::eo::parent = win);
  rect4.color_set( 255, 255, 0, 255);
  rect4.size_hint_min_set(50, 50);
  rect4.visibility_set(true);
  table.pack(rect4, 1, 2, 1, 1);

  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()
