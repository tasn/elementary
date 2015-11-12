#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#include <Elementary.h>

static void
_my_bt_go_300_300(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   elm_scroller_region_bring_in((Evas_Object *)data, 300, 300, 318, 318);
}

static void
_my_bt_go_900_300(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   elm_scroller_region_bring_in((Evas_Object *)data, 900, 300, 318, 318);
}

static void
_my_bt_go_300_900(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   elm_scroller_region_bring_in((Evas_Object *)data, 300, 900, 318, 318);
}

static void
_my_bt_go_900_900(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   elm_scroller_region_bring_in((Evas_Object *)data, 900, 900, 318, 318);
}

static void
_my_bt_prev_page(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *sc = (Evas_Object *)data;
	int page_x = 0, page_y = 0;
	elm_scroller_current_page_get(sc, &page_x, &page_y);
	elm_scroller_page_bring_in(sc, --page_x, page_y);
}

static void
_my_bt_next_page(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *sc = (Evas_Object *)data;
	int page_x = 0, page_y = 0;
	elm_scroller_current_page_get(sc, &page_x, &page_y);
	elm_scroller_page_bring_in(sc, ++page_x, page_y);
}

static void
_my_bt_freeze_toggle(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   if (elm_check_state_get(obj))
     elm_object_scroll_freeze_push((Evas_Object *)data);
   else
     elm_object_scroll_freeze_pop((Evas_Object *)data);
}

static void
_my_bt_hold_toggle(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   if (elm_check_state_get(obj))
     elm_object_scroll_hold_push((Evas_Object *)data);
   else
     elm_object_scroll_hold_pop((Evas_Object *)data);
}

static void
_my_bt_block_movements_x_axis(void *data, Evas_Object *obj,
                             void *event_info EINA_UNUSED)
{
   Elm_Scroller_Movement_Block block;

   block = elm_scroller_movement_block_get((Evas_Object *)data);

   if (elm_check_state_get(obj))
     {
        elm_scroller_movement_block_set((Evas_Object *)data,
                                        ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL |
                                        block);
     }
   else if (block & ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL)
     {
        elm_scroller_movement_block_set((Evas_Object *)data,
                                        ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL);
     }
   else
     {
        elm_scroller_movement_block_set((Evas_Object *)data,
                                        ELM_SCROLLER_MOVEMENT_NO_BLOCK);
     }
}

static void
_my_bt_snap_to_pages(void *data,
                    Evas_Object *obj,
                    void *event_info EINA_UNUSED)
{
   if (elm_check_state_get(obj))
     {
        elm_scroller_page_snap_set(data, EINA_TRUE, EINA_TRUE);
     }
   else
     {
        elm_scroller_page_snap_set(data, EINA_FALSE, EINA_FALSE);
     }
}

static void
_my_bt_block_movements_y_axis(void *data, Evas_Object *obj,
                             void *event_info EINA_UNUSED)
{
   Elm_Scroller_Movement_Block block;

   block = elm_scroller_movement_block_get((Evas_Object *)data);

   if (elm_check_state_get(obj))
     {
        elm_scroller_movement_block_set((Evas_Object *)data,
                                        ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL |
                                        block);
     }
   else if (block & ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL)
     {
        elm_scroller_movement_block_set((Evas_Object *)data,
                                        ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL);
     }
   else
     {
        elm_scroller_movement_block_set((Evas_Object *)data,
                                        ELM_SCROLLER_MOVEMENT_NO_BLOCK);
     }
}

static void
_my_bt_loop_x_axis(void *data, Evas_Object *obj EINA_UNUSED,
                  void *event_info EINA_UNUSED)
{
   Eina_Bool loop_h, loop_v;
   Evas_Object *scroller = (Evas_Object *)data;

   elm_scroller_loop_get(scroller, &loop_h, &loop_v);
   elm_scroller_loop_set(scroller, !loop_h, loop_v);
}

static void
_my_bt_loop_y_axis(void *data, Evas_Object *obj EINA_UNUSED,
                  void *event_info EINA_UNUSED)
{
   Eina_Bool loop_h, loop_v;
   Evas_Object *scroller = (Evas_Object *)data;

   elm_scroller_loop_get(scroller, &loop_h, &loop_v);
   elm_scroller_loop_set(scroller, loop_h, !loop_v);
}

