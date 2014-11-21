/**
 * @defgroup Gengrid Gengrid (Generic grid)
 * @ingroup elm_widget_group
 *
 * @image html gengrid_inheritance_tree.png
 * @image latex gengrid_inheritance_tree.eps
 *
 * @brief This widget aims to position objects in a grid layout while
 *        actually creating and rendering only the visible ones.
 *
 * Gengrid using the same idea as the @ref Genlist "genlist": the user defines
 * a @b class for each item, specifying functions that are called at
 * object creation, deletion, etc. When those items are selected by
 * the user, a callback function is issued. Users may interact with
 * the gengrid via a mouse (by clicking on items to select them and
 * clicking on the grid's viewport and swiping to pan the whole
 * view) or via the keyboard, navigating through items using the
 * arrow keys.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for gengrid objects.
 *
 * This widget implements the elm-scrollable-interface
 * interface, so that all (non-deprecated) functions for the base @ref
 * Scroller widget also work for gengrids.
 *
 * Some calls on the gengrid's API are marked as @b deprecated, as
 * they just wrap the scrollable widgets counterpart functions. Use
 * the ones mentioned for each case of deprecation here,
 * Eventually the deprecated ones are discarded (next
 * major release).
 *
 * @section Gengrid_Layouts Gengrid layouts
 *
 * Gengrid may layout its items in one of the following two possible layouts:
 * - horizontal
 * - vertical
 *
 * When in the "horizontal mode", items are placed in @b columns
 * from top to bottom and when the space for a column is filled,
 * another one is started on the right, thus expanding the grid
 * horizontally and allowing horizontal scrolling. When in the "vertical
 * mode", though items are placed in @b rows from left to
 * right, and when the space for a row is filled, another one is
 * started below, thus expanding the grid vertically (and allowing
 * vertical scrolling).
 *
 * @section Gengrid_Items Gengrid items
 *
 * An item in a gengrid can have @c 0 or more texts (they can be
 * regular text or textblock Evas objects - that's up to the style
 * to determine), @c 0 or more contents (which are simply objects
 * swallowed into the gengrid item's theming Edje object) and @c 0 or
 * more <b>boolean states</b>, which have their behavior left to the
 * user to define. The Edje part names for each of these properties
 * are looked up in the theme file for the gengrid, under the
 * Edje (string) data items named @c "texts", @c "contents", and @c
 * "states", respectively. For each of these properties, if more
 * than one part is provided, they must have names listed and separated
 * by spaces in the data fields. By default, in a gengrid item
 * theme, we have @b one text part (@c "elm.text"), @b two content
 * parts (@c "elm.swalllow.icon" and @c "elm.swallow.end"), and @b
 * no state parts.
 *
 * A gengrid item may have one of the several styles. Elementary
 * provides one by default - "default", but this can be extended by the
 * system or application custom themes/overlays/extensions (see
 * @ref Theme "themes" for more details).
 *
 * @section Gengrid_Item_Class Gengrid item classes
 *
 * In order to have the ability to add and delete items on the fly,
 * gengrid implements a class (callback) system where the
 * application provides a structure with information about that
 * type of item (gengrid may contain multiple items of different types with
 * different classes, states, and styles). Gengrid calls the
 * functions in this struct (methods) when an item is "realized"
 * (i.e., created dynamically, while the user is scrolling the
 * grid). All objects are simply deleted when no longer needed
 * with evas_object_del(). The #Elm_Gengrid_Item_Class structure
 * contains the following members:
 * - @c item_style - This is a constant string and simply defines
 * the name of the item style. It @b must be specified and the
 * default should be @c "default".
 * - @c func.text_get - This function is called when an item
 * object is actually created. The @a data parameter points to
 * the same data passed to elm_gengrid_item_append() and other related
 * item creation functions. The @a obj parameter is the gengrid
 * object itself, while the @c part one is the name string of one
 * of the existing text parts in the Edje group implementing the
 * item's theme. This function @b must return a strdup'()ed string,
 * as the caller uses free() to free it when done.
 * See #Elm_Gengrid_Item_Text_Get_Cb.
 * - @c func.content_get - This function is called when an item object
 * is actually created. The @a data parameter points to the
 * same data passed to elm_gengrid_item_append() and other related item
 * creation functions. The @a obj parameter is the gengrid object
 * itself, while the @c part one is the name string of one of the
 * existing (content) swallow parts in the Edje group implementing the
 * item's theme. It must return @c NULL when no content is desired,
 * otherwise, a valid object handle. The object is deleted
 * by the gengrid on its deletion or when the item is "unrealized".
 * See #Elm_Gengrid_Item_Content_Get_Cb.
 * - @c func.state_get - This function is called when an item
 * object is actually created. The @a data parameter points to
 * the same data passed to elm_gengrid_item_append() and other related
 * item creation functions. The @a obj parameter is the gengrid
 * object itself, while the @c part one is the name string of one
 * of the state parts in the Edje group implementing the item's
 * theme. Return @c EINA_FALSE for false/off or @c EINA_TRUE for
 * true/on. Gengrids emit a signal to its theming Edje object
 * with @c "elm,state,xxx,active" and @c "elm" as "emission" and
 * "source" arguments respectively, when the state is @c true (the
 * default is false), where @c xxx is the name of the (state) part.
 * See #Elm_Gengrid_Item_State_Get_Cb.
 * - @c func.del - This is called when elm_object_item_del() is
 * called on an item or elm_gengrid_clear() is called on the
 * gengrid. This is intended for use when gengrid items are
 * deleted, so any data attached to the item (e.g. its @a data
 * parameter on creation) can be deleted. See #Elm_Gengrid_Item_Del_Cb.
 *
 * @section Gengrid_Usage_Hints Usage hints
 *
 * If the user wants to have multiple items selected at the same
 * time, elm_gengrid_multi_select_set() permits it. If the
 * gengrid is single-selection only (the default), then
 * elm_gengrid_select_item_get() returns the selected item, otherwise
 * it returns @c NULL, if no item is selected. If the gengrid is under
 * multi-selection, then elm_gengrid_selected_items_get()
 * returns a list (that is only valid as long as no items are
 * modified (added, deleted, selected, or unselected) from the child items
 * on a gengrid.
 *
 * If an item changes (internal (boolean) state, text, or content
 * changes), then use elm_gengrid_item_update() to have gengrid
 * update the item with the new state. A gengrid re-realizes
 * the item, thus calling the functions in the #Elm_Gengrid_Item_Class
 * set for that item.
 *
 * To programmatically (un)select an item, use
 * elm_gengrid_item_selected_set(). To get its selected state use
 * elm_gengrid_item_selected_get(). To disable an item
 * (unable to be selected and appear differently) use
 * elm_object_item_disabled_set() to set this and
 * elm_object_item_disabled_get() to get the disabled state.
 *
 * Grid cells only have their selection smart callbacks called
 * when getting selected for the first time. Any further clicks do
 * nothing, unless you enable the "always select mode", with
 * elm_gengrid_select_mode_set() as @c ELM_OBJECT_SELECT_MODE_ALWAYS,
 * thus making every click to issue selection callbacks.
 * elm_gengrid_select_mode_set() as @c ELM_OBJECT_SELECT_MODE_NONE
 * turns off the ability to select items entirely in the widget and
 * they neither appear selected nor call the selection smart
 * callbacks.
 *
 * Remember that you can create new styles and add your own theme
 * augmentation for each application using elm_theme_extension_add(). If
 * you absolutely must have a specific style that overrides any
 * theme the user or system sets up, you can use
 * elm_theme_overlay_add() to add such a file.
 *
 * @section Gengrid_Smart_Events Gengrid smart events
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "activated" - The user has double-clicked or pressed
 *   (enter|return|spacebar) on an item. The @a event_info parameter
 *   is the gengrid item that is activated.
 * - @c "pressed" - The user pressed an item. The @a event_info
 *   parameter is the item that is pressed.
 * - @c "released" - The user released an item. The @a event_info
 *   parameter is the item that is released.
 * - @c "clicked,double" - The user has double-clicked an item.
 *   The @a event_info parameter is the gengrid item that is double-clicked.
 * - @c "longpressed" - This is called when the item is pressed for a certain
 *   amount of time. By default it's @c 1 second.
 * - @c "selected" - The user has selected an item. The
 *   @a event_info parameter is the gengrid item that is selected.
 * - @c "unselected" - The user has unselected an item. The
 *   @a event_info parameter is the gengrid item that is unselected.
 * - @c "realized" - This is called when the item in the gengrid
 *   has its implementing Evas object instantiated, de facto. @a
 *   event_info is the gengrid item that is created. The object
 *   may be deleted at any time, so it is highly advised to the
 *   caller @b not to use the object pointer returned from
 *   elm_gengrid_item_object_get(), because it may point to freed
 *   objects.
 * - @c "unrealized" - This is called when the implementing Evas
 *   object for this item is deleted. @a event_info is the gengrid
 *   item that is deleted.
 * - @c "changed" - Called when an item is added, removed, resized,
 *   or moved and when the gengrid is resized or gets "horizontal"
 *   property changes.
 * - @c "scroll,anim,start" - This is called when scrolling animation has
 *   started.
 * - @c "scroll,anim,stop" - This is called when scrolling animation has
 *   stopped.
 * - @c "drag,start,up" - Called when the item in the gengrid has
 *   been dragged (not scrolled) up.
 * - @c "drag,start,down" - Called when the item in the gengrid has
 *   been dragged (not scrolled) down.
 * - @c "drag,start,left" - Called when the item in the gengrid has
 *   been dragged (not scrolled) left.
 * - @c "drag,start,right" - Called when the item in the gengrid has
 *   been dragged (not scrolled) right.
 * - @c "drag,stop" - Called when the item in the gengrid has
 *   stopped being dragged.
 * - @c "drag" - Called when the item in the gengrid is being
 *   dragged.
 * - @c "scroll" - Called when the content has been scrolled
 *   (moved).
 * - @c "scroll,drag,start" - Called when dragging the content has
 *   started.
 * - @c "scroll,drag,stop" - Called when dragging the content has
 *   stopped.
 * - @c "edge,top" - Called when the gengrid is scrolled till
 *   the top edge.
 * - @c "edge,bottom" - Called when the gengrid is scrolled
 *   until the bottom edge.
 * - @c "edge,left" - Called when the gengrid is scrolled
 *   till the left edge.
 * - @c "edge,right" - Called when the gengrid is scrolled
 *   till the right edge.
 * - @c "highlighted" - Called when an item in the list is pressed and highlighted.
 *   The @a event_info parameter is the item that is highlighted.
 * - @c "unhighlighted" - an item in the list is unpressed and unhighlighted.
 *   The @a event_info parameter is the item that is unhighlighted.
 * - @c "language,changed" - Called when the program's language is
 *   changed. Call elm_gengrid_realized_items_update() if items text needs to
 *   be translated.
 *
 * Supported common elm_object APIs.
 * @li elm_object_signal_emit()
 *
 * Supported common elm_object_item APIs.
 * @li elm_object_item_part_content_get()
 * @li elm_object_item_part_content_set()
 * @li elm_object_item_part_content_unset()
 * @li elm_object_item_part_text_set()
 * @li elm_object_item_part_text_get()
 * @li elm_object_item_disabled_set()
 * @li elm_object_item_disabled_get()
 *
 * @{
 */

