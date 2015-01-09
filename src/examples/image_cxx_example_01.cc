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

#include <sstream>

#include <Elementary.hh>
#include <Eina.hh>
#include <Evas.hh>

#include "canvas/evas_canvas.eo.hh"
#include "canvas/evas_rectangle.eo.hh"

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  ::evas_init();

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  elm_app_info_set(reinterpret_cast<void*>(elm_main), "elementary", "images/plant_01.jpg");

  ::elm_win win(elm_win_util_standard_add("image", "Image"));
  win.autohide_set(true);

  std::stringstream ss;
  ss << elm_app_data_dir_get() << "/images/plant_01.jpg";

  ::elm_image image(efl::eo::parent = win);
  if (!image.file_set(ss.str(), "image"))
    {
      std::cout << "error: could not load image " << ss.str() << std::endl;
      return -1;
    }

  image.no_scale_set(true);
  image.resizable_set(false, true);
  image.smooth_set(false);
  image.orient_set(ELM_IMAGE_FLIP_HORIZONTAL);
  image.aspect_fixed_set(true);
  image.fill_outside_set(true);
  image.editable_set(true);

  image.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(image);
  image.visibility_set(true);

  win.size_set(320, 320);
  win.visibility_set(true);
  
  elm_run();
  return 0;
}
ELM_MAIN()

