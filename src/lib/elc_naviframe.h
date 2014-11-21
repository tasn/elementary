/**
 * @defgroup Naviframe Naviframe
 * @ingroup elm_widget_group
 *
 * @image html naviframe_inheritance_tree.png
 * @image latex naviframe_inheritance_tree.eps
 *
 * @brief Naviframe stands for navigation frame. It's a views manager
 * for applications.
 *
 * A naviframe holds views (or pages) as its items. Those items are
 * organized in a stack, so that new items get pushed on top of the
 * old, and only the topmost view is displayed at one time. The
 * transition between views is animated, depending on the theme
 * applied to the widget.
 *
 * Naviframe views hold spaces to various elements, which are:
 * - back button, used to navigate to previous views,
 * - next button, used to navigate to next views in the stack,
 * - title label,
 * - sub-title label,
 * - title icon and
 * - content area.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for naviframe objects.
 *
 * Because this widget is a layout, one places content on those areas
 * by using elm_layout_content_set() on the right swallow part names
 * expected for each, which are:
 * @li @c "default" - The main content of the current page
 * @li @c "icon" - An icon in the title area of the current page
 * @li @c "prev_btn" - A button of the current page to go to the
 *                     previous page
 * @li @c "next_btn" - A button of the current page to go to the next
 *                     page
 *
 * For text, elm_layout_text_set() will work here on:
 * @li @c "default" - Title label in the title area of the current
 *                    page
 * @li @c "subtitle" - Sub-title label in the title area of the
 *                     current page
 *
 * Most of those content objects can be passed at the time of an item
 * creation (see elm_naviframe_item_push()).
 *
 * Naviframe items can have different styles, which affect the
 * transition between views, for example. On the default theme, two of
 * them are supported:
 * - @c "basic"   - views are switched sliding horizontally, one after
 *                  the other
 * - @c "overlap" - like the previous one, but the previous view stays
 *                  at its place and is overlapped by the new
 *
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li @c "transition,finished" - When the transition is finished in
 *                                changing the item
 * @li @c "title,transition,finished" - When the title transition is
 *                                      finished in changing enabled
 *                                      state of the title
 * @li @c "title,clicked" - User clicked title area
 *
 * All the parts, for content and text, described here will also be
 * reachable by naviframe @b items direct calls:
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_part_text_get
 * @li @ref elm_object_item_part_content_set
 * @li @ref elm_object_item_part_content_get
 * @li @ref elm_object_item_part_content_unset
 * @li @ref elm_object_item_signal_emit
 *
 * What happens is that the topmost item of a naviframe will be the
 * widget's target layout, when accessed directly. Items lying below
 * the top one can be interacted with this way.
 *
 * @{
 */

/**
 * @typedef Elm_Naviframe_Item_Pop_Cb
 *
 * @since_tizen 2.3
 *
 * Pop callback called when @c it is going to be popped. @c data is user
 * specific data. If it returns the @c EINA_FALSE in the callback, item popping
 * will be cancelled.
 *
 * @see elm_naviframe_item_pop_cb_set()
 *
 * @since 1.8
 */
typedef Eina_Bool (*Elm_Naviframe_Item_Pop_Cb)(void *data, Elm_Object_Item *it);

/**
 * @brief Add a new Naviframe object to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent Parent object
 * @return New object or @c NULL, if it cannot be created
 */
EAPI Evas_Object     *elm_naviframe_add(Evas_Object *parent);

/**
 * @brief Push a new item to the top of the naviframe stack (and show it).
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @param[in] title_label The label in the title area. The name of the title
 *        label part is "elm.text.title"
 * @param[in] prev_btn The button to go to the previous item. If it is NULL,
 *        then naviframe will create a back button automatically. The name of
 *        the prev_btn part is "elm.swallow.prev_btn"
 * @param[in] next_btn The button to go to the next item. Or It could be just an
 *        extra function button. The name of the next_btn part is
 *        "elm.swallow.next_btn"
 * @param[in] content The main content object. The name of content part is
 *        "elm.swallow.content"
 * @param[in] item_style The current item style name. @c NULL would be default.
 * @return The created item or @c NULL upon failure.
 *
 * The item pushed becomes one page of the naviframe, this item will be
 * deleted when it is popped.
 *
 * @see also elm_naviframe_item_style_set()
 * @see also elm_naviframe_item_insert_before()
 * @see also elm_naviframe_item_insert_after()
 *
 * The following styles are available for this item:
 * @li @c "default"
 */
EAPI Elm_Object_Item *elm_naviframe_item_push(Evas_Object *obj, const char *title_label, Evas_Object *prev_btn, Evas_Object *next_btn, Evas_Object *content, const char *item_style);

