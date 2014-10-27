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

static char *
gl_text_get(void *data, Evas_Object *obj EINA_UNUSED, const char *part EINA_UNUSED)
{
   char buf[256];
   snprintf(buf, sizeof(buf), "Item # %i", (int)(uintptr_t)data);
   return strdup(buf);
}

static Evas_Object *gl_content_get(void *data EINA_UNUSED, Evas_Object *obj, const char *part)
{
   char buf[PATH_MAX];
   Evas_Object *ic = elm_icon_add(obj);
   if (!strcmp(part, "elm.swallow.end"))
     snprintf(buf, sizeof(buf), "%s/images/bubble.png", elm_app_data_dir_get());
   else
     snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_image_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   return ic;
}

static Eina_Bool gl_state_get(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, const char *part EINA_UNUSED)
{
   return EINA_FALSE;
}

static char *gl_group_text_get(void *data, Evas_Object *obj EINA_UNUSED, const char *part EINA_UNUSED)
{
   char buf[256];
   snprintf(buf, sizeof(buf), "Group Index # %i (Item # %i)", (int)((uintptr_t)data / 10), (int)(uintptr_t)data);
   return strdup(buf);
}

static void
gl_sel(void *data, Evas_Object *obj, void *event_info)
{
   printf("sel item data [%p] on genlist obj [%p], item pointer [%p]\n", data, obj, event_info);
}

void
test_combobox(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *bx, *combobox;
   Elm_Genlist_Item_Class *itc;
   Elm_Genlist_Item_Class *itg;
   Elm_Object_Item *gli, *git = NULL;
   int i;

   win = elm_win_util_standard_add("combobox", "Combobox");
   elm_win_autodel_set(win, EINA_TRUE);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   itc = elm_genlist_item_class_new();
   itc->item_style = "default";
   itc->func.text_get = gl_text_get;
   itc->func.content_get = gl_content_get;
   itc->func.state_get = gl_state_get;
   itc->func.del = NULL;

   itg = elm_genlist_item_class_new();
   itg->item_style = "group_index";
   itg->func.text_get = gl_group_text_get;
   itg->func.content_get = NULL;
   itg->func.state_get = NULL;
   itg->func.del = NULL;

   combobox = elm_combobox_add(win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
// FIXME: need to add horizontal combobox some day
//   elm_combobox_horizontal_set(bt, EINA_TRUE);
   elm_combobox_hover_parent_set(combobox, win);
   elm_object_text_set(combobox, "Labels");
   for (i = 0; i < 10; i++)
     gli = elm_genlist_item_append(combobox, itc, (void *)(uintptr_t)i,
                                   NULL, ELM_GENLIST_ITEM_NONE, gl_sel,
                                   (void*)(uintptr_t)(i * 10));
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
   fprintf(stderr, "done\n");

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "Simple genlist");

   // FIXME: add some item
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "Group genlist");
   for (i = 0; i < 200; i++)
     {
        if (!(i % 10))
          {
             git = gli = elm_genlist_item_append(combobox, itg,
                                                 (void*)(uintptr_t)i,
                                                 NULL,
                                                 ELM_GENLIST_ITEM_GROUP,
                                                 gl_sel,
                                                 (void*)(uintptr_t)(i * 10));
             elm_genlist_item_select_mode_set(git, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
          }
        else
          {
             gli = elm_genlist_item_append(combobox, itc,
                                           (void*)(uintptr_t)i,
                                           git,
                                           ELM_GENLIST_ITEM_NONE,
                                           gl_sel,
                                           (void*)(uintptr_t)(i * 10));
          }
     }
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "Tree genlist");
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
   elm_object_text_set(combobox, "Empty genlist");
   // FIXME: add some items with icon
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   combobox = elm_combobox_add(win);
   elm_combobox_hover_parent_set(combobox, win);
   evas_object_size_hint_weight_set(combobox, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(combobox, EVAS_HINT_FILL, 0);
   elm_object_text_set(combobox, "A long list");
   // FIXME: add custom style
   elm_box_pack_end(bx, combobox);
   evas_object_show(combobox);

   evas_object_resize(win, 320, 500);

   evas_object_show(win);
}
