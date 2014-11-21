/**
 * @defgroup Datetime Datetime
 * @ingroup elm_widget_group
 *
 * @image html datetime_inheritance_tree.png
 * @image latex datetime_inheritance_tree.eps
 *
 * @brief The Datetime widget is used to display the input date & time values.
 *
 * This widget displays date and time as per the <b>system's locale</b> settings (Date
 * includes Day, Month & Year along with the defined separators and
 * Time includes Hour, Minute & AM/PM fields. Separator for AM/PM field is ignored.
 *
 * The corresponding Month, AM/PM strings are displayed according to the
 * system’s language settings.
 *
 * The Datetime format is a combination of LIBC standard characters like
 * “%%d %%b %%Y %%I : %%M  %%p” which, as a whole represents both Date as well as Time
 * format.
 *
 * Elm_datetime supports only the following sub set of LIBC date format specifiers:
 *
 * @b %%Y : The year as a decimal number including the century (example: 2011).
 *
 * @b %%y : The year as a decimal number without a century (range 00 to 99).
 *
 * @b %%m : The month as a decimal number (range 01 to 12).
 *
 * @b %%b : The abbreviated month name according to the current locale.
 *
 * @b %%B : The full month name according to the current locale.
 *
 * @b %%h : The abbreviated month name according to the current locale(same as %%b).
 *
 * @b %%d : The day of the month as a decimal number (range 01 to 31).
 *
 * @b %%e : The day of the month as a decimal number (range 1 to 31). Single
 * digits are preceded by a blank.
 *
 * @b %%I : The hour as a decimal number using a 12-hour clock (range 01 to 12).
 *
 * @b %%H : The hour as a decimal number using a 24-hour clock (range 00 to 23).
 *
 * @b %%k : The hour (24-hour clock) as a decimal number (range 0 to 23). Single
 * digits are preceded by a blank.
 *
 * @b %%l : The hour (12-hour clock) as a decimal number (range 1 to 12). Single
 * digits are preceded by a blank.
 *
 * @b %%M : The minute as a decimal number (range 00 to 59).
 *
 * @b %%p : Either 'AM' or 'PM' according to the given time value, or the
 * corresponding strings for the current locale. Noon is treated as 'PM'
 * and midnight as 'AM'
 *
 * @b %%P : Like %p, but in lower case: 'am' or 'pm' or a corresponding string for
 * the current locale.
 *
 * @b %%c : The preferred date and time representation for the current locale.
 *
 * @b %%x : The preferred date representation for the current locale without the time.
 *
 * @b %%X : The preferred time representation for the current locale without the date.
 *
 * @b %%r : The complete calendar time using the AM/PM format of the current locale.
 *
 * @b %%R : The hour and minute in decimal numbers using the format %H:%M.
 *
 * @b %%T : The time of the day in decimal numbers using the format %H:%M:%S.
 *
 * @b %%D : The date using the format %%m/%%d/%%y.
 *
 * @b %%F : The date using the format %%Y-%%m-%%d.
 *
 * (For more reference on the available <b>LIBC date format specifiers</b>, please
 * visit the link:
 * http://www.gnu.org/s/hello/manual/libc.html#Formatting-Calendar-Time )
 *
 * The Datetime widget can provide Unicode @b separators in between its fields
 * except for AM/PM field.
 * A separator can be any <b>Unicode character</b> other than the LIBC standard
 * date format specifiers.( Example: In the format %%b %%d , %%y %%H : %%M
 * comma(,) is a separator for the date field %%d and colon(:) is a separator for
 * the hour field %%H ).
 *
 * The default format is a predefined one, based on the system Locale.
 *
 * The Hour format 12hr(1-12) or 24hr(0-23) display can be selected by setting
 * the corresponding user format.
 *
 * Datetime supports six fields: Year, Month, Date, Hour, Minute, AM/PM.
 * Depending on the Datetime module that is loaded, the user can see
 * different UIs to select the individual field values.
 *
 * The individual fields of Datetime can be arranged in any order according to the format
 * set by the application.
 *
 * There is a provision to set the visibility of a particular field as TRUE/ FALSE
 * so that <b>only time/ only date / only required fields</b> are displayed.
 *
 * Each field has default minimum and maximum values just like the daily
 * calendar information. These min/max values can be modified as per the application usage.
 *
 * A user can enter the values only in between the range of the maximum and minimum values.
 * Apart from these APIs, there is a provision to display only a limited set of
 * values out of the possible values. APIs to select the individual field limits
 * are intended for this purpose.
 *
 * The whole widget is left aligned and its size grows horizontally depending
 * on the current format and each field's visible/disabled state.
 *
 * Datetime individual field selection is implemented in a modular style.
 * Module can be implemented as a Ctxpopup based selection or an ISE based
 * selection, or even a spinner like selection.
 *
 * <b>Datetime Module design:</b>
 *
 * The following functions are expected to be implemented in a Datetime module:
 *
 * <b>Field creation:</b>
 * <pre>
 *
 *  __________                                            __________
 * |          |----- obj_hook() ---------------------->>>|          |
 * |          |<<<----------------returns Mod_data ------|          |
 * | Datetime |_______                                   |          |
 * |  widget  |       |Assign module call backs          |  Module  |
 * |   base   |<<<____|                                  |          |
 * |          |                                          |          |
 * |          |----- field_create() ------------------>>>|          |
 * |__________|<<<----------------returns field_obj -----|__________|
 *
 * </pre>
 *
 * <b>Field value setting:</b>
 * <pre>
 *
 *  __________                                          __________
 * |          |                                        |          |
 * | Datetime |<<<----------elm_datetime_value_set()---|          |
 * |  widget  |                                        |  Module  |
 * |   base   |----display_field_value()------------>>>|          |
 * |__________|                                        |__________|
 *
 * </pre>
 *
 * <b>del_hook:</b>
 * <pre>
 *  __________                                          __________
 * |          |                                        |          |
 * | Datetime |----obj_unhook()-------------------->>>>|          |
 * |  widget  |                                        |  Module  |
 * |   base   |         <<<-----frees mod_data---------|          |
 * |__________|                                        |__________|
 *
 * </pre>
 *
 *
 * Any module can use the following shared functions that are implemented in elm_datetime.c :
 *
 * <b>field_format_get()</b> - Gives the field format.
 *
 * <b>field_limit_get()</b>  - Gives the field minimum and maximum limits.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for datetime objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li @b "changed" - Whenever the Datetime field value is changed, this
 * signal is sent.
 *
 * @li @b "language,changed" - Whenever the system locale changes, this
 * signal is sent.
 *
 * @{
 */

