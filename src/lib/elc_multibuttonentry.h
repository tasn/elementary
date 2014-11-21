/**
 * @defgroup Multibuttonentry Multibuttonentry
 * @ingroup elm_widget_group
 *
 * @image html multibuttonentry_inheritance_tree.png
 * @image latex multibuttonentry_inheritance_tree.eps
 *
 * @brief A multi-button entry is a widget letting an user enter text and
 *        each chunk of text managed as a set of buttons.
 *
 * Each text button is inserted by pressing the "return" key. If there is no
 * space in the current row, a new button is added to the next row. When a text
 * button is pressed, it will become focused. Backspace removes the
 * focus. When the multi-button entry loses focus, items longer than
 * one line are shrunk to one line.
 *
 * The typical use case of multi-button entry is composing
 * emails/messages to a group of addresses, each of which is an item
 * that can be clicked for further actions.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for multi-button entry objects
 * (@since 1.8).
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "item,selected" - this is called when an item is selected by
 *       api, user interaction, and etc. this is also called when a
 *       user press back space while cursor is on the first field of
 *       entry.
 * - @c "item,added" - when a new multi-button entry item is added.
 * - @c "item,deleted" - when a multi-button entry item is deleted.
 * - @c "item,clicked" - this is called when an item is clicked by user
 *       interaction. Both "item,selected" and "item,clicked" are needed.
 * - @c "clicked" - when multi-button entry is clicked.
 * - @c "focused" - when multi-button entry is focused.
 * - @c "unfocused" - when multi-button entry is unfocused.
 * - @c "expanded" - when multi-button entry is expanded.
 * - @c "contracted" - when multi-button entry is contracted.
 * - @c "expand,state,changed" - when shrink mode state of
 *       multi-button entry is changed.
 * - @c "longpressed" - when multi-button entry is pressed for a long time.
 *
 * Default text parts of the multi-button entry widget that you can use are:
 * @li "default" - A label of the multi-button entry
 *
 * Default text parts of multi-button entry @b items that you can use are:
 * @li "default" - A label of the multi-button entry item
 *
 * Supported elm_object_item common APIs.
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_part_text_get
 *
 * @{
 */

/**
 * @brief Callback to be invoked when an item is added to the multibuttonentry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The parent object
 * @param[in] item_label The label corresponding to the added item.
 * @param[in] item_data data specific to this item.
 * @param[in] data data specific to the multibuttonentry.
 *
 * @return EINA_TRUE
 *         EINA_FALSE otherwise.
 */
typedef Eina_Bool                   (*Elm_Multibuttonentry_Item_Filter_Cb)(Evas_Object *obj, const char *item_label, const void *item_data, const void *data);

/**
 * @brief Add a new multibuttonentry to the parent
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object or NULL if it cannot be created
 */
EAPI Evas_Object               *elm_multibuttonentry_add(Evas_Object *parent);


/**
 * @brief Get the entry of the multibuttonentry object
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @return The entry object, or NULL if none
 */
EAPI Evas_Object               *elm_multibuttonentry_entry_get(const Evas_Object *obj);

/**
 * @brief Get the value of expanded state.
 *
 e @since_tizen 2.3
 *
 * @remarks In expanded state, the complete entry will be displayed.
 *           Otherwise, only single line of the entry will be displayed.
 *
 * @param[in] obj The multibuttonentry object
 * @return EINA_TRUE if the widget is in expanded state. EINA_FALSE if not.
 */
EAPI Eina_Bool                  elm_multibuttonentry_expanded_get(const Evas_Object *obj);

/**
 * @brief Set/Unset the multibuttonentry to expanded state.
 *
 * @since_tizen 2.3
 *
 * @remarks In expanded state, the complete entry will be displayed.
 *          Otherwise, only single line of the entry will be displayed.
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] expanded the value of expanded state.
 *        Set this to EINA_TRUE for expanded state.
 *        Set this to EINA_FALSE for single line state.
 */
EAPI void                       elm_multibuttonentry_expanded_set(Evas_Object *obj, Eina_Bool expanded);

/**
 * @brief Prepend a new item to the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] label The label of new item
 * @param[in] func The callback function to be invoked when this item is pressed.
 * @param[in] data The pointer to the data to be attached
 * @return A handle to the item added or NULL if not possible
 *
 * @see Use elm_object_item_del() to delete the item.
 */
EAPI Elm_Object_Item *elm_multibuttonentry_item_prepend(Evas_Object *obj, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Append a new item to the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] label The label of new item
 * @param[in] func The callback function to be invoked when this item is pressed.
 * @param[in] data The pointer to the data to be attached
 * @return A handle to the item added or NULL if not possible
 *
 * @see Use elm_object_item_del() to delete the item.
 */
EAPI Elm_Object_Item *elm_multibuttonentry_item_append(Evas_Object *obj, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Add a new item to the multibuttonentry before the indicated object
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] before The item before which to add it
 * @param[in] label The label of new item
 * @param[in] func The callback function to be invoked when this item is pressed.
 * @param[in] data The pointer to the data to be attached
 * @return A handle to the item added or NULL if not possible
 *
 * @see Use elm_object_item_del() to delete the item.
 */
