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

#include <iostream>
#include <sstream>

EAPI_MAIN int
elm_main(int argc, char *argv[])
{
  elm_need_ethumb();

  elm_app_info_set(reinterpret_cast<void*>(elm_main), "elementary", "images/plant_01.jpg");
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("thumb", "Thumbnailer"));
  win.autohide_set(true);

  ::elm_thumb thumb(efl::eo::parent = win);

  auto generation_started = std::bind([] { std::cout << "thumbnail generation started." << std::endl; });

  auto generation_finished = std::bind([] { std::cout << "thumbnail generation finished." << std::endl; });

  auto generation_error = std::bind([] { std::cout << "thumbnail generation error." << std::endl; } );

  thumb.callback_generate_start_add(generation_started);
  thumb.callback_generate_stop_add(generation_finished);
  thumb.callback_generate_error_add(generation_error);

  thumb.size_set(160, 160);
  thumb.editable_set(false);
  std::stringstream ss;
  ss << elm_app_data_dir_get() << "/images/plant_01.jpg";
  thumb.file_set(ss.str(), "image");
  thumb.reload();

  thumb.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(thumb);
  thumb.visibility_set(true);

  win.size_set(320, 320);
  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()

