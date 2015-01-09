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
#include <pthread.h>

#include "elm_interface_atspi_accessible.h"
#include "elm_interface_atspi_widget_action.h"
#include "elm_interface_atspi_text.h"
#include "elm_interface_scrollable.h"
}

#include <Elementary.hh>
#include <Eina.hh>

#include <Evas.hh>

#include <sstream>

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  elm_app_compile_data_dir_set("/home/luciana/Enlightenment/local/share/elementary/");
  elm_app_info_set(reinterpret_cast<void*>(elm_main), "elementary", "images/logo_small.png");
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("popup", "Popup"));
  win.autohide_set(true);

  ::elm_popup popup(efl::eo::parent = win);
   popup.text_set(nullptr, "This is the Content-Text for popup. The wrap" 
		  "for the content-text is character wrapping");
  popup.content_text_wrap_type_set(ELM_WRAP_CHAR);
  popup.text_set("elm.text", "Title");

  ::elm_icon icon1(efl::eo::parent = popup);
  std::stringstream ss;
  ss << elm_app_data_dir_get() << "/images/logo_small.png";
  icon1.file_set(ss.str(), "image");
  popup.content_set("icon", icon1);

  ::elm_button btn1(efl::eo::parent = popup);
  btn1.text_set(nullptr, "OK");
  popup.content_set("button1", btn1);

  auto response_cb = std::bind([&] () { popup.visibility_set(false); });

  btn1.callback_clicked_add(response_cb);

  ::elm_button btn2(efl::eo::parent = popup);
  btn2.text_set(nullptr, "Cancel");
  popup.content_set("button2", btn2);
  btn2.callback_clicked_add(response_cb);

  ::elm_button btn3(efl::eo::parent = popup);
  btn3.text_set(nullptr, "Close");
  popup.content_set("button3", btn3);
  btn3.callback_clicked_add(response_cb);
  popup.orient_set(ELM_POPUP_ORIENT_TOP);
  popup.visibility_set(true);

  win.size_set( 480, 800);
  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()
