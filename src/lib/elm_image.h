/**
 * @defgroup Image Image
 * @ingroup elm_widget_group
 *
 * @image html image_inheritance_tree.png
 * @image latex image_inheritance_tree.eps
 *
 * @brief Image widget allows one to load and display an @b image
 *        file on it, be it from a disk file or from a memory region.
 *
 * Exceptionally, one may also load an Edje group as the contents of the image.
 * In this case, though, most of the functions of the image API act as a no-op.
 *
 * An Elementary image object is a direct realization of elm-image-class.
 *
 * One can tune various properties of the image, like:
 * - pre-scaling,
 * - smooth scaling,
 * - orientation,
 * - aspect ratio during resizes, etc.
 *
 * An image object may also be made a valid source and destination for
 * drag and drop actions, through the elm_image_editable_set() call.
 *
 * Signals that you can add callbacks for are:
 *
 * @li @c "drop" - This is called when a user has dropped an image
 *                 typed object onto the object in question, the
 *                 event info argument is the path to that image file.
 * @li @c "clicked" - This is called when a user has clicked the image.
 *
 * @{
 */

/**
 * @brief Enumeration that defines the possible orientation options for elm_image_orient_set().
 *
 * @image html elm_image_orient_set.png
 * @image latex elm_image_orient_set.eps "elm image orient set" width=\textwidth
 */
typedef enum
{
   ELM_IMAGE_ORIENT_NONE = 0, /**< No orientation change */
   ELM_IMAGE_ORIENT_0 = 0, /**< No orientation change */
   ELM_IMAGE_ROTATE_90 = 1, /**< Rotate 90 degrees clockwise */
   ELM_IMAGE_ROTATE_180 = 2, /**< Rotate 180 degrees clockwise */
   ELM_IMAGE_ROTATE_270 = 3, /**< Rotate 90 degrees counter-clockwise (i.e. 270 degrees clockwise) */
   ELM_IMAGE_FLIP_HORIZONTAL = 4, /**< Flip image horizontally */
   ELM_IMAGE_FLIP_VERTICAL = 5, /**< Flip image vertically */
   ELM_IMAGE_FLIP_TRANSPOSE = 6, /**< Flip the image along the y = (width - x) line (bottom-left to top-right) */
   ELM_IMAGE_FLIP_TRANSVERSE = 7 /**< Flip the image along the y = x line (top-left to bottom-right) */
} Elm_Image_Orient;

/**
 * @brief Adds a new image to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @see elm_image_file_set()
 */
EAPI Evas_Object     *elm_image_add(Evas_Object *parent);

/**
 * @brief Sets a location in the memory to be used as an image object's source
 *        bitmap.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks This function is handy when the contents of an image file are
 *          mapped into the memory, for example.
 *
 * @remarks The @a format string should be something like @c "png", @c "jpg",
 *          @c "tga", @c "tiff", @c "bmp" etc, when provided (@c NULL, on the
 *          contrary). This improves the loader performance as it tries the
 *          "correct" loader first, before trying a range of other possible
 *          loaders until one succeeds.
 *
 * @param[in] obj The image object
 * @param[in] img The binary data that is used as an image source
 * @param[in] size The size of the binary data blob @a img
 * @param[in] format The (Optional) expected format of @a img bytes
 * @param[in] key The optional indexing key of @a img to be passed to the
 *            image loader (eg. if @a img is a memory-mapped EET file)
 *
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 */
EAPI Eina_Bool             elm_image_memfile_set(Evas_Object *obj, const void *img, size_t size, const char *format, const char *key);

/**
 * @brief Sets the file that is used as the image's source.
 *
 * @details This function triggers the Edje file case based on the
 *          extension of the @a file string (expect @c ".edj", for this
 *          case). If one wants to force this type of file independent of the
 *          extension, elm_image_file_edje_set() must be used, instead.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @param[in] file The path to the file that is used as an image source
 * @param[in] group The group that the image belongs to, in case it's an
 *              EET (including Edje case) file
 *
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 *
 * @see elm_image_file_get()
 */
EAPI Eina_Bool        elm_image_file_set(Evas_Object *obj, const char *file, const char *group);

