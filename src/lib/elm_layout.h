/**
 * @defgroup Layout Layout
 * @ingroup elm_widget_group
 *
 * @image html layout_inheritance_tree.png
 * @image latex layout_inheritance_tree.eps
 *
 * @image html img/layout-predefined.png
 * @image latex img/layout-predefined.eps "layout predefined" width=\textwidth
 *
 * A Layout is a direct realization of elm-layout-class.
 *
 * @brief This is a container widget that takes a standard Edje design file and
 *        wraps it very thinly in a widget.
 *
 * An Edje design (theme) file has a very wide range of possibilities to
 * describe the behavior of elements added to the Layout. Check out the Edje
 * documentation and the EDC reference to get more information about what can
 * be done with Edje.
 *
 * Just like @ref List, @ref Box, and other container widgets, any
 * object added to the Layout becomes its child, meaning that it is
 * deleted if the Layout is deleted, moved if the Layout is moved, and so on.
 *
 * The Layout widget can contain as many Contents, Boxes, or Tables as
 * described in its theme file. For instance, objects can be added to
 * different Tables by specifying the respective Table part names. The same
 * is valid for Contents and Boxes.
 *
 * The objects added as the child of the Layout behave as described in the
 * part description where they were added. There are 3 possible types of
 * parts where a child can be added:
 *
 * @section secContent Content (SWALLOW part)
 *
 * Only one object can be added to the @c SWALLOW part (but you still can
 * have many @c SWALLOW parts and one object on each of them). Use the
 * elm_object_content_set/get/unset functions to set, retrieve, and unset
 * objects as content of the @c SWALLOW. After being set to this part, the
 * object size, position, visibility, clipping, and other description
 * properties are totally controlled by the description of the given part
 * (inside the Edje theme file).
 *
 * One can use @c evas_object_size_hint_* functions on the child to have some
 * kind of control over its behavior, but the resulting behavior still
 * depends heavily on the @c SWALLOW part description.
 *
 * The Edje theme also can change the part description, based on signals or
 * scripts running inside the theme. This change can also be animated. All of
 * this affects the child object set as the content accordingly. The object
 * size changes if the part size is changed, it animates move if
 * the part is moving, and so on.
 *
 * The following picture demonstrates a Layout widget with a child object
 * added to its @c SWALLOW:
 *
 * @image html layout_swallow.png
 * @image latex layout_swallow.eps "layout swallow" width=\textwidth
 *
 * @section secBox Box (BOX part)
 *
 * An Edje @c BOX part is very similar to the Elementary @ref Box widget. It
 * allows one to add objects to the box and have them distributed along its
 * area, accordingly as per the specified @a layout property (now by @a layout we
 * mean the chosen layouting design of the Box, not the Layout widget
 * itself).
 *
 * A similar effect for having a box with its position, size, and other things
 * controlled by the Layout theme would be to create an Elementary @ref Box
 * widget and add it as Content in the @c SWALLOW part.
 *
 * The main difference of using the Layout Box is that its behavior, the box
 * properties like layouting format, padding, align, etc. is all
 * controlled by the theme. This means, for example, that a signal could be
 * sent to the Layout theme (with elm_object_signal_emit()) and the theme
 * handles the signal by changing the box padding, or alignment, or both. Using
 * the Elementary @ref Box widget is not necessarily harder or easier, it
 * just depends on the circumstances and requirements.
 *
 * The Layout Box can be used through the @c elm_layout_box_* set of
 * functions.
 *
 * The following picture demonstrates a Layout widget with many child objects
 * added to its @c BOX part:
 *
 * @image html layout_box.png
 * @image latex layout_box.eps "layout box" width=\textwidth
 *
 * @section secTable Table (TABLE part)
 *
 * Just like the @ref secBox, the Layout Table is very similar to the
 * Elementary @ref Table widget. It allows one to add objects to the Table
 * specifying the row and column where the object should be added, and any
 * column or row span if necessary.
 *
 * Again, we could have this design by adding a @ref Table widget to the @c
 * SWALLOW part using elm_object_part_content_set(). The same difference happens
 * here when choosing to use the Layout Table (a @c TABLE part) instead of
 * the @ref Table plus @c SWALLOW part. It's just a matter of convenience.
 *
 * The Layout Table can be used through the @c elm_layout_table_* set of
 * functions.
 *
 * The following picture demonstrates a Layout widget with many child objects
 * added to its @c TABLE part:
 *
 * @image html layout_table.png
 * @image latex layout_table.eps "layout table" width=\textwidth
 *
 * @section secPredef Predefined Layouts
 *
 * Another interesting thing about the Layout widget is that it offers some
 * predefined themes that come with the default Elementary theme. These
 * themes can be set by elm_layout_theme_set(), and can provide some
 * basic functionality depending on the theme used.
 *
 * Most of them already send some signals, some already provide a toolbar or
 * back and next buttons.
 *
 * These are available, predefined theme layouts. All of them have class = @c
 * layout, group = @c application, and style = one of the following options:
 *
 * @li @c toolbar-content - Application with a toolbar and main content area.
 * @li @c toolbar-content-back - Application with a toolbar and main content
 * area having a back button and title area.
 * @li @c toolbar-content-back-next - Application with a toolbar and main
 * content area having back and next buttons and a title area.
 * @li @c content-back - Application with a main content area having a back
 * button and title area.
 * @li @c content-back-next - Application with a main content area having
 * back and next buttons and a title area.
 * @li @c toolbar-vbox - Application with a toolbar and main content area as a
 * vertical box.
 * @li @c toolbar-table - Application with a toolbar and main content area as a
 * table.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_signal_emit
 * @li @ref elm_object_signal_callback_add
 * @li @ref elm_object_signal_callback_del
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * @section layout-signals Emitted signals
 *
 * This widget emits the following signals:
 *
 * @li "theme,changed" - The theme is changed.
 * @li "language,changed" - The program's language is changed.
 *
 * @{
 */

