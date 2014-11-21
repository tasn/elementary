/**
 * @defgroup Slider Slider
 * @ingroup elm_widget_group
 *
 * @image html slider_inheritance_tree.png
 * @image latex slider_inheritance_tree.eps
 *
 * @brief The slider adds a draggable “slider” widget for selecting the value 
 *        of something within a range.
 *
 * A slider can be horizontal or vertical. It can contain an Icon and can have a
 * primary label as well as a units label (that is formatted with floating
 * point values and thus accepts a printf-style format string, like
 * “%1.2f units”. There is also an indicator string that may be somewhere
 * else (like on the slider itself) that also accepts a format string like
 * units. Label, Icon Unit, and Indicator strings/objects are optional.
 *
 * A slider may be inverted, which means values invert, with high values being
 * on the left or top and low values on the right or bottom (as opposed to
 * normally being low on the left or top and high at the bottom and right).
 *
 * The slider should have its minimum and maximum values set by the
 * application with  elm_slider_min_max_set() and values should also be set by
 * the application before it is used with  elm_slider_value_set(). The span of the
 * slider is its length (horizontally or vertically). This is scaled by
 * the object or applications scaling factor. At any point, the code can query the
 * slider for its value using elm_slider_value_get().
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for slider objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - Whenever the slider value is changed by the user.
 * - @c "slider,drag,start" - Dragging the slider indicator has started.
 * - @c "slider,drag,stop" - Dragging the slider indicator has stopped.
 * - @c "delay,changed" - A short time after the value is changed by the user.
 * This is called only when the user stops dragging for
 * a very short period or when they release their
 * finger/mouse, so it avoids the possible expensive reactions to
 * the value change.
 *
 * Available styles for it:
 * - @c "default"
 *
 * The default content parts of the slider widget that you can use are:
 * @li "icon" - An icon of the slider.
 * @li "end" - An end part content of the slider.
 *
 * The default text parts of the slider widget that you can use are:
 * @li "default" - Label of the slider.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * @{
 */

/**
 * @brief Adds a new slider widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new slider widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new slider widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object                 *elm_slider_add(Evas_Object *parent);

/**
 * @brief Sets the (exact) length of the bar region of a given slider widget.
 *
 * @details This sets the minimum width (when in the horizontal mode) or height
 *          (when in the vertical mode) of the actual bar area of the slider
 *          @a obj. This in turn affects the object's minimum size. Use
 *          this when you're not setting other size hints expanding on the
 *          given direction (like weight and alignment hints), and you would
 *          like it to have a specific size.
 *
 * @since_tizen 2.3
 *
 * @remarks Icon, end, label, indicator, and unit text around @a obj
 *          require their own space, which makes @a obj require more than @a size,
 *          actually.
 *
 * @param[in] obj The slider object
 * @param[in] size The length of the slider's bar region
 *
 * @see elm_slider_span_size_get()
 */
EAPI void                         elm_slider_span_size_set(Evas_Object *obj, Evas_Coord size);

/**
 * @brief Gets the length set for the bar region of a given slider widget.
 *
 * @since_tizen 2.3
 *
 * @remarks If that size is not set previously, with
 *          elm_slider_span_size_set(), this call returns @c 0.
 *
 * @param[in] obj The slider object
 * @return The length of the slider's bar region
 */
EAPI Evas_Coord                   elm_slider_span_size_get(const Evas_Object *obj);

/**
 * @brief Sets the format string for the unit label.
 *
 * @since_tizen 2.3
 *
 * @remarks Unit label is displayed all the time, if set, after the slider's bar.
 *          In the horizontal mode, on the right and in the vertical mode, at the bottom.
 *
 * @remarks If @c NULL, the unit label won't be visible. If not, it sets the format
 *          string for the label text. For the label text a floating point value is provided,
 *          so the label text can display up to @c 1 floating point value.
 *          Note that this is optional.
 *
 * @remarks Use a format string such as "%1.2f meters" for example, and it
 *          displays values like: "3.14 meters" for a value equal to 3.14159.
 *
 * @remarks By default, unit label is disabled.
 *
 * @param[in] obj The slider object
 * @param[in] format The format string for the unit display
 *
 * @see elm_slider_indicator_format_get()
 */
EAPI void                         elm_slider_unit_format_set(Evas_Object *obj, const char *format);

