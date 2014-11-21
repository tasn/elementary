/**
 * @brief Sets the text of an object.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary objects may have many text parts (e.g. Action Slider).
 *
 * @param[in] obj The Elementary object
 * @param[in] part The text part name to set (@c NULL for the default part)
 * @param[in] text The new text of the part
 *
 * @ingroup General
 */
EAPI void                         elm_object_part_text_set(Evas_Object *obj, const char *part, const char *text);

/**
 * @def elm_object_text_set
 * @brief Convenient macro for elm_object_part_text_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_part_text_set
 * @ingroup General
 */
#define elm_object_text_set(obj, text) elm_object_part_text_set((obj), NULL, (text))

/**
 * @brief Gets the text of an object.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary objects may have many text parts (e.g. Action Slider).
 *
 * @param[in] obj The Elementary object
 * @param[in] part The text part name to get (@c NULL for the default part)
 * @return The text of the part, otherwise @c NULL for any error
 *
 * @ingroup General
 */
EAPI const char                  *elm_object_part_text_get(const Evas_Object *obj, const char *part);

/**
 * @def elm_object_text_get
 * @brief Convenient macro for elm_object_part_text_get
 *
 * @since_tizen 2.3
 *
 * @see elm_object_part_text_get
 * @ingroup General
 */
#define elm_object_text_get(obj) elm_object_part_text_get((obj), NULL)

/**
 * @brief Sets the text for an object's part, marking it translatable.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks The string to set as @a text must be the original one. Do not pass the
 *          return of gettext() here. Elementary translates the string
 *          internally and sets it on the object using elm_object_part_text_set(),
 *          also stores the original string so that it can be automatically
 *          translated when the language is changed with elm_language_set().
 *
 * @remarks The @a domain is also stored to find the translation in the
 *          correct catalog. It can be @c NULL, in which case it uses whatever
 *          domain is set by the application with textdomain(). This is useful
 *          in case you are building a library on top of Elementary that has
 *          its own translatable strings, that should not be mixed with those of
 *          programs using the library.
 *
 * @param[in] obj The object containing the text part
 * @param[in] part The name of the part to set
 * @param[in] domain The translation domain to use
 * @param[in] text The original, non-translated text to set
 *
 * @ingroup General
 */
EAPI void      elm_object_domain_translatable_part_text_set(Evas_Object *obj, const char *part, const char *domain, const char *text);

/**
 * @def elm_object_domain_translatable_text_set
 * @brief Convenient macro for elm_object_domain_translatable_part_text_set.
 *
 * @since_tizen 2.3
 *
 * @see elm_object_domain_translatable_part_text_set
 * @ingroup General
 */
#define elm_object_domain_translatable_text_set(obj, domain, text) elm_object_domain_translatable_part_text_set((obj), NULL, (domain), (text))

/**
 * @def  elm_object_translatable_text_set
 * @brief Convenient macro for  elm_object_domain_translatable_part_text_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_domain_translatable_part_text_set
 * @ingroup General
 */
#define elm_object_translatable_text_set(obj, text)                elm_object_domain_translatable_part_text_set((obj), NULL, NULL, (text))

/**
 * @def  elm_object_translatable_part_text_set
 * @brief Convenient macro for  elm_object_domain_translatable_part_text_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_domain_translatable_part_text_set
 * @ingroup General
 */
#define elm_object_translatable_part_text_set(obj, part, text)     elm_object_domain_translatable_part_text_set((obj), (part), NULL, (text))

/**
 * @brief Gets the original string set as translatable for an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks When setting translated strings, the function elm_object_part_text_get()
 *          returns the translation returned by gettext(). To get the
 *          original string use this function.
 *
 * @param[in] obj The object
 * @param[in] part The name of the part that is set
 *
 * @return The original, untranslated string
 *
 * @see elm_object_translatable_part_text_set()
 *
 * @ingroup General
 */
EAPI const char *elm_object_translatable_part_text_get(const Evas_Object *obj, const char *part);

