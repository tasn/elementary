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
}

#include <Elementary.hh>

#include <Eina.hh>

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
   
  ::elm_win win(elm_win_util_standard_add("Greetings", "Hello, World!"));
  win.autohide_set(true);

  ::elm_button btn(efl::eo::parent = win);
  btn.text_set("elm.text","Good-Bye, World!");
  btn.callback_clicked_add(std::bind([] () {
	std::cout << "TEST" << std::endl;
	elm_exit(); }));
  btn.size_set(120, 30);
  btn.position_set(60, 15);
  btn.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  btn.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  btn.visibility_set(true);

  win.size_set(240, 60);
  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()
