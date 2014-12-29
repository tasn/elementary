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

// void _block_clicked(::elm_popup obj, Eo_Event_Description const& desc, void *info);
// void _timeout(::elm_popup obj, Eo_Event_Description const& desc, void *info);

EAPI_MAIN int
elm_main(int argc, char **argv)
{
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("popup", "Popup"));
  win.autohide_set(true);

  ::elm_label content(efl::eo::parent = win);
  content.text_set("elm.text" , "Content");
   
  ::elm_popup popup(efl::eo::parent = win);
  popup.timeout_set(3.0);

  auto popup_hide = std::bind ( [] (::elm_popup obj) { obj.visibility_set(false); } , std::placeholders::_1 );

  popup.callback_timeout_add(popup_hide);
  popup.content_set("elm.text", content);
  popup.text_set("elm.text", "Title");
  popup.visibility_set(true);
  popup.callback_block_clicked_add(popup_hide);
  win.size_set(480, 800);
  win.visibility_set(true); 

  elm_run();

  return 0;
}
ELM_MAIN()

// void
// _block_clicked(::elm_popup obj, Eo_Event_Description const& desc, void *info)
// {
//   obj.visibility_set(false);
// }

// void
// _timeout(::elm_popup obj, Eo_Event_Description const& desc, void *info)
// {
//   obj.visibility_set(false);
// }


