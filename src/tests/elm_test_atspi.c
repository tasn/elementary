#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERNAL_API_ARGESFSDFEFC
#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED

#include <Elementary.h>
#include "elm_priv.h"
#include "elm_suite.h"

static Evas_Object *g_win, *g_btn, *g_bg, *g_btn2, *g_btn3, *g_box, *g_lbl;

void generate_app(void)
{
   g_win = elm_win_add(NULL, "Title", ELM_WIN_BASIC);
   evas_object_geometry_set(g_win, 100, 100, 100, 100);

   g_bg = elm_bg_add(g_win);

   g_btn = elm_button_add(g_win);

   evas_object_show(g_btn);
   evas_object_show(g_bg);
   evas_object_show(g_win);
}

void generate_app2(void)
{
   g_win = elm_win_add(NULL, "Title", ELM_WIN_BASIC);
   evas_object_geometry_set(g_win, 100, 100, 100, 100);

   g_bg = elm_bg_add(g_win);

   g_box = elm_box_add(g_win);

   g_btn = elm_button_add(g_win);
   g_btn2 = elm_button_add(g_win);
   g_btn3 = elm_button_add(g_win);
   g_lbl = elm_label_add(g_win);

   elm_box_pack_end(g_box, g_btn);
   elm_box_pack_end(g_box, g_btn2);
   elm_box_pack_end(g_box, g_btn3);
   elm_box_pack_end(g_box, g_lbl);

   evas_object_show(g_btn);
   evas_object_show(g_bg);
   evas_object_show(g_win);
   evas_object_show(g_box);
   evas_object_show(g_btn2);
   evas_object_show(g_btn3);
   evas_object_show(g_lbl);
}

START_TEST (elm_atspi_name_get)
{
   elm_init(0, NULL);

   Eo* root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

   ck_assert(root != NULL);

   const char *ret = NULL;

   elm_app_name_set("Test name");

   eo_do(root, ret = elm_interface_atspi_accessible_name_get());

   ck_assert_str_eq(ret, "Test name");

   eo_unref(root);
   elm_shutdown();
}
END_TEST

START_TEST (elm_atspi_role_get)
{
   elm_init(0, NULL);

   Eo* root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

   Elm_Atspi_Role role;

   eo_do(root, role = elm_interface_atspi_accessible_role_get());

   ck_assert(role == ELM_ATSPI_ROLE_APPLICATION);

   eo_unref(root);
   elm_shutdown();
}
END_TEST


START_TEST (elm_atspi_role_name_get)
{
   elm_init(0, NULL);

   Eo* root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

   const char *ret = NULL;

   eo_do(root, ret = elm_interface_atspi_accessible_role_name_get());

   ck_assert(ret != NULL);

   eo_unref(root);
   elm_shutdown();
}
END_TEST

START_TEST (elm_atspi_localized_role_name_get)
{
   elm_init(0, NULL);

   Eo* root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

   const char *ret = NULL;

   eo_do(root, ret = elm_interface_atspi_accessible_localized_role_name_get());

   ck_assert(ret != NULL);

   eo_unref(root);
   elm_shutdown();
}
END_TEST

START_TEST (elm_atspi_description_set)
{
   elm_init(0, NULL);

   Eo* root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

   const char *ret = NULL;

   const char *desc = "Test description";

   eo_do(root, ret = elm_interface_atspi_accessible_description_get());

   ck_assert(ret == NULL);

   eo_do(root, elm_interface_atspi_accessible_description_set(desc));

   eo_do(root, ret = elm_interface_atspi_accessible_description_get());

   ck_assert(ret != NULL);
   ck_assert_str_eq(ret, "Test description");

   eo_unref(root);
   elm_shutdown();
}
END_TEST