#define ELM_GENGRID_ITEM_CLASS_VERSION ELM_GEN_ITEM_CLASS_VERSION
#define ELM_GENGRID_ITEM_CLASS_HEADER ELM_GEN_ITEM_CLASS_HEADER

/**
 * @brief Enumeration that defines the type of item part.
 * @remarks It is used while updating item parts
 * @remarks It can be used at updating multi fields.
 */
typedef enum
{
   ELM_GENGRID_ITEM_FIELD_ALL = 0,     /**< The item contains all fields */
   ELM_GENGRID_ITEM_FIELD_TEXT = (1 << 0), /**< The item contains a text field */
   ELM_GENGRID_ITEM_FIELD_CONTENT = (1 << 1), /**< The item contains a content field */
   ELM_GENGRID_ITEM_FIELD_STATE = (1 << 2)  /**< The item contains a state field */
} Elm_Gengrid_Item_Field_Type;

/**
 * @brief Enumeration that defines where to position the item in the gengrid.
 */
typedef enum
{
   ELM_GENGRID_ITEM_SCROLLTO_NONE = 0,   /**< Scrolls to nowhere */
   ELM_GENGRID_ITEM_SCROLLTO_IN = (1 << 0),   /**< Scrolls to the nearest viewport */
   ELM_GENGRID_ITEM_SCROLLTO_TOP = (1 << 1),   /**< Scrolls to the top of the viewport */
   ELM_GENGRID_ITEM_SCROLLTO_MIDDLE = (1 << 2)   /**< Scrolls to the middle of the viewport */
} Elm_Gengrid_Item_Scrollto_Type;


/**
 * @see Elm_Gen_Item_Class
 */
typedef Elm_Gen_Item_Class Elm_Gengrid_Item_Class;

/**
 * @see Elm_Gen_Item_Text_Get_Cb
 */
typedef Elm_Gen_Item_Text_Get_Cb Elm_Gengrid_Item_Text_Get_Cb;

/**
 * @see Elm_Gen_Item_Content_Get_Cb
 */
typedef Elm_Gen_Item_Content_Get_Cb Elm_Gengrid_Item_Content_Get_Cb;

/**
 * @see Elm_Gen_Item_State_Get_Cb
 */
typedef Elm_Gen_Item_State_Get_Cb Elm_Gengrid_Item_State_Get_Cb;

/**
 * @see Elm_Gen_Item_Del_Cb
 */
typedef Elm_Gen_Item_Del_Cb Elm_Gengrid_Item_Del_Cb;

/**
 * @brief Adds a new gengrid widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new gengrid widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new gengrid widget handle, otherwise @c NULL in case of an error

 *
 * @see elm_gengrid_item_size_set()
 * @see elm_gengrid_group_item_size_set()
 * @see elm_gengrid_horizontal_set()
 * @see elm_gengrid_item_append()
 * @see elm_object_item_del()
 * @see elm_gengrid_clear()
 */
EAPI Evas_Object                  *elm_gengrid_add(Evas_Object *parent);

/**
 * @brief Removes all items from a given gengrid widget.
 *
 * @details This removes (and deletes) all items in @a obj, making it
 *          empty.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 *
 * @see elm_object_item_del() to remove just one item.
 */
EAPI void                          elm_gengrid_clear(Evas_Object *obj);

/**
 * @brief Enables or disables multi-selection in a given gengrid widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Multi-selection is the ability to have @b more than one
 *          item selected, on a given gengrid, simultaneously. When it is
 *          enabled, a sequence of clicks on different items makes them
 *          all selected, progressively. A click on an already selected item
 *          unselects it. If interacting via the keyboard,
 *          multi-selection is enabled while holding the "Shift" key.
 *
 * @remarks By default, multi-selection is @b disabled on gengrids.
 *
 * @param[in] obj The gengrid object
 * @param[in] multi If @c EINA_TRUE multi-selection is enabled,
 *              otherwise @c EINA_FALSE to disable it
 *
 * @see elm_gengrid_multi_select_get()
 */
EAPI void                          elm_gengrid_multi_select_set(Evas_Object *obj, Eina_Bool multi);

/**
 * @brief Gets whether multi-selection is enabled or disabled for a given
 *        gengrid widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return @c EINA_TRUE if multi-selection is enabled, 
 *         otherwise @c EINA_FALSE
 *
 * @see elm_gengrid_multi_select_set()
 */
EAPI Eina_Bool                     elm_gengrid_multi_select_get(const Evas_Object *obj);

