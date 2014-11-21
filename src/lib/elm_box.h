/**
 * @defgroup Box Box
 * @ingroup elm_container_group
 *
 * @image html box_inheritance_tree.png
 * @image latex box_inheritance_tree.eps
 *
 * @image html img/box.png
 * @image latex img/box.eps "box" width=\textwidth
 *
 * @brief A box arranges objects in a linear fashion, governed by a layout
 *        function that defines the details of this arrangement.
 *
 * By default, the box uses an internal function to set the layout to
 * a single row, either vertical or horizontal. This layout is affected
 * by a number of parameters, such as the homogeneous flag set by
 * elm_box_homogeneous_set(), the values given by elm_box_padding_set() and
 * elm_box_align_set(), and the hints set to each object in the box.
 *
 * For this default layout, it's possible to change the orientation with
 * elm_box_horizontal_set(). The box starts in the vertical orientation,
 * placing its elements ordered from top to bottom. When horizontal is set,
 * the order goes from left to right. If the box is set to be
 * homogeneous, every object in it is assigned the same space, which is that
 * of the largest object. Padding can be used to set some spacing between
 * the cell given to each object. The alignment of the box, set with
 * elm_box_align_set(), determines how the bounding box of all the elements
 * is placed within the space given to the box widget itself.
 *
 * The size hints of each object also affect how they are placed and sized
 * within the box. evas_object_size_hint_min_set() gives the minimum
 * size that the object can have, and the box uses it as the basis for all
 * latter calculations. Elementary widgets set their own minimum size as
 * needed, so there's rarely any need to use it manually.
 *
 * evas_object_size_hint_weight_set(), when not in the homogeneous mode, is
 * used to tell whether the object is allocated the minimum size it
 * needs or if the space given to it should be expanded. It's important
 * to realize that expanding the size given to the object is not the same
 * thing as resizing the object. It could very well end being a small
 * widget floating in a much larger empty space. If not set, the weight
 * for objects normally is @c 0.0 for both axis, meaning the widget is
 * not expanded. To take as much space as possible, set the weight to
 * EVAS_HINT_EXPAND (defined to @c 1.0) for the desired axis to expand.
 *
 * Besides how much space is allocated for each object, it's possible to control
 * how the widget is placed within that space using
 * evas_object_size_hint_align_set(). By default, this value is @c 0.5
 * for both axes, meaning the object is centered, but any value from
 * @c 0.0 (left or top, for the @c x and @c y axis, respectively) to @c 1.0
 * (right or bottom) can be used. The special value EVAS_HINT_FILL, which
 * is @c -1.0, means the object is resized to fill the entire space it
 * is allocated.
 *
 * In addition, customized functions to define the layout can be set, which
 * allow the application developer to organize the objects within the box
 * in a number of ways.
 *
 * The special elm_box_layout_transition() function can be used
 * to switch from one layout to another, animating the motion of the
 * children of the box.
 *
 * @remarks Objects should not be added to box objects using _add() calls.
 *
 * @{
 */

/**
 * @typedef Elm_Box_Transition
 *
 * @brief The structure type which is an opaque handler containing the parameters to perform an animated
 *        transition of the layout that the box uses.
 *
 * @see elm_box_transition_new()
 * @see elm_box_layout_set()
 * @see elm_box_layout_transition()
 */
typedef struct _Elm_Box_Transition Elm_Box_Transition;

/**
 * @brief Adds a new box to the parent.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, the box is in the vertical mode and is non-homogeneous.
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object        *elm_box_add(Evas_Object *parent);

/**
 * @brief Sets the horizontal orientation.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, the box object arranges their contents vertically from top to
 *          bottom.
 *
 * @remarks By calling this function with @a horizontal as @c EINA_TRUE, the box
 *          becomes horizontal, arranging contents from left to right.
 *
 * @remarks This flag is ignored if a custom layout function is set.
 *
 * @param[in] obj The box object
 * @param[in] horizontal The horizontal flag (@c EINA_TRUE = horizontal,
 *                   @c EINA_FALSE = vertical)
 */
EAPI void                elm_box_horizontal_set(Evas_Object *obj, Eina_Bool horizontal);

