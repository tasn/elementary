/**
 * @defgroup Bg Background
 * @ingroup elm_widget_group
 *
 * @image html bg_inheritance_tree.png
 * @image latex bg_inheritance_tree.eps
 *
 * @brief Background object, used for setting a solid color, image, or
 * Edje group as a background to a window or any container object.
 *
 * The bg (background) widget is used for setting (solid) background
 * decorations to a window (unless it has transparency enabled) or to
 * any container object. It works just like an image, but has some
 * properties useful to a background, like setting it to tiled,
 * centered, scaled, or stretched.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for background objects.
 *
 * The default content parts of the bg widget that you can use are:
 * @li @c "overlay" - Overlay of the bg.
 *
 * @{
 */

/**
 * @brief Enumeration of identifiers on how a background widget is to display its image,
 *        if it is set to use an image file.
 *
 * @see elm_bg_option_set()
 * @see elm_bg_option_get()
 */
typedef enum
{
   ELM_BG_OPTION_CENTER, /**< Center the background image */
   ELM_BG_OPTION_SCALE, /**< Scale the background image, retaining the aspect ratio */
   ELM_BG_OPTION_STRETCH, /**< Stretch the background image to fill the widget's area */
   ELM_BG_OPTION_TILE, /**< Tile background image at its original size */
   ELM_BG_OPTION_LAST /**< Sentinel value, also used to indicate errors */
} Elm_Bg_Option;

/**
 * @brief Adds a new background to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @ingroup Bg
 */
EAPI Evas_Object                 *elm_bg_add(Evas_Object *parent);

/**
 * @brief Sets the file (image or edje collection) to give life for the
 *        background.
 *
 * @details This sets the image file used in the background object. If the
 *          image comes from an Edje group, it is stretched to completely
 *          fill the background object. If it comes from a traditional image file, it
 *          by default is centered in this widget's area (thus retaining
 *          its aspect), what could lead to some parts being not visible. You
 *          may change the mode of exhibition for a real image file with
 *          elm_bg_option_set().
 *
 * @since_tizen 2.3
 *
 * @remarks Once the image of @a obj is set, a previously set one is
 *          deleted, even if @a file is @c NULL.
 *
 * @remarks This only affects the content of one of the background's
 *          swallow spots, namely @c "elm.swallow.background". If you want to
 *          achieve the @c Layout's file setting behavior, you have to call
 *          that method on this object.
 *
 * @param[in] obj The background object handle
 * @param[in] file The file path
 * @param[in] group The optional key (group in Edje) within the file
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool                    elm_bg_file_set(Evas_Object *obj, const char *file, const char *group);

/**
 * @brief Gets the file (image or edje collection) set on a given background
 *        widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use @c NULL pointers on the file components you're not
 *          interested in, they are ignored by the function.
 *
 * @param[in] obj The background object handle
 * @param[out] file The location to store the requested file's path
 * @param[out] group The group to store the optional key within @a file, @b if
 *              it's an Edje file
 */
EAPI void                         elm_bg_file_get(const Evas_Object *obj, const char **file, const char **group);

/**
 * @brief Sets the mode of display for a given background widget's image
 *
 * @details This sets how the background widget displays its image. This
 *          only works if the elm_bg_file_set() was previously called with
 *          an image file on @a obj. The image can be display tiled, scaled,
 *          centered or stretched.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The background object handle
 * @param[in] option The desired background option (see #Elm_Bg_Option)
 *
 * @see elm_bg_option_get()
 */
EAPI void                         elm_bg_option_set(Evas_Object *obj, Elm_Bg_Option option);

/**
 * @brief Gets the mode of display for a given background widget's image.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The background object handle
 * @return The image displaying mode in use for @a obj or #ELM_BG_OPTION_LAST,
 *         on errors
 *
 * @see elm_bg_option_set()
 */
EAPI Elm_Bg_Option                elm_bg_option_get(const Evas_Object *obj);

/**
 * @brief Sets the color on a given background widget.
 *
 * @details This sets the color used for the background rectangle, in RGB
 *          format. Each color component's range is from @c 0 to @c 255.
 *
 * @since_tizen 2.3
 *
 * @remarks You probably only want to use this function if you haven't
 *          previously called elm_bg_file_set(), so that you just want a solid
 *          color background.
 *
 * @param[in] obj The background object handle
 * @param[in] r The red color component's value
 * @param[in] g The green color component's value
 * @param[in] b The blue color component's value
 *
 * @see elm_bg_color_get()
 */
EAPI void                         elm_bg_color_set(Evas_Object *obj, int r, int g, int b);

/**
 * @brief Gets the color set on a given background widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Use @c NULL pointers on the file components you're not
 *          interested in, they are ignored by the function.
 *
 * @param[in] obj The background object handle
 * @param[out] r The location to store the red color component's value
 * @param[out] g The location to store the green color component's value
 * @param[out] b The location to store the blue color component's value
 *
 * @see elm_bg_color_get()
 */
EAPI void                         elm_bg_color_get(const Evas_Object *obj, int *r, int *g, int *b);

/**
 * @brief Sets the size of the pixmap representation of the image set on a
 *        given background widget.
 *
 * @since_tizen 2.3
 *
 * @remarks This function just makes sense if an image file is set on
 *          @a obj, with elm_bg_file_set().
 *
 * @remarks This function sets a new size for pixmap representation of the
 *          given bg image. It allows for the image to be loaded in advance in the
 *          specified size, reducing the memory usage and load time (for
 *          example, when loading a big image file with its load size set to a
 *          smaller size)
 *
 * @remarks This is just a hint for the underlying system. The real size
 *          of the pixmap may differ depending on the type of image being
 *          loaded, being bigger than requested.
 *
 * @param[in] obj The background object handle
 * @param[in] w The new width of the image pixmap representation
 * @param[in] h The new height of the image pixmap representation
 */
EAPI void                         elm_bg_load_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h);

/**
 * @}
 */
