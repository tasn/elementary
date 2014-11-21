/**
 * @defgroup General General
 * @ingroup elm_infra_group
 *
 * @brief General Elementary API. Functions that don't relate to
 *        Elementary objects specifically.
 *
 * These are documented functions that init/shutdown the library,
 * that apply to generic Elementary objects, that deal with
 * configuration, etc.
 *
 * @{
 */

/**
 * @brief Enumeration that defines a couple of standard Evas_Object layers to be used
 *        with evas_object_layer_set().
 *
 * @remarks Whenever extending with new values, try to keep some padding
 *          to siblings so that there is room for further extensions.
 */
typedef enum
{
   ELM_OBJECT_LAYER_BACKGROUND = EVAS_LAYER_MIN + 64, /**< Where to place backgrounds */
   ELM_OBJECT_LAYER_DEFAULT = 0, /**< Evas_Object default layer (and thus for Elementary) */
   ELM_OBJECT_LAYER_FOCUS = EVAS_LAYER_MAX - 128, /**< Where the focus object visualization is */
   ELM_OBJECT_LAYER_TOOLTIP = EVAS_LAYER_MAX - 64, /**< Where to show tooltips */
   ELM_OBJECT_LAYER_CURSOR = EVAS_LAYER_MAX - 32, /**< Where to show cursors */
   ELM_OBJECT_LAYER_LAST /**< The last layer known by Elementary */
} Elm_Object_Layer;

/**************************************************************************/
EAPI extern int ELM_ECORE_EVENT_ETHUMB_CONNECT;

/**
 * @brief Emitted when the application has reconfigured the elementary settings because
 *        an external configuration tool asked it to.
 */
EAPI extern int ELM_EVENT_CONFIG_ALL_CHANGED;

/**
 * @brief Emitted when any Elementary's policy value is changed.
 */
EAPI extern int ELM_EVENT_POLICY_CHANGED;

/**
 * @typedef Elm_Event_Policy_Changed
 *
 * @brief The structure type containing data on the event when an Elementary policy has changed.
 */
typedef struct _Elm_Event_Policy_Changed Elm_Event_Policy_Changed;

/**
 * @struct _Elm_Event_Policy_Changed
 *
 * @brief The structure type containing data on the event when an Elementary policy has changed.
 */
struct _Elm_Event_Policy_Changed
{
   unsigned int policy; /**< The policy identifier */
   int          new_value; /**< The value that the policy had before the change */
   int          old_value; /**< The new value that the policy got */
};

/**
 * @brief Enumeration that defines the policy identifiers.
 */
typedef enum
{
   ELM_POLICY_QUIT, /**< Under which circumstances the application
                     * should quit automatically. @see
                     * Elm_Policy_Quit
                     */
   ELM_POLICY_EXIT, /**< Defines the elm_exit() behaviour. @see Elm_Policy_Exit
                     * @since 1.8
                     */
   ELM_POLICY_THROTTLE, /**< Defines how throttling should work @see Elm_Policy_Throttle
                         * @since 1.8
                         */
   ELM_POLICY_LAST
} Elm_Policy; /**< Elementary policy identifiers/groups enumeration.  @see elm_policy_set() */

/**
 * @brief Enumeration that defines the possible values for the #ELM_POLICY_QUIT policy.
 */
typedef enum
{
   ELM_POLICY_QUIT_NONE = 0, /**< Never quit the application
                              * automatically */
   ELM_POLICY_QUIT_LAST_WINDOW_CLOSED /**< Quit when the
                                       * application's last
                                       * window is closed */
} Elm_Policy_Quit;

/**
 * @brief Enumeration that defines the possible values for the #ELM_POLICY_EXIT policy.
 * @since 1.8
 */
typedef enum
{
   ELM_POLICY_EXIT_NONE = 0, /**< Just quit the main loop on elm_exit() */
   ELM_POLICY_EXIT_WINDOWS_DEL /**< Delete all the windows after quitting
                                * the main loop */
} Elm_Policy_Exit;

/**
 * @brief Enumeration that defines the possible values for the #ELM_POLICY_THROTTLE policy.
 * @since 1.8
 */
typedef enum
{
   ELM_POLICY_THROTTLE_CONFIG = 0, /**< Do whatever elementary config is configured to do */
   ELM_POLICY_THROTTLE_HIDDEN_ALWAYS, /**< Always throttle when all windows are no longer visible */
   ELM_POLICY_THROTTLE_NEVER /**< Never throttle when windows are hidden, regardless of the config settings */
} Elm_Policy_Throttle;

