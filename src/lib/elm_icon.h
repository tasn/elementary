/**
 * @defgroup Icon Icon
 * @ingroup elm_widget_group
 *
 * @image html icon_inheritance_tree.png
 * @image latex icon_inheritance_tree.eps
 *
 * @brief An icon object is used to display standard icon images ("delete",
 *        "edit", "arrows", etc.) or images coming from a custom file (PNG,
 *         JPG, EDJE, etc.), on icon context.
 *
 * The icon image requested can be in the Elementary theme in use, or
 * in the @c freedesktop.org theme paths. It's possible to set the
 * order of preference from where an image is fetched.
 *
 * This widget inherits from the @ref Image one, so that all the
 * functions acting on it also work for icon objects.
 *
 * You should be using an icon, instead of an image, whenever one of
 * the following apply:
 * - You need a @b thumbnail version of an original image.
 * - You need freedesktop.org provided icon images.
 * - You need theme provided icon images (Edje groups).
 *
 * Various calls on the icon's API are marked as @b deprecated, as
 * they just wrap the image counterpart functions. Use the ones we
 * mentioned for each case of deprecation here.
 * Eventually, the deprecated ones are discarded (next major
 * release).
 *
 * Default images provided by Elementary's default theme are described
 * below.
 *
 * These are names for icons that were first intended to be used in
 * toolbars, but can be used in many other places too:
 * @li @c "home"
 * @li @c "close"
 * @li @c "apps"
 * @li @c "arrow_up"
 * @li @c "arrow_down"
 * @li @c "arrow_left"
 * @li @c "arrow_right"
 * @li @c "chat"
 * @li @c "clock"
 * @li @c "delete"
 * @li @c "edit"
 * @li @c "refresh"
 * @li @c "folder"
 * @li @c "file"
 *
 * These are names for icons that were designed to be used in menus
 * (but again, you can use them anywhere):
 * @li @c "menu/home"
 * @li @c "menu/close"
 * @li @c "menu/apps"
 * @li @c "menu/arrow_up"
 * @li @c "menu/arrow_down"
 * @li @c "menu/arrow_left"
 * @li @c "menu/arrow_right"
 * @li @c "menu/chat"
 * @li @c "menu/clock"
 * @li @c "menu/delete"
 * @li @c "menu/edit"
 * @li @c "menu/refresh"
 * @li @c "menu/folder"
 * @li @c "menu/file"
 *
 * And these are names for some media player specific icons:
 * @li @c "media_player/forward"
 * @li @c "media_player/info"
 * @li @c "media_player/next"
 * @li @c "media_player/pause"
 * @li @c "media_player/play"
 * @li @c "media_player/prev"
 * @li @c "media_player/rewind"
 * @li @c "media_player/stop"
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Image :
 * - @c "thumb,done" - elm_icon_thumb_set() has completed with success
 *                     (@since 1.7)
 * - @c "thumb,error" - elm_icon_thumb_set() has failed (@since 1.7)
 *
 * Elementary icon objects support the following API calls:
 * @li elm_object_signal_emit()
 * @li elm_object_signal_callback_add()
 * @li elm_object_signal_callback_del()
 * for emitting and listening to signals on the object, when the
 * internal image comes from an Edje object. This behavior is added
 * unintentionally, though, and is @b deprecated. Expect it to be
 * dropped on future releases.
 *
 * @{
 */

typedef enum
{
   ELM_ICON_NONE,
   ELM_ICON_FILE,
   ELM_ICON_STANDARD
} Elm_Icon_Type;

/**
 * @enum Elm_Icon_Lookup_Order
 * @typedef Elm_Icon_Lookup_Order
 *
 * @brief Enumeration that defines the lookup order used by elm_icon_standard_set(). Should look for icons in the
 *        theme, FDO paths, or both.
 */
typedef enum
{
   ELM_ICON_LOOKUP_FDO_THEME, /**< Icon look up order: freedesktop, theme */
   ELM_ICON_LOOKUP_THEME_FDO, /**< Icon look up order: theme, freedesktop */
   ELM_ICON_LOOKUP_FDO, /**< Icon look up order: freedesktop */
   ELM_ICON_LOOKUP_THEME /**< Icon look up order: theme */
} Elm_Icon_Lookup_Order;

