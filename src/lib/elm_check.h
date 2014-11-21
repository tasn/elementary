/**
 * @defgroup Check Check
 * @ingroup elm_widget_group
 *
 * @image html check_inheritance_tree.png
 * @image latex check_inheritance_tree.eps
 *
 * @brief The check widget allows for toggling a value between @c true
 *        and @c false.
 *
 * Check objects are a lot like radio objects in the layout and
 * functionality, except that they do not work as a group, but
 * independently, and only toggle the value of a boolean between @c false
 * and @c true. elm_check_state_set() sets the boolean state and
 * elm_check_state_get() returns the current state. For convenience,
 * like the radio objects, you can set a pointer to a boolean directly
 * with elm_check_state_pointer_set() for it to modify.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for check objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - This is called whenever the user changes the state of
 *             the check objects (@a event_info is always @c NULL).
 *
 * The default content parts of the check widget that you can use are:
 * @li "icon" - An icon of the check.
 *
 * The default text parts of the check widget that you can use are:
 * @li "default" - A label of the check.
 * @li "on" - On state label of the check.
 * @li "off" - Off state label of the check.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 * @li @ref elm_object_signal_emit
 * @li @ref elm_object_signal_callback_add
 * @li @ref elm_object_signal_callback_del
 *
 * @{
 */

/**
 * @brief Adds a new Check object.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object *                elm_check_add(Evas_Object *parent);

/**
 * @brief Sets the on/off state of the check object.
 *
 * @details This sets the state of the check. If set with elm_check_state_pointer_set()
 *          the state of that variable is also changed. Calling this @b doesn't cause
 *          the "changed" signal to be emitted.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The check object
 * @param[in] state The state to use (@c 1 == on, @c 0 == off)
 */
EAPI void                         elm_check_state_set(Evas_Object *obj, Eina_Bool state);

/**
 * @brief Gets the state of the check object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The check object
 * @return The boolean state
 */
EAPI Eina_Bool                    elm_check_state_get(const Evas_Object *obj);

/**
 * @brief Sets a convenience pointer to a boolean to change.
 *
 * @details This sets a pointer to a boolean, that, in addition to the check objects
 *          state is also modified directly. To stop setting the object
 *          to simply use @c NULL as the @a statep parameter. If @a statep is not @c NULL,
 *          then when this is called, the check objects state is also modified to
 *          reflect the value of the boolean that @a statep points to, just like calling
 *          elm_check_state_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The check object
 * @param[in] statep A pointer to the boolean to modify
 */
EAPI void                         elm_check_state_pointer_set(Evas_Object *obj, Eina_Bool *statep);

/**
 * @}
 */
