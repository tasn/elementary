/**
 * @defgroup List List
 * @ingroup elm_widget_group
 *
 * @image html list_inheritance_tree.png
 * @image latex list_inheritance_tree.eps
 *
 * @image html img/list.png
 * @image latex img/list.eps "list" width=\textwidth
 *
 * @brief A list widget is a container whose children are displayed
 *        vertically or horizontally, in order, and can be selected.
 *
 * The list can accept only one or multiple item selections. Also it has many
 * modes of items displaying.
 *
 * A list is a very simple type of list widget. For more robust lists,
 * @ref Genlist should be used.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for list objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "activated" - The user has double-clicked or pressed
 *   (enter|return|spacebar) an item. The @a event_info parameter
 *   is the item that is activated.
 * - @c "clicked,double" - The user has double-clicked an item.
 *   The @a event_info parameter is the item that is double-clicked.
 * - @c "selected" - When the user selected an item.
 * - @c "unselected" - When the user unselected an item.
 * - @c "longpressed" - An item in the list is long-pressed.
 * - @c "edge,top" - The list is scrolled till the top edge.
 * - @c "edge,bottom" - The list is scrolled till the bottom edge.
 * - @c "edge,left" - The list is scrolled till the left edge.
 * - @c "edge,right" - The list is scrolled till the right edge.
 * - @c "language,changed" - The program's language is changed.
 * - @c "highlighted" - An item in the list is pressed and highlighted.
 *   The @a event_info parameter is the item that is highlighted.
 * - @c "unhighlighted" - An item in the list is unpressed and unhighlighted.
 *   The @a event_info parameter is the item that is unhighlighted.
 *
 *
 *
 * Available styles for it are:
 * - @c "default"
 *
 * The default content parts of the list items that you can use are:
 * @li @c "start" - A start position object in the list item.
 * @li @c "end" - An end position object in the list item.
 *
 * The default text parts of the list items that you can use are:
 * @li @c "default" - The label in the list item.
 *
 * Supported common @c elm_object_item APIs.
 * @li @ref elm_object_item_disabled_set
 * @li @ref elm_object_item_disabled_get
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_part_text_get
 * @li @ref elm_object_item_part_content_set
 * @li @ref elm_object_item_part_content_get
 * @li @ref elm_object_item_part_content_unset
 *
 * This widget implements the elm-scrollable-interface
 * interface, so that all (non-deprecated) functions for the base @ref
 * Scroller widget also work for lists.
 *
 * Some calls on the list's API are marked as @b deprecated, as they
 * just wrap the scrollable widgets counterpart functions. Use the
 * ones mentioned for each case of deprecation here.
 * Eventually, the deprecated ones are discarded (next major
 * release).
 *
 * @{
 */

/**
 * @enum Elm_List_Mode
 * @typedef Elm_List_Mode
 *
 * @brief Enumeration that sets the list's resizing behavior, transverse axis scrolling and items
 *        cropping. See each mode's description for more details.
 *
 * @remarks The default value is #ELM_LIST_SCROLL.
 *
 * @remarks The values here @b don't work as bitmasks, only one can be chosen at
 *          a time.
 *
 * @see elm_list_mode_set()
 * @see elm_list_mode_get()
 */
typedef enum
{
   ELM_LIST_COMPRESS = 0, /**< The list @b won't set any of its size hints to inform how a possible container should resize it. If it's not created as a "resize object", it might end with zeroed dimensions. The list respects the container's geometry and if any of its items don't fit into its @b transverse axis, one won't be able to scroll it in that direction */
   ELM_LIST_SCROLL, /**< The default value. This is the same as #ELM_LIST_COMPRESS, with the exception that if any of its items don't fit into its transverse axis, one @b won't be able to scroll it in that direction */
   ELM_LIST_LIMIT, /**< Sets a minimum size hint on the list object, so that containers may respect it (and resize itself to fit the child properly). More specifically, a @b minimum size hint is set for its @b transverse axis, so that the @b largest item in that direction fits well. This is naturally bound by the list object's maximum size hints, set externally */
   ELM_LIST_EXPAND, /**< Besides setting a minimum size on the transverse axis, just like on #ELM_LIST_LIMIT, the list sets a minimum size on the @b longitudinal axis, trying to reserve space for all its children to be visible at a time. This is naturally bound by the list object's maximum size hints, set externally */
   ELM_LIST_LAST /**< Indicates an error if returned by elm_list_mode_get() */
} Elm_List_Mode;

