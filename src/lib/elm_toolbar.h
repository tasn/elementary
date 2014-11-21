/**
 * @defgroup Toolbar Toolbar
 * @ingroup elm_widget_group
 *
 * @image html toolbar_inheritance_tree.png
 * @image latex toolbar_inheritance_tree.eps
 *
 * @image html img/toolbar.png
 * @image latex img/toolbar.eps "toolbar" width=\textwidth
 *
 * @brief A toolbar is a widget that displays a list of items inside a box.
 *
 * It can be scrollable, can show a menu with items that don't fit
 * to the toolbar size or an can even crop those menu items.
 *
 * Only one item can be selected at a time.
 *
 * Items can have multiple states, or show menus when selected by the user.
 *
 * This widget implements the elm-scrollable-interface
 * interface, so that all (non-deprecated) functions for the base
 * @ref Scroller widget also work for toolbars (@since 1.8)
 *
 * Smart callbacks that one can listen to:
 * - @c "clicked" - When the user clicks on a toolbar item and it gets
 *                  selected.
 * - @c "longpressed" - When the toolbar is pressed for a certain
 *                      amount of time.
 * - @c "language,changed" - When the program language changes.
 *
 * Available styles for it:
 * - @c "default"
 * - @c "transparent" - No background or shadow, just shows the content.
 *
 * Default text parts of the toolbar items that you can use are:
 * @li "default" - Label of the toolbar item
 *
 * Supported common elm_object_item APIs.
 * @li @ref elm_object_item_disabled_set
 * @li @ref elm_object_item_disabled_get
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_part_text_get
 *
 * @{
 */

/**
 * @enum Elm_Toolbar_Shrink_Mode
 * @typedef Elm_Toolbar_Shrink_Mode
 *
 * @brief Enumeration that sets the toolbar items display behavior, it can be scrollable,
 *        can show a menu with exceeding items, or simply hide them.
 *
 * @since_tizen 2.3
 *
 * @remarks Default value is #ELM_TOOLBAR_SHRINK_MENU. It reads values
 *          from elm config.
 *
 * @remarks Values <b> don't </b> work as bitmask, only one can be chosen.
 *
 * @see elm_toolbar_shrink_mode_set()
 * @see elm_toolbar_shrink_mode_get()
 */
typedef enum
{
   ELM_TOOLBAR_SHRINK_NONE, /**< Sets minimum toolbar size to fit all the items */
   ELM_TOOLBAR_SHRINK_HIDE, /**< Hides exceeding items */
   ELM_TOOLBAR_SHRINK_SCROLL, /**< Allows accessing exceeding items through a scroller */
   ELM_TOOLBAR_SHRINK_MENU, /**< Inserts a button to pop up a menu with exceeding items */
   ELM_TOOLBAR_SHRINK_EXPAND, /**< Expands all items according to the size of the toolbar */
   ELM_TOOLBAR_SHRINK_LAST /**< Indicates an error if returned by elm_toolbar_shrink_mode_get() */
} Elm_Toolbar_Shrink_Mode;

/**
 * @brief Enumeration that defines where to position the item in the toolbar.
 *
 * @since_tizen 2.3
 */
typedef enum
{
   ELM_TOOLBAR_ITEM_SCROLLTO_NONE = 0,   /**< Scroll to nowhere */
   ELM_TOOLBAR_ITEM_SCROLLTO_IN = (1 << 0),   /**< Scroll to the nearest viewport */
   ELM_TOOLBAR_ITEM_SCROLLTO_FIRST = (1 << 1),   /**< Scroll to the first of the viewport */
   ELM_TOOLBAR_ITEM_SCROLLTO_MIDDLE = (1 << 2),   /**< Scroll to the middle of the viewport */
   ELM_TOOLBAR_ITEM_SCROLLTO_LAST = (1 << 3)   /**< Scroll to the last of the viewport */
} Elm_Toolbar_Item_Scrollto_Type;

typedef struct _Elm_Toolbar_Item_State Elm_Toolbar_Item_State;    /**< States of a Elm_Toolbar_Item. Can be created with elm_toolbar_item_state_add() and removed with elm_toolbar_item_state_del() */

