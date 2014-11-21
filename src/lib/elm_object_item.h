/**
 * @typedef Elm_Object_Item_Signal_Cb
 *
 * @brief It is the Elm_Object_Item Signal Callback functions' prototype definition. @a data
 *        is going to have the auxiliary data pointer at the time of the callback registration.
 *        @a it is a pointer to Elm_Object_Item that has the edje object from where
 *        the signal comes. @a emission identifies the exact signal's emission
 *        string and @a source identifies the exact signal's source.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_signal_callback_add()
 *
 * @ingroup General
 */
typedef void                  (*Elm_Object_Item_Signal_Cb)(void *data, Elm_Object_Item *it, const char *emission, const char *source);

/**
 * @brief Gets the widget object's handle which contains a given item.
 *
 * @details This returns the widget object itself to which an item belongs.
 *
 * @since_tizen 2.3
 *
 * @remarks Every elm_object_item supports this API.
 *
 * @param[in] it The Elementary object item
 * @return The widget object
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_widget_get(const Elm_Object_Item *it);

/**
 * @brief Sets the content of an object item.
 *
 * @details This sets a new object to an item as a content object. If any object is
 *          already set as a content object in the same part, this previous object is
 *          deleted automatically.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary object items may have many contents.
 *
 * @param[in] it The Elementary object item
 * @param[in] part The content part name to set (@c NULL for the default content)
 * @param[in] content The new content of the object item
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_part_content_set(Elm_Object_Item *it, const char *part, Evas_Object *content);

/**
 * @def  elm_object_item_content_set
 * @brief Convenient macro for  elm_object_item_part_content_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_part_content_set
 * @ingroup General
 */
#define elm_object_item_content_set(it, content) elm_object_item_part_content_set((it), NULL, (content))

/**
 * @brief Gets the content of an object item.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary object items may have many contents.
 *
 * @param[in] it The Elementary object item
 * @param[in] part The content part name to unset (@c NULL for the default content)
 * @return The content of the object item, otherwise @c NULL for any error
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_part_content_get(const Elm_Object_Item *it, const char *part);

/**
 * @def  elm_object_item_content_get
 * @brief Convenient macro for  elm_object_item_part_content_get
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_part_content_get
 * @ingroup General
 */
#define elm_object_item_content_get(it) elm_object_item_part_content_get((it), NULL)

/**
 * @brief Unsets the content of an object item.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary object items may have many contents.
 *
 * @param[in] it The Elementary object item
 * @param[in] part The content part name to unset (@c NULL for the default content)
 * @return #EINA_TRUE if succeed, otherwise #EINA_FALSE.
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_part_content_unset(Elm_Object_Item *it, const char *part);

/**
 * @def  elm_object_item_content_unset
 * @brief Convenient macro for  elm_object_item_part_content_unset
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_part_content_unset
 * @ingroup General
 */
#define elm_object_item_content_unset(it) elm_object_item_part_content_unset((it), NULL)

/**
 * @brief Sets the label of an object item.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary object items may have many labels.
 *
 * @param[in] it The Elementary object item
 * @param[in] part The text part name to set (@c NULL for the default label)
 * @param[in] label The new text of the label
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_part_text_set(Elm_Object_Item *it, const char *part, const char *label);

/**
 * @brief Sets the macro label of an object item.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary object items may have many labels.
 *
 * @param[in] it The Elementary object item
 * @param[in] label The new text of the label
 *
 * @ingroup General
 */
#define elm_object_item_text_set(it, label) elm_object_item_part_text_set((it), NULL, (label))

/**
 * @brief Gets the label of an object item.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary object items may have many labels.
 *
 * @param[in] it The Elementary object item
 * @param[in] part The text part name to get (@c NULL for the default label)
 * @return The text of the label, otherwise @c NULL for any error
 *
 * @ingroup General
 */
EAPI const char                  *elm_object_item_part_text_get(const Elm_Object_Item *it, const char *part);

/**
 * @def  elm_object_item_text_get
 * @brief Convenient macro for  elm_object_item_part_text_get
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_part_text_get
 * @ingroup General
 */
#define elm_object_item_text_get(it) elm_object_item_part_text_get((it), NULL)

