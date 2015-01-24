#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#include <Elementary.h>

static const char* simple_menu[3][3] =
   {
      { "Fonts",
        "Here you can set the Settings of the Fonts, this should be a very very very long text as a description, so someone can also enter something really really long!",
        "./logo_small.png"},
      { "Color",
        "Here you can find the settings for settings up colors bla bla bla text text text",
        "./icon_00.png"},
      { "Themes",
        "Here you can set up themes installed to ~/.yourappsawesomethemefolder/",
        "./icon_07.png",
      }
};

static void
entry_changed(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Elm_Settingspane_Item *it = data;
   elm_settingspane_item_changed_set(it, EINA_TRUE);
}

static void
anchor_clicked(void *data, Evas_Object *obj, void *event_info)
{
   Elm_Entry_Anchor_Hover_Info *ei = event_info;
   Elm_Settingspane_Item *it = NULL;
   const char *name = ei->anchor_info->name;

   if (!strcmp(name, "color"))
     {
        it = elm_settingspane_item_get(data, "Color");
     }
   else if (!strcmp(name, "font"))
     {
        it = elm_settingspane_item_get(data, "Fonts");
     }
   else if (!strcmp(name, "theme"))
     {
        it = elm_settingspane_item_get(data, "Themes");
     }
   elm_entry_anchor_hover_end(obj);
   elm_settingspane_item_focus(it);
}

static Evas_Object*
content_font_get(Evas_Object *par, Elm_Settingspane_Item *item, void *data EINA_UNUSED)
{
   Evas_Object *box, *o;

   box = elm_box_add(par);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

   o = elm_entry_add(par);
   evas_object_size_hint_weight_set(o, 0.0, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(o, "This are the font settings, here you can set up everything regarding font etc. etc. etc. if you want to change fonts color you have to search <a href=color> here </a> Normal options are now following wuu wuu:");
   elm_entry_editable_set(o, EINA_FALSE);
   evas_object_smart_callback_add(o, "anchor,hover,opened", anchor_clicked, par);
   elm_box_pack_end(box, o);
   evas_object_show(o);

   o = elm_entry_add(par);
   elm_object_text_set(o, "A entry");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_single_line_set(o, EINA_TRUE);
   evas_object_smart_callback_add(o, "changed", entry_changed, item);
   elm_box_pack_end(box, o);
   evas_object_show(o);

   return box;
}

static Evas_Object*
content_color_get(Evas_Object *par, Elm_Settingspane_Item *item EINA_UNUSED, void *data EINA_UNUSED)
{
   Evas_Object *box, *o;

   box = elm_box_add(par);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

   o = elm_entry_add(par);
   evas_object_size_hint_weight_set(o, 0.0, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(o, "This are the color settings, here you can setup everything regarding colors, maybe you have been here because you clicked a link in <a href=font> font </a> If you want to change the applications complete look and feel the <a href=theme> theme </a> settings could be more usefull than the colors.");
   elm_entry_editable_set(o, EINA_FALSE);
   evas_object_smart_callback_add(o, "anchor,hover,opened", anchor_clicked, par);
   elm_box_pack_end(box, o);
   evas_object_show(o);

   return box;
}


static Evas_Object*
content_theme_get(Evas_Object *par, Elm_Settingspane_Item *item, void *data EINA_UNUSED)
{
   Evas_Object *box, *o;

   box = elm_box_add(par);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

   o = elm_entry_add(par);
   evas_object_size_hint_weight_set(o, 0.0, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(o, "Theme settings - change the entry to see what happens if you change something.");
   elm_entry_editable_set(o, EINA_FALSE);
   evas_object_smart_callback_add(o, "anchor,hover,opened", anchor_clicked, par);
   elm_box_pack_end(box, o);
   evas_object_show(o);

   o = elm_entry_add(par);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_single_line_set(o, EINA_TRUE);
   elm_object_text_set(o, "Lets think you can set here a theme (:");
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "changed", entry_changed, item);
   evas_object_show(o);

   return box;
}

static Evas_Object*
content_complex_child_get(Evas_Object *par, Elm_Settingspane_Item *item, void *data EINA_UNUSED)
{
   Evas_Object *box, *o;

   box = elm_box_add(par);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

   o = elm_entry_add(par);
   evas_object_size_hint_weight_set(o, 0.0, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(o, "Complex settings - This is just a sample of how a menu structur can be, those can be n examples.");
   elm_entry_editable_set(o, EINA_FALSE);
   evas_object_smart_callback_add(o, "anchor,hover,opened", anchor_clicked, par);
   elm_box_pack_end(box, o);
   evas_object_show(o);

   o = elm_entry_add(par);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_single_line_set(o, EINA_TRUE);
   elm_object_text_set(o, "EFL EFL EFL EFL EFL :) >>>change me<<<");
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "changed", entry_changed, item);
   evas_object_show(o);

   return box;
}

static Content_Get_Cb content_cb[4] =
{
   content_font_get,content_color_get, content_theme_get, content_complex_child_get
};


static void
reset_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Settingspane_Item *item = event_info;
   elm_settingspane_item_changed_set(item, EINA_FALSE);
}

static void
apply_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Settingspane_Item *item = event_info;
   elm_settingspane_item_changed_set(item, EINA_FALSE);
}