/**
 * @brief Insert a new item into the naviframe before item @p before.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @param[in] before The naviframe item to insert before.
 * @param[in] title_label The label in the title area. The name of the title
 *        label part is "elm.text.title"
 * @param[in] prev_btn The button to go to the previous item. If it is NULL,
 *        then naviframe will create a back button automatically. The name of
 *        the prev_btn part is "elm.swallow.prev_btn"
 * @param[in] next_btn The button to go to the next item. Or It could be just an
 *        extra function button. The name of the next_btn part is
 *        "elm.swallow.next_btn"
 * @param[in] content The main content object. The name of content part is
 *        "elm.swallow.content"
 * @param[in] item_style The current item style name. @c NULL would be default.
 * @return The created item or @c NULL upon failure.
 *
 * The item is inserted into the naviframe straight away without any
 * transition operations. This item will be deleted when it is popped.
 *
 * @see also elm_naviframe_item_style_set()
 * @see also elm_naviframe_item_push()
 * @see also elm_naviframe_item_insert_after()
 *
 * The following styles are available for this item:
 * @li @c "default"
 */
EAPI Elm_Object_Item *elm_naviframe_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, const char *title_label, Evas_Object *prev_btn, Evas_Object *next_btn, Evas_Object *content, const char *item_style);

/**
 * @brief Insert a new item into the naviframe after item @p after.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @param[in] after The naviframe item to insert after.
 * @param[in] title_label The label in the title area. The name of the title
 *        label part is "elm.text.title"
 * @param[in] prev_btn The button to go to the previous item. If it is NULL,
 *        then naviframe will create a back button automatically. The name of
 *        the prev_btn part is "elm.swallow.prev_btn"
 * @param[in] next_btn The button to go to the next item. Or It could be just an
 *        extra function button. The name of the next_btn part is
 *        "elm.swallow.next_btn"
 * @param[in] content The main content object. The name of content part is
 *        "elm.swallow.content"
 * @param[in] item_style The current item style name. @c NULL would be default.
 * @return The created item or @c NULL upon failure.
 *
 * The item is inserted into the naviframe straight away without any
 * transition operations. This item will be deleted when it is popped.
 *
 * @see also elm_naviframe_item_style_set()
 * @see also elm_naviframe_item_push()
 * @see also elm_naviframe_item_insert_before()
 *
 * The following styles are available for this item:
 * @li @c "default"
 */
EAPI Elm_Object_Item *elm_naviframe_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, const char *title_label, Evas_Object *prev_btn, Evas_Object *next_btn, Evas_Object *content, const char *item_style);

/**
 * @brief Pop an item that is on top of the stack
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @return @c NULL or the content object(if the
 *         elm_naviframe_content_preserve_on_pop_get is true).
 *
 * This pops an item that is on the top(visible) of the naviframe, makes it
 * disappear, then deletes the item. The item that was underneath it on the
 * stack will become visible.
 *
 * @see also elm_naviframe_content_preserve_on_pop_get()
 * @see also elm_naviframe_item_pop_cb_set()
 */
EAPI Evas_Object     *elm_naviframe_item_pop(Evas_Object *obj);

/**
 * @brief Pop the items between the top and the above one on the given item.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The naviframe item
 */
EAPI void             elm_naviframe_item_pop_to(Elm_Object_Item *it);

/**
 * @brief Promote an item already in the naviframe stack to the top of the stack
 *
 * @since_tizen 2.3
 *
 * @remarks This will take the indicated item and promote it to the top of
 *          the stack as if it had been pushed there. The item must already
 *          be inside the naviframe stack to work.
 *
 * @param[in] it The naviframe item
 */
EAPI void             elm_naviframe_item_promote(Elm_Object_Item *it);

/**
 * @brief preserve the content objects when items are popped.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @param[in] preserve Enable the preserve mode if EINA_TRUE, disable otherwise
 *
 * @see also elm_naviframe_content_preserve_on_pop_get()
 */
EAPI void             elm_naviframe_content_preserve_on_pop_set(Evas_Object *obj, Eina_Bool preserve);

/**
 * @brief Get a value whether preserve mode is enabled or not.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @return If @c EINA_TRUE, preserve mode is enabled
 *
 * @see also elm_naviframe_content_preserve_on_pop_set()
 */
EAPI Eina_Bool        elm_naviframe_content_preserve_on_pop_get(const Evas_Object *obj);

/**
 * @brief Get a top item on the naviframe stack
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @return The top item on the naviframe stack or @c NULL, if the stack is
 *         empty
 */
EAPI Elm_Object_Item *elm_naviframe_top_item_get(const Evas_Object *obj);

/**
 * @brief Get a bottom item on the naviframe stack
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @return The bottom item on the naviframe stack or @c NULL, if the stack is
 *         empty
 */
EAPI Elm_Object_Item *elm_naviframe_bottom_item_get(const Evas_Object *obj);

/**
 * @brief Set an item style
 *
 * @since_tizen 2.3
 *
 * @remarks The following styles are available for this item: @li @c "default"
 *
 * @param[in] it The naviframe item
 * @param[in] item_style The current item style name. @c NULL would be default
 *
 * @see also elm_naviframe_item_style_get()
 */
EAPI void             elm_naviframe_item_style_set(Elm_Object_Item *it, const char *item_style);

/**
 * @brief Get an item style
 *
 * @since_tizen 2.3
 *
 * @param[in] it The naviframe item
 * @return The current item style name
 *
 * @see also elm_naviframe_item_style_set()
 */