/**
 * @def elm_object_translatable_text_get
 * @brief Convenient macro for elm_object_translatable_part_text_get
 *
 * @since_tizen 2.3
 *
 * @see elm_object_translatable_part_text_get
 * @ingroup General
 */
#define elm_object_translatable_text_get(obj) elm_object_translatable_part_text_get((obj), NULL)

/**
 * @brief Marks the part text to be transltable.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks Once you mark the part text to be translatable, the text is translated
 *          internally regardless of elm_object_part_text_set() and
 *          elm_object_domain_translatable_part_text_set(). In other cases, if you set the
 *          Elementary policy that all text should be translatable by default, you can set
 *          the part text to not be translated by calling this API.
 *
 * @param[in] obj The object containing the text part
 * @param[in] part The part name of the translatable text
 * @param[in] domain The translation domain to use
 * @param[in] translatable If @c EINA_TRUE the part text is translated internally, otherwise @c EINA_FALSE
 *
 * @see elm_object_domain_part_text_translatable_set()
 * @see elm_object_part_text_set()
 * @see elm_policy()
 *
 * @ingroup General
 */
EAPI void elm_object_domain_part_text_translatable_set(Evas_Object *obj, const char *part, const char *domain, Eina_Bool translatable);

/**
 * @def elm_object_part_text_translatable_set
 * @brief Convenient macro for elm_object_domain_part_text_translatable_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_domain_part_text_translatable_set
 * @ingroup General
 */
#define elm_object_part_text_translatable_set(obj, part, translatable) elm_object_domain_part_text_translatable_set((obj), (part), NULL, (translatable))

/**
 * @def elm_object_domain_text_translatable_set
 * @brief Convenient macro for elm_object_domain_part_text_translatable_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_domain_part_text_translatable_set
 * @ingroup General
 */
#define elm_object_domain_text_translatable_set(obj, domain, translatable) elm_object_domain_part_text_translatable_set((obj), NULL, (domain), (translatable))


/**
 * @brief Sets the content at a part of a given container widget.
 *
 * @since_tizen 2.3
 *
 * @remarks All widgets deriving from elm-container-class may hold
 *          child objects as content at given parts.  This sets new content to
 *          a given part. If any object is already set as a content object in
 *          the same part, the previous object is deleted automatically
 *          with this call. If you wish to preserve it, issue
 *          elm_object_part_content_unset() on it first.
 *
 * @param[in] obj The Elementary container widget
 * @param[in] part The container's part name to set (some might accept
 *             @c NULL for the default part)
 * @param[in] content The new content for that part
 *
 * @see elm_object_part_content_set()
 *
 * @ingroup General
 */
EAPI void                         elm_object_part_content_set(Evas_Object *obj, const char *part, Evas_Object *content);

/**
 * @def elm_object_content_set
 * @brief Convenient macro for elm_object_part_content_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_part_content_set
 * @ingroup General
 */
#define elm_object_content_set(obj, content) elm_object_part_content_set((obj), NULL, (content))

/**
 * @brief Gets the content at a part of a given container widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary container widget
 * @param[in] part The container's part name to get (some might accept
 *             @c NULL for the default part)
 * @return The content of the object at the given part, otherwise @c NULL in case of an error
 *
 * @see elm_object_part_content_set()
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_part_content_get(const Evas_Object *obj, const char *part);

/**
 * @def elm_object_content_get
 * @brief Convenient macro for elm_object_part_content_get
 *
 * @since_tizen 2.3
 *
 * @see elm_object_part_content_get
 * @ingroup General
 */
#define elm_object_content_get(obj) elm_object_part_content_get((obj), NULL)

/**
 * @brief Unsets the content at a part of a given container widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary container widget
 * @param[in] part The container's part name to unset (some might accept
 *             @c NULL for the default part)
 * @return The content of the object at the given part, otherwise @c NULL in case of an error
 *
 * @see elm_object_part_content_set()
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_part_content_unset(Evas_Object *obj, const char *part);

/**
 * @def elm_object_content_unset
 * @brief Convenient macro for elm_object_part_content_unset
 *
 * @since_tizen 2.3
 *
 * @see elm_object_part_content_unset
 * @ingroup General
 */
