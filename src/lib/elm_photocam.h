/**
 * @defgroup Photocam Photocam
 * @ingroup elm_widget_group
 *
 * @image html photocam_inheritance_tree.png
 * @image latex photocam_inheritance_tree.eps
 *
 * @brief Photocam is a widget meant specifically for displaying
 *        high-resolution digital camera photos, giving speedy feedback
 *        (fast * load), zooming and panning as well as fitting logic, all with
 *        low memory footprint.
 *
 * It is entirely focused on @b jpeg images, and takes advantage of the
 * properties of the jpeg format (via Evas loader features in the jpeg loader).
 *
 * Signals that you can add callbacks for are:
 * @li @c "clicked" - This is called when a user has clicked the photo
 *        without dragging around.
 * @li @c "press" - This is called when a user has pressed down on the
 *        photo.
 * @li @c "longpressed" - This is called when a user has pressed down
 *        on the photo for a long time without dragging around.
 * @li @c "clicked,double" - This is called when a user has
 *        double-clicked the photo.
 * @li @c "load" - Photo load begins.
 * @li @c "loaded" - This is called when the image file load is
 *        completed for the first view (low resolution blurry version).
 * @li @c "load,detail" - Detailed photo data load begins.
 * @li @c "loaded,detail" - This is called when the image file load is
 *        completed with the detailed image data (full resolution
 *        needed).
 * @li @c "zoom,start" - Zoom animation started.
 * @li @c "zoom,stop" - Zoom animation stopped.
 * @li @c "zoom,change" - Zoom changed when using the auto zoom mode.
 * @li @c "scroll" - The content has been scrolled (moved).
 * @li @c "scroll,anim,start" - Scrolling animation has started.
 * @li @c "scroll,anim,stop" - Scrolling animation has stopped.
 * @li @c "scroll,drag,start" - Dragging the contents around has started.
 * @li @c "scroll,drag,stop" - Dragging the contents around has stopped.
 *
 * This widget implements the elm-scrollable-interface
 * interface, so that all (non-deprecated) functions for the base @ref
 * Scroller widget also work for photocam objects.
 *
 * Some calls on the photocam's API are marked as @b deprecated, as
 * they just wrap the scrollable widget's counterpart functions. Use
 * the ones mentioned for each case of deprecation here.
 * Eventually, the deprecated ones are discarded (next
 * major release).
 *
 * @{
 */

/**
 * @brief Enumeration that defines the types of zoom available.
 */
typedef enum
{
   ELM_PHOTOCAM_ZOOM_MODE_MANUAL = 0, /**< Zoom controlled normally by elm_photocam_zoom_set */
   ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT, /**< Zoom until the photo fits in the photocam */
   ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL, /**< Zoom until the photo fills the photocam */
   ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT_IN, /**< Zoom in until the photo fits in the photocam */
   ELM_PHOTOCAM_ZOOM_MODE_LAST
} Elm_Photocam_Zoom_Mode;

/**
 * @brief Adds a new Photocam object.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object           *elm_photocam_add(Evas_Object *parent);

/**
 * @brief Sets the photo file to be shown.
 *
 * @details This sets (and shows) the specified file (with a relative or absolute
 *          path) and returns a load error (same error that
 *          evas_object_image_load_error_get() returns). The image changes and
 *          adjusts its size at this point and begins a background load process for this
 *          photo that at some time in the future is displayed at the full
 *          quality needed.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @param[in] file The photo file
 * @return The return error (see EVAS_LOAD_ERROR_NONE, EVAS_LOAD_ERROR_GENERIC)
 */
EAPI Evas_Load_Error        elm_photocam_file_set(Evas_Object *obj, const char *file);

/**
 * @brief Gets the path of the current image file.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @return The path
 *
 * @see elm_photocam_file_set()
 */
EAPI const char            *elm_photocam_file_get(const Evas_Object *obj);

/**
 * @brief Sets the zoom level of the photo.
 *
 * @details This sets the zoom level. @c 1 is 1:1 pixel for pixel. @c 2 is 2:1
 *          (that is 2x2 photo pixels displays as 1 on-screen pixel). 4:1 is
 *          4x4 photo pixels as 1 screen pixel, and so on. The @a zoom parameter must
 *          be greater than @c 0. It is suggested to stick to powers of @c 2. (1, 2, 4, 8,
 *          16, 32, etc.).
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @param[in] zoom The zoom level to set
 */
EAPI void                   elm_photocam_zoom_set(Evas_Object *obj, double zoom);

/**
 * @brief Gets the zoom level of the photo.
 *
 * @details This returns the current zoom level of the photocam object. Note that if
 *          you set the fill mode to a value other than @c ELM_PHOTOCAM_ZOOM_MODE_MANUAL
 *          (which is the default), the zoom level may be changed at any time by the
 *          photocam object itself to account for the photo size and the photocam viewport
 *          size.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @return The current zoom level
 *
 * @see elm_photocam_zoom_set()
 * @see elm_photocam_zoom_mode_set()
 */
EAPI double                 elm_photocam_zoom_get(const Evas_Object *obj);

/**
 * @brief Sets the zoom mode.
 *
 * @details This sets the zoom mode to manual or one of the several automatic levels.
 *          Manual (@c ELM_PHOTOCAM_ZOOM_MODE_MANUAL) means that zoom is set manually by
 *          elm_photocam_zoom_set() and stays at that level until changed by code
 *          or until the zoom mode is changed. This is the default mode. The Automatic
 *          modes allow the photocam object to automatically adjust the zoom mode
 *          based on its properties. @c ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT adjusts the zoom so that
 *          the photo fits EXACTLY inside the scroll frame with no pixels outside this
 *          region. @c ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL is similar but ensures that no
 *          pixels within the frame are left unfilled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @param[in] mode The desired mode
 */
