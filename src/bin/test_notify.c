/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <Elementary.h>
#ifndef ELM_LIB_QUICKLAUNCH


static void
_bt(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *notify = data;
   evas_object_show(notify);
}

static void
_bt_close(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *notify = data;
   evas_object_hide(notify);
}

   void
test_notify(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *win, *bg, *bx, *bx2, *notify, *bt, *lb;
   char buf[PATH_MAX];

   win = elm_win_add(NULL, "Notify", ELM_WIN_BASIC);
   elm_win_title_set(win, "Notify");
   elm_win_autodel_set(win, 1);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   evas_object_show(bg);

   bx2 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx2, 1.0, 1.0);
   elm_win_resize_object_add(win, bx2);
   evas_object_show(bx2);


   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "This position is the default.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Top");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);



   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM);
   elm_notify_timeout_set(notify, 5);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "Bottom position. This notify use a timeout of 5 sec.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Bottom");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);


   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_LEFT);
   elm_notify_timeout_set(notify, 10);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "Left position. This notify use a timeout of 10 sec.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Left");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_RIGHT);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "Right position.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Right");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);


   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_TOP_LEFT);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "Top Left position.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Top Left");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_TOP_RIGHT);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "Top Right position.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Top Right");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);



   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM_LEFT);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "Bottom Left position.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Bottom Left");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);


   notify = elm_notify_add(win);
   elm_win_resize_object_add(win, notify);
   evas_object_size_hint_weight_set(notify, 1.0, 1.0);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM_RIGHT);

   bx = elm_box_add(win);
   elm_notify_content_set(notify, bx);
   elm_box_horizontal_set(bx, 1);
   evas_object_show(bx);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "Bottom Right position.");
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close, notify);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Bottom Right");
   evas_object_smart_callback_add(bt, "clicked", _bt, notify);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   evas_object_show(win);
}
#endif