static void
_my_bt_wheel_disable_cb(void *data, Evas_Object *obj EINA_UNUSED,
                  void *event_info EINA_UNUSED)
{
   Evas_Object *scroller = (Evas_Object *)data;

   elm_scroller_wheel_disabled_set(scroller, elm_check_state_get(obj));
}

static void
_page_change_cb(void *data EINA_UNUSED,
               Evas_Object *obj,
               void *event_info EINA_UNUSED)
{
   int page_x = 0, page_y = 0;

   elm_scroller_current_page_get(obj, &page_x, &page_y);

   printf("Page changed to %d, %d\n", page_x, page_y);
}

static void
_sc_move_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Coord x = 0, y = 0;
   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   evas_object_move(data, x, y);
}

static void
_sc_resize_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Coord w = 0, h = 0;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   evas_object_resize(data, w, h);
}

static void
_size_changed(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Object *sc = data;
   int size = elm_spinner_value_get(obj);
   elm_scroller_step_size_set(sc, ELM_SCALE_SIZE(size), ELM_SCALE_SIZE(size));
}

void
test_scroller(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bg2, *tb, *tb2, *sc, *bt, *ck1, *ck2, *bx, *bx2, *fr,
       *ck3, *ck4, *ck5, *ck6, *ck7, *ck8;
   int i, j, n;
   char buf[PATH_MAX];
   Evas_Coord x = 0, y = 0, w = 0, h = 0;
   const char *img[9] =
     {
        "panel_01.jpg",
        "plant_01.jpg",
        "rock_01.jpg",
        "rock_02.jpg",
        "sky_01.jpg",
        "sky_02.jpg",
        "sky_03.jpg",
        "sky_04.jpg",
        "wood_01.jpg"
     };

   win = elm_win_util_standard_add("scroller", "Scroller");
   elm_win_autodel_set(win, EINA_TRUE);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   fr = elm_frame_add(win);
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, 0.0);
   elm_object_text_set(fr, "Options");
   elm_box_pack_end(bx, fr);
   evas_object_show(fr);

   bx2 = elm_box_add(win);
   elm_object_content_set(fr, bx2);
   elm_box_horizontal_set(bx2, EINA_TRUE);
   evas_object_show(bx2);

   ck1 = elm_check_add(win);
   elm_object_text_set(ck1, "Freeze");
   elm_box_pack_end(bx2, ck1);
   evas_object_show(ck1);

   ck2 = elm_check_add(win);
   elm_object_text_set(ck2, "Hold");
   elm_box_pack_end(bx2, ck2);
   evas_object_show(ck2);

   ck3 = elm_check_add(win);
   elm_object_text_set(ck3, "Block movements in X axis");
   elm_box_pack_end(bx2, ck3);
   evas_object_show(ck3);

   ck4 = elm_check_add(win);
   elm_object_text_set(ck4, "Block movements in Y axis");
   elm_box_pack_end(bx2, ck4);
   evas_object_show(ck4);

   ck5 = elm_check_add(win);
   elm_object_text_set(ck5, "Snap to pages");
   elm_box_pack_end(bx2, ck5);
   evas_object_show(ck5);

   ck6 = elm_check_add(win);
   elm_object_text_set(ck6, "Loop in X axis");
   elm_box_pack_end(bx2, ck6);
   evas_object_show(ck6);

   ck7 = elm_check_add(win);
   elm_object_text_set(ck7, "Loop in Y axis");
   elm_box_pack_end(bx2, ck7);
   evas_object_show(ck7);

   ck8 = elm_check_add(win);
   elm_object_text_set(ck8, "Wheel Disable");
   elm_box_pack_end(bx2, ck8);
   evas_object_show(ck8);

   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   tb = elm_table_add(win);

   n = 0;
   for (j = 0; j < 12; j++)
     {
        for (i = 0; i < 12; i++)
          {
             bg2 = elm_bg_add(win);
             snprintf(buf, sizeof(buf), "%s/images/%s",
                      elm_app_data_dir_get(), img[n]);
             n++;
             if (n >= 9) n = 0;
             elm_bg_file_set(bg2, buf, NULL);
             evas_object_size_hint_weight_set(bg2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(bg2, EVAS_HINT_FILL, EVAS_HINT_FILL);
             evas_object_size_hint_min_set(bg2, 318, 318);
             elm_table_pack(tb, bg2, i, j, 1, 1);
             evas_object_show(bg2);
          }
     }

   elm_object_content_set(sc, tb);
   evas_object_show(tb);

   elm_scroller_page_size_set(sc, 318, 318);
   evas_object_show(sc);

   evas_object_smart_callback_add
       (sc, "scroll,page,changed", _page_change_cb, sc);

   evas_object_smart_callback_add(ck1, "changed", _my_bt_freeze_toggle, tb);
   evas_object_smart_callback_add(ck2, "changed", _my_bt_hold_toggle, tb);
   evas_object_smart_callback_add(ck3, "changed", _my_bt_block_movements_x_axis,
                                  sc);
   evas_object_smart_callback_add(ck4, "changed", _my_bt_block_movements_y_axis,
                                  sc);
   evas_object_smart_callback_add(ck5, "changed", _my_bt_snap_to_pages, sc);
   evas_object_smart_callback_add(ck6, "changed", _my_bt_loop_x_axis, sc);
   evas_object_smart_callback_add(ck7, "changed", _my_bt_loop_y_axis, sc);
   evas_object_smart_callback_add(ck8, "changed", _my_bt_wheel_disable_cb, sc);

   bt = elm_spinner_add(win);
   elm_spinner_min_max_set(bt, 0, 500);
   elm_scroller_step_size_get(sc, &x, &y);
   elm_spinner_value_set(bt, x);
   elm_spinner_editable_set(bt, EINA_TRUE);
   elm_spinner_label_format_set(bt, "Step size: %.0f");
   evas_object_smart_callback_add(bt, "changed", _size_changed, sc);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   tb2 = elm_table_add(win);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "to 300 300");
   evas_object_smart_callback_add(bt, "clicked", _my_bt_go_300_300, sc);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.1, 0.1);
   elm_table_pack(tb2, bt, 0, 0, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "to 900 300");
   evas_object_smart_callback_add(bt, "clicked", _my_bt_go_900_300, sc);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.1);
   elm_table_pack(tb2, bt, 2, 0, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "to 300 900");
   evas_object_smart_callback_add(bt, "clicked", _my_bt_go_300_900, sc);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.1, 0.9);
   elm_table_pack(tb2, bt, 0, 2, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "to 900 900");
   evas_object_smart_callback_add(bt, "clicked", _my_bt_go_900_900, sc);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.9);
   elm_table_pack(tb2, bt, 2, 2, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "prev page");
   evas_object_smart_callback_add(bt, "clicked", _my_bt_prev_page, sc);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.1, 0.5);
   elm_table_pack(tb2, bt, 0, 1, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "next page");
   evas_object_smart_callback_add(bt, "clicked", _my_bt_next_page, sc);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.5);
   elm_table_pack(tb2, bt, 2, 1, 1, 1);
   evas_object_show(bt);

   evas_object_event_callback_add(sc, EVAS_CALLBACK_MOVE, _sc_move_cb, tb2);
   evas_object_event_callback_add(sc, EVAS_CALLBACK_RESIZE, _sc_resize_cb, tb2);
   evas_object_geometry_get(sc, &x, &y, &w, &h);
   evas_object_move(tb2, x, y);
   evas_object_resize(tb2, w, h);
   evas_object_show(tb2);

   evas_object_resize(win, 320, 420);
   evas_object_show(win);
}

