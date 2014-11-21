/**
 * @internal
 * @defgroup Web Web
 * @ingroup Elementary
 *
 * @image html web_inheritance_tree.png
 * @image latex web_inheritance_tree.eps
 *
 * @image html img/widget/web/preview-00.png
 * @image latex img/widget/web/preview-00.eps
 *
 * A web widget is used for displaying web pages (HTML/CSS/JS)
 * using WebKit-EFL. You must have compiled Elementary with
 * ewebkit support.
 *
 * Signals that you can add callbacks for are:
 * @li "download,request": A file download has been requested. Event info is
 * a pointer to Elm_Web_Download.
 * @li "editorclient,contents,changed": Editor client's content changed.
 * @li "editorclient,selection,changed": Editor client's selection changed.
 * @li "frame,created": A new frame was created. Event info is an
 * Evas_Object that can be handled with the WebKit's ewk_frame API.
 * @li "icon,received": An icon is received by the main frame.
 * @li "inputmethod,changed": The input method has changed. Event info is an
 * Eina_Bool indicating whether it's enabled or not.
 * @li "js,windowobject,clear": JS window object has been cleared.
 * @li "link,hover,in": Mouse cursor is hovering over a link. Event info
 * is a char *link[2], where the first string contains the URL that the link
 * points to, and the second one is the title of the link.
 * @li "link,hover,out": Mouse cursor has left the link.
 * @li "load,document,finished": Loading of a document has finished. Event info
 * is the frame that finished loading.
 * @li "load,error": Loading failed. Event info is a pointer to
 * Elm_Web_Frame_Load_Error.
 * @li "load,finished": Loading finished. Event info is @c NULL on success, on
 * error it's a pointer to Elm_Web_Frame_Load_Error.
 * @li "load,newwindow,show": A new window is created and is ready to be
 * shown.
 * @li "load,progress": Overall load progress. Event info is a pointer to
 * a double containing a value between @c 0.0 and @c 1.0.
 * @li "load,provisional": Started provisional load.
 * @li "load,started": Loading of a document has started.
 * @li "menubar,visible,get": Queries if the menubar is visible. Event info
 * is a pointer to Eina_Bool where the callback should set @c EINA_TRUE if
 * the menubar is visible, or @c EINA_FALSE in case it's not.
 * @li "menubar,visible,set": Informs menubar visibility. Event info is
 * an Eina_Bool indicating the visibility.
 * @li "popup,created": A dropdown widget is activated, requesting its
 * popup menu to be created. Event info is a pointer to Elm_Web_Menu.
 * @li "popup,willdelete": The web object is ready to destroy the popup
 * object created. Event info is a pointer to Elm_Web_Menu.
 * @li "ready": Page is fully loaded.
 * @li "scrollbars,visible,get": Queries visibility of scrollbars. Event
 * info is a pointer to Eina_Bool where the visibility state should be set.
 * @li "scrollbars,visible,set": Informs scrollbars visibility. Event info
 * is an Eina_Bool with the visibility state set.
 * @li "statusbar,text,set": Text of the statusbar has changed. Even info is
 * a string with the new text.
 * @li "statusbar,visible,get": Queries visibility of the status bar.
 * Event info is a pointer to Eina_Bool where the visibility state should be
 * set.
 * @li "statusbar,visible,set": Informs statusbar visibility. Event info is
 * an Eina_Bool with the visibility value.
 * @li "title,changed": Title of the main frame has changed. Event info is a
 * string with the new title.
 * @li "toolbars,visible,get": Queries visibility of toolbars. Event info
 * is a pointer to Eina_Bool where the visibility state should be set.
 * @li "toolbars,visible,set": Informs the visibility of toolbars. Event
 * info is an Eina_Bool with the visibility state.
 * @li "tooltip,text,set": Shows and sets the text of a tooltip. Event info is
 * a string with the text to show.
 * @li "uri,changed": URI of the main frame has changed. Event info is a string
 * with the new URI.
 * @li "view,resized": The web object's internal view has changed sized
 * @li "windows,close,request": A JavaScript request to close the current
 * window has been requested.
 * @li "zoom,animated,end": Animated zoom has finished.
 *
 * Available styles:
 * - default
 *
 * @{
 */

