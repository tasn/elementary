/**
 * @internal
 * @defgroup Separator Separator
 * @ingroup elm_widget_group
 *
 * @image html separator_inheritance_tree.png
 * @image latex separator_inheritance_tree.eps
 *
 * @brief Separator is a very thin object used to separate other objects.
 *
 * @remarks A separator can be vertical or horizontal.
 *
 * @remarks This widget inherits from the @ref Layout one, so that all the
 *          functions acting on it also work for separator objects.
 *
 * @remarks This widget emits the signals coming from @ref Layout.
 *
 * @{
 */

/**
 * @brief Adds a separator object to @a parent.
 *
 * @param[in] parent The parent object
 *
 * @return The separator object, otherwise @c NULL on failure
 */
EAPI Evas_Object *elm_separator_add(Evas_Object *parent);

/**
 * @brief Sets the horizontal mode of a separator object.
 *
 * @param[in] obj The separator object
 * @param[in] horizontal If @c true the separator is horizontal,
 *                   otherwise @c false
 */
EAPI void      elm_separator_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Gets the horizontal mode of a separator object.
 *
 * @param[in] obj The separator object
 * @return If @c true the separator is horizontal,
 *         otherwise @c false
 *
 * @see elm_separator_horizontal_set()
 */
EAPI Eina_Bool elm_separator_horizontal_get(const Evas_Object *obj);

/**
 * @}
 */
