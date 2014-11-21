/**
 * @defgroup Index Index
 * @ingroup elm_widget_group
 *
 * @image html index_inheritance_tree.png
 * @image latex index_inheritance_tree.eps
 *
 * @brief An index widget gives you an index for fast access to whichever
 *        group of other UI items one might have.
 *
 * It's a list of text items (usually letters, for alphabetically ordered
 * access).
 *
 * Index widgets are by default hidden and just appear when the
 * user clicks over it's reserved area in the canvas. In its
 * default theme, it's an area of one @ref Fingers "finger" wide on
 * the right side of the index widget's container.
 *
 * When items on the index are selected, smart callbacks get
 * called, so that its user can make other container objects to
 * show a given area or child object depending on the index item
 * selected. You would probably be using an index together with @ref
 * List "lists", @ref Genlist "generic lists" or @ref Gengrid
 * "general grids".
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for index objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - When the selected index item changes.
 *      @c event_info is the selected item's data pointer.
 * - @c "delay,changed" - When the selected index item changes, but
 *      after a small idling period. @a event_info is the selected
 *      item's data pointer.
 * - @c "selected" - When the user releases a mouse button and
 *      selects an item. @a event_info is the selected item's data
 *      pointer.
 * - @c "level,up" - When the user moves a finger from the first
 *      level to the second level.
 * - @c "level,down" - When the user moves a finger from the second
 *      level to the first level.
 * - @c "language,changed" - The program's language is changed.
 *
 * The @c "delay,changed" event is so that it waits for a small period of time
 * before actually reporting those events and, moreover, just the
 * last event happening on those time frames is actually
 * reported.
 *
 * @{
 */

/**
 * @brief Adds a new index widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new index widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new index widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object          *elm_index_add(Evas_Object *parent);

/**
 * @brief Enables or disables the auto hiding feature for a given index widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @param[in] disabled If @c EINA_TRUE auto hiding is disabled, otherwise @c EINA_FALSE to enable it
 *
 * @see elm_index_autohide_disabled_get()
 */
EAPI void                  elm_index_autohide_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Gets whether the auto hiding feature is enabled for a given index widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @return @c EINA_TRUE if auto hiding is disabled, otherwise @c EINA_FALSE
 *
 * @see elm_index_autohide_disabled_set()
 */
EAPI Eina_Bool             elm_index_autohide_disabled_get(const Evas_Object *obj);

/**
 * @brief Sets the items level for a given index widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @param[in] level @c 0 or @c 1, the currently implemented levels
 *
 * @see elm_index_item_level_get()
 */
EAPI void                  elm_index_item_level_set(Evas_Object *obj, int level);

/**
 * @brief Gets the items level set for a given index widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @return @c 0 or @c 1, which are the levels that @a obj might be at
 *
 * @see elm_index_item_level_set()
 */
EAPI int                   elm_index_item_level_get(const Evas_Object *obj);

/**
 * @brief Sets the selected state of an item.
 *
 * @details This sets the selected state of the given item @a it.
 *          @c EINA_TRUE for selected, @c EINA_FALSE for not selected.
 *
 * @since_tizen 2.3
 *
 * @remarks If a new item is selected the previously selected is unselected.
 *          Previously selected item can be obtained with function
 *          elm_index_selected_item_get().
 *
 * @remarks Selected items are highlighted.
 *
 * @param[in] it The index item
 * @param[in] selected The selected state
 *
 * @see elm_index_selected_item_get()
 */
EAPI void                  elm_index_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);

/**
 * @brief Gets the last selected item, for a given index widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @param[in] level @c 0 or @c 1, the currently implemented levels
 * @return The last item @b selected on @a obj (or @c NULL on errors)
 */
EAPI Elm_Object_Item      *elm_index_selected_item_get(const Evas_Object *obj, int level);

/**
 * @brief Appends a new item on a given index widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Despite the fact that the most common usage of the @a letter argument is for
 *          single char strings, one could use arbitrary strings as index
 *          entries.
 *
 * @remarks @a it is the pointer returned back on the @c "changed", @c
 *          "delay,changed", and @c "selected" smart events.
 *
 * @param[in] obj The index object
 * @param[in] letter The letter under which the item should be indexed
 * @param[in] func The function to call when the item is selected
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case of an error
 */
EAPI Elm_Object_Item      *elm_index_item_append(Evas_Object *obj, const char *letter, Evas_Smart_Cb func, const void *data);