/**
 * @brief The structure type used to report load errors.
 *
 * @remarks Load errors are reported as signals by elm_web. All the strings are
 *          temporary references and should @b not be used after the signal
 *          callback returns. If it's required, make copies with strdup() or
 *          eina_stringshare_add() (they are not even guaranteed to be
 *          stringshared, so must use eina_stringshare_add() and not
 *          eina_stringshare_ref()).
 */
typedef struct _Elm_Web_Frame_Load_Error Elm_Web_Frame_Load_Error;

/**
 * @brief The structure type used to report load errors.
 *
 * @remarks Load errors are reported as signal by elm_web. All the strings are
 *          temporary references and should @b not be used after the signal
 *          callback returns. If it's required, make copies with strdup() or
 *          eina_stringshare_add() (they are not even guaranteed to be
 *          stringshared, so must use eina_stringshare_add() and not
 *          eina_stringshare_ref()).
 */
struct _Elm_Web_Frame_Load_Error
{
   int          code; /**< Numeric error code */
   Eina_Bool    is_cancellation; /**< Error produced by canceling a request */
   const char  *domain; /**< Error domain name */
   const char  *description; /**< Error description (already localized) */
   const char  *failing_url; /**< The URL that failed to load */
   Evas_Object *frame; /**< Frame object that produced the error */
};

/**
 * @brief The possibles types of items in a menu.
 */
typedef enum
{
   ELM_WEB_MENU_SEPARATOR,
   ELM_WEB_MENU_GROUP,
   ELM_WEB_MENU_OPTION
} Elm_Web_Menu_Item_Type;

/**
 * @brief The structure type describing the items in a menu.
 */
typedef struct _Elm_Web_Menu_Item Elm_Web_Menu_Item;

/**
 * @brief The structure describing the items in a menu.
 */
struct _Elm_Web_Menu_Item
{
   const char            *text; /**< The text for the item */
   Elm_Web_Menu_Item_Type type; /**< The type of the item */
};

/**
 * @brief The structure type describing the menu of a popup.
 *
 * @remarks This structure is passed as the @a event_info for the @c "popup,create"
 *          signal, which is emitted when a dropdown menu is opened. Users wanting
 *          to handle these popups by themselves should listen to this signal and
 *          set the @c handled property of the struct to @c EINA_TRUE. Leaving this
 *          property as @c EINA_FALSE means that the user does not handle the popup
 *          and the default implementation is used.
 *
 * @remarks When the popup is ready to be dismissed, a @c "popup,willdelete" signal
 *          is emitted to notify the user that it can destroy any object and
 *          free all data related to it.
 *
 * @see elm_web_popup_selected_set()
 * @see elm_web_popup_destroy()
 */
typedef struct _Elm_Web_Menu Elm_Web_Menu;

/**
 * @internal
 * @brief The structure type describing the menu of a popup.
 *
 * @remarks This structure is passed as the @a event_info for the @c "popup,create"
 *          signal, which is emitted when a dropdown menu is opened. Users wanting
 *          to handle these popups by themselves should listen to this signal and
 *          set the @c handled property of the struct to @c EINA_TRUE. Leaving this
 *          property as @c EINA_FALSE means that the user does not handle the popup
 *          and the default implementation is used.
 *
 * @remarks When the popup is ready to be dismissed, a @c "popup,willdelete" signal
 *          is emitted to notify the user that it can destroy any object and
 *          free all data related to it.
 *
 * @see elm_web_popup_selected_set()
 * @see elm_web_popup_destroy()
 */
struct _Elm_Web_Menu
{
   Eina_List *items; /**< List of #Elm_Web_Menu_Item */
   int        x; /**< The X position of the popup, relative to elm_web object */
   int        y; /**< The Y position of the popup, relative to elm_web object */
   int        width; /**< Width of the popup menu */
   int        height; /**< Height of the popup menu */

   Eina_Bool  handled : 1; /**< Set to @c EINA_TRUE by the user to indicate that the popup has been handled and the default implementation should be ignored. Otherwise leave it as @c EINA_FALSE */
};

typedef struct _Elm_Web_Download Elm_Web_Download;
struct _Elm_Web_Download
{
   const char *url;
};

/**
 * @brief Enumeration for the types of zoom levels available.
 */
typedef enum
{
   ELM_WEB_ZOOM_MODE_MANUAL = 0, /**< Zoom controlled normally by elm_web_zoom_set */
   ELM_WEB_ZOOM_MODE_AUTO_FIT, /**< Zoom until the content fits in the web object */
   ELM_WEB_ZOOM_MODE_AUTO_FILL, /**< Zoom until the content fills the web object */
   ELM_WEB_ZOOM_MODE_LAST /**< Sentinel value that indicates the end */
} Elm_Web_Zoom_Mode;