/**
 * @brief Gets the unit label format of the slider.
 *
 * @since_tizen 2.3
 *
 * @remarks Unit label is displayed all the time, if set, after the slider's bar.
 *          In the horizontal mode, on the right and in the vertical mode, at the bottom.
 *
 * @param[in] obj The slider object
 * @return The unit label format string in UTF-8
 *
 * @see elm_slider_unit_format_set() for more information on how this works.
 */
EAPI const char                  *elm_slider_unit_format_get(const Evas_Object *obj);

/**
 * @brief Sets the format string for the indicator label.
 *
 * @since_tizen 2.3
 *
 * @remarks The slider may display its value somewhere other than the unit label,
 *          for example, above the slider knob that is dragged around. This function
 *          sets the format string used for this.
 *
 * @remarks If @c NULL, the indicator label won't be visible. If not, it sets the format
 *          string for the label text. For the label text floating point value is provided,
 *          so the label text can display up to @c 1 floating point value.
 *          Note that this is optional.
 *
 * @remarks Use a format string such as "%1.2f meters" for example, and it
 *          displays values like: "3.14 meters" for a value equal to 3.14159.
 *
 * @remarks By default, the indicator label is disabled.
 *
 * @param[in] obj The slider object
 * @param[in] indicator The format string for the indicator display
 *
 * @see elm_slider_indicator_format_get()
 */
EAPI void                         elm_slider_indicator_format_set(Evas_Object *obj, const char *indicator);

/**
 * @brief Gets the indicator label format of the slider.
 *
 * @since_tizen 2.3
 *
 * @remarks The slider may display its value somewhere other than the unit label,
 *          for example, above the slider knob that is dragged around. This function
 *          gets the format string used for this.
 *
 * @param[in] obj The slider object
 * @return The indicator label format string in UTF-8
 *
 * @see elm_slider_indicator_format_set() for more information on how this works.
 */
EAPI const char                  *elm_slider_indicator_format_get(const Evas_Object *obj);

/**
 * @brief Sets the format function pointer for the indicator label.
 *
 * @details This sets the callback function to format the indicator string.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The slider object
 * @param[in] func The indicator format function
 * @param[in] free_func The freeing function for the format string
 *
 * @see elm_slider_indicator_format_set() for more info on how this works.
 */
EAPI void                         elm_slider_indicator_format_function_set(Evas_Object *obj, char *(*func)(double val), void (*free_func)(char *str));

/**
 * @brief Sets the format function pointer for the units label.
 *
 * @details This sets the callback function to format the units string.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The slider object
 * @param[in] func The units format function
 * @param[in] free_func The freeing function for the format string
 *
 * @see elm_slider_unit_format_set() for more info on how this works.
 */
EAPI void                         elm_slider_units_format_function_set(Evas_Object *obj, char *(*func)(double val), void (*free_func)(char *str));

/**
 * @brief Sets the orientation of a given slider widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this function to change how your slider is to be
 *          disposed: vertically or horizontally.
 *
 * @remarks By default, it's displayed horizontally.
 *
 * @param[in] obj The slider object
 * @param[in] horizontal If @c EINA_TRUE, it makes @a obj @b horizontal,
 *                   otherwise @c EINA_FALSE to make it @b vertical
 *
 * @see elm_slider_horizontal_get()
 */
EAPI void                         elm_slider_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Retrieves the orientation of a given slider widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The slider object
 * @return @c EINA_TRUE, if @a obj is set to @b horizontal,
 *         otherwise @c EINA_FALSE if it's @b vertical (and on errors)
 *
 * @see elm_slider_horizontal_set()
 */
EAPI Eina_Bool                    elm_slider_horizontal_get(const Evas_Object *obj);

/**
 * @brief Sets the minimum and maximum values for the slider.
 *
 * @since_tizen 2.3
 *
 * @remarks This defines the allowed range of values to be selected by the user.
 *
 * @remarks If the actual value is less than @a min, it is updated to @a min. If it
 *          is bigger then @a max, it is updated to @a max. Actual value can be
 *          obtained with elm_slider_value_get().
 *
 * @remarks By default, @a min is equal to @c 0.0, and @a max is equal to @c 1.0.
 *
 * @remarks Maximum must be greater than minimum, otherwise the behavior
 *          is undefined.
 *
 * @param[in] obj The slider object
 * @param[in] min The minimum value
 * @param[in] max The maximum value
 *
 * @see elm_slider_min_max_get()
 */
