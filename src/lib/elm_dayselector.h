/**
 * @internal
 * @defgroup Dayselector Dayselector
 * @ingroup Elementary
 *
 * @image html dayselector_inheritance_tree.png
 * @image latex dayselector_inheritance_tree.eps
 *
 * @image html img/widget/dayselector/preview-00.png
 * @image latex img/widget/dayselector/preview-00.eps
 *
 * @brief <b>Dayselector widget:</b>
 *
 *"elm_dayselector" is a day selection widget. It displays all seven days of
 * the week and allows the user to select multiple days.
 *
 * The selection can be toggled by just clicking on the day.
 *
 * Dayselector also provides the functionality to check whether a day is
 * selected or not.
 *
 * First day of the week is taken from the config settings by default. It can be
 * altered by using the elm_dayselector_week_start_set() API.
 *
 * APIs are provided for setting the duration of weekend
 * elm_dayselector_weekend_start_set() and elm_dayselector_weekend_length_set()
 * does this job.
 *
 * Two styles of weekdays and weekends are supported in Dayselector.
 * Applications can emit signals on individual check objects for setting the
 * weekday, weekend styles.
 *
 * Once the weekend start day or weekend length changes, all the weekday &
 * weekend styles are reset to the default style. It's the application's
 * responsibility to set the styles again by the sending corresponding signals.
 *
 * Supported common elm_object_item APIs.
 *
 * @li @ref elm_object_part_text_set,
 * @li @ref elm_object_part_text_get,
 * @li @ref elm_object_part_content_set,
 * @li @ref elm_object_part_content_get,
 * @li @ref elm_object_part_content_unset
 *
 * @li "day0" indicates Sunday, "day1" indicates Monday etc. continues and so,
 * "day6" indicates the Saturday part name.
 *
 * Application can change individual day display string by using the API
 * elm_object_part_text_set().
 *
 * elm_object_part_content_set() API sets the individual day object only if
 * the passed one is a Check widget.
 *
 * Check object representing a day can be set/get by the application by using
 * the elm_object_part_content_set/get APIs thus providing a way to handle
 * the different check styles for individual days.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for dayselector objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li @c "dayselector,changed" - When the user changes the state of a day.
 * @li @c "language,changed" - The program's language is changed.
 *
 * Available styles for dayselector are:
 * @li default
 *
 * @{
 */

/**
 * @brief Enumeration that identifies the day of the week.
 * @remarks This API can call the selection/unselection of a day with this as a parameter.
 *
 * @see elm_dayselector_day_selected_set()
 * @see elm_dayselector_day_selected_get()
 */
typedef enum
{
   ELM_DAYSELECTOR_SUN = 0,/**< Indicates Sunday */
   ELM_DAYSELECTOR_MON,    /**< Indicates Monday */
   ELM_DAYSELECTOR_TUE,    /**< Indicates Tuesday */
   ELM_DAYSELECTOR_WED,    /**< Indicates Wednesday */
   ELM_DAYSELECTOR_THU,    /**< Indicates Thursday */
   ELM_DAYSELECTOR_FRI,    /**< Indicates Friday */
   ELM_DAYSELECTOR_SAT,    /**< Indicates Saturday */
   ELM_DAYSELECTOR_MAX     /**< Sentinel value, @b don't use */
} Elm_Dayselector_Day;

/**
 * @brief Adds the dayselector.
 *
 * @param[in] parent The parent object
 * @return A new dayselector object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object *elm_dayselector_add(Evas_Object *parent);

/**
 * @brief Sets the state of a given Dayselector_Day.
 *
 * @param[in] obj The Dayselector object
 * @param[in] day The Dayselector_Day that the user want to set state of
 * @param[in] selected The state of the day, @c EINA_TRUE is selected
 *
 * @see Elm_Dayselector_Day
 * @see elm_dayselector_day_selected_get()
 */
EAPI void   elm_dayselector_day_selected_set(Evas_Object *obj, Elm_Dayselector_Day day, Eina_Bool selected);

/**
 * @brief Gets the state of a given Dayselector_Day.
 *
 * @param[in] obj The Dayselector object
 * @param[in] day The Dayselector_Day that the user want to know state of
 * @return @c EINA_TRUE if the day is selected,
 *         otherwise @c EINA_FALSE
 *
 * @see Elm_Dayselector_Day
 * @see elm_dayselector_day_selected_set()
 */
EAPI Eina_Bool   elm_dayselector_day_selected_get(const Evas_Object *obj, Elm_Dayselector_Day day);

/**
 * @brief Sets the starting day of Dayselector.
 *
 * @param[in] obj The Dayselector object
 * @param[in] day The Dayselector_Day whose first day the user wants to display
 *
 * @see Elm_Dayselector_Day
 * @see elm_dayselector_week_start_get()
 */
EAPI void   elm_dayselector_week_start_set(Evas_Object *obj, Elm_Dayselector_Day day);

/**
 * @brief Gets the starting day of Dayselector.
 *
 * @param[in] obj The Dayselector object
 * @return The day from where the Dayselector displays all the weekdays in order
 *
 * @see Elm_Dayselector_Day
 * @see elm_dayselector_week_start_set()
 */
EAPI Elm_Dayselector_Day   elm_dayselector_week_start_get(const Evas_Object *obj);

/**
 * @brief Sets the weekend starting day of Dayselector.
 *
 * @param[in] obj The Dayselector object
 * @param[in] day The Dayselector_Day which is the first day from where weekend starts
 *
 * @see Elm_Dayselector_Day
 * @see elm_dayselector_weekend_start_get()
 */
EAPI void   elm_dayselector_weekend_start_set(Evas_Object *obj, Elm_Dayselector_Day day);

/**
 * @brief Gets the weekend starting day of Dayselector.
 *
 * @param[in] obj The Dayselector object
 * @return The Elm_Dayselector_Day Day from where the weekend starts
 *
 * @see Elm_Dayselector_Day
 * @see elm_dayselector_weekend_start_set()
 */
EAPI Elm_Dayselector_Day   elm_dayselector_weekend_start_get(const Evas_Object *obj);

/**
 * @brief Sets the weekend length of Dayselector.
 *
 * @param[in] obj The Dayselector object
 * @param[in] length The weekend length, number of days as an integer
 *
 * @see elm_dayselector_weekend_length_get()
 */
EAPI void   elm_dayselector_weekend_length_set(Evas_Object *obj, unsigned int length);

/**
 * @brief Gets the weekend length of Dayselector.
 *
 * @param[in] obj The Dayselector object
 * @return The number of days marked as a weekend
 *
 * @see Elm_Dayselector_Day
 * @see elm_dayselector_weekend_length_set()
 */
EAPI unsigned int   elm_dayselector_weekend_length_get(const Evas_Object *obj);

/**
 * @}
 */
