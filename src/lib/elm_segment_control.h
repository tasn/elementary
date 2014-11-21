/**
 * @defgroup SegmentControl SegmentControl
 * @ingroup elm_widget_group
 *
 * @image html segment_control_inheritance_tree.png
 * @image latex segment_control_inheritance_tree.eps
 *
 * @image html img/segment_control.png
 * @image latex img/segment_control.eps "segment control" width=\textwidth
 *
 * @brief Segment control widget is a horizontal control made of multiple
 *        segment items, each segment item functioning similar to a discrete
 *        two state button.
 *
 * A segment control groups the items together and provides a compact
 * single button with multiple equal size segments.
 *
 * Segment item size is determined by base widget
 * size and the number of items added.
 * Only one segment item can be at a selected state. A segment item can display
 * a combination of Text and any Evas_Object like Images or other widgets.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for segment control objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - When the user clicks on a segment item which is not
 *   previously selected and it gets selected. The @a event_info parameter is the
 *   segment item pointer.
 *
 * Available styles for it:
 * - @c "default"
 *
 * The default content parts of the segment control items that you can use are:
 * @li "icon" - An icon in a segment control item.
 *
 * The default text parts of the segment control items that you can use are:
 * @li "default" - Title label in a segment control item.
 *
 * Supported common elm_object APIs.
 * @li elm_object_disabled_set
 * @li elm_object_disabled_get
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
 * @brief Adds a new segment control widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new segment control widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new segment control widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object      *elm_segment_control_add(Evas_Object *parent);

/**
 * @brief Appends a new item to the segment control object.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and appended to the segment control, i.e., it
 *          is set as the @b last item.
 *
 * @remarks If it should be inserted at another position,
 *          elm_segment_control_item_insert_at() should be used instead.
 *
 * @remarks Items created with this function can be deleted with function
 *          elm_object_item_del() or elm_object_item_del_at().
 *
 * @remarks @a label set to @c NULL is different from an empty string "".
 *          If an item only has icon, it is displayed bigger and it is centered. If it has
 *          an icon and a label, even an empty string, the icon is smaller and
 *          positioned towards the left.
 *
 * Simple example:
 * @code
 * sc = elm_segment_control_add(win);
 * ic = elm_icon_add(win);
 * elm_image_file_set(ic, "path/to/image", NULL);
 * elm_icon_resizable_set(ic, EINA_TRUE, EINA_TRUE);
 * elm_segment_control_item_add(sc, ic, "label");
 * evas_object_show(sc);
 * @endcode
 *
 * @param[in] obj The segment control object.
 * @param[in] icon The icon object to use for the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created
 *             with elm_icon_add().
 * @param[in] label The label of the item \n
 *              Note that, @c NULL is different from an empty string "".
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_segment_control_item_insert_at()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item *elm_segment_control_item_add(Evas_Object *obj, Evas_Object *icon, const char *label);

/**
 * @brief Inserts a new item to the segment control object at the specified position.
 *
 * @since_tizen 2.3
 *
 * @remarks Index values must be between @c 0, when the item is prepended to the
 *          segment control, and items count, that can be obtained with
 *          elm_segment_control_item_count_get(), similar to the case when the item is appended
 *          to the segment control, just like elm_segment_control_item_add().
 *
 * @remarks Items created with this function can be deleted with function
 *          elm_object_item_del() or elm_segment_control_item_del_at().
 *
 * @remarks @a label set to @c NULL is different from an empty string "".
 *          If an item only has an icon, it is displayed bigger and it is centered. If it has
 *          an icon and a label, even an empty string, the icon is smaller and
 *          positioned towards the left.
 *
 * @param[in] obj The segment control object
 * @param[in] icon The icon object to use for the left side of the item \n
 *             An icon can be any Evas object, but usually it is an icon created
 *             with elm_icon_add().
 * @param[in] label The label of the item
 * @param[in] index The item position \n
 *              The value should be between @c 0 and the items count.
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_segment_control_item_add()
 * @see elm_segment_control_item_count_get()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item *elm_segment_control_item_insert_at(Evas_Object *obj, Evas_Object *icon, const char *label, int index);

/**
 * @brief Removes a segment control item at the given index from its parent,
 *        deleting it.
 *
 * @since_tizen 2.3
 *
 * @remarks Items can be added with elm_segment_control_item_add() or
 *          elm_segment_control_item_insert_at().
 *
 * @param[in] obj The segment control object
 * @param[in] index The position of the segment control item to be deleted
 */
