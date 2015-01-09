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

  ::elm_win win(elm_win_util_standard_add("notify", "Notify"));
  win.autohide_set(true);
  
  ::elm_box bx(efl::eo::parent = win);
  bx.horizontal_set(false);
  bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(bx);
  bx.visibility_set(true);

  ::elm_label content(efl::eo::parent = win);
  content.text_set("elm.text", "default");
  content.visibility_set(true);

  ::elm_notify notify(efl::eo::parent = win);
  notify.allow_events_set(false);
  notify.timeout_set( 5 );
  notify.content_set(nullptr, content);
  notify.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  notify.visibility_set(true);
  bx.pack_end(notify);

  auto hide = std::bind([&] () { notify.visibility_set(false); });

  notify.callback_block_clicked_add(hide);

  ::elm_label content2(efl::eo::parent = win);
  content2.text_set("elm.text", "bottom");
  content2.visibility_set(true);

  ::elm_notify notify2(efl::eo::parent = win);
  notify2.align_set( 0.5, 1.0);
  notify2.content_set(nullptr, content2);
  notify2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  notify2.visibility_set(true);
  bx.pack_end(notify2);

  ::elm_label content3(efl::eo::parent = win);
  content3.text_set("elm.text", "center");
  content3.visibility_set(true);

  ::elm_notify notify3(efl::eo::parent = win);
  notify3.align_set( 0.5, 0.5);
  notify3.content_set(nullptr, content3);
  notify3.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  notify3.visibility_set(true);
  bx.pack_end(notify3);

  win.size_set(100, 200);
  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()
