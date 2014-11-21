/**
 * @internal
 * @defgroup Photo Photo
 * @ingroup elm_widget_group
 *
 * @image html photo_inheritance_tree.png
 * @image latex photo_inheritance_tree.eps
 *
 * @brief The Elementary photo widget is intended for displaying a photo,
 *        for eg., a person's image (contact).
 *
 * Simple, yet with a very specific purpose. It has a decorative frame around
 * the inner image itself, on the default theme. If and while no photo is set
 * on it, it displays a person icon, indicating it's a photo placeholder.
 *
 * This widget relies on an internal @ref Icon, so that the APIs of
 * these two widgets are similar (drag and drop is also possible here,
 * for example).
 *
 * Signals that you can add callbacks for are:
 * - @c "clicked" - This is called when a user has clicked the photo.
 * - @c "drag,start" - One has started dragging the inner image out of the
 *                     photo's frame.
 * - @c "drag,end" - One has dropped the dragged image somewhere.
 *
 * @{
 */

/**
 * @brief Addw a new photo to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object *elm_photo_add(Evas_Object *parent);

/**
 * @brief Sets the file that is used as the photo widget's image.
 *
 * @since_tizen 2.3
 *
 * @remarks Use @c NULL on a @a file to set the photo widget back to it's
 *          initial state, which indicates "no photo".
 *
 * @param[in] obj The photo object
 * @param[in] file The path to the file that is used as the @a obj image
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool elm_photo_file_set(Evas_Object *obj, const char *file);

/**
 * @brief Sets the file that is used as a thumbnail in the photo.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photo object
 * @param[in] file The path to the file that is used as the thumbnail
 * @param[in] group The key used in case of an EET file
 */
EAPI void      elm_photo_thumb_set(const Evas_Object *obj, const char *file, const char *group);

/**
 * @brief Sets the size that is used on the photo.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photo object
 * @param[in] size The size of the photo
 */
EAPI void      elm_photo_size_set(Evas_Object *obj, int size);

/**
 * @brief Sets whether the photo should be completely visible.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photo object
 * @param[in] fill If @c true the photo is completely visible,
 *             otherwise @c false
 */
EAPI void      elm_photo_fill_inside_set(Evas_Object *obj, Eina_Bool fill);

/**
 * @brief Sets editability of the photo.
 *
 * @since_tizen 2.3
 *
 * @remarks An editable photo can be dragged to or from, and can be cut or
 *          pasted too. Note that pasting an image or dropping an item on
 *          the image deletes the existing content.
 *
 * @param[in] obj The photo object
 * @param[in] set The boolean value that sets or clears editability
 */
EAPI void      elm_photo_editable_set(Evas_Object *obj, Eina_Bool set);

/**
 * @brief Sets whether the original aspect ratio of the photo should be kept on resize.
 *
 * @remarks The original aspect ratio (width / height) of the photo is usually
 *          distorted to match the object's size. Enabling this option fixes
 *          this original aspect, and the way the photo is fit into
 *          the object's area.
 *
 * @param obj The photo object
 * @param fixed If @c EINA_TRUE the photo should fix the aspect,
 *              otherwise @c EINA_FALSE
 *
 * @see elm_photo_aspect_fixed_get()
 */
EAPI void             elm_photo_aspect_fixed_set(Evas_Object *obj, Eina_Bool fixed);

/**
 * @brief Gets whether the object keeps the original aspect ratio.
 *
 * @param obj The photo object
 * @return @c EINA_TRUE if the object keeps the original aspect, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool        elm_photo_aspect_fixed_get(const Evas_Object *obj);

/**
 * @}
 */