/**
 * @brief Sets the direction in which a given gengrid widget expands while
 *        placing its items.
 *
 * @since_tizen 2.3
 *
 * @remarks When in the @c "horizontal mode" (@c EINA_TRUE), items are placed
 *          in @b columns from top to bottom and when the space for a
 *          column is filled, another one is started on the right, thus
 *          expanding the grid horizontally. When in the @c "vertical mode"
 *          (@c EINA_FALSE), though items are placed in @b rows from left
 *          to right, and when the space for a row is filled, another one is
 *          started below, thus expanding the grid vertically.
 *
 * @param[in] obj The gengrid object
 * @param[in] horizontal If @c EINA_TRUE the gengrid expands horizontally,
 *                   otherwise @c EINA_FALSE to expand vertically
 *
 * @see elm_gengrid_horizontal_get()
 */
EAPI void                          elm_gengrid_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Gets the direction for which a given gengrid widget expands while
 *        placing its items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return @c EINA_TRUE if @a obj is set to expand horizontally,
 *         otherwise @c EINA_FALSE if it's set to expand vertically
 *
 * @see elm_gengrid_horizontal_set()
 */
EAPI Eina_Bool                     elm_gengrid_horizontal_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Enables or disables bouncing effect for a given gengrid widget.
 *
 * @remarks The bouncing effect occurs whenever one reaches the gengrid's
 *          edges while panning it.It scrolls past its limits a
 *          little bit and returns to the edge again, in an animated form,
 *          automatically.
 *
 * @remarks By default, gengrids have bouncing enabled on both axis.
 *
 * @param obj The gengrid object
 * @param h_bounce If @c EINA_TRUE @b horizontal bouncing is enabled,
 *                 otherwise @c EINA_FALSE to disable it
 * @param v_bounce If @c EINA_TRUE @b vertical bouncing is enabled,
 *                 otherwise @c EINA_FALSE to disable it
 *
 * @deprecated Use elm_scroller_bounce_set() instead.
 *
 * @see elm_scroller_bounce_set()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce);

/**
 * @internal
 *
 * @brief Gets whether bouncing effects are enabled or disabled for a
 *        given gengrid widget on each axis.
 *
 * @param obj The gengrid object
 * @param h_bounce The pointer to a variable where the horizontal bouncing flag is stored
 *
 * @param v_bounce The pointer to a variable where the vertical bouncing flag is stored
 *
 *
 * @deprecated Use elm_scroller_bounce_get() instead.
 *
 * @see elm_scroller_bounce_get()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_bounce_get(const Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce);

/**
 * @brief Appends a new item to a given gengrid widget.
 *
 * @details This adds an item to the beginning of the gengrid.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @param[in] gic The item class for the item
 * @param[in] data The item data
 * @param[in] func The convenience function that is called when the item is
 *             selected
 * @param[in] func_data The data to be passed to @a func
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see elm_gengrid_item_prepend()
 * @see elm_gengrid_item_insert_before()
 * @see elm_gengrid_item_insert_after()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_gengrid_item_append(Evas_Object *obj, const Elm_Gengrid_Item_Class *gic, const void *data, Evas_Smart_Cb func, const void *func_data);

/**
 * @brief Prepends a new item to a given gengrid widget.
 *
 * @details This adds an item to the end of the gengrid.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @param[in] gic The item class for the item
 * @param[in] data The item data
 * @param[in] func The convenience function that is called when the item is
 *             selected
 * @param[in] func_data The data to be passed to @a func
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see elm_gengrid_item_append()
 * @see elm_gengrid_item_insert_before()
 * @see elm_gengrid_item_insert_after()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_gengrid_item_prepend(Evas_Object *obj, const Elm_Gengrid_Item_Class *gic, const void *data, Evas_Smart_Cb func, const void *func_data);

/**
 * @brief Inserts an item before another in a gengrid widget.
 *
 * @details This inserts an item before another in the gengrid.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @param[in] gic The item class for the item
 * @param[in] data The item data
 * @param[in] relative The item tbefore which to place this new one
 * @param[in] func The convenience function that is called when the item is
 *             selected
 * @param[in] func_data The data to be passed to @a func
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see elm_gengrid_item_append()
 * @see elm_gengrid_item_prepend()
 * @see elm_gengrid_item_insert_after()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_gengrid_item_insert_before(Evas_Object *obj, const Elm_Gengrid_Item_Class *gic, const void *data, Elm_Object_Item *relative, Evas_Smart_Cb func, const void *func_data);

/**
 * @brief Inserts an item after another in a gengrid widget.
 *
 * @details This inserts an item after another in the gengrid.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @param[in] gic The item class for the item
 * @param[in] data The item data
 * @param[in] relative The item after which to place this new one
 * @param[in] func The convenience function that is called when the item is
 *             selected
 * @param[in] func_data The data to be passed to @a func
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see elm_gengrid_item_append()
 * @see elm_gengrid_item_prepend()
 * @see elm_gengrid_item_insert_after()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_gengrid_item_insert_after(Evas_Object *obj, const Elm_Gengrid_Item_Class *gic, const void *data, Elm_Object_Item *relative, Evas_Smart_Cb func, const void *func_data);

/**
 * @brief Inserts an item in a gengrid widget using a user-defined sort function.
 *
 * @details This inserts an item in the gengrid based on a user defined comparison
 *          function. The two arguments passed to the function @a func are gengrid
 *          item handles to compare.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @param[in] gic The item class for the item
 * @param[in] data The item data
 * @param[in] comp The user defined comparison function that defines the sort order
 *             based on Elm_Gen_Item and its @a data parameter.
 * @param[in] func The convenience function that is called when the item is selected
 * @param[in] func_data The data to be passed to @a func
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see elm_gengrid_item_append()
 * @see elm_gengrid_item_prepend()
 * @see elm_gengrid_item_insert_after()
 * @see elm_object_item_del()
 */
EAPI Elm_Object_Item             *elm_gengrid_item_sorted_insert(Evas_Object *obj, const Elm_Gengrid_Item_Class *gic, const void *data, Eina_Compare_Cb comp, Evas_Smart_Cb func, const void *func_data);

/**
 * @brief Gets the selected item in a given gengrid widget.
 *
 * @details This returns the selected item in @a obj. If multi selection is
 *          enabled on @a obj (@see elm_gengrid_multi_select_set()), only
 *          the first item in the list is selected, which might not be very
 *          useful. In that case, see elm_gengrid_selected_items_get().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return The selected item's handle, otherwise @c NULL if none are
 *         selected at the moment (and on errors)
 */
EAPI Elm_Object_Item             *elm_gengrid_selected_item_get(const Evas_Object *obj);

/**
 * @brief Gets <b>a list</b> of selected items in a given gengrid.
 *
 * @details This returns a list of the selected items, in the order that
 *          they appear in the grid. This list is only valid as long as no
 *          more items are selected or unselected (or unselected implicitly
 *          by deletion). The list contains gengrid item pointers as
 *          data, naturally.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return The list of selected items, otherwise @c NULL if none are
 *         selected at the moment (and on errors)
 *
 * @see elm_gengrid_selected_item_get()
 */
EAPI const Eina_List              *elm_gengrid_selected_items_get(const Evas_Object *obj);

/**
 * @brief Gets a list of realized items in the gengrid.
 *
 * @details This returns a list of realized items in the gengrid. The list
 *          contains gengrid item pointers. The list must be freed by the
 *          caller when done using eina_list_free(). The item pointers in the
 *          list are only valid as long as those items are not deleted or the
 *          gengrid is not deleted.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return The list of realized items, otherwise @c NULL if none are realized
 *
 * @see elm_gengrid_realized_items_update()
 */
EAPI Eina_List                    *elm_gengrid_realized_items_get(const Evas_Object *obj);