static void
_click_through(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   printf("click went through on %p\n", obj);
}

void
test_scroller2(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bt, *bx, *bx2, *sc, *tb, *tb2, *rc;
   int i, j;

   win = elm_win_util_standard_add("scroller2", "Scroller 2");
   elm_win_autodel_set(win, EINA_TRUE);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, 0.0);

   /* { */
   for (i = 0; i < 3; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "Vertical");
        evas_object_smart_callback_add(bt, "clicked", _click_through, NULL);
        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
        elm_box_pack_end(bx, bt);
        evas_object_show(bt);
     }
   /* } */

   /* { */
   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, 0.5);
   elm_scroller_bounce_set(sc, EINA_TRUE, EINA_FALSE);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_box_pack_end(bx, sc);
   evas_object_show(sc);

   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, EINA_TRUE);

   for (i = 0; i < 10; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "... Horizontal scrolling ...");
        evas_object_smart_callback_add(bt, "clicked", _click_through, NULL);
        elm_box_pack_end(bx2, bt);
        evas_object_show(bt);
     }

   elm_object_content_set(sc, bx2);
   evas_object_show(bx2);
   /* } */

   /* { */
   for (i = 0; i < 3; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "Vertical");
        evas_object_smart_callback_add(bt, "clicked", _click_through, NULL);
        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
        elm_box_pack_end(bx, bt);
        evas_object_show(bt);
     }
   /* } */

   /* { */
   tb = elm_table_add(win);
   evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, tb);
   evas_object_show(tb);

   rc = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_size_hint_min_set(rc, 200, 120);
   elm_table_pack(tb, rc, 0, 0, 1, 1);

   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb, sc, 0, 0, 1, 1);
   evas_object_show(sc);

   tb2 = elm_table_add(win);

   for (j = 0; j < 16; j++)
     {
        for (i = 0; i < 16; i++)
          {
             bt = elm_button_add(win);
             elm_object_text_set(bt, "Both");
             evas_object_smart_callback_add(bt, "clicked", _click_through, NULL);
             elm_table_pack(tb2, bt, i, j, 1, 1);
             evas_object_show(bt);
          }
     }

   elm_object_content_set(sc, tb2);
   evas_object_show(tb2);
   /* } */

   for (i = 0; i < 24; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "Vertical");
        evas_object_smart_callback_add(bt, "clicked", _click_through, NULL);
        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
        elm_box_pack_end(bx, bt);
        evas_object_show(bt);
     }

   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_scroller_bounce_set(sc, EINA_FALSE, EINA_TRUE);
   elm_win_resize_object_add(win, sc);
   elm_object_content_set(sc, bx);
   evas_object_show(bx);
   evas_object_show(sc);

   evas_object_resize(win, 320, 480);
   evas_object_show(win);
}