/**
 * @brief The structure type that represents an opaque handler containing the features (such as statusbar, menubar, etc)
 *        that are to be set on a newly requested window.
 */
typedef struct _Elm_Web_Window_Features Elm_Web_Window_Features;


/**
 * @brief Enumeration that defines the web window features.
 */
typedef enum
{
   ELM_WEB_WINDOW_FEATURE_TOOLBAR,
   ELM_WEB_WINDOW_FEATURE_STATUSBAR,
   ELM_WEB_WINDOW_FEATURE_SCROLLBARS,
   ELM_WEB_WINDOW_FEATURE_MENUBAR,
   ELM_WEB_WINDOW_FEATURE_LOCATIONBAR,
   ELM_WEB_WINDOW_FEATURE_FULLSCREEN
} Elm_Web_Window_Feature_Flag;

/**
 * @brief Called for the create_window hook.
 *
 * @param[in] data The user data pointer set when setting the hook function
 * @param[in] obj The elm_web object requesting the new window
 * @param[in] js If @c EINA_TRUE the request originated from
 *           JavaScript, otherwise @c EINA_FALSE
 * @param[in] window_features A pointer of #Elm_Web_Window_Features indicating
 *                        the features requested for the new window
 *
 * @return The @c elm_web widget where the request is loaded \n
 *         That is, if a new window or tab is created, the @c elm_web widget in it should be
 *         returned, and @b NOT the window object \n
 *         Returning @c NULL should cancel the request.
 *
 * @see elm_web_window_create_hook_set()
 */
typedef Evas_Object *(*Elm_Web_Window_Open)(void *data, Evas_Object *obj, Eina_Bool js, const Elm_Web_Window_Features *window_features);

/**
 * @brief Called for the JS alert hook.
 *
 * @param[in] data The user data pointer set when setting the hook function
 * @param[in] obj The elm_web object requesting the new window
 * @param[in] message The message to show in the alert dialog
 *
 * @return The object representing the alert dialog
 *         Elm_Web runs a second main loop to handle the dialog and the normal
 *         flow of the application is restored when the object is deleted, so
 *         the user should handle the popup properly in order to delete the object
 *         when the action is finished \n
 *         If the function returns @c NULL the popup is ignored.
 *
 * @see elm_web_dialog_alert_hook_set()
 */
typedef Evas_Object *(*Elm_Web_Dialog_Alert)(void *data, Evas_Object *obj, const char *message);

/**
 * @brief Called for the JS confirm hook.
 *
 * @param[in] data The user data pointer set when setting the hook function
 * @param[in] obj The elm_web object requesting the new window
 * @param[in] message The message to show in the confirm dialog
 * @param[out] ret The pointer to store the user selection \n
 *            If @c EINA_TRUE the user selected @c OK, otherwise @c EINA_FALSE
 *
 * @return The object representing the confirm dialog \n
 *         Elm_Web runs a second main loop to handle the dialog and normal
 *         flow of the application is restored when the object is deleted, so
 *         the user should handle the popup properly in order to delete the object
 *         when the action is finished \n
 *         If the function returns @c NULL the popup is ignored.
 *
 * @see elm_web_dialog_confirm_hook_set()
 */
typedef Evas_Object *(*Elm_Web_Dialog_Confirm)(void *data, Evas_Object *obj, const char *message, Eina_Bool *ret);

/**
 * @brief Called for the JS prompt hook.
 *
 * @param[in] data The ser data pointer set when setting the hook function
 * @param[in] obj The elm_web object requesting the new window
 * @param[in] message The message to show in the prompt dialog
 * @param[in] def_value The default value to present the user in the entry
 * @param[in] value The pointer to store the value given by the user \n
 *              Must be a malloc'ed string or @c NULL if the user canceled the popup
 * @param[out] ret The pointer to store the user selection \n
 *            If @c EINA_TRUE the user selected @c OK, otherwise @c EINA_FALSE
 *
 * @return The object representing the prompt dialog \n
 *         Elm_Web runs a second main loop to handle the dialog and normal
 *         flow of the application is restored when the object is deleted, so
 *         the user should handle the popup properly in order to delete the object
 *         when the action is finished \n
 *         If the function returns @c NULL the popup is ignored.
 *
 * @see elm_web_dialog_prompt_hook_set()
 */