/**
 * @brief Gets the file that is used as an image.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @param[out] file The path to the file
 * @param[out] group The group that the image belongs to, in edje file
 *
 * @see elm_image_file_set()
 */
EAPI void             elm_image_file_get(const Evas_Object *obj, const char **file, const char **group);

/**
 * @brief Sets the smooth effect for an image.
 *
 * @details This sets the scaling algorithm to be used when scaling the image. Smooth
 *          scaling provides a better resulting image, but is slower.
 *
 * @since_tizen 2.3
 *
 * @remarks The smooth scaling should be disabled when making animations that change
 *          the image size, since it is faster. Animations that don't require
 *          resizing of the image can keep the smooth scaling enabled (even if the
 *          image is already scaled, since the scaled image is cached).
 *
 * @param[in] obj The image object
 * @param[in] smooth If @c EINA_TRUE smooth scaling should be used, otherwise @c EINA_FALSE \n
 *               Default is @c EINA_TRUE.
 *
 * @see elm_image_smooth_get()
 */
EAPI void             elm_image_smooth_set(Evas_Object *obj, Eina_Bool smooth);

/**
 * @brief Gets the smooth effect for an image.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @return @c EINA_TRUE if smooth scaling is enabled, otherwise @c EINA_FALSE
 *
 * @see elm_image_smooth_get()
 */
EAPI Eina_Bool        elm_image_smooth_get(const Evas_Object *obj);

/**
 * @brief Gets the current size of the image.
 *
 * @since_tizen 2.3
 *
 * @remarks This is the real size of the image, not the size of the object.
 *
 * @param[in] obj The image object
 * @param[out] w The pointer to the store width, otherwise @c NULL
 * @param[out] h The pointer to the store height, otherwise @c NULL
 */
EAPI void             elm_image_object_size_get(const Evas_Object *obj, int *w, int *h);

/**
 * @brief Disables scaling of this object.
 *
 * @details This function disables scaling of elm_image widget through the
 *          function elm_object_scale_set(). However, this does not affect the widget
 *          size/resize in any way. For that effect, take a look at
 *          elm_image_resizable_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @param[in] no_scale If @c EINA_TRUE the object is not scalable, otherwise @c EINA_FALSE \n
 *                 Default is @c EINA_FALSE.
 *
 * @see elm_image_no_scale_get()
 * @see elm_image_resizable_set()
 * @see elm_object_scale_set()
 */
EAPI void             elm_image_no_scale_set(Evas_Object *obj, Eina_Bool no_scale);

/**
 * @brief Gets whether scaling is disabled on the object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @return @c EINA_TRUE if scaling is disabled, otherwise @c EINA_FALSE
 *
 * @see elm_image_no_scale_set()
 */
EAPI Eina_Bool        elm_image_no_scale_get(const Evas_Object *obj);

/**
 * @brief Sets whether the object is (up/down) resizeable.
 *
 * @details This function limits the image resize ability. If @a size_up is set to
 *          @c EINA_FALSE, the object can't have its height or width resized to a value
 *          higher than the original image size. Same is valid for @a size_down.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @param[in] size_up The boolean value to set if the object is resizeable up \n
 *                Default is @c EINA_TRUE.
 * @param[in] size_down The boolean value to set if the object is resizeable down \n
 *                  Default is @c EINA_TRUE.
 *
 * @see elm_image_resizable_get()
 */
EAPI void             elm_image_resizable_set(Evas_Object *obj, Eina_Bool size_up, Eina_Bool size_down);

/**
 * @brief Gets whether the object is (up/down) resizable.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @param[out] size_up The boolean value to set if the object is resizable up
 * @param[out] size_down The boolean to set if the object is resizable down
 *
 * @see elm_image_resizable_set()
 */
EAPI void             elm_image_resizable_get(const Evas_Object *obj, Eina_Bool *size_up, Eina_Bool *size_down);

