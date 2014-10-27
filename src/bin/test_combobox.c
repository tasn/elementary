#include "test.h"
#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#include <Elementary.h>

static void
_combobox_clicked_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
                     void *event_info EINA_UNUSED)
{
   printf("Hover button is clicked and 'clicked' callback is called.\n");
}

static void
_combobox_selected_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
                      void *event_info)
{
   const char *txt = elm_object_item_text_get(event_info);

   printf("'selected' callback is called. (selected item : %s)\n", txt);
   elm_object_text_set(obj, txt);
}

static void
_combobox_dismissed_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
                       void *event_info EINA_UNUSED)
{
   printf("'dismissed' callback is called.\n");
}

static void
_combobox_expanded_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
                       void *event_info EINA_UNUSED)
{
   printf("'expanded' callback is called.\n");
}

void
test_combobox(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bx, *combobox;

   win = elm_win_util_standard_add("combobox", "Combobox");
   elm_win_autodel_set(win, EINA_TRUE);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   combobox = elm_combobox_add(win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
// FIXME: need to add horizontal combobox some day
//   elm_combobox_horizontal_set(bt, EINA_TRUE);
   elm_combobox_hover_parent_set(combobox, win);
   elm_object_text_set(combobox, "Labels");
   // FIXME: add some item
   evas_object_smart_callback_add(combobox, "clicked",
                                  _combobox_clicked_cb, NULL);
   evas_object_smart_callback_add(combobox, "selected",
                                  _combobox_selected_cb, NULL);
   evas_object_smart_callback_add(combobox, "dismissed",
                                  _combobox_dismissed_cb, NULL);
   evas_object_smart_callback_add(combobox, "expanded",
                                  _combobox_expanded_cb, NULL);
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "Some Icons");
   // FIXME: add some item
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "All Icons");
   // FIXME: add some item
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "All Icons");
   // FIXME: add some item
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "Disabled Combobox");
   // FIXME: add some items
   elm_object_disabled_set(combobox, EINA_TRUE);
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "Icon + Label");
   // FIXME: add some items with icon
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "Custom Item Style");
   // FIXME: add custom style
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   evas_object_resize(win, 320, 500);

   evas_object_show(win);
}