/**
 * @brief Adds a new toolbar widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new toolbar widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new toolbar widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object                 *elm_toolbar_add(Evas_Object *parent);

/**
 * @brief Sets the icon size, in pixels, to be used by toolbar items.
 *
 * @since_tizen 2.3
 *
 * @remarks The default value is @c 32. It reads values from elm config.
 *
 * @param[in] obj The toolbar object
 * @param[in] icon_size The icon size in pixels
 *
 * @see elm_toolbar_icon_size_get()
 */
EAPI void                         elm_toolbar_icon_size_set(Evas_Object *obj, int icon_size);

/**
 * @brief Gets the icon size, in pixels, to be used by toolbar items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The icon size in pixels
 *
 * @see elm_toolbar_icon_size_set()
 */
EAPI int                          elm_toolbar_icon_size_get(const Evas_Object *obj);

/**
 * @brief Sets the icon lookup order, for toolbar item icons.
 *
 * @since_tizen 2.3
 *
 * @remarks Icons added before calling this function are not affected.
 *          The default lookup order is #ELM_ICON_LOOKUP_THEME_FDO.
 *
 * @param[in] obj The toolbar object
 * @param[in] order The icon lookup order
 *
 * @see elm_toolbar_icon_order_lookup_get()
 */
EAPI void                         elm_toolbar_icon_order_lookup_set(Evas_Object *obj, Elm_Icon_Lookup_Order order);

/**
 * @brief Gets the icon lookup order.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The icon lookup order
 *
 * @see elm_toolbar_icon_order_lookup_set()
 */
EAPI Elm_Icon_Lookup_Order        elm_toolbar_icon_order_lookup_get(const Evas_Object *obj);

/**
 * @brief Appends items to the toolbar.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and appended to the toolbar, i.e., it
 *          is set as the @b last item.
 *
 * @remarks Items created with this method can be deleted with
 *          elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when an item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @remarks Toolbar loads the icon image from fdo or the current theme.
 *          This behavior can be set by the elm_toolbar_icon_order_lookup_set() function.
 *          If an absolute path is provided it loads it directly from a file.
 *
 * @param[in] obj The toolbar object
 * @param[in] icon A string with the icon name or the absolute path of an image file
 * @param[in] label The label of the item
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_icon_set()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_toolbar_item_append(Evas_Object *obj, const char *icon, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Prepends items to the toolbar.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and prepended to the toolbar, i.e., it
 *          is set as the @b first item.
 *
 * @remarks Items created with this method can be deleted with
 *          elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when an item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @remarks Toolbar loads the icon image from fdo or the current theme.
 *          This behavior can be set by the elm_toolbar_icon_order_lookup_set() function.
 *          If an absolute path is provided it loads it directly from a file.
 *
 * @param[in] obj The toolbar object
 * @param[in] icon A string with the icon name or the absolute path of an image file
 * @param[in] label The label of the item
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_icon_set()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_toolbar_item_prepend(Evas_Object *obj, const char *icon, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the toolbar object before item @a before.
 *
 * @since_tizen 2.3
 *
 * @remarks A new item is created and added to the toolbar. Its position in
 *          this toolbar is just before item @a before.
 *
 * @remarks Items created with this method can be deleted with
 *          elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when an item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @remarks Toolbar loads the icon image from fdo or the current theme.
 *          This behavior can be set by the elm_toolbar_icon_order_lookup_set() function.
 *          If an absolute path is provided it loads it directly from a file.
 *
 * @param[in] obj The toolbar object
 * @param[in] before The toolbar item to insert before
 * @param[in] icon A string with the icon name or the absolute path of an image file
 * @param[in] label The label of the item
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_icon_set()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_toolbar_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, const char *icon, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Inserts a new item into the toolbar object after item @a after.
 *
 * @since_tizen 2.3
 *
 * @remarks If new item is created and added to the toolbar, its position in
 *          this toolbar is just after item @a after.
 *
 * @remarks Items created with this method can be deleted with
 *          elm_object_item_del().
 *
 * @remarks Associated @a data can be properly freed when an item is deleted if a
 *          callback function is set with elm_object_item_del_cb_set().
 *
 * @remarks If a function is passed as an argument, it is called every time this item
 *          is selected, i.e., the user clicks over an unselected item.
 *          If such a function isn't needed, just passing
 *          @c NULL as @a func is enough. The same should be done for @a data.
 *
 * @remarks Toolbar loads the icon image from fdo or the current theme.
 *          This behavior can be set by the elm_toolbar_icon_order_lookup_set() function.
 *          If an absolute path is provided it loads it directly from a file.
 *
 * @param[in] obj The toolbar object
 * @param[in] after The toolbar item to insert after
 * @param[in] icon A string with the icon name or the absolute path of an image file
 * @param[in] label The label of the item
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The data to associate with the item for related callbacks
 * @return The created item, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_icon_set()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_toolbar_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, const char *icon, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Gets the first item in the given toolbar widget's list of
 *        items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The first item, otherwise @c NULL if it has no items (and on
 *         errors)
 *
 * @see elm_toolbar_item_append()
 * @see elm_toolbar_last_item_get()
 */
