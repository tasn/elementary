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
#include "elm_interface_scrollable.h"
}

#include <Elementary.hh>
#include <Eina.hh>

#define TABLE "example/table"
#define BOX "example/box"
#define TITLE "example/title"
#define SWALLOW "example/custom"

static int _box_buttons = 0;

EAPI_MAIN int
elm_main (int argc, char *argv[])
{
  char buf[30];
  elm_app_info_set(elm_main, "elementary", "examples/layout_example.edj");
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

  ::elm_win win(elm_win_util_standard_add("layout", "Layout"));
  win.autohide_set(true);

  ::elm_layout layout(efl::eo::parent = win);
  layout.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win.resize_object_add(layout);
  snprintf(buf, sizeof(buf), "%s/examples/layout_example.edj", elm_app_data_dir_get());
  layout.file_set(buf, "example/mylayout");
  layout.visibility_set(true);

  efl::eina::optional<std::string> title = layout.data_get("title");
  if (title)
    {
      win.title_set(title);
      static_cast<efl::eina::optional<std::string>>(TITLE).text_set(nullptr, title);
    }

  ::elm_icon icon(efl::eo::parent = win);
  icon.standard_set("home");
  icon.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  icon.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  layout.table_pack(TABLE, icon, 0, 0, 1, 1);
  icon.visibility_set(true);

  ::elm_icon icon2(efl::eo::parent = win);
  icon2.standard_set("close");
  icon2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  icon2.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  layout.table_pack(TABLE, icon2, 1, 0, 1, 1);
  icon2.visibility_set(true);

  ::elm_clock clk(efl::eo::parent = win);
  clk.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  clk.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  layout.table_pack(TABLE, clk, 2, 0, 1, 1);
  clk.visibility_set(true);

  ::elm_button bt(efl::eo::parent = win);
  bt.text_set(nullptr, "Click me!");
  bt.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  layout.table_pack(TABLE, bt, 0, 1, 3, 1);

  auto tbl_btn_cb = std::bind([&] () { layout.table_unpack(TABLE, bt);
			    bt.del(); });
  bt.callback_clicked_add(tbl_btn_cb);
  bt.visibility_set(true);

  ::elm_button item(efl::eo::parent = win);
  item.text_set(nullptr, "Position 0");
  item.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  item.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  layout.box_insert_at(BOX, item, 0);

  box_btn_cb= std::bind([&] (::elm_button obj)  { snprintf(buf, sizeof(buf), "Button %02d", _box_buttons++);
						       obj.text_set(nullptr, buf);
						       obj.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
						       obj.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
						       layout.box_insert_before(BOX, obj, item);
						       obj.callback_clicked_add(auto box_btn_cb);
						       obj.visibility_set(true);
                                                     }, std::placeholders:_1);
  item.callback_clicked_add(box_btn_cb);
  item.visibility_set(true);

  ::elm_button item2(efl::eo::parent = win);
  item2.text_set(nullptr, "Prepended");
  item2.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  item2.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
  layout.box_prepend(BOX, item2);
  item.callback_clicked_add(box_btn_cb);
  item.visibility_set(true);

  ::elm_button bt2(efl::eo::parent = win);
  bt2.text_set(nullptr, "Delete All");
  layout.content_set(SWALLOW, bt2);

  auto swallow_btn_cb = std::bind([&] () { layout.table_clear(TABLE, true);
			    layout.box_remove_all(BOX, true);
			    item = layout.content_unset(SWALLOW);
			    item.del(); });
  bt2.callback_clicked_add(swallow_btn_cb);
   	
  win.size_set(320, 320);
  win.visibility_set(true);

  elm_run();
  return 0;
}
ELM_MAIN()
