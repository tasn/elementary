/**
 * @internal
 * @defgroup Frame Frame
 * @ingroup Elementary
 *
 * @image html frame_inheritance_tree.png
 * @image latex frame_inheritance_tree.eps
 *
 * @image html img/widget/frame/preview-00.png
 * @image latex img/widget/frame/preview-00.eps
 *
 * @brief Frame is a widget that holds some content and has a title.
 *
 * The default look is a frame with a title, but Frame supports multiple
 * styles:
 * @li default
 * @li pad_small
 * @li pad_medium
 * @li pad_large
 * @li pad_huge
 * @li outdent_top
 * @li outdent_bottom
 *
 * Of all these styles only default shows the title.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for frame objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "clicked" - The user has clicked the frame's label.
 *
 * The default content parts of the frame widget that you can use are:
 * @li "default" - Content of the frame.
 *
 * The default text parts of the frame widget that you can use are:
 * @li "default" - Label of the frame.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * @{
 */

/**
 * @brief Adds a new frame to the parent.
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @ingroup Frame
 */
EAPI Evas_Object                 *elm_frame_add(Evas_Object *parent);

/**
 * @brief Toggles autocollapsing of a frame.
 *
 * @remarks When @a enable is @c EINA_TRUE, clicking a frame's label collapses the frame
 *          vertically, shrinking it to the height of the label.
 *          By default, this is DISABLED.
 *
 * @param[in] obj The frame
 * @param[in] autocollapse The boolean value that indicates whether to enable autocollapse
 *
 * @ingroup Frame
 */
EAPI void elm_frame_autocollapse_set(Evas_Object *obj, Eina_Bool autocollapse);

/**
 * @brief Determines autocollapsing of a frame.
 *
 * @remarks When this returns @c EINA_TRUE, clicking a frame's label collapses the frame
 *          vertically, shrinking it to the height of the label.
 *          By default, this is DISABLED.
 *
 * @param[in] obj The frame
 * @return The boolean value that indicates whether autocollapse is enabled
 *
 * @ingroup Frame
 */
EAPI Eina_Bool elm_frame_autocollapse_get(const Evas_Object *obj);

/**
 * @brief Manually collapses a frame without animations.
 *
 * @remarks Use this to toggle the collapsed state of a frame, bypassing animations.
 *
 * @param[in] obj The frame
 * @param[in] collapse If @c true the frame collapses, otherwise @c false to expand it
 *
 * @ingroup Frame
 */
EAPI void elm_frame_collapse_set(Evas_Object *obj, Eina_Bool collapse);

/**
 * @brief Determines the collapse state of a frame.
 *
 * @remarks Use this to determine the collapse state of a frame.
 *
 * @param[in] obj The frame
 * @return @c true if collapsed, otherwise @c false
 *
 * @ingroup Frame
 */
EAPI Eina_Bool elm_frame_collapse_get(const Evas_Object *obj);

/**
 * @brief Manually collapses a frame with animations.
 *
 * @remarks Use this to toggle the collapsed state of a frame, triggering animations.
 *
 * @param[in] obj The frame
 * @param[in] collapse If @c true the frame collapses, otherwise @c false to expand it
 *
 * @ingroup Frame
 */
EAPI void elm_frame_collapse_go(Evas_Object *obj, Eina_Bool collapse);

/**
 * @}
 */