/**
 * @brief Enumeration of Elm Object Select mode
 */
typedef enum
{
   ELM_OBJECT_SELECT_MODE_DEFAULT = 0, /**< Default select mode */
   ELM_OBJECT_SELECT_MODE_ALWAYS, /**< Always select mode */
   ELM_OBJECT_SELECT_MODE_NONE, /**< No select mode */
   ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY, /**< No select mode with no finger size rule */
   ELM_OBJECT_SELECT_MODE_MAX
} Elm_Object_Select_Mode;

/**
 * @typedef Elm_Object_Item
 * @brief An Elementary Object item handle.
 */
typedef struct _Elm_Object_Item Elm_Object_Item;

typedef Eina_Bool             (*Elm_Event_Cb)(void *data, Evas_Object *obj, Evas_Object *src, Evas_Callback_Type type, void *event_info); /**< Function prototype definition for callbacks on input events happening on Elementary widgets. @a data receives the user data pointer passed to elm_object_event_callback_add(). @a src is a pointer to the widget on which the input event took place. @a type gets the type of this event and @a event_info, the struct with details on this event */

#ifndef ELM_LIB_QUICKLAUNCH
#define ELM_MAIN() int main(int argc, char **argv) {elm_init(argc, argv); return elm_main(argc, argv); } /**< Macro to be used after the elm_main() function */
#else
#define ELM_MAIN() int main(int argc, char **argv) {return elm_quicklaunch_fallback(argc, argv); } /**< Macro to be used after the elm_main() function */
#endif

/**************************************************************************/
/* General calls */

/**
 * @brief Initializes Elementary.
 *
 * @details This function initializes Elementary and increments a counter of
 *          the number of calls to it. It returns the new counter's value.
 *
 * @since_tizen 2.3
 *
 * @remarks This call is exported only for use by the ELM_MAIN()
 *          macro. There is no need to use this if you use this macro (which
 *          is highly advisable). An elm_main() should contain the entry
 *          point code for your application, having the same prototype as
 *          elm_init(), and @b not being static (putting the @c EAPI_MAIN symbol
 *          in front of its type declaration is advisable). The
 *          ELM_MAIN() call should be placed just after it.
 *
 * @param[in] argc The system's argument count value
 * @param[in] argv The system's pointer to the array of argument strings
 * @return The init counter value
 *
 * @see elm_shutdown()
 */
EAPI int       elm_init(int argc, char **argv);

/**
 * @brief Shuts down Elementary.
 *
 * @since_tizen 2.3
 *
 * @remarks This should be called at the end of your application, just
 *          before it ceases to do any more processing. This cleans up
 *          any permanent resources that your application may have allocated via
 *          Elementary that would otherwise persist.
 *
 * @remarks elm_shutdown() iterates the main loop until all ecore_evas are freed.
 *          There is a possibility to call your ecore callbacks(timer, animator, event,
 *          job, and etc.) in elm_shutdown().
 *
 * @return The init counter value
 *
 * @see elm_init()
 */
EAPI int       elm_shutdown(void);

/**
 * @brief Runs Elementary's main loop.
 *
 * @since_tizen 2.3
 *
 * @remarks This call should be issued just after all initialization is
 *          completed. This function returns until elm_exit() is
 *          called. It keeps looping, running the main
 *          (event/processing) loop for Elementary.
 *
 * @see elm_init()
 */
EAPI void      elm_run(void);

/**
 * @brief Asks to exit Elementary's main loop.
 *
 * @since_tizen 2.3
 *
 * @remarks If this call is issued, it flags the main loop to cease
 *          processing and returns back to its parent function (usually your
 *          elm_main() function). This does not mean that the main loop instantly quits.
 *          So your ecore callbacks(timer, animator, event, job, and etc.) have chances
 *          to be called even after elm_exit().
 *
 * @remarks Just after a request to
 *          close the window comes, the main loop is left.
 *
 * @remarks By using the appropriate #ELM_POLICY_QUIT on your Elementary
 *          applications, you are able to get this function called automatically for you.
 *
 * @see elm_init()
 */
EAPI void      elm_exit(void);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @param[in] ql_on The quicklaunch mode to be set.
 *
 * @since_tizen 2.3
 */