/**
 * @brief Sets the text for an object item's part, marking it as translatable.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks The string to set as @a text must be the original one. Do not pass the
 *          return of gettext() here. Elementary translates the string
 *          internally and sets it on the object item using
 *          elm_object_item_part_text_set(), also storing the original string so that it
 *          can be automatically translated when the language is changed with
 *          elm_language_set(). The @a domain is stored along with it to find the
 *          translation in the correct catalog. It can be @c NULL, in which case it uses
 *          whatever domain is set by the application with textdomain(). This is
 *          useful in case you are building a library on top of Elementary that should have
 *          its own translatable strings, that should not be mixed with those of programs
 *          using the library.
 *
 * @param[in] it The object item containing the text part
 * @param[in] part The name of the part to set
 * @param[in] domain The translation domain to use
 * @param[in] text The original, non-translated text to set
 *
 * @ingroup General
 */
EAPI void      elm_object_item_domain_translatable_part_text_set(Elm_Object_Item *it, const char *part, const char *domain, const char *text);

/**
 * @def  elm_object_item_domain_translatable_text_set
 * @brief Convenient macro for  elm_object_item_domain_translatable_part_text_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_domain_translatable_part_text_set
 * @ingroup General
 */
#define elm_object_item_domain_translatable_text_set(it, domain, text) elm_object_item_domain_translatable_part_text_set((it), NULL, (domain), (text))

/**
 * @def  elm_object_item_translatable_text_set
 * @brief Convenient macro for  elm_object_item_domain_translatable_part_text_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_domain_translatable_part_text_set
 * @ingroup General
 */
#define elm_object_item_translatable_text_set(it, text) elm_object_item_domain_translatable_part_text_set((it), NULL, NULL, (text))

/**
 * @def  elm_object_item_translatable_part_text_set
 * @brief Convenient macro for  elm_object_item_domain_translatable_part_text_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_domain_translatable_part_text_set
 * @ingroup General
 */
#define elm_object_item_translatable_part_text_set(it, part, text) elm_object_item_domain_translatable_part_text_set((it), (part), NULL, (text))

/**
 * @brief Gets the original string set as translatable for an object item.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks When setting translated strings, the function elm_object_item_part_text_get()
 *          returns the translation returned by gettext(). To get the original
 *          string use this function.
 *
 * @param[in] it The object item
 * @param[in] part The name of the part that is set
 *
 * @return The original, untranslated string
 *
 * @ingroup General
 */
EAPI const char *elm_object_item_translatable_part_text_get(const Elm_Object_Item *it, const char *part);

/**
 * @def  elm_object_item_translatable_text_get
 * @brief Convenient macro for  elm_object_item_translatable_part_text_get
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_translatable_part_text_get
 * @ingroup General
 */
#define elm_object_item_translatable_text_get(it) elm_object_item_translatable_part_text_get((it), NULL)

/**
 * @brief Marks the part text to be translatable.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks Once you mark the part text to be translatable, the text is translated
 *          internally regardless of elm_object_item_part_text_set() and
 *          elm_object_item_domain_translatable_part_text_set(). In another case, if you
 *          set the Elementary policy that all text should be translatable by default, you
 *          can set the part text to not be translated by calling this API.
 *
 * @param[in] it The object item containing the text part
 * @param[in] part The part name of the translatable text
 * @param[in] domain The translation domain to use
 * @param[in] translatable If @c EINA_TRUE, the part text is translated
 *                     internally, otherwise @c EINA_FALSE
 *
 * @see elm_object_item_domain_translatable_part_text_set()
 * @see elm_object_item_part_text_set()
 * @see elm_policy()
 *
 * @ingroup General
 */
EAPI void elm_object_item_domain_part_text_translatable_set(Elm_Object_Item *it, const char *part, const char *domain, Eina_Bool translatable);

/**
 * @def  elm_object_item_part_text_translatable_set
 * @brief Convenient macro for  elm_object_item_domain_part_text_translatable_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_domain_part_text_translatable_set
 * @ingroup General
 */
#define elm_object_item_part_text_translatable_set(it, part, translatable) elm_object_item_domain_part_text_translatable_set((it), (part), NULL, (translatable))

/**
 * @def  elm_object_item_domain_text_translatable_set
 * @brief Convenient macro for  elm_object_item_domain_part_text_translatable_set
 *
 * @since_tizen 2.3
 *
 * @see elm_object_item_domain_part_text_translatable_set
 * @ingroup General
 */
#define elm_object_item_domain_text_translatable_set(it, domain, translatable) elm_object_item_domain_part_text_translatable_set((it), NULL, (domain), (translatable))