/**
 * @brief Adds a new list widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new list widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new list widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object                 *elm_list_add(Evas_Object *parent);

/**
 * @brief Starts the list.
 *
 * @since_tizen 2.3
 *
 * @remarks Call before running show() on the list object.
 * @remarks If not called, it won't display the list properly.
 *
 * @code
 * li = elm_list_add(win);
 * elm_list_item_append(li, "First", NULL, NULL, NULL, NULL);
 * elm_list_item_append(li, "Second", NULL, NULL, NULL, NULL);
 * elm_list_go(li);
 * evas_object_show(li);
 * @endcode
 *
 * @param[in] obj The list object
 */
EAPI void                         elm_list_go(Evas_Object *obj);

/**
 * @brief Enables or disables multiple items selection on the list object.
 *
 * @since_tizen 2.3
 *
 * @remarks It is disabled by default. If disabled, the user can select a single item from
 *          the list each time. Selected items are highlighted on the list.
 *          If enabled, many items can be selected.
 *
 * @remarks If a selected item is selected again, it is unselected.
 *
 * @param[in] obj The list object
 * @param[in] multi If @c EINA_TRUE multi selection is enabled, otherwise @c EINA_FALSE to
 *              disable it
 *
 * @see elm_list_multi_select_get()
 */
EAPI void                         elm_list_multi_select_set(Evas_Object *obj, Eina_Bool multi);

/**
 * @brief Gets a value that indicates whether multiple items selection is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @return @c EINA_TRUE indicates that multiple items selection is enabled,
 *         otherwise @c EINA_FALSE indicates it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_list_multi_select_set()
 */
EAPI Eina_Bool                    elm_list_multi_select_get(const Evas_Object *obj);

/**
 * @brief Sets which mode to use for the list object.
 *
 * @details This sets the list's resize behavior, transverse axis scroll and
 *          items cropping. See each mode's description for more details.
 *
 * @since_tizen 2.3
 *
 * @remarks This default value is #ELM_LIST_SCROLL.
 *
 * @remarks Only one mode at a time can be set. If a previous one is set, it
 *          is changed by the new mode after this call. Bitmasks won't
 *          work here.
 *
 * @remarks This function's behavior clashes with those of
 *          elm_scroller_content_min_limit(), so use either one of them, but
 *          not both.
 *
 * @param[in] obj The list object
 * @param[in] mode One of #Elm_List_Mode: #ELM_LIST_COMPRESS,
 *             #ELM_LIST_SCROLL, #ELM_LIST_LIMIT, or #ELM_LIST_EXPAND.
 *
 * @see elm_list_mode_get()
 */
EAPI void                         elm_list_mode_set(Evas_Object *obj, Elm_List_Mode mode);

/**
 * @brief Gets the mode that the list is at.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @return One of #Elm_List_Mode: #ELM_LIST_COMPRESS,
 *         #ELM_LIST_SCROLL, #ELM_LIST_LIMIT, #ELM_LIST_EXPAND, or #ELM_LIST_LAST on errors
 *
 * @see elm_list_mode_set()
 */
EAPI Elm_List_Mode                elm_list_mode_get(const Evas_Object *obj);

/**
 * @brief Enables or disables horizontal mode on the list object.
 *
 * @since_tizen 2.3
 *
 * @remarks The vertical mode is set by default.
 *
 * @remarks Use this when the horizontal mode items are displayed on the list from left to right,
 *          instead of from top to bottom. Also, the list scrolls horizontally.
 *          Each item presents the left icon on top and the right icon, or end, at
 *          the bottom.
 *
 * @param[in] obj The list object
 * @param[in] horizontal If @c EINA_TRUE horizontal is enabled, otherwise @c EINA_FALSE to
 *                   disable it, i.e., to enable the vertical mode
 *
 * @see elm_list_horizontal_get()
 */
