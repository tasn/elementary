/**
 * @defgroup Scrollhints Scrollhints
 * @ingroup elm_infra_group
 * @brief This group provides functions to handle scroll behavior of Elementary
 *        Widgets.
 *
 * Objects when inside a scroller can scroll, but this may not always be
 * desirable in certain situations. This allows an object to hint to itself
 * and its parents to "not scroll" in one of the following 2 ways. If any child object of a
 * scroller has pushed a scroll freeze or hold then it affects all parent
 * scrollers until all children have released them.
 *
 * 1. To hold on scrolling. This means just flicking and dragging may no
 * longer scroll, but pressing/dragging near the edge of the scroller
 * still scrolls. This is automatically used by the entry object when
 * selecting text.
 *
 * 2. To totally freeze scrolling. This means it stops until
 * popped/released.
 *
 * @{
 */

/**
 * @brief Pushes the scroll hold by @c 1.
 *
 * @details This increments the scroll hold count by one. If it is more than @c 0 it
 *          takes effect on the parents of the indicated object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 */
EAPI void      elm_object_scroll_hold_push(Evas_Object *obj);

/**
 * @brief Pops the scroll hold by @c 1.
 *
 * @details This decrements the scroll hold count by one. If it is more than @c 0 it
 *          takes effect on the parents of the indicated object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 */
EAPI void      elm_object_scroll_hold_pop(Evas_Object *obj);

/**
 * @brief Gets the scroll hold by @c 1.
 *
 * @details This gets the scroll hold count by @c 1.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 * @return The scroll hold count
 */
EAPI int       elm_object_scroll_hold_get(const Evas_Object *obj);

/**
 * @brief Pushes the scroll freeze by @c 1.
 *
 * @details This increments the scroll freeze count by @c 1. If it is more
 *          than @c 0 it takes effect on the parents of the indicated
 *          object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 */
EAPI void      elm_object_scroll_freeze_push(Evas_Object *obj);

/**
 * @brief Pops the scroll freeze by @c 1.
 *
 * @details This decrements the scroll freeze count by @c 1. If it is more
 *          than @c 0 it takes effect on the parents of the indicated
 *          object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 */
EAPI void      elm_object_scroll_freeze_pop(Evas_Object *obj);

/**
 * @brief Gets the scroll freeze by @c 1.
 *
 * @details This gets the scroll freeze count by @c 1.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 * @return The scroll freeze count
 */
EAPI int       elm_object_scroll_freeze_get(const Evas_Object *obj);

/**
 * @brief Locks the scrolling of the given widget (and thus all the parents).
 *
 * @details This locks the given object from scrolling in the X axis (and implicitly
 *          locks all the parent scrollers too from doing the same).
 *
 * @param obj The object
 * @param lock The lock state (@c 1 == locked, @c 0 == unlocked)
 */
EAPI void      elm_object_scroll_lock_x_set(Evas_Object *obj, Eina_Bool lock);

/**
 * @brief Locks the scrolling of the given widget (and thus all the parents).
 *
 * @details This locks the given object from scrolling in the Y axis (and implicitly
 *          locks all the parent scrollers too from doing the same).
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object
 * @param[in] lock The lock state (@c 1 == locked, @c 0 == unlocked)
 */
EAPI void      elm_object_scroll_lock_y_set(Evas_Object *obj, Eina_Bool lock);

/**
 * @brief Gets the scrolling lock of the given widget.
 *
 * @details This gets the lock for the X axis scrolling.
 *
 * @param[in] obj The object
 * @return #EINA_TRUE if it is locked, otherwise #EINA_FALSE
 */
EAPI Eina_Bool elm_object_scroll_lock_x_get(const Evas_Object *obj);

/**
 * @brief Gets the scrolling lock of the given widget.
 *
 * @details This gets the lock for the Y axis scrolling.
 *
 * @param[in] obj The object
 * @return #EINA_TRUE if it is locked, otherwise #EINA_FALSE
 */
EAPI Eina_Bool elm_object_scroll_lock_y_get(const Evas_Object *obj);

/**
 * @}
 */