/**
 * @brief Sets the text to read out when in the accessibility mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The object item which is to be described
 * @param[in] txt The text that describes the widget to people with poor or no vision
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_access_info_set(Elm_Object_Item *it, const char *txt);

/**
 * @brief Registers the object item as an accessible object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] item The elementary object item
 * @return The accessible object of the object item, otherwise @c NULL for any error
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_access_register(Elm_Object_Item *item);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Registers a part on an object item as an accessible object.
 * @since 1.8
 *
 * @param[in] item The elementary object item
 * @param[in] part The part name to set
 * @return The accessible object of the object item, otherwise @c NULL for any error
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_part_access_register(Elm_Object_Item *item, const char *part);

/**
 * @brief Unregisters the accessible object of the object item.
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] item The elementary object item
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_access_unregister(Elm_Object_Item *item);

/**
 * @brief Gets the accessible object of the object item.
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] item The elementary object item
 * @return The accessible object of the object item, otherwise @c NULL for any error
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_access_object_get(const Elm_Object_Item *item);

/**
 * @brief Sets the highlight order.
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] item The container object item
 * @param[in] objs The order of objects to pass highlight
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_access_order_set(Elm_Object_Item *item, Eina_List *objs);

/**
 * @brief Gets the highlight order.
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] item The container object item
 * @return The order of objects to pass highlight
 *
 * @ingroup General
 */
EAPI const Eina_List              *elm_object_item_access_order_get(const Elm_Object_Item *item);

/**
 * @brief Unsets the highlight order.
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] item The container object item
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_access_order_unset(Elm_Object_Item *item);

/**
 * @brief Gets the data associated with an object item.
 *
 * @since_tizen 2.3
 *
 * @remarks Every elm_object_item supports this API.
 *
 * @param[in] it The Elementary object item
 * @return The data associated with @a it
 *
 * @ingroup General
 */
EAPI void                        *elm_object_item_data_get(const Elm_Object_Item *it);

/**
 * @brief Sets the data associated with an object item.
 *
 * @since_tizen 2.3
 *
 * @remarks Every elm_object_item supports this API.
 *
 * @param[in] it The Elementary object item
 * @param[in] data The data to be associated with @a it
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_data_set(Elm_Object_Item *it, void *data);

/**
 * @internal
 * @remarks Tizen only feature for changeable GUI
 *
 * @brief Gets the edje layout.
 *
 * @remarks Every elm_object_item supports this API.
 *
 * @param it The Elementary object item
 *
 * @return An Evas_Object with the edje layout created internally.
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_edje_get(Elm_Object_Item *it);

/**
 * @brief Sends a signal to the edje object of the widget item.
 *
 * @details This function sends a signal to the edje object of the obj item. An
 *          edje program can respond to a signal by specifying matching
 *          'signal' and 'source' fields.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The Elementary object item
 * @param[in] emission The signal name
 * @param[in] source The signal source
 * @ingroup General
 */
EAPI void                         elm_object_item_signal_emit(Elm_Object_Item *it, const char *emission, const char *source);

/**
 * @brief Adds a callback for a signal emitted by the object item, edje object.
 *
 * @details This function connects a callback function to a signal emitted by the
 *          edje object of the object item.
 *          Globs can occur in either the emission or source name.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] it The elementary object item
 * @param[in] emission The signal name
 * @param[in] source The signal source
 * @param[in] func The callback function to be executed when the signal is
 *             emitted
 * @param[in] data A pointer to the data to pass to the callback function
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_signal_callback_add(Elm_Object_Item *it, const char *emission, const char *source, Elm_Object_Item_Signal_Cb func, void *data);

/**
 * @brief Removes a signal-triggered callback from the object item, edje object.
 *
 * @details This function removes the @b last callback, previously attached to
 *          a signal emitted by an underlying Edje object of @a it, whose
 *          parameters @a emission, @a source, and @c func match exactly with
 *          those passed to a previous call to
 *          elm_object_item_signal_callback_add(). The data pointer that is passed
 *          to this call is returned.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] it The elementary object item
 * @param[in] emission The signal name
 * @param[in] source The signal source
 * @param[in] func The callback function to be executed when the signal is
 *             emitted
 * @return The data pointer of the signal callback, otherwise @c NULL on
 *         errors
 *
 * @see elm_object_item_signal_callback_add()
 *
 * @ingroup General
 */
EAPI void                        *elm_object_item_signal_callback_del(Elm_Object_Item *it, const char *emission, const char *source, Elm_Object_Item_Signal_Cb func);