/**
 * @brief Adds a new layout to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @see elm_layout_file_set()
 * @see elm_layout_theme_set()
 */
EAPI Evas_Object                 *elm_layout_add(Evas_Object *parent);

/**
 * @brief Sets the file that is used as a layout.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] file The path to the file (edj) that is used as a layout
 * @param[in] group The group that the layout belongs to in the edje file
 *
 * @return (@c 1 = success, @c 0 = error)
 */
EAPI Eina_Bool                    elm_layout_file_set(Evas_Object *obj, const char *file, const char *group);

/**
 * @brief Freezes the Elementary layout object.
 *
 * @details This function puts all changes on hold. Successive freezes are
 *          nested, requiring an equal number of thaws.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj A handle to an Elementary layout object
 * @return The frozen state, otherwise @c 0 on error
 *
 * @see elm_layout_thaw()
 */
EAPI int elm_layout_freeze(Evas_Object *obj);

/**
 * @brief Thaws the Elementary object.
 *
 * @details This function thaws the given Edje object and the Elementary sizing calc.
 *
 * @since_tizen 2.3
 *
 * @remarks If sucessive freezes are done, an equal number of
 *          thaws are required.
 *
 * @param[in] obj A handle to an Elementary layout object
 * @return The frozen state, otherwise @c 0 if the object is not frozen or on error
 *
 * @see elm_layout_freeze()
 */
EAPI int elm_layout_thaw(Evas_Object *obj);

/**
 * @brief Sets the edje group from the elementary theme that is used as a layout.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that @a style is the new style of @a obj too, as in an
 *          elm_object_style_set() call.
 *
 * @param[in] obj The layout object
 * @param[in] clas The class of the group
 * @param[in] group The group
 * @param[in] style The style to use
 *
 * @return (@c 1 = success, @c 0 = error)
 */
EAPI Eina_Bool                    elm_layout_theme_set(Evas_Object *obj, const char *clas, const char *group, const char *style);