/**
 * @brief Enumeration that identifies a Datetime field, the widget supports 6 fields : Year, Month,
 *        Date, Hour, Minute, AM/PM
 *
 */
typedef enum _Elm_Datetime_Field_Type
{
   ELM_DATETIME_YEAR    = 0, /**< Indicates the Year field */
   ELM_DATETIME_MONTH   = 1, /**< Indicates the Month field */
   ELM_DATETIME_DATE    = 2, /**< Indicates the Date field */
   ELM_DATETIME_HOUR    = 3, /**< Indicates the Hour field */
   ELM_DATETIME_MINUTE  = 4, /**< Indicates the Minute field */
   ELM_DATETIME_AMPM    = 5, /**< Indicates the AM/PM field */
   ELM_DATETIME_LAST    = 6
} Elm_Datetime_Field_Type;

typedef enum _Elm_Datetime_Picker_Type
{
   ELM_DATETIME_DATE_PICKER    = 0,
   ELM_DATETIME_TIME_PICKER   = 1
} Elm_Datetime_Picker_Type;

/**
 * @brief Adds a new datetime widget.
 *
 * @since_tizen 2.3
 *
 * @remarks The default datetime format and corresponding strings are based on the current locale.
 *
 * @remarks This function inserts a new datetime widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object *elm_datetime_add(Evas_Object *parent);

/**
 * @brief Gets the datetime format. Format is a combination of allowed LIBC date format
 *        specifiers like: "%b %d, %Y %I : %M %p".
 *
 * @since_tizen 2.3
 *
 * @remarks The maximum allowed format length is @c 64 chars.
 *
 * @remarks The format can include separators for each individual datetime field except
 *          for the AM/PM field.
 *
 * @remarks Each separator can be a maximum of @c 6 UTF-8 bytes.
 *          Space is also taken as a separator.
 *
 * @remarks Following are the allowed set of format specifiers for each datetime field.
 *
 * @b %%Y : The year as a decimal number including the century.
 *
 * @b %%y : The year as a decimal number without a century (range 00 to 99).
 *
 * @b %%m : The month as a decimal number (range 01 to 12).
 *
 * @b %%b : The abbreviated month name according to the current locale.
 *
 * @b %%B : The full month name according to the current locale.
 *
 * @b %%h : The abbreviated month name according to the current locale(same as %%b).
 *
 * @b %%d : The day of the month as a decimal number (range 01 to 31).
 *
 * @b %%e : The day of the month as a decimal number (range 1 to 31). Single
 *          digits are preceded by a blank.
 *
 * @b %%I : The hour as a decimal number using a 12-hour clock (range 01 to 12).
 *
 * @b %%H : The hour as a decimal number using a 24-hour clock (range 00 to 23).
 *
 * @b %%k : The hour (24-hour clock) as a decimal number (range 0 to 23). Single
 *          digits are preceded by a blank.
 *
 * @b %%l : The hour (12-hour clock) as a decimal number (range 1 to 12). Single
 *          digits are preceded by a blank.
 *
 * @b %%M : The minute as a decimal number (range 00 to 59).
 *
 * @b %%p : Either 'AM' or 'PM' according to the given time value, or the
 *          corresponding strings for the current locale. Noon is treated as 'PM'
 *          and midnight as 'AM'.
 *
 * @b %%P : Like %p, but in lower case: 'am' or 'pm' or a corresponding string for
 *          the current locale.
 *
 * @b %%c : The preferred date and time representation for the current locale.
 *
 * @b %%x : The preferred date representation for the current locale without the time.
 *
 * @b %%X : The preferred time representation for the current locale without the date.
 *
 * @b %%r : The complete calendar time using the AM/PM format of the current locale.
 *
 * @b %%R : The hour and minute in decimal numbers using the format %H:%M.
 *
 * @b %%T : The time of the day in decimal numbers using the format %H:%M:%S.
 *
 * @b %%D : The date using the format %%m/%%d/%%y.
 *
 * @b %%F : The date using the format %%Y-%%m-%%d.
 *
 * @remarks These specifiers can be arranged in any order and the widget displays the
 *          fields accordingly.
 *
 * @remarks The default format is taken as per the system locale settings.
 *
 * @param[in] obj The datetime object
 * @return The datetime format string \n
 *         Example: "%b %d, %Y %I : %M %p".
 *
 * @see elm_datetime_format_set()
 */
   EAPI const char *elm_datetime_format_get(const Evas_Object *obj);