#define elm_object_content_unset(obj) elm_object_part_content_unset((obj), NULL)

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Registers a part of an object as an access object.
 * @since 1.8
 *
 * @param obj The Elementary object
 * @param part The object's part name to register
 *
 * @ingroup General
 */
EAPI Evas_Object *                elm_object_part_access_register(Evas_Object *obj, const char *part);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Get the access object which is registered to part
 * @since 1.8
 *
 * @param obj The Elementary object
 * @param part The object's part name to get access object
 *
 * @ingroup General
 */
EAPI Evas_Object *                elm_object_part_access_object_get(const Evas_Object *obj, const char *part);


/**
 * @brief Sets the text to read out when in the accessibility mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object which is to be described
 * @param[in] txt The text that describes the widget to people with poor or no vision
 *
 * @ingroup General
 */
EAPI void                         elm_object_access_info_set(Evas_Object *obj, const char *txt);

/**
 * @brief Gets a named object from the children.
 *
 * @details This function searches the children (or recursively children of
 *          children and so on) of the given @a obj object looking for a child with
 *          the name of @a name. If the child is found the object is returned, or
 *          @c NULL is returned. You can set the name of an object with
 *          evas_object_name_set(). If the name is not unique within the child
 *          objects (or the tree is @a recurse or is greater than @c 0) then it is
 *          undefined as to which child of that name is returned, so ensure that the name
 *          is unique amongst children. If recurse is set to @c -1 it recurses
 *          without limit.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The parent object whose children to look at
 * @param[in] name The name of the child to find
 * @param[in] recurse Set to the maximum number of levels to recurse (@c 0 == none, @c 1 is only to look at one level of children and so on)
 * @return The found object of that name, otherwise @c NULL if none are found
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_name_find(const Evas_Object *obj, const char *name, int recurse);

/**
 * @defgroup Styles Styles
 * @ingroup elm_infra_group
 *
 * @brief Widgets can have different visual styles. These generic API
 *        set styles of widgets, they support them (and the theme(s)
 *        do).
 *
 * @{
 */

/**
 * @brief Sets the style to be used by a given widget.
 *
 * @details This sets the style (by name) that defines the appearance of a
 *          widget. Styles vary from widget to widget and may also be defined
 *          by other themes by means of extensions and overlays.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary widget to style
 * @param[in] style The name of the style to use on it
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE
 *
 * @see elm_theme_extension_add()
 * @see elm_theme_extension_del()
 * @see elm_theme_overlay_add()
 * @see elm_theme_overlay_del()
 */
EAPI Eina_Bool    elm_object_style_set(Evas_Object *obj, const char *style);

/**
 * @brief Gets the style used by the widget.
 *
 * @details This gets the style being used for that widget. Note that the string
 *          pointer is only valid as long as the object is valid and the style doesn't
 *          change.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary widget to query for its style
 * @return The style name used
 *
 * @see elm_object_style_set()
 */
EAPI const char  *elm_object_style_get(const Evas_Object *obj);

/**
 * @}
 */

/**
 * @brief Sets the disabled state of an Elementary object.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary objects can be @b disabled, in which state they won't
 *          receive input and, in general, get themed differently from
 *          their normal state, usually greyed out. Useful for contexts
 *          where you don't want your users to interact with some of the
 *          parts of your interface.
 *
 * @remarks This sets the state for the widget, either disabling it or
 *          enabling it back.
 *
 * @param[in] obj The Elementary object to operate on
 * @param[in] disabled If @c EINA_TRUE that state is disabled,
 *                 otherwise @c EINA_FALSE if it is enabled
 * @ingroup General
 */
EAPI void         elm_object_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Gets the disabled state of an Elementary object.
 *
 * @details This gets the state of the widget, which might be enabled or disabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary object to operate on
 * @return @c EINA_TRUE if the widget is disabled, otherwise @c EINA_FALSE
 *         if it's enabled (or on errors)
 * @ingroup General
 */
EAPI Eina_Bool    elm_object_disabled_get(const Evas_Object *obj);

