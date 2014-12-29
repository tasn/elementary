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

  ::elm_win win(elm_win_util_standard_add("separator", "Separator"));
  win.autohide_set(true);
  	
  ::elm_box bx(efl::eo::parent = win);
  bx.horizontal_set(true);
  bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(bx);
  bx.visibility_set(true);

  efl::evas::rectangle rect(efl::eo::parent = win);
  rect.color_set( 0, 255, 0, 255);
  rect.size_hint_min_set( 90, 200);  
  rect.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  rect.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  rect.visibility_set(true);
  bx.pack_end(rect);

  ::elm_separator separator(efl::eo::parent = win);
  separator.horizontal_set(true);
  separator.visibility_set(true);
  bx.pack_end(separator);

  efl::evas::rectangle rect2(efl::eo::parent = win);
  rect2.color_set( 0, 0, 255, 255);
  rect2.size_hint_min_set( 90, 200);  
  rect2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  rect2.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  rect2.visibility_set(true);
  bx.pack_end(rect2);
	
  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()