EAPI const char      *elm_naviframe_item_style_get(const Elm_Object_Item *it);

/**
 * @brief Enable/Disable the title area with transition effect
 *
 * @since_tizen 2.3
 *
 * @remarks When the title area is disabled, then the controls would be hidden
 *          so as to expand the content area to full-size.
 *
 * @param[in] it The naviframe item
 * @param[in] enabled If @c EINA_TRUE, title area will be enabled, disabled
 *        otherwise
 * @param[in] transition If @c EINA_TRUE, transition effect of the title will be
 *        visible, invisible otherwise
 *
 * @see also elm_naviframe_item_title_enabled_get()
 * @see also elm_naviframe_item_title_visible_set()
 */
EAPI void             elm_naviframe_item_title_enabled_set(Elm_Object_Item *it, Eina_Bool enabled, Eina_Bool transition);

/**
 * @brief Get a value whether title area is enabled or not.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The naviframe item
 * @return If @c EINA_TRUE, title area is enabled
 *
 * @see also elm_naviframe_item_title_enabled_set()
 */
EAPI Eina_Bool        elm_naviframe_item_title_enabled_get(const Elm_Object_Item *it);

/**
 * @brief Set a function to be called when @c it of the naviframe is going to 
 *        be popped.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks Don't set "clicked" callback to the prev button additionally if the
 * function does a exact same logic with this @c func. When hardware back key is
 * pressed then both callbacks will be called.
 *
 * @param[in] it The item to set the callback on
 * @param[in] func the callback function.
 * @param[in] data user data
 */
EAPI void             elm_naviframe_item_pop_cb_set(Elm_Object_Item *it, Elm_Naviframe_Item_Pop_Cb func, void *data);

/**
 * @brief Set creating prev button automatically or not
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @param[in] auto_pushed If @c EINA_TRUE, the previous button(back button) will
 *        be created internally when you pass the @c NULL to the prev_btn
 *        parameter in elm_naviframe_item_push
 *
 * @see also elm_naviframe_item_push()
 */
EAPI void             elm_naviframe_prev_btn_auto_pushed_set(Evas_Object *obj, Eina_Bool auto_pushed);

/**
 * @brief Get a value whether prev button(back button) will be auto pushed or
 *        not.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @return If @c EINA_TRUE, prev button will be auto pushed.
 *
 * @see also elm_naviframe_item_push()
 *           elm_naviframe_prev_btn_auto_pushed_set()
 */
EAPI Eina_Bool        elm_naviframe_prev_btn_auto_pushed_get(const Evas_Object *obj);

/**
 * @brief Get a list of all the naviframe items.
 *
 * @since_tizen 2.3
 *
 * @remarks The returned list MUST be freed.
 *
 * @param[in] obj The naviframe object
 * @return An Eina_List of naviframe items, #Elm_Object_Item,
 *         or @c NULL on failure.
 */
EAPI Eina_List *elm_naviframe_items_get(const Evas_Object *obj) EINA_MALLOC EINA_WARN_UNUSED_RESULT;

/**
 * @brief Set the event enabled when pushing/popping items
 *
 * @since_tizen 2.3
 *
 * @remarks If @c enabled is EINA_TRUE, the contents of the naviframe item will
 *          receives events from mouse and keyboard during view changing such as
 *          item push/pop.
 *
 * @remarks Events will be blocked by calling evas_object_freeze_events_set()
 *          internally. So don't call the API whiling pushing/popping items.
 *
 * @param[in] obj The naviframe object
 * @param[in] enabled Events are received when enabled is @c EINA_TRUE, and
 * ignored otherwise.
 *
 * @see elm_naviframe_event_enabled_get()
 * @see evas_object_freeze_events_set()
 */
EAPI void             elm_naviframe_event_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Get the value of event enabled status.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @return EINA_TRUE, when event is enabled
 *
 * @see elm_naviframe_event_enabled_set()
 */
EAPI Eina_Bool        elm_naviframe_event_enabled_get(const Evas_Object *obj);

/**
 * @brief Simple version of item_push.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The naviframe object
 * @param[in] content The main content object. The name of content part is
 *                    "elm.swallow.content"
 * @return The created item or @c NULL upon failure.
 *
 * @see elm_naviframe_item_push
 */
static inline Elm_Object_Item *
elm_naviframe_item_simple_push(Evas_Object *obj, Evas_Object *content)
{
   Elm_Object_Item *it;
   it = elm_naviframe_item_push(obj, NULL, NULL, NULL, content, NULL);
   elm_naviframe_item_title_enabled_set(it, EINA_FALSE, EINA_FALSE);
   return it;
}

/**
 * @brief Simple version of item_promote.
 *
 * @since_tizen 2.3
 * @param[in] obj The naviframe object
 * @param[in] content The main content object. The name of content part is
 *                    "elm.swallow.content"
 *
 * @see elm_naviframe_item_promote
 */
EAPI void             elm_naviframe_item_simple_promote(Evas_Object *obj, Evas_Object *content);

/**
 * @}
 */