/**
 * @defgroup WidgetNavigation Widget Tree Navigation
 * @ingroup elm_infra_group
 *
 * @brief These functions provide checks on whether a Evas_Object is an Elementary widget,
 *        the possibility of getting a widget's parent, top level parent, and getting a
 *        string representation of a widget's type.
 *
 * @{
 */

/**
 * @brief Checks whether the given Evas Object is an Elementary widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object to query
 * @return @c EINA_TRUE if it is an elementary widget variant,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool    elm_object_widget_check(const Evas_Object *obj);

/**
 * @brief Gets the first parent of the given object that is an Elementary
 *        widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this to query an object's parent widget.
 *
 * @remarks Most of the Elementary users don't mix non-Elementary
 *          smart objects in the objects tree of an application, as this is
 *          an advanced usage of Elementary with Evas. So, except for the
 *          application's window, which is the root of that tree, all other
 *          objects have valid Elementary widget parents.
 *
 * @param[in] obj The Elementary object to query the parent from
 * @return The parent object that is an Elementary widget,
 *         otherwise @c NULL if it is not found
 */
EAPI Evas_Object *elm_object_parent_widget_get(const Evas_Object *obj);

/**
 * @brief Gets the top level parent of an Elementary widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object to query
 * @return The top level Elementary widget, otherwise @c NULL if the parent cannot be found
 */
EAPI Evas_Object *elm_object_top_widget_get(const Evas_Object *obj);

/**
 * @brief Gets the string that represents this Elementary widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary is weird and exposes itself as a single
 *          Evas_Object_Smart_Class of type "elm_widget", so
 *          evas_object_type_get() always returns that, making debug and
 *          language bindings hard. This function tries to mitigate this
 *          problem, but the solution is to change Elementary to use
 *          proper inheritance.
 *
 * @param[in] obj The object to query
 * @return The Elementary widget name, otherwise @c NULL if it is not a valid widget
 */
EAPI const char  *elm_object_widget_type_get(const Evas_Object *obj);

/**
 * @}
 */

/**
 * @brief Sends a signal to the widget edje object.
 *
 * @details This function sends a signal to the edje object of the @a obj. An
 *          edje program can respond to a signal by specifying matching
 *          'signal' and 'source' fields.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 * @param[in] emission The signal name
 * @param[in] source The signal source
 * @ingroup General
 */
EAPI void         elm_object_signal_emit(Evas_Object *obj, const char *emission, const char *source);

/**
 * @brief Adds a callback for a signal emitted by the widget edje object.
 *
 * @details This function connects a callback function to a signal emitted by the
 *          edje object of the @a obj.
 *          Globs can occur in either the emission or source name.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 * @param[in] emission The signal name
 * @param[in] source The signal source
 * @param[in] func The callback function to be executed when the signal is emitted
 * @param[in] data A pointer to the data to pass to the callback function
 * @ingroup General
 */
EAPI void         elm_object_signal_callback_add(Evas_Object *obj, const char *emission, const char *source, Edje_Signal_Cb func, void *data);

/**
 * @brief Removes a signal-triggered callback from a widget edje object.
 *
 * @details This function removes the @b last callback, previously attached to
 *          a signal emitted by an underlying Edje object of @a obj, whose
 *          parameters @a emission, @a source, and @c func match exactly with
 *          those passed to a previous call to
 *          elm_object_signal_callback_add(). The data pointer that is passed
 *          to this call is returned.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object handle
 * @param[in] emission The signal name
 * @param[in] source The signal source
 * @param[in] func The callback function to be executed when the signal is emitted
 * @return The data pointer of the signal callback, otherwise @c NULL in case of an error
 *
 * @ingroup General
 */
EAPI void        *elm_object_signal_callback_del(Evas_Object *obj, const char *emission, const char *source, Edje_Signal_Cb func);