/**
 * @brief Sets the datetime format. Format is a combination of allowed LIBC date format
 *        specifiers like: "%b %d, %Y %I : %M %p".
 *
 * @since_tizen 2.3
 *
 * @remarks The maximum allowed format length is @c 64 chars.
 *
 * @remarks The format can include separators for each individual datetime field except
 *          for the AM/PM field.
 *
 * @remarks Each separator can be a maximum of @c 6 UTF-8 bytes.
 *          Space is also taken as a separator.
 *
 * @remarks Following are the allowed set of format specifiers for each datetime field.
 *
 * @b %%Y : The year as a decimal number including the century.
 *
 * @b %%y : The year as a decimal number without a century (range 00 to 99).
 *
 * @b %%m : The month as a decimal number (range 01 to 12).
 *
 * @b %%b : The abbreviated month name according to the current locale.
 *
 * @b %%B : The full month name according to the current locale.
 *
 * @b %%h : The abbreviated month name according to the current locale(same as %%b).
 *
 * @b %%d : The day of the month as a decimal number (range 01 to 31).
 *
 * @b %%e : The day of the month as a decimal number (range 1 to 31). Single
 *          digits are preceded by a blank.
 *
 * @b %%I : The hour as a decimal number using a 12-hour clock (range 01 to 12).
 *
 * @b %%H : The hour as a decimal number using a 24-hour clock (range 00 to 23).
 *
 * @b %%k : The hour (24-hour clock) as a decimal number (range 0 to 23). Single
 *          digits are preceded by a blank.
 *
 * @b %%l : The hour (12-hour clock) as a decimal number (range 1 to 12). Single
 *          digits are preceded by a blank.
 *
 * @b %%M : The minute as a decimal number (range 00 to 59).
 *
 * @b %%p : Either 'AM' or 'PM' according to the given time value, or the
 *          corresponding strings for the current locale. Noon is treated as 'PM'
 *          and midnight as 'AM'.
 *
 * @b %%P : Like %p, but in lower case: 'am' or 'pm' or a corresponding string for
 *          the current locale.
 *
 * @b %%c : The preferred date and time representation for the current locale.
 *
 * @b %%x : The preferred date representation for the current locale without the time.
 *
 * @b %%X : The preferred time representation for the current locale without the date.
 *
 * @b %%r : The complete calendar time using the AM/PM format of the current locale.
 *
 * @b %%R : The hour and minute in decimal numbers using the format %H:%M.
 *
 * @b %%T : The time of the day in decimal numbers using the format %H:%M:%S.
 *
 * @b %%D : The date using the format %%m/%%d/%%y.
 *
 * @b %%F : The date using the format %%Y-%%m-%%d.
 *
 * @remarks These specifiers can be arranged in any order and the widget displays the
 *          fields accordingly.
 *
 * @remarks The default format is taken as per the system locale settings.
 *
 * @param[in] obj The datetime object
 * @param[in] fmt The datetime format
 *
 * @see elm_datetime_format_get()
 */