/**
 * @brief Sends a (Edje) signal to a given layout widget's underlying Edje
 *        object.
 *
 * @details This function sends a signal to the underlying Edje object of @a
 *          obj. An Edje program on that Edje object's definition can respond
 *          to a signal by specifying matching 'signal' and 'source' fields.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object handle
 * @param[in] emission The signal's name string
 * @param[in] source The signal's source string
 */
EAPI void elm_layout_signal_emit(Evas_Object *obj, const char *emission, const char *source);

/**
 * @brief Adds a callback for a (Edje) signal emitted by a layout widget's
 *        underlying Edje object.
 *
 * @details This function connects a callback function to a signal emitted by
 *          the underlying Edje object of @a obj. Globs are accepted in either
 *          the emission or source strings (see
 *          edje_object_signal_callback_add()).
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object handle
 * @param[in] emission The signal's name string
 * @param[in] source The signal's source string
 * @param[in] func The callback function to be executed when the signal is
 *             emitted
 * @param[in] data A pointer to data to pass to the callback function
 */
EAPI void elm_layout_signal_callback_add(Evas_Object *obj, const char *emission, const char *source, Edje_Signal_Cb func, void *data);

/**
 * @brief Removes a signal-triggered callback from a given layout widget.
 *
 * @details This function removes the @b last callback attached to a signal
 *          emitted by the undelying Edje object of @a obj, with parameters @a
 *          emission, @a source, and @a func matching exactly those passed to a
 *          previous call to elm_object_signal_callback_add(). The data pointer
 *          that is passed to this call is returned.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object handle
 * @param[in] emission The signal's name string
 * @param[in] source The signal's source string
 * @param[in] func The callback function being executed when the signal
 *             is emitted
 * @return The data pointer of the signal callback (passed on
 *         elm_layout_signal_callback_add()), otherwise @c NULL on errors
 */
EAPI void *elm_layout_signal_callback_del(Evas_Object *obj, const char *emission, const char *source, Edje_Signal_Cb func);

/**
 * @brief Appends a child to the layout box part.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the object is appended, it becomes a child of the layout. Its
 *          lifetime is bound to the layout, whenever the layout dies the child
 *          is deleted automatically. One should use elm_layout_box_remove() to
 *          make this layout forget about the object.
 *
 * @param[in] obj The layout object
 * @param[in] part The box part to which the object is appended
 * @param[in] child The child object to append to the box
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_box_prepend()
 * @see elm_layout_box_insert_before()
 * @see elm_layout_box_insert_at()
 * @see elm_layout_box_remove()
 */
EAPI Eina_Bool                    elm_layout_box_append(Evas_Object *obj, const char *part, Evas_Object *child);

/**
 * @brief Prepends a child to the layout box part.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the object is prepended, it becomes the child of the layout. Its
 *          lifetime is bound to the layout, whenever the layout dies the child
 *          is deleted automatically. One should use elm_layout_box_remove() to
 *          make this layout forget about the object.
 *
 * @param[in] obj The layout object
 * @param[in] part The box part to prepend
 * @param[in] child The child object to prepend to the box
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_box_append()
 * @see elm_layout_box_insert_before()
 * @see elm_layout_box_insert_at()
 * @see elm_layout_box_remove()
 */
EAPI Eina_Bool                    elm_layout_box_prepend(Evas_Object *obj, const char *part, Evas_Object *child);

/**
 * @brief Inserts a child to the layout box part before a reference object.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the object is inserted, it becomes the child of the layout. Its
 *          lifetime is bound to the layout, whenever the layout dies the child
 *          is deleted automatically. One should use elm_layout_box_remove() to
 *          make this layout forget about the object.
 *
 * @param[in] obj The layout object
 * @param[in] part The box part to insert
 * @param[in] child The child object to insert into the box
 * @param[in] reference Another reference object to insert before in the box
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_box_append()
 * @see elm_layout_box_prepend()
 * @see elm_layout_box_insert_before()
 * @see elm_layout_box_remove()
 */
