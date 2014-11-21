/**
 * @defgroup Button Button
 * @ingroup elm_widget_group
 *
 * @image html button_inheritance_tree.png
 * @image latex button_inheritance_tree.eps
 *
 * @brief This is a push-button. Press it and run some function. It can contain
 *        a simple label and icon object and it also has an autorepeat feature.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for button objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li "clicked": The user clicked the button (press/release).
 * @li "repeated": The user pressed the button without releasing it.
 * @li "pressed": The button is pressed.
 * @li "unpressed": The button is released after being pressed.
 * In all cases, the @a event parameter of the callback is
 * @c NULL.
 *
 * Also, defined in the default theme, the button has the following styles
 * available:
 * @li default: A normal button.
 * @li anchor: Like default, but the button fades away when the mouse is not
 * over it, leaving only the text or icon.
 * @internal
 * @li hoversel_vertical: Internally used by @ref Hoversel to give a
 * continuous look across its options.
 * @li hoversel_vertical_entry: Another internal for @ref Hoversel.
 * @endinternal
 * @li naviframe: Internally used by @ref Naviframe for its back button.
 * @li colorselector: Internally used by @ref Colorselector
 * for its left and right buttons.
 *
 * The default content parts of the button widget that you can use are:
 * @li "icon" - An icon of the button.
 *
 * The default text parts of the button widget that you can use are:
 * @li "default" - Label of the button.
 *
 * Supported common elm_object APIs.
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
 * @brief Adds a new button to the parent's canvas
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object                 *elm_button_add(Evas_Object *parent);

/**
 * @brief Turns on/off the autorepeat event generated when the button is kept pressed.
 *
 * @since_tizen 2.3
 *
 * @remarks When off, no autorepeat is performed and buttons emit a normal @c clicked
 *          signal when they are clicked.
 *
 * @remarks When on, keeping a button pressed continuously emits a @c repeated
 *          signal until the button is released. The time it takes until it starts
 *          emitting the signal is given by
 *          elm_button_autorepeat_initial_timeout_set(), and the time between each
 *          new emission is given by elm_button_autorepeat_gap_timeout_set().
 *
 * @param[in] obj The button object
 * @param[in] on  The boolean value to turn on/off the event
 */
EAPI void                         elm_button_autorepeat_set(Evas_Object *obj, Eina_Bool on);

/**
 * @brief Gets whether the autorepeat feature is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The button object
 * @return @c EINA_TRUE if autorepeat is on, otherwise @c EINA_FALSE
 *
 * @see elm_button_autorepeat_set()
 */
EAPI Eina_Bool                    elm_button_autorepeat_get(const Evas_Object *obj);

/**
 * @brief Sets the initial timeout before the autorepeat event is generated.
 *
 * @details This sets the timeout, in seconds, since the button is pressed until the
 *          first @c repeated signal is emitted. If @a t is @c 0.0 or less, there
 *          won't be any delay and the event is fired the moment the button is
 *          pressed.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The button object
 * @param[in] t   The timeout in seconds
 *
 * @see elm_button_autorepeat_set()
 * @see elm_button_autorepeat_gap_timeout_set()
 */
EAPI void                         elm_button_autorepeat_initial_timeout_set(Evas_Object *obj, double t);

/**
 * @brief Gets the initial timeout before the autorepeat event is generated.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The button object
 * @return The timeout in seconds
 *
 * @see elm_button_autorepeat_initial_timeout_set()
 */
EAPI double                       elm_button_autorepeat_initial_timeout_get(const Evas_Object *obj);

/**
 * @brief Sets the interval between each generated autorepeat event.
 *
 * @since_tizen 2.3
 *
 * @remarks After the first @c repeated event is fired, all subsequent ones
 *          follow after a delay of @a t seconds for each.
 *
 * @param[in] obj The button object
 * @param[in] t   The interval in seconds
 *
 * @see elm_button_autorepeat_initial_timeout_set()
 */
EAPI void                         elm_button_autorepeat_gap_timeout_set(Evas_Object *obj, double t);

/**
 * @brief Gets the interval between each generated autorepeat event.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The button object
 * @return The interval in seconds
 */
EAPI double                       elm_button_autorepeat_gap_timeout_get(const Evas_Object *obj);

/**
 * @}
 */
