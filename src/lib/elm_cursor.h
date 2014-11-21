/**
 * @defgroup Cursors Cursors
 * @ingroup elm_infra_group
 * @brief The Elementary cursor is an internal smart object used to
 *        customize the mouse cursor displayed over objects (or
 *        widgets).
 *
 * In the most common scenario, the cursor decoration
 * comes from the graphical @b engine Elementary that is running.
 * Those engines may provide different decorations for cursors,
 * and Elementary provides functions to choose them (think of X11
 * cursors, as an example).
 *
 * By default, Elementary searches cursors only from engine.
 * There's also the possibility of, besides using engine provided
 * cursors, to also use the ones coming from Edje theme files. Both
 * globally and per widget, Elementary makes it possible for one to
 * make the cursors lookup to be held on engines only or on
 * Elementary's theme file, too. To set the cursor's hot spot,
 * two data items should be added to the cursor's theme: "hot_x" and
 * "hot_y", that are the offset from the upper-left corner of the cursor
 * (coordinates (0,0)).
 *
 * @{
 */

/**
 * @brief Sets the cursor to be shown when the mouse is over the object.
 *
 * @details This sets the cursor that is displayed when the mouse is over the
 *          object. The object can have only one cursor set to it, so if
 *          this function is called twice for an object, the previous set
 *          is unset.
 *
 *          If using X cursors, a definition of all the valid cursor names
 *          is listed on Elementary_Cursors.h. If an invalid name is set
 *          the default cursor is used.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object being set as a cursor
 * @param[in] cursor The cursor name to be used
 */
EAPI void        elm_object_cursor_set(Evas_Object *obj, const char *cursor);

/**
 * @brief Gets the cursor to be shown when the mouse is over the object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object with the cursor already set
 * @return The cursor name
 */
EAPI const char *elm_object_cursor_get(const Evas_Object *obj);

/**
 * @brief Unsets the cursor for an object.
 *
 * @details This unsets the cursor for an object, and sets the cursor to default if the mouse
 *          is over this object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The target object
 * @see elm_object_cursor_set()
 */
EAPI void        elm_object_cursor_unset(Evas_Object *obj);

/**
 * @brief Sets a different style for this object cursor.
 *
 * @remarks Before you set a style you should define a cursor with
 *          elm_object_cursor_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj An object with the cursor already set
 * @param[in] style The theme style to use (default, transparent, ...)
 */
EAPI void        elm_object_cursor_style_set(Evas_Object *obj, const char *style);

/**
 * @brief Gets the style for this object cursor.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj An object with the cursor already set
 * @return style The theme style in use, defaults to "default" \n
 *               If the object does not have a cursor set, then @c NULL is returned.
 */
EAPI const char *elm_object_cursor_style_get(const Evas_Object *obj);

/**
 * @brief Sets whether the cursor set should be searched on the theme or should use
 *        the theme provided by the engine, only.
 *
 * @remarks Before you set engine_only you should define a cursor with
 *          elm_object_cursor_set(). By default it only looks for cursors
 *          provided by the engine.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj An object with the cursor already set
 * @param[in] theme_search The boolean value to define if cursors should be looked for only
 *                     from the theme provided by the engine or should be searched on the widget's theme as well
 */
EAPI void elm_object_cursor_theme_search_enabled_set(Evas_Object *obj, Eina_Bool theme_search);

/**
 * @brief Gets the cursor engine only usage for this object cursor.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj An object with the cursor already set
 * @return engine_only A boolean value to define if cursors should be
 *                     looked for only from the theme provided by the engine or should be searched on
 *                     the widget's theme as well \n
 *                     If the object does not have a cursor set, then @c EINA_FALSE is returned.
 */
EAPI Eina_Bool elm_object_cursor_theme_search_enabled_get(const Evas_Object *obj);

/**
 * @}
 */