typedef Evas_Object *(*Elm_Web_Dialog_Prompt)(void *data, Evas_Object *obj, const char *message, const char *def_value, const char **value, Eina_Bool *ret);

/**
 * @brief Called for the JS file selector hook.
 *
 * @param[in] data The user data pointer set when setting the hook function
 * @param[in] obj The elm_web object requesting the new window
 * @param[in] allows_multiple If @c EINA_TRUE multiple files can be selected
 * @param[in] accept_types The MIME types that are accepted
 * @param[out] selected The pointer to store the list of malloc'ed strings
 *                 containing the path to each file selected \n
 *                 Must be @c NULL if the file dialog is canceled.
 * @param[out] ret The pointer to store the user selection \n
 *            If @c EINA_TRUE the user selected @c OK, otherwise @c EINA_FALSE
 *
 * @return The object representing the file selector dialog \n
 *         Elm_Web runs a second main loop to handle the dialog and normal
 *         flow of the application is restored when the object is deleted, so
 *         the user should handle the popup properly in order to delete the object
 *         when the action is finished \n
 *         If the function returns @c NULL the popup is ignored.
 *
 * @see elm_web_dialog_file selector_hook_set()
 */
typedef Evas_Object *(*Elm_Web_Dialog_File_Selector)(void *data, Evas_Object *obj, Eina_Bool allows_multiple, Eina_List *accept_types, Eina_List **selected, Eina_Bool *ret);

/**
 * @brief Called for the JS console message hook.
 *
 * @remarks When a console message is added from JavaScript, any set function to the
 *          console message hook is called for the user to handle. There is no
 *          default implementation of this hook.
 *
 * @param[in] data The user data pointer set when setting the hook function
 * @param[in] obj The elm_web object from which the messsage originated
 * @param[in] message The message sent
 * @param[in] line_number The line number
 * @param[in] source_id The source ID
 *
 * @see elm_web_console_message_hook_set()
 */
typedef void (*Elm_Web_Console_Message)(void *data, Evas_Object *obj, const char *message, unsigned int line_number, const char *source_id);

/**
 * @brief Adds a new web object to the parent.
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @see elm_web_uri_set()
 * @see elm_web_webkit_view_get()
 */
EAPI Evas_Object      *elm_web_add(Evas_Object *parent);

/**
 * @brief Changes the user agent of elm_web object.
 *
 * @param[in] obj The object
 * @param[in] user_agent The string for the user agent
 */
EAPI void elm_web_useragent_set(Evas_Object *obj, const char *user_agent);

/**
 * @brief Returns the current user agent of elm_web object.
 *
 * @param[in] obj The object
 * @return The user agent string
 */
EAPI const char* elm_web_useragent_get(const Evas_Object *obj);

/**
 * @brief Gets an internal ewk_view object from the web object.
 *
 * @remarks Elementary may not provide some low level features of EWebKit,
 *          instead of cluttering the API with proxy methods we opted to
 *          return the internal reference. Be careful using it as it may
 *          interfere with elm_web behavior.
 *
 * @param[in] obj The web object
 * @return The internal ewk_view object, otherwise @c NULL if it does not
 *         exist (Failure to create or Elementary compiled without
 *         ewebkit)
 *
 * @see elm_web_add()
 */
EAPI Evas_Object      *elm_web_webkit_view_get(const Evas_Object *obj);

/**
 * @brief Sets the function to call when a new window is requested.
 *
 * @remarks This hook is called when a request to create a new window is
 *          issued from the web page loaded.
 *          There is no default implementation for this feature, so leaving this
 *          unset or passing @c NULL in @a func prevents new windows from
 *          opening.
 *
 * @param[in] obj The web object to set the hook function
 * @param[in] func The hook function to be called when a window is requested
 * @param[in] data The user data
 */
EAPI void              elm_web_window_create_hook_set(Evas_Object *obj, Elm_Web_Window_Open func, void *data);

/**
 * @brief Sets the function to call when an alert dialog is requested.
 *
 * @remarks This hook is called when a JavaScript alert dialog is requested.
 *          If no function is set or @c NULL is passed in @a func, the default
 *          implementation takes place.
 *
 * @param[in] obj The web object to set the hook function
 * @param[in] func The callback function to be used
 * @param[in] data The user data
 *
 * @see elm_web_inwin_mode_set()
 */
