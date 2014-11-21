/**
 * @defgroup Spinner Spinner
 * @ingroup elm_widget_group
 *
 * @image html spinner_inheritance_tree.png
 * @image latex spinner_inheritance_tree.eps
 *
 * @brief A spinner is a widget which allows the user to increase or decrease
 *        numeric values using arrow buttons, or edit values directly by
 *        clicking over it and typing the new value.
 *
 * By default, the spinner does not wrap and has a label
 * of "%.0f" (just showing the integer value of double).
 *
 * A spinner has a label that is formatted with floating
 * point values and thus accepts a printf-style format string, like
 * “%1.2f units”.
 *
 * It also allows specific values to be replaced by pre-defined labels.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for spinner objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - Whenever the spinner value is changed.
 * - @c "delay,changed" - A short time after the value is changed by
 *    the user.  This is called only when the user stops dragging
 *    for a very short period or when they release their finger/mouse,
 *    so it avoids the possible expensive reactions to the value change.
 * - @c "language,changed" - The program's language is changed.
 *
 * Available styles for it:
 * - @c "default";
 * - @c "vertical": Up/down buttons on the right side and text aligned to the left.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_signal_emit
 * @li @ref elm_object_signal_callback_add
 * @li @ref elm_object_signal_callback_del
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 *
 * @{
 */

/**
 * @brief Adds a new spinner widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new spinner widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new spinner widget handle, otherwise @c NULL in case of an error
 *
 */
EAPI Evas_Object *elm_spinner_add(Evas_Object *parent);

/**
 * @brief Sets the format string of the displayed label.
 *
 * @since_tizen 2.3
 *
 * @remarks If @c NULL, this sets the format to "%.0f". If not, it sets the format
 *          string for the label text. For label text a floating point value is provided,
 *          so the label text can display up to @c 1 floating point value.
 *          Note that this is optional.
 *
 * @remarks Use a format string such as "%1.2f meters" for example, and it
 *          displays values like: "3.14 meters" for a value equal to 3.14159.
 *
 * @remarks Default is "%0.f".
 *
 * @param[in] obj The spinner object
 * @param[in] fmt The format string for the label display
 *
 * @see elm_spinner_label_format_get()
 */
EAPI void        elm_spinner_label_format_set(Evas_Object *obj, const char *fmt);

/**
 * @brief Gets the label format of the spinner.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @return The text label format string in UTF-8
 *
 * @see elm_spinner_label_format_set()
 */
EAPI const char *elm_spinner_label_format_get(const Evas_Object *obj);

/**
 * @brief Sets the minimum and maximum values for the spinner.
 *
 * @details This defines the allowed range of values to be selected by the user.
 *
 * @since_tizen 2.3
 *
 * @remarks If the actual value is less than @a min, it is updated to @a min. If it
 *          is bigger then @a max, it is updated to @a max. Actual value can be
 *          obtained with elm_spinner_value_get().
 *
 * @remarks By default, @a min is equal to @c 0, and @a max is equal to @c 100.
 *
 * @remarks Maximum must be greater than minimum.
 *
 * @param[in] obj The spinner object
 * @param[in] min The minimum value
 * @param[in] max The maximum value
 *
 * @see elm_spinner_min_max_get()
 */
EAPI void        elm_spinner_min_max_set(Evas_Object *obj, double min, double max);

/**
 * @brief Gets the minimum and maximum values of the spinner.
 *
 * @since_tizen 2.3
 *
 * @remarks If only one value is needed, the other pointer can be passed
 *          as @c NULL.
 *
 * @param[in] obj The spinner object
 * @param[out] min The pointer to store the minimum value
 * @param[out] max The pointer to store the maximum value
 *
 * @see elm_spinner_min_max_set()
 */
EAPI void        elm_spinner_min_max_get(const Evas_Object *obj, double *min, double *max);

/**
 * @brief Sets the step used to increment or decrement the spinner value.
 *
 * @since_tizen 2.3
 *
 * @remarks This value is incremented or decremented to the displayed value.
 *          It is incremented while the user keeps the right or top arrow pressed,
 *          and is decremented while the user keeps the left or bottom arrow pressed.
 *
 * @remarks The interval to increment / decrement can be set using
 *          elm_spinner_interval_set().
 *
 * @remarks By default the step value is equal to @c 1.
 *
 * @param[in] obj The spinner object
 * @param[in] step The step value
 *
 * @see elm_spinner_step_get()
 */
