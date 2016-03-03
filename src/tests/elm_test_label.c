#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#include <Elementary.h>
#include "elm_suite.h"


START_TEST (elm_atspi_role_get)
{
   Evas_Object *win, *label;
   Elm_Atspi_Role role;

   elm_init(1, NULL);
   win = elm_win_add(NULL, "label", ELM_WIN_BASIC);

   label = elm_label_add(win);
   role = elm_interface_atspi_accessible_role_get(label);

   ck_assert(role == ELM_ATSPI_ROLE_LABEL);

   elm_shutdown();
}
END_TEST

void elm_test_label(TCase *tc)
{
 tcase_add_test(tc, elm_atspi_role_get);
}