EAPI Elm_Object_Item             *elm_toolbar_first_item_get(const Evas_Object *obj);

/**
 * @brief Gets the last item in the given toolbar widget's list of
 *        items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The last item, otherwise @c NULL if it has no items (and on
 *         errors)
 *
 * @see elm_toolbar_item_prepend()
 * @see elm_toolbar_first_item_get()
 */
EAPI Elm_Object_Item             *elm_toolbar_last_item_get(const Evas_Object *obj);

/**
 * @brief Gets the item after @a it in the toolbar.
 *
 * @since_tizen 2.3
 *
 * @remarks If it is the last item, @c NULL is returned.
 *
 * @param[in] it The toolbar item
 * @return The item after @a it, otherwise @c NULL if none are present or on failure
 *
 * @see elm_toolbar_item_append()
 */
EAPI Elm_Object_Item             *elm_toolbar_item_next_get(const Elm_Object_Item *it);

/**
 * @brief Gets the item before @a it in the toolbar.
 *
 * @since_tizen 2.3
 *
 * @remarks If it is the first item, @c NULL is returned.
 *
 * @param[in] it The toolbar item
 * @return The item before @a it, otherwise @c NULL if none are present or on failure
 *
 * @see elm_toolbar_item_prepend()
 */
EAPI Elm_Object_Item             *elm_toolbar_item_prev_get(const Elm_Object_Item *it);

/**
 * @brief Sets the priority of a toolbar item.
 *
 * @since_tizen 2.3
 *
 * @remarks This is used only when the toolbar shrink mode is set
 *          to #ELM_TOOLBAR_SHRINK_MENU or #ELM_TOOLBAR_SHRINK_HIDE.
 *          When space is less than required, items with low priority
 *          are removed from the toolbar and added to a dynamically-created menu,
 *          while items with higher priority remain on the toolbar,
 *          in the same order as they were added.
 *
 * @param[in] it The toolbar item
 * @param[in] priority The item priority \n
 *                 The default is zero.
 *
 * @see elm_toolbar_item_priority_get()
 */
EAPI void                         elm_toolbar_item_priority_set(Elm_Object_Item *it, int priority);

/**
 * @brief Gets the priority of a toolbar item.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @return The @a it priority, otherwise @c 0 on failure
 *
 * @see elm_toolbar_item_priority_set()
 */
EAPI int                          elm_toolbar_item_priority_get(const Elm_Object_Item *it);

/**
 * @brief Returns a pointer to a toolbar item by its label.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @param[in] label The label of the item to find
 *
 * @return The pointer to the toolbar item matching @a label, otherwise @c NULL
 *         on failure
 */
EAPI Elm_Object_Item             *elm_toolbar_item_find_by_label(const Evas_Object *obj, const char *label);

