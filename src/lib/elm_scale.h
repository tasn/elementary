/**
 * @defgroup Scaling Widget Scaling
 * @ingroup elm_infra_group
 *
 * @brief Different widgets can be scaled independently. These functions
 *        allow you to manipulate this scaling on a per-widget basis. The
 *        object and all its children get their scaling factors multiplied
 *        by the scale factor set. This is multiplicative, in that if a
 *        child also has a scale size set, it is in turn multiplied by its
 *        parent's scale size. @c 1.0 means “don't scale”, @c 2.0 is
 *        double size, @c 0.5 is half, etc.
 *
 */

/**
 * @brief Sets the scaling factor for a given Elementary object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary to operate on
 * @param[in] scale The scale factor (from @c 0.0 up, with @c 1.0 meaning
 *              no scaling)
 *
 * @ingroup Scaling
 */
EAPI void   elm_object_scale_set(Evas_Object *obj, double scale);

/**
 * @brief Gets the scaling factor for a given Elementary object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 * @return The scaling factor set by elm_object_scale_set()
 *
 * @ingroup Scaling
 */
EAPI double elm_object_scale_get(const Evas_Object *obj);