EAPI void              elm_web_dialog_alert_hook_set(Evas_Object *obj, Elm_Web_Dialog_Alert func, void *data);

/**
 * @brief Sets the function to call when an confirm dialog is requested.
 *
 * @remarks This hook is called when a JavaScript confirm dialog is requested.
 *          If no function is set or @c NULL is passed in @a func, the default
 *          implementation takes place.
 *
 * @param[in] obj The web object to set the hook function
 * @param[in] func The callback function to be used
 * @param[in] data The user data
 *
 * @see elm_web_inwin_mode_set()
 */
EAPI void              elm_web_dialog_confirm_hook_set(Evas_Object *obj, Elm_Web_Dialog_Confirm func, void *data);

/**
 * @brief Sets the function to call when an prompt dialog is requested.
 *
 * @remarks This hook is called when a JavaScript prompt dialog is requested.
 *          If no function is set or @c NULL is passed in @a func, the default
 *          implementation takes place.
 *
 * @param[in] obj The web object to set the hook function
 * @param[in] func The callback function to be used
 * @param[in] data The user data
 *
 * @see elm_web_inwin_mode_set()
 */
EAPI void              elm_web_dialog_prompt_hook_set(Evas_Object *obj, Elm_Web_Dialog_Prompt func, void *data);

/**
 * @brief Sets the function to call when an file selector dialog is requested.
 *
 * @remarks This hook is called when a JavaScript file selector dialog is
 *          requested.
 * @remarks If no function is set or @c NULL is passed in @a func, the default
 *          implementation takes place.
 *
 * @param[in] obj The web object to set the hook function
 * @param[in] func The callback function to be used
 * @param[in] data The user data
 *
 * @see elm_web_inwin_mode_set()
 */
EAPI void              elm_web_dialog_file_selector_hook_set(Evas_Object *obj, Elm_Web_Dialog_File_Selector func, void *data);

/**
 * @brief Sets the function to call when a console message is emitted from JS.
 *
 * @remarks This hook is called when a console message is emitted from
 *          JavaScript. There is no default implementation for this feature.
 *
 * @param[in] obj The web object to set the hook function
 * @param[in] func The callback function to be used
 * @param[in] data The user data
 */
EAPI void              elm_web_console_message_hook_set(Evas_Object *obj, Elm_Web_Console_Message func, void *data);

/**
 * @brief Gets the status of the tab propagation.
 *
 * @param[in] obj The web object to query
 * @return @c EINA_TRUE if tab propagation is enabled, otherwise @c EINA_FALSE
 *
 * @see elm_web_tab_propagate_set()
 */
EAPI Eina_Bool         elm_web_tab_propagate_get(const Evas_Object *obj);

/**
 * @brief Sets whether to use tab propagation.
 *
 * @remarks If tab propagation is enabled, whenever the user presses the Tab key,
 *          Elementary handles it and switches the focus to the next widget.
 *          The default value is disabled, where WebKit handles the Tab key to
 *          cycle focus through its internal objects and jumping to the next widget
 *          only when that cycle ends.
 *
 * @param[in] obj The web object
 * @param[in] propagate The boolean value that indicates whether to propagate Tab keys to Elementary
 */
EAPI void              elm_web_tab_propagate_set(Evas_Object *obj, Eina_Bool propagate);

/**
 * @brief Sets the URI for the web object.
 *
 * @remarks It must be a full URI, with resource included, in the form
 *          http://www.enlightenment.org or file:///tmp/something.html
 *
 * @param[in] obj The web object
 * @param[in] uri The URI to set
 * @return @c EINA_TRUE if the URI could be set, otherwise @c EINA_FALSE if an error occurs
 */
EAPI Eina_Bool         elm_web_uri_set(Evas_Object *obj, const char *uri);

/**
 * @brief Gets the current URI for the object.
 *
 * @remarks The returned string must not be freed and is guaranteed to be
 *          stringshared.
 *
 * @param[in] obj The web object
 * @return A stringshared internal string with the current URI, otherwise @c NULL on
 * failure
 */
EAPI const char       *elm_web_uri_get(const Evas_Object *obj);

/**
 * @brief Gets the current title.
 *
 * @remarks The returned string must not be freed and is guaranteed to be
 *          stringshared.
 *
 * @param[in] obj The web object
 * @return A stringshared internal string with the current title, otherwise @c NULL on
 * failure
 */