EAPI void elm_datetime_format_set(Evas_Object *obj, const char *fmt);

/**
 * @brief Gets the upper boundary of a field.
 *
 * @since_tizen 2.3
 *
 * @remarks Year: years since 1900. Negative value represents a year below 1900 (year
 *          value -30 represents 1870). Year default range is from 70 to 137.
 *
 * @remarks Month: default value range is from 0 to 11.
 *
 * @remarks Date: default value range is from 1 to 31 according to the month value.
 *
 * @remarks Hour: default value is in terms of the 24 hr format (0~23).
 *
 * @remarks Minute: default value range is from 0 to 59.
 *
 * @param[in] obj The datetime object
 * @param[out] maxtime The time structure containing the maximum time value
 * @return @c EINA_TRUE if the maximum value is returned successfully,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_datetime_value_max_set()
 */
EAPI Eina_Bool elm_datetime_value_max_get(const Evas_Object *obj, struct tm *maxtime);

/**
 * @brief Sets the upper boundary of a field.
 *
 * @since_tizen 2.3
 *
 * @remarks Year: years since 1900. Negative value represents a year below 1900 (year
 *          value -30 represents 1870). Year default range is from 70 to 137.
 *
 * @remarks Month: default value range is from 0 to 11.
 *
 * @remarks Date: default value range is from 1 to 31 according to the month value.
 *
 * @remarks Hour: default value is in terms of the 24 hr format (0~23).
 *
 * @remarks Minute: default value range is from 0 to 59.
 *
 * @param[in] obj The datetime object
 * @param[in] maxtime The time structure containing the maximum time value.
 * @return @c EINA_TRUE if the maximum value is accepted,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_datetime_value_max_get()
 */
EAPI Eina_Bool elm_datetime_value_max_set(Evas_Object *obj, const struct tm *maxtime);

/**
 * @brief Gets the lower boundary of a field.
 *
 * @since_tizen 2.3
 *
 * @remarks Year: years since 1900. Negative value represents a year below 1900 (year
 *          value -30 represents 1870). Year default range is from 70 to 137.
 *
 * @remarks Month: default value range is from 0 to 11.
 *
 * @remarks Date: default value range is from 1 to 31 according to the month value.
 *
 * @remarks Hour: default value is in terms of the 24 hr format (0~23).
 *
 * @remarks Minute: default value range is from 0 to 59.
 *
 * @param[in] obj The datetime object
 * @param[out] mintime The time structure
 * @return @c EINA_TRUE if the minimum value is successfully returned,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_datetime_value_min_set()
 */
EAPI Eina_Bool elm_datetime_value_min_get(const Evas_Object *obj, struct tm *mintime);

/**
 * @brief Sets the lower boundary of a field.
 *
 * @since_tizen 2.3
 *
 * @remarks Year: years since 1900. Negative value represents a year below 1900 (year
 *          value -30 represents 1870). Year default range is from 70 to 137.
 *
 * @remarks Month: default value range is from 0 to 11.
 *
 * @remarks Date: default value range is from 1 to 31 according to the month value.
 *
 * @remarks Hour: default value is in terms of 24 hr format (0~23)
 *
 * @remarks Minute: default value range is from 0 to 59.
 *
 * @param[in] obj The datetime object
 * @param[in] mintime The time structure containing the minimum time value
 * @return @c EINA_TRUE if the minimum value is accepted,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_datetime_value_min_get()
 */
EAPI Eina_Bool elm_datetime_value_min_set(Evas_Object *obj, const struct tm *mintime);

/**
 * @brief Gets the field limits of a field.
 *
 * @since_tizen 2.3
 *
 * @remarks Limits can be set to individual fields, independently, except for the AM/PM field.
 *          Any field can display values only in between these Minimum and Maximum limits unless
 *          the corresponding time value is restricted from MinTime to MaxTime.
 *          That is, Min/ Max field limits always work under the limitations of MinTime/ MaxTime.
 *
 * @remarks There is no provision to set the limits of the AM/PM field.
 *
 * @param[in] obj The datetime object
 * @param[in] fieldtype The type of the field, @c ELM_DATETIME_YEAR
 * @param[out] min A reference to the field's minimum value
 * @param[out] max A reference to field's maximum value
 *
 * @see elm_datetime_field_limit_set()
 */
EAPI void      elm_datetime_field_limit_get(const Evas_Object *obj, Elm_Datetime_Field_Type fieldtype, int *min, int *max);