/**
 * @brief Adds a new icon object to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @see elm_image_file_set()
 */
EAPI Evas_Object          *elm_icon_add(Evas_Object *parent);

/**
 * @internal
 *
 * @brief Sets the file that is used as an icon.
 *
 * @remarks The icon image set by this function can be changed by
 *          elm_icon_standard_set().
 *
 * @param obj The icon object
 * @param file The path to the file that is used as an icon image
 * @param group The group that the icon belongs to
 *
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 *
 * @deprecated Use elm_image_file_set() instead.
 *
 * @see elm_icon_file_get()
 */
EINA_DEPRECATED EAPI Eina_Bool             elm_icon_file_set(Evas_Object *obj, const char *file, const char *group);

/**
 * @internal
 *
 * @brief Sets a location in the memory to be used as an icon.
 *
 * @remarks The @a format string should be something like "png", "jpg", "tga",
 *          "tiff", "bmp" etc., if it is provided (@c NULL if not). This improves
 *          the loader performance as it tries the "correct" loader first before
 *          trying a range of other possible loaders until one succeeds.
 *
 * @remarks The icon image set by this function can be changed by
 *          elm_icon_standard_set().
 *
 * @param obj The icon object
 * @param img The binary data that is used as an image
 * @param size The size of the binary data @a img
 * @param format The optional format of @a img to pass to the image loader
 * @param key The optional key of @a img to pass to the image loader (eg. if @a img is an edje file)
 *
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 *
 * @deprecated Use elm_image_memfile_set() instead.
 */
EINA_DEPRECATED EAPI Eina_Bool             elm_icon_memfile_set(Evas_Object *obj, const void *img, size_t size, const char *format, const char *key);

/**
 * @internal
 * @brief Gets the file that is used as an icon.
 *
 * @param obj The icon object
 * @param file The path to the file that is used as the icon image
 * @param group The group that the icon belongs to
 *
 * @see elm_image_file_set()
 *
 * @deprecated Use elm_image_file_get() instead.
 */
EINA_DEPRECATED EAPI void                  elm_icon_file_get(const Evas_Object *obj, const char **file, const char **group);

/**
 * @brief Sets the file that is used, but uses a generated thumbnail.
 *
 * @since_tizen 2.3
 *
 * @remarks This function is like elm_image_file_set() but requires the Ethumb library
 *          support to be enabled successfully with elm_need_ethumb(). When set,
 *          the file indicated has a thumbnail generated and cached on disk for
 *          future use or directly uses an existing cached thumbnail if it
 *          is valid.
 *
 * @param[in] obj The icon object
 * @param[in] file The path to the file that is used as an icon image
 * @param[in] group The group that the icon belongs to
 *
 * @see elm_image_file_set()
 */
EAPI void                  elm_icon_thumb_set(Evas_Object *obj, const char *file, const char *group);

/**
 * @brief Sets the icon by icon standard names.
 *
 * @since_tizen 2.3
 *
 * @remarks For example, freedesktop.org defines the standard icon names such as "home",
 *          "network", etc. There can be different icon sets to match those icon
 *          keys. The @a name given as a parameter is one of these "keys", and is
 *          used to look in the freedesktop.org paths and elementary theme. One can
 *          change the lookup order with elm_icon_order_lookup_set().
 *
 * @remarks If @a name is not found in any of the expected locations and it is the
 *          absolute path of an image file, this image is used.
 *
 * @remarks The icon image set by this function can be changed by
 *          elm_image_file_set().
 *
 * @param[in] obj The icon object
 * @param[in] name The icon name
 *
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 *
 * @see elm_icon_standard_get()
 * @see elm_image_file_set()
 */
EAPI Eina_Bool             elm_icon_standard_set(Evas_Object *obj, const char *name);

/**
 * @brief Gets the icon name set by icon standard names.
 *
 * @since_tizen 2.3
 *
 * @remarks If the icon image is set using elm_image_file_set() instead of
 *          elm_icon_standard_set(), then this function returns @c NULL.
 *
 * @param[in] obj The icon object
 * @return The icon name
 *
 * @see elm_icon_standard_set()
 */
