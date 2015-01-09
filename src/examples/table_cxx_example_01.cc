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

  ::elm_win win(elm_win_util_standard_add("table", "Table"));
  win.autohide_set(true);

  ::elm_table table(efl::eo::parent = win);
  win.resize_object_add(table);
  table.visibility_set(true);
  table.padding_set(5, 5);
  table.homogeneous_set(true);

  ::elm_label label(efl::eo::parent = win);
  label.text_set(nullptr, "label 0");
  label.visibility_set(true);
  table.pack(label, 0, 0, 1, 1);
   	
  ::elm_label label1(efl::eo::parent = win);
  label1.text_set(nullptr, "label 1");
  label1.visibility_set(true);
  table.pack(label1, 1, 0, 1, 1);
   	
  ::elm_label label2(efl::eo::parent = win);
  label2.text_set(nullptr, "label 2");
  label2.visibility_set(true);
  table.pack(label2, 0, 1, 1, 1);
   	
  ::elm_label label3(efl::eo::parent = win);
  label3.text_set(nullptr, "label 3");
  label3.visibility_set(true);
  table.pack(label3, 1, 1, 1, 1);

  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()