/**
 * @brief Sets the field limits of a field.
 *
 * @since_tizen 2.3
 *
 * @remarks Limits can be set to individual fields, independently, except for the AM/PM field.
 *          Any field can display values only in between these Minimum and Maximum limits unless
 *          the corresponding time value is restricted from MinTime to MaxTime.
 *          That is, Min/ Max field limits always work under the limitations of MinTime/ MaxTime.
 *
 * @remarks There is no provision to set the limits of the AM/PM field.
 *
 * @param[in] obj The datetime object
 * @param[in] fieldtype The type of the field, @c ELM_DATETIME_YEAR
 * @param[in] min A reference to the field's minimum value
 * @param[in] max A reference to thr field's maximum value
 *
 * @see elm_datetime_field_limit_set()
 */
EAPI void      elm_datetime_field_limit_set(Evas_Object *obj, Elm_Datetime_Field_Type fieldtype, int min, int max);

/**
 * @brief Gets the current value of a field.
 *
 * @since_tizen 2.3
 *
 * @remarks Year: years since 1900. Negative value represents a year below 1900 (year
 *          value -30 represents 1870). Year default range is from 70 to 137.
 *
 * @remarks Month: default value range is from 0 to 11.
 *
 * @remarks Date: default value range is from 1 to 31 according to the month value.
 *
 * @remarks Hour: default value is in terms of the 24 hr format (0~23).
 *
 * @remarks Minute: default value range is from 0 to 59.
 *
 * @param[in] obj The datetime object
 * @param[in] currtime The time structure
 * @return @c EINA_TRUE if the current time is returned successfully,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_datetime_field_value_set()
 */
EAPI Eina_Bool elm_datetime_value_get(const Evas_Object *obj, struct tm *currtime);

/**
 * @brief Sets the current value of the Datetime object.
 *
 * @since_tizen 2.3
 *
 * @remarks Year: years since 1900. Negative value represents a year below 1900 (year
 *          value -30 represents 1870). Year default range is from 70 to 137.
 *
 * @remarks Month: default value range is from 0 to 11.
 *
 * @remarks Date: default value range is from 1 to 31 according to the month value.
 *
 * @remarks Hour: default value is in terms of the 24 hr format (0~23)
 *
 * @remarks Minute: default value range is from 0 to 59.
 *
 *
 * @param[in] obj The datetime object
 * @param[in] newtime The time structure filled with values to be set
 * @return @c EINA_TRUE if the current time is set successfully,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_datetime_value_set()
 */
EAPI Eina_Bool elm_datetime_value_set(Evas_Object *obj, const struct tm *newtime);

/**
 * @brief Gets whether a field can be visible.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The datetime object
 * @param[in] fieldtype The type of the field, @c ELM_DATETIME_YEAR
 * @return @c EINA_TRUE if the field can be visible, 
 *         otherwise @c EINA_FALSE
 *
 * @see elm_datetime_field_visible_set()
 */
EAPI Eina_Bool elm_datetime_field_visible_get(const Evas_Object *obj, Elm_Datetime_Field_Type fieldtype);

/**
 * @brief Sets a field to be visible.
 *
 * @since_tizen 2.3
 *
 * @remarks Setting this API to @c true does not ensure that the field is visible, apart from
 *          this, the field's format must be present in the Datetime overall format.
 *          If a field's visibility is set to @c false then it won't appear even though
 *          its format is present in the overall format.
 *          So if and only if this API is set to @c true and the corresponding field's format
 *          is present in the Datetime format, the field is visible.
 *
 * @remarks By default the field visibility is set to @c true.
 *
 * @param[in] obj The datetime object
 * @param[in] fieldtype The type of the field, @c ELM_DATETIME_YEAR
 * @param[in] visible If @c EINA_TRUE the field can be visible, 
 *                otherwise @c EINA_FALSE
 *
 * @see elm_datetime_field_visible_get()
 */
EAPI void elm_datetime_field_visible_set(Evas_Object *obj, Elm_Datetime_Field_Type fieldtype, Eina_Bool visible);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief This API is used to get the picker layout.
.* @details Based on the format of the datetime date and time picker layouts are created.
 *          If format set for date or time then get the date or  picker layout passing approprite type
 *          respectively else it will return null.This picker layout can be used in any widget or layout.
 *
 * @param obj The datetime object
 * @param type Type of the field.ELM_DATETIME_DATE_PICKER or ELM_DATETIME_TIME_PICKER
 */
EAPI Evas_Object* elm_datetime_picker_get(Evas_Object *obj, Elm_Datetime_Picker_Type type);

/**
 * @}
 */