/**
 * @brief Gets whether the @a it is selected.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @return @c EINA_TRUE indicates that the item is selected, otherwise @c EINA_FALSE indicates it's not \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_toolbar_selected_item_set() for details.
 * @see elm_toolbar_item_selected_get()
 */
EAPI Eina_Bool                    elm_toolbar_item_selected_get(const Elm_Object_Item *it);

/**
 * @brief Sets the selected state of an item.
 *
 * @since_tizen 2.3
 *
 * @details This sets the selected state of the given item @a it.
 *          @c EINA_TRUE for selected, @c EINA_FALSE for not selected.
 *
 * @remarks If a new item is selected the previously selected item is unselected.
 *          Previously selected items can be obtained with
 *          elm_toolbar_selected_item_get().
 *          Selected items are highlighted.
 *
 * @param[in] it The toolbar item
 * @param[in] selected The selected state
 *
 * @see elm_toolbar_item_selected_get()
 * @see elm_toolbar_selected_item_get()
 */
EAPI void                         elm_toolbar_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);

/**
 * @brief Gets the selected item.
 *
 * @since_tizen 2.3
 *
 * @remarks The selected item can be unselected with function
 *          elm_toolbar_item_selected_set().
 *          The selected item is highlighted on the toolbar.
 *
 * @param[in] obj The toolbar object
 * @return The selected toolbar item
 *
 * @see elm_toolbar_selected_items_get()
 */
EAPI Elm_Object_Item             *elm_toolbar_selected_item_get(const Evas_Object *obj);

/**
 * @brief Gets the more item.
 *
 * @since_tizen 2.3
 *
 * @remarks The more item can be changed with function
 *          elm_object_item_text_set() and elm_object_item_content_set.
 *
 * @param[in] obj The toolbar object
 * @return The toolbar more item
 */
EAPI Elm_Object_Item             *elm_toolbar_more_item_get(const Evas_Object *obj);

/**
 * @brief Sets the icon associated with @a it.
 *
 * @since_tizen 2.3
 *
 * @remarks Toolbar loads the icon image from fdo or the current theme.
 *          This behavior can be set by the elm_toolbar_icon_order_lookup_set() function.
 *          If an absolute path is provided it loads it directly from a file.
 *
 * @param[in] it The toolbar item
 * @param[in] icon A string with the icon name or the absolute path of an image file
 *
 * @see elm_toolbar_icon_order_lookup_set()
 * @see elm_toolbar_icon_order_lookup_get()
 */
EAPI void                         elm_toolbar_item_icon_set(Elm_Object_Item *it, const char *icon);

/**
 * @brief Gets the string used to set the icon of @a it.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @return The string associated with the icon object
 *
 * @see elm_toolbar_item_icon_set()
 */
EAPI const char                  *elm_toolbar_item_icon_get(const Elm_Object_Item *it);

/**
 * @brief Gets the object of @a it.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @return The object
 */
EAPI Evas_Object                 *elm_toolbar_item_object_get(const Elm_Object_Item *it);

/**
 * @brief Gets the icon object of @a it.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @return The icon object
 *
 * @see elm_toolbar_item_icon_set()
 * @see elm_toolbar_item_icon_file_set()
 * @see elm_toolbar_item_icon_memfile_set()
 */
EAPI Evas_Object                 *elm_toolbar_item_icon_object_get(Elm_Object_Item *it);

/**
 * @brief Sets the icon associated with @a it to an image in a binary buffer.
 *
 * @since_tizen 2.3
 *
 * @remarks The icon image set by this function can be changed by
 *          elm_toolbar_item_icon_set().
 *
 * @param[in] it The toolbar item
 * @param[in] img The binary data that is used as an image
 * @param[in] size The size of binary data @a img
 * @param[in] format The optional format of @a img to pass to the image loader
 * @param[in] key The optional key of @a img to pass to the image loader (eg. if @a img is an edje file)
 *
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 */
EAPI Eina_Bool                    elm_toolbar_item_icon_memfile_set(Elm_Object_Item *it, const void *img, size_t size, const char *format, const char *key);

