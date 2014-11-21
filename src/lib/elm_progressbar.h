/**
 * @defgroup Progressbar Progress bar
 * @ingroup elm_widget_group
 *
 * @image html progressbar_inheritance_tree.png
 * @image latex progressbar_inheritance_tree.eps
 *
 * @brief The progress bar is a widget for visually representing the progress
 *        status of a given job/task.
 *
 * A progress bar may be horizontal or vertical. It may display an
 * icon besides it, as well as primary and @b units labels. The
 * former is meant to label the widget as a whole, while the
 * latter, which is formatted with floating point values (and thus
 * accepts a <c>printf</c>-style format string, like <c>"%1.2f
 * units"</c>), is meant to label the widget's <b>progress
 * value</b>. Label, icon, and unit strings/objects are @b optional
 * for progress bars.
 *
 * A progress bar may be @b inverted, in which case it gets its
 * values inverted, i.e., high values being on the left or top and
 * low values on the right or bottom, for horizontal and vertical modes
 * respectively.
 *
 * The @b span of the progress, as set by
 * elm_progressbar_span_size_set(), is its length (horizontally or
 * vertically), unless one puts size hints on the widget to expand
 * in desired directions, by any container. That length is
 * scaled by the object or application's scaling factor.
 * Applications can query the progress bar for its value with
 * elm_progressbar_value_get().
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for progress bar objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li @c "changed" - When the value is changed.
 * @since 1.7
 *
 * This widget has the following styles:
 * - @c "default"
 * - @c "wheel" (simple style, no text, no progression, only
 *      "pulse" effect is available)
 *
 * The default text parts of the progressbar widget that you can use are:
 * @li "default" - Label of the progressbar.
 *
 * The default content parts of the progressbar widget that you can use are:
 * @li "icon" - Icon of the progressbar.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * @{
 */

/**
 * @brief Adds a new progress bar widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new progress bar widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new progress bar widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object                 *elm_progressbar_add(Evas_Object *parent);

/**
 * @brief Sets whether a given progress bar widget is at the "pulsing mode".
 *
 * @since_tizen 2.3
 *
 * @remarks By default, progress bars display values from low to
 *          high value boundaries. There are, though, contexts in which the
 *          progress of a given task is @b unknown.  For such cases,
 *          one can set a progress bar widget to a "pulsing state", to give
 *          the user an idea that some computation is being held, but
 *          without exact progress values. In the default theme, it
 *          animates its bar with the contents filling in constantly and back
 *          to non-filled, in a loop. To start and stop this pulsing
 *          animation, one has to explicitly call elm_progressbar_pulse().
 *
 * @param[in] obj The progress bar object
 * @param[in] pulse If @c EINA_TRUE @a obj is put in the pulsing mode,
 *              otherwise @c EINA_FALSE to put it back to its default one
 *
 * @see elm_progressbar_pulse_get()
 * @see elm_progressbar_pulse()
 */
EAPI void                         elm_progressbar_pulse_set(Evas_Object *obj, Eina_Bool pulse);

/**
 * @brief Gets whether a given progress bar widget is at the "pulsing mode".
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The progress bar object
 * @return @c EINA_TRUE if @a obj is in the pulsing mode, otherwise @c EINA_FALSE
 *         if it's in the default one (and on errors)
 */
EAPI Eina_Bool                    elm_progressbar_pulse_get(const Evas_Object *obj);

/**
 * @brief Starts or stops a given progress bar "pulsing" animation, if its
 *        under that mode.
 *
 * @since_tizen 2.3
 *
 * @remarks This call won't do anything if @a obj is not under the "pulsing mode".
 *
 * @param[in] obj The progress bar object
 * @param[in] state If @c EINA_TRUE, @b start the pulsing animation,
 *              otherwise @c EINA_FALSE to @b stop it
 *
 * @see elm_progressbar_pulse_set()
 */
EAPI void                         elm_progressbar_pulse(Evas_Object *obj, Eina_Bool state);

/**
 * @brief Sets the progress value (in percentage) on a given progress bar
 *        widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this call to set the progress bar levels.
 *
 * @remarks If you pass a value out of the specified range for @a
 *          val, it is interpreted as the @b closest of the @b boundary
 *          values in the range.
 *
 * @param[in] obj The progress bar object
 * @param[in] val The progress value (@b must be between @c 0.0 and @c
 *            1.0)
 */
EAPI void                         elm_progressbar_value_set(Evas_Object *obj, double val);

/**
 * @brief Gets the progress value (in percentage) on a given progress bar
 *        widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The progress bar object
 * @return The value of the progressbar
 *
 * @see elm_progressbar_value_set()
 */
EAPI double                       elm_progressbar_value_get(const Evas_Object *obj);

