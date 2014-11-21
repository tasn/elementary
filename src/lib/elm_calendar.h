/**
 * @internal
 * @defgroup Calendar Calendar
 * @ingroup Elementary
 *
 * @image html calendar_inheritance_tree.png
 * @image latex calendar_inheritance_tree.eps
 *
 * This is a calendar widget. It helps applications to flexibly
 * display a calender with days of the week, date, year, and
 * month. Applications are able to set specific dates to be reported
 * back, when selected, in the smart callbacks of the calendar
 * widget. The API of this widget lets the applications perform other
 * functions, like:
 *
 * - placing marks on specific dates.
 * - setting the bounds for the calendar (minimum and maximum years).
 * - setting the day names of the week (e.g. "Thu" or "Thursday").
 * - setting the year and month format.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for calendar objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - Emitted when the date in the calendar is changed.
 * - @c "display,changed" - Emitted when the current month displayed in the
 * calendar is changed.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_signal_emit
 * @li @ref elm_object_signal_callback_add
 * @li @ref elm_object_signal_callback_del
 *
 * @{
 */

/**
 * Enumeration of Elm Calendar Mark Repeat Type
 */
typedef enum
{
   ELM_CALENDAR_UNIQUE, /**< Default value. Marks are displayed only on the event day */
   ELM_CALENDAR_DAILY, /**< Marks are displayed every day after the event day (inclusive) */
   ELM_CALENDAR_WEEKLY, /**< Marks are displayed every week after the event day (inclusive) - i.e. every seven days */
   ELM_CALENDAR_MONTHLY, /**< Marks are displayed every month day that coincides with the event day. E.g.: if an event is set to 30th Jan, no marks are displayed on Feb, but are displayed on 30th Mar */
   ELM_CALENDAR_ANNUALLY, /**< Marks are displayed every year that coincides with the event day (and month). E.g. an event added to 30th Jan 2012 is repeated on 30th Jan 2013 */
   ELM_CALENDAR_LAST_DAY_OF_MONTH /**< Marks are displayed every last day of the month after the event day (inclusive)  @since 1.7 */
} _Elm_Calendar_Mark_Repeat_Type;

/**
 * @enum _Elm_Calendar_Mark_Repeat_Type
 * @typedef Elm_Calendar_Mark_Repeat_Type
 *
 * @brief Enumeration that shows an event's periodicity, used to define whether a mark should be repeated
 *        @a beyond the event's day. It's set when a mark is added.
 *
 * @remarks So, for a mark added to 13th May with periodicity set to WEEKLY,
 *          there are marks every week after this date. Marks are displayed
 *          at 13th, 20th, 27th, 3rd June ...
 *
 * @remarks Values don't work as bitmask, only one can be chosen.
 *
 * @see elm_calendar_mark_add()
 */
typedef _Elm_Calendar_Mark_Repeat_Type Elm_Calendar_Mark_Repeat_Type;

typedef enum
{
   ELM_DAY_SUNDAY,
   ELM_DAY_MONDAY,
   ELM_DAY_TUESDAY,
   ELM_DAY_WEDNESDAY,
   ELM_DAY_THURSDAY,
   ELM_DAY_FRIDAY,
   ELM_DAY_SATURDAY,
   ELM_DAY_LAST
} _Elm_Calendar_Weekday;

/**
 * @enum _Elm_Calendar_Weekday
 * @typedef Elm_Calendar_Weekday
 *
 * @brief Enumeration that defines the select mode for a weekday calendar.
 *
 * @see elm_calendar_first_day_of_week_set()
 */
typedef _Elm_Calendar_Weekday Elm_Calendar_Weekday;

/**
 * @brief Enumeration of Elm Calendar Select Mode
 */
typedef enum
{
   ELM_CALENDAR_SELECT_MODE_DEFAULT = 0, /**< Default value. A day is always selected */
   ELM_CALENDAR_SELECT_MODE_ALWAYS, /**< A day is always selected */
   ELM_CALENDAR_SELECT_MODE_NONE, /**< None of the days can be selected */
   ELM_CALENDAR_SELECT_MODE_ONDEMAND /**< User may have selected a day or maybe not */
} _Elm_Calendar_Select_Mode;

/**
 * @enum _Elm_Calendar_Select_Mode
 * @typedef Elm_Calendar_Select_Mode
 *
 * @brief Enumeration that defines the mode, which determine how a user could select a day.
 *
 * @see elm_calendar_select_mode_set()
 */
typedef _Elm_Calendar_Select_Mode Elm_Calendar_Select_Mode;

typedef enum
{
   ELM_CALENDAR_SELECTABLE_NONE = 0,
   ELM_CALENDAR_SELECTABLE_YEAR = (1 << 0),
   ELM_CALENDAR_SELECTABLE_MONTH = (1 << 1),
   ELM_CALENDAR_SELECTABLE_DAY = (1 << 2)
} _Elm_Calendar_Selectable;

