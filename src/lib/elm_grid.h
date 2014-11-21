/**
 * @defgroup Grid Grid
 * @ingroup elm_container_group
 *
 * @image html grid_inheritance_tree.png
 * @image latex grid_inheritance_tree.eps
 *
 * @brief The grid is a grid layout widget that lays out a series of children
 *        as a fixed "grid" of widgets using a given percentage of the grid
 *        width and height each using the child object.
 *
 * The Grid uses a "Virtual resolution" that is stretched to fill the grid
 * widgets size itself. The default is 100 x 100, so that means the
 * position and sizes of children are effectively percentages (0 to 100)
 * of the width or height of the grid widget.
 *
 * @{
 */

/**
 * @brief Adds a new grid to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object *elm_grid_add(Evas_Object *parent);

/**
 * @brief Sets the virtual size of the grid.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The grid object
 * @param[in] w The virtual width of the grid
 * @param[in] h The virtual height of the grid
 */
EAPI void         elm_grid_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h);

/**
 * @brief Gets the virtual size of the grid.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The grid object
 * @param[out] w A pointer to an integer to store the virtual width of the grid
 * @param[out] h A pointer to an integer to store the virtual height of the grid
 */
EAPI void         elm_grid_size_get(const Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Packs the child at a given position and size.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The grid object
 * @param[in] subobj The child to pack
 * @param[in] x The virtual x coordinate at which to pack it
 * @param[in] y The virtual y coordinate at which to pack it
 * @param[in] w The virtual width at which to pack it
 * @param[in] h The virtual height at which to pack it
 */
EAPI void         elm_grid_pack(Evas_Object *obj, Evas_Object *subobj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

/**
 * @brief Unpacks a child from a grid object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The grid object
 * @param[in] subobj The child to unpack
 */
EAPI void         elm_grid_unpack(Evas_Object *obj, Evas_Object *subobj);

/**
 * @brief Removes all child objects from a grid object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The grid object
 * @param[in] clear If @c true it deletes the just removed children,
 *              otherwise @c false
 */
EAPI void         elm_grid_clear(Evas_Object *obj, Eina_Bool clear);

/**
 * @brief Sets packing of an existing child at a given position and size.
 *
 * @since_tizen 2.3
 *
 * @param[in] subobj The child to set packing of
 * @param[in] x The virtual x coordinate at which to pack it
 * @param[in] y The virtual y coordinate at which to pack it
 * @param[in] w The virtual width at which to pack it
 * @param[in] h The virtual height at which to pack it
 */
EAPI void         elm_grid_pack_set(Evas_Object *subobj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

/**
 * @brief Gets packing of a child.
 *
 * @since_tizen 2.3
 *
 * @param[in] subobj The child to query
 * @param[out] x A pointer to an integer to store the virtual x coordinate
 * @param[out] y A pointer to an integer to store the virtual y coordinate
 * @param[out] w A pointer to an integer to store the virtual width
 * @param[out] h A pointer to an integer to store the virtual height
 */
EAPI void         elm_grid_pack_get(Evas_Object *subobj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Gets the list of the children for the grid.
 *
 * @since_tizen 2.3
 *
 * @remarks This is a duplicate of the list kept by the grid internally.
 *          It's up to the user to destroy it when it no longer needs it.
 *          It's possible to remove objects from the grid when walking through this
 *          list, but these removals won't be reflected on it.
 *
 * @param[in] obj The grid object
 * @return The children list
 */
EAPI Eina_List *elm_grid_children_get(const Evas_Object *obj);

/**
 * @}
 */