static Ecore_Timer *_timer = NULL;
static int _append = 0;
static int _count = 0;

static void
_del_item(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   evas_object_del(obj);
}

static void
_append_item(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *bx = data, *bt;
   char buf[PATH_MAX];

   bt = elm_button_add(bx);
   snprintf(buf, sizeof(buf), "Item %d", ++_count);
   elm_object_text_set(bt, buf);
   elm_box_pack_end(bx, bt);
   evas_object_smart_callback_add(bt, "clicked", _del_item, NULL);
   evas_object_show(bt);
}

static void
_prepend_item(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *bx = data, *bt;
   char buf[PATH_MAX];

   bt = elm_button_add(bx);
   snprintf(buf, sizeof(buf), "Item %d", ++_count);
   elm_object_text_set(bt, buf);
   elm_box_pack_start(bx, bt);
   evas_object_smart_callback_add(bt, "clicked", _del_item, NULL);
   evas_object_show(bt);
}

static Eina_Bool
_append_cb(void *data)
{
   Evas_Object *bx = data, *bt;
   char buf[PATH_MAX];

   bt = elm_button_add(bx);
   snprintf(buf, sizeof(buf), "Item %d", ++_count);
   elm_object_text_set(bt, buf);
   elm_box_pack_end(bx, bt);
   evas_object_smart_callback_add(bt, "clicked", _del_item, NULL);
   evas_object_show(bt);

   _append--;

   if (_append <= 0)
     {
        _timer = NULL;
        return ECORE_CALLBACK_CANCEL;
     }
   else
     return ECORE_CALLBACK_RENEW;
}

static void
_append_items(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   _append += 10;
   _timer = ecore_timer_add(0.3, _append_cb, data);
}

static void
_win_del_cb(void *data EINA_UNUSED,
		Evas *e EINA_UNUSED,
		Evas_Object *obj EINA_UNUSED,
		void *event_info EINA_UNUSED)
{
   ecore_timer_del(_timer);
   _timer = NULL;
}