EAPI Eina_Bool                    elm_layout_box_insert_before(Evas_Object *obj, const char *part, Evas_Object *child, const Evas_Object *reference);

/**
 * @brief Inserts a child to the layout box part at a given position.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the object is inserted, it becomes the child of the layout. Its
 *          lifetime is bound to the layout, whenever the layout dies the child
 *          is deleted automatically. One should use elm_layout_box_remove() to
 *          make this layout forget about the object.
 *
 * @param[in] obj The layout object
 * @param[in] part The box part to insert
 * @param[in] child The child object to insert into the box
 * @param[in] pos The numeric position >=0 to insert the child
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_box_append()
 * @see elm_layout_box_prepend()
 * @see elm_layout_box_insert_before()
 * @see elm_layout_box_remove()
 */
EAPI Eina_Bool                    elm_layout_box_insert_at(Evas_Object *obj, const char *part, Evas_Object *child, unsigned int pos);

/**
 * @brief Removes a child of the given part box.
 *
 * @since_tizen 2.3
 *
 * @remarks The object is removed from the box part and its lifetime is
 *          not handled by the layout anymore. This is equivalent to
 *          elm_object_part_content_unset() for the box.
 *
 * @param[in] obj The layout object
 * @param[in] part The box part name from which to remove the child
 * @param[in] child The object to remove from the box
 * @return The object that is being used, otherwise @c NULL if not found
 *
 * @see elm_layout_box_append()
 * @see elm_layout_box_remove_all()
 */
EAPI Evas_Object                 *elm_layout_box_remove(Evas_Object *obj, const char *part, Evas_Object *child);

/**
 * @brief Removes all children of the given part box.
 *
 * @since_tizen 2.3
 *
 * @remarks The objects are removed from the box part and their lifetime is
 *          not handled by the layout anymore. This is equivalent to
 *          elm_layout_box_remove() for all box children.
 *
 * @param[in] obj The layout object
 * @param[in] part The box part name from which to remove the child
 * @param[in] clear If @c EINA_TRUE all objects are deleted as
 *              well, otherwise @c EINA_FALSE if they are just removed and are
 *              dangling on the canvas
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_box_append()
 * @see elm_layout_box_remove()
 */
EAPI Eina_Bool                    elm_layout_box_remove_all(Evas_Object *obj, const char *part, Eina_Bool clear);

/**
 * @brief Inserts a child to the layout table part.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the object is inserted, it becomes the child of the table. Its
 *          lifetime is bound to the layout, and whenever the layout dies the
 *          child is deleted automatically. One should use
 *          elm_layout_table_remove() to make this layout forget about the object.
 *
 * @remarks If @a colspan or @a rowspan are bigger than @c 1, that object occupies
 *          more space than a single cell. For instance, the following code:
 * @code
 * elm_layout_table_pack(layout, "table_part", child, 0, 1, 3, 1);
 * @endcode
 *
 * @remarks This would result in an object being added like the following picture:
 *
 * @image html layout_colspan.png
 * @image latex layout_colspan.eps "layout colspan" width=\textwidth
 *
 * @param[in] obj The layout object
 * @param[in] part The box part to pack the child
 * @param[in] child_obj The child object to pack into the table
 * @param[in] col The column to which the child should be added (>= 0)
 * @param[in] row The row to which the child should be added (>= 0)
 * @param[in] colspan The number of columns that should be used to store this object (>=
 *                1)
 * @param[in] rowspan The number of rows that should be used to store this object (>= 1)
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_table_unpack()
 * @see elm_layout_table_clear()
 */
EAPI Eina_Bool                    elm_layout_table_pack(Evas_Object *obj, const char *part, Evas_Object *child_obj, unsigned short col, unsigned short row, unsigned short colspan, unsigned short rowspan);