static void
highlight_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   elm_settingspane_raise_unsaved(data);
}

static void
save_q_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   if (!elm_settingspane_save_quit(data))
     printf("No.\n");
   else
     printf("Yes :)\n");
}

static void
del_focused_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
  Elm_Settingspane_Item *item;

  item = elm_settingspane_focused_get(data);

  elm_settingspane_item_delete(item);
}


void
test_settingspane(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *win, *o, *box, *sw, *btnbox, *ic;
   Elm_Settingspane_Item *it, *it2;
   char buf[PATH_MAX];

   win = elm_win_util_standard_add("settingspane", "Settingspane");
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_focus_highlight_enabled_set(win, EINA_TRUE);

   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   sw = elm_settingspane_add(win);
   evas_object_size_hint_weight_set(sw, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(sw, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, sw);

   int i = 0;
   for(i = 0; i < 3; i++)
     {
        snprintf(buf, sizeof(buf), "%s/images/%s",
                 elm_app_data_dir_get(), simple_menu[i][2]);
        ic = elm_icon_add(win);
        elm_image_file_set(ic, buf, NULL);
        evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

        it = elm_settingspane_item_append(sw, ic, simple_menu[i][0],
                                          simple_menu[i][1], ic, NULL);
        elm_settingspane_item_attach_panel(it, content_cb[i], reset_cb, apply_cb);
     }

   /* A complex item! */
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/%s",
            elm_app_data_dir_get(), "icon_09.png");
   elm_image_file_set(ic, buf, NULL);
   evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

   it = elm_settingspane_item_append(sw, ic, "Complex",
                                    "No we are not going to jump here into complex numbers, this is just a complex menu item, this means it has childrens!",
                                    ic, NULL);
   elm_settingspane_item_keyword_add(it, eina_stringshare_add("Hardone"));
   elm_settingspane_item_keyword_add(it, eina_stringshare_add("second"));
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/%s",
            elm_app_data_dir_get(), "bubble.png");
   elm_image_file_set(ic, buf, NULL);
   evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
   it2 = elm_settingspane_item_append(sw, ic, "Child",
                                "This is a children entry",
                                ic, it);
   elm_settingspane_item_keyword_add(it2, eina_stringshare_add("Heavyone"));
   elm_settingspane_item_attach_panel(it2, content_cb[3], reset_cb, apply_cb);

   evas_object_show(sw);

   btnbox = elm_box_add(win);
   elm_box_horizontal_set(btnbox, EINA_TRUE);
   evas_object_size_hint_weight_set(btnbox, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(btnbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, btnbox);
   evas_object_show(btnbox);

   o = elm_button_add(win);
   elm_object_text_set(o, "Highlight Unsaved Changes");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(o, "clicked", highlight_cb, sw);
   elm_box_pack_end(btnbox, o);
   evas_object_show(o);

   o = elm_button_add(win);
   elm_object_text_set(o, "Everything Saved ?");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(o, "clicked", save_q_cb, sw);
   elm_box_pack_end(btnbox, o);
   evas_object_show(o);

   o = elm_button_add(win);
   elm_object_text_set(o, "Delete focused item");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(o, "clicked", del_focused_cb, sw);
   elm_box_pack_end(btnbox, o);
   evas_object_show(o);

   evas_object_show(win);
   evas_object_resize(win, 500, 500);
}


