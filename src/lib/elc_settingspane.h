typedef Evas_Object* (*Content_Get_Cb)(Evas_Object *par, Eo *item, void *data);

#ifdef EFL_EO_API_SUPPORT
#include "elc_settingspane_eo.h"
#endif
#ifndef EFL_NO_LEGACY_API_SUPPORT
#include "elc_settingspane_legacy.h"
#endif
