/**
 * @defgroup Radio Radio
 * @ingroup elm_widget_group
 *
 * @image html radio_inheritance_tree.png
 * @image latex radio_inheritance_tree.eps
 *
 * @brief Radio is a widget that allows for @c 1 or more options to be displayed
 * and have the user choose only @c 1 of them.
 *
 * A radio object contains an indicator, an optional Label and an optional
 * icon object. While it's possible to have a group of only one radio, they
 * are normally used in groups of @c 2 or more.
 *
 * elm_radio objects are grouped in a slightly different manner compared to other
 * UI toolkits. There is no separate group name/id to remember or manage.
 * The members represent the group there are in. To make a group,
 * use elm_radio_group_add() and pass an existing radio object and the new radio
 * object.
 *
 * The radio object(s) select one from a set
 * of integer values, so any value they are configuring needs to be mapped to
 * a set of integers. To configure what value that radio object represents,
 * use  elm_radio_state_value_set() to set the integer it represents. To set
 * the value the whole group(which one is currently selected) is to indicate
 * use elm_radio_value_set() on any group member, and to get the groups value
 * use elm_radio_value_get(). For convenience the radio objects are also able
 * to directly set an integer(int) to the value that is selected. To specify
 * the pointer to this integer to modify, use elm_radio_value_pointer_set().
 * The radio objects modify this directly. That implies the pointer must
 * point to a valid memory for as long as the radio objects exist.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for radio objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li changed - This is called whenever the user changes the state of one of
 * the radio objects within the group of radio objects that work together.
 *
 * The default text parts of the radio widget that you can use are:
 * @li "default" - Label of the radio.
 *
 * The default content parts of the radio widget that you can use are:
 * @li "icon" - An icon of the radio.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 *
 * @{
 */

/**
 * @brief Adds a new radio to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object                 *elm_radio_add(Evas_Object *parent);

/**
 * @brief Adds this radio to a group of other radio objects.
 *
 * @since_tizen 2.3
 *
 * @remarks Radio objects work in groups. Each member should have a different integer
 *          value assigned. In order to have them work as a group, they need to know
 *          about each other. This adds the given radio object to the group of which
 *          the group object indicated is a member.
 *
 * @param[in] obj The radio object
 * @param[in] group Any object whose group the @a obj is to join
 */
EAPI void                         elm_radio_group_add(Evas_Object *obj, Evas_Object *group);

/**
 * @brief Sets the integer value that this radio object represents.
 *
 * @details This sets the value of the radio.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The radio object
 * @param[in] value The value to use if this radio object is selected
 */
EAPI void                         elm_radio_state_value_set(Evas_Object *obj, int value);

/**
 * @brief Gets the integer value that this radio object represents.
 *
 * @details This gets the value of the radio.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The radio object
 * @return The value used if this radio object is selected
 *
 * @see elm_radio_value_set()
 */
EAPI int                          elm_radio_state_value_get(const Evas_Object *obj);

/**
 * @brief Sets the value of the radio group.
 *
 * @details This sets the value of the radio group and also sets the value if
 *          pointed to, to the value supplied, but does not call any callbacks.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The radio object (any radio object of the group).
 * @param[in] value The value to use for the group
 */
EAPI void                         elm_radio_value_set(Evas_Object *obj, int value);

/**
 * @brief Gets the value of the radio group.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The radio object (any radio object of the group)
 * @return The integer state
 */
EAPI int                          elm_radio_value_get(const Evas_Object *obj);

/**
 * @brief Sets a convenience pointer to an integer to change when the radio group
 *        value changes.
 *
 * @details This sets a pointer to an integer, that, in addition to the radio objects
 *          state is also modified directly. To stop setting the object it points
 *          to simply use @c NULL as the @a valuep argument. If @a valuep is not @c NULL, then
 *          when this is called, the radio objects state is also modified to
 *          reflect the value of the integer that valuep points to, just like calling
 *          elm_radio_value_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The radio object (any object of a group)
 * @param[out] valuep A pointer to the integer to modify
 */
EAPI void                         elm_radio_value_pointer_set(Evas_Object *obj, int *valuep);

/**
 * @brief Gets the selected radio object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Any radio object (any object of a group)
 * @return The selected radio object
 */
EAPI Evas_Object                 *elm_radio_selected_object_get(Evas_Object *obj);

/**
 * @}
 */
