/**
 * @defgroup Table Table
 * @ingroup elm_container_group
 *
 * @image html table_inheritance_tree.png
 * @image latex table_inheritance_tree.eps
 *
 * @brief A container widget to arrange other widgets in a table where items
 *        can span multiple columns or rows - even overlap (and then be raised
 *        or lowered accordingly to adjust stacking if they do overlap).
 *
 * The row and column count is not fixed. The table widget adjusts itself when
 * subobjects are added to it dynamically.
 *
 * The most common way to use a table is:
 * @code
 * table = elm_table_add(win);
 * evas_object_show(table);
 * elm_table_padding_set(table, space_between_columns, space_between_rows);
 * elm_table_pack(table, table_content_object, x_coord, y_coord, colspan, rowspan);
 * elm_table_pack(table, table_content_object, next_x_coord, next_y_coord, colspan, rowspan);
 * elm_table_pack(table, table_content_object, other_x_coord, other_y_coord, colspan, rowspan);
 * @endcode
 *
 * @{
 */

/**
 * @brief Adds a new table to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @ingroup Table
 */
EAPI Evas_Object *elm_table_add(Evas_Object *parent);

/**
 * @brief Sets the homogeneous layout in the table.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] homogeneous The boolean value to set if the layout is homogeneous in the
 *                    table (@c EINA_TRUE = homogeneous,  @c EINA_FALSE = no homogeneous)
 *
 * @ingroup Table
 */
EAPI void      elm_table_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous);

/**
 * @brief Gets the current table homogeneous mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The table object
 * @return The boolean that indicates if the layout is homogeneous in the table
 *         (@c EINA_TRUE = homogeneous,  @c EINA_FALSE = no homogeneous)
 *
 * @ingroup Table
 */
EAPI Eina_Bool elm_table_homogeneous_get(const Evas_Object *obj);

/**
 * @brief Sets padding between the cells.
 *
 * @since_tizen 2.3
 *
 * @remarks Default value is @c 0.
 *
 * @param[in] obj The layout object
 * @param[in] horizontal The horizontal padding
 * @param[in] vertical The vertical padding
 *
 * @ingroup Table
 */
EAPI void      elm_table_padding_set(Evas_Object *obj, Evas_Coord horizontal, Evas_Coord vertical);

/**
 * @brief Gets the padding between cells.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[out] horizontal The horizontal padding
 * @param[out] vertical The vertical padding
 *
 * @ingroup Table
 */
EAPI void      elm_table_padding_get(const Evas_Object *obj, Evas_Coord *horizontal, Evas_Coord *vertical);

/**
 * @brief Adds a subobject on the table with the coordinates passed.
 *
 * @since_tizen 2.3
 *
 * @remarks All positioning inside the table is relative to rows and columns, so
 *          a value of @c 0 for @a x and @a y, means the top left cell of the table, and a
 *          value of @c 1 for @a w and @a h means @a subobj only takes that @c 1 cell.
 *
 * @remarks Note that columns and rows only guarantee 16bit unsigned values at best.
 *          That means that col + colspan AND row + rowspan must fit inside 16bit
 *          unsigned values cleanly. You have been warned once that values exceed 15bit
 *          storage, and attempting to use values that are not able to fit in 16bits
 *          result in failure.
 *
 * @param[in] obj The table object
 * @param[in] subobj The subobject to be added to the table
 * @param[in] col The column number
 * @param[in] row The row number
 * @param[in] colspan The column span
 * @param[in] rowspan The row span
 *
 * @ingroup Table
 */
EAPI void      elm_table_pack(Evas_Object *obj, Evas_Object *subobj, int col, int row, int colspan, int rowspan);

/**
 * @brief Removes the child from the table.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The table object
 * @param[in] subobj The subobject
 *
 * @ingroup Table
 */
EAPI void      elm_table_unpack(Evas_Object *obj, Evas_Object *subobj);

/**
 * @brief Removes all child objects from a table object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The table object
 * @param[in] clear If @c true, the children are deleted,
 *              otherwise @c false to just remove them from the table
 *
 * @ingroup Table
 */
EAPI void      elm_table_clear(Evas_Object *obj, Eina_Bool clear);

/**
 * @brief Sets the packing location of an existing child of the table.
 *
 * @since_tizen 2.3
 *
 * @remarks Modifies the position of an object already in the table.
 *
 * @remarks All positioning inside the table is relative to rows and columns, so
 *          a value of @c 0 for @a x and @a y, means the top left cell of the table, and a
 *          value of @c 1 for @a w and @a h means @a subobj only takes that @c 1 cell.
 *
 * @param[in] subobj The subobject to be modified in the table
 * @param[in] x The row number
 * @param[in] y The column number
 * @param[in] w The row span
 * @param[in] h The column span
 *
 * @ingroup Table
 */
EAPI void      elm_table_pack_set(Evas_Object *subobj, int x, int y, int w, int h);

/**
 * @brief Gets the packing location of an existing child of the table.
 *
 * @since_tizen 2.3
 *
 * @param[in] subobj The subobject to be modified in the table
 * @param[out] x The row number
 * @param[out] y The column number
 * @param[out] w The row span
 * @param[out] h The column span
 *
 * @see elm_table_pack_set()
 *
 * @ingroup Table
 */
EAPI void      elm_table_pack_get(Evas_Object *subobj, int *x, int *y, int *w, int *h);

/**
 * @}
 */
