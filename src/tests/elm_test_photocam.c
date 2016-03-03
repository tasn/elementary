#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#include <Elementary.h>
#include "elm_suite.h"


START_TEST (elm_atspi_role_get)
{
   Evas_Object *win, *photocam;
   Elm_Atspi_Role role;

   elm_init(1, NULL);
   win = elm_win_add(NULL, "photocam", ELM_WIN_BASIC);

   photocam = elm_photocam_add(win);
   role = elm_interface_atspi_accessible_role_get(photocam);

   ck_assert(role == ELM_ATSPI_ROLE_IMAGE);

   elm_shutdown();
}
END_TEST

void elm_test_photocam(TCase *tc)
{
 tcase_add_test(tc, elm_atspi_role_get);
}
