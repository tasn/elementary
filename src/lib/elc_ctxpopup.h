/**
 * @defgroup Ctxpopup Ctxpopup
 * @ingroup elm_widget_group
 *
 * @image html ctxpopup_inheritance_tree.png
 * @image latex ctxpopup_inheritance_tree.eps
 *
 * @brief A ctxpopup is a widget that, when shown, pops up a list of items.
 *
 * It automatically chooses an area inside its parent object's view
 * (set via elm_ctxpopup_add() and elm_ctxpopup_hover_parent_set()) to
 * optimally fit into it. In the default theme, it will also point an
 * arrow to it's top left position at the time one shows it. Ctxpopup
 * items have a label and/or an icon. It is intended for a small
 * number of items (hence the use of list, not genlist).
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for context popup objects
 * (@since 1.8).
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "dismissed" - this is called when the outside of ctxpopup was clicked or
 * it's parent area is changed or the language is changed. and then ctxpopup is
 * dismissed.
 *
 * Default content parts of the ctxpopup widget that you can use for are:
 * @li "default" - A content of the ctxpopup
 *
 * Default content parts of the ctxpopup items that you can use for are:
 * @li "icon" - An icon in the title area
 *
 * Default text parts of the ctxpopup items that you can use for are:
 * @li "default" - Title label in the title area
 *
 * Supported elm_object common APIs.
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 *
 * Supported elm_object_item common APIs.
 * @li @ref elm_object_item_disabled_set
 * @li @ref elm_object_item_disabled_get
 * @li @ref elm_object_item_part_text_set
 * @li @ref elm_object_item_part_text_get
 * @li @ref elm_object_item_part_content_set
 * @li @ref elm_object_item_part_content_get
 * @li @ref elm_object_item_signal_emit
 *
 * @{
 */

/**
 * @brief Enumeration of ctxpopup direction type
 */
typedef enum
{
   ELM_CTXPOPUP_DIRECTION_DOWN, /**< ctxpopup show appear below clicked area */
   ELM_CTXPOPUP_DIRECTION_RIGHT, /**< ctxpopup show appear to the right of the clicked area */
   ELM_CTXPOPUP_DIRECTION_LEFT, /**< ctxpopup show appear to the left of the clicked area */
   ELM_CTXPOPUP_DIRECTION_UP, /**< ctxpopup show appear above the clicked area */
   ELM_CTXPOPUP_DIRECTION_UNKNOWN, /**< ctxpopup does not determine it's direction yet*/
} Elm_Ctxpopup_Direction; /**< Direction in which to show the popup */

/**
 * @brief Add a new Ctxpopup object to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent Parent object
 * @return New object or @c NULL, if it cannot be created
 */
EAPI Evas_Object                 *elm_ctxpopup_add(Evas_Object *parent);

/**
 * @brief Set the Ctxpopup's parent object
 *
 * @since_tizen 2.3
 *
 * @remarks elm_ctxpopup_add() will automatically call this function
 *          with its @c parent argument.
 *
 * @param[in] obj The ctxpopup object
 * @param[in] parent The parent to use
 *
 * @see elm_ctxpopup_add()
 * @see elm_hover_parent_set()
 */
EAPI void                         elm_ctxpopup_hover_parent_set(Evas_Object *obj, Evas_Object *parent);

/**
 * @brief Get the Ctxpopup's parent
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The ctxpopup object
 * @return The parent object
 *
 * @see elm_ctxpopup_hover_parent_set() for more information
 */
EAPI Evas_Object                 *elm_ctxpopup_hover_parent_get(const Evas_Object *obj);

/**
 * @brief Clear all items in the given ctxpopup object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 */
EAPI void                         elm_ctxpopup_clear(Evas_Object *obj);

/**
 * @brief Change the ctxpopup's orientation to horizontal or vertical.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 * @param[in] horizontal @c EINA_TRUE for horizontal mode, @c EINA_FALSE for vertical
 */
EAPI void                         elm_ctxpopup_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Get the value of current ctxpopup object's orientation.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 * @return @c EINA_TRUE for horizontal mode, @c EINA_FALSE for vertical mode (or errors)
 *
 * @see elm_ctxpopup_horizontal_set()
 */