EAPI void                         elm_list_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Gets a value that indicates whether the horizontal mode is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @return @c EINA_TRUE means horizontal mode selection is enabled,
 *         otherwise @c EINA_FALSE indicates it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_list_horizontal_set()
 */
EAPI Eina_Bool                    elm_list_horizontal_get(const Evas_Object *obj);

/**
 * @brief Sets the list select mode.
 *
 * @since_tizen 2.3
 *
 * @remarks elm_list_select_mode_set() changes the item select mode in the list widget.
 *          - ELM_OBJECT_SELECT_MODE_DEFAULT : Items only call their selection @a func and
 *                                             callback when first becoming selected. Any further clicks
 *                                             do nothing, unless you set the always select mode.
 *          - ELM_OBJECT_SELECT_MODE_ALWAYS : This means that, even if selected,
 *                                            every click calls the selected callbacks.
 *          - ELM_OBJECT_SELECT_MODE_NONE : This turns off the ability to select items
 *                                          entirely and they neither appear selected nor call selected
 *                                          callback functions.
 *
 * @param[in] obj The list object
 * @param[in] mode The select mode
 *
 * @see elm_list_select_mode_get()
 */
EAPI void
elm_list_select_mode_set(Evas_Object *obj, Elm_Object_Select_Mode mode);

/**
 * @brief Gets the list select mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @return The select mode
 *         (If the getting mode is fails, it returns @c ELM_OBJECT_SELECT_MODE_MAX)
 *
 * @see elm_list_select_mode_set()
 */
EAPI Elm_Object_Select_Mode
elm_list_select_mode_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets the bouncing behaviour when the scrolled content reaches an edge.
 *
 * @remarks It tells the internal scroller object whether it should bounce or not
 *          when it reaches the respective edges for each axis.
 *
 * @param obj The list object
 * @param h_bounce The boolean value that indicates whether to bounce in the horizontal axis
 * @param v_bounce The boolean value that indicates whether to bounce in the vertical axis
 *
 * @deprecated Use elm_scroller_bounce_set() instead.
 *
 * @see elm_scroller_bounce_set()
 */
EINA_DEPRECATED EAPI void         elm_list_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce);

/**
 * @internal
 * @brief Gets the bouncing behaviour of the internal scroller.
 *
 * @details This gets whether the internal scroller should bounce when the edge of each
 *          axis has reached scrolling.
 *
 * @param obj The list object
 * @param h_bounce The pointer to store the bounce state of the horizontal
 *                 axis
 * @param v_bounce The pointer to store the bounce state of the vertical
 *                 axis
 *
 * @deprecated Use elm_scroller_bounce_get() instead.
 *
 * @see elm_scroller_bounce_get()
 * @see elm_list_bounce_set()
 */
EINA_DEPRECATED EAPI void         elm_list_bounce_get(const Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce);

/**
 * @internal
 * @brief Sets the scrollbar policy.
 *
 * @details This sets the scrollbar visibility policy for the given
 *          scroller. #ELM_SCROLLER_POLICY_AUTO means the scrollbar is made
 *          visible if it is needed, and otherwise kept
 *          hidden. #ELM_SCROLLER_POLICY_ON turns it on all the time, and
 *          #ELM_SCROLLER_POLICY_OFF always keeps it off. This applies
 *          respectively for the horizontal and vertical scrollbars.
 *
 * @remarks Both are disabled by default, i.e., they are set to
 *          #ELM_SCROLLER_POLICY_OFF.
 *
 * @param obj The list object
 * @param policy_h The horizontal scrollbar policy
 * @param policy_v The vertical scrollbar policy
 *
 * @deprecated Use elm_scroller_policy_set() instead.
 */
EINA_DEPRECATED EAPI void         elm_list_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v);

/**
 * @internal
 * @brief Gets the scrollbar policy.
 *
 * @param obj The list object
 * @param policy_h The pointer to store the horizontal scrollbar policy
 * @param policy_v The pointer to store the vertical scrollbar policy
 *
 * @deprecated Use elm_scroller_policy_get() instead.
 *
 * @see elm_list_scroller_policy_get()
 */
