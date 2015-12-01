#ifndef ELM_INTERFACE_ATSPI_COMPONENT_H
#define ELM_INTERFACE_ATSPI_COMPONENT_H

#ifdef EFL_BETA_API_SUPPORT

typedef enum
{
   ELM_ATSPI_COMPONENT_CONVERT_SCREEN_2_WINDOW,
   ELM_ATSPI_COMPONENT_CONVERT_WINDOW_2_SCREEN
} Elm_Atspi_Component_Convert_Direction;

void elm_atspi_componenet_coords_convert(Elm_Atspi_Component_Convert_Direction dir, Evas_Object *obj, int *x, int *y);

#ifdef EFL_EO_API_SUPPORT
#include "elm_interface_atspi_component.eo.h"
#endif
#ifndef EFL_NOLEGACY_API_SUPPORT
#include "elm_interface_atspi_component.eo.legacy.h"
#endif

#endif
#endif