/**
 * @enum _Elm_Calendar_Selectable
 * @typedef Elm_Calendar_Selectable
 *
 * @brief The structure type that is a bitmask used to define which fields of a @b tm struct are taken into
 *        account, when elm_calendar_selected_time_set() is invoked.
 *
 * @since 1.8
 *
 * @see elm_calendar_selectable_set()
 * @see elm_calendar_selected_time_set()
 */
typedef _Elm_Calendar_Selectable Elm_Calendar_Selectable;

typedef struct _Elm_Calendar_Mark Elm_Calendar_Mark;    /**< Item handle for a calendar mark. Created with elm_calendar_mark_add() and deleted with elm_calendar_mark_del() */

/**
 * @typedef Elm_Calendar_Format_Cb
 *
 * @brief Called to format the string that is used
 *        to display the month and year.
 *
 * @param[in] stime The struct representing the time
 * @return The string representing the time that is set to the calendar's text
 *
 * @see elm_calendar_format_function_set()
 */
typedef char * (*Elm_Calendar_Format_Cb)(struct tm *stime);

/**
 * @brief Adds a new calendar widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new calendar widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return The new calendar widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object         *elm_calendar_add(Evas_Object *parent);

/**
 * @brief Gets weekdays names displayed by the calendar.
 *
 * @remarks By default, weekdays abbreviations obtained from the system are displayed:
 *          E.g. for an en_US locale: "Sun, Mon, Tue, Wed, Thu, Fri, Sat"
 *          The first string is related to Sunday, the second to Monday, and so on.
 *
 * @param[in] obj The calendar object
 * @return The array of seven strings to be used as weekday names
 *
 * @see elm_calendar_weekdays_name_set()
 */
EAPI const char         **elm_calendar_weekdays_names_get(const Evas_Object *obj);

/**
 * @brief Sets weekdays names to be displayed by the calendar.
 *
 * @remarks It must have @c 7 elements, or it accesses invalid memory.
 * @remarks The strings must be @c NULL terminated ('@\0').
 *
 * @remarks By default, weekdays abbreviations obtained from the system are displayed:
 *          E.g. for an en_US locale: "Sun, Mon, Tue, Wed, Thu, Fri, Sat"
 *          The first string should be related to Sunday, the second to Monday, and so on.
 *
 * The usage should be like this:
 * @code
 *   const char *weekdays[] =
 *   {
 *      "Sunday", "Monday", "Tuesday", "Wednesday",
 *      "Thursday", "Friday", "Saturday"
 *   };
 *   elm_calendar_weekdays_names_set(calendar, weekdays);
 * @endcode
 *
 * @param[in] obj The calendar object
 * @param[in] weekdays The array of seven strings to be used as weekday names
 *
 * @see elm_calendar_weekdays_name_get()
 */
EAPI void                 elm_calendar_weekdays_names_set(Evas_Object *obj, const char *weekdays[]);

/**
 * @brief Sets the minimum and maximum values for the year.
 *
 * @remarks Maximum must be greater than minimum, except if you don't want to set the
 *          maximum year. Default values are @c 1902 and @c -1.
 *
 * @remarks If the maximum year is a negative value, it is limited depending
 *          on the platform architecture (year @c 2037 for @c 32 bits).
 *
 * @param[in] obj The calendar object
 * @param[in] min The minimum year, greater than @c 1901
 * @param[in] max The maximum year
 *
 * @see elm_calendar_min_max_year_get()
 */
EAPI void                 elm_calendar_min_max_year_set(Evas_Object *obj, int min, int max);

/**
 * @brief Gets the minimum and maximum values for the year.
 *
 * @remarks Default values are @c 1902 and @c -1.
 *
 * @param[in] obj The calendar object
 * @param[out] min The minimum year
 * @param[out] max The maximum year
 *
 * @see elm_calendar_min_max_year_get()
 */
EAPI void                 elm_calendar_min_max_year_get(const Evas_Object *obj, int *min, int *max);

/**
 * @brief Sets the select day mode to use.
 *
 * @details This sets the day selection mode that is used.
 *
 * @param[in] obj The calendar object
 * @param[in] mode The select mode to use
 */
EAPI void                 elm_calendar_select_mode_set(Evas_Object *obj, Elm_Calendar_Select_Mode mode);

/**
 * @brief Gets the select day mode used.
 *
 * @details This sets the day selection mode that is used.
 *
 * @param[in] obj The calendar object
 *
 * @return The selected mode
 *
 * @see elm_calendar_select_mode_set()
 */
EAPI Elm_Calendar_Select_Mode elm_calendar_select_mode_get(const Evas_Object *obj);

