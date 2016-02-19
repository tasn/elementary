#include <Elementary.h>
#include "elm_priv.h"

#include "elm_focuswidget.eo.h"
#include "elm_focusable.eo.h"
#include "elm_focus_manager.eo.h"
#include "elm_layout.eo.h"
typedef struct {
    Eina_Bool focusable;
    Eina_Bool focus;
} Elm_Focuswidget_Data;


EOLIAN static void
_elm_focuswidget_elm_focusable_focus_set(Eo *obj, Elm_Focuswidget_Data *pd, Eina_Bool focus)
{
    if (!pd->focusable) return;

    pd->focus = focus;

    eo_do(obj, eo_event_callback_call(pd->focus ? ELM_FOCUSABLE_EVENT_FOCUS_ON : ELM_FOCUSABLE_EVENT_FOCUS_OFF, NULL));
}


EOLIAN static Eina_Bool
_elm_focuswidget_elm_focusable_focus_get(Eo *obj, Elm_Focuswidget_Data *pd)
{
    return pd->focus;
}


EOLIAN static void
_elm_focuswidget_elm_focusable_focusable_set(Eo *obj, Elm_Focuswidget_Data *pd, Eina_Bool focusable)
{
    pd->focusable = focusable;

    if (!focusable)
      eo_do(obj, elm_focusable_focus_set(EINA_FALSE));
}


EOLIAN static Eina_Bool
_elm_focuswidget_elm_focusable_focusable_get(Eo *obj, Elm_Focuswidget_Data *pd)
{
    return pd->focusable;
}

EOLIAN static Eo*
_elm_focuswidget_eo_base_finalize(Eo *obj, Elm_Focuswidget_Data *pd)
{
   Eo *eo;
   Eo *nextParent;
   eo_do_super(obj, ELM_FOCUSWIDGET_CLASS, eo = eo_finalize());

   eo_do(obj, nextParent = eo_parent_get());

   do {
     if (eo_isa(nextParent, ELM_FOCUS_MANAGER_INTERFACE))
       {
          eo_do(nextParent, elm_focus_manager_register_simple(obj));
          break;
       }
     eo_do(nextParent, nextParent = eo_parent_get());
   } while (nextParent);

   if (!nextParent)
     printf("Failed to find the highest focus manager\n");

   return eo;
}


#include "elm_focuswidget.eo.c"