EINA_DEPRECATED EAPI void         elm_list_scroller_policy_get(const Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v);

/**
 * @brief Appends a new item to the list object.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and appended to the list, i.e., it is
 *          set as the @b last item.
 *
 * @remarks Items created with this method can be deleted with elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when the item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If always select is enabled it calls this function every time the
 *          user clicks over an item (already selected or not).
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * Simple example (with no function callback or data associated):
 * @code
 * li = elm_list_add(win);
 * ic = elm_icon_add(win);
 * elm_image_file_set(ic, "path/to/image", NULL);
 * elm_icon_resizable_set(ic, EINA_TRUE, EINA_TRUE);
 * elm_list_item_append(li, "label", ic, NULL, NULL, NULL);
 * elm_list_go(li);
 * evas_object_show(li);
 * @endcode
 *
 * @param[in] obj The list object
 * @param[in] label The label of the list item
 * @param[in] icon The icon object to use for the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created
 *             with elm_icon_add().
 * @param[in] end The icon object to use for the right side of the item \n
 *            An icon can be any Evas object.
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 *
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_list_select_mode_set()
 * @see elm_object_item_del()
 * @see elm_object_item_del_cb_set()
 * @see elm_list_clear()
 * @see elm_icon_add()
 */
EAPI Elm_Object_Item               *elm_list_item_append(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, const void *data);

/**
 * @brief Prepends a new item to the list object.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and prepended to the list, i.e., it
 *          is set as the @b first item.
 *
 * @remarks Items created with this method can be deleted with elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when the item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If always select is enabled it calls this function every time the
 *          user clicks over an item (already selected or not).
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @param[in] obj The list object
 * @param[in] label The label of the list item
 * @param[in] icon The icon object to use for the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created
 *             with elm_icon_add().
 * @param[in] end The icon object to use for the right side of the item \n
 *            An icon can be any Evas object.
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 *
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_list_item_append() for a simple code example.
 * @see elm_list_select_mode_set()
 * @see elm_object_item_del()
 * @see elm_object_item_del_cb_set()
 * @see elm_list_clear()
 * @see elm_icon_add()
 */