EAPI const char       *elm_web_title_get(const Evas_Object *obj);

/**
 * @brief Sets the background color to be used by the web object.
 *
 * @remarks This is the color that is used by default when the loaded page
 *          does not set it's own. Color values are pre-multiplied.
 *
 * @param[in] obj The web object
 * @param[in] r The red component
 * @param[in] g The green component
 * @param[in] b The blue component
 * @param[in] a The alpha component
 */
EAPI void              elm_web_bg_color_set(Evas_Object *obj, int r, int g, int b, int a);

/**
 * @brief Gets the background color to be used by the web object.
 *
 * @remarks This is the color that is used by default when the loaded page
 *          does not set it's own. Color values are pre-multiplied.
 *
 * @param[in] obj The web object
 * @param[out] r The red component
 * @param[out] g The green component
 * @param[out] b The blue component
 * @param[out] a The alpha component
 */
EAPI void              elm_web_bg_color_get(const Evas_Object *obj, int *r, int *g, int *b, int *a);

/**
 * @brief Gets a copy of the currently selected text.
 *
 * @remarks The string returned must be freed by the user when it's done with it.
 *
 * @param[in] obj The web object
 * @return A newly allocated string, otherwise @c NULL if nothing is selected or an
 *         error occurs
 */
EAPI const char       *elm_web_selection_get(const Evas_Object *obj);

/**
 * @brief Tells the web object which index in the currently open popup is selected.
 *
 * @remarks When the user handles the popup creation from the @c "popup,created" signal,
 *          it needs to tell the web object which item is selected by calling this
 *          function with the index corresponding to the item.
 *
 * @param[in] obj The web object
 * @param[in] index The index selected
 *
 * @see elm_web_popup_destroy()
 */
EAPI void              elm_web_popup_selected_set(Evas_Object *obj, int index);

/**
 * @brief Dismisses an open dropdown popup.
 *
 * @remarks When the popup from a dropdown widget is to be dismissed, either after
 *          selecting an option or to cancel it, this function must be called, which
 *          later emits a @c "popup,willdelete" signal to notify the user that
 *          any memory and objects related to this popup can be freed.
 *
 * @param[in] obj The web object
 * @return @c EINA_TRUE if the menu is successfully destroyed, otherwise @c EINA_FALSE
 *         if there is no menu to destroy
 */
EAPI Eina_Bool         elm_web_popup_destroy(Evas_Object *obj);

/**
 * @brief Searches the given string in a document.
 *
 * @param[in] obj The web object to search the text
 * @param[in] string The string to search
 * @param[in] case_sensitive The boolean value that indicates whether the search should be case sensitive
 * @param[in] forward The boolean value that indicates whether the search is from forwards or backwards
 * @param[in] wrap The boolean value that indicates whether the search should wrap at the end
 *
 * @return @c EINA_TRUE if the given string is found, otherwise @c EINA_FALSE if it is not
 */
EAPI Eina_Bool         elm_web_text_search(const Evas_Object *obj, const char *string, Eina_Bool case_sensitive, Eina_Bool forward, Eina_Bool wrap);

/**
 * @brief Marks matches of the given string in a document.
 *
 * @param[in] obj The web object to search text
 * @param[in] string The string to match
 * @param[in] case_sensitive The boolean value that indicates whether the match should be case sensitive
 * @param[in] highlight The boolean value that indicates whether the matches should be highlighted
 * @param[in] limit Maximum amount of matches, or zero to unlimited
 *
 * @return The number of matched @a string
 */
EAPI unsigned int      elm_web_text_matches_mark(Evas_Object *obj, const char *string, Eina_Bool case_sensitive, Eina_Bool highlight, unsigned int limit);

/**
 * @brief Clears all marked matches in the document.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool         elm_web_text_matches_unmark_all(Evas_Object *obj);

/**
 * @brief Sets whether to highlight the matched marks.
 *
 * @remarks If enabled, marks set with elm_web_text_matches_mark() and
 *          highlights it.
 *
 * @param[in] obj The web object
 * @param[in] highlight The boolean value that indicates whether to highlight the marks
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool         elm_web_text_matches_highlight_set(Evas_Object *obj, Eina_Bool highlight);

/**
 * @brief Gets whether highlighting marks is enabled.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE indicates that marks are set to be highlighted, 
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool         elm_web_text_matches_highlight_get(const Evas_Object *obj);

/**
 * @brief Gets the overall loading progress of the page.
 *
 * @remarks Returns the estimated loading progress of the page, with a value between
 *          @c 0.0 and @c 1.0. This is an estimated progress accounting for all the frames
 *          included in the page.
 *
 * @param[in] obj The web object
 *
 * @return A value between @c 0.0 and @c 1.0 indicating the progress, otherwise @c -1.0 on
 *         failure
 */
