/**
 * @internal
 * @defgroup Flipselector Flip Selector
 * @ingroup Elementary
 *
 * @image html flipselector_inheritance_tree.png
 * @image latex flipselector_inheritance_tree.eps
 *
 * @image html img/widget/flipselector/preview-00.png
 * @image latex img/widget/flipselector/preview-00.eps
 *
 * A flip selector is a widget to show a set of @b text items, one
 * at a time, with the same sheet switching style as the @ref Clock
 * "clock" widget, when one changes the current displaying sheet
 * (thus, the word "flip" in the name).
 *
 * User clicks to flip sheets which are @b held for some time which
 * make the flip selector to flip continuously and automatically for
 * the user. The interval between flips keep growing with time,
 * so that it helps the user to reach an item which is distant from
 * the current selection.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for flip selector objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "selected" - When the widget's selected text item is changed.
 * - @c "overflowed" - When the widget's current selection is changed
 *   from the first item in its list to the last.
 * - @c "underflowed" - When the widget's current selection is changed
 *   from the last item in its list to the first.
 *
 * Available styles for it:
 * - @c "default"
 *
 * The default text parts of the flipselector items that you can use are:
 * @li "default" - Label of the flipselector item.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 *
 * Supported common elm_object_item APIs.
 * @li @ref elm_object_item_text_set
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_signal_emit
 *
 * @{
 */

/**
 * @brief Adds a new flip selector widget to the given parent Elementary
 *        (container) widget.
 *
 * @details This function inserts a new flip selector widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return A new flip selector widget handle, otherwise @c NULL in case of an error
 *
 * @ingroup Flipselector
 */
EAPI Evas_Object                *elm_flipselector_add(Evas_Object *parent);

/**
 * @brief Programmatically selects the next item of a flip selector widget.
 *
 * @remarks The selection is animated. Also, if it reaches the
 *          end of its list of member items, it continues with the first
 *          one onwards.
 *
 * @param[in] obj The flipselector object
 *
 * @ingroup Flipselector
 */
EAPI void                        elm_flipselector_flip_next(Evas_Object *obj);

/**
 * @brief Programmatically selects the previous item of a flip selector
 *        widget.
 *
 * @remarks The selection is animated. Also, if it reaches the
 *          beginning of its list of member items, it continues with the
 *          last one backwards.
 *
 * @param[in] obj The flipselector object
 *
 * @ingroup Flipselector
 */
EAPI void                        elm_flipselector_flip_prev(Evas_Object *obj);

/**
 * @brief Appends a (text) item to a flip selector widget.
 *
 * @remarks The widget's list of labels to show are appended with the
 *          given value. If the user wishes so, a callback function pointer
 *          can be passed, which gets called when this same item is
 *          selected.
 *
 * @remarks The current selection @b won't be modified by appending an
 *          element to the list.
 *
 * @remarks The maximum length of the text label is going to be
 *          determined <b>by the widget's theme</b>. Strings larger than
 *          that value are going to be @b truncated.
 *
 * @param[in] obj The flipselector object
 * @param[in] label The (text) label of the new item
 * @param[in] func The convenience callback function to take place when the
 *             item is selected
 * @param[in] data The data passed to @a func mentioned above
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @ingroup Flipselector
 */
EAPI Elm_Object_Item            *elm_flipselector_item_append(Evas_Object *obj, const char *label, Evas_Smart_Cb func, void *data);

/**
 * @brief Prepends a (text) item to a flip selector widget.
 *
 * @remarks The widget's list of labels to show are prepended with the
 *          given value. If the user wishes so, a callback function pointer
 *          can be passed, which gets called when this same item is
 *          selected.
 *
 * @remarks The current selection @b won't be modified by prepending
 *          an element to the list.
 *
 * @remarks The maximum length of the text label is going to be
 *          determined <b>by the widget's theme</b>. Strings larger than
 *          that value are going to be @b truncated.
 *
 * @param[in] obj The flipselector object
 * @param[in] label The (text) label of the new item
 * @param[in] func The convenience callback function to take place when the
 *             item is selected
 * @param[in] data The data passed to @a func mentioned above
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @ingroup Flipselector
 */
EAPI Elm_Object_Item            *elm_flipselector_item_prepend(Evas_Object *obj, const char *label, Evas_Smart_Cb func, void *data);

/**
 * @brief Gets the internal list of items in a given flip selector widget.
 *
 * @remarks This list is @b not to be modified in any way and must not be
 *          freed. Use the list members with functions like
 *          elm_object_item_text_set(),
 *          elm_object_item_text_get(),
 *          elm_object_item_del(),
 *          elm_flipselector_item_selected_get(),
 *          elm_flipselector_item_selected_set().
 *
 * @remarks This list is only valid until @a obj object's internal
 *          items list is changed. It should be fetched again with another
 *          call to this function when changes happen.
 *
 * @param[in] obj The flipselector object
 * @return The list of items (#Elm_Object_Item as data),
 *         otherwise @c NULL on errors
 *
 * @ingroup Flipselector
 */