/**
 * @brief Sets the icon associated with @a it to an image in a binary buffer.
 *
 * @since_tizen 2.3
 *
 * @remarks The icon image set by this function can be changed by
 *          elm_toolbar_item_icon_set().
 *
 * @param[in] it The toolbar item
 * @param[in] file The file that contains the image
 * @param[in] key The optional key of @a img to pass to the image loader (eg. if @a img is an edje file)
 *
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 */
EAPI Eina_Bool                    elm_toolbar_item_icon_file_set(Elm_Object_Item *it, const char *file, const char *key);

/**
 * @brief Sets or unsets item as a separator.
 *
 * @since_tizen 2.3
 *
 * @remarks Items aren't set as a separator by default.
 *          If set as a separator it displays a separator theme, so it won't display
 *          icons or labels.
 *
 * @param[in] it The toolbar item
 * @param[in] separator If @c EINA_TRUE set item @a it as a separator,
 *                  otherwise @c EINA_FALSE to unset it, i.e., the item is used as a regular item
 *
 * @see elm_toolbar_item_separator_get()
 */
EAPI void                         elm_toolbar_item_separator_set(Elm_Object_Item *it, Eina_Bool separator);

/**
 * @brief Gets a value that indicates whether the item is a separator.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @return @c EINA_TRUE indicates that item @a it is a separator, otherwise @c EINA_FALSE indicates it's not \n
 *         If @a it is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_toolbar_item_separator_set()
 */
EAPI Eina_Bool                    elm_toolbar_item_separator_get(const Elm_Object_Item *it);

/**
 * @brief Sets the item displaying mode of a given toolbar widget @a obj.
 *
 * @since_tizen 2.3
 *
 * @remarks The toolbar won't scroll under the #ELM_TOOLBAR_SHRINK_NONE mode, but
 *          it would enforce a minimum size, so that all the items fit
 *          inside it. It won't scroll and won't show the items that don't fit
 *          under the #ELM_TOOLBAR_SHRINK_HIDE mode. Finally, it scrolls under the
 *          #ELM_TOOLBAR_SHRINK_SCROLL mode, and it creates a button to
 *          aggregate items which didn't fit with the #ELM_TOOLBAR_SHRINK_MENU
 *          mode.
 *
 * @remarks This function's behavior clashes with that of
 *          elm_scroller_policy_set(), so use either one of them, but not both.
 *
 * @param[in] obj The toolbar object handle
 * @param[in] shrink_mode The toolbar's items display behavior
 */
EAPI void                         elm_toolbar_shrink_mode_set(Evas_Object *obj, Elm_Toolbar_Shrink_Mode shrink_mode);

/**
 * @brief Gets the shrink mode of the toolbar @a obj.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The toolbar's items display behavior
 *
 * @see elm_toolbar_shrink_mode_set()
 */
EAPI Elm_Toolbar_Shrink_Mode      elm_toolbar_shrink_mode_get(const Evas_Object *obj);

/**
 * @brief Sets the item's transverse expansion of a given toolbar widget @a obj.
 *
 * @details This expands the transverse length of the item according to the transverse length of the toolbar.
 *          The default is what the transverse length of the item is set to according to its minimum value.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @param[in] transverse_expanded The transverse expansion of the item
 *                            (EINA_TRUE = on, EINA_FALSE = off, default = EINA_FALSE)
 */
EAPI void                         elm_toolbar_transverse_expanded_set(Evas_Object *obj, Eina_Bool transverse_expanded);

/**
 * @brief Gets the transverse expansion of the toolbar @a obj.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The transverse expansion of the item
 *         (EINA_TRUE = on, EINA_FALSE = off, default = EINA_FALSE)
 *
 * @see elm_toolbar_transverse_expand_set()
 */
EAPI Eina_Bool                    elm_toolbar_transverse_expanded_get(const Evas_Object *obj);

/**
 * @brief Enables or disables the homogeneous mode.
 *
 * @details This enables the homogeneous mode where items are of the same size.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @param[in] homogeneous The boolean value that assumes that the items within the toolbar are of the
 *                    same size (EINA_TRUE = on, EINA_FALSE = off) \n
 *                    The default is @c EINA_FALSE.
 *
 * @see elm_toolbar_homogeneous_get()
 */