EAPI Eina_Bool                    elm_ctxpopup_horizontal_get(const Evas_Object *obj);

/**
 * @brief Add a new item to a ctxpopup object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 * @param[in] label The Label of the new item
 * @param[in] icon Icon to be set on new item
 * @param[in] func Convenience function called when item selected
 * @param[in] data Data passed to @p func
 * @return A handle to the item added or @c NULL, on errors
 *
 * @warning Ctxpopup can't hold both an item list and a content at the same
 * time. When an item is added, any previous content will be removed.
 *
 * @see elm_object_content_set()
 */
EAPI Elm_Object_Item             *elm_ctxpopup_item_append(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Smart_Cb func, const void *data);

/**
 * @brief Set the direction priority of a ctxpopup.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 * @param[in] first 1st priority of direction
 * @param[in] second 2nd priority of direction
 * @param[in] third 3th priority of direction
 * @param[in] fourth 4th priority of direction
 *
 * This functions gives a chance to user to set the priority of ctxpopup
 * showing direction. This doesn't guarantee the ctxpopup will appear in the
 * requested direction.
 *
 * @see Elm_Ctxpopup_Direction
 */
EAPI void                         elm_ctxpopup_direction_priority_set(Evas_Object *obj, Elm_Ctxpopup_Direction first, Elm_Ctxpopup_Direction second, Elm_Ctxpopup_Direction third, Elm_Ctxpopup_Direction fourth);

/**
 * @brief Get the direction priority of a ctxpopup.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 * @param[out] first 1st priority of direction to be returned
 * @param[out] second 2nd priority of direction to be returned
 * @param[out] third 3th priority of direction to be returned
 * @param[out] fourth 4th priority of direction to be returned
 *
 * @see elm_ctxpopup_direction_priority_set() for more information.
 */
EAPI void                         elm_ctxpopup_direction_priority_get(Evas_Object *obj, Elm_Ctxpopup_Direction *first, Elm_Ctxpopup_Direction *second, Elm_Ctxpopup_Direction *third, Elm_Ctxpopup_Direction *fourth);

/**
 * @brief Get the current direction of a ctxpopup.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 * @return current direction of a ctxpopup
 *
 * @warning Once the ctxpopup showed up, the direction would be determined
 */
EAPI Elm_Ctxpopup_Direction       elm_ctxpopup_direction_get(const Evas_Object *obj);

/**
 * @brief Dismiss a ctxpopup object
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The ctxpopup object
 * Use this function to simulate clicking outside the ctxpopup to dismiss it.
 * In this way, the ctxpopup will be hidden and the "clicked" signal will be
 * emitted.
 */
EAPI void                         elm_ctxpopup_dismiss(Evas_Object *obj);

/**
 * brief Get the possibility that the direction would be available
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The ctxpopup object
 * @param[in] direction A direction user wants to check
 *
 * Use this function to check whether input direction is proper for ctxpopup.
 * If ctxpopup cannot be at the direction since there is no sufficient area it can be,
 *
 * @return @c EINA_FALSE if you cannot put it in the direction.
 *         @c EINA_TRUE if it's possible.
 */
EAPI Eina_Bool                    elm_ctxpopup_direction_available_get(Evas_Object *obj, Elm_Ctxpopup_Direction direction);

/**
 * @brief Set whether ctxpopup hide automatically or not when parent of ctxpopup is resized
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Ctxpopup object
 * @param[in] disabled @c EINA_TRUE for not hiding, @c EINA_FALSE for hiding automatically
 *
 * Use this function when user wants ctxpopup not to hide automatically.
 * In default, ctxpopup is dismissed whenever mouse clicked its background area, language is changed,
 * and its parent geometry is updated(changed).
 * Not to hide ctxpopup automatically, disable auto hide function by calling this API,
 * then ctxpopup won't be dismissed in those scenarios.
 *
 * Default value of disabled is EINA_FALSE.
 */
EAPI void                         elm_ctxpopup_auto_hide_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @}
 */
