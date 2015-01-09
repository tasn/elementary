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
 
  ::elm_win win(elm_win_util_standard_add("label", "Label"));
  win.autohide_set(true);
 
  ::elm_label label(efl::eo::parent = win);
  label.text_set(nullptr, "Some long text for our label, that is long but ""not too long.");
  label.slide_duration_set(3);
  label.slide_mode_set(ELM_LABEL_SLIDE_MODE_ALWAYS);
  label.style_set("slide_bounce");
  label.position_set(0, 10);
  label.size_set(200, 15);
  label.visibility_set(true);
 

  ::elm_label label2(efl::eo::parent = win);
  label2.text_set(nullptr,"This is the text for our second label, which is"
		      " much longer than the previous one, maybe even "
		      "too long, but maybe not.");
  label2.ellipsis_set(true);
  label2.size_set(200, 15);
  label2.position_set(0, 30);
  label2.visibility_set(true);
  
  ::elm_label label3(efl::eo::parent = win);
  label3.text_set(nullptr, "Some more long text much as before, long but ""not too long.");
  label3.line_wrap_set(ELM_WRAP_CHAR);
  label3.ellipsis_set(true);
  label3.size_set(200, 15);
  label3.position_set (0, 50);
  label3.visibility_set(true);

  ::elm_label label4(efl::eo::parent = win);
  label4.text_set(nullptr, "And for this label we choose a different text, "
		      "for no reason other than that we can.");
  label4.line_wrap_set(ELM_WRAP_CHAR);
  label4.size_set(200, 30);
  label4.position_set (0, 80);
  label4.visibility_set(true);
 

  ::elm_label label5(efl::eo::parent = win);
  label5.text_set(nullptr,"And for this label we choose a different text, "
		      "for no reason other than that we can.");
  label5.line_wrap_set(ELM_WRAP_WORD);
  label5.size_set(200, 40);
  label5.position_set (0, 110);
  label5.visibility_set(true);

  ::elm_label label6(efl::eo::parent = win);
  label6.text_set(nullptr,"Short text");
  label6.style_set("marker");
  label6.object::color_set(255, 0, 0, 255);
  label6.size_set(200, 15);
  label6.position_set (0, 140);
  label6.visibility_set(true);

  win.size_set(200, 170);
  win.visibility_set(true);  
  
  elm_run();
  return 0;
}
ELM_MAIN()