EAPI void                         elm_toolbar_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous);

/**
 * @brief Gets whether the homogeneous mode is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The boolean value that assumes that the items within the toolbar are of the same height
 *         and width (EINA_TRUE = on, EINA_FALSE = off)
 *
 * @see elm_toolbar_homogeneous_set()
 */
EAPI Eina_Bool                    elm_toolbar_homogeneous_get(const Evas_Object *obj);

/**
 * @brief Sets the parent object of the toolbar items menus.
 *
 * @since_tizen 2.3
 *
 * @remarks Each item can be set as an item menu, with elm_toolbar_item_menu_set().
 *
 * @remarks For more details about setting the parent for toolbar menus, see
 *          elm_menu_parent_set().
 *
 * @param[in] obj The toolbar object
 * @param[in] parent The parent of the menu objects
 *
 * @see elm_menu_parent_set()
 * @see elm_toolbar_item_menu_set()
 */
EAPI void                         elm_toolbar_menu_parent_set(Evas_Object *obj, Evas_Object *parent);

/**
 * @brief Gets the parent object of the toolbar items menus.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The parent of the menu objects
 *
 * @see elm_toolbar_menu_parent_set()
 */
EAPI Evas_Object                 *elm_toolbar_menu_parent_get(const Evas_Object *obj);

/**
 * @brief Sets the alignment of the items.
 *
 * @since_tizen 2.3
 *
 * @remarks Alignment of the toolbar items, from <tt> 0.0 </tt> that indicates to align
 *          to the left, to <tt> 1.0 </tt>, that indicates to align to the right. <tt> 0.5 </tt> to centralize
 *          items.
 *
 * @remarks Items are centered by default.
 *
 * @param[in] obj The toolbar object
 * @param[in] align The new alignment, a float between <tt> 0.0 </tt>
 *              and <tt> 1.0 </tt>
 *
 * @see elm_toolbar_align_get()
 */
EAPI void                         elm_toolbar_align_set(Evas_Object *obj, double align);

/**
 * @brief Gets the alignment of the items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The toolbar items alignment, a float between <tt> 0.0 </tt> and
 *         <tt> 1.0 </tt>
 *
 * @see elm_toolbar_align_set()
 */
EAPI double                       elm_toolbar_align_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks This API is unusable.
 * @brief Sets whether the toolbar item opens a menu.
 *
 * @since_tizen 2.3
 *
 * @remarks A toolbar item can be set to be a menu, using this function.
 *
 * @remarks Once it is set to be a menu, it can be manipulated through the
 *          menu-like function elm_toolbar_menu_parent_set() and the other
 *          elm_menu functions, using the Evas_Object @a menu returned by
 *          elm_toolbar_item_menu_get().
 *
 *          So, items to be displayed in this item's menu should be added with
 *          elm_menu_item_add().
 *
 * @remarks The following code exemplifies the most basic usage:
 * @code
 * tb = elm_toolbar_add(win)
 * item = elm_toolbar_item_append(tb, "refresh", "Menu", NULL, NULL);
 * elm_toolbar_item_menu_set(item, EINA_TRUE);
 * elm_toolbar_menu_parent_set(tb, win);
 * menu = elm_toolbar_item_menu_get(item);
 * elm_menu_item_add(menu, NULL, "edit-cut", "Cut", NULL, NULL);
 * menu_item = elm_menu_item_add(menu, NULL, "edit-copy", "Copy", NULL,
 * NULL);
 * @endcode
 *
 * @param[in] it The toolbar item
 * @param[in] menu If @c EINA_TRUE @a it opens a menu when selected,
 *             otherwise @c EINA_FALSE
 *
 * @see elm_toolbar_item_menu_get()
 */
EAPI void                         elm_toolbar_item_menu_set(Elm_Object_Item *it, Eina_Bool menu);