EAPI const char           *elm_icon_standard_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets the smooth scaling for an icon object.
 *
 * @details This sets the scaling algorithm to be used when scaling the icon image. Smooth
 *          scaling provides a better resulting image, but is slower.
 *
 * @remarks The smooth scaling should be disabled when making animations that change
 *          the icon size, since they are faster. Animations that don't require
 *          resizing of the icon can keep the smooth scaling enabled (even if the icon
 *          is already scaled, since the scaled icon image is cached).
 *
 * @param obj The icon object
 * @param smooth If @c EINA_TRUE smooth scaling should be used, otherwise @c EINA_FALSE \n
 *               Default is @c EINA_TRUE.
 *
 * @deprecated Use elm_image_smooth_set() instead.
 *
 * @see elm_icon_smooth_get()
 */
EINA_DEPRECATED EAPI void                  elm_icon_smooth_set(Evas_Object *obj, Eina_Bool smooth);

/**
 * @internal
 *
 * @brief Gets whether smooth scaling is enabled for an icon object.
 *
 * @param obj The icon object
 * @return @c EINA_TRUE if smooth scaling is enabled, otherwise @c EINA_FALSE
 *
 * @deprecated Use elm_image_smooth_get() instead.
 *
 * @see elm_icon_smooth_set()
 */
EINA_DEPRECATED EAPI Eina_Bool             elm_icon_smooth_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Disables scaling of this object.
 *
 * @details This function disables scaling of the icon object through the function
 *          elm_object_scale_set(). However, this does not affect the object
 *          size/resize in any way. For that effect, take a look at
 *          elm_icon_resizable_set().
 *
 * @param obj The icon object
 * @param no_scale If @c EINA_TRUE the object is not scalable, otherwise @c EINA_FALSE \n
 *                 Default is @c EINA_FALSE.
 *
 * @see elm_icon_no_scale_get()
 * @see elm_icon_resizable_set()
 * @see elm_object_scale_set()
 *
 * @deprecated Use elm_image_no_scale_set() instead.
 */
EINA_DEPRECATED EAPI void                  elm_icon_no_scale_set(Evas_Object *obj, Eina_Bool no_scale);

/**
 * @internal
 *
 * @brief Gets whether scaling is disabled on the object.
 *
 * @param obj The icon object
 * @return @c EINA_TRUE if scaling is disabled, otherwise @c EINA_FALSE
 *
 * @see elm_icon_no_scale_set()
 *
 * @deprecated Use elm_image_no_scale_get() instead.
 */
EINA_DEPRECATED EAPI Eina_Bool             elm_icon_no_scale_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets whether the object is (up/down) resizable.
 *
 * @details This function limits the icon object resize ability. If @a size_up is set to
 *          @c EINA_FALSE, the object can't have its height or width resized to a value
 *          higher than the original icon size. Same is valid for @a size_down.
 *
 * @param obj The icon object
 * @param size_up The boolean to set if the object is resizable upwards \n
 *                Default is @c EINA_TRUE.
 * @param size_down The boolean to set if the object is resizable downwards \n
 *                  Default is @c EINA_TRUE.
 *
 * @see elm_icon_resizable_get()
 *
 * @deprecated Use elm_image_resizable_set() instead.
 */
EINA_DEPRECATED EAPI void                  elm_icon_resizable_set(Evas_Object *obj, Eina_Bool size_up, Eina_Bool size_down);

/**
 * @internal
 *
 * @brief Get if the object is (up/down) resizable.
 *
 * @param obj The icon object
 * @param size_up The boolean to set if the object is resizable upwards
 * @param size_down The boolean to set if the object is resizable downwards
 *
 * @see elm_icon_resizable_set()
 *
 * @deprecated Use elm_image_resizable_get() instead.
 */
EINA_DEPRECATED EAPI void                  elm_icon_resizable_get(const Evas_Object *obj, Eina_Bool *size_up, Eina_Bool *size_down);

/**
 * @internal
 *
 * @brief Gets the object's image size.
 *
 * @param obj The icon object
 * @param w The pointer to store the width in
 * @param h The pointer to store the height in
 *
 * @deprecated Use elm_image_object_size_get() instead.
 */
EINA_DEPRECATED EAPI void                  elm_icon_size_get(const Evas_Object *obj, int *w, int *h);