/**
 * @brief Sets whether the image fills the entire object area, when keeping the aspect ratio.
 *
 * @since_tizen 2.3
 *
 * @remarks When the image should keep its aspect ratio even if resized to another
 *          aspect ratio, there are two possibilities to resize it: keep the entire
 *          image inside the limits of the height and width of the object (@a fill_outside
 *          is @c EINA_FALSE) or let the extra width or height go outside the object,
 *          and the image fills the entire object (@a fill_outside is @c EINA_TRUE).
 *
 * @remarks This option has no effect if
 *          elm_image_aspect_fixed_set() is set to @c EINA_FALSE.
 *
 * @param[in] obj The image object
 * @param[in] fill_outside If @c EINA_TRUE the object is filled outside, otherwise @c EINA_FALSE \n
 *                     Default is @c EINA_FALSE.
 *
 * @see elm_image_fill_outside_get()
 * @see elm_image_aspect_fixed_set()
 */
EAPI void             elm_image_fill_outside_set(Evas_Object *obj, Eina_Bool fill_outside);

/**
 * @brief Gets whether the object is filled outside.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @return @c EINA_TRUE if the object is filled outside, otherwise @c EINA_FALSE
 *
 * @see elm_image_fill_outside_set()
 */
EAPI Eina_Bool        elm_image_fill_outside_get(const Evas_Object *obj);

/**
 * @brief Enables or disables preloading of the image.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @param[in] disabled If @c EINA_TRUE preloading is disabled, otherwise @c EINA_FALSE
 */
EAPI void                  elm_image_preload_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Sets the prescale size for the image.
 *
 * @details This function sets a new size for pixmap representation of the given
 *          image. It allows the image to be loaded in advance in the specified size,
 *          reducing the memory usage and load time when loading a big image with load
 *          size set to a smaller size.
 *
 * @since_tizen 2.3
 *
 * @remarks It's equivalent to the elm_bg_load_size_set() function for bg.
 *
 * @remarks This is just a hint, the real size of the pixmap may differ
 *          depending on the type of image being loaded, being bigger than requested.
 *
 * @param[in] obj The image object
 * @param[in] size The prescale size \n
 *             This value is used for both width and height.
 *
 * @see elm_image_prescale_get()
 * @see elm_bg_load_size_set()
 */
EAPI void             elm_image_prescale_set(Evas_Object *obj, int size);

/**
 * @brief Gets the prescale size for the image.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @return The prescale size
 *
 * @see elm_image_prescale_set()
 */
EAPI int              elm_image_prescale_get(const Evas_Object *obj);

/**
 * @brief Sets the image orientation.
 *
 * @details This function allows to rotate or flip the given image.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @param[in] orient The image orientation @ref Elm_Image_Orient \n
 *               Default is #ELM_IMAGE_ORIENT_NONE.
 *
 * @see elm_image_orient_get()
 * @see @ref Elm_Image_Orient
 */
EAPI void             elm_image_orient_set(Evas_Object *obj, Elm_Image_Orient orient);

/**
 * @brief Gets the image orientation.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @return The image orientation @ref Elm_Image_Orient
 *
 * @see elm_image_orient_set()
 * @see @ref Elm_Image_Orient
 */
EAPI Elm_Image_Orient elm_image_orient_get(const Evas_Object *obj);

/**
 * @brief Makes the image 'editable'.
 *
 * @since_tizen 2.3
 *
 * @remarks This means the image is a valid drag target for drag and drop, and can be
 *          cut or pasted.
 *
 * @param[in] obj The image object
 * @param[in] set The boolean value to turn on or turn off editability \n
 *            Default is @c EINA_FALSE.
 */
EAPI void             elm_image_editable_set(Evas_Object *obj, Eina_Bool set);

/**
 * @brief Checks whether the image is 'editable'.
 *
 * @since_tizen 2.3
 *
 * @remarks A return value of @c EINA_TRUE means the image is a valid drag target
 *          for drag and drop, and can be cut or pasted.
 *
 * @param[in] obj The image object
 * @return The boolean value to turn on or turn off editability
 */
EAPI Eina_Bool        elm_image_editable_get(const Evas_Object *obj);

/**
 * @brief Gets the inlined image object of the image widget.
 *
 * @details This function allows one to get the underlying @c Evas_Object of type
 *          Image from this elementary widget. It can be useful to do things like get
 *          the pixel data, save the image to a file, etc.
 *
 * @since_tizen 2.3
 *
 * @remarks Be careful to not manipulate it, as it is under the control of
 *          elementary.
 *
 * @param[in] obj The image object to get the inlined image from
 * @return The inlined image object, otherwise @c NULL if none exist
 */