/**
 * @internal
 * @remarks This API is unusable.
 *
 * @brief Gets the toolbar item's menu.
 *
 * @since_tizen 2.3
 *
 * @remarks If @a it isn't set as a menu item using elm_toolbar_item_menu_set(),
 *          this function sets it.
 *
 * @param[in] it The toolbar item
 * @return The item's menu object, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_menu_set()
 */
EAPI Evas_Object                 *elm_toolbar_item_menu_get(const Elm_Object_Item *it);

/**
 * @brief Adds a new state to @a it.
 *
 * @since_tizen 2.3
 *
 * @remarks Toolbar loads the icon image from fdo or the current theme.
 *          This behavior can be set by the elm_toolbar_icon_order_lookup_set() function.
 *          If an absolute path is provided it loads it directly from a file.
 *
 * @remarks States created with this function can be removed using
 *          elm_toolbar_item_state_del().
 *
 * @param[in] it The toolbar item
 * @param[in] icon A string with the icon name or the absolute path of an image file
 * @param[in] label The label of the new state
 * @param[in] func The function to call when the item is clicked and when this
 *             state is selected
 * @param[in] data The data to associate with the state
 * @return The toolbar item state, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_state_del()
 * @see elm_toolbar_item_state_sel()
 * @see elm_toolbar_item_state_get()
 */