/**
 * @internal
 *
 * @brief Sets whether the icon fills the entire object area.
 *
 * @remarks When the icon object is resized to a different aspect ratio from the
 *          original icon image, the icon image still keeps its aspect. This flag
 *          tells how the image should fill the object's area. They are: keep the
 *          entire icon inside the limits of the height and width of the object (@a
 *          fill_outside is @c EINA_FALSE) or let the extra width or height go outside
 *          the object, and the icon fills the entire object (@a fill_outside
 *          is @c EINA_TRUE).
 *
 * @remarks Unlike @ref Image, there's no option in the icon to set the aspect ratio
 *          retain property to @c false. Thus, the icon image always keeps its
 *          original aspect ratio.
 *
 * @param obj The icon object
 * @param fill_outside If @c EINA_TRUE the object is filled outside, otherwise @c EINA_FALSE \n
 *                     Default is @c EINA_FALSE.
 *
 * @deprecated Use elm_image_fill_outside_set() instead.
 *
 * @see elm_icon_fill_outside_get()
 */
EINA_DEPRECATED EAPI void                  elm_icon_fill_outside_set(Evas_Object *obj, Eina_Bool fill_outside);

/**
 * @internal
 *
 * @brief Gets whether the object is filled outside.
 *
 * @param obj The icon object
 * @return @c EINA_TRUE if the object is filled outside, otherwise @c EINA_FALSE
 *
 * @deprecated Use elm_image_fill_outside_get() instead.
 *
 * @see elm_icon_fill_outside_set()
 */
EINA_DEPRECATED EAPI Eina_Bool             elm_icon_fill_outside_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets the prescale size for the icon.
 *
 * @details This function sets a new size for the pixmap representation of the given
 *          icon. It allows the icon to be loaded in advance in the specified size,
 *          reducing the memory usage and load time when loading a big icon with load
 *          size set to a smaller size.
 *
 * @remarks It's equivalent to the elm_bg_load_size_set() function for bg.
 *
 * @remarks This is just a hint, the real size of the pixmap may differ
 *          depending on the type of icon being loaded, being bigger than requested.
 *
 * @param obj The icon object
 * @param size The prescale size \n
 *             This value is used for both width and height.
 *
 * @deprecated Use elm_image_prescale_set() instead.
 *
 * @see elm_icon_prescale_get()
 * @see elm_bg_load_size_set()
 */
EINA_DEPRECATED EAPI void                  elm_icon_prescale_set(Evas_Object *obj, int size);

/**
 * @internal
 *
 * @brief Gets the prescale size for the icon.
 *
 * @param obj The icon object
 * @return The prescale size
 *
 * @deprecated Use elm_image_prescale_get() instead.
 *
 * @see elm_icon_prescale_set()
 */
EINA_DEPRECATED EAPI int                   elm_icon_prescale_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Gets the image object of the icon. DO NOT MODIFY THIS.
 *
 * @param obj The icon object
 * @return The internal icon object
 *
 * @deprecated Use elm_image_object_get() instead.
 */
EINA_DEPRECATED EAPI Evas_Object          *elm_icon_object_get(Evas_Object *obj);

/**
 * @brief Sets the icon lookup order used by elm_icon_standard_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The icon object
 * @param[in] order The icon lookup order (can be one of
 *              ELM_ICON_LOOKUP_FDO_THEME, ELM_ICON_LOOKUP_THEME_FDO, ELM_ICON_LOOKUP_FDO,
 *              or ELM_ICON_LOOKUP_THEME)
 *
 * @see elm_icon_order_lookup_get()
 * @see Elm_Icon_Lookup_Order
 */
EAPI void                  elm_icon_order_lookup_set(Evas_Object *obj, Elm_Icon_Lookup_Order order);

/**
 * @brief Gets the icon lookup order.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The icon object
 * @return The icon lookup order
 *
 * @see elm_icon_order_lookup_set()
 * @see Elm_Icon_Lookup_Order
 */
EAPI Elm_Icon_Lookup_Order elm_icon_order_lookup_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Enables or disables preloading of the icon.
 *
 * @param obj The icon object
 * @param disabled If @c EINA_TRUE preloading is disabled,
 *                 otherwise @c EINA_FALSE
 *
 * @deprecated Use elm_image_preload_disabled_set() instead.
 */