/**
 * @brief Sets the disabled state of a widget item.
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary object item can be @b disabled, in which state it won't
 *          receive input and, in general, is themed differently from
 *          its normal state, usually greyed out. Useful for contexts
 *          where you don't want your users to interact with some of the
 *          parts of your interface.
 *
 * @remarks This sets the state for the widget item, either disabling it or
 *          enabling it back.
 *
 * @param[in] it The Elementary object item
 * @param[in] disabled If @c EINA_TRUE the state is disabled,
 *                 otherwise @c EINA_FALSE if it is enabled
 *
 * @ingroup Styles
 */
EAPI void                         elm_object_item_disabled_set(Elm_Object_Item *it, Eina_Bool disabled);

/**
 * @brief Gets the disabled state of a widget item.
 *
 * @details This gets the state of the widget, which might be enabled or disabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The Elementary object item
 * @return @c EINA_TRUE if the widget item is disabled,
 *         otherwise @c EINA_FALSE if it's enabled (or on errors)
 *
 * @ingroup Styles
 */
EAPI Eina_Bool                    elm_object_item_disabled_get(const Elm_Object_Item *it);

/**
 * @brief Sets the function to be called when an item from the widget is
 *        freed.
 *
 * @since_tizen 2.3
 *
 * @remarks This function receives these parameters:
 *          @li void * item data
 *          @li Evas_Object * widget object
 *          @li Elm_Object_Item * widget item
 *
 * @remarks Every elm_object_item supports this API.
 *
 * @param[in] it The item to set the callback on
 * @param[in] del_cb The function called
 *
 * @see elm_object_item_del()
 * @ingroup General
 */
EAPI void                         elm_object_item_del_cb_set(Elm_Object_Item *it, Evas_Smart_Cb del_cb);

/**
 * @brief Deletes the given item.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The item to be deleted
 *
 * @ingroup General
 */
EAPI void                       elm_object_item_del(Elm_Object_Item *it);

/**
 * @brief Sets the text to be shown in a given object item's tooltips.
 *
 * @since_tizen 2.3
 *
 * @remarks Setup the text as a tooltip to the object. The item can have only one tooltip,
 *          so any previous tooltip data that is set with this function or
 *          elm_object_item_tooltip_content_cb_set() is removed.
 *
 * @param[in] it The target item
 * @param[in] text The text to set in the content
 *
 * @see elm_object_tooltip_text_set()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_tooltip_text_set(Elm_Object_Item *it, const char *text);

/**
 * @brief Disables the size restrictions on an object's tooltip.
 *
 * @details This function allows a tooltip to expand beyond its parent window's canvas.
 *          It is instead limited only by the size of the display.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The tooltip's anchor object
 * @param[in] disable If @c EINA_TRUE size restrictions are disabled,
 *                otherwise @c EINA_FALSE
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 * @ingroup General
 */
EAPI Eina_Bool                    elm_object_item_tooltip_window_mode_set(Elm_Object_Item *it, Eina_Bool disable);

/**
 * @brief Retrieves the size restriction state of an object's tooltip.
 *
 * @details This function returns whether a tooltip is allowed to expand beyond
 *          its parent window's canvas.
 *          It is instead limited only by the size of the display.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The tooltip's anchor object
 * @return @c EINA_TRUE if the size restrictions are disabled,
 *         otherwise @c EINA_FALSE
 *
 * @ingroup General
 */
EAPI Eina_Bool                    elm_object_item_tooltip_window_mode_get(const Elm_Object_Item *it);