/**
 * @brief Gets the horizontal orientation.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @return @c EINA_TRUE if the box is set to the horizontal mode, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool           elm_box_horizontal_get(const Evas_Object *obj);

/**
 * @brief Sets the box to arrange its children homogeneously.
 *
 * @since_tizen 2.3
 *
 * @remarks If enabled, homogeneous layout makes all items of the same size, according
 *          to the size of the largest of its children.
 *
 * @remarks This flag is ignored if a custom layout function is set.
 *
 * @param[in] obj The box object
 * @param[in] homogeneous The homogeneous flag
 */
EAPI void                elm_box_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous);

/**
 * @brief Gets whether the box is using the homogeneous mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @return @c EINA_TRUE if it's homogeneous, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool           elm_box_homogeneous_get(const Evas_Object *obj);

/**
 * @brief Adds an object to the beginning of the pack list.
 *
 * @since_tizen 2.3
 *
 * @remarks This packs @a subobj into the box @a obj, placing it first in the list of
 *          children objects. The actual position that the object gets on screen
 *          depends on the layout used. If no custom layout is set, it is at
 *          the top or left, depending on whether the box is vertical or horizontal,
 *          respectively.
 *
 * @param[in] obj The box object
 * @param[in] subobj The object to add to the box
 *
 * @see elm_box_pack_end()
 * @see elm_box_pack_before()
 * @see elm_box_pack_after()
 * @see elm_box_unpack()
 * @see elm_box_unpack_all()
 * @see elm_box_clear()
 */
EAPI void                elm_box_pack_start(Evas_Object *obj, Evas_Object *subobj);

/**
 * @brief Adds an object at the end of the pack list.
 *
 * @since_tizen 2.3
 *
 * @remarks This packs @a subobj into the box @a obj, placing it last in the list of
 *          children objects. The actual position that the object gets on screen
 *          depends on the layout used. If no custom layout is set, it is at
 *          the bottom or right, depending on whether the box is vertical or horizontal,
 *          respectively.
 *
 * @param obj The box object
 * @param subobj The object to add to the box
 *
 * @see elm_box_pack_start()
 * @see elm_box_pack_before()
 * @see elm_box_pack_after()
 * @see elm_box_unpack()
 * @see elm_box_unpack_all()
 * @see elm_box_clear()
 */
EAPI void                elm_box_pack_end(Evas_Object *obj, Evas_Object *subobj);

/**
 * @brief Adds an object to the box before the indicated object.
 *
 * @details This adds @a subobj to the box indicated before the object
 *          indicated with @a before. If @a before is not already in the box, the results
 *          are undefined. Before means either to the left of the indicated object or
 *          above it depending on the orientation.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @param[in] subobj The object to add to the box
 * @param[in] before The object before which to add it
 *
 * @see elm_box_pack_start()
 * @see elm_box_pack_end()
 * @see elm_box_pack_after()
 * @see elm_box_unpack()
 * @see elm_box_unpack_all()
 * @see elm_box_clear()
 */
EAPI void                elm_box_pack_before(Evas_Object *obj, Evas_Object *subobj, Evas_Object *before);

/**
 * @brief Adds an object to the box after the indicated object.
 *
 * @details This adds @a subobj to the box indicated after the object
 *          indicated with @a after. If @a after is not already in the box, the results
 *          are undefined. After means either to the right of the indicated object or
 *          below it depending on the orientation.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @param[in] subobj The object to add to the box
 * @param[in] after The object after which to add it
 *
 * @see elm_box_pack_start()
 * @see elm_box_pack_end()
 * @see elm_box_pack_before()
 * @see elm_box_unpack()
 * @see elm_box_unpack_all()
 * @see elm_box_clear()
 */
EAPI void                elm_box_pack_after(Evas_Object *obj, Evas_Object *subobj, Evas_Object *after);

/**
 * @brief Clears the box of all its children.
 *
 * @details This removes all the elements contained by the box, deleting the respective
 *          objects.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 *
 * @see elm_box_unpack()
 * @see elm_box_unpack_all()
 */
EAPI void                elm_box_clear(Evas_Object *obj);

/**
 * @brief Unpacks a box item.
 *
 * @details This removes the object given by @a subobj from the box @a obj without
 *          deleting it.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @param[in] subobj The object to unpack
 *
 * @see elm_box_unpack_all()
 * @see elm_box_clear()
 */
EAPI void                elm_box_unpack(Evas_Object *obj, Evas_Object *subobj);