EINA_DEPRECATED EAPI void  elm_icon_preload_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @internal
 *
 * @brief Gets whether the icon supports animation.
 *
 * @details This returns whether this elm icon's image can be animated. Currently Evas only
 *          supports gif animation. If the return value is @c EINA_FALSE, other
 *          elm_icon_animated_xxx APIs won't work.
 *
 * @param obj The icon object
 * @return @c EINA_TRUE if the icon supports animation,
 *         otherwise @c EINA_FALSE
 *
 * @deprecated Use elm_image_animated_available_get() instead.
 */
EINA_DEPRECATED EAPI Eina_Bool elm_icon_animated_available_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets the animation mode of the icon.
 *
 * @remarks Since the default animation mode is set to @c EINA_FALSE,
 *          the icon is shown without animation. Files like animated GIF files
 *          can animate, and this is supported if you enable animated support
 *          on the icon.
 *          Set it to @c EINA_TRUE when the icon needs to be animated.
 *
 * @param obj The icon object
 * @param animated If @c EINA_TRUE the object does the animation job, otherwise @c EINA_FALSE \n
 *                 Default is @c EINA_FALSE.
 *
 * @deprecated Use elm_image_animated_set() instead.
 */
EINA_DEPRECATED EAPI void  elm_icon_animated_set(Evas_Object *obj, Eina_Bool animated);

/**
 * @internal
 *
 * @brief Gets the animation mode of the icon.
 *
 * @param obj The icon object
 * @return The animation mode of the icon object

 *
 * @deprecated Use elm_image_animated_get() instead.
 *
 * @see elm_icon_animated_set
 */
EINA_DEPRECATED EAPI Eina_Bool elm_icon_animated_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets the animation play mode of the icon.
 *
 * @remarks To play elm icon's animation, set play to @c EINA_TRUE.
 *          For example, you make gif player using this set/get API and click event.
 *          This literally lets you control the current play or paused state. To have
 *          this work with animated GIF files for example, you first, before
 *          setting the file, have to use elm_icon_animated_set() to enable animation
 *          on the icon.
 *
 *          1. Click event occurs.
 *          2. Check play flag using elm_icon_animated_play_get.
 *          3. If the elm icon is playing, set play to @c EINA_FALSE.
 *             Then animation is stopped and vice versa
 *
 * @param obj The icon object
 * @param play If @c EINA_TRUE the object plays animation images, otherwise @c EINA_FALSE
 *             Default is @c EINA_FALSE.
 *
 * @deprecated Use elm_image_animated_play_set() instead.
 */
EINA_DEPRECATED EAPI void  elm_icon_animated_play_set(Evas_Object *obj, Eina_Bool play);

/**
 * @internal
 *
 * @brief Gets the animation play mode of the icon.
 *
 * @param obj The icon object
 * @return The play mode of the icon object
 *
 * @deprecated Use elm_image_animated_play_get() instead.
 *
 * @see elm_icon_animated_play_get
 */
EINA_DEPRECATED EAPI Eina_Bool elm_icon_animated_play_get(const Evas_Object *obj);

/**
 * @internal
 *
 * @brief Sets whether the original aspect ratio of the icon should be kept on resize.
 *
 * @remarks The original aspect ratio (width / height) of the icon is usually
 *          distorted to match the object's size. Enabling this option retains
 *          this original aspect, and the way that the icon is fit into the object's
 *          area depends on the option set by elm_icon_fill_outside_set().
 *
 * @param obj The icon object
 * @param fixed If @c EINA_TRUE the icon should retain the aspect, otherwise @c EINA_FALSE
 *
 * @deprecated Use elm_image_aspect_fixed_set() instead.
 *
 * @see elm_icon_aspect_fixed_get()
 * @see elm_icon_fill_outside_set()
 */
EINA_DEPRECATED EAPI void  elm_icon_aspect_fixed_set(Evas_Object *obj, Eina_Bool fixed);

/**
 * @internal
 *
 * @brief Gets whether the object retains the original aspect ratio.
 *
 * @param obj The icon object
 * @return @c EINA_TRUE if the object keeps the original aspect, otherwise @c EINA_FALSE
 *
 * @deprecated Use elm_image_aspect_fixed_get() instead.
 */
EINA_DEPRECATED EAPI Eina_Bool elm_icon_aspect_fixed_get(const Evas_Object *obj);

/**
 * @}
 */