/**
 * @brief Prepends a new item on a given index widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Despite the fact that the most common usage of the @a letter argument is for
 *          single char strings, one could use arbitrary strings as index
 *          entries.
 *
 * @remarks @a it is the pointer returned back on the @c "changed", @c
 *          "delay,changed", and @c "selected" smart events.
 *
 * @param[in] obj The index object
 * @param[in] letter The letter under which the item should be indexed
 * @param[in] func The function to call when the item is selected
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case if an error
 */
EAPI Elm_Object_Item      *elm_index_item_prepend(Evas_Object *obj, const char *letter, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the index object after the item @a after.
 *
 * @since_tizen 2.3
 *
 * @remarks Despite the fact that the most common usage of the @a letter argument is for
 *          single char strings, one could use arbitrary strings as index
 *          entries.
 *
 * @remarks @a it is the pointer returned back on the @c "changed", @c
 *          "delay,changed", and @c "selected" smart events.
 *
 * @remarks If @a relative is @c NULL this function behaves like
 *          elm_index_item_append().
 *
 * @param[in] obj The index object
 * @param[in] after The index item to insert after
 * @param[in] letter The letter under which the item should be indexed
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case of an error
 */
EAPI Elm_Object_Item      *elm_index_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, const char *letter, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the index object before the item @a before.
 *
 * @since_tizen 2.3
 *
 * @remarks Despite that fact that the most common usage of the @a letter argument is for
 *          single char strings, one could use arbitrary strings as index
 *          entries.
 *
 * @remarks @a it is the pointer returned back on the @c "changed", @c
 *          "delay,changed", and @c "selected" smart events.
 *
 * @remarks If @a relative is @c NULL this function behave like
 *          elm_index_item_prepend().
 *
 * @param[in] obj The index object
 * @param[in] before The index item to insert after
 * @param[in] letter The letter under which the item should be indexed
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case of an error
 */
EAPI Elm_Object_Item      *elm_index_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, const char *letter, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the given index widget, using @a cmp_func
 *        function to sort items (by item handles).
 *
 * @since_tizen 2.3
 *
 * @remarks Despite the fact that the most common usage of the @a letter argument is for
 *          single char strings, one could use arbitrary strings as index
 *          entries.
 *
 * @remarks @a it is the pointer returned back on the @c "changed", @c
 *          "delay,changed", and @c "selected" smart events.
 *
 * @param[in] obj The index object
 * @param[in] letter The letter under which the item should be indexed
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The item data to set for the index's item
 * @param[in] cmp_func The comparing function to be used to sort index
 *                 items <b>by index item handles</b>
 * @param[in] cmp_data_func A @b callback function to be called for the
 *                      sorting of index items <b>by item data</b>) \n
 *                      It is used when @a cmp_func returns @c 0 (equality), which means an index
 *                      item with provided item data already exists \n
 *                      To decide which data item should be pointed to by the index item in question, @a
 *                      cmp_data_func is used \n
 *                      If @a cmp_data_func returns a non-negative value, the previous index item data is
 *                      replaced by the given @a it pointer \n
 *                      If the previous data needs to be freed, it should be done by the @a cmp_data_func function,
 *                      because all references to it are lost \n
 *                      If this function is not provided (@c NULL is given), index items are @b
 *                      duplicated, if @a cmp_func returns @c 0.
 * @return A handle to the item added, otherwise @c NULL in case of an error
 */
EAPI Elm_Object_Item      *elm_index_item_sorted_insert(Evas_Object *obj, const char *letter, Evas_Smart_Cb func, const void *data, Eina_Compare_Cb cmp_func, Eina_Compare_Cb cmp_data_func);

/**
 * @brief Finds a given index widget's item, <b>using item data</b>.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @param[in] data The item data pointed to by the desired index item
 * @return The index item handle if found, otherwise @c NULL
 */
EAPI Elm_Object_Item      *elm_index_item_find(Evas_Object *obj, const void *data);

/**
 * @brief Removes @b all items from a given index widget.
 *
 * @since_tizen 2.3
 *
 * @remarks If deletion callbacks are set, via elm_object_item_del_cb_set(),
 *          that callback function is called for each item in @a obj.
 *
 * @param[in] obj The index object
 */
EAPI void                  elm_index_item_clear(Evas_Object *obj);

/**
 * @brief Flushes the changes made to the index items so they work correctly.
 *
 * @details This flushes any changes made to items indicating that the object is ready to
 *          go. You should call this before any expected changes work. This
 *          is similar to elm_list_go().
 *
 * @since_tizen 2.3
 *
 * @remarks If not called, it won't display the index properly.
 *
 * @param[in] obj The index object
 * @param[in] level The index level (either @c 0 or @c 1) where changes were made
 */