EAPI Elm_Object_Item               *elm_list_item_prepend(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the list object before item @a before.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and added to the list. Its position in
 *          this list is just before item @a before.
 *
 * @remarks Items created with this method can be deleted with elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when the item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If always select is enabled it calls this function every time the
 *          user clicks over an item (already selected or not).
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @param[in] obj The list object
 * @param[in] before The list item to insert before
 * @param[in] label The label of the list item
 * @param[in] icon The icon object to use for the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created
 *             with elm_icon_add().
 * @param[in] end The icon object to use for the right side of the item \n
 *            An icon can be any Evas object.
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 *
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_list_item_append() for a simple code example.
 * @see elm_list_select_mode_set()
 * @see elm_object_item_del()
 * @see elm_object_item_del_cb_set()
 * @see elm_list_clear()
 * @see elm_icon_add()
 */
EAPI Elm_Object_Item               *elm_list_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, const char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the list object after item @a after.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and added to the list. Its position in
 *          this list is just after item @a after.
 *
 * @remarks Items created with this method can be deleted with elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when the item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If always select is enabled it calls this function every time the
 *          user clicks over an item (already selected or not).
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @param[in] obj The list object
 * @param[in] after The list item to insert after
 * @param[in] label The label of the list item
 * @param[in] icon The icon object to use for the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created
 *             with elm_icon_add().
 * @param[in] end The icon object to use for the right side of the item \n
 *            An icon can be any Evas object.
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 *
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_list_item_append() for a simple code example.
 * @see elm_list_select_mode_set()
 * @see elm_object_item_del()
 * @see elm_object_item_del_cb_set()
 * @see elm_list_clear()
 * @see elm_icon_add()
 */
EAPI Elm_Object_Item               *elm_list_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, const char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the sorted list object.
 *
 * @details This function inserts values into a list object assuming it is
 *          sorted and the result is going to be sorted.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and added to the list. Its position in
 *          this list is found by comparing the new item with previously inserted
 *          items using function @a cmp_func.
 *
 * @remarks Items created with this method can be deleted with elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when the item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If always select is enabled it calls this function every time the
 *          user clicks over an item (already selected or not).
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @param[in] obj The list object
 * @param[in] label The label of the list item
 * @param[in] icon The icon object to use for the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created
 *             with elm_icon_add().
 * @param[in] end The icon object to use for the right side of the item \n
 *            An icon can be any Evas object.
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 * @param[in] cmp_func The comparing function to be used to sort list
 *                 items <b>by #Elm_Object_Item item handles</b> \n
 *                 This function receives two items and compares them, returning a non-negative integer
 *                 if the second item should be placed after the first, or a negative value
 *                 if it should be placed before.
 *
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_list_item_append() for a simple code example.
 * @see elm_list_select_mode_set()
 * @see elm_object_item_del()
 * @see elm_object_item_del_cb_set()
 * @see elm_list_clear()
 * @see elm_icon_add()
 */
EAPI Elm_Object_Item               *elm_list_item_sorted_insert(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, const void *data, Eina_Compare_Cb cmp_func);

/**
 * @brief Removes all list items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 *
 * @see elm_object_item_del()
 * @see elm_list_item_append()
 */
EAPI void                         elm_list_clear(Evas_Object *obj);

/**
 * @brief Gets a list of all the list items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @return An @c Eina_List of the list items, #Elm_Object_Item,
 *         otherwise @c NULL on failure
 *
 * @see elm_list_item_append()
 * @see elm_object_item_del()
 * @see elm_list_clear()
 */
EAPI const Eina_List             *elm_list_items_get(const Evas_Object *obj);

/**
 * @brief Gets the selected item.
 *
 * @since_tizen 2.3
 *
 * @remarks The selected item can be unselected with function
 *          elm_list_item_selected_set().
 *
 * @remarks The selected item is always highlighted on the list.
 *
 * @param[in] obj The list object
 * @return The selected list item
 *
 * @see elm_list_selected_items_get()
 */
EAPI Elm_Object_Item               *elm_list_selected_item_get(const Evas_Object *obj);

/**
 * @brief Returns a list of the currently selected list items.
 *
 * @since_tizen 2.3
 *
 * @remarks Multiple items can be selected if multi select is enabled. It can be
 *          done with elm_list_multi_select_set().
 *
 * @param[in] obj The list object
 * @return An @c Eina_List of list items, #Elm_Object_Item,
 *         otherwise @c NULL on failure
 *
 * @see elm_list_selected_item_get()
 * @see elm_list_multi_select_set()
 */
EAPI const Eina_List             *elm_list_selected_items_get(const Evas_Object *obj);

/**
 * @brief Sets the selected state of an item.
 *
 * @since_tizen 2.3
 *
 * @remarks This sets the selected state of the given item @a it.
 *          @c EINA_TRUE for selected, @c EINA_FALSE for not selected.
 *
 * @remarks If a new item is selected the previously selected is unselected,
 *          unless multiple selection is enabled with elm_list_multi_select_set().
 *          Previously selected item can be obtained with function
 *          elm_list_selected_item_get().
 *
 * @remarks Selected items are highlighted.
 *
 * @param[in] it The list item
 * @param[in] selected The selected state
 *
 * @see elm_list_item_selected_get()
 * @see elm_list_selected_item_get()
 * @see elm_list_multi_select_set()
 */
EAPI void                         elm_list_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);

/**
 * @brief Gets whether the @a it is selected.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The list item
 * @return @c EINA_TRUE indicates that the item is selected, otherwise @c EINA_FALSE indicates it's not \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_list_selected_item_set()
 * @see elm_list_item_selected_get()
 */
EAPI Eina_Bool                    elm_list_item_selected_get(const Elm_Object_Item *it);

/**
 * @brief Sets or unsets an item as a separator.
 *
 * @since_tizen 2.3
 *
 * @remarks Items aren't set as a separator by default.
 *
 * @remarks If set as a separator it displays the separator theme, so it won't display
 *          icons or labels.
 *
 * @param[in] it The list item
 * @param[in] setting If @c EINA_TRUE it sets the item @a it as a separator,
 *                otherwise @c EINA_FALSE to unset it, i.e., item is used as a regular item
 *
 * @see elm_list_item_separator_get()
 */