/**
 * @brief Unpacks (remove) a child of the given part table.
 *
 * @since_tizen 2.3
 *
 * @remarks The object is unpacked from the table part and its lifetime
 *          is not handled by the layout anymore. This is equivalent to
 *          elm_object_part_content_unset() for the table.
 *
 * @param[in] obj The layout object
 * @param[in] part The table part name from which to remove the child
 * @param[in] child_obj The object to remove from the table
 * @return The object that is being used, otherwise @c NULL if no object is found
 *
 * @see elm_layout_table_pack()
 * @see elm_layout_table_clear()
 */
EAPI Evas_Object                 *elm_layout_table_unpack(Evas_Object *obj, const char *part, Evas_Object *child_obj);

/**
 * @brief Removes all the child objects of the given part table.
 *
 * @since_tizen 2.3
 *
 * @remarks The objects are removed from the table part and their lifetime is
 *          not handled by the layout anymore. This is equivalent to
 *          elm_layout_table_unpack() for all table children.
 *
 * @param[in] obj The layout object
 * @param[in] part The table part name from which to remove the child
 * @param[in] clear If @c EINA_TRUE all objects are deleted as
 *              well, otherwise @c EINA_FALSE if they are just removed and are
 *              dangling on the canvas
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_table_pack()
 * @see elm_layout_table_unpack()
 */
EAPI Eina_Bool                    elm_layout_table_clear(Evas_Object *obj, const char *part, Eina_Bool clear);

/**
 * @brief Gets the edje layout.
 *
 * @since_tizen 2.3
 *
 * @remarks This returns the edje object. It is not expected to be used to then
 *          swallow objects via edje_object_part_swallow() for example. Use
 *          elm_object_part_content_set() instead so child object handling and sizing is
 *          done properly.
 *
 * @remarks This function should only be used if you really need to call some
 *          low level Edje function on this edje object. All the common stuff (setting
 *          text, emitting signals, hooking callbacks to signals, etc.) can be done
 *          with proper elementary functions.
 *
 * @param[in] obj The layout object
 *
 * @return An Evas_Object with the edje layout settings loaded
 *         using function elm_layout_file_set
 *
 * @see elm_object_signal_callback_add()
 * @see elm_object_signal_emit()
 * @see elm_object_part_text_set()
 * @see elm_object_part_content_set()
 * @see elm_layout_box_append()
 * @see elm_layout_table_pack()
 * @see elm_layout_data_get()
 */
EAPI Evas_Object                 *elm_layout_edje_get(const Evas_Object *obj);

/**
 * @brief Gets the edje data from the given layout.
 *
 * @details This function fetches the data specified inside the edje theme of this layout.
 *          This function returns @c NULL if data is not found.
 *
 * @since_tizen 2.3
 *
 * @remarks In EDC this comes from a data block within the group block that @a
 *          obj is loaded from. E.g.
 *
 * @code
 * collections {
 *   group {
 *     name: "a_group";
 *     data {
 *       item: "key1" "value1";
 *       item: "key2" "value2";
 *     }
 *   }
 * }
 * @endcode
 *
 * @param[in] obj The layout object
 * @param[in] key The data key
 *
 * @return The edje data string
 */
EAPI const char                  *elm_layout_data_get(const Evas_Object *obj, const char *key);

/**
 * @brief Re-evaluates Eval sizing.
 *
 * @since_tizen 2.3
 *
 * @remarks Manually forces a sizing re-evaluation. This is useful when the minimum
 *          size required by the edje theme of this layout has changed. The change on
 *          the minimum size required by the edje theme is not immediately reported to
 *          the elementary layout, so one needs to call this function in order to tell
 *          the widget (layout) that it needs to re-evaluate its own size.
 *
 * @remarks The minimum size of the theme is calculated based on the minimum size of the
 *          parts, the size of the elements inside containers like box and table, etc. All
 *          of this can change due to state changes, and that's when this function
 *          should be called.
 *
 * @remarks Also note that a standard signal of "size,eval" "elm" emitted from the
 *          edje object causes this to happen too.
 *
 * @param[in] obj The layout object
 */
