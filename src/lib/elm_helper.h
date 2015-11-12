/**
 * @defgroup Helper Helper
 * @ingroup Elementary
 *
 * The validation helper feature.
 *
 * @{
 */

struct _Elm_Validate_Content
{
   const char *text;
   Eina_Tmpstr *signal;
};
/**
 * Data for the elm_validator_regexp_helper()
 */
typedef struct _Elm_Validate_Content Elm_Validate_Content;

/**
 * The Regexp validator data.
 */
typedef struct _Elm_Validator_Regexp Elm_Validator_Regexp;

/**
 * @brief Enumeration that defines the regex error codes
 * @since 1.14
 */
typedef enum
{
   /** Regex maches to the Entrys text. */
   ELM_REG_NOERROR = 0,
   /** Failed to match. */
   ELM_REG_NOMATCH,
   /** Invalid regular expression. */
   ELM_REG_BADPAT,
} Elm_Regexp_Status;

/**
 * @brief Create a new regex validator.
 * General designed for validate inputed entry text.
 *
 * @param pattern The regex pattern
 * @param signal The part of signal name, which will be emitted to style
 * @return The regex validator
 *
 * @see elm_validator_regexp_del()
 * @see elm_validator_regex_regex_set()
 * @see elm_validator_regexp_status_get()
 * @see elm_validator_regexp_helper()
 *
 * @since 1.14
 */
EAPI Elm_Validator_Regexp *
elm_validator_regexp_new(const char *pattern, const char *signal) EINA_ARG_NONNULL(1);

/**
 * @brief Delete the existing regex validator.
 *
 * @param validator The given validator
 *
 * @see elm_validator_regexp_add()
 *
 * @since 1.14
 */
EAPI void
elm_validator_regexp_free(Elm_Validator_Regexp *validator) EINA_ARG_NONNULL(1);

/**
 * @brief Get the validation status.
 *
 * @param The given validator
 *
 * @note All return value see here: http://www.gnu.org/software/libc/manual/html_node/Regular-Expressions.html
 *
 * @since 1.14
 */
EAPI Elm_Regexp_Status
elm_validator_regexp_status_get(Elm_Validator_Regexp *validator) EINA_ARG_NONNULL(1);

#ifdef EO_BETA_API
/**
 * @brief The regex validator. Used as callback to validate event.
 *
 * Example:
 * @code
 * extern Evas_Object *parent;
 * Evas_Object *entry;
 * Elm_Validator_Regexp *re;
 *
 * //add validator
 * entry = elm_entry_add(parent);
 * re = elm_validator_regexp_add("^[0-9]*$");
 * eo_do(entry, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, re));
 *
 * //delete validator
 * eo_do(entry, eo_event_callback_del(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, re));
 * @endcode
 *
 * @see elm_validator_regexp_add()
 * @see elm_validotor_regex_regex_set()
 * @since 1.14
 */
EAPI Eina_Bool
elm_validator_regexp_helper(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
#endif

/**
 * @}
 */
