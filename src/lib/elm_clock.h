/**
 * @internal
 * @defgroup Clock Clock
 * @ingroup Elementary
 *
 * @image html clock_inheritance_tree.png
 * @image latex clock_inheritance_tree.eps
 *
 * @image html img/widget/clock/preview-00.png
 * @image latex img/widget/clock/preview-00.eps
 *
 * This is a @b digital clock widget. In its default theme, it has a
 * vintage "flipping numbers clock" appearance, which animates
 * sheets of individual algarisms individually as time goes by.
 *
 * A newly created clock fetches the system's time (already
 * considering local time adjustments) to start with, and tick
 * accordingly. It may or may not show seconds.
 *
 * Clocks have an @b edition  mode. When in it, the sheets
 * display extra arrow indications on the top and bottom and the
 * user may click on them to raise or lower the time values. After
 * it's told to exit the edition mode, it keeps ticking with that
 * new time set (it keeps the difference from the local time).
 *
 * Also, when under the edition mode, the user clicks on the cited arrows
 * which are @b held for some time making the clock flip the
 * sheet, thus editing the time, continuously and automatically for
 * the user. The interval between sheet flips keeps growing with
 * time, so that it helps the user to reach a time which is distant
 * from the one set.
 *
 * The time display is, by default, in the military mode (24h), but an
 * am/pm indicator may be optionally shown, too, when it
 * switches to 12h.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for clock objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - The clock's user changed the time.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_signal_emit
 * @li @ref elm_object_signal_callback_add
 * @li @ref elm_object_signal_callback_del
 *
 * @{
 */

/**
 * @brief Enumeration of the identifiers for which clock digits should be editable, when a
 *        clock widget is in the edition mode. Values may be OR-ed together to
 *        make a mask, naturally.
 *
 * @see elm_clock_edit_set()
 * @see elm_clock_edit_mode_set()
 */
typedef enum
{
   ELM_CLOCK_EDIT_DEFAULT = 0, /**< Default value. Means that all digits are editable, when in the edition mode */
   ELM_CLOCK_EDIT_HOUR_DECIMAL = 1 << 0, /**< Decimal algarism of hours value should be editable */
   ELM_CLOCK_EDIT_HOUR_UNIT = 1 << 1, /**< Unit algarism of hours value should be editable */
   ELM_CLOCK_EDIT_MIN_DECIMAL = 1 << 2, /**< Decimal algarism of minutes value should be editable */
   ELM_CLOCK_EDIT_MIN_UNIT = 1 << 3, /**< Unit algarism of minutes value should be editable */
   ELM_CLOCK_EDIT_SEC_DECIMAL = 1 << 4, /**< Decimal algarism of seconds value should be editable */
   ELM_CLOCK_EDIT_SEC_UNIT = 1 << 5, /**< Unit algarism of seconds value should be editable */
   ELM_CLOCK_EDIT_ALL = (1 << 6) - 1 /**< All digits should be editable */
} Elm_Clock_Edit_Mode;

/**
 * @brief Adds a new clock widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new clock widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return A new clock widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object      *elm_clock_add(Evas_Object *parent);

/**
 * @brief Sets a clock widget's time, programmatically.
 *
 * @details This function updates the time that is shown by the clock
 *          widget.
 *
 * @remarks Values @b must be set within the following ranges:
 *          - 0 - 23, for hours
 *          - 0 - 59, for minutes
 *          - 0 - 59, for seconds,
 *
 *          even if the clock is not in the "military" mode.
 *
 * @remarks The behavior for the values set out of those ranges is @b
 *          undefined.
 *
 * @param[in] obj The clock widget object
 * @param[in] hrs The hours to set
 * @param[in] min The minutes to set
 * @param[in] sec The seconds to set
 */
EAPI void              elm_clock_time_set(Evas_Object *obj, int hrs, int min, int sec);

/**
 * @brief Gets a clock widget's time values.
 *
 * @details This function gets the time set for @a obj, returning
 *          it on the variables passed as the arguments to the function.
 *
 * @remarks Use @c NULL pointers on the time values you're not
 *          interested in, they are ignored by the function.
 *
 * @param[in] obj The clock object
 * @param[out] hrs A pointer to the variable to get the hours value
 * @param[out] min A pointer to the variable to get the minutes value
 * @param[out] sec A pointer to the variable to get the seconds value
 */
EAPI void              elm_clock_time_get(const Evas_Object *obj, int *hrs, int *min, int *sec);

/**
 * @brief Sets whether a given clock widget is under the <b>edition mode</b> or
 *        under the (default) displaying-only mode.
 *
 * @details This function allows a clock's time to be editable or not <b>by
 *          user interaction</b>. When in the edition mode, clocks @b stop
 *          ticking, until one brings them back to the canonical mode. The
 *          elm_clock_edit_mode_set() function influences which digits
 *          of the clock are editable.
 *
 * @remarks am/pm sheets, if being shown, are @b always editable
 *          under the edition mode.
 *
 * @param[in] obj The clock object
 * @param[in] edit If @c EINA_TRUE it is put in the edition mode,
 *             otherwise @c EINA_FALSE to put it back to the "displaying only" mode
 *
 * @see elm_clock_edit_get()
 */
