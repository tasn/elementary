/**
 * @defgroup Inwin Inwin
 * @ingroup Win
 *
 * @image html inwin_inheritance_tree.png
 * @image latex inwin_inheritance_tree.eps
 *
 * An inwin is a window inside a window that is useful for a quick popup.
 * It does not hover.
 *
 * It works by creating an object that occupies the entire window, so it
 * must be created using an @ref Win "elm_win" as the parent only. The inwin
 * object can be hidden or restacked below every other object if it's
 * needed to show what's behind it without destroying it. If this is done,
 * the elm_win_inwin_activate() function can be used to bring it back to
 * full visibility again.
 *
 * There are three styles available in the default theme. These are:
 * @li default: The inwin is sized to take over most of the window it's
 * placed in.
 * @li minimal: The size of the inwin is the minimum size necessary to show
 * its contents.
 * @li minimal_vertical: Horizontally, the inwin takes as much space as
 * possible, but it's sized vertically is the most it needs to fit its
 * contents.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for inner windown objects. It also
 * emits the signals inherited from @ref Layout.
 *
 * @{
 */

/**
 * @brief Adds an inwin to the current window.
 *
 * @since_tizen 2.3
 *
 * @remarks The @a obj used as a parent @b MUST be an @ref Win "Elementary Window".
 *          Never call this function with anything other than the top-most window
 *          as its parameter, unless you are fond of undefined behavior.
 *
 * @remarks After creating the object, the widget sets itself as the resize object
 *          for the window with elm_win_resize_object_add(), so when shown it will
 *          appear to cover almost the entire window (how much of it depends on its
 *          content and the style used). It must not be added into any other container
 *          objects and it need not be moved or resized manually.
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @ingroup Inwin
 */
EAPI Evas_Object *elm_win_inwin_add(Evas_Object *parent);

/**
 * @brief Activates an inwin object, ensuring its visibility.
 *
 * @details This function makes sure that the inwin @a obj is completely visible
 *          by calling evas_object_show() and evas_object_raise() on it, to bring it
 *          to the front. It also sets the keyboard focus to it, which is passed
 *          onto its content.
 *
 * @since_tizen 2.3
 *
 * @remarks The object's theme also receives the signal "elm,action,show" with
 *          source "elm".
 *
 * @param[in] obj The inwin to activate
 *
 * @ingroup Inwin
 */
EAPI void         elm_win_inwin_activate(Evas_Object *obj);

/**
 * @brief Sets the content of an inwin object.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the content object is set, a previously set one is deleted.
 *          If you want to keep that old content object, use the
 *          elm_win_inwin_content_unset() function.
 *
 * @param[in] obj The inwin object
 * @param[in] content The object to set as content
 *
 * @ingroup Inwin
 */
EAPI void         elm_win_inwin_content_set(Evas_Object *obj, Evas_Object *content);

/**
 * @brief Gets the content of an inwin object.
 *
 * @details This returns the content object for this widget.
 *
 * @since_tizen 2.3
 *
 * @remarks The returned object is valid as long as the inwin is still alive and no
 *          other content is set on it. Deleting the object notifies the inwin
 *          about it and this one is left empty.
 *
 * @remarks If you need to remove an inwin's content to be reused somewhere else,
 *          see elm_win_inwin_content_unset().
 *
 * @param[in] obj The inwin object
 * @return The content that is being used
 *
 * @ingroup Inwin
 */
EAPI Evas_Object *elm_win_inwin_content_get(const Evas_Object *obj);

/**
 * @brief Unsets the content of an inwin object.
 *
 * @details This unparents and returns the content object that is set for this widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The inwin object
 * @return The content that is being used
 *
 * @ingroup Inwin
 */
EAPI Evas_Object *elm_win_inwin_content_unset(Evas_Object *obj);

/**
 * @}
 */