/**
 * @brief Sets the selected date to be highlighted on the calendar.
 *
 * @details This sets the selected date, changing the displayed month if needed.
 *          Selected date changes when the user goes to the next/previous month or
 *          selects a day by pressing it on the calendar.
 *
 * @param[in] obj The calendar object
 * @param[in] selected_time A @b tm struct to represent the selected date
 *
 * @see elm_calendar_selected_time_get()
 */
EAPI void                 elm_calendar_selected_time_set(Evas_Object *obj, struct tm *selected_time);

/**
 * @brief Get the selected date.
 *
 * @details This gets the date selected by the user or set by the function
 *          elm_calendar_selected_time_set().
 *          Selected date changes when the user goes to the next/previous month or
 *          selects a day by pressing it on the calendar.
 *
 * @param[in] obj The calendar object
 * @param[out] selected_time A @b tm struct to point to the selected date
 * @return @c EINA_FALSE means an error occurred and the returned time shouldn't
 *         be considered, otherwise @c EINA_FALSE
 *
 * @see elm_calendar_selected_time_get()
 */
EAPI Eina_Bool            elm_calendar_selected_time_get(const Evas_Object *obj, struct tm *selected_time);

/**
 * @brief Sets a function to format the string that is used to display the
 *        month and year.
 *
 * @remarks By default it uses strftime with the "%B %Y" format string.
 *          It should allocate the memory that is used by the string,
 *          that is freed by the widget after usage.
 *          A pointer to the string and a pointer to the time struct is provided.
 *
 * Example:
 * @code
 * static char *
 * _format_month_year(struct tm *selected_time)
 * {
 *    char buf[32];
 *    if (!strftime(buf, sizeof(buf), "%B %Y", selected_time)) return NULL;
 *    return strdup(buf);
 * }
 *
 * elm_calendar_format_function_set(calendar, _format_month_year);
 * @endcode
 *
 * @param[in] obj The calendar object
 * @param[in] format_func The function to set the month-year string given that
 * the selected date is provided
 */
EAPI void                 elm_calendar_format_function_set(Evas_Object *obj, Elm_Calendar_Format_Cb format_func);

/**
 * @brief Adds a new mark to the calendar.
 *
 * @details This adds a mark that is drawn in the calendar with respect to the insertion
 *          time and periodicity. It emits the type as a signal to the widget theme.
 *          Default theme supports "holiday" and "checked", but it can be extended.
 *
 * @remarks It won't immediately update the calendar, drawing the marks.
 *          For this, call elm_calendar_marks_draw(). However, when the user selects the
 *          next or previous month, the calendar forces marks to be drawn.
 *
 * @remarks Marks created with this method can be deleted with
 *          elm_calendar_mark_del().
 *
 * Example
 * @code
 * struct tm selected_time;
 * time_t current_time;
 *
 * current_time = time(NULL) + 5 * 84600;
 * localtime_r(&current_time, &selected_time);
 * elm_calendar_mark_add(cal, "holiday", selected_time,
 *     ELM_CALENDAR_ANNUALLY);
 *
 * current_time = time(NULL) + 1 * 84600;
 * localtime_r(&current_time, &selected_time);
 * elm_calendar_mark_add(cal, "checked", selected_time, ELM_CALENDAR_UNIQUE);
 *
 * elm_calendar_marks_draw(cal);
 * @endcode
 *
 * @param[in] obj The calendar object
 * @param[in] mark_type A string used to define the type of mark \n
 *                  It is emitted to the theme, that should display a related modification on this
 *                  day's representation.
 * @param[in] mark_time A time struct to represent the date of inclusion of the mark \n
 *                  For marks that repeats it is just displayed after the inclusion
 *                  date in the calendar.
 * @param[in] repeat Repeats the event following this periodicity \n
 *               Can be a unique mark (that doesn't repeat), daily, weekly, monthly, or annually.
 * @return The created mark, otherwise @c NULL on failure
 *
 * @see elm_calendar_marks_draw()
 * @see elm_calendar_mark_del()
 */
EAPI Elm_Calendar_Mark   *elm_calendar_mark_add(Evas_Object *obj, const char *mark_type, struct tm *mark_time, Elm_Calendar_Mark_Repeat_Type repeat);

/**
 * @brief Deletes a mark from the calendar.
 *
 * @remarks If deleting all calendar marks is required, elm_calendar_marks_clear()
 *          should be used instead of getting the marks list and deleting on by one.
 *
 * @param[in] mark The mark to be deleted
 *
 * @see elm_calendar_mark_add()
 *
 * @ingroup Calendar
 */
EAPI void                 elm_calendar_mark_del(Elm_Calendar_Mark *mark);

/**
 * @brief Removes all calendar marks.
 *
 * @param[in] obj The calendar object
 *
 * @see elm_calendar_mark_add()
 * @see elm_calendar_mark_del()
 */