EAPI void                  elm_index_level_go(Evas_Object *obj, int level);

/**
 * @brief Gets the letter (string) set on a given index widget item.
 *
 * @since_tizen 2.3
 *
 * @param[in] item The index item handle
 * @return The letter string set on @a it
 */
EAPI const char           *elm_index_item_letter_get(const Elm_Object_Item *item);

/**
 * @brief Sets the indicator to be disabled.
 *
 * @since_tizen 2.3
 *
 * @remarks In the Index widget, Indicator notes the popup text, which shows that a letter has been selected.
 *
 * @param[in] obj The index object
 * @param[in] disabled  If @c EINA_TRUE the indicator is disabled,
 *                  otherwise @c EINA_FALSE to enable it
 *
 * @see elm_index_indicator_disabled_get()
 */
EAPI void                 elm_index_indicator_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Gets the value of the indicator's disabled status.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @return @c EINA_TRUE if the indicator is disabled,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_index_indicator_disabled_set()
 */
EAPI Eina_Bool            elm_index_indicator_disabled_get(const Evas_Object *obj);

/**
 * @brief Enables or disables the horizontal mode on the index object.
 *
 * @since_tizen 2.3
 *
 * @remarks The vertical mode is set by default.
 *
 * @remarks On the horizontal mode items are displayed on the index from left to right,
 *          instead of from top to bottom. Also, the index scrolls horizontally.
 *
 * @param[in] obj The index object
 * @param[in] horizontal If @c EINA_TRUE the horizontal is enabled, otherwise @c EINA_FALSE to
 *                   disable it, i.e., to enable the vertical mode \n
 *                   It's an area of one @ref Fingers "finger" wide at the bottom side of the index widget's container.
 *
 * @see elm_index_horizontal_get()
 */
EAPI void                      elm_index_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Gets a value that indicates whether the horizontal mode is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @return @c EINA_TRUE means the horizontal mode selection is enabled,
 *         otherwise @c EINA_FALSE indicates that it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_index_horizontal_set()
 */
EAPI Eina_Bool                 elm_index_horizontal_get(const Evas_Object *obj);

/**
 * @brief Sets a delay change time for the index object.
 *
 * @since_tizen 2.3
 *
 * @remarks The delay time is @c 0.2 sec by default.
 *
 * @param[in] obj The index object
 * @param[in] delay_change_time The delay change time to set.
 *
 * @see elm_index_delay_change_time_get
 */
EAPI void                      elm_index_delay_change_time_set(Evas_Object *obj, double delay_change_time);

/**
 * @brief Gets a delay change time for the index object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @return The delay change time in seconds
 *
 * @see elm_index_delay_change_time_set
 */
EAPI double                    elm_index_delay_change_time_get(const Evas_Object *obj);

/**
 * @brief Enables or disables the omit feature for a given index widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @param[in] enabled If @c EINA_TRUE the omit feature is enabled, otherwise @c EINA_FALSE to disable it
 *
 * @see elm_index_omit_enabled_get()
 */
EAPI void                      elm_index_omit_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Gets whether the omit feature is enabled for a given index widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The index object
 * @return @c EINA_TRUE if the omit feature is enabled, otherwise @c EINA_FALSE
 *
 * @see elm_index_omit_enabled_set()
 */
EAPI Eina_Bool                 elm_index_omit_enabled_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sets the priority of an item.
 *
 * @remarks The priority is @c -1 by default, which means that the item doesn't belong to a group.
 *
 * @remarks The value of the priority starts from @c 0.
 *
 * @remarks In elm_index_level_go, the items are sorted in ascending order according to priority.
 *          Items of the same priority make a group and the primary group is shown by default.
 *
 * @param it The index item
 * @param priority The priority
 */
EAPI void                      elm_index_item_priority_set(Elm_Object_Item *it, int priority);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Set priority group of index. Priority group will be shown as many items as it can,
 *        and other group will be shown one character only.
 *
 * @remarks The value of the priority starts from -1.
 *          Change Index's current priority to target.
 *
 * @param obj The index object
 * @param priority target priority value in index
 *
 * @see elm_index_priority_get()
 */
EAPI void                      elm_index_priority_set(Evas_Object *obj, int priority);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Get current priority group of index.
 *
 * @param obj The index object
 * @return priority value in index
 *
 * @see elm_index_priority_set()
 */
EAPI int                       elm_index_priority_get(const Evas_Object *obj);

/**
 * @}
 */
