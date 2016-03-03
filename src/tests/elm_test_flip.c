#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#include <Elementary.h>
#include "elm_suite.h"


START_TEST (elm_atspi_role_get)
{
   Evas_Object *win, *flip;
   Elm_Atspi_Role role;

   elm_init(1, NULL);
   win = elm_win_add(NULL, "flip", ELM_WIN_BASIC);

   flip = elm_flip_add(win);
   role = elm_interface_atspi_accessible_role_get(flip);

   ck_assert(role == ELM_ATSPI_ROLE_PAGE_TAB_LIST);

   elm_shutdown();
}
END_TEST

void elm_test_flip(TCase *tc)
{
 tcase_add_test(tc, elm_atspi_role_get);
}
