/**
 * @internal
 * @defgroup Menu Menu
 * @ingroup Elementary
 *
 * @image html menu_inheritance_tree.png
 * @image latex menu_inheritance_tree.eps
 *
 * @image html img/widget/menu/preview-00.png
 * @image latex img/widget/menu/preview-00.eps
 *
 * A menu is a list of items displayed above its parent. When the menu is
 * showing, its parent is darkened. Each item can have a sub-menu. The menu
 * object can be used to display a menu on a right click event, in a toolbar,
 * anywhere.
 *
 * Signals that you can add callbacks for are:
 * @li "clicked" - The user clicked the empty space in the menu to dismiss.
 *
 * The default content parts of the menu items that you can use are:
 * @li "default" - The main content of the menu item.
 *
 * The default text parts of the menu items that you can use are:
 * @li "default" - The label in the menu item.
 *
 * Supported common elm_object_item APIs.
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_part_text_get
 * @li @ref elm_object_item_part_content_set
 * @li @ref elm_object_item_part_content_get
 * @li @ref elm_object_item_disabled_set
 * @li @ref elm_object_item_disabled_get
 * @li @ref elm_object_item_signal_emit
 *
 * @{
 */

/**
 * @brief Adds a new menu to the parent.
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @ingroup Menu
 */
EAPI Evas_Object                 *elm_menu_add(Evas_Object *parent);

/**
 * @brief Sets the parent for the given menu widget.
 *
 * @param[in] obj The menu object
 * @param[in] parent The new parent
 *
 * @ingroup Menu
 */
EAPI void                         elm_menu_parent_set(Evas_Object *obj, Evas_Object *parent);

/**
 * @brief Gets the parent for the given menu widget.
 *
 * @param[in] obj The menu object
 * @return The parent
 *
 * @see elm_menu_parent_set()
 *
 * @ingroup Menu
 */
EAPI Evas_Object                 *elm_menu_parent_get(const Evas_Object *obj);

/**
 * @brief Moves the menu to a new position.
 *
 * @details This sets the top-left position of the menu to (@a x,@a y).
 *
 * @remarks The @a x and @a y coordinates are relative to the parent.
 *
 * @param[in] obj The menu object
 * @param[in] x The new position
 * @param[in] y The new position
 *
 * @ingroup Menu
 */
EAPI void                         elm_menu_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);

/**
 * @brief Closes an opened menu.
 *
 * @details This hides the menu and all it's sub-menus.
 *
 * @param[in] obj The menu object
 * @return A void value
 *
 * @ingroup Menu
 */
EAPI void                         elm_menu_close(Evas_Object *obj);

/**
 * @brief Gets a list of the @a it items.
 *
 * @param[in] obj The menu object
 * @return An Eina_List* of the @a it items
 *
 * @ingroup Menu
 */
EAPI const Eina_List             *elm_menu_items_get(const Evas_Object *obj);

/**
 * @brief Gets the Evas_Object of an Elm_Object_Item.
 *
 * @remarks Don't manipulate this object.
 *
 * @param[in] it The menu item object
 * @return The edje object containing the swallowed content
 *
 * @ingroup Menu
 */
EAPI Evas_Object                 *elm_menu_item_object_get(const Elm_Object_Item *it);

/**
 * @brief Adds an item at the end of the given menu widget.
 *
 * @param[in] obj The menu object
 * @param[in] parent The parent menu item (optional)
 * @param[in] icon An icon display on the item \n
 *             The icon is destroyed by the menu.
 * @param[in] label The label of the item
 * @param[in] func The function called when the user selects the item
 * @param[in] data The data sent by the callback
 * @return The new item
 *
 * @ingroup Menu
 */
EAPI Elm_Object_Item             *elm_menu_item_add(Evas_Object *obj, Elm_Object_Item *parent, const char *icon, const char *label, Evas_Smart_Cb func, const void *data);

/**
 * @brief Sets the icon of a menu item to the standard icon with name @a icon.
 *
 * @remarks Once this icon is set, any previously set icon is deleted.
 *
 * @param[in] it The menu item object
 * @param[in] icon The name of the icon object to set for the content of @a it
 *
 * @ingroup Menu
 */