EAPI void                 elm_calendar_marks_clear(Evas_Object *obj);

/**
 * @brief Gets a list of all the calendar marks.
 *
 * @param[in] obj The calendar object
 * @return An @c Eina_List of calendar marks objects, otherwise @c NULL on failure
 *
 * @see elm_calendar_mark_add()
 * @see elm_calendar_mark_del()
 * @see elm_calendar_marks_clear()
 */
EAPI const Eina_List     *elm_calendar_marks_get(const Evas_Object *obj);

/**
 * @brief Draws calendar marks.
 *
 * @remarks Should be used after adding, removing, or clearing marks.
 *          It goes through the entire marks list, updating the calendar.
 *          If lots of marks are added, add all the marks and then call
 *          this function.
 *
 * @remarks When the month is changed, i.e. the user selects the next or previous month,
 *          marks are drawn.
 *
 * @param[in] obj The calendar object
 *
 * @see elm_calendar_mark_add()
 * @see elm_calendar_mark_del()
 * @see elm_calendar_marks_clear()
 */
EAPI void                 elm_calendar_marks_draw(Evas_Object *obj);

/**
 * @brief Sets the interval on time updates for an user's mouse button hold
 *        on the calendar widgets' month selection.
 *
 * @remarks This interval value is @b decreased while the user holds the
 *          mouse pointer either by selecting the next or previous month.
 *
 * @remarks This helps the user to get to a given month distant from the
 *          current one in an easier/faster way, as it starts to change quicker and
 *          quicker on mouse button holds.
 *
 * @remarks The calculation for the next change interval value, starting from
 *          the one set with this call, is the previous interval divided by
 *          @c 1.05, so it decreases a little bit.
 *
 * @remarks The default starting interval value for automatic changes is
 *          @b 0.85 seconds.
 *
 * @param[in] obj The calendar object
 * @param[in] interval The (first) interval value in seconds
 *
 * @see elm_calendar_interval_get()
 */
EAPI void                 elm_calendar_interval_set(Evas_Object *obj, double interval);

/**
 * @brief Gets the interval on time updates for a user's mouse button hold
 *        on the calendar widgets' month selection.
 *
 * @param[in] obj The calendar object
 * @return The (first) interval value, in seconds, set on it
 *
 * @see elm_calendar_interval_set()
 */
EAPI double               elm_calendar_interval_get(const Evas_Object *obj);

/**
 * @brief Sets the first day of the week to use on the calendar widgets'.
 *
 * @param[in] obj The calendar object
 * @param[in] day An integer which corresponds to the first day of the week (Sunday = 0, monday = 1,
 *            ..., saturday = 6)
 */
EAPI void                 elm_calendar_first_day_of_week_set(Evas_Object *obj, Elm_Calendar_Weekday day);

/**
 * @brief Gets the first day of the week that is used on calendar widgets'.
 *
 * @param[in] obj The calendar object
 * @return An integer which corresponds to the first day of the week (Sunday = 0, monday = 1,
 *         ..., saturday = 6)
 *
 * @see elm_calendar_first_day_of_week_set()
 */
EAPI Elm_Calendar_Weekday elm_calendar_first_day_of_week_get(const Evas_Object *obj);

/**
 * @brief Defines which fields of a @b tm struct are taken into account, when
 *        elm_calendar_selected_time_set() is invoked.
 *
 * @since 1.8
 *
 * @remarks By default the bitmask is set to use all fields of a @b tm struct (year,
 *          month, and day of the month).
 *
 * @param[in] obj The calendar object
 * @param[in] selectable A bitmask of Elm_Calendar_Selectable
 *
 * @see elm_calendar_selected_time_set
 */
EAPI void                 elm_calendar_selectable_set(Evas_Object *obj, Elm_Calendar_Selectable selectable);


/**
 * @brief Gets how elm_calendar_selected_time_set() manages a date.
 *
 * @since 1.8
 *
 * @param[in] obj The calendar object
 * @return The flag used to manage a date with elm_calendar_selected_time_set()
 *
 * @see elm_calendar_selectable_set()
 * @see elm_calendar_selected_time_set()
 */
EAPI Elm_Calendar_Selectable elm_calendar_selectable_get(const Evas_Object *obj);

/**
 * @brief Gets the current time displayed in the widget.
 *
 * @since 1.8
 *
 * @param[in] obj The calendar object
 * @param[out] displayed_time A @b tm struct to point to the displayed date
 * @return @c EINA_FALSE means an error has occurred, otherwise @c EINA_FALSE if the returned
 *         time is zero filled
 */
EAPI Eina_Bool                    elm_calendar_displayed_time_get(const Evas_Object *obj, struct tm *displayed_time);

/**
 * @}
 */