/**
 * @brief Sets the content to be shown in the tooltip item.
 *
 * @since_tizen 2.3
 *
 * @remarks Setup the tooltip to the item. The item can have only one tooltip,
 *          so any previous tooltip data is removed. @a func(with @a data) is
 *          called every time you need show the tooltip and it should
 *          return a valid Evas_Object. This object is then managed fully by the
 *          tooltip system and is deleted when the tooltip is gone.
 *
 * @param[in] it The object item being attached as a tooltip
 * @param[in] func The function used to create the tooltip contents
 * @param[in] data The data to provide to @a func as callback data/context
 * @param[in] del_cb Called when data is not needed anymore, either when
 *               another callback replaces @a func, the tooltip is unset with
 *               elm_object_item_tooltip_unset() or the owner @a it
 *               dies. This callback receives as the given @a data as the first parameter
 *               and @a event_info is the item.
 *
 * @see elm_object_tooltip_content_cb_set()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_tooltip_content_cb_set(Elm_Object_Item *it, Elm_Tooltip_Item_Content_Cb func, const void *data, Evas_Smart_Cb del_cb);

/**
 * @brief Unsets the tooltip from the item.
 *
 * @details This removes the tooltip from the item. The callback provided as @a del_cb to
 *          elm_object_item_tooltip_content_cb_set() is called to notify that
 *          it is not used anymore.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The object item from which to remove previously set tooltip
 *
 * @see elm_object_tooltip_unset()
 * @see elm_object_item_tooltip_content_cb_set()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_tooltip_unset(Elm_Object_Item *it);

/**
 * @brief Sets a different style for this item tooltip.
 *
 * @since_tizen 2.3
 *
 * @remarks Before you set a style you should define a tooltip with
 *          elm_object_item_tooltip_content_cb_set() or
 *          elm_object_item_tooltip_text_set()
 *
 * @param[in] it The object item with the tooltip already set
 * @param[in] style The theme style to use (default, transparent, ...)
 *
 * @see elm_object_tooltip_style_set()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_tooltip_style_set(Elm_Object_Item *it, const char *style);

/**
 * @brief Gets the style for this item tooltip.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The object item with the tooltip already set
 * @return style The theme style in use, defaults to "default" \n
 *               If the object does not have a tooltip set, then @c NULL is returned.
 *
 * @see elm_object_tooltip_style_get()
 * @see elm_object_item_tooltip_style_set()
 *
 * @ingroup General
 */
EAPI const char                  *elm_object_item_tooltip_style_get(const Elm_Object_Item *it);

/**
 * @brief Sets the type of mouse pointer/cursor decoration to be shown,
 *        when the mouse pointer is over the given item.
 *
 * @since_tizen 2.3
 *
 * @remarks This function is analogous to elm_object_cursor_set(), but
 *          here the cursor's changing area is restricted to the item's
 *          area, and not the whole widget's. Note that the item cursors
 *          have precedence over widget cursors, so a mouse over an
 *          item with custom cursor set always shows @b that cursor.
 *
 * @remarks If this function is called twice for an object, a previously set
 *          cursor is unset on the second call.
 *
 * @param[in] it The item to customize the cursor on
 * @param[in] cursor The cursor type's name
 *
 * @see elm_object_cursor_set()
 * @see elm_object_item_cursor_get()
 * @see elm_object_item_cursor_unset()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_cursor_set(Elm_Object_Item *it, const char *cursor);

/**
 * @brief Gets the type of mouse pointer/cursor decoration set to be shown,
 *        when the mouse pointer is over the given item.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The item with the custom cursor set
 * @return The cursor type's name, otherwise @c NULL if no custom cursors
 *         are set to @a it (and on errors)
 *
 * @see elm_object_cursor_get()
 * @see elm_object_item_cursor_set()
 * @see elm_object_item_cursor_unset()
 *
 * @ingroup General
 */
EAPI const char                  *elm_object_item_cursor_get(const Elm_Object_Item *it);

/**
 * @brief Unsets any custom mouse pointer/cursor decoration set to be
 *        shown, when the mouse pointer is over the given
 *        item, thus making it show the @b default cursor again.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this call to undo any custom settings on this item's cursor
 *          decoration, bringing it back to defaults (no custom style set).
 *
 * @param[in] it The item
 *
 * @see elm_object_cursor_unset()
 * @see elm_object_item_cursor_set()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_cursor_unset(Elm_Object_Item *it);

/**
 * @brief Sets a different @b style for a given custom cursor set of an item.
 *
 * @since_tizen 2.3
 *
 * @remarks This function only makes sense when one is using custom mouse
 *          cursor decorations <b>defined in a theme file</b>, which can have,
 *          a given cursor name/type and <b>alternate styles</b> on it. It
 *          is analogous to elm_object_cursor_style_set(), but here it
 *          applies only to item objects.
 *
 * @remarks Before you set a cursor style you should have defined a
 *          custom cursor previously on the item, with
 *          elm_object_item_cursor_set()
 *
 * @param[in] it The item with the custom cursor set
 * @param[in] style The <b>theme style</b> to use (e.g. @c "default",
 *              @c "transparent", etc)
 *
 * @see elm_object_item_cursor_engine_only_set()
 * @see elm_object_item_cursor_style_get()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_cursor_style_set(Elm_Object_Item *it, const char *style);

/**
 * @brief Get the current @b style set for a given item's custom
 *        cursor.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The item with the custom cursor set
 * @return style The cursor style in use \n
 *               If the object does not have a cursor set, then @c NULL is returned.
 *
 * @see elm_object_item_cursor_style_set()
 *
 * @ingroup General
 */