/**
 * @brief Adds a callback for input events (key up, key down, mouse wheel)
 *        on a given Elementary widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Every widget in an Elementary interface is set to receive focus,
 *          with elm_object_focus_allow_set(), propagate @b all of its
 *          key up, key down, and mouse wheel input events up to its parent
 *          object, and so on. All of the focusable ones in this chain that
 *          had an event callback set, with this call, are able to treat
 *          those events. There are two ways of making the propagation of
 *          these event upwards in the tree of widgets to @b cease:
 *          - Just return @c EINA_TRUE on @a func. @c EINA_FALSE means that
 *            the event is @b not processed, so the propagation goes on.
 *          - The @a event_info pointer passed to @p func contains the
 *            event's structure and, if you OR its @a event_flags inner
 *            value to @c EVAS_EVENT_FLAG_ON_HOLD, you're telling Elementary
 *            has already handled it, thus killing the event's
 *            propagation.
 *
 * @remarks Your event callback is issued on those events taking
 *          place only if no other child widget of @a obj has consumed the
 *          event already.
 *
 * @remarks Not to be confused with
 *          evas_object_event_callback_add(), which adds event callbacks
 *          per type on general Evas objects (no event propagation
 *          infrastructure taken into account).
 *
 * @remarks Not to be confused with
 *          elm_object_signal_callback_add(), which adds callbacks to @b
 *          signals coming from a widget's theme, not input events.
 *
 * @remarks Not to be confused with
 *          edje_object_signal_callback_add(), which does the same as
 *          elm_object_signal_callback_add(), but directly on an Edje
 *          object.
 *
 * @remarks Not to be confused with
 *          evas_object_smart_callback_add(), which adds callbacks to smart
 *          objects' <b>smart events</b>, and not input events.
 *
 * @param[in] obj The widget to add an event callback on
 * @param[in] func The callback function to be executed when the event
 *             happens
 * @param[in] data The data to pass into @a func
 *
 * @see elm_object_event_callback_del()
 *
 * @ingroup General
 */
EAPI void         elm_object_event_callback_add(Evas_Object *obj, Elm_Event_Cb func, const void *data);

/**
 * @brief Removes an event callback from a widget.
 *
 * @details This function removes a callback, previously attached to the event emission
 *          by the @a obj.
 *          The parameters @a func and @a data must match exactly those passed to
 *          a previous call to elm_object_event_callback_add(). The data pointer that
 *          is passed to this call is returned.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 * @param[in] func The callback function to be executed when the event is
 *             emitted
 * @param[in] data The data to pass to the callback function
 * @return A data pointer
 * @ingroup General
 */
EAPI void        *elm_object_event_callback_del(Evas_Object *obj, Elm_Event_Cb func, const void *data);

/**
 * @brief Disables the orientation mode of a given widget.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks Orientation mode is used for widgets to change their styles or to send signals
 *          whenever its window degree is changed. If the orientation mode is enabled
 *          and the widget has different looks and styles for the window degree(0, 90,
 *          180, 270), it applies a style that is readied for the current degree,
 *          otherwise, it sends signals to its own edje to change its states if
 *          the style is not readied.
 *
 * @param[in] obj The Elementary object to operate in the orientation mode
 * @param[in] disabled If @c EINA_TRUE the state is disabled,
 *                 otherwise @c EINA_FALSE if it is enabled
 *
 * @ingroup General
 */
EAPI void        elm_object_orientation_mode_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Gets the orientation mode of a given widget.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary widget to query for its orientation mode
 * @return @c EINA_TRUE, if the orientation mode is disabled, otherwise @c EINA_FALSE
 *         if the orientation mode is enabled (or on errors)
 *
 * @see elm_object_orientation_mode_disabled_set()
 *
 * @ingroup General
 */
EAPI Eina_Bool   elm_object_orientation_mode_disabled_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature 2013.12.08
 *
 * @brief If show_region_repeat_disabled is set,
 *        do not call on_show_region of the parent object.
 *
 * Need Documents
 *
 * @ingroup General
 */
EAPI void        elm_object_show_region_repeat_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @internal
 * @remarks Tizen only feature 2013.12.08
 *
 * @brief If show_region_repeat_disabled is set,
 *        do not call on_show_region of the parent object.
 *
 * Need Documents
 *
 * @ingroup General
 */
EAPI Eina_Bool   elm_object_show_region_repeat_disabled_get(Evas_Object *obj);