/**
 * @brief Removes all the items from the box, without deleting them.
 *
 * @details This clears the box from all its children, but doesn't delete the respective objects.
 *          If no other references of the box children exist, the objects is never
 *          deleted, and thus the application leaks the memory. Make sure
 *          when using this function that you hold a reference to all the objects
 *          in the box @a obj.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 *
 * @see elm_box_clear()
 * @see elm_box_unpack()
 */
EAPI void                elm_box_unpack_all(Evas_Object *obj);

/**
 * @brief Retrieves a list of the objects packed into the box.
 *
 * @since_tizen 2.3
 *
 * @remarks This returns a new @c Eina_List with a pointer to @c Evas_Object in its nodes.
 *          The order of the list corresponds to the packing order that the box uses.
 *
 * @remarks You must free this list with eina_list_free() once you are done with it.
 *
 * @param[in] obj The box object
 * @return The children objects list
 */
EAPI Eina_List    *elm_box_children_get(const Evas_Object *obj);

/**
 * @brief Sets the space (padding) between the box's elements.
 *
 * @since_tizen 2.3
 *
 * @remarks This gives the extra space in pixels that is added between a box child and its
 *          neighbors after its containing cell has been calculated. This padding
 *          is set for all elements in the box, besides any possible padding that
 *          individual elements may have through their size hints.
 *
 * @param[in] obj The box object
 * @param[in] horizontal The horizontal space between elements
 * @param[in] vertical The vertical space between elements
 */
EAPI void                elm_box_padding_set(Evas_Object *obj, Evas_Coord horizontal, Evas_Coord vertical);

/**
 * @brief Gets the space (padding) between the box's elements.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @param[out] horizontal The horizontal space between elements
 * @param[out] vertical The vertical space between elements
 *
 * @see elm_box_padding_set()
 */
EAPI void                elm_box_padding_get(const Evas_Object *obj, Evas_Coord *horizontal, Evas_Coord *vertical);

/**
 * @brief Sets the alignment of the whole bounding box of contents.
 *
 * @details This sets how the bounding box containing all the elements of the box, after
 *          their sizes and position has been calculated, is aligned within
 *          the space given for the whole box widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @param[in] horizontal The horizontal alignment of elements
 * @param[in] vertical The vertical alignment of elements
 */
EAPI void                elm_box_align_set(Evas_Object *obj, double horizontal, double vertical);

/**
 * @brief Gets the alignment of the whole bounding box of contents.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The box object
 * @param[out] horizontal The horizontal alignment of elements
 * @param[out] vertical The vertical alignment of elements
 *
 * @see elm_box_align_set()
 */
EAPI void                elm_box_align_get(const Evas_Object *obj, double *horizontal, double *vertical);

/**
 * @brief Forces the box to recalculate its children packing.
 *
 * @since_tizen 2.3
 *
 * @remarks If any child is added or removed, the box does not calculate the
 *          values immediately rather it leaves it to the next main loop
 *          iteration. While this is great as it would save lots of
 *          recalculation, whenever you need to get the position of a recently
 *          added item you must force recalculation before doing so.
 *
 * @param[in] obj The box object
 */
EAPI void                elm_box_recalculate(Evas_Object *obj);

/**
 * @brief Sets the layout defining function to be used by the box.
 *
 * @since_tizen 2.3
 *
 * @remarks Whenever anything changes that requires the box in @a obj to recalculate
 *          the size and position of its elements, the function @a cb is called
 *          to determine what the layout of the children is.
 *
 * @remarks Once a custom function is set, everything about the children layout
 *          is defined by it. The flags set by elm_box_horizontal_set() and
 *          elm_box_homogeneous_set() no longer have any meaning, and the values
 *          given by elm_box_padding_set() and elm_box_align_set() are up to the
 *          layout function to decide whether they are used and how. These last two
 *          are found in the @a priv parameter, of type @c Evas_Object_Box_Data,
 *          passed to @a cb. The @c Evas_Object that the function receives is not the
 *          Elementary widget, but the internal Evas Box it uses, so none of the
 *          functions described here can be used on it.
 *
 * @remarks Any of the layout functions in @c Evas can be used here, as well as the
 *          special elm_box_layout_transition().
 *
 * @remarks The final @a data argument received by @a cb is the same @a data passed
 *          here, and the @a free_data function is called to free it
 *          whenever the box is destroyed or another layout function is set.
 *
 * @remarks Setting @a cb to @c NULL reverts back to the default layout function.
 *
 * @param[in] obj The box object
 * @param[in] cb The callback function used for the layout
 * @param[in] data The data that is passed to the layout function
 * @param[in] free_data The function called to free @a data
 *
 * @see elm_box_layout_transition()
 */
