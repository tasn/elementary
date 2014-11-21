/**
 * @defgroup Panes Panes
 * @ingroup elm_widget_group
 *
 * @image html panes_inheritance_tree.png
 * @image latex panes_inheritance_tree.eps
 *
 * @image html img/panes.png
 * @image latex img/panes.eps "panes" width=\textwidth
 *
 * @brief The panes widget adds a draggable bar between two contents.
 *        When dragged this bar resizes contents' size.
 *
 * Panes can be displayed vertically or horizontally, and contents
 * size proportion can be customized (homogeneous by default).
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for panes objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "press" - The panes have been pressed (button isn't released yet).
 * - @c "unpressed" - The panes are released after being pressed.
 * - @c "clicked" - The panes have been clicked.
 * - @c "clicked,double" - The panes have been double clicked.
 *
 * Available styles for it:
 * - @c "default"
 *
 * The default content parts of the panes widget that you can use are:
 * @li "left" - Leftside content of the panes.
 * @li "right" - Rightside content of the panes.
 *
 * If panes are displayed vertically, left content is displayed on
 * top.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * @{
 */

/**
 * @brief Adds a new panes widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new panes widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new panes widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object                 *elm_panes_add(Evas_Object *parent);

/**
 * @brief Sets whether the left and right panes resize homogeneously.
 *
 * @since_tizen 2.3
 *
 * @remarks By default panes are resized homogeneously.
 *
 * @param[in] obj The panes object
 * @param[in] fixed If @c EINA_TRUE @a obj resizes the left and right panes @b homogeneously,
 *              otherwise @c EINA_FALSE to make use of the values specified in
 *              elm_panes_content_left_size_set() and
 *              elm_panes_content_right_size_set()
 *              to resize the left and right panes
 *
 * @see elm_panes_fixed_get()
 * @see elm_panes_content_left_size_set()
 * @see elm_panes_content_right_size_set()
 */
EAPI void                         elm_panes_fixed_set(Evas_Object *obj, Eina_Bool fixed);

/**
 * @brief Gets the resize mode for the panes of a given panes widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panes object
 * @return @c EINA_TRUE if @a obj is set to be resized @b homogeneously,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_panes_fixed_set()
 * @see elm_panes_content_left_size_get()
 * @see elm_panes_content_right_size_get()
 */
EAPI Eina_Bool                    elm_panes_fixed_get(const Evas_Object *obj);

/**
 * @brief Gets the size proportion of panes widget's left side.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panes object
 * @return The float value between @c 0.0 and @c 1.0 representing the size proportion
 *         of the left side
 *
 * @see elm_panes_content_left_size_set()
 */
EAPI double                       elm_panes_content_left_size_get(const Evas_Object *obj);

/**
 * @brief Sets the size proportion of the panes widget's left side.
 *
 * @since_tizen 2.3
 *
 * @remarks By default it's homogeneous, i.e., both sides have the same size.
 *
 * @remarks If something different is required, it can be set with this function.
 *          For example, if the left content should be displayed over
 *          75% of the panes size, @a size should be passed as @c 0.75.
 *          This way, the right content is resized to 25% of the panes size.
 *
 * @remarks If displayed vertically, left content is displayed at the top, and
 *          right content at the bottom.
 *
 * @remarks This proportion changes when the user drags the panes bar.
 *
 * @param[in] obj The panes object
 * @param[in] size The value between @c 0.0 and @c 1.0 representing the size proportion
 *             of the left side
 *
 * @see elm_panes_content_left_size_get()
 */
EAPI void                         elm_panes_content_left_size_set(Evas_Object *obj, double size);

/**
 * @brief Gets the size proportion of the panes widget's right side.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panes object
 * @return The float value between @c 0.0 and @c 1.0 representing the size proportion
 *         of the right side
 *
 * @see elm_panes_content_right_size_set()
 */
EAPI double                       elm_panes_content_right_size_get(const Evas_Object *obj);

/**
 * @brief Sets the size proportion of the panes widget's right side.
 *
 * @since_tizen 2.3
 *
 * @remarks By default it's homogeneous, i.e., both sides have the same size.
 *
 * @remarks If something different is required, it can be set with this function.
 *          For example, if the right content should be displayed over
 *          75% of the panes size, @a size should be passed as @c 0.75.
 *          This way, the left content is resized to 25% of the panes size.
 *
 * @remarks If displayed vertically, the left content is displayed at the top, and
 *          the right content at the bottom.
 *
 * @remarks This proportion changes when the user drags the panes bar.
 *
 * @param[in] obj The panes object
 * @param[in] size The values between @c 0.0 and @c 1.0 representing the size proportion
 *             of the right side
 *
 * @see elm_panes_content_right_size_get()
 */
EAPI void                         elm_panes_content_right_size_set(Evas_Object *obj, double size);


/**
 * @brief Sets the orientation of a given panes widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this function to change how your panes are to be
 *          disposed: vertically or horizontally.
 *
 *          By default it's displayed horizontally.
 *
 * @param[in] obj The panes object
 * @param[in] horizontal If @c EINA_TRUE @a obj is @b horizontal,
 *                   otherwise @c EINA_FALSE to make it @b vertical
 *
 * @see elm_panes_horizontal_get()
 */
EAPI void                         elm_panes_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Gets the orientation of a given panes widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panes object
 * @return @c EINA_TRUE, if @a obj is set to be @b horizontal,
 *         otherwise @c EINA_FALSE if it's @b vertical (and on errors)
 *
 * @see elm_panes_horizontal_set()
 */
EAPI Eina_Bool                    elm_panes_horizontal_get(const Evas_Object *obj);

/**
 * @}
 */