/**
 * @brief Updates the contents of all the realized items.
 *
 * @details This updates all realized items by calling all the item class functions again
 *          to get the content, text, and states. Use this when the original
 *          item data has changed and the changes are desired to reflect.
 *
 * @since_tizen 2.3
 *
 * @remarks To update just one item, use elm_gengrid_item_update().
 *
 * @param[in] obj The gengrid object
 *
 * @see elm_gengrid_realized_items_get()
 * @see elm_gengrid_item_update()
 */
EAPI void                          elm_gengrid_realized_items_update(Evas_Object *obj);

/**
 * @brief Gets the first item in a given gengrid widget.
 *
 * @details This returns the first item in the @a obj internal list of
 *          items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return The first item's handle, otherwise @c NULL if there are no
 *         items in @a obj (and on errors)
 *
 * @see elm_gengrid_last_item_get()
 */
EAPI Elm_Object_Item             *elm_gengrid_first_item_get(const Evas_Object *obj);

/**
 * @brief Gets the last item in a given gengrid widget.
 *
 * @details This returns the last item in the @a obj internal list of
 *          items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return The last item's handle, otherwise @c NULL if there are no
 *         items in @a obj (and on errors)
 *
 * @see elm_gengrid_first_item_get()
 */
EAPI Elm_Object_Item             *elm_gengrid_last_item_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 * @brief Set the timeout in seconds for the longpress event.
 *
 * @details This option will change how long it takes to send an event
 *          "longpressed" after the mouse down signal is sent to the list.
 *          If this event occurs, no "clicked" event will be sent.
 *
 * @since_tizen 2.3
 *
 * @remarks If you set the longpress timeout value with this API, your gengrid
 * will not be affected by the longpress value of elementary config value
 * later.
 *
 * @param[in] obj The gengrid object
 * @param[in] timeout timeout in seconds. Default is elm config value(1.0)
 *
 * @see elm_gengrid_longpress_timeout_set()
 */
EAPI void                          elm_gengrid_longpress_timeout_set(Evas_Object *obj, double timeout);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the timeout in seconds for the longpress event.
 *
 * @param obj The gengrid object
 * @return The timeout in seconds
 *
 * @see elm_gengrid_longpress_timeout_get()
 */
EAPI double                        elm_gengrid_longpress_timeout_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets the scrollbar policy.
 *
 * @details This sets the scrollbar visibility policy for the given gengrid
 *          scroller. #ELM_SCROLLER_POLICY_AUTO means that the scrollbar is made
 *          visible if needed, and otherwise kept
 *          hidden. #ELM_SCROLLER_POLICY_ON turns it on at all times, and
 *          #ELM_SCROLLER_POLICY_OFF always keeps it off.  This applies
 *          for the horizontal and vertical scrollbars respectively. The default value
 *          is #ELM_SCROLLER_POLICY_AUTO.
 *
 * @param obj The gengrid object
 * @param policy_h The horizontal scrollbar policy
 * @param policy_v The vertical scrollbar policy
 *
 * @deprecated Use elm_scroller_policy_set() instead.
 *
 * @see elm_scroller_policy_set()
 *
 * @see elm_gengrid_scroller_policy_get()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v);

/**
 * @internal
 *
 * @brief Gets the scrollbar policy.
 *
 * @param obj The gengrid object
 * @param policy_h The pointer to store the horizontal scrollbar policy
 * @param policy_v The pointer to store the vertical scrollbar policy
 *
 * @deprecated Use elm_scroller_policy_get() instead.
 *
 * @see elm_scroller_policy_get()
 *
 * @see elm_gengrid_scroller_policy_set()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_scroller_policy_get(const Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v);

/**
 * @brief Gets the @b next item in a gengrid widget's internal list of items,
 *        given that a handle to one of those items is present.
 *
 * @details This returns the item placed after the @a it, on the container
 *          gengrid.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item to fetch next from
 * @return The item after @a it, otherwise @c NULL if there are none (and
 *         on errors)
 *
 * @see elm_gengrid_item_prev_get()
 */
EAPI Elm_Object_Item             *elm_gengrid_item_next_get(const Elm_Object_Item *it);

/**
 * @brief Gets the @b previous item in a gengrid widget's internal list of items,
 *        given that a handle to one of those items is present.
 *
 * @details This returns the item placed before the @a it, on the container
 *          gengrid.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item to fetch previous from
 * @return The item before @a it, otherwise @c NULL if there are none (and
 *         on errors)
 *
 * @see elm_gengrid_item_next_get()
 */
EAPI Elm_Object_Item             *elm_gengrid_item_prev_get(const Elm_Object_Item *it);

/**
 * @brief Sets whether a given gengrid item is selected.
 *
 * @details This sets the selected state of an item. If multi-selection is
 *          not enabled on the containing gengrid and @a selected is @c
 *          EINA_TRUE, any other previously selected items get
 *          unselected in favor of this new one.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item
 * @param[in] selected If @c EINA_TRUE it is selected,
 *                 otherwise @c EINA_FALSE to unselect it
 *
 * @see elm_gengrid_item_selected_get()
 */
EAPI void                          elm_gengrid_item_selected_set(Elm_Object_Item *it, Eina_Bool selected);

/**
 * @brief Gets whether a given gengrid item is selected.
 *
 * @since_tizen 2.3
 *
 * @remarks This API returns @c EINA_TRUE for all the items selected in the multi-select mode as well.
 *
 * @param[in] it The gengrid item
 * @return @c EINA_TRUE if it's selected, otherwise @c EINA_FALSE
 *
 * @see elm_gengrid_item_selected_set()
 */
EAPI Eina_Bool                     elm_gengrid_item_selected_get(const Elm_Object_Item *it);

/**
 * @brief Shows the portion of a gengrid internal grid containing a given
 *        item @b immediately.
 *
 * @since_tizen 2.3
 *
 * @remarks This causes gengrid to @b redraw its viewport's contents to the
 *          region containing the given @a it item, if it is not fully
 *          visible.
 *
 * @param[in] it The item to display
 * @param[in] type The position of the item in the viewport
 *
 * @see elm_gengrid_item_bring_in()
 */
EAPI void                          elm_gengrid_item_show(Elm_Object_Item *it, Elm_Gengrid_Item_Scrollto_Type type);

/**
 * @brief Animatedly brings a given item to the visible area of a gengrid.
 *
 * @details This causes gengrid to jump to the given @a it and show
 *          it (by scrolling), if it is not fully visible. This uses
 *          animation to do so and takes a period of time to complete.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item to display
 * @param[in] type The position of the item in the viewport
 *
 * @see elm_gengrid_item_show()
 */
EAPI void                          elm_gengrid_item_bring_in(Elm_Object_Item *it, Elm_Gengrid_Item_Scrollto_Type type);

/**
 * @brief Updates the content of a given gengrid item.
 *
 * @details This updates an item by calling all the item class functions
 *          again to get the content, text, and states. Use this when the
 *          original item data has changed and you want the changes to
 *          reflect.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item
 */
EAPI void                          elm_gengrid_item_update(Elm_Object_Item *it);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Updates the part of an item.
 *
 * @details This updates an item's part by calling the item's fetching functions again
 *          to get the content, text, and states. Use this when the original
 *          item data has changed and the changes are desired to reflect.
 *          Second part's argument is used for globbing to match '*', '?', and '.'
 *          It can be used for updating multi fields.
 *
 * @since_tizen 2.3
 *
 * @remarks Use elm_gengrid_realized_items_update() to update an item's all
 *          property.
 *
 * @param it The item
 * @param parts The name of the item's part
 * @param itf The type of the item's part
 *
 * @see elm_gengrid_item_update()
 */
EAPI void                          elm_gengrid_item_fields_update(Elm_Object_Item *it, const char *parts, Elm_Gengrid_Item_Field_Type itf);

/**
 * @brief Updates the item class of a gengrid item.
 *
 * @details This sets another class of the item, changing the way it is
 *          displayed. After changing the item class, elm_gengrid_item_update() is
 *          called on the item @a it.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item
 * @param[in] gic The gengrid item class describing the function pointers and the item style
 */