EAPI void                         elm_slider_min_max_set(Evas_Object *obj, double min, double max);

/**
 * @brief Gets the minimum and maximum values of the slider.
 *
 * @since_tizen 2.3
 *
 * @remarks If only one value is needed, the other pointer can be passed
 *          as @c NULL.
 *
 * @param[in] obj The slider object
 * @param[out] min The pointer to store the minimum value
 * @param[out] max The pointer to store the maximum value
 *
 * @see elm_slider_min_max_set()
 */
EAPI void                         elm_slider_min_max_get(const Evas_Object *obj, double *min, double *max);

/**
 * @brief Sets the value that the slider displays.
 *
 * @since_tizen 2.3
 *
 * @remarks The value is presented on the unit label following the format specified with
 *          elm_slider_unit_format_set() and on the indicator with
 *          elm_slider_indicator_format_set().
 *
 * @remarks The value must be between the min and max values. These values
 *          are set by elm_slider_min_max_set().
 *
 * @param[in] obj The slider object
 * @param[in] val The value to be displayed
 *
 * @see elm_slider_value_get()
 * @see elm_slider_unit_format_set()
 * @see elm_slider_indicator_format_set()
 * @see elm_slider_min_max_set()
 */
EAPI void                         elm_slider_value_set(Evas_Object *obj, double val);

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
EAPI double                       elm_slider_value_get(const Evas_Object *obj);

/**
 * @brief Inverts a given slider widget's displaying values order.
 *
 * @since_tizen 2.3
 *
 * @remarks A slider may be @b inverted, in which case it gets its
 *          values inverted, with high values being on the left or top and
 *          low values on the right or bottom, as opposed to normally have
 *          the low values on the former and high values on the latter,
 *          respectively, for the horizontal and vertical modes.
 *
 * @param[in] obj The slider object
 * @param[in] inverted If @c EINA_TRUE @a obj is inverted,
 *                 otherwise @c EINA_FALSE to bring it back to the default, non-inverted value
 *
 * @see elm_slider_inverted_get()
 */
EAPI void                         elm_slider_inverted_set(Evas_Object *obj, Eina_Bool inverted);

/**
 * @brief Gets whether a given slider widget's displaying values are
 *        inverted.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The slider object
 * @return @c EINA_TRUE, if @a obj has inverted values,
 *         otherwise @c EINA_FALSE (and on errors)
 *
 * @see elm_slider_inverted_set()
 */
EAPI Eina_Bool                    elm_slider_inverted_get(const Evas_Object *obj);

/**
 * @brief Sets whether to enlarge the slider indicator (augmented knob).
 *
 * @since_tizen 2.3
 *
 * @remarks By default, the indicator is bigger when dragged by the user.
 *
 * @remarks It won't display values set with
 *          elm_slider_indicator_format_set() if you disable the indicator.
 *
 * @param[in] obj The slider object
 * @param[in] show If @c EINA_TRUE the knob is enlarged, otherwise @c EINA_FALSE always lets the
 *             knob of the default size
 */
EAPI void                         elm_slider_indicator_show_set(Evas_Object *obj, Eina_Bool show);

/**
 * @brief Gets whether a given slider widget is an enlarging indicator.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The slider object
 * @return @c EINA_TRUE, if @a obj is an enlarging indicator,
 *         otherwise @c EINA_FALSE (and on errors)
 *
 * @see elm_slider_indicator_show_set()
 */
EAPI Eina_Bool                    elm_slider_indicator_show_get(const Evas_Object *obj);

/**
 * @brief Sets the step by which the slider indicator moves.
 *
 * @since_tizen 2.3
 *
 * @remarks This value is used when the draggable object is moved automatically i.e., in case
 *          of a key event when up/down/left/right key is pressed or in case accessibility is set
 *          and the flick event is used to inc/dec slider values.
 *          By default, the step value is equal to @c 0.05.
 *
 * @param[in] obj The slider object
 * @param[in] step The step value
 *
 * @see elm_slider_step_get()
 */
EAPI void                         elm_slider_step_set(Evas_Object *obj, double step);

/**
 * @brief Gets the step by which the slider indicator moves.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The slider object
 * @return The step value
 *
 * @see elm_slider_step_set()
 */
EAPI double                       elm_slider_step_get(const Evas_Object *obj);

/**
 * @}
 */