EAPI double            elm_web_load_progress_get(const Evas_Object *obj);

/**
 * @brief Stops loading the current page.
 *
 * @remarks Cancels the loading of the current page in the web object. This
 *          causes a @c "load,error" signal to be emitted, with the is_cancellation
 *          flag set to @c EINA_TRUE.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE if the cancel is successful, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool         elm_web_stop(Evas_Object *obj);

/**
 * @brief Requests a reload of the current document in the object.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool         elm_web_reload(Evas_Object *obj);

/**
 * @brief Requests a reload of the current document, avoiding any existing caches.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool         elm_web_reload_full(Evas_Object *obj);

/**
 * @brief Goes back one step in the browsing history.
 *
 * @remarks This is equivalent to calling elm_web_object_navigate(obj, -1);
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_web_history_enabled_set()
 * @see elm_web_back_possible()
 * @see elm_web_forward()
 * @see elm_web_navigate()
 */
EAPI Eina_Bool         elm_web_back(Evas_Object *obj);

/**
 * @brief Goes forward one step in the browsing history.
 *
 * @remarks This is equivalent to calling elm_web_object_navigate(obj, 1);
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_web_history_enabled_set()
 * @see elm_web_forward_possible_get()
 * @see elm_web_back()
 * @see elm_web_navigate()
 */
EAPI Eina_Bool         elm_web_forward(Evas_Object *obj);

/**
 * @brief Jumps the given number of steps in the browsing history.
 *
 * @remarks The @a steps value can be a negative integer backwards in history, or a
 *          positive to move forward.
 *
 * @param[in] obj The web object
 * @param[in] steps The number of steps to jump
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on error or if not enough
 *         history exists to jump the given number of steps
 *
 * @see elm_web_history_enabled_set()
 * @see elm_web_back()
 * @see elm_web_forward()
 */
EAPI Eina_Bool         elm_web_navigate(Evas_Object *obj, int steps);

/**
 * @brief Queries whether it's possible to go back in history.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE if it's possible to go back in history, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool         elm_web_back_possible_get(Evas_Object *obj);

/**
 * @brief Queries whether it's possible to go forward in history.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE if it's possible to go forward in history, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool         elm_web_forward_possible_get(Evas_Object *obj);

/**
 * @brief Queries whether it's possible to jump the given number of steps.
 *
 * @remarks The @a steps value can be a negative integer backwards in history, or a
 *          positive to move forward.
 *
 * @param[in] obj The web object
 * @param[in] steps The number of steps to check for
 *
 * @return @c EINA_TRUE if enough history exists to perform the given jump,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool         elm_web_navigate_possible_get(Evas_Object *obj, int steps);

/**
 * @brief Gets whether browsing history is enabled for the given object.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE if history is enabled, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool         elm_web_history_enabled_get(const Evas_Object *obj);

/**
 * @brief Enables or disables the browsing history.
 *
 * @param[in] obj The web object
 * @param[in] enabled The boolean value that indicates whether to enable or disable browsing history
 */
EAPI void              elm_web_history_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Sets the zoom level of the web object.
 *
 * @remarks Zoom level matches the Webkit API, so @c 1.0 means normal zoom, with higher
 *          values meaning zoom in and lower meaning zoom out. This function
 *          only affects the zoom level if the mode set with elm_web_zoom_mode_set()
 *          is ::ELM_WEB_ZOOM_MODE_MANUAL.
 *
 * @param[in] obj The web object
 * @param[in] zoom The zoom level to set
 */
EAPI void              elm_web_zoom_set(Evas_Object *obj, double zoom);

