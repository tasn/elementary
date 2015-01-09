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
  ::evas_init();
	
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
 
  ::elm_win win(elm_win_util_standard_add("frame", "Frame"));
  win.autohide_set(true);
	
  efl::evas::rectangle r1(efl::eo::parent = win);
  r1.color_set(255, 0, 0, 255);
  r1.visibility_set(true);
   	
  ::elm_frame f1(efl::eo::parent = win);
  f1.content_set(nullptr, r1);
  f1.text_set(nullptr, "Default frame");
  f1.size_set(100, 100);
  f1.position_set(25, 25);
  f1.visibility_set(true);
  	
  efl::evas::rectangle r2(efl::eo::parent = win);
  r2.color_set(0, 255, 0, 255);
  r2.visibility_set(true);
	
  ::elm_frame f2(efl::eo::parent = win);
  f2.content_set(nullptr, r2);
  f2.text_set(nullptr, "Padding frame");
  f2.size_set(100, 100);
  f2.position_set(150, 25);
  f2.style_set("pad_small");
  f2.visibility_set(true);
  	
  efl::evas::rectangle r3(efl::eo::parent = win);
  r3.color_set( 0, 0, 255, 255);
  r3.visibility_set(true);

  ::elm_frame f3(efl::eo::parent = win);
  f3.content_set(nullptr, r3);
  f3.text_set(nullptr, "Top outdent frame");
  f3.size_set(100, 100);
  f3.position_set(25, 150);
  f3.style_set("outdent_top");
  f3.visibility_set(true);
  	
  efl::evas::rectangle r4(efl::eo::parent = win);
  r4.color_set( 0, 0, 0, 255);
  r4.visibility_set(true);
  	
  ::elm_frame f4(efl::eo::parent = win);
  f4.content_set(nullptr, r4);
  f4.text_set(nullptr, "Bottom outdent frame");
  f4.size_set(100, 100);
  f4.position_set(150, 150);
  f4.style_set("outdent_bottom");
  f4.visibility_set(true);
  	
  win.size_set(275, 275);
  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()