EAPI void                         elm_list_item_separator_set(Elm_Object_Item *it, Eina_Bool setting);

/**
 * @brief Gets a value that indicates whether an item is a separator.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The list item
 * @return @c EINA_TRUE indicates that the item @a it is a separator, otherwise @c EINA_FALSE indicates it's not \n
 *         If @a it is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_list_item_separator_set()
 */
EAPI Eina_Bool                    elm_list_item_separator_get(const Elm_Object_Item *it);

/**
 * @brief Shows @a it in the list view.
 *
 * @since_tizen 2.3
 *
 * @remarks It won't animate the list until an item is visible. If such a behavior is wanted,
 *          use elm_list_bring_in() instead.
 *
 * @param[in] it The list item to be shown
 */
EAPI void                         elm_list_item_show(Elm_Object_Item *it);

/**
 * @brief Brings in the given item to the list view.
 *
 * @since_tizen 2.3
 *
 * @remarks This causes the list to jump to the given item @a it and show it
 *          (by scrolling), if it is not fully visible.
 *
 * @remarks This may use animation to do so and may take a period of time.
 *
 * @remarks If animation isn't wanted, elm_list_item_show() can be used.
 *
 * @param[in] it The item
 */
EAPI void                         elm_list_item_bring_in(Elm_Object_Item *it);

/**
 * @brief Gets the base object of the item.
 *
 * @since_tizen 2.3
 *
 * @remarks The base object is the @c Evas_Object that represents that item.
 *
 * @param[in] it The list item
 * @return The base object associated with @a it
 */
EAPI Evas_Object                 *elm_list_item_object_get(const Elm_Object_Item *it);

/**
 * @brief Gets the item before @a it in list.
 *
 * @since_tizen 2.3
 *
 * @remarks If it is the first item, @c NULL is returned.
 *
 * @param it The list item
 * @return The item before @a it, otherwise @c NULL if none are present or on failure
 *
 * @see elm_list_item_append()
 * @see elm_list_items_get()
 */
EAPI Elm_Object_Item               *elm_list_item_prev(const Elm_Object_Item *it);

/**
 * @brief Gets the item after @a it in list.
 *
 * @since_tizen 2.3
 *
 * @remarks If it is the last item, @c NULL is returned.
 *
 * @param[in] it The list item
 * @return The item after @a it, otherwise @c NULL if none are present or on failure
 *
 * @see elm_list_item_append()
 * @see elm_list_items_get()
 */
EAPI Elm_Object_Item               *elm_list_item_next(const Elm_Object_Item *it);

/**
 * @brief Gets the first item in the list.
 *
 * @details This returns the first item in the list.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @return The first item, otherwise @c NULL if none are present
 */
EAPI Elm_Object_Item             *elm_list_first_item_get(const Evas_Object *obj);

/**
 * @brief Gets the last item in the list.
 *
 * @details This returns the last item in the list.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @return The last item, otherwise @c NULL if none are present
 */
EAPI Elm_Object_Item             *elm_list_last_item_get(const Evas_Object *obj);

/**
 * @brief Gets the item that is at the x, y canvas coordinates.
 *
 * @details This returns the item at the given coordinates (which are canvas
 *          relative, not object-relative). If an item is at that coordinate,
 *          that item handle is returned, and if @a posret is not @c NULL, the
 *          integer pointed to is set to a value of @c -1, @c 0 or @c 1, depending on whether
 *          the coordinate is at the upper portion of that item (-1), in the
 *          middle section (0) or at the lower part (1). If @c NULL is returned as
 *          an item (no item found there), then @a posret may indicate @c -1 or @c 1
 *          based on whether the coordinate is above or below all items in
 *          the list respectively.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The list object
 * @param[in] x The input x coordinate
 * @param[in] y The input y coordinate
 * @param[out] posret The position relative to the returned item
 * @return The item at the coordinates, otherwise @c NULL if no item is present
 */
EAPI Elm_Object_Item             *elm_list_at_xy_item_get(const Evas_Object *obj, Evas_Coord x, Evas_Coord y, int *posret);

/**
 * @}
 */