EAPI Elm_Object_Item *elm_multibuttonentry_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Add a new item to the multibuttonentry after the indicated object
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] after The item after which to add it
 * @param[in] label The label of new item
 * @param[in] func The callback function to be invoked when this item is pressed.
 * @param[in] data The pointer to the data to be attached
 * @return A handle to the item added or NULL if not possible
 *
 * @see Use elm_object_item_del() to delete the item.
 */
EAPI Elm_Object_Item *elm_multibuttonentry_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Get a list of items in the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @return The list of items, or NULL if none
 */
EAPI const Eina_List           *elm_multibuttonentry_items_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the base object of the item.
 *
 * @remarks Base object is the @c Evas_Object that represents that item.
 *
 * @param[in] it The multibuttonentry item
 * @return The base object associated with @p item
 */
EAPI Evas_Object *elm_multibuttonentry_item_object_get(const Elm_Object_Item *it);

/**
 * @brief Get the first item in the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @return The first item, or NULL if none
 */
EAPI Elm_Object_Item *elm_multibuttonentry_first_item_get(const Evas_Object *obj);

/**
 * @brief Get the last item in the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @return The last item, or NULL if none
 */
EAPI Elm_Object_Item *elm_multibuttonentry_last_item_get(const Evas_Object *obj);

/**
 * @brief Get the selected item in the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @return The selected item, or NULL if none
 */
EAPI Elm_Object_Item *elm_multibuttonentry_selected_item_get(const Evas_Object *obj);

/**
 * @brief Set the selected state of an item
 *
 * @since_tizen 2.3
 *
 * @param[in] it The item
 * @param[in] selected if it's EINA_TRUE, select the item otherwise, unselect the item
 */
EAPI void                       elm_multibuttonentry_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);


/**
 * @brief Get the selected state of an item
 *
 * @since_tizen 2.3
 *
 * @param[in] it The item
 * @return EINA_TRUE if the item is selected, EINA_FALSE otherwise.
 */
EAPI Eina_Bool elm_multibuttonentry_item_selected_get(const Elm_Object_Item *it);

/**
 * @brief Remove all items in the multibuttonentry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 */
EAPI void                       elm_multibuttonentry_clear(Evas_Object *obj);

/**
 * @brief Get the previous item in the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] it The item
 * @return The item before the item @p it
 */
EAPI Elm_Object_Item *elm_multibuttonentry_item_prev_get(const Elm_Object_Item *it);

/**
 * @brief Get the next item in the multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @param[in] it The item
 * @return The item after the item @p it
 */
EAPI Elm_Object_Item *elm_multibuttonentry_item_next_get(const Elm_Object_Item *it);

/**
 * @brief Append an item filter function for text inserted in the Multibuttonentry
 *
 * @since_tizen 2.3
 *
 * @remarks Append the given callback to the list. This functions will be
 *          called whenever any text is inserted into the Multibuttonentry,
 *          with the text to be inserted as a parameter. The callback function
 *          is free to alter the text in any way it wants, but it must remember
 *          to free the given pointer and update it. If the new text is to be
 *          discarded, the function can free it and set it text parameter
 *          to NULL. This will also prevent any following filters from being
 *          called.
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] func The function to use as item filter
 * @param[in] data User data to pass to @p func
 */
EAPI void                       elm_multibuttonentry_item_filter_append(Evas_Object *obj, Elm_Multibuttonentry_Item_Filter_Cb func, const void *data);

/**
 * @brief Prepend a filter function for text inserted in the Multibuttonentry
 *
 * @details Prepend the given callback to the list.
 *
 * @since_tizen 2.3
 *
 * @see elm_multibuttonentry_item_filter_append()
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] func The function to use as text filter
 * @param[in] data User data to pass to @p func
 */
EAPI void                       elm_multibuttonentry_item_filter_prepend(Evas_Object *obj, Elm_Multibuttonentry_Item_Filter_Cb func, const void *data);

/**
 * @brief Remove a filter from the list
 *
 * @details Removes the given callback from the filter list.
 *
 * @since_tizen 2.3
 *
 * @see elm_multibuttonentry_item_filter_append()
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] func The filter function to remove
 * @param[in] data The user data passed when adding the function
 */
EAPI void                       elm_multibuttonentry_item_filter_remove(Evas_Object *obj, Elm_Multibuttonentry_Item_Filter_Cb func, const void *data);

/**
 * @brief Sets if the multibuttonentry is to be editable or not.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @param[in] editable If EINA_TRUE, user can add/delete item in multibuttonentry, if not, the multibuttonentry is non-editable.
 *
 * @since 1.7
 */
EAPI void elm_multibuttonentry_editable_set(Evas_Object *obj, Eina_Bool editable);

/**
 * @brief Gets whether the multibuttonentry is editable or not.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The multibuttonentry object
 * @return EINA_TRUE if the multibuttonentry is editable by the user. EINA_FALSE if not.
 *
 * @since 1.7
 */
EAPI Eina_Bool elm_multibuttonentry_editable_get(const Evas_Object *obj);

/**
 * @}
 */
