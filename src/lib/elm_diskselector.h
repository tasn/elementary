/**
 * @internal
 * @defgroup Diskselector Diskselector
 * @ingroup Elementary
 *
 * @image html diskselector_inheritance_tree.png
 * @image latex diskselector_inheritance_tree.eps
 *
 * @image html img/widget/diskselector/preview-00.png
 * @image latex img/widget/diskselector/preview-00.eps
 *
 * A diskselector is a kind of list widget. It scrolls horizontally,
 * and can contain labels and icon objects. Three items are displayed
 * with the selected one in the middle.
 *
 * It can act like a circular list with a round mode and labels can be
 * reduced for a defined length for side items.
 *
 * This widget implements the @ref elm-scrollable-interface
 * interface, so that all (non-deprecated) functions for the base @ref
 * Scroller widget also work for diskselectors.
 *
 * Some calls on the diskselector's API are marked as @b deprecated,
 * as they just wrap the scrollable widget's counterpart functions. Use
 * the ones mentioned here for each case of deprecation.
 * Eventually, the deprecated ones are discarded (next
 * major release).
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li @c "selected" - When the item is selected, i.e. scroller stops.
 * @li @c "clicked" - This is called when a user clicks an item (@since 1.8).
 * @li @c "scroll,anim,start" - Scrolling animation has started.
 * @li @c "scroll,anim,stop" - Scrolling animation has stopped.
 * @li @c "scroll,drag,start" - Dragging the diskselector has started.
 * @li @c "scroll,drag,stop" - Dragging the diskselector has stopped.
 * @remarks The "scroll,anim,*" and "scroll,drag,*" signals are only emitted by
 * user intervention.
 *
 * Available styles for it:
 * - @c "default"
 *
 * The default content parts of the diskselector items that you can use are:
 * @li "icon" - An icon in the diskselector item.
 *
 * The default text parts of the diskselector items that you can use are:
 * @li "default" - The label of the diskselector item.
 *
 * Supported common elm_object_item APIs.
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_part_text_get
 * @li @ref elm_object_item_part_content_set
 * @li @ref elm_object_item_part_content_get
 *
 * @{
 */

/**
 * @brief Adds a new diskselector widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new diskselector widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return A new diskselector widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object           *elm_diskselector_add(Evas_Object *parent);

/**
 * @brief Enables or disables the round mode.
 *
 * @remarks This is disabled by default. If the round mode is enabled, the items list
 *          works like a circular list, so when the user reaches the last item,
 *          the first one pops up.
 *
 * @param[in] obj The diskselector object
 * @param[in] enabled If @c EINA_TRUE the round mode is enabled, otherwise @c EINA_FALSE to disable it
 *
 * @see elm_diskselector_round_enabled_get()
 */
EAPI void                   elm_diskselector_round_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Gets a value that indicates whether the round mode is enabled.
 *
 * @param[in] obj The diskselector object
 * @return @c EINA_TRUE if the round mode is enabled, otherwise @c EINA_FALSE if it is disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_diskselector_round_enabled_set()
 */
EAPI Eina_Bool              elm_diskselector_round_enabled_get(const Evas_Object *obj);

/**
 * @brief Gets the side labels maximum length.
 *
 * @param[in] obj The diskselector object
 * @return The maximum length defined for side labels, otherwise @c 0 if it is not a valid diskselector
 *
 * @see elm_diskselector_side_text_max_length_set()
 */
EAPI int                    elm_diskselector_side_text_max_length_get(const Evas_Object *obj);

/**
 * @brief Sets the side labels maximum length.
 *
 * @remarks Length is the number of characters of items' label that are
 *          visible when it's set on side positions. It just crops
 *          the string after the defined size. E.g.:
 *
 *          An item with label "January" would be displayed on the side position as
 *          "Jan" if the maximum length is set to @c 3, or "Janu", if this property
 *          is set to @c 4.
 *
 * @remarks When it's selected, the entire label is displayed, except for the
 *          width restrictions. In this case, the label is cropped and "..."
 *          is concatenated.
 *
 * @remarks The default side label maximum length is 3.
 *
 * @remarks This property is applied over all items, included before or
 * 			after this function call.
 *
 * @param[in] obj The diskselector object
 * @param[in] len The maximum length defined for side labels
 */
EAPI void                   elm_diskselector_side_text_max_length_set(Evas_Object *obj, int len);

/**
 * @brief Sets the number of items to be displayed.
 *
 * @remarks The default value is @c 3, and also it's the minimum value. If @a num is less
 *          than @c 3, it is set to @c 3.
 *
 * @remarks Also, it can be set on the theme, using data item @c display_item_num
 *          on the group "elm/diskselector/item/X", where X is the style set.
 * E.g.:
 *
 * group { name: "elm/diskselector/item/X";
 * data {
 *     item: "display_item_num" "5";
 *     }
 *
 * @param[in] obj The diskselector object
 * @param[in] num The number of items the diskselector displays
 */