EAPI void      elm_quicklaunch_mode_set(Eina_Bool ql_on);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @return #EINA_TRUE if mode is on, otherwise #EINA_FALSE.
 *
 * @since_tizen 2.3
 */
EAPI Eina_Bool elm_quicklaunch_mode_get(void);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @param[in] argc The system's argument count value
 * @param[in] argv The system's pointer to the array of argument strings
 * @return The init counter value
 *
 * @since_tizen 2.3
 */
EAPI int       elm_quicklaunch_init(int argc, char **argv);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @param[in] argc The system's argument count value
 * @param[in] argv The system's pointer to the array of argument strings
 * @return The init counter value
 *
 * @since_tizen 2.3
 */
EAPI int       elm_quicklaunch_sub_init(int argc, char **argv);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @return The init counter value
 *
 * @since_tizen 2.3
 */
EAPI int       elm_quicklaunch_sub_shutdown(void);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @return The init counter value
 *
 * @since_tizen 2.3
 */
EAPI int       elm_quicklaunch_shutdown(void);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @since_tizen 2.3
 */
EAPI void      elm_quicklaunch_seed(void);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @param[in] argc The system's argument count value
 * @param[in] argv The system's pointer to the array of argument strings
 * @return #EINA_TRUE if succeed, otherwise #EINA_FALSE.
 *
 * @since_tizen 2.3
 */
EAPI Eina_Bool elm_quicklaunch_prepare(int argc, char **argv);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @param[in] argc The system's argument count value
 * @param[in] argv The system's pointer to the array of argument strings
 * @param[in] cwd The current working directory
 * @param[in] postfork_func The function will be called after fork
 * @param[in] postfork_data The used data for postfork_func
 * @return #EINA_TRUE if succeed, otherwise #EINA_FALSE
 *
 * @since_tizen 2.3
 */
EAPI Eina_Bool elm_quicklaunch_fork(int argc, char **argv, char *cwd, void (*postfork_func) (void *data), void *postfork_data);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @since_tizen 2.3
 */
EAPI void      elm_quicklaunch_cleanup(void);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @param[in] argc The system's argument count value
 * @param[in] argv The system's pointer to the array of argument strings
 * @return The return value from main function
 *
 * @since_tizen 2.3
 */
EAPI int       elm_quicklaunch_fallback(int argc, char **argv);

/**
 * @brief Exposed symbol used only by macros and should not be used by apps.
 *
 * @param[in] exe The executable path
 * @return The exectuable path
 *
 * @since_tizen 2.3
 */
EAPI char     *elm_quicklaunch_exe_path_get(const char *exe);

/**
 * @brief Sets a new policy's value (for a given policy group/identifier).
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary policies define an applications' behavior,
 *          somehow. These behaviors are divided into policy groups
 *          (see #Elm_Policy enumeration). This call emits the Ecore
 *          event #ELM_EVENT_POLICY_CHANGED, which can be hooked with
 *          handlers. An #Elm_Event_Policy_Changed struct is passed,
 *          after that.
 *
 * @remarks Currently, we have only one policy identifier/group
 *          (#ELM_POLICY_QUIT), which has two possible values.
 *
 * @param[in] policy The policy identifier, as in @ref Elm_Policy
 * @param[in] value The policy value, which depends on the identifier
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on error
 */
EAPI Eina_Bool elm_policy_set(unsigned int policy, int value);

/**
 * @brief Gets the policy value for the given policy identifier.
 *
 * @since_tizen 2.3
 *
 * @param[in] policy The policy identifier, as in #Elm_Policy
 * @return The currently set policy value for that identifier \n
 *         It is @c 0 if the @a policy passed is invalid.
 */
EAPI int       elm_policy_get(unsigned int policy);

/**
 * @brief Changes the language of the current application.
 *
 * @since_tizen 2.3
 *
 * @remarks The @a lang passed must be the full name of the locale to use, for
 *          example "en_US.utf8" or "es_ES@euro".
 *
 * @remarks Changing the language with this function makes Elementary run through
 *          all its widgets, translating strings set with
 *          elm_object_domain_translatable_text_part_set(). This way, an entire
 *          UI can have its language changed without having to restart the program.
 *
 * @remarks For more complex cases, like having formatted strings that need
 *          translation, widgets also emit a @c "language,changed" signal that
 *          the user can listen to, in order to manually translate the text.
 *
 * @param[in] lang The language to set, must be the full name of the locale
 */
EAPI void      elm_language_set(const char *lang);

/**
 * @}
 */
