#ifdef HAVE_CONFIG_H
  #include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED

#include <Elementary.h>
#include "elm_widget.h"
#include "elm_priv.h"

extern Eina_List *_elm_win_list;

EOLIAN static Eina_List*
_elm_atspi_app_object_elm_interface_atspi_accessible_children_get(Eo *obj EINA_UNUSED, void *_pd EINA_UNUSED)
{
   Eina_List *l, *accs = NULL;
   Evas_Object *win;

   EINA_LIST_FOREACH(_elm_win_list, l, win)
     {
        Elm_Win_Type type = elm_win_type_get(win);
        if (type != ELM_WIN_SOCKET_IMAGE)
          accs = eina_list_append(accs, win);
     }
   return accs;
}

EOLIAN static char*
_elm_atspi_app_object_elm_interface_atspi_accessible_name_get(Eo *obj EINA_UNUSED, void *_pd EINA_UNUSED)
{
   char *name;

   eo_do_super(obj, ELM_ATSPI_APP_OBJECT_CLASS, name = elm_interface_atspi_accessible_name_get());
   if (name) return name;

   return strdup(elm_app_name_get());
}

EOLIAN static Elm_Atspi_Role
_elm_atspi_app_object_elm_interface_atspi_accessible_role_get(Eo *obj EINA_UNUSED, void *_pd EINA_UNUSED)
{
   return ELM_ATSPI_ROLE_APPLICATION;
}

EOLIAN static void
_elm_atspi_app_object_eo_base_destructor(Eo *obj, void *pd EINA_UNUSED)
{
   eo_do(obj,
         elm_interface_atspi_accessible_description_set(NULL),
         elm_interface_atspi_accessible_name_set(NULL),
         elm_interface_atspi_accessible_translation_domain_set(NULL),
         elm_interface_atspi_accessible_relationships_clear()
         );

   eo_do_super(obj, ELM_ATSPI_APP_OBJECT_CLASS, eo_destructor());
}

#include "elm_atspi_app_object.eo.c"
