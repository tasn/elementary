//Compile with:
//gcc -o prefs_example_04 prefs_example_04.c -g `pkg-config --cflags --libs elementary`

#ifdef HAVE_CONFIG_H
#include <elementary_config.h>
#endif

#include <Elementary.h>
#include <Efl.h>
#include <Eio.h>
#include <eio_model.eo.h>

static void
_page_saved_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *page = event_info;

   printf("page %s was saved!\n", page);
}

static void
_page_loaded_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *page = event_info;

   printf("page %s was loaded!\n", page);
}

static void
_item_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *item = event_info;

   printf("item %s was changed!\n", item);
}

static Eina_Bool
_filter_cb(void *data EINA_UNUSED,
           Eio_File *handler EINA_UNUSED,
           const Eina_File_Direct_Info *info)
{
   if (info->type == EINA_FILE_DIR &&
       info->path[info->name_start] != '.')
     return EINA_TRUE;

   return EINA_FALSE;
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Evas_Object *win, *prefs;

   win = elm_win_util_standard_add("prefs", "Prefs Example 04");
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   elm_win_autodel_set(win, EINA_TRUE);

   prefs = elm_prefs_add(win);
   evas_object_size_hint_weight_set(prefs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, prefs);
   evas_object_show(prefs);

   evas_object_smart_callback_add(prefs, "page,saved", _page_saved_cb, NULL);
   evas_object_smart_callback_add(prefs, "page,loaded", _page_loaded_cb, NULL);
   evas_object_smart_callback_add(prefs, "item,changed", _item_changed_cb, win);

   Efl_Model_Base *model = eo_add(EIO_MODEL_CLASS, NULL, eio_model_path_set("test"));
   eo_do(model, eio_model_children_filter_set(_filter_cb, NULL));
   eo_do(model, efl_model_load());

   elm_prefs_file_set(prefs, "prefs_example_04.epb", NULL);
   eo_do(prefs, elm_obj_prefs_property_connect("path", "main:text"),
                elm_obj_prefs_model_set(model));

   evas_object_resize(win, 320, 320);
   evas_object_show(win);

   elm_run();

   eo_unref(model);

   return 0;
}
ELM_MAIN()