EAPI void                         elm_menu_item_icon_name_set(Elm_Object_Item *it, const char *icon);

/**
 * @brief Gets the string representation from the icon of a menu item.
 *
 * @param[in] it The menu item object
 * @return The string representation of the @a it icon, otherwise @c NULL
 *
 * @see elm_menu_item_icon_name_set()
 *
 * @ingroup Menu
 */
EAPI const char                  *elm_menu_item_icon_name_get(const Elm_Object_Item *it);

/**
 * @brief Sets the selected state of @a it.
 *
 * @param[in] it The menu item object
 * @param[in] selected The selected/unselected state of the item
 *
 * @ingroup Menu
 */
EAPI void                         elm_menu_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);

/**
 * @brief Gets the selected state of @a it.
 *
 * @param[in] it The menu item object
 * @return The selected/unselected state of the item
 *
 * @see elm_menu_item_selected_set()
 *
 * @ingroup Menu
 */
EAPI Eina_Bool                    elm_menu_item_selected_get(const Elm_Object_Item *it);

/**
 * @brief Adds a separator item to the menu @a obj under the @a parent.
 *
 * @remarks This item is a @ref Separator.
 *
 * @param[in] obj The menu object
 * @param[in] parent The item to add the separator under
 * @return The created item, otherwise @c NULL on failure
 *
 * @ingroup Menu
 */
EAPI Elm_Object_Item             *elm_menu_item_separator_add(Evas_Object *obj, Elm_Object_Item *parent);

/**
 * @brief Returns whether @a it is a separator.
 *
 * @param[in] it The item to check
 * @return If @c true @a it is a separator,
 *         otherwise @c false
 *
 * @see elm_menu_item_separator_add()
 *
 * @ingroup Menu
 */
EAPI Eina_Bool                    elm_menu_item_is_separator(Elm_Object_Item *it);

/**
 * @brief Returns a list of the @a it subitems.
 *
 * @param[in] it The item
 * @return An Eina_List* of the @a it subitems
 *
 * @see elm_menu_add()
 *
 * @ingroup Menu
 */
EAPI const Eina_List             *elm_menu_item_subitems_get(const Elm_Object_Item *it);

/**
 * @brief Gets the position of a menu item.
 *
 * @details This function returns the index position of a menu item in a menu.
 *          For a sub-menu, this number is relative to the first item in the sub-menu.
 *
 * @remarks Index values begin with @c 0.
 *
 * @param[in] it The menu item
 * @return The item's index
 *
 * @ingroup Menu
 */
EAPI unsigned int                 elm_menu_item_index_get(const Elm_Object_Item *it);

/**
 * @brief Gets the selected item in the menu.
 *
 * @param[in] obj The menu object
 * @return The selected item, otherwise @c NULL if none are selected
 *
 * @see elm_menu_item_selected_get()
 * @see elm_menu_item_selected_set()
 *
 * @ingroup Menu
 */
EAPI Elm_Object_Item             *elm_menu_selected_item_get(const Evas_Object *obj);

/**
 * @brief Gets the last item in the menu.
 *
 * @param[in] obj The menu object
 * @return The last item, otherwise @c NULL if none are present
 *
 * @ingroup Menu
 */
EAPI Elm_Object_Item             *elm_menu_last_item_get(const Evas_Object *obj);

/**
 * @brief Gets the first item in the menu.
 *
 * @param[in] obj The menu object
 * @return The first item, otherwise @c NULL if none are present
 *
 * @ingroup Menu
 */
EAPI Elm_Object_Item             *elm_menu_first_item_get(const Evas_Object *obj);

/**
 * @brief Gets the next item in the menu.
 *
 * @param[in] it The menu item object
 * @return The item after @a it, otherwise @c NULL if none are present
 *
 * @ingroup Menu
 */
EAPI Elm_Object_Item             *elm_menu_item_next_get(const Elm_Object_Item *it);

/**
 * @brief Gets the previous item in the menu.
 *
 * @param[in] it The menu item object
 * @return The item before @a it, otherwise @c NULL if none are present
 *
 * @ingroup Menu
 */
EAPI Elm_Object_Item             *elm_menu_item_prev_get(const Elm_Object_Item *it);

/**
 * @}
 */