EAPI void                         elm_layout_sizing_eval(Evas_Object *obj);

/**
 * @brief Sets a specific cursor for an edje part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part_name The part from the loaded edje group
 * @param[in] cursor The cursor name to use, see Elementary_Cursor.h
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure, that may be a
 *         part not existing or it may have "mouse_events: 0"
 */
EAPI Eina_Bool                    elm_layout_part_cursor_set(Evas_Object *obj, const char *part_name, const char *cursor);

/**
 * @brief Gets the cursor to be shown when the mouse is over an edje part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part_name The part from the loaded edje group
 * @return The cursor name
 */
EAPI const char                  *elm_layout_part_cursor_get(const Evas_Object *obj, const char *part_name);

/**
 * @brief Unsets a cursor previously set with elm_layout_part_cursor_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part_name The part from the loaded edje group, that had a cursor set
 *                  with elm_layout_part_cursor_set()
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool                    elm_layout_part_cursor_unset(Evas_Object *obj, const char *part_name);

/**
 * @brief Sets a specific cursor style for an edje part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part_name The part from the loaded edje group
 * @param[in] style The theme style to use (default, transparent, ...)
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure, that may be a
 *         part not existing or it may not have a cursor set.
 */
EAPI Eina_Bool                    elm_layout_part_cursor_style_set(Evas_Object *obj, const char *part_name, const char *style);

/**
 * @brief Gets a specific cursor style for an edje part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part_name The part from the loaded edje group
 *
 * @return The theme style in use, defaults to "default" \n
 *         If the object does not have a cursor set, then @c NULL is returned
 */
EAPI const char                  *elm_layout_part_cursor_style_get(const Evas_Object *obj, const char *part_name);

/**
 * @brief Sets whether the cursor set should be searched on the theme or should use what is
 *        provided by the engine, only.
 *
 * @since_tizen 2.3
 *
 * @remarks Before you set its look on the theme you should define a
 *          cursor with elm_layout_part_cursor_set(). By default it only
 *          looks for cursors provided by the engine.
 *
 * @param[in] obj The layout object
 * @param[in] part_name The part from the loaded edje group
 * @param[in] engine_only If cursors should be just provided by the engine (@c EINA_TRUE),
 *                    otherwise should also search on the widget's theme as well (@c EINA_FALSE)
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure, that may be a
 *         part not existing or it may not had a cursor set
 */
EAPI Eina_Bool                    elm_layout_part_cursor_engine_only_set(Evas_Object *obj, const char *part_name, Eina_Bool engine_only);

/**
 * @brief Sets accessibility to all texblock(text) parts in the layout object.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] can_access It makes all textblock(text) parts in the layout @a obj possible
 *                   to have accessibility \n
 *                   @c EINA_TRUE means textblock(text) parts can be accessible.
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure \n
 *         If @a obj is not a proper layout object, @c EINA_FALSE is returned.
 */
EAPI Eina_Bool                    elm_layout_edje_object_can_access_set(Evas_Object *obj, Eina_Bool can_access);

/**
 * @brief Gets the accessibility state of the texblock(text) parts in the layout object.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 *
 * @return @c EINA_TRUE, if all textblock(text) parts in the layout can be accessible,
 *         otherwise @c EINA_FALSE if those cannot be accessible \n
 *         If @a obj is not a proper layout object, @c EINA_FALSE is returned.
 *
 * @see elm_layout_edje_object_access_set()
 */
EAPI Eina_Bool                    elm_layout_edje_object_can_access_get(Evas_Object *obj);

/**
 * @brief Gets a specific cursor engine_only for an edje part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part_name The part from the loaded edje group
 *
 * @return Whenever the cursor is just provided by the engine or also from the theme
 */
EAPI Eina_Bool                    elm_layout_part_cursor_engine_only_get(const Evas_Object *obj, const char *part_name);

