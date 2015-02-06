typedef Evas_Object* (*Content_Get_Cb)(Evas_Object *par, Eo *item, void *data);

/**
 * @defgroup Settingspane Settingspane
 * @ingroup Elementary
 *
 * This widget aims to displays categorys sub categorys and items.
 *
 * @image html img/widget/settingspane/preview-00.png
 *
 * The way items are working:
 *
 * You can add items to the panel with other items as parent, so you can create a tree structure.
 *
 * If a item has no children (The item was never passed as parent to another item) you can attach a panel to it,
 *
 * Attaching a panel means the Evas_Object returned by the callback will be displayed. If the item gets changed Save and Reset buttons are displayed,
 * if they are pressed the specific callbacks will be called.
 *
 * If a item is marked as changed and another items is focused, the changed item will be appended to a list which you can see in the upper right corner.
 *
 * A rightclick on the back button will bring up a history menu which entrys are visited in the past.
 *
 */

#ifdef EFL_EO_API_SUPPORT
#include "elc_settingspane_eo.h"
#endif
#ifndef EFL_NO_LEGACY_API_SUPPORT
#include "elc_settingspane_legacy.h"
#endif
