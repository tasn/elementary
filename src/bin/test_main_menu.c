#include <Elementary.h>
#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#ifndef ELM_LIB_QUICKLAUNCH

static void
_click_me(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   fputs(":-)\n", stderr);
}

void
test_main_menu(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   Evas_Object *win, *bg, *menu, *label, *bx;
   Elm_Object_Item *menu_it, *menu_it1;
   char *s;
   Eina_Bool enabled = EINA_FALSE;

   win = elm_win_add(NULL, "menu", ELM_WIN_BASIC);
   elm_win_title_set(win, "Menu");
   elm_win_autodel_set(win, EINA_TRUE);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);

   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   label = elm_label_add(win);
   elm_object_text_set(label, "Note: this example requires support from the "
                              "desktop environment to display the application menu");
   evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, label);
   evas_object_show(label);

   s = getenv("ELM_EXTERNAL_MENU");
   if (s)
     enabled = !!atoi(s);

   if (!enabled)
     {
        label = elm_label_add(win);
        elm_object_text_set(label, "(ELM_EXTERNAL_MENU environment variable not "
                                   "set. Test won't display it)");
        evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND,
                                         EVAS_HINT_EXPAND);
        elm_box_pack_end(bx, label);
        evas_object_show(label);
     }

   menu = elm_win_main_menu_get(win);

   menu_it = elm_menu_item_add(menu, NULL, NULL, "first item", NULL, NULL);
   elm_menu_item_add(menu, menu_it, "elementary", "first item", NULL, NULL);
   menu_it1 = elm_menu_item_add(menu, menu_it, NULL, "submenu", NULL, NULL);
   elm_menu_item_add(menu, menu_it1, NULL, "first item", NULL, NULL);
   elm_menu_item_add(menu, menu_it1, "gimp", "second item", NULL, NULL);

   menu_it = elm_menu_item_add(menu, NULL, NULL, "second item", NULL, NULL);
   menu_it1 = elm_menu_item_add(menu, menu_it, NULL, "disabled item", NULL, NULL);
   elm_object_item_disabled_set(menu_it1, EINA_TRUE);
   elm_menu_item_separator_add(menu, menu_it);
   elm_menu_item_add(menu, menu_it, NULL, "click me :-)", _click_me, NULL);
   elm_menu_item_add(menu, menu_it, "applications-email-panel", "third item", NULL, NULL);
   menu_it1 = elm_menu_item_add(menu, menu_it, NULL, "sub menu", NULL, NULL);
   elm_menu_item_add(menu, menu_it1, NULL, "first item", NULL, NULL);

   evas_object_resize(win, 250, 350);
   evas_object_show(win);
}

#endif
