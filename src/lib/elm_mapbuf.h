/**
 * @defgroup Mapbuf Mapbuf
 * @ingroup elm_widget_group
 *
 * @image html mapbuf_inheritance_tree.png
 * @image latex mapbuf_inheritance_tree.eps
 *
 * @brief This holds one content object and uses an Evas Map of transformation
 *        points to be later used with this content.
 *
 * So the content is moved, resized, etc as a single image. So it improves
 * performance when you have a complex interface, with a lot of elements,
 * and you need to resize or move it frequently (the content object and its
 * children).
 *
 * The functions meant to act on it works for mapbuf objects:
 *
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * The default content parts of the mapbuf widget that you can use are:
 * @li "default" - The main content of the mapbuf.
 *
 * To enable map, elm_mapbuf_enabled_set() should be used.
 *
 * @{
 */

/**
 * @brief Adds a new mapbuf widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new mapbuf widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new mapbuf widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object                 *elm_mapbuf_add(Evas_Object *parent);

/**
 * @brief Enables or disables the map.
 *
 * @details This enables the map that is set or disables it. On enable, the object
 *          geometry is saved, and the new geometry changes (position and
 *          size) to reflect the map geometry set.
 *
 * @since_tizen 2.3
 *
 * @remarks Also, when enabled, alpha and smooth states are used, so if the
 *          content isn't solid, alpha should be enabled, for example, otherwise
 *          a black rectangle fills the content.
 *
 * @remarks When disabled, the stored map is freed and the geometry prior to
 *          enabling the map is restored.
 *
 *          It's disabled by default.
 *
 * @param[in] obj The mapbuf object
 * @param[in] enabled If @c EINA_TRUE the map is enabled, otherwise @c EINA_FALSE to disable it
 *
 * @see elm_mapbuf_alpha_set()
 * @see elm_mapbuf_smooth_set()
 */
EAPI void                         elm_mapbuf_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Gets a value that indicates whether the map is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The mapbuf object
 * @return @c EINA_TRUE means the map is enabled, otherwise @c EINA_FALSE indicates that it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_mapbuf_enabled_set()
 */
EAPI Eina_Bool                    elm_mapbuf_enabled_get(const Evas_Object *obj);

/**
 * @brief Enables or disables smooth map rendering.
 *
 * @details This sets smoothing for map rendering. If the object is a type that has
 *          its own smoothing settings, then both the smooth settings for this object
 *          and the map must be turned off.
 *
 *          By default smooth maps are enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The mapbuf object
 * @param[in] smooth If @c EINA_TRUE smooth map rendering is enabled,
 *               otherwise @c EINA_FALSE to disable it
 */
EAPI void                         elm_mapbuf_smooth_set(Evas_Object *obj, Eina_Bool smooth);

/**
 * @brief Gets a value that indicates whether smooth map rendering is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The mapbuf object
 * @return @c EINA_TRUE means smooth map rendering is enabled, otherwise @c EINA_FALSE indicates that it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_mapbuf_smooth_set()
 */
EAPI Eina_Bool                    elm_mapbuf_smooth_get(const Evas_Object *obj);

/**
 * @brief Sets or unsets the alpha flag for map rendering.
 *
 * @details This sets the alpha flag for map rendering. If the object is a type that has
 *          its own alpha settings, then this takes precedence. Only image objects
 *          have this currently. It stops alpha blending of the map area, and is
 *          useful if you know the object and/or all the sub-objects is 100% solid.
 *
 *          Alpha is enabled by default.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The mapbuf object
 * @param[in] alpha If @c EINA_TRUE alpha blending is enabled, otherwise @c EINA_FALSE
 *              to disable it
 */
EAPI void                         elm_mapbuf_alpha_set(Evas_Object *obj, Eina_Bool alpha);

/**
 * @brief Gets a value that indicates whether alpha blending is enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The mapbuf object
 * @return @c EINA_TRUE means alpha blending is enabled, otherwise @c EINA_FALSE indicates it's disabled \n
 *         If @a obj is @c NULL, @c EINA_FALSE is returned.
 *
 * @see elm_mapbuf_alpha_set()
 */
EAPI Eina_Bool                    elm_mapbuf_alpha_get(const Evas_Object *obj);

/**
 * @brief Sets the color of a vertex in the mapbuf.
 *
 * @details This sets the color of the vertex in the mapbuf. Colors are linearly
 *          interpolated between vertex points through the mapbuf. Color multiplies
 *          the "texture" pixels (like GL_MODULATE in OpenGL). The default color of
 *          a vertex in a mapbuf is white solid (255, 255, 255, 255) which means it
 *          has no effect on modifying the texture pixels.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The mapbuf object
 * @param[in] idx The index of the point to change, must be smaller than the mapbuf size
 * @param[in] r Red (0 - 255)
 * @param[in] g Green (0 - 255)
 * @param[in] b Blue (0 - 255)
 * @param[in] a Alpha (0 - 255)
 *
 * @see evas_object_map_set()
 */
EAPI void                         elm_mapbuf_point_color_set(Evas_Object *obj, int idx, int r, int g, int b, int a);

/**
 * @brief Gets the color set on a vertex in the mapbuf.
 *
 * @details This gets the color set by elm_mapbuf_point_color_set() on the given vertex
 *          of the mapbuf.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The mapbuf object
 * @param[in] idx The index of the point to get, must be smaller than the map size.
 * @param[out] r A pointer to red
 * @param[out] g A pointer to green
 * @param[out] b A pointer to blue
 * @param[out] a A pointer to alpha
 *
 * @see elm_mapbuf_point_color_set()
 */
EAPI void                         elm_mapbuf_point_color_get(Evas_Object *obj, int idx, int *r, int *g, int *b, int *a);

/**
 * @}
 */