EAPI void                   elm_diskselector_display_item_num_set(Evas_Object *obj, int num);

/**
 * @brief Gets the number of items in the diskselector object.
 *
 * @param[in] obj The diskselector object
 */
EAPI int                   elm_diskselector_display_item_num_get(const Evas_Object *obj);

/**
 * @brief Sets the bouncing behaviour when the scrolled content reaches an edge.
 *
 * @details This tells the internal scroller object whether it should bounce or not
 *          when it reaches the respective edges of each axis.
 *
 * @param[in] obj The diskselector object
 * @param[in] h_bounce The boolean value that indicates whether to bounce in the horizontal axis
 * @param[in] v_bounce The boolean value that indicates whether to bounce in the vertical axis
 *
 * @deprecated Use elm_scroller_bounce_set() instead.
 *
 * @see elm_scroller_bounce_set()
 */
EINA_DEPRECATED EAPI void elm_diskselector_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce);

/**
 * @brief Gets the bouncing behaviour of the internal scroller.
 *
 * @details This sets whether the internal scroller should bounce when the edge of each
 *          axis is reached by scrolling.
 *
 * @param[in] obj The diskselector object
 * @param[in] h_bounce The pointer to store the bounce state of the horizontal axis
 * @param[in] v_bounce The pointer to store the bounce state of the vertical axis
 *
 * @deprecated Use elm_scroller_bounce_get() instead.
 *
 * @see elm_scroller_bounce_get()
 * @see elm_diskselector_bounce_set()
 */
EINA_DEPRECATED EAPI void elm_diskselector_bounce_get(const Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce);

/**
 * @brief Gets the scrollbar policy.
 *
 * @param[in] obj The diskselector object
 * @param[out] policy_h The pointer to store the horizontal scrollbar policy
 * @param[out] policy_v The pointer to store the vertical scrollbar policy
 *
 * @deprecated Use elm_scroller_policy_get() instead.
 *
 * @see elm_diskselector_scroller_policy_get()
 * @see elm_scroller_policy_get()
 */
EINA_DEPRECATED EAPI void elm_diskselector_scroller_policy_get(const Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v);

/**
 * @brief Sets the scrollbar policy.
 *
 * @details This sets the scrollbar visibility policy for the given
 *          scroller. #ELM_SCROLLER_POLICY_AUTO means the scrollbar is made visible if
 *          needed, and otherwise kept hidden. #ELM_SCROLLER_POLICY_ON turns
 *          it on at all times, and #ELM_SCROLLER_POLICY_OFF always keeps it off.
 *          This applies for the horizontal and vertical scrollbars respectively.
 *
 * @remarks Both are disabled by default, i.e., they are set to #ELM_SCROLLER_POLICY_OFF.
 *
 * @param[in] obj The diskselector object
 * @param[in] policy_h The horizontal scrollbar policy
 * @param[in] policy_v The vertical scrollbar policy
 *
 * @deprecated Use elm_scroller_policy_set() instead.
 *
 * @see elm_scroller_policy_set()
 */
EINA_DEPRECATED EAPI void elm_diskselector_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v);

/**
 * @brief Removes all the diskselector items.
 *
 * @param[in] obj The diskselector object
 *
 * @see elm_object_item_del()
 * @see elm_diskselector_item_append()
 */
EAPI void                   elm_diskselector_clear(Evas_Object *obj);

/**
 * @brief Gets a list of all the diskselector items.
 *
 * @param[in] obj The diskselector object
 * @return An @c Eina_List of diskselector items, #Elm_Object_Item, otherwise @c NULL on failure
 *
 * @see elm_diskselector_item_append()
 * @see elm_object_item_del()
 * @see elm_diskselector_clear()
 */
EAPI const Eina_List       *elm_diskselector_items_get(const Evas_Object *obj);

/**
 * @brief Appends a new item to the diskselector object.
 *
 * @remarks A new item is created and appended to the diskselector, i.e., it is
 *          set as the last item. Also, if there is no selected item, it is
 *          selected. This always happens for the first appended item.
 *
 * @remarks If no icon is set, the label is centered at the item position, otherwise
 *          the icon is placed on the left of the label, that is shifted
 *          to the right.
 *
 * @remarks Items created with this method can be deleted with
 *          elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when the item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user stops the diskselector with this
 *          item at the center position. If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * Simple example (with no function callback or data is associated):
 * @code
 * disk = elm_diskselector_add(win);
 * ic = elm_icon_add(win);
 * elm_image_file_set(ic, "path/to/image", NULL);
 * elm_icon_resizable_set(ic, EINA_TRUE, EINA_TRUE);
 * elm_diskselector_item_append(disk, "label", ic, NULL, NULL);
 * @endcode
 *
 * @param[in] obj The diskselector object
 * @param[in] label The label of the diskselector item
 * @param[in] icon The icon object to use on the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created with elm_icon_add().
 * @param[in] func The function to call when the item is selected
 * @param[in] data The data to associate with the item for related callbacks
 *
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_object_item_del()
 * @see elm_diskselector_clear()
 * @see elm_icon_add()
 */
