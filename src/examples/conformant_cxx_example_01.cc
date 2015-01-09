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
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
 
  ::elm_win win(elm_win_util_standard_add("conformant", "Conformant Example"));
  win.autohide_set(true);

  ::elm_box bx(efl::eo::parent = win);
  bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(bx);
  bx.visibility_set(true);

  ::elm_button btn(efl::eo::parent = win);
  btn.text_set(nullptr, "Test Conformant");
  btn.size_hint_weight_set(EVAS_HINT_EXPAND, 0);
  btn.size_hint_align_set(EVAS_HINT_FILL, 0);
  bx.pack_end(btn);
  btn.visibility_set(true);
  
  ::elm_entry en(efl::eo::parent = win);
  en.scrollable_set(true);
  en.text_set(nullptr,
		      "This is a multi-line entry at the bottom<br>"
		      "This can contain more than 1 line of text and be "
		      "scrolled around to allow for entering of lots of "
		      "content. It is also to test to see that autoscroll "
		      "moves to the right part of a larger multi-line "
		      "text entry that is inside of a scroller than can be "
		      "scrolled around, thus changing the expected position "
		      "as well as cursor changes updating auto-scroll when "
		      "it is enabled.");

  en.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  en.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  en.visibility_set(true);
  bx.pack_end(en);

  ::elm_button btn2(efl::eo::parent = win);
  btn2.text_set(nullptr, "Test Conformant");
  btn2.size_hint_weight_set(EVAS_HINT_EXPAND, 0);
  btn2.size_hint_align_set(EVAS_HINT_FILL, 0);
  bx.pack_end(btn2);
  btn2.visibility_set(true);
  
  win.size_set(240, 480);
  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()