EAPI const Eina_List            *elm_flipselector_items_get(const Evas_Object *obj);

/**
 * @brief Gets the first item in the given flip selector widget's list of
 *        items.
 *
 * @param[in] obj The flipselector object
 * @return The first item, otherwise @c NULL if it has no items (and on
 *         errors)
 *
 * @see elm_flipselector_item_append()
 * @see elm_flipselector_last_item_get()
 *
 * @ingroup Flipselector
 */
EAPI Elm_Object_Item            *elm_flipselector_first_item_get(const Evas_Object *obj);

/**
 * @brief Gets the last item in the given flip selector widget's list of
 *        items.
 *
 * @param[in] obj The flipselector object
 * @return The last item, otherwise @c NULL if it has no items (and on
 *         errors)
 *
 * @see elm_flipselector_item_prepend()
 * @see elm_flipselector_first_item_get()
 *
 * @ingroup Flipselector
 */
EAPI Elm_Object_Item            *elm_flipselector_last_item_get(const Evas_Object *obj);

/**
 * @brief Gets the currently selected item in a flip selector widget.
 *
 * @param[in] obj The flipselector object
 * @return The selected item, otherwise @c NULL if the widget has no items
 *         (and on errors)
 *
 * @ingroup Flipselector
 */
EAPI Elm_Object_Item            *elm_flipselector_selected_item_get(const Evas_Object *obj);

/**
 * @brief Sets whether a given flip selector widget's item should be the
 *        currently selected one.
 *
 * @details This sets whether @a it is or is not the selected (thus, under
 *          display) one. If @a it is different from the one under display,
 *          the latter is unselected. If the @a it is set to be
 *          unselected, on the other hand, the @b first item in the widget's
 *          internal members list is the new selected one.
 *
 * @param[in] it The flip selector item
 * @param[in] selected If @c EINA_TRUE it is selected, otherwise @c EINA_FALSE to unselect it
 *
 * @see elm_flipselector_item_selected_get()
 *
 * @ingroup Flipselector
 */
EAPI void                        elm_flipselector_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);

/**
 * @brief Gets whether a given flip selector widget's item is the currently
 *        selected one.
 *
 * @param[in] it The flip selector item
 * @return @c EINA_TRUE if it's selected, otherwise @c EINA_FALSE
 *         (or on errors)
 *
 * @see elm_flipselector_item_selected_set()
 *
 * @ingroup Flipselector
 */
EAPI Eina_Bool  elm_flipselector_item_selected_get(const Elm_Object_Item *it);

/**
 * @brief Gets the item before @a it in a flip selector widget's internal list of
 *        items.
 *
 * @param[in] it The item to fetch previous from
 * @return The item before the @a it, in its parent's list \n 
 *         If there is no previous item for @a it or there's an error, @c NULL is returned.
 *
 * @see elm_flipselector_item_next_get()
 *
 * @ingroup Flipselector
 */
EAPI Elm_Object_Item *elm_flipselector_item_prev_get(const Elm_Object_Item *it);

/**
 * @brief Gets the item after @a it in a flip selector widget's
 *        internal list of items.
 *
 * @param[in] it The item to fetch next from
 * @return The item after the @a it, in its parent's list \n
 *         If there is no next item for @a it or there's an error, @c NULL is returned.
 *
 * @see elm_flipselector_item_prev_get()
 *
 * @ingroup Flipselector
 */
EAPI Elm_Object_Item            *elm_flipselector_item_next_get(const Elm_Object_Item *it);

/**
 * @brief Sets the interval on time updates for a user's mouse button hold
 *        on a flip selector widget.
 *
 * @remarks This interval value is @b decreased while the user holds the
 *          mouse pointer either by flipping up or flipping down a given flip
 *          selector.
 *
 * @remarks This helps the user to get to a given item, which is distant from the
 *          current one, in an easier/faster way, as it start to flip quicker and
 *          quicker on mouse button holds.
 *
 * @remarks The calculation for the next flip interval value, starting from
 *          the one set with this call, is the previous interval divided by
 *          @c 1.05, so it decreases a little bit.
 *
 * @remarks The default starting interval value for automatic flips is
 *          @b 0.85 seconds.
 *
 * @param[in] obj The flip selector object
 * @param[in] interval The (first) interval value in seconds
 *
 * @see elm_flipselector_first_interval_get()
 *
 * @ingroup Flipselector
 */
EAPI void                        elm_flipselector_first_interval_set(Evas_Object *obj, double interval);

/**
 * @brief Gets the interval on time updates for a user's mouse button hold
 *        on a flip selector widget.
 *
 * @param[in] obj The flip selector object
 * @return The (first) interval value, in seconds, set on it
 *
 * @see elm_flipselector_first_interval_set()
 *
 * @ingroup Flipselector
 */
EAPI double                      elm_flipselector_first_interval_get(const Evas_Object *obj);

/**
 * @}
 */