EAPI const char                  *elm_object_item_cursor_style_get(const Elm_Object_Item *it);

/**
 * @brief Sets whether the (custom)cursor for a given item should be
 *        searched in its theme or should only rely on the
 *        rendering engine.
 *
 * @since_tizen 2.3
 *
 * @remarks This call is of use only if you've set a custom cursor
 *          for items using elm_object_item_cursor_set().
 *
 * @remarks By default, cursors are only looked for from those
 *          provided by the rendering engine.
 *
 * @param[in] it The item with custom (custom) cursor already set on it
 * @param[in] engine_only If @c EINA_TRUE cursors are looked for
 *                    only from those provided by the rendering engine, otherwise @c EINA_FALSE to
 *                    have them searched on the widget's theme as well
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_cursor_engine_only_set(Elm_Object_Item *it, Eina_Bool engine_only);

/**
 * @brief Gets whether the (custom) cursor for a given item is being
 *        searched in its theme or is only relying on the rendering
 *        engine.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The object item
 * @return @c EINA_TRUE, if cursors are being looked for only from
 *         those provided by the rendering engine, otherwise @c EINA_FALSE if they
 *         are being searched on the widget's theme as well.
 *
 * @see elm_object_item_cursor_engine_only_set()
 *
 * @ingroup General
 */
EAPI Eina_Bool                    elm_object_item_cursor_engine_only_get(const Elm_Object_Item *it);

/**
 * @brief Returns the track object of the item.
 *
 * @details This gets a rectangle object that represents the object item's internal
 *          object. If you want to check the geometry and visibility of the item, you
 *          can call the evas apis, such as evas_object_geometry_get() and
 *          evas_object_visible_get(), to the track object. Note that all of the
 *          widget items may/may not have the internal object so this API may
 *          return @c NULL if the widget item doesn't have it. Additionally, the
 *          widget item is managed/controlled by the widget, the widget item could
 *          be changed(moved, resized even deleted) anytime by its own widget's
 *          decision. So dont' change the track object and don't
 *          keep the track object on your side as far as possible, but get the track object
 *          the moment you need to refer. Otherwise, you need to add some
 *          callbacks to the track object to track it's attributes changes.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks After using the track object, call
 *          elm_object_item_untrack() paired to elm_object_item_track()
 *          without fail to free the track object properly. Don't delete the
 *          track object.
 *
 * @param[in] it The Elementary Object Item to be tracked
 * @return The track object
 *
 * @see elm_object_item_untrack()
 * @see elm_object_item_track_get()
 *
 * @ingroup General
 */
EAPI Evas_Object                 *elm_object_item_track(Elm_Object_Item *it);

/**
 * @brief Retrieves the track object of the item.
 *
 * @details This retrieves the track object that is returned from
 *          elm_object_item_track().
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] it The Elementary Object Item that returned the track object
 *
 * @see elm_object_item_track()
 * @see elm_object_item_track_get()
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_untrack(Elm_Object_Item *it);

/**
 * @brief Gets the track object reference count.
 *
 * @details This gets the reference count for the track object. Whenever you call
 *          elm_object_item_track(), the reference count is increased by
 *          one. Similarly, the reference count is decreased again when you call
 *          the elm_object_item_untrack(). Unless the reference count reaches to
 *          zero, the track object won't be deleted. So be sure to call
 *          elm_object_item_untrack() paired to the elm_object_item_track() call
 *          count.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] it The Elementary Object Item that returned the track object
 * @return The track object reference count
 *
 * @see elm_object_item_track()
 * @see elm_object_item_track_get()
 *
 * @ingroup General
 */
EAPI int                          elm_object_item_track_get(const Elm_Object_Item *it);

/**
 * @brief Sets the style of an object item.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The Elementary object item
 * @param[in] part The style of the object item
 *
 * @ingroup General
 */
EAPI void                         elm_object_item_style_set(Elm_Object_Item *it, const char *part);

/**
 * @brief Gets the style of an object item.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The Elementary object item
 * @return The style of the object item
 *
 * @ingroup General
 */
EAPI const char                  *elm_object_item_style_get(Elm_Object_Item *it);