EAPI void              elm_segment_control_item_del_at(Evas_Object *obj, int index);

/**
 * @brief Gets the segment items count from the segment control.
 *
 * @details It just returns the number of items added to the segment control @a obj.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The segment control object
 * @return The segment items count
 */
EAPI int               elm_segment_control_item_count_get(const Evas_Object *obj);

/**
 * @brief Gets the item placed at the specified index.
 *
 * @since_tizen 2.3
 *
 * @remarks Index is the position of an item in the segment control widget. Its
 *          range is from @c 0 to <tt> count - 1 </tt>.
 *          Count is the number of items, that can be obtained with
 *          elm_segment_control_item_count_get().
 *
 * @param[in] obj The segment control object
 * @param[in] index The index of the segment item
 * @return The segment control item, otherwise @c NULL on failure
 */
EAPI Elm_Object_Item *elm_segment_control_item_get(const Evas_Object *obj, int index);

/**
 * @brief Gets the label of the item.
 *
 * @since_tizen 2.3
 *
 * @remarks The return value is a pointer to the label associated to the item when
 *          it is created, with function elm_segment_control_item_add(), or later
 *          with function elm_object_item_text_set(). If no label
 *          is passed as an argument, it returns @c NULL.
 *
 * @param[in] obj The segment control object
 * @param[in] index The index of the segment item
 * @return The label of the item at @a index
 *
 * @see elm_object_item_text_set()
 * @see elm_segment_control_item_add()
 */
EAPI const char       *elm_segment_control_item_label_get(const Evas_Object *obj, int index);

/**
 * @brief Gets the icon associated to the item.
 *
 * @since_tizen 2.3
 *
 * @remarks The return value is a pointer to the icon associated to the item when
 *          it is created, with function elm_segment_control_item_add(), or later
 *          with function elm_object_item_part_content_set(). If no icon
 *          is passed as an argument, it returns @c NULL.
 *
 * @param[in] obj The segment control object
 * @param[in] index The index of the segment item
 * @return The left side icon associated to the item at @a index
 *
 * @see elm_segment_control_item_add()
 * @see elm_object_item_part_content_set()
 */
EAPI Evas_Object      *elm_segment_control_item_icon_get(const Evas_Object *obj, int index);

/**
 * @brief Gets the index of an item.
 *
 * @since_tizen 2.3
 *
 * @remarks Index is the position of an item in the segment control widget. Its
 *          range is from @c 0 to <tt> count - 1 </tt>.
 *          Count is the number of items, that can be obtained with
 *          elm_segment_control_item_count_get().
 *
 * @param[in] it The segment control item
 * @return The position of the item in the segment control widget
 */
EAPI int               elm_segment_control_item_index_get(const Elm_Object_Item *it);

/**
 * @brief Gets the base object of the item.
 *
 * @since_tizen 2.3
 *
 * @remarks Base object is the @c Evas_Object that represents that item.
 *
 * @param[in] it The segment control item
 * @return The base object associated with @a it
 */
EAPI Evas_Object      *elm_segment_control_item_object_get(const Elm_Object_Item *it);

/**
 * @brief Gets the selected item.
 *
 * @since_tizen 2.3
 *
 * @remarks The selected item can be unselected with the function
 *          elm_segment_control_item_selected_set().
 *
 * @remarks The selected item is always highlighted on the segment control.
 *
 * @param[in] obj The segment control object
 * @return The selected item, otherwise @c NULL if none of the segment items are
 *         selected
 */
EAPI Elm_Object_Item *elm_segment_control_item_selected_get(const Evas_Object *obj);

/**
 * @brief Sets the selected state of an item.
 *
 * @details This sets the selected state of the given item @a it.
 *          @c EINA_TRUE for selected, @c EINA_FALSE for not selected.
 *
 * @since_tizen 2.3
 *
 * @remarks If a new item is selected the previously selected item is unselected.
 *          Selected item can be obtained with the function
 *          elm_segment_control_item_selected_get().
 *
 * @remarks The selected item is always highlighted on the segment control.
 *
 * @param[in] it The segment control item
 * @param[in] select The selected state
 *
 * @see elm_segment_control_item_selected_get()
 */
EAPI void              elm_segment_control_item_selected_set(Elm_Object_Item *it, Eina_Bool select);

/**
 * @}
 */