EAPI void        elm_spinner_step_set(Evas_Object *obj, double step);

/**
 * @brief Gets the step used to increment or decrement the spinner value.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @return The step value
 *
 * @see elm_spinner_step_get()
 */
EAPI double      elm_spinner_step_get(const Evas_Object *obj);

/**
 * @brief Sets the value that the spinner displays.
 *
 * @since_tizen 2.3
 *
 * @remarks The value is presented on the label following the format specified by
 *          elm_spinner_format_set().
 *
 * @remarks The value must to be between the @a min and @a max values. These values
 *          are set by elm_spinner_min_max_set().
 *
 * @param[in] obj The spinner object
 * @param[in] val The value to be displayed
 *
 * @see elm_spinner_value_get()
 * @see elm_spinner_format_set()
 * @see elm_spinner_min_max_set()
 */
EAPI void        elm_spinner_value_set(Evas_Object *obj, double val);

/**
 * @brief Gets the value displayed by the spinner.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @return The value displayed
 *
 * @see elm_spinner_value_set()
 */
EAPI double      elm_spinner_value_get(const Evas_Object *obj);

/**
 * @brief Sets whether the spinner should wrap when it reaches its
 *        minimum or maximum value.
 *
 * @since_tizen 2.3
 *
 * @remarks It is disabled by default. If disabled when the user tries to increment the
 *          value, but the displayed value plus the step value is bigger than the maximum value,
 *          the new value is the maximum value.
 *          The same happens when the user tries to decrement it,
 *          but the value less than step is less than the minimum value. In this case,
 *          the new displayed value is the minimum value.
 *
 * @remarks If wrap is enabled when the user tries to increment the value,
 *          but the displayed value plus the step value is bigger than the maximum value,
 *          the new value is the minimum value. When the the user tries to
 *          decrement it, but the value less step is less than the minimum value,
 *          the new displayed value is the maximum value.
 *
 *          E.g.:
 *          @li min value = 10
 *          @li max value = 50
 *          @li step value = 20
 *          @li displayed value = 20
 *
 *          When the user decrements the value (using left or bottom arrow), it
 *          displays @c 50.
 *
 * @param[in] obj The spinner object
 * @param[in] wrap If @c EINA_TRUE wrap is enabled, otherwise @c EINA_FALSE to
 *             disable it
 *
 * @see elm_spinner_wrap_get()
 */
EAPI void        elm_spinner_wrap_set(Evas_Object *obj, Eina_Bool wrap);

/**
 * @brief Gets whether the spinner should wrap when it reaches its
 *        minimum or maximum value.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @return @c EINA_TRUE indicates that wrap is enabled, otherwise @c EINA_FALSE indicates that it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_spinner_wrap_set()
 */
EAPI Eina_Bool   elm_spinner_wrap_get(const Evas_Object *obj);

/**
 * @brief Sets whether the spinner can be directly edited by the user.
 *
 * @since_tizen 2.3
 *
 * @remarks Spinner objects can have an edition @b disabled, in which case they can
 *          be changed only by arrows.
 *          Useful for contexts
 *          where you don't want your users to interact by writing the value.
 *          Specially when using special values, the user can see real values instead
 *          of special labels on the edition.
 *
 * @remarks It's enabled by default.
 *
 * @param[in] obj The spinner object
 * @param[in] editable If @c EINA_TRUE allow users to edit it, otherwise @c EINA_FALSE to
 *                 not allow users to edit it directly
 *
 * @see elm_spinner_editable_get()
 */
EAPI void        elm_spinner_editable_set(Evas_Object *obj, Eina_Bool editable);

/**
 * @brief Gets whether the spinner can be directly edited by the user.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @return @c EINA_TRUE indicates that the edition is enabled, otherwise @c EINA_FALSE indicates that it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_spinner_editable_set()
 */
EAPI Eina_Bool   elm_spinner_editable_get(const Evas_Object *obj);

