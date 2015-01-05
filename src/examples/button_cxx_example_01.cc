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

#include <iostream>
#include <string>

EAPI_MAIN int
elm_main(int argc, char *argv[])
{
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("Button example", "Button example"));
  win.autohide_set(true);

  Evas_Object *icon_still = NULL;;
  ::elm_button mid(efl::eo::parent = win);
  ::elm_button up(efl::eo::parent = win);
  ::elm_button down(efl::eo::parent = win);
  ::elm_button left(efl::eo::parent = win);
  ::elm_button right(efl::eo::parent = win);

  ::elm_box box(efl::eo::parent = win);
  box.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(box);
  box.visibility_set(true);

  ::elm_box box2(efl::eo::parent = win);
  box2.horizontal_set(true);
  box2.size_hint_weight_set(EVAS_HINT_EXPAND, 0.0);
  box.pack_end(box2);
  box2.visibility_set(true);

  ::elm_button btn(efl::eo::parent = win);
  btn.text_set("elm.text", "Initial: 0.0");
  box2.pack_end(btn);
  btn.visibility_set(true);

  auto btn_options = std::bind([&] (evas::clickable_interface ci)
    {
      ::elm_button b(eo_ref(ci._eo_ptr()));
      std::string::size_type ptr;
      efl::eina::optional<std::string> lbl = b.text_get("elm.text");
      
      ptr = lbl->find(":");
      ptr += 2;
      double t = std::stod(lbl->substr(ptr));

      if (lbl->compare(0,7,"Initial") != 0)
	{
	  up.autorepeat_initial_timeout_set(t);
	  down.autorepeat_initial_timeout_set(t);
	  left.autorepeat_initial_timeout_set(t);
	  right.autorepeat_initial_timeout_set(t);
	}
      else if (lbl->compare(0,3,"Gap") != 0)
	{
	  up.autorepeat_gap_timeout_set(t);
	  down.autorepeat_gap_timeout_set(t);
	  left.autorepeat_gap_timeout_set(t);
	  right.autorepeat_gap_timeout_set(t);
	}
    }, std::placeholders::_1);
  
  btn.callback_clicked_add(btn_options);
  
  ::elm_button btn2(efl::eo::parent = win);
  btn2.text_set("elm.text", "Initial: 1.0");
  box2.pack_end(btn2);
  btn2.visibility_set(true);
  btn2.callback_clicked_add(btn_options);

  ::elm_button btn3(efl::eo::parent = win);
  btn3.text_set("elm.text", "Initial: 5.0");
  box2.pack_end(btn3);
  btn3.visibility_set(true);
  btn3.callback_clicked_add(btn_options);

  ::elm_box box3(efl::eo::parent = win);
  box3.horizontal_set(true);
  box3.size_hint_weight_set(EVAS_HINT_EXPAND, 0.0);
  box.pack_end(box3);
  box3.visibility_set(true);

  ::elm_button btn4(efl::eo::parent = win);
  btn4.text_set("elm.text", "Gap: 0.1");
  box2.pack_end(btn4);
  btn4.visibility_set(true);
  btn4.callback_clicked_add(btn_options);

  ::elm_button btn5(efl::eo::parent = win);
  btn5.text_set("elm.text", "Gap: 0.5");
  box2.pack_end(btn5);
  btn5.visibility_set(true);
  btn5.callback_clicked_add(btn_options);

  ::elm_button btn6(efl::eo::parent = win);
  btn6.text_set("elm.text", "Gap: 1.0");
  box2.pack_end(btn6);
  btn6.visibility_set(true);
  btn6.callback_clicked_add(btn_options);

  up.autorepeat_set(true);
  up.autorepeat_initial_timeout_set(1.0);
  up.autorepeat_gap_timeout_set(0.5);
  up.size_hint_weight_set(EVAS_HINT_EXPAND, 0.0);
  up.size_hint_align_set(EVAS_HINT_FILL, 0.0);
  box.pack_end(up);
  up.visibility_set(true);

  auto btn_cursors_move = std::bind([&] (evas::clickable_interface ci2)
    {
      ::elm_button button(eo_ref(ci2._eo_ptr()));
      double ax, ay;

      if (!icon_still)
	{
	  ::elm_icon obj(efl::eo::parent = mid);
	  efl::eina::optional<evas::object> icon_still = mid.content_unset("icon");
	  icon_still->visibility_set(false);
	  obj.standard_set("chat");
	  mid.content_set("icon", obj);
	}

      mid.size_hint_align_get(&ax, &ay);
      if (button == up)
	{
	  ay -= 0.05;
	  if (ay < 0.0)
	    ay = 0.0;
	}
      else if (button == down)
	{
	  ay += 0.05;
	  if (ay > 1.0)
	    ay = 1.0;
	}
      else if (button == left)
	{
	  ax -= 0.05;
	  if (ax < 0.0)
	    ax = 0.0;
	}
      else if (button == right)
	{
	  ax += 0.05;
	  if (ax > 1.0)
	    ax = 1.0;
	}
      mid.size_hint_align_set(ax, ay);
    }, std::placeholders::_1);

  up.callback_repeated_add(btn_cursors_move);
  
  auto btn_cursors_release = std::bind([&] () 
    {
      if (icon_still)
	{
	  mid.content_set("icon", evas::object(eo_ref(icon_still)));
	  icon_still = NULL;
	}
    });

  up.callback_unpressed_add(btn_cursors_release);

  ::elm_icon icon(efl::eo::parent = win);
  icon.standard_set("arrow_up");
  up.content_set("icon", icon);

  ::elm_box box4(efl::eo::parent = win);
  box4.horizontal_set(true);
  box4.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  box4.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  box.pack_end(box4);
  box4.visibility_set(true);

  left.autorepeat_set(true);
  left.autorepeat_initial_timeout_set(1.0);
  left.autorepeat_gap_timeout_set(0.5);
  left.size_hint_weight_set(0.0, EVAS_HINT_EXPAND);
  left.size_hint_align_set(0.0, EVAS_HINT_FILL);
  box4.pack_end(left);
  left.visibility_set(true);
  left.callback_repeated_add(btn_cursors_move);
  left.callback_unpressed_add(btn_cursors_release);
 
  ::elm_icon icon2 (efl::eo::parent = win);
  icon2.standard_set("arrow_left");
  left.content_set("icon", icon);

  mid.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  box4.pack_end(mid);
  mid.visibility_set(true);

  ::elm_icon icon3 (efl::eo::parent = win);
  icon3.standard_set("close");
  mid.content_set("icon", icon);

  right.autorepeat_set(true);
  right.autorepeat_initial_timeout_set(1.0);
  right.autorepeat_gap_timeout_set(0.5);
  right.size_hint_weight_set(0.0, EVAS_HINT_EXPAND);
  right.size_hint_align_set(0.0, EVAS_HINT_FILL);
  box4.pack_end(right);
  right.visibility_set(true);
  right.callback_repeated_add(btn_cursors_move);
  right.callback_unpressed_add(btn_cursors_release);

  ::elm_icon icon4 (efl::eo::parent = win);
  icon4.standard_set("arrow_right");
  right.content_set("icon", icon);

  down.autorepeat_set(true);
  down.autorepeat_initial_timeout_set(1.0);
  down.autorepeat_gap_timeout_set(0.5);
  down.size_hint_weight_set(EVAS_HINT_EXPAND, 0.0);
  down.size_hint_align_set(EVAS_HINT_FILL, 0.0);
  box4.pack_end(down);
  down.visibility_set(true);
  down.callback_repeated_add(btn_cursors_move);
  down.callback_unpressed_add(btn_cursors_release);

  ::elm_icon icon5 (efl::eo::parent = win);
  icon5.standard_set("arrow_down");
  down.content_set("icon", icon);

  win.size_set(300, 320);
  win.visibility_set(true);

  elm_run();

  return 0;
}
ELM_MAIN()
