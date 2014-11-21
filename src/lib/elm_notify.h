/**
 * @defgroup Notify Notify
 * @ingroup elm_widget_group
 *
 * @image html notify_inheritance_tree.png
 * @image latex notify_inheritance_tree.eps
 *
 * @brief Display a container in a particular region of the parent(top, bottom,
 *        etc).
 *
 * A timeout can be set to automatically hide the notify widget. This is so
 * that, after an evas_object_show() on a notify object, if a timeout is set
 * on it, it @b automatically gets hidden after that time.
 *
 * Signals that you can add callbacks for are:
 * @li "timeout" - When timeout happens on a notification and it's hidden.
 * @li "block,clicked" - When a click outside of the notification happens.
 *
 * The functions meant to act on it work for mapbuf objects:
 *
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * The default content parts of the notify widget that you can use are:
 * @li @c "default" - The main content of the notify widget.
 *
 * @{
 */

#define ELM_NOTIFY_ALIGN_FILL                   -1.0  /**< Use with elm_notify_align_set() @since 1.8 */

/**
 * @brief Adds a new notify widget to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object                 *elm_notify_add(Evas_Object *parent);

/**
 * @brief Sets the notify parent.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the parent object is set, a previously set one is disconnected
 *          and replaced.
 *
 * @param[in] obj The notify object
 * @param[in] parent The new parent
 */
EAPI void                         elm_notify_parent_set(Evas_Object *obj, Evas_Object *parent);

/**
 * @brief Gets the notify parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The notify object
 * @return The parent
 *
 * @see elm_notify_parent_set()
 */
EAPI Evas_Object                 *elm_notify_parent_get(const Evas_Object *obj);

/**
 * @brief Sets the time interval after which the notify window is going to be
 *        hidden.
 *
 * @details This function sets a timeout and starts the timer controlling when the
 *          notification is hidden. Since calling evas_object_show() on a notification restarts
 *          the timer controlling when the notification is hidden, setting this before the
 *          notification is shown means starting the timer when the notification is
 *          shown.
 *
 * @since_tizen 2.3
 *
 * @remarks Set a value <= @c 0.0 to disable a running timer.
 *
 * @remarks If the value > @c 0.0 and the notification is previously visible, the
 *          timer is started with this value, cancelling any running timer.
 *
 * @param[in] obj The notify object
 * @param[in] timeout The timeout in seconds
 */
EAPI void                         elm_notify_timeout_set(Evas_Object *obj, double timeout);

/**
 * @brief Gets the timeout value (in seconds).
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The notify object
 * @return The timeout in seconds
 *
 * @see elm_notify_timeout_set()
 */
EAPI double                       elm_notify_timeout_get(const Evas_Object *obj);

/**
 * @brief Sets whether events should be passed by a click outside
 *        its area.
 *
 * @since_tizen 2.3
 *
 * @remarks When @c true if the user clicks outside the window the events are caught
 *          by the other widgets, else the events are blocked.
 *
 * @remarks The default value is @c EINA_TRUE.
 *
 * @param[in] obj The notify object
 * @param[in] allow If @c EINA_TRUE events are allowed,
 *              otherwise @c EINA_FALSE if they are not
 */
EAPI void                         elm_notify_allow_events_set(Evas_Object *obj, Eina_Bool allow);

/**
 * @brief Gets true if events are allowed below the notify object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The notify object
 * @return #EINA_TRUE if events are allowed, otherwise #EINA_FALSE.
 *
 * @see elm_notify_allow_events_set()
 */
EAPI Eina_Bool                    elm_notify_allow_events_get(const Evas_Object *obj);

/**
 * @brief Sets the alignment of the notify object.
 *
 * @details This sets the alignment in which the notify appears in its parent.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks To fill the notify box in the parent area, pass the
 *          @c ELM_NOTIFY_ALIGN_FILL to @a horizontal, @a vertical.
 *
 * @param[in] obj The notify object
 * @param[in] horizontal The horizontal alignment of the notification
 * @param[in] vertical The vertical alignment of the notification
 */
EAPI void                         elm_notify_align_set(Evas_Object *obj, double horizontal, double vertical);

/**
 * @brief Gets the alignment of the notify object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The notify object
 * @param[out] horizontal The horizontal alignment of the notification
 * @param[out] vertical The vertical alignment of the notification
 *
 * @see elm_notify_align_set()
 */
EAPI void                         elm_notify_align_get(const Evas_Object *obj, double *horizontal, double *vertical);

/**
 * @}
 */