/**
 * @brief Sets the layout content.
 *
 * @since_tizen 2.3
 *
 * @remarks Once the content object is set, a previously set one is deleted.
 *          If you want to keep that old content object, use the
 *          elm_object_part_content_unset() function.
 *
 * @remarks In an Edje theme, the part used as a content container is called @c
 *          SWALLOW. This is why the parameter name is called @a swallow, but it is
 *          expected to be a part name just like the second parameter of
 *          elm_layout_box_append().
 *
 * @param[in] obj The layout object
 * @param[in] swallow The swallow part name in the edje file
 * @param[in] content The child that is added in this layout object
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_layout_box_append()
 * @see elm_object_part_content_get()
 * @see elm_object_part_content_unset()
 * @see @ref secBox
 */
EAPI Eina_Bool                    elm_layout_content_set(Evas_Object *obj, const char *swallow, Evas_Object *content);

/**
 * @brief Gets the child object in the given content part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] swallow The SWALLOW part to get its content
 *
 * @return The swallowed object, otherwise @c NULL if none are present or an error occurs
 */
EAPI Evas_Object                 *elm_layout_content_get(const Evas_Object *obj, const char *swallow);

/**
 * @brief Unsets the layout content.
 *
 * @since_tizen 2.3
 *
 * @remarks Unparent and return the content object which is set for this part.
 *
 * @param[in] obj The layout object
 * @param[in] swallow The swallow part name in the edje file
 * @return The content that is being used
 *
 */
EAPI Evas_Object                 *elm_layout_content_unset(Evas_Object *obj, const char *swallow);

/**
 * @brief Sets the text of the given part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part The TEXT part where to set the text
 * @param[in] text The text to set
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool                    elm_layout_text_set(Evas_Object *obj, const char *part, const char *text);

/**
 * @brief Gets the text set in the given part.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The layout object
 * @param[in] part The TEXT part to retrieve the text from
 *
 * @return The text set in @a part
 */
EAPI const char                  *elm_layout_text_get(const Evas_Object *obj, const char *part);

/**
 * @def elm_layout_icon_set
 * @brief Convenience macro to set the icon object in a layout that follows the
 *        Elementary naming convention for its parts.
 *
 * @since_tizen 2.3
 */
#define elm_layout_icon_set(_ly, _obj)                                 \
  do {                                                                 \
       const char *sig;                                                \
       elm_object_part_content_set((_ly), "elm.swallow.icon", (_obj)); \
       if ((_obj)) sig = "elm,state,icon,visible";                     \
       else sig = "elm,state,icon,hidden";                             \
       elm_object_signal_emit((_ly), sig, "elm");                      \
    } while (0)

/**
 * @def elm_layout_icon_get
 * @brief Convenience macro to get the icon object from a layout that follows the
 *        Elementary naming convention for its parts.
 *
 * @since_tizen 2.3
 */
#define elm_layout_icon_get(_ly) \
  elm_object_part_content_get((_ly), "elm.swallow.icon")

/**
 * @def elm_layout_end_set
 * @brief Convenience macro to set the end object in a layout that follows the
 *        Elementary naming convention for its parts.
 *
 * @since_tizen 2.3
 */
#define elm_layout_end_set(_ly, _obj)                                 \
  do {                                                                \
       const char *sig;                                               \
       elm_object_part_content_set((_ly), "elm.swallow.end", (_obj)); \
       if ((_obj)) sig = "elm,state,end,visible";                     \
       else sig = "elm,state,end,hidden";                             \
       elm_object_signal_emit((_ly), sig, "elm");                     \
    } while (0)

/**
 * @def elm_layout_end_get
 * @brief Convenience macro to get the end object in a layout that follows the
 *        Elementary naming convention for its parts.
 *
 * @since_tizen 2.3
 */
#define elm_layout_end_get(_ly) \
  elm_object_part_content_get((_ly), "elm.swallow.end")

/**
 * @}
 */