EAPI Elm_Object_Item *elm_diskselector_item_append(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Smart_Cb func, const void *data);

/**
 * @brief Gets the selected item.
 *
 * @remarks The selected item can be unselected with function
 *          elm_diskselector_item_selected_set(), and the first item of the
 *          diskselector is selected.
 *
 * @remarks The selected item is always centered on the diskselector, with the
 *          entire label displayed, i.e., maximum length set to side labels won't
 *          apply on the selected item. For more details, see
 *          elm_diskselector_side_text_max_length_set().
 *
 * @param[in] obj The diskselector object
 * @return The selected diskselector item
 */
EAPI Elm_Object_Item *elm_diskselector_selected_item_get(const Evas_Object *obj);

/**
 * @brief Sets the selected state of an item.
 *
 * @details This sets the selected state of the given item @a it.
 *          @c EINA_TRUE for selected, @c EINA_FALSE for not selected.
 *
 * @remarks If a new item is selected the previously selected is unselected.
 *          Previously selected item can be obtained with the function
 *          elm_diskselector_selected_item_get().
 *
 * @remarks If the item @a it is unselected, the first item of diskselector is selected.
 *
 * @remarks Selected items are visible at the center position of the diskselector.
 *          So if it is at another position before getting selected, or is invisible,
 *          the diskselector animates the items until the selected item reaches the center
 *          position.
 *
 * @param[in] it The diskselector item
 * @param[in] selected The selected state
 *
 * @see elm_diskselector_item_selected_get()
 * @see elm_diskselector_selected_item_get()
 */
EAPI void                   elm_diskselector_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);

/**
 * @brief Gets whether the @a item is selected.
 *
 * @param[in] it The diskselector item
 * @return @c EINA_TRUE indicates that the item is selected, otherwise @c EINA_FALSE if it is not \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_diskselector_selected_item_set()
 * @see elm_diskselector_item_selected_get()
 */
EAPI Eina_Bool              elm_diskselector_item_selected_get(const Elm_Object_Item *it);

/**
 * @brief Gets the first item of the diskselector.
 *
 * @remarks The list of items follows an append order. So it returns the first
 *          item appended to the widget that isn't deleted.
 *
 * @param[in] obj The diskselector object
 * @return The first item, otherwise @c NULL if none are present
 *
 * @see elm_diskselector_item_append()
 * @see elm_diskselector_items_get()
 */
EAPI Elm_Object_Item *elm_diskselector_first_item_get(const Evas_Object *obj);

/**
 * @brief Gets the last item of the diskselector.
 *
 * @remarks The list of items follows an append order. So it returns the last
 *          item appended to the widget that isn't deleted.
 *
 * @param[in] obj The diskselector object
 * @return The last item, otherwise @c NULL if none are present
 *
 * @see elm_diskselector_item_append()
 * @see elm_diskselector_items_get()
 */
EAPI Elm_Object_Item *elm_diskselector_last_item_get(const Evas_Object *obj);

/**
 * @brief Gets the item before @a item in the diskselector.
 *
 * @remarks The list of items follows an append order. So it returns the item appended
 *          just before @a item which isn't deleted.
 *
 * @remarks If it is the first item, @c NULL is returned.
 *          First item can be obtained by elm_diskselector_first_item_get().
 *
 * @param[in] it The diskselector item
 * @return The item before @a item, otherwise @c NULL if none are present or on failure
 *
 * @see elm_diskselector_item_append()
 * @see elm_diskselector_items_get()
 */
EAPI Elm_Object_Item *elm_diskselector_item_prev_get(const Elm_Object_Item *it);

/**
 * @brief Gets the item after @a item in the diskselector.
 *
 * @remarks The list of items follows an append order. So it returns the item appended
 *          just after @a item which isn't deleted.
 *
 * @remarks If it is the last item, @c NULL is returned.
 *          Last item can be obtained by elm_diskselector_last_item_get().
 *
 * @param[in] it The diskselector item
 * @return The item after @a item, otherwise @c NULL if none are present or on failure
 *
 * @see elm_diskselector_item_append()
 * @see elm_diskselector_items_get()
 */
EAPI Elm_Object_Item *elm_diskselector_item_next_get(const Elm_Object_Item *it);

/**
 * @}
 */