START_TEST (elm_atspi_children_and_parent)
{
   elm_init(0, NULL);

   generate_app();
   Eo* root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

   Eina_List *child_list = NULL;

   eo_do(root, child_list = elm_interface_atspi_accessible_children_get());

   //eo_do(eina_list_nth(child_list, 0), bg_child_list = elm_interface_atspi_accessible_children_get());

   ck_assert(eina_list_count(child_list) == 1);

   Eo *win = NULL;

   win = eina_list_nth(child_list, 0);

   ck_assert(win != NULL);
   ck_assert(win == g_win);

   eo_unref(root);
   elm_shutdown();
}
END_TEST

START_TEST (elm_atspi_children_and_parent2)
{
   elm_init(0, NULL);

   generate_app();
   Eo* root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

   Eo *win = NULL;

   Eina_List *root_children;
   eo_do(root, root_children = elm_interface_atspi_accessible_children_get());
   win = eina_list_nth(root_children, 0);

   Eina_List *win_children;
   eo_do(win, win_children = elm_interface_atspi_accessible_children_get());

   ck_assert(eina_list_count(win_children) == 2);

   Eo *btn = NULL;

   btn = eina_list_nth(win_children, 0);
   ck_assert(btn != NULL);
   ck_assert(btn == g_bg);

   btn = eina_list_nth(win_children, 1);
   ck_assert(btn != NULL);
   ck_assert(btn == g_btn);

   eo_unref(root);
   elm_shutdown();
}
END_TEST

START_TEST (elm_atspi_elm_widget_custom_relations)
{
   Elm_Atspi_Relation_Set set;
   Elm_Atspi_Relation *rel, *rel_to, *rel_from;
   Eina_List *l;

   elm_init(0, NULL);
   generate_app2();

   eo_do(g_btn, set = elm_interface_atspi_accessible_relation_set_get());
   ck_assert(set == NULL);

   elm_widget_atspi_relationship_append(g_btn, ELM_ATSPI_RELATION_FLOWS_TO, g_lbl);
   elm_widget_atspi_relationship_append(g_btn, ELM_ATSPI_RELATION_FLOWS_FROM, g_win);

   eo_do(g_btn, set = elm_interface_atspi_accessible_relation_set_get());
   ck_assert(set != NULL);
   ck_assert(eina_list_count(set) >= 2);

   rel_to = rel_from = NULL;
   EINA_LIST_FOREACH(set, l, rel)
     {
        if (rel->type == ELM_ATSPI_RELATION_FLOWS_TO)
          rel_to = rel;
        if (rel->type == ELM_ATSPI_RELATION_FLOWS_FROM)
          rel_from = rel;
     }

   ck_assert(rel_to != NULL);
   ck_assert(rel_from != NULL);
   ck_assert(eina_list_data_find(rel_to->objects, g_lbl) != NULL);
   ck_assert(eina_list_data_find(rel_from->objects, g_win) != NULL);

   elm_atspi_relation_set_free(&set);

   elm_widget_atspi_relationship_remove(g_btn, ELM_ATSPI_RELATION_FLOWS_TO, g_lbl);
   eo_do(g_btn, set = elm_interface_atspi_accessible_relation_set_get());
   ck_assert(set != NULL);
   ck_assert(eina_list_count(set) >= 1);

   rel_to = rel_from = NULL;
   EINA_LIST_FOREACH(set, l, rel)
     {
        if (rel->type == ELM_ATSPI_RELATION_FLOWS_TO)
          ck_assert(EINA_FALSE);
     }

   elm_atspi_relation_set_free(&set);

   elm_shutdown();
}
END_TEST

void elm_test_atspi(TCase *tc)
{
   tcase_add_test(tc, elm_atspi_name_get);
   tcase_add_test(tc, elm_atspi_role_get);
   tcase_add_test(tc, elm_atspi_role_name_get);
   tcase_add_test(tc, elm_atspi_localized_role_name_get);
   tcase_add_test(tc, elm_atspi_description_set);
   tcase_add_test(tc, elm_atspi_children_and_parent);
   tcase_add_test(tc, elm_atspi_children_and_parent2);
   tcase_add_test(tc, elm_atspi_elm_widget_custom_relations);
}
