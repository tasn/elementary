/**
 * @defgroup Mirroring Mirroring
 * @ingroup elm_infra_group
 *
 * @brief These functions allow you to set UI-mirroring on specific
 *        widgets or the whole interface. Widgets can be in one of the following two
 *        modes, automatic and manual. Automatic means they are changed
 *        according to the system mirroring mode and manual means only
 *        explicit changes matter. You are not supposed to change the
 *        mirroring state of a widget set to automatic, that mostly works,
 *        but the behavior is not really defined.
 *
 * @{
 */

/**
 * @brief Gets the widget's mirrored mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The widget
 * @return @c EINA_TRUE if mirrored is set, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool elm_object_mirrored_get(const Evas_Object *obj);

/**
 * @brief Sets the widget's mirrored mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The widget
 * @param[in] mirrored If @c EINA_TRUE the mirrored mode is set, otherwise @c EINA_FALSE to unset it
 */
EAPI void      elm_object_mirrored_set(Evas_Object *obj, Eina_Bool mirrored);

/**
 * @brief Gets the widget's mirrored mode setting.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The widget
 * @return The mirrored mode setting of the object
 */
EAPI Eina_Bool elm_object_mirrored_automatic_get(const Evas_Object *obj);

/**
 * @brief Sets the widget's mirrored mode setting.
 *
 * @since_tizen 2.3
 *
 * @remarks When the widget is in the automatic mode, it follows the system mirrored mode set by
 *          elm_mirrored_set().
 *
 * @param[in] obj The widget
 * @param[in] automatic If @c EINA_TRUE the auto mirrored mode is set, otherwise @c EINA_FALSE for manual
 */
EAPI void      elm_object_mirrored_automatic_set(Evas_Object *obj, Eina_Bool automatic);

/**
 * @}
 */