EAPI void              elm_clock_edit_set(Evas_Object *obj, Eina_Bool edit);

/**
 * @brief Retrieves whether a given clock widget is under the editing mode
 *        or under the (default) displaying-only mode.
 *
 * @details This function retrieves whether the clock's time can be edited
 *          or not by user interaction.
 *
 * @param[in] obj The clock object
 * @return @c EINA_TRUE if it's in the edition mode, otherwise @c EINA_FALSE
 *
 * @see elm_clock_edit_set()
 */
EAPI Eina_Bool         elm_clock_edit_get(const Evas_Object *obj);

/**
 * @brief Sets what digits of the given clock widget should be editable
 *        when in the edition mode.
 *
 * @param[in] obj The clock object
 * @param[in] digedit The bit mask indicating the digits to be editable
 *                (values in #Elm_Clock_Edit_Mode)
 *
 * @see elm_clock_edit_mode_get()
 */
EAPI void              elm_clock_edit_mode_set(Evas_Object *obj, Elm_Clock_Edit_Mode digedit);

/**
 * @brief Retrieves what digits of the given clock widget should be
 *        editable when in the edition mode.
 *
 * @param[in] obj The clock object
 * @return The bit mask indicating the digits to be editable
 *         (values in #Elm_Clock_Edit_Mode)
 *
 * @see elm_clock_edit_mode_set()
 */
EAPI Elm_Clock_Edit_Mode elm_clock_edit_mode_get(const Evas_Object *obj);

/**
 * @brief Sets whether the given clock widget must show hours in the military or the
 *        am/pm mode.
 *
 * @details This function sets if the clock must show hours in the military or
 *          am/pm mode. In some countries like Brazil, the military mode
 *          (00-24h-format) is used, in opposition to USA, where the
 *          am/pm mode is more commonly used.
 *
 * @param[in] obj The clock object
 * @param[in] am_pm If @c EINA_TRUE it is put in the am/pm mode, otherwise @c EINA_FALSE
 *              to put it in the military mode
 *
 * @see elm_clock_show_am_pm_get()
 */
EAPI void              elm_clock_show_am_pm_set(Evas_Object *obj, Eina_Bool am_pm);

/**
 * @brief Gets whether the given clock widget shows hours in the military or am/pm
 *        mode.
 *
 * @details This function gets if the clock shows hours in the military or am/pm
 *          mode.
 *
 * @param[in] obj The clock object
 * @return @c EINA_TRUE, if in the am/pm mode, otherwise @c EINA_FALSE if in
 *         the military mode
 *
 * @see elm_clock_show_am_pm_set()
 */
EAPI Eina_Bool         elm_clock_show_am_pm_get(const Evas_Object *obj);

/**
 * @brief Sets whether the given clock widget must show time in seconds.
 *
 * @details This function sets if the given clock must show the elapsed
 *          seconds or not. By default, they are @b not shown.
 *
 * @param[in] obj The clock object
 * @param[in] seconds If @c EINA_TRUE it shows seconds, otherwise @c EINA_FALSE
 *
 * @see elm_clock_show_seconds_get()
 */
EAPI void              elm_clock_show_seconds_set(Evas_Object *obj, Eina_Bool seconds);

/**
 * @brief Gets whether the given clock widget is showing time in seconds.
 *
 * @details This function gets whether @a obj is showing the elapsed
 *          seconds or not.
 *
 * @param[in] obj The clock object
 * @return @c EINA_TRUE if it's showing seconds, otherwise @c EINA_FALSE
 *
 * @see elm_clock_show_seconds_set()
 */
EAPI Eina_Bool         elm_clock_show_seconds_get(const Evas_Object *obj);

/**
 * @brief Sets the first interval on time updates for a user's mouse button hold
 *        on the clock widgets' time edition.
 *
 * @remarks This interval value is @b decreased while the user holds the
 *          mouse pointer either by incrementing or decrementing a given the
 *          clock digit's value.
 *
 * @remarks This helps the user to get to a given time, which is distant from the
 *          current one, in an easier/faster way, as it starts to flip quicker and
 *          quicker on mouse button holds.
 *
 * @remarks The calculation for the next flip interval value, starting from
 *          the one set with this call, is the previous interval divided by
 *          @c 1.05, so it decreases a little bit.
 *
 * @remarks The default starting interval value for automatic flips is
 *          @b 0.85 seconds.
 *
 * @param[in] obj The clock object
 * @param[in] interval The first interval value in seconds
 *
 * @see elm_clock_first_interval_get()
 */
EAPI void              elm_clock_first_interval_set(Evas_Object *obj, double interval);

/**
 * @brief Gets the first interval on time updates for a user's mouse button hold
 *        on the clock widgets' time edition.
 *
 * @param[in] obj The clock object
 * @return The first interval value, in seconds, set on it
 *
 * @see elm_clock_first_interval_set()
 */
EAPI double            elm_clock_first_interval_get(const Evas_Object *obj);

/**
 * @}
 */