/**
 * @brief Sets a special string to display in place of the numerical value.
 *
 * @since_tizen 2.3
 *
 * @remarks It's useful for cases when a user should select an item that is
 *          better indicated by a label than a value. For example, weekdays or months.
 *
 * E.g.:
 * @code
 * sp = elm_spinner_add(win);
 * elm_spinner_min_max_set(sp, 1, 3);
 * elm_spinner_special_value_add(sp, 1, "January");
 * elm_spinner_special_value_add(sp, 2, "February");
 * elm_spinner_special_value_add(sp, 3, "March");
 * evas_object_show(sp);
 * @endcode
 *
 * @remarks If another label was previously set to @a value, it is replaced
 *          by the new label.
 *
 * @param[in] obj The spinner object
 * @param[in] value The value to be replaced
 * @param[in] label The label to be used
 *
 * @see elm_spinner_special_value_get()
 * @see elm_spinner_special_value_del()
 */
EAPI void        elm_spinner_special_value_add(Evas_Object *obj, double value, const char *label);

/**
 * @brief Deletes the special string displayed in place of the numerical value.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks It removes a previously added special value. After this, the spinner
 *          displays the value itself instead of a label.
 *
 * @param[in] obj The spinner object
 * @param[in] value The replaced value
 *
 * @see elm_spinner_special_value_add()
 */
EAPI void elm_spinner_special_value_del(Evas_Object *obj, double value);

/**
 * @brief Gets the special string displayed in place of the numerical value.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @param[in] value The replaced value
 * @return The used label
 *
 * @see elm_spinner_special_value_add()
 */
EAPI const char *elm_spinner_special_value_get(Evas_Object *obj, double value);

/**
 * @brief Sets the interval on the time updates for a user's mouse button hold
 *        on the spinner widgets' arrows.
 *
 * @since_tizen 2.3
 *
 * @remarks This interval value is @b decreased while the user holds the
 *          mouse pointer either by incrementing or decrementing the spinner's value.
 *
 * @remarks This helps the user to get to a given value, which is distant from the
 *          current one, in an easier/faster way, as it updates the value in that set interval time
 *          on mouse button holds.
 *
 * @remarks The default starting interval value for automatic changes is
 *          @c 0.85 seconds.
 *
 * @param[in] obj The spinner object
 * @param[in] interval The interval value in seconds
 *
 * @see elm_spinner_interval_get()
 */
EAPI void        elm_spinner_interval_set(Evas_Object *obj, double interval);

/**
 * @brief Gets the interval on the time updates for a user's mouse button hold
 *        on the spinner widgets' arrows.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @return The interval value, in seconds, set on it
 *
 * @see elm_spinner_interval_set()
 */
EAPI double      elm_spinner_interval_get(const Evas_Object *obj);

/**
 * @brief Sets the base for rounding.
 *
 * @since_tizen 2.3
 *
 * @remarks Rounding works as follows:
 *
 *          rounded_val = base + (double)(((value - base) / round) * round)
 *
 *          Where rounded_val, value, and base are doubles, and round is an integer.
 *
 *          This means that things are rounded to increments (or decrements) of
 *          "round" starting from the value @a base. The default base for rounding is @c 0.
 *
 *          Example: round = 3, base = 2
 *          Values:  3, 6, 9, 12, 15, ...
 *
 *          Example: round = 2, base = 5.5
 *          Values: 5.5, 7.5, 9.5, 11.5, ...
 *
 * @param[in] obj The spinner object
 * @param[in] base The base value
 *
 * @see elm_spinner_round_get()
 * @see elm_spinner_base_get()
 */
EAPI void elm_spinner_base_set(Evas_Object *obj, double base);

/**
 * @brief Gets the base for rounding.
 *
 * @since_tizen 2.3
 *
 * @remarks This returns the base for rounding
 *
 * @param[in] obj The spinner object
 * @return The base rounding value
 *
 * @see elm_spinner_round_set()
 * @see elm_spinner_base_set()
 */
EAPI double elm_spinner_base_get(const Evas_Object *obj);

/**
 * @brief Sets the round value for rounding.
 *
 * @details This sets the rounding value used for the value rounding in the spinner.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @param[in] rnd The rounding value
 *
 * @see elm_spinner_round_get()
 * @see elm_spinner_base_set()
 */
EAPI void elm_spinner_round_set(Evas_Object *obj, int rnd);

/**
 * @brief Gets the round value for rounding.
 *
 * @details This returns the round value for rounding.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The spinner object
 * @return The rounding value
 *
 * @see elm_spinner_round_set()
 * @see elm_spinner_base_set()
 */
EAPI int elm_spinner_round_get(const Evas_Object *obj);
/**
 * @}
 */