EAPI void                          elm_gengrid_item_item_class_update(Elm_Object_Item *it, const Elm_Gengrid_Item_Class *gic);

/**
 * @brief Gets the gengrid item class for the given gengrid item.
 *
 * @details This returns the Gengrid_Item_Class for the given item. It can be used to examine
 *          the function pointers and item style.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item
 * @return The item class
 */
EAPI const Elm_Gengrid_Item_Class *elm_gengrid_item_item_class_get(const Elm_Object_Item *it);

/**
 * @brief Gets the index of the item. It is only valid once it is displayed.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item
 * @return The position inside the list of items
 */
EAPI int                           elm_gengrid_item_index_get(const Elm_Object_Item *it);

/**
 * @brief Returns the number of items that are currently in a list.
 *
 * @since_tizen 2.3
 *
 * @remarks This behavior is O(1) and includes items which may or may not be realized.
 *
 * @param[in] obj The list
 * @return The total number of list items in the list
 */
EAPI unsigned int elm_gengrid_items_count(const Evas_Object *obj);

/**
 * @brief Adds a new gengrid item class in a given gengrid widget.
 *
 * @details This adds the gengrid item class for the gengrid widget. When adding an item,
 *          the gengrid_item_{append, prepend, insert} function needs the item class of the item.
 *          Given callback parameters are used for retrieving {text, content} of an
 *          added item. Set as @c NULL if it's not used.
 *          If there's no available memory, it returns @c NULL.
 *
 * @since_tizen 2.3
 *
 * @return The newly allocated gengrid item class
 *
 * @see elm_gengrid_item_class_free()
 * @see elm_gengrid_item_append()
 */
EAPI Elm_Gengrid_Item_Class *elm_gengrid_item_class_new(void);

/**
 * @brief Removes an item class in a given gengrid widget.
 *
 * @details This removes the item class from the gengrid widget.
 *          Whenever it has no more references to it, the item class is freed.
 *          Otherwise it just decreases its reference count.
 *
 * @since_tizen 2.3
 *
 * @param[in] itc The itc to be removed
 *
 * @see elm_gengrid_item_class_new()
 * @see elm_gengrid_item_class_ref()
 * @see elm_gengrid_item_class_unref()
 */
EAPI void elm_gengrid_item_class_free(Elm_Gengrid_Item_Class *itc);

/**
 * @brief Increments the object reference count for the item class.
 *
 * @since_tizen 2.3
 *
 * @remarks This API just increases its reference count for item class management.
 *
 * @param[in] itc The given item class object to reference
 *
 * @see elm_gengrid_item_class_unref()
 */
EAPI void elm_gengrid_item_class_ref(Elm_Gengrid_Item_Class *itc);

/**
 * @brief Decrements the object reference count for the item class.
 *
 * @since_tizen 2.3
 *
 * @remarks This API just decreases its reference count for item class management.
 *          the reference count can't be less than @c 0.
 *
 * @param[in] itc The given item class object to reference
 *
 * @see elm_gengrid_item_class_ref()
 * @see elm_gengrid_item_class_free()
 */
EAPI void elm_gengrid_item_class_unref(Elm_Gengrid_Item_Class *itc);

/**
 * @brief Sets the text to be shown in a given gengrid item's tooltips.
 *
 * @since_tizen 2.3
 *
 * @remarks This call is used to setup the text to be used as a tooltip for that item
 *          (analogous to elm_object_tooltip_text_set(), but are item
 *          tooltips with higher precedence than object tooltips). It can
 *          have only one tooltip at a time, so any previous tooltip data
 *          is removed.
 *
 * @remarks In order to set content or something else as a tooltip, see
 *          elm_gengrid_item_tooltip_content_cb_set().
 *
 * @param[in] it The gengrid item
 * @param[in] text The text to set in the content
 */
EAPI void                          elm_gengrid_item_tooltip_text_set(Elm_Object_Item *it, const char *text);

/**
 * @brief Sets the content to be shown in a given gengrid item's tooltips.
 *
 * @since_tizen 2.3
 *
 * @remarks This call is used to setup the tooltip's content to @a it
 *          (analogous to elm_object_tooltip_content_cb_set(), but are
 *          item tooltips with higher precedence than object tooltips). It
 *          can have only one tooltip at a time, so any previous tooltip
 *          content is removed. @a func (with @a data) is called
 *          every time Elementary needs to display the tooltip. Moreover, it should
 *          return a valid Evas object, which is fully managed by the
 *          tooltip system and gets deleted when the tooltip is gone.
 *
 * @remarks In order to set just text as a tooltip, see
 *          elm_gengrid_item_tooltip_text_set().
 *
 * @param[in] it The gengrid item
 * @param[in] func The function returning the tooltip contents
 * @param[in] data The data to provide to @a func as callback data/context
 * @param[in] del_cb Called when data is not needed anymore, either when
 *               another callback replaces @a func, the tooltip is unset with
 *               elm_gengrid_item_tooltip_unset() or the owner @a it
 *               dies. This callback receives the given @a data as its first parameter
 *               with @a event_info as the item handle
 */
EAPI void                          elm_gengrid_item_tooltip_content_cb_set(Elm_Object_Item *it, Elm_Tooltip_Item_Content_Cb func, const void *data, Evas_Smart_Cb del_cb);

/**
 * @brief Unsets a tooltip from a given gengrid item.
 *
 * @since_tizen 2.3
 *
 * @remarks This call removes any tooltip set on @a item. The callback
 *          provided as @a del_cb to
 *          elm_gengrid_item_tooltip_content_cb_set() is called to
 *          notify that it is not used anymore (and have resources cleaned, if
 *          needed).
 *
 * @param[in] it The gengrid item from which to remove a previously set tooltip
 *
 * @see elm_gengrid_item_tooltip_content_cb_set()
 */
EAPI void                          elm_gengrid_item_tooltip_unset(Elm_Object_Item *it);

/**
 * @brief Sets a different @b style for a given gengrid item's tooltip.
 *
 * @since_tizen 2.3
 *
 * @remarks Tooltips can have <b>alternate styles</b> to be displayed on,
 *          which are defined by the theme set on Elementary. This function
 *          is analogous to elm_object_tooltip_style_set(), but is
 *          applied only to gengrid item objects. The default style for
 *          tooltips is @c "default".
 *
 * @remarks Before you set a style, you should define a tooltip with
 *          elm_gengrid_item_tooltip_content_cb_set() or
 *          elm_gengrid_item_tooltip_text_set()
 *
 * @param[in] it The gengrid item with a tooltip set
 * @param[in] style The <b>theme style</b> to use on tooltips (e.g. @c
 *              "default", @c "transparent", etc)
 *
 * @see elm_gengrid_item_tooltip_style_get()
 */
EAPI void                          elm_gengrid_item_tooltip_style_set(Elm_Object_Item *it, const char *style);

/**
 * @brief Gets the style set for a given gengrid item's tooltip.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item on which the tooltip is already set
 * @return style The theme style in use, whose default value is @c "default" \n
 *               If the object does not have a tooltip set,
 *               then @c NULL is returned.
 *
 * @see elm_gengrid_item_tooltip_style_set() for more details
 */
EAPI const char                   *elm_gengrid_item_tooltip_style_get(const Elm_Object_Item *it);