/**
 * @brief Gets the current zoom level set on the web object.
 *
 * @remarks Note that this is the zoom level set on the web object and not that
 *          of the underlying Webkit one. In the ::ELM_WEB_ZOOM_MODE_MANUAL mode,
 *          the two zoom levels should match, but for the other two modes the
 *          Webkit zoom is calculated internally to match the chosen mode without
 *          changing the zoom level set for the web object.
 *
 * @param[in] obj The web object
 *
 * @return The zoom level set on the object
 */
EAPI double            elm_web_zoom_get(const Evas_Object *obj);

/**
 * @brief Sets the zoom mode to use.
 *
 * @remarks The modes can be any of those defined in ::Elm_Web_Zoom_Mode, except
 *          ::ELM_WEB_ZOOM_MODE_LAST. The default is ::ELM_WEB_ZOOM_MODE_MANUAL.
 *          ::ELM_WEB_ZOOM_MODE_MANUAL means the zoom level is controlled
 *          with the elm_web_zoom_set() function.
 *          ::ELM_WEB_ZOOM_MODE_AUTO_FIT calculates the needed zoom level to
 *          make sure that the entire portion of the web object's content is shown.
 *          ::ELM_WEB_ZOOM_MODE_AUTO_FILL calculates the needed zoom level to
 *          fit the content in the web object's size, without leaving any space
 *          unused.
 *
 * @param[in] obj The web object
 * @param[in] mode The mode to set
 */
EAPI void              elm_web_zoom_mode_set(Evas_Object *obj, Elm_Web_Zoom_Mode mode);

/**
 * @brief Gets the currently set zoom mode
 *
 * @param[in] obj The web object
 *
 * @return The current zoom mode set for the object
 *         or ::ELM_WEB_ZOOM_MODE_LAST on error
 */
EAPI Elm_Web_Zoom_Mode elm_web_zoom_mode_get(const Evas_Object *obj);

/**
 * @brief Shows the given region in the web object.
 *
 * @param[in] obj The web object
 * @param[in] x The x coordinate of the region to show
 * @param[in] y The y coordinate of the region to show
 * @param[in] w The width of the region to show
 * @param[in] h The height of the region to show
 */
EAPI void              elm_web_region_show(Evas_Object *obj, int x, int y, int w, int h);

/**
 * @brief Brings the region to the visible area.
 *
 * @remarks Like elm_web_region_show(), but it animates the scrolling of the object
 *          to show the area.
 *
 * @param[in] obj The web object
 * @param[in] x The x coordinate of the region to show
 * @param[in] y The y coordinate of the region to show
 * @param[in] w The width of the region to show
 * @param[in] h The height of the region to show
 */
EAPI void              elm_web_region_bring_in(Evas_Object *obj, int x, int y, int w, int h);

/**
 * @brief Sets the default dialogs to use an Inwin instead of a normal window.
 *
 * @remarks If set, then the default implementation for the JavaScript dialogs and
 *          file selector are opened in an Inwin. Otherwise they use a
 *          normal separated window.
 *
 * @param[in] obj The web object
 * @param[in] value If @c EINA_TRUE use Inwin,
 *              otherwise @c EINA_FALSE to use a normal window
 */
EAPI void              elm_web_inwin_mode_set(Evas_Object *obj, Eina_Bool value);

/**
 * @brief Gets whether the Inwin mode is set for the current object.
 *
 * @param[in] obj The web object
 *
 * @return @c EINA_TRUE if the Inwin mode is set, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool         elm_web_inwin_mode_get(const Evas_Object *obj);

EAPI void              elm_web_window_features_ref(Elm_Web_Window_Features *wf);
EAPI void              elm_web_window_features_unref(Elm_Web_Window_Features *wf);

/**
 * @brief Gets the boolean properties from Elm_Web_Window_Features
 *        (such as statusbar, menubar, etc) that are on a window.
 *
 * @param[in] wf The web window features object
 * @param[in] flag The web window feature flag whose value is required
 *
 * @return @c EINA_TRUE if the flag is set, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool              elm_web_window_features_property_get(const Elm_Web_Window_Features *wf, Elm_Web_Window_Feature_Flag flag);

/**
 * @brief TODO : Adds documentation.
 *
 * @param[in] wf The web window features object
 * @param[out] x A co-ordinate of the web view window
 * @param[out] y A co-ordinate of the web view window
 * @param[out] w A co-ordinate of the web view window
 * @param[out] h A co-ordinate of the web view window
 */
EAPI void              elm_web_window_features_region_get(const Elm_Web_Window_Features *wf, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);

/**
 * @}
 */
