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
#include <Evas.hh>

#include "canvas/evas_canvas.eo.hh"
#include "canvas/evas_rectangle.eo.hh"

namespace efl { namespace evas {
    using ::evas::canvas;
    using ::evas::rectangle;
  } }

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  ::evas_init();

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);
 
  ::elm_win win(elm_win_util_standard_add("bubble", "Bubble"));
  win.autohide_set(true);

  ::elm_label label1(efl::eo::parent = win);
  label1.text_set(nullptr, "Bubble with no icon, info or label");
  label1.visibility_set(true);
		
  efl::evas::rectangle icon(efl::eo::parent = win);
  icon.color_set( 0, 0, 255, 255);
  icon.visibility_set(true);

  elm_bubble bubble1(efl::eo::parent = win);
  bubble1.content_set("icon", icon);
  bubble1.text_set("info", "INFO");
  bubble1.text_set(nullptr, "LABEL");  
  bubble1.content_set(nullptr, label1);
  bubble1.size_set(300, 100);
  bubble1.visibility_set(true);

  auto bla = std::bind([&] () { static unsigned char corner = 0;
		     ++corner;
		     if (corner > 3)
		       bubble1.pos_set(ELM_BUBBLE_POS_TOP_LEFT);
		     else
		       bubble1.pos_set(static_cast<Elm_Bubble_Pos>(corner));
                      });
  
  bubble1.callback_clicked_add(bla);

  ::elm_label label2(efl::eo::parent = win);
  label2.text_set(nullptr, "Bubble with no icon, info or label");
  label2.visibility_set(true);

  ::elm_bubble bubble2(efl::eo::parent = win);
  bubble2.content_set(nullptr, label2);
  bubble2.size_set(200, 50);
  bubble2.position_set(0, 110);
  bubble2.visibility_set(true);

  win.size_set(300, 200);
  win.visibility_set(true);
  
  elm_run();
  return 0;
}
ELM_MAIN()