EAPI void                   elm_photocam_zoom_mode_set(Evas_Object *obj, Elm_Photocam_Zoom_Mode mode);

/**
 * @brief Gets the zoom mode.
 *
 * @details This gets the current zoom mode of the photocam object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @return The current zoom mode
 *
 * @see elm_photocam_zoom_mode_set()
 */
EAPI Elm_Photocam_Zoom_Mode elm_photocam_zoom_mode_get(const Evas_Object *obj);

/**
 * @brief Gets the current image pixel's width and height.
 *
 * @details This gets the current photo pixel's width and height (for the original).
 *          The size is returned in the integers @a w and @a h that are pointed
 *          to.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @param[out] w A pointer to the width
 * @param[out] h A pointer to the height
 */
EAPI void                   elm_photocam_image_size_get(const Evas_Object *obj, int *w, int *h);

/**
 * @brief Gets the region of the image that is currently shown.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @param[out] x A pointer to the X-coordinate of the region
 * @param[out] y A pointer to the Y-coordinate of the region
 * @param[out] w A pointer to the width
 * @param[out] h A pointer to the height
 *
 * @see elm_photocam_image_region_show()
 * @see elm_photocam_image_region_bring_in()
 */
EAPI void                   elm_photocam_image_region_get(const Evas_Object *obj, int *x, int *y, int *w, int *h);

/**
 * @brief Sets the viewed region of the image.
 *
 * @since_tizen 2.3
 *
 * @remarks This shows the region of the image without using animation.
 *
 * @param[in] obj The photocam object
 * @param[in] x The X-coordinate of the region in the image's original pixels
 * @param[in] y The Y-coordinate of the region in the image's original pixels
 * @param[in] w The width of the region in the image's original pixels
 * @param[in] h The height of the region in the image's original pixels
 */
EAPI void                   elm_photocam_image_region_show(Evas_Object *obj, int x, int y, int w, int h);

/**
 * @brief Brings in the viewed portion of the image.
 *
 * @since_tizen 2.3
 *
 * @remarks This shows the region of the image using animation.
 *
 * @param[in] obj The photocam object
 * @param[in] x The X-coordinate of the region in the image's original pixels
 * @param[in] y The Y-coordinate of the region in the image's original pixels
 * @param[in] w The width of the region in the image's original pixels
 * @param[in] h The height of the region in the image's original pixels
 */
EAPI void                   elm_photocam_image_region_bring_in(Evas_Object *obj, int x, int y, int w, int h);

/**
 * @brief Sets the paused state for the photocam.
 *
 * @details This sets the paused state to on(@c EINA_TRUE) or off (@c EINA_FALSE) for
 *          the photocam. The default is off. This stops zooming using animation on
 *          zoom level changes and changes instantly. This stops any existing
 *          animations that are running.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @param[in] paused The pause state to set
 */
EAPI void                   elm_photocam_paused_set(Evas_Object *obj, Eina_Bool paused);

/**
 * @brief Gets the paused state for the photocam.
 *
 * @details This gets the current paused state for the photocam object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @return The current paused state
 *
 * @see elm_photocam_paused_set()
 */
EAPI Eina_Bool              elm_photocam_paused_get(const Evas_Object *obj);

/**
 * @brief Gets the internal low-resolution image used for the photocam.
 *
 * @details This gets the internal image object inside the photocam. Do not modify it. It
 *          is for inspection only, and hooking callbacks to, nothing else. It may be
 *          deleted at any time.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @return The internal image object handle, otherwise @c NULL if none exist
 */
EAPI Evas_Object           *elm_photocam_internal_image_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets the photocam scrolling bouncing.
 *
 * @param[in] obj The photocam object
 * @param[in] h_bounce If @c EINA_TRUE horizontal bouncing is enabled,
 *                 otherwise @c EINA_FALSE to disable it
 * @param[in] v_bounce If @c EINA_TRUE vertical bouncing is enabled
 *                 otherwise @c EINA_FALSE to disable it
 * @deprecated Use elm_scroller_bounce_set() instead.
 */
EINA_DEPRECATED EAPI void   elm_photocam_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce);

/**
 * @internal
 *
 * @brief Gets the photocam scrolling bouncing.
 *
 * @param[in] obj The photocam object
 * @param[out] h_bounce If @c EINA_TRUE horizontal bouncing is enabled,
 *                 otherwise @c EINA_FALSE to disable it
 * @param[out] v_bounce If @c EINA_TRUE vertical bouncing is enabled
 *                 otherwise @c EINA_FALSE to disable it
 *
 * @deprecated Use elm_scroller_bounce_get() instead.
 *
 * @see elm_photocam_bounce_set()
 */
EINA_DEPRECATED EAPI void   elm_photocam_bounce_get(const Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce);

/**
 * @brief Sets the gesture state for the photocam.
 *
 * @details This sets the gesture state to on(@c EINA_TRUE) or off (@c EINA_FALSE) for
 *          the photocam. The default is off. This starts multi touch zooming.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @param[in] gesture The gesture state to set
 */
EAPI void                  elm_photocam_gesture_enabled_set(Evas_Object *obj, Eina_Bool gesture);

/**
 * @brief Gets the gesture state for the photocam.
 *
 * @details This gets the current gesture state for the photocam object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The photocam object
 * @return The current gesture state
 *
 * @see elm_photocam_gesture_enabled_set()
 */
EAPI Eina_Bool             elm_photocam_gesture_enabled_get(const Evas_Object *obj);

/**
 * @}
 */