EAPI void                elm_box_layout_set(Evas_Object *obj, Evas_Object_Box_Layout cb, const void *data, Ecore_Cb free_data);

/**
 * @brief Special layout function that animates the transition from one layout to another.
 *
 * @since_tizen 2.3
 *
 * @remarks Normally, when switching the layout function for a box, this
 *          reflects immediately on screen on the next render, but it's also
 *          possible to do this through an animated transition.
 *
 * @remarks This is done by creating an ::Elm_Box_Transition and setting the box
 *          layout to this function.
 *
 * For example:
 * @code
 * Elm_Box_Transition *t = elm_box_transition_new(1.0,
 *                            evas_object_box_layout_vertical, // start
 *                            NULL, // data for initial layout
 *                            NULL, // free function for initial data
 *                            evas_object_box_layout_horizontal, // end
 *                            NULL, // data for final layout
 *                            NULL, // free function for final data
 *                            anim_end, // will be called when animation ends
 *                            NULL); // data for anim_end function\
 * elm_box_layout_set(box, elm_box_layout_transition, t,
 *                    elm_box_transition_free);
 * @endcode
 *
 * @remarks This function can only be used with elm_box_layout_set(). Calling
 *          it directly does not have the expected results.
 *
 * @param[in] obj  The box object
 * @param[in] priv The smart data of the @p obj
 * @param[in] data The data pointer passed
 *
 * @see elm_box_transition_new
 * @see elm_box_transition_free
 * @see elm_box_layout_set
 */
EAPI void                elm_box_layout_transition(Evas_Object *obj, Evas_Object_Box_Data *priv, void *data);

/**
 * @brief Creates a new ::Elm_Box_Transition to animate the switch of the layouts.
 *
 * @since_tizen 2.3
 *
 * @remarks If you want to animate the change from one layout to another, you need
 *          to set the layout function of the box to elm_box_layout_transition(),
 *          passing user data to it as an instance of ::Elm_Box_Transition with the
 *          necessary information to perform this animation. The free function to
 *          set the layout is elm_box_transition_free().
 *
 * @remarks The parameters to create an ::Elm_Box_Transition sum up to how long
 *          it is, in seconds, a layout function to describe the initial point,
 *          another for the final position of the children and one function to be
 *          called when the whole animation ends. This last function is useful to
 *          set the definitive layout for the box, usually it is same as the end
 *          layout for the animation, but could be used to start another transition.
 *
 * @param[in] duration The duration of the transition in seconds
 * @param[in] start_layout The layout function that is used to start the animation
 * @param[in] start_layout_data The data to be passed to the @a start_layout function
 * @param[in] start_layout_free_data The function to free @a start_layout_data
 * @param[in] end_layout The layout function that is used to end the animation
 * @param[in] end_layout_data The data parameter passed to @a end_layout
 * @param end_layout_free_data The data to be passed to the @a end_layout function
 * @param end_layout_free_data The function to free @a end_layout_data
 * @param[in] transition_end_cb The callback function called when the animation ends
 * @param[in] transition_end_data The data to be passed to @a transition_end_cb
 * @return An instance of ::Elm_Box_Transition
 *
 * @see elm_box_transition_new
 * @see elm_box_layout_transition
 */
EAPI Elm_Box_Transition *elm_box_transition_new(const double duration, Evas_Object_Box_Layout start_layout, void *start_layout_data, Ecore_Cb start_layout_free_data, Evas_Object_Box_Layout end_layout, void *end_layout_data, Ecore_Cb end_layout_free_data, Ecore_Cb transition_end_cb, void *transition_end_data);

/**
 * @brief Frees an Elm_Box_Transition instance created with elm_box_transition_new().
 *
 * @since_tizen 2.3
 *
 * @remarks This function is mostly useful as the @a free_data parameter in
 *          elm_box_layout_set() with elm_box_layout_transition().
 *
 * @param[in] data The Elm_Box_Transition instance to be freed
 *
 * @see elm_box_transition_new
 * @see elm_box_layout_transition
 */
EAPI void                elm_box_transition_free(void *data);

/**
 * @}
 */