/**
 * @brief Sets the (exact) length of the bar region of a given progress bar
 *        widget.
 *
 * @details This sets the minimum width (when in the horizontal mode) or height
 *          (when in the vertical mode) of the actual bar area of the progress
 *          bar @a obj. This in turn affects the object's minimum size. Use
 *          this when you're not setting other size hints expanding in the
 *          given direction (like weight and alignment hints) and you would
 *          like it to have a specific size.
 *
 * @since_tizen 2.3
 *
 * @remarks Icon, label, and unit text around @a obj require their
 *          own space, which makes @a obj to require more @a size,
 *          actually.
 *
 * @param[in] obj The progress bar object
 * @param[in] size The length of the progress bar's bar region
 *
 * @see elm_progressbar_span_size_get()
 */
EAPI void                         elm_progressbar_span_size_set(Evas_Object *obj, Evas_Coord size);

/**
 * @brief Gets the length set for the bar region of a given progress bar
 *        widget.
 *
 * @since_tizen 2.3
 *
 * @remarks If that size is not set previously, with
 *          elm_progressbar_span_size_set(), this call returns @c 0.
 *
 * @param[in] obj The progress bar object
 * @return The length of the progress bar's bar region
 */
EAPI Evas_Coord                   elm_progressbar_span_size_get(const Evas_Object *obj);

/**
 * @brief Sets the format string for a given progress bar widget's units
 *        label.
 *
 * @since_tizen 2.3
 *
 * @remarks If @c NULL is passed on @a format, it makes @a obj units
 *          area to be hidden completely. If not, it sets the <b>format
 *          string</b> for the units label's @b text. The units label is
 *          provided with a floating point value, so the units text displays
 *          at most one floating point value. Note that the units label is
 *          optional. Use a format string such as "%1.2f meters" for
 *          example.
 *
 * @remarks The default format string for a progress bar is an integer
 *          percentage, as in @c "%.0f %%".
 *
 * @param[in] obj The progress bar object
 * @param[in] format The format string for the @a obj units label
 *
 * @see elm_progressbar_unit_format_get()
 */
EAPI void                         elm_progressbar_unit_format_set(Evas_Object *obj, const char *format);

/**
 * @brief Retrieves the format string set for a given progress bar widget's
 *        units label.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The progress bar object
 * @return The format set string for @a obj units label,
 *         otherwise @c NULL if none are set (and on errors)
 *
 * @see elm_progressbar_unit_format_set()
 */
EAPI const char                  *elm_progressbar_unit_format_get(const Evas_Object *obj);

/**
 * @brief Sets the format function pointer for the units label.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks This sets the callback function to format the unit string.
 *
 * @param[in] obj The progress bar object
 * @param[in] free_func The freeing function for the format string
 *
 * @see elm_progressbar_unit_format_set() for more info on how this works.
 */
EAPI void                         elm_progressbar_unit_format_function_set(Evas_Object *obj, char *(func)(double), void (*free_func) (char *));

/**
 * @brief Sets the orientation of a given progress bar widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this function to change how your progress bar is to be
 *          disposed: vertically or horizontally.
 *
 * @param[in] obj The progress bar object
 * @param[in] horizontal If @c EINA_TRUE @a obj is @b horizontal,
 *                   otherwise @c EINA_FALSE if @a obj is @b vertical
 *
 * @see elm_progressbar_horizontal_get()
 */
EAPI void                         elm_progressbar_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Retrieves the orientation of a given progress bar widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The progress bar object
 * @return @c EINA_TRUE if @a obj is set to be @b horizontal,
 *         otherwise @c EINA_FALSE if it's @b vertical (and on errors)
 *
 * @see elm_progressbar_horizontal_set()
 */
EAPI Eina_Bool                    elm_progressbar_horizontal_get(const Evas_Object *obj);

/**
 * @brief Inverts a given progress bar widget's displaying values order.
 *
 * @since_tizen 2.3
 *
 * @remarks A progress bar may be @b inverted, in which case it gets its
 *          values inverted, with high values being on the left or top and
 *          low values on the right or bottom, as opposed to normally having
 *          low values on the former and high values on the latter,
 *          for horizontal and vertical modes respectively.
 *
 * @param[in] obj The progress bar object
 * @param[in] inverted If @c EINA_TRUE @a obj is inverted,
 *                 otherwise @c EINA_FALSE brings it back to the default, non-inverted values
 *
 * @see elm_progressbar_inverted_get()
 */
EAPI void                         elm_progressbar_inverted_set(Evas_Object *obj, Eina_Bool inverted);

/**
 * @brief Gets whether a given progress bar widget's displaying values are
 *        inverted.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The progress bar object
 * @return @c EINA_TRUE if @a obj has inverted values,
 *         otherwise @c EINA_FALSE (and on errors)
 *
 * @see elm_progressbar_inverted_set()
 */
EAPI Eina_Bool                    elm_progressbar_inverted_get(const Evas_Object *obj);

/**
 * @}
 */