EAPI Elm_Toolbar_Item_State      *elm_toolbar_item_state_add(Elm_Object_Item *it, const char *icon, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Deletes a previously added state to @a it.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @param[in] state The state to be deleted
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_toolbar_item_state_add()
 */
EAPI Eina_Bool                    elm_toolbar_item_state_del(Elm_Object_Item *it, Elm_Toolbar_Item_State *state);

/**
 * @brief Sets @a state as the current state of @a it.
 *
 * @since_tizen 2.3
 *
 * @remarks If @a state is @c NULL, it won't select any state and the default item's
 *          icon and label is used. It's the same behaviour as
 *          elm_toolbar_item_state_unset().
 *
 * @param[in] it The toolbar item
 * @param[in] state The state to use
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_toolbar_item_state_unset()
 */
EAPI Eina_Bool                    elm_toolbar_item_state_set(Elm_Object_Item *it, Elm_Toolbar_Item_State *state);

/**
 * @brief Unsets the state of @a it.
 *
 * @since_tizen 2.3
 *
 * @remarks The default icon and label from this item is displayed.
 *
 * @param[in] it The toolbar item
 *
 * @see elm_toolbar_item_state_set()
 */
EAPI void                         elm_toolbar_item_state_unset(Elm_Object_Item *it);

/**
 * @brief Gets the current state of @a it.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @return The selected state, otherwise @c NULL if none are selected or on failure
 *
 * @see elm_toolbar_item_state_set() for details.
 * @see elm_toolbar_item_state_unset()
 * @see elm_toolbar_item_state_add()
 */
EAPI Elm_Toolbar_Item_State      *elm_toolbar_item_state_get(const Elm_Object_Item *it);

/**
 * @brief Gets the state after the selected state in the toolbar's @a it.
 *
 * @since_tizen 2.3
 *
 * @remarks If the last state is selected, this function returns the first state.
 *
 * @param[in] it The toolbar item to change state
 * @return The state after the current state, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_state_set()
 * @see elm_toolbar_item_state_add()
 */
EAPI Elm_Toolbar_Item_State      *elm_toolbar_item_state_next(Elm_Object_Item *it);

/**
 * @brief Gets the state before the selected state in the toolbar's @a it.
 *
 * @since_tizen 2.3
 *
 * @remarks If the first state is selected, this function returns the last state.
 *
 * @param[in] it The toolbar item to change state
 * @return The state before the current state, otherwise @c NULL on failure
 *
 * @see elm_toolbar_item_state_set()
 * @see elm_toolbar_item_state_add()
 */
EAPI Elm_Toolbar_Item_State      *elm_toolbar_item_state_prev(Elm_Object_Item *it);

/**
 * @brief Changes a toolbar's orientation.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, a toolbar is horizontal. Use this function to create a vertical toolbar.
 *
 * @param[in] obj The toolbar object
 * @param[in] horizontal If @c EINA_TRUE the toolbar is horizontal,
 *                   otherwise @c EINA_FALSE
 */
EAPI void                         elm_toolbar_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Gets a toolbar's orientation.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, a toolbar is horizontal. Use this function to determine whether a toolbar is vertical.
 *
 * @param[in] obj The toolbar object
 * @return If @c EINA_TRUE, the toolbar is horizontal,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool                    elm_toolbar_horizontal_get(const Evas_Object *obj);

/**
 * @brief Gets the number of items in a toolbar.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The number of items in the @a obj toolbar
 */
EAPI unsigned int                 elm_toolbar_items_count(const Evas_Object *obj);

/**
 * @brief Sets the standard priority of visible items in a toolbar.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks If the priority of the item is upto the standard priority, it is shown in the basic panel.
 *          The other items are located in the more menu or panel. The more menu or panel can be shown when the more item is clicked.
 *
 * @param[in] obj The toolbar object
 * @param[in] priority The standard priority of visible items
 *
 * @see elm_toolbar_standard_priority_get()
 */
EAPI void                         elm_toolbar_standard_priority_set(Evas_Object *obj, int priority);

/**
 * @brief Gets the standard priority of visible items in a toolbar.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The standard priority of items in the @a obj toolbar
 *
 * @see elm_toolbar_standard_priority_set()
 */
EAPI int                          elm_toolbar_standard_priority_get(const Evas_Object *obj);

/**
 * @brief Sets the toolbar select mode.
 *
 * @since_tizen 2.3
 *
 * @remarks elm_toolbar_select_mode_set() changes the item select mode in the toolbar widget.
 *          - ELM_OBJECT_SELECT_MODE_DEFAULT : Items only call their selection @a func and
 *                                             callback on first getting selected. Any further clicks
 *                                             do nothing, unless you set the always select mode.
 *          - ELM_OBJECT_SELECT_MODE_ALWAYS :  This means that, even if selected,
 *                                             every click calls the selected callbacks.
 *          - ELM_OBJECT_SELECT_MODE_NONE : This turns off the ability to select items
 *                                          entirely and they neither appear selected nor call selected
 *                                          callback functions.
 *
 * @param[in] obj The toolbar object
 * @param[in] mode The select mode
 *
 * @see elm_toolbar_select_mode_get()
 */
EAPI void
elm_toolbar_select_mode_set(Evas_Object *obj, Elm_Object_Select_Mode mode);

/**
 * @brief Gets the toolbar select mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The select mode
 *         (If the getting mode fails, it returns @c ELM_OBJECT_SELECT_MODE_MAX)
 *
 * @see elm_toolbar_select_mode_set()
 */
EAPI Elm_Object_Select_Mode
elm_toolbar_select_mode_get(const Evas_Object *obj);

/**
 * @brief Sets the reorder mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @param[in] reorder_mode The reorder mode
 *                     (EINA_TRUE = on, EINA_FALSE = off)
 */
EAPI void                          elm_toolbar_reorder_mode_set(Evas_Object *obj, Eina_Bool reorder_mode);

/**
 * @brief Gets the reorder mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The toolbar object
 * @return The reorder mode
 *         (EINA_TRUE = on, EINA_FALSE = off)
 */
EAPI Eina_Bool                     elm_toolbar_reorder_mode_get(const Evas_Object *obj);

/**
 * @brief Shows a specific item, when the toolbar can be scrolled.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @param[in] type Item scroll to type
 *
 * @see elm_toolbar_item_bring_in()
 */
EAPI void                          elm_toolbar_item_show(Elm_Object_Item *it, Elm_Toolbar_Item_Scrollto_Type type);

/**
 * @brief Shows a specific item with scroll animation, when the toolbar can be scrolled.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] it The toolbar item
 * @param[in] type Item scroll to type
 *
 * @see elm_toolbar_item_show()
 */
EAPI void                          elm_toolbar_item_bring_in(Elm_Object_Item *it, Elm_Toolbar_Item_Scrollto_Type type);

/**
 * @}
 */
