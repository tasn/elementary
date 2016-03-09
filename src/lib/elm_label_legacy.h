/**
 * @brief Add a new label to the parent
 *
 * @param parent The parent object
 * @return The new object or NULL if it cannot be created
 *
 * @ingroup Label
 */
EAPI Evas_Object                *elm_label_add(Evas_Object *parent);
EAPI void                        elm_label_line_wrap_set(Evas_Object *obj, Elm_Wrap_Type wrap);
EAPI Elm_Wrap_Type               elm_label_line_wrap_get(const Evas_Object *obj);
EAPI void                        elm_label_ellipsis_set(Evas_Object *obj, Eina_Bool ellipsis);
EAPI Eina_Bool                   elm_label_ellipsis_get(const Evas_Object *obj);

#include "elm_label.eo.legacy.h"