EAPI Evas_Object     *elm_image_object_get(const Evas_Object *obj);

/**
 * @brief Set whether the original aspect ratio of the image should be kept on resize.
 *
 * @since_tizen 2.3
 *
 * @remarks The original aspect ratio (width / height) of the image is usually
 *          distorted to match the object's size. Enabling this option retains
 *          this original aspect, and the way that the image is fit into the object's
 *          area depends on the option set by elm_image_fill_outside_set().
 *
 * @param[in] obj The image object
 * @param[in] fixed If @c EINA_TRUE the image should retain the aspect, otherwise @c EINA_FALSE
 *
 * @see elm_image_aspect_fixed_get()
 * @see elm_image_fill_outside_set()
 */
EAPI void             elm_image_aspect_fixed_set(Evas_Object *obj, Eina_Bool fixed);

/**
 * @brief Gets whether the object retains the original aspect ratio.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @return @c EINA_TRUE if the object keeps the original aspect, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool        elm_image_aspect_fixed_get(const Evas_Object *obj);

/**
 * @brief Gets whether an image object supports animation.
 *
 * @details This function returns if this Elementary image object's internal
 *          image can be animated. Currently Evas only supports GIF
 *          animation. If the return value is @c EINA_FALSE, other
 *          @c elm_image_animated_xxx API calls won't work.
 *
 * @since_tizen 2.3
 *
 * @since 1.7
 *
 * @param[in] obj The image object
 * @return @c EINA_TRUE if the image supports animation, otherwise @c EINA_FALSE
 *
 * @see elm_image_animated_set()
 */
EAPI Eina_Bool        elm_image_animated_available_get(const Evas_Object *obj);

/**
 * @brief Sets whether an image object (which supports animation) is to
 *        animate itself.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks An image object, even if it supports animation, is displayed
 *          by default without animation. Call this function with @a animated
 *          set to @c EINA_TRUE to enable its animation. To start or stop the
 *          animation, use elm_image_animated_play_set().
 *
 * @param[in] obj The image object

 * @param[in] animated If @c EINA_TRUE the object is to animate itself, otherwise @c EINA_FALSE \n
 *                 Default is @c EINA_FALSE.
 *
 * @see elm_image_animated_get()
 * @see elm_image_animated_available_get()
 * @see elm_image_animated_play_set()
 */
EAPI void             elm_image_animated_set(Evas_Object *obj, Eina_Bool animated);

/**
 * @brief Gets whether an image object has animation enabled.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 *
 * @return @c EINA_TRUE if the image has animation enabled, otherwise @c EINA_FALSE
 *
 * @see elm_image_animated_set()
 */
EAPI Eina_Bool        elm_image_animated_get(const Evas_Object *obj);

/**
 * @brief Starts or stops an image object's animation.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks To actually start playing any image object's animation, if it
 *          supports it, one must do something like:
 *
 * @code
 * if (elm_image_animated_available_get(img))
 *   {
 *      elm_image_animated_set(img, EINA_TRUE);
 *      elm_image_animated_play_set(img, EINA_TRUE);
 *   }
 * @endcode
 *
 * @remarks elm_image_animated_set() enables animation on the image, <b>but does
 *          not start it yet</b>. This is the function one uses to start and
 *          stop animations on image objects.
 *
 * @param[in] obj The image object
 * @param[in] play If @c EINA_TRUE animation is started, otherwise @c EINA_FALSE \n
 *             Default is @c EINA_FALSE.
 *
 * @see elm_image_animated_available_get()
 * @see elm_image_animated_set()
 * @see elm_image_animated_play_get()
 */
EAPI void             elm_image_animated_play_set(Evas_Object *obj, Eina_Bool play);

/**
 * @brief Gets whether an image object is under animation.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The image object
 * @return @c EINA_TRUE if the image is being animated, otherwise @c EINA_FALSE
 *
 * @see elm_image_animated_play_get()
 */
EAPI Eina_Bool        elm_image_animated_play_get(const Evas_Object *obj);

/**
 * @}
 */