void
test_scroller3(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bt, *bt2, *bt3, *bx, *bx2, *bx3, *bx4, *sc;
   _count = 0;

   win = elm_win_util_standard_add("scroller3", "Scroller 3");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_event_callback_add(win, EVAS_CALLBACK_DEL, _win_del_cb, NULL);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   bx2 = elm_box_add(bx);
   evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.1);
   evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_horizontal_set(bx2, EINA_TRUE);
   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);

   bt = elm_button_add(bx2);
   elm_object_text_set(bt, "Append Item");
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   bt2 = elm_button_add(bx2);
   elm_object_text_set(bt2, "Prepend Item");
   elm_box_pack_end(bx2, bt2);
   evas_object_show(bt2);

   bt3 = elm_button_add(bx2);
   elm_object_text_set(bt3, "Append 10 Items in 3s");
   elm_box_pack_end(bx2, bt3);
   evas_object_show(bt3);

   bx3 = elm_box_add(bx);
   evas_object_size_hint_align_set(bx3, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bx3, EVAS_HINT_EXPAND, 0.9);
   elm_box_pack_end(bx, bx3);
   evas_object_show(bx3);

   sc = elm_scroller_add(bx3);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx3, sc);
   evas_object_show(sc);

   bx4 = elm_box_add(sc);
   elm_box_padding_set(bx4, 0, 5);
   evas_object_size_hint_weight_set(bx4, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_object_content_set(sc, bx4);
   evas_object_show(bx4);

   evas_object_smart_callback_add(bt, "clicked", _append_item, bx4);
   evas_object_smart_callback_add(bt2, "clicked", _prepend_item, bx4);
   evas_object_smart_callback_add(bt3, "clicked", _append_items, bx4);

   evas_object_resize(win, 500, 500);
   evas_object_show(win);
}

void
test_scroller4(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *sc, *bx, *ly, *sc2, *rect, *rect2;
   char buf[PATH_MAX];

   win = elm_win_util_standard_add("scroller3", "Scroller 3");
   elm_win_autodel_set(win, EINA_TRUE);

   sc = elm_scroller_add(win);
   elm_scroller_loop_set(sc, EINA_TRUE, EINA_FALSE);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_scroller_page_relative_set(sc, 1.0, 0.0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_page_scroll_limit_set(sc, 1, 0);
   elm_win_resize_object_add(win, sc);
   evas_object_show(sc);

   bx = elm_box_add(sc);
   elm_box_horizontal_set(bx, EINA_TRUE);
   elm_object_content_set(sc, bx);
   evas_object_show(bx);

   ly = elm_layout_add(bx);
   snprintf(buf, sizeof(buf), "%s/objects/test.edj", elm_app_data_dir_get());
   elm_layout_file_set(ly, buf, "page_layout");
   elm_object_part_text_set(ly, "text", "Page1");
   elm_box_pack_end(bx, ly);
   evas_object_show(ly);

   sc2 = elm_scroller_add(ly);
   elm_object_part_content_set(ly, "page", sc2);
   evas_object_show(sc2);

   rect2 = evas_object_rectangle_add(evas_object_evas_get(sc2));
   evas_object_color_set(rect2, 50, 0, 0, 50);
   evas_object_size_hint_min_set(rect2, 0, 2000);
   elm_object_content_set(sc2, rect2);
   evas_object_show(rect2);

   ly = elm_layout_add(bx);
   snprintf(buf, sizeof(buf), "%s/objects/test.edj", elm_app_data_dir_get());
   elm_layout_file_set(ly, buf, "page_layout");
   rect = evas_object_rectangle_add(evas_object_evas_get(ly));
   evas_object_color_set(rect, 0, 50, 0, 50);
   elm_object_part_content_set(ly, "page", rect);
   elm_object_part_text_set(ly, "text", "Page2");
   elm_box_pack_end(bx, ly);
   evas_object_show(ly);

   ly = elm_layout_add(bx);
   snprintf(buf, sizeof(buf), "%s/objects/test.edj", elm_app_data_dir_get());
   elm_layout_file_set(ly, buf, "page_layout");
   rect = evas_object_rectangle_add(evas_object_evas_get(ly));
   evas_object_color_set(rect, 0, 0, 50, 50);
   elm_object_part_content_set(ly, "page", rect);
   elm_object_part_text_set(ly, "text", "Page3");
   elm_box_pack_end(bx, ly);
   evas_object_show(ly);

   evas_object_resize(win, 400, 550);
   evas_object_show(win);
}