/**
 * @brief Disables the size restrictions on an object's tooltip.
 *
 * @details This function allows a tooltip to expand beyond its parent window's canvas.
 *
 * @since_tizen 2.3
 *
 * @remarks It is instead limited only to the size of the display.
 *
 * @param[in] it The tooltip's anchor object
 * @param disable If @c EINA_TRUE size restrictions are disabled,
 *                otherwise @c EINA_FALSE to enable it
 * @return  @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool                     elm_gengrid_item_tooltip_window_mode_set(Elm_Object_Item *it, Eina_Bool disable);

/**
 * @brief Retrieves the size restriction state of an object's tooltip.
 *
 * @details This function returns a value that indicates whether a tooltip is allowed to expand beyond
 *          its parent window's canvas.
 *          It is instead limited only to the size of the display.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The tooltip's anchor object
 * @return @c EINA_TRUE if size restrictions are disabled,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool                     elm_gengrid_item_tooltip_window_mode_get(const Elm_Object_Item *it);

/**
 * @brief Sets the type of mouse pointer/cursor decoration to be displayed,
 *        when the mouse pointer is over the given gengrid widget item.
 *
 * @details This function is analogous to elm_object_cursor_set(), but
 *          the cursor's changing area is restricted to the item's
 *          area, and not the whole widget. Note that item cursors
 *          have precedence over widget cursors, so a mouse over @a
 *          it always shows the @a cursor type.
 *
 * @since_tizen 2.3
 *
 * @remarks If this function is called twice for an object, a previously set
 *          cursor is unset on the second call
 *
 * @param[in] it The gengrid item on which to customize the cursor
 * @param[in] cursor The cursor type
 *
 * @see elm_object_cursor_set()
 * @see elm_gengrid_item_cursor_get()
 * @see elm_gengrid_item_cursor_unset()
 */
EAPI void                          elm_gengrid_item_cursor_set(Elm_Object_Item *it, const char *cursor);

/**
 * @brief Gets the type of mouse pointer/cursor decoration set to be displayed,
 *        when the mouse pointer is over the given gengrid widget item.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item with a custom cursor set
 * @return The cursor type, otherwise @c NULL if no custom cursors
 *         are set to @a it(and on errors)
 *
 * @see elm_object_cursor_get()
 * @see elm_gengrid_item_cursor_set()
 * @see elm_gengrid_item_cursor_unset()
 */
EAPI const char                   *elm_gengrid_item_cursor_get(const Elm_Object_Item *it);

/**
 * @brief Unsets any custom mouse pointer/cursor decoration set to be
 *        displayed, when the mouse pointer is over the given gengrid widget
 *        item, thus making it show the @b default cursor again.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this call to undo any custom settings on this item's cursor
 *          decoration, bringing it back to the default value (no custom style set).
 *
 * @param[in] it The gengrid item
 *
 * @see elm_object_cursor_unset()
 * @see elm_gengrid_item_cursor_set()
 */
EAPI void                          elm_gengrid_item_cursor_unset(Elm_Object_Item *it);

/**
 * @brief Sets a different @b style for a given custom cursor set for a
 *        gengrid item.
 *
 * @details This function only makes sense when one is using customized mouse
 *          cursor decorations <b>defined in a theme file</b> , which can
 *          have, given a cursor name/type, <b>alternate styles</b> on
 *          it. It is analogous to elm_object_cursor_style_set(), but
 *          is applied only to gengrid item objects.
 *
 * @since_tizen 2.3
 *
 * @remarks Before you set a cursor style, you should define a
 *          custom cursor on the item using
 *          elm_gengrid_item_cursor_set()
 *
 * @param[in] it The gengrid item with a custom cursor set
 * @param[in] style The <b>theme style</b> to use (e.g. @c "default",
 *              @c "transparent", etc)
 *
 * @see elm_gengrid_item_cursor_engine_only_set()
 * @see elm_gengrid_item_cursor_style_get()
 */
EAPI void                          elm_gengrid_item_cursor_style_set(Elm_Object_Item *it, const char *style);

/**
 * @brief Gets the current @b style set for a given gengrid item's custom
 *        cursor.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item with a custom cursor set
 * @return style The cursor style in use \n
 *               If the object does not have a cursor set, then @c NULL is returned.
 *
 * @see elm_gengrid_item_cursor_style_set() for more details
 */
EAPI const char                   *elm_gengrid_item_cursor_style_get(const Elm_Object_Item *it);

/**
 * @brief Sets whether the (custom) cursor for a given gengrid item should be
 *        searched in its theme or should only rely on the
 *        rendering engine.
 *
 * @since_tizen 2.3
 *
 * @remarks This call is used only if you have set a custom cursor
 *          for gengrid items using elm_gengrid_item_cursor_set().
 *
 * @remarks By default, cursors are looked for only from those
 *          provided by the rendering engine.
 *
 * @param[in] it The item with custom (custom) cursor already set on it
 * @param[in] engine_only If @c EINA_TRUE look for cursors
 *                    only from those provided by the rendering engine,
 *                    otherwise @c EINA_FALSE to have them searched on the widget's theme as well
 */
EAPI void                          elm_gengrid_item_cursor_engine_only_set(Elm_Object_Item *it, Eina_Bool engine_only);

/**
 * @brief Gets whether the (custom) cursor for a given gengrid item is being
 *        searched in its theme or is only relying on the rendering
 *        engine.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item
 * @return @c EINA_TRUE if cursors are being looked for only from
 *         those provided by the rendering engine,
 *         otherwise @c EINA_FALSE if they are being searched on the widget's theme as well
 *
 * @see elm_gengrid_item_cursor_engine_only_set()
 */
EAPI Eina_Bool                     elm_gengrid_item_cursor_engine_only_get(const Elm_Object_Item *it);

/**
 * @brief Sets the size for the items of a given gengrid widget.
 *
 * @since_tizen 2.3
 *
 * @remarks A gengrid, after creation, still has no information on the size
 *          to give to each of its cells. So you most probably end up
 *          with squares of one @ref Fingers "finger" wide, the default
 *          size. Use this function to force a custom size for your items,
 *          making them as big as you wish.
 *
 * @param[in] obj The gengrid object
 * @param[in] w The items width
 * @param[in] h The items height
 *
 * @see elm_gengrid_item_size_get()
 */
EAPI void                          elm_gengrid_item_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h);

/**
 * @brief Gets the size set for the items of a given gengrid widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use @c NULL pointers for the size values you're not
 *          interested in, they get ignored by the function.
 *
 * @param[in] obj The gengrid object
 * @param[out] w The pointer to a variable that stores the item's width
 * @param[out] h The pointer to a variable that stores the item's height
 *
 * @see elm_gengrid_item_size_get() for more details
 */
EAPI void                          elm_gengrid_item_size_get(const Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Sets the size of the group items of a given gengrid widget.
 *
 * @since_tizen 2.3
 *
 * @remarks A gengrid, after creation, still has no information on the size
 *          to give to each of its cells. So you most probably end up
 *          with squares of one @ref Fingers "finger" wide, the default
 *          size. Use this function to force a custom size for your group items,
 *          making them as big as you wish.
 *
 * @param[in] obj The gengrid object
 * @param[in] w The group item's width
 * @param[in] h The group item's height
 *
 * @see elm_gengrid_group_item_size_get()
 */
EAPI void                          elm_gengrid_group_item_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h);

/**
 * @brief Gets the size set for the group items of a given gengrid widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use @c NULL pointers for the size values you're not
 *          interested in, they get ignored by the function.
 *
 * @param[in] obj The gengrid object
 * @param[out] w The pointer to a variable that stores the group item's width
 * @param[out] h The pointer to a variable that stores the group item's height
 *
 * @see elm_gengrid_group_item_size_get()
 */
