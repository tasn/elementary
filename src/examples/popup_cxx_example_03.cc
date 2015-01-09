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

#include <sstream>
#include <iostream>

//static void _item_selected_cb(::elm_popup obj, Eo_Event_Description const& desc, void* info);

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  elm_app_compile_data_dir_set("/home/luciana/Enlightenment/local/share/elementary/");
  elm_app_info_set(reinterpret_cast<void*>(elm_main), "elementary", "images/logo_small.png");

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("popup", "Popup"));
  win.autohide_set(true);

  ::elm_popup popup(efl::eo::parent = win);

  ::elm_icon icon1(efl::eo::parent = popup);
  std::stringstream ss;
  ss << elm_app_data_dir_get() << "images/logo_small.png";
  icon1.file_set(ss.str(), "image");
  popup.text_set("title", "Title");
  Elm_Object_Item *popup_it1;
  auto _item_selected_cb = [&] () { auto aux = elm_object_item_text_get(popup_it1);
				   std::cout << "popup item selected: " << aux << std::endl; };
  
  popup.item_append("Message", nullptr, _item_selected_cb, NULL);
  popup.item_append("Email", nullptr, _item_selected_cb, NULL);
  popup.item_append("Contacts", nullptr, _item_selected_cb, NULL);
  popup.item_append("Video", nullptr, _item_selected_cb, NULL);
  popup.item_append("Music", nullptr, _item_selected_cb, NULL);
  popup.item_append("Memo", nullptr, _item_selected_cb, NULL);
  popup_it1 = popup.item_append("Radio", nullptr, _item_selected_cb, NULL);

  elm_object_item_text_set(popup_it1, "FM");
  popup.item_append("Messenger", nullptr, _item_selected_cb, NULL);
  popup.item_append("Settings", nullptr, _item_selected_cb, NULL);
  popup.item_append("App Installer", nullptr, _item_selected_cb, NULL);
  popup.item_append("Browser", nullptr, _item_selected_cb, NULL);
  popup.item_append("Weather", icon1, _item_selected_cb, NULL);
  popup.item_append("News Feeds", nullptr, _item_selected_cb, NULL);

  popup.text_set("title", "Title");

  ::elm_button btn1(efl::eo::parent = popup);
  btn1.text_set(nullptr, "OK");
  popup.content_set( "button1", btn1);

  auto response_cb = std::bind([&] () { popup.visibility_set(false); });
  btn1.callback_clicked_add(response_cb);

  ::elm_button btn2(efl::eo::parent = popup);
  btn2.text_set(nullptr, "Cancel");
  btn2.callback_clicked_add(response_cb);
  popup.content_set("button2", btn2);
  popup.visibility_set(true);

  win.size_set(480, 800);
  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()