EAPI void                          elm_gengrid_group_item_size_get(const Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Sets the item's grid alignment within a given gengrid widget.
 *
 * @details This sets the alignment of the whole grid of gengrid items
 *          within its given viewport. By default, those values are both
 *          @c 0.5, meaning that the gengrid has its items grid placed
 *          exactly in the middle of its viewport.
 *
 * @since_tizen 2.3
 *
 * @remarks If the given alignment values are out of the cited range,
 *          they are changed to the nearest boundary values on the valid
 *          range.
 *
 * @param[in] obj The gengrid object
 * @param[in] align_x The alignment along the horizontal axis (0 <= align_x <= 1)
 * @param[in] align_y The alignment along the vertical axis (0 <= align_y <= 1)
 *
 * @see elm_gengrid_align_get()
 */
EAPI void                          elm_gengrid_align_set(Evas_Object *obj, double align_x, double align_y);

/**
 * @brief Gets the item's grid alignment values within a given gengrid
 *        widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use @c NULL pointers for the alignment values you're not
 *          interested in, they get ignored by the function.
 *
 * @param[in] obj The gengrid object
 * @param[out] align_x The pointer to a variable that stores the
 *                horizontal alignment
 * @param[out] align_y The pointer to a variable that stores the vertical
 *                alignment
 *
 * @see elm_gengrid_align_set()
 */
EAPI void                          elm_gengrid_align_get(const Evas_Object *obj, double *align_x, double *align_y);

/**
 * @brief Sets whether a given gengrid widget is able to have items
 *        @b reordered.
 *
 * @since_tizen 2.3
 *
 * @remarks If a gengrid is set to allow reordering, a click held for more
 *          than @c 0.5 over a given item highlights it specially,
 *          signaling the gengrid has entered the reordering state. From
 *          that time on, the user is able to, while still holding the
 *          mouse button down, move the item freely in the gengrid's
 *          viewport, replacing the said item by the locations it goes to.
 *          The replacements are animated and, whenever the user
 *          releases the mouse button, the item being replaced gets a new
 *          definitive place in the grid.
 *
 * @param[in] obj The gengrid object
 * @param[in] reorder_mode If @c EINA_TRUE reordering is turned on,
 *                     otherwise @c EINA_FALSE to turn it off
 *
 * @see elm_gengrid_reorder_mode_get()
 */
EAPI void                          elm_gengrid_reorder_mode_set(Evas_Object *obj, Eina_Bool reorder_mode);

/**
 * @brief Gets whether a given gengrid widget is able to have items
 *        @b reordered
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return @c EINA_TRUE if reordering is on,
 *         otherwise @c EINA_FALSE if it's off
 *
 * @see elm_gengrid_reorder_mode_set()
 */
EAPI Eina_Bool                     elm_gengrid_reorder_mode_get(const Evas_Object *obj);


/**
 * @internal
 *
 * @brief Sets a given gengrid widget's scrolling page size, relative to
 *        its viewport size.
 *
 * @remarks The gengrid's scroller is capable of binding scrolling by the
 *          user to "pages". It means that, while scrolling and, specially
 *          after releasing the mouse button, the grid @b snaps to the
 *          nearest displaying page's area. When page sizes are set, the
 *          grid's continuous content area is split into (equal) page sized
 *          pieces.
 *
 * @remarks This function sets the size of a page <b>relatively to the
 *          viewport dimensions</b> of the gengrid, for each axis. A value
 *          @c 1.0 means "the exact viewport's size", in that axis, while @c
 *          0.0 turns paging off in that axis. Likewise, @c 0.5 means "half
 *          a viewport". Similarly, usable values are between @c 0.0 and @c
 *          1.0. Values beyond this range make it behave
 *          inconsistently. If you only want one axis to snap to pages, use
 *          the value @c 0.0 for the other one.
 *
 * @remarks There is a function that sets page size values in @b absolute
 *          values. This function is elm_gengrid_page_size_set(). Naturally, its use
 *          is mutually exclusive to this one.
 *
 * @param obj The gengrid object
 * @param h_pagerel The horizontal page (relative) size
 * @param v_pagerel The vertical page (relative) size
 *
 * @deprecated Use elm_scroller_page_relative_set() instead.
 *
 * @see elm_scroller_page_relative_set()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_page_relative_set(Evas_Object *obj, double h_pagerel, double v_pagerel);

/**
 * @internal
 * @brief Gets a given gengrid widget's scrolling page size, relative to
 *        its viewport size.
 *
 * @param obj The gengrid object
 * @param h_pagerel The pointer to a variable that stores the
 *                  horizontal page (relative) size
 * @param v_pagerel The pointer to a variable that stores the
 *                  vertical page (relative) size
 *
 * @deprecated Use elm_scroller_page_relative_get() instead.
 *
 * @see elm_scroller_page_relative_get()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_page_relative_get(const Evas_Object *obj, double *h_pagerel, double *v_pagerel);

/**
 * @internal
 *
 * @brief Sets a given gengrid widget's scrolling page size.
 *
 * @remarks The gengrid's scroller is capable of binding scrolling by the
 *          user to "pages". This means that, while scrolling and, specially
 *          after releasing the mouse button, the grid @b snaps to the
 *          nearest displaying page's area. When page sizes are set, the
 *          grid's continuous content area is split into (equal) page sized
 *          pieces.
 *
 * @remarks This function sets the size of a page of the gengrid, in pixels,
 *          for each axis. Similarly, usable values are between @c 0 and the
 *          dimensions of @a obj, for each axis. Values beyond these
 *          make it behave inconsistently. If you only want one axis
 *          to snap to pages, use the value @c 0 for the other one.
 *
 * @remarks There is a function that sets page size values in @b relative
 *          values. This function is elm_gengrid_page_relative_set(). Naturally, its
 *          use is mutually exclusive to this one.
 *
 * @param obj The gengrid object
 * @param h_pagesize The horizontal page size in pixels
 * @param v_pagesize The vertical page size in pixels
 *
 * @deprecated Use elm_scroller_page_size_set() instead.
 *
 * @see elm_scroller_page_size_set()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_page_size_set(Evas_Object *obj, Evas_Coord h_pagesize, Evas_Coord v_pagesize);

/**
 * @internal
 *
 * @brief Gets the current gengrid page number.
 *
 * @remarks The page number starts from @c 0. @c 0 is the first page.
 *          Current page means the page that meets the top-left of the viewport.
 *          If there are two or more pages in the viewport, it returns the number of pages
 *          that meet the top-left of the viewport.
 *
 * @param obj The gengrid object
 * @param h_pagenumber The horizontal page number
 * @param v_pagenumber The vertical page number
 *
 * @deprecated Use elm_scroller_current_page_set() instead.
 *
 * @see elm_scroller_current_page_set()
 *
 * @see elm_gengrid_last_page_get()
 * @see elm_gengrid_page_show()
 * @see elm_gengrid_page_bring_in()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_current_page_get(const Evas_Object *obj, int *h_pagenumber, int *v_pagenumber);

/**
 * @internal
 *
 * @brief Gets the last gengrid page number.
 *
 * @details This returns the last page number among the pages.
 * @remarks The page number starts from @c 0. @c 0 is the first page.
 *
 * @param obj The gengrid object
 * @param h_pagenumber The horizontal page number
 * @param v_pagenumber The vertical page number
 *
 * @deprecated Use elm_scroller_last_page_set() instead.
 *
 * @see elm_scroller_last_page_set()
 *
 * @see elm_gengrid_current_page_get()
 * @see elm_gengrid_page_show()
 * @see elm_gengrid_page_bring_in()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_last_page_get(const Evas_Object *obj, int *h_pagenumber, int *v_pagenumber);

/**
 * @brief Shows a specific virtual region within the gengrid content object by its page number.
 *
 * @since_tizen 2.3
 *
 * @remarks (0, 0) of the indicated page is located at the top-left of the viewport.
 *          This jumps to the page directly without animation.
 *
 * Example of usage:
 *
 * @code
 * sc = elm_gengrid_add(win);
 * elm_gengrid_content_set(sc, content);
 * elm_gengrid_page_relative_set(sc, 1, 0);
 * elm_gengrid_current_page_get(sc, &h_page, &v_page);
 * elm_gengrid_page_show(sc, h_page + 1, v_page);
 * @endcode
 *
 * @param[in] obj The gengrid object
 * @param[in] h_pagenumber The horizontal page number
 * @param[in] v_pagenumber The vertical page number
 *
 * @see elm_gengrid_page_bring_in()
 */
EAPI void                          elm_gengrid_page_show(const Evas_Object *obj, int h_pagenumber, int v_pagenumber);

/**
 * @internal
 *
 * @brief Shows a specific virtual region within the gengrid content object by its page number.
 *
 * @remarks (0, 0) of the indicated page is located at the top-left of the viewport.
 *          This slides to the page with animation.
 *
 * Example of usage:
 *
 * @code
 * sc = elm_gengrid_add(win);
 * elm_gengrid_content_set(sc, content);
 * elm_gengrid_page_relative_set(sc, 1, 0);
 * elm_gengrid_last_page_get(sc, &h_page, &v_page);
 * elm_gengrid_page_bring_in(sc, h_page, v_page);
 * @endcode
 *
 * @param obj The gengrid object
 * @param h_pagenumber The horizontal page number
 * @param v_pagenumber The vertical page number
 *
 * @deprecated Use elm_scroller_page_bring_in() instead.
 *
 * @see elm_scroller_page_bring_in()
 *
 * @see elm_gengrid_page_show()
 */
EINA_DEPRECATED EAPI void          elm_gengrid_page_bring_in(const Evas_Object *obj, int h_pagenumber, int v_pagenumber);

/**
 * @brief Gets a given gengrid item's position, relative to the whole
 *        gengrid's grid area.
 *
 * @details This returns the "logical" position of the item within the
 *          gengrid. For example, @c (0, 1) would stand for the first row and the
 *          second column.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item
 * @param[out] x The pointer to a variable that stores the item's <b>row number</b>
 * @param[out] y The pointer to a variable that stores the item's <b>column number</b>
 */
EAPI void                          elm_gengrid_item_pos_get(const Elm_Object_Item *it, unsigned int *x, unsigned int *y);

/**
 * @brief Sets the manner in which the items grid is filled within a given gengrid widget.
 *
 * @details This sets the fill state of the whole grid of items of a gengrid
 *          within its given viewport. By default, this value is @c false, meaning
 *          that if the first line of items grid isn't filled, the items are
 *          centered with the alignment.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @param[in] fill If @c true if it is filled, otherwise @c false
 *
 * @see elm_gengrid_filled_get()
 *
 */
EAPI void                          elm_gengrid_filled_set(Evas_Object *obj, Eina_Bool fill);

/**
 * @brief Gets the manner in which the items grid is filled within a given gengrid widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use @c NULL pointers for the alignment values you're not
 *          interested in, they are ignored by the function.
 *
 * @param[in] obj The gengrid object
 * @return @c EINA_TRUE if filled is on,
 *         otherwise @c EINA_FALSE if it is off
 *
 * @see elm_gengrid_align_set() for more details
 */
EAPI Eina_Bool                     elm_gengrid_filled_get(const Evas_Object *obj);

/**
 * @brief Sets the gengrid select mode.
 *
 * @since_tizen 2.3
 *
 * @remarks elm_gengrid_select_mode_set() changes the item select mode in the gengrid widget.
 *          - ELM_OBJECT_SELECT_MODE_DEFAULT : Items only call their selection func and
 *            callback when they get selected for the first time. Any further clicks
 *            do nothing, unless you set the always select mode.
 *          - ELM_OBJECT_SELECT_MODE_ALWAYS :  This means that, even if selected,
 *            every click calls the selected callbacks.
 *          - ELM_OBJECT_SELECT_MODE_NONE : This turns off the ability to select items
 *            entirely and they neither appear selected nor call selected
 *            callback functions.
 *
 * @param[in] obj The gengrid object
 * @param[in] mode The select mode
 *
 * @see elm_gengrid_select_mode_get()
 */
EAPI void elm_gengrid_select_mode_set(Evas_Object *obj, Elm_Object_Select_Mode mode);

/**
 * @brief Gets the gengrid select mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return The select mode
 *         (If getting the mode fails, it returns @c ELM_OBJECT_SELECT_MODE_MAX)
 *
 * @see elm_gengrid_select_mode_set()
 */
EAPI Elm_Object_Select_Mode elm_gengrid_select_mode_get(const Evas_Object *obj);

/**
 * @brief Sets whether the gengrid items should be highlighted when an item is selected.
 *
 * @details This turns on/off the highlight effect when items are selected and
 *          they either get or do not get highlighted. The selected and clicked
 *          callback functions are still called.
 *
 * @since_tizen 2.3
 *
 * @remarks Highlighting is enabled by default.
 *
 * @param[in] obj The gengrid object
 * @param[in] highlight If @c EINA_TRUE highlighting is enabled,
 *                  otherwise @c EINA_FALSE to disable it
 *
 * @see elm_gengrid_highlight_mode_get()
 */

EAPI void                          elm_gengrid_highlight_mode_set(Evas_Object *obj, Eina_Bool highlight);

/**
 * @brief Gets whether the gengrid items should be highlighted when an item is selected.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @return @c EINA_TRUE indicates that items can be highlighted,
 *         otherwise @c EINA_FALSE indicates that they can't \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_gengrid_highlight_mode_set()
 */

EAPI Eina_Bool                     elm_gengrid_highlight_mode_get(const Evas_Object *obj);

/**
 * @brief Sets the gengrid item's select mode.
 *
 * @since_tizen 2.3
 *
 * @remarks elm_gengrid_select_mode_set() changes the item's select mode.
 *          - ELM_OBJECT_SELECT_MODE_DEFAULT : Items only call their selection @a func and
 *            callback when they get selected for the first time. Any further clicks
 *            do nothing, unless you set the always select mode.
 *          - ELM_OBJECT_SELECT_MODE_ALWAYS : This means that, even if selected,
 *            every click calls the selected callbacks.
 *          - ELM_OBJECT_SELECT_MODE_NONE : This turns off the ability to select the item
 *            entirely and they neither appear selected nor call selected
 *            callback functions.
 *          - ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY : This applies the no-finger-size rule
 *            with @c ELM_OBJECT_SELECT_MODE_NONE. The no-finger-size rule makes an item
 *            smaller than the lower limit. Clickable objects should be bigger than
 *            the human touch point device (your finger) for some touch or
 *            small screen devices. Once it is enabled, the item can shrink beyond the
 *            predefined finger-size value. And the item gets updated.
 *
 * @param[in] it The gengrid item object
 * @param[in] mode The select mode
 *
 * @see elm_gengrid_item_select_mode_get()
 */
EAPI void                          elm_gengrid_item_select_mode_set(Elm_Object_Item *it, Elm_Object_Select_Mode mode);

/**
 * @brief Gets the gengrid item's select mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The gengrid item object
 * @return The select mode
 *         (If the getting mode fails, it returns @c ELM_OBJECT_SELECT_MODE_MAX)
 *
 * @see elm_gengrid_item_select_mode_set()
 */
EAPI Elm_Object_Select_Mode        elm_gengrid_item_select_mode_get(const Elm_Object_Item *it);

/**
 * @brief Gets the item that is at the x, y canvas coordinates.
 *
 * @details This returns the item at the given coordinates (which is canvas
 *          relative, not object-relative). If an item is at that coordinate,
 *          that item handle is returned, and if @a xposret is not @c NULL, the
 *          integer it points to is set to either @c -1, @c 0, or @c 1, depending on whether
 *          the coordinate is on the left portion of that item (-1), in the
 *          middle section (0), or on the right part (1).
 *          If @a yposret is not @c NULL, the
 *          integer it points to is set to either @c -1, @c 0, or @c 1, depending on whether
 *          the coordinate is at the upper portion of that item (-1), in the
 *          middle section (0), or at the lower part (1). If @c NULL is returned as
 *          an item (no item found there), then posret may indicate @c -1 or @c 1
 *          based on whether the coordinate is above or below the items respectively in
 *          the gengrid.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gengrid object
 * @param[in] x The input x coordinate
 * @param[in] y The input y coordinate
 * @param[out] xposret The position relative to the returned item
 * @param[out] yposret The position relative to the returned item
 * @return The item at the coordinates, otherwise @c NULL if no item is present
 */
EAPI Elm_Object_Item             *elm_gengrid_at_xy_item_get(const Evas_Object *obj, Evas_Coord x, Evas_Coord y, int *xposret, int *yposret);

/**
 * @}
 */
