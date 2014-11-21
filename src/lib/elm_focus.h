/**
 * @defgroup Focus Focus
 * @ingroup elm_infra_group
 * @brief This group provides functions to handle Elementary Focus.
 *
 * An Elementary application has, at all times, one (and only one)
 * @b focused object. This is what determines where the input
 * events go to within the application's window. Also, focused
 * objects can be decorated differently, in order to signal to the
 * user where the input is, at a given moment.
 *
 * Elementary applications also have the concept of <b>focus
 * chain</b>: one can cycle through all the windows' focusable
 * objects by input (tab key) or programmatically. The default
 * focus chain for an application is the one defined by the order in
 * which the widgets are added in the code. One cycles through the
 * top level widgets, and, for each one containing sub-object, cycle
 * through them all, before returning to the level
 * above. Elementary also allows one to set @b custom focus chains
 * for their applications.
 *
 * Besides the focused decoration that a widget may exhibit, when it
 * gets focus, Elementary has a @b global focus highlight object
 * that can be enabled for a window. If one chooses to do so, this
 * extra highlight effect surrounds the current focused object,
 * too.
 *
 * @remarks Some Elementary widgets are @b unfocusable, after
 * creation, by their very nature: they are not meant to be
 * interacted with input events, but are there just for visual
 * purposes.  *
 *
 * @{
 */

/**
 * @brief Enumeration that defines the focus directions.
 */
typedef enum
{
   ELM_FOCUS_PREVIOUS, /**< Previous direction */
   ELM_FOCUS_NEXT,     /**< Next direction */
   ELM_FOCUS_UP,       /**< Up direction */
   ELM_FOCUS_DOWN,     /**< Down direction */
   ELM_FOCUS_RIGHT,    /**< Right direction */
   ELM_FOCUS_LEFT,      /**< Left direction */
   ELM_FOCUS_NONE
} Elm_Focus_Direction;

/**
 * @typedef Elm_Focus_Info
 *
 * @brief The structure type containing the info sent in the callback for the "focused" signals.
 */
typedef struct _Elm_Focus_Info Elm_Focus_Info;

struct _Elm_Focus_Info
{
   Evas_Object        *prev_focused;
   Elm_Focus_Direction dir;
};

/**
 * @brief Gets whether an Elementary object has the focus.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary object to get the information from
 * @return @c EINA_TRUE if the object is focused, otherwise @c EINA_FALSE if
 *         not (and on errors)
 *
 * @see elm_object_focus_set()
 */
EAPI Eina_Bool            elm_object_focus_get(const Evas_Object *obj);

/**
 * @brief Sets or unsets focus to a given Elementary object.
 *
 * @since_tizen 2.3
 *
 * @remarks When you set focus to this object, if it can handle focus, it
 *          takes the focus away from the one who had it previously and, from
 *          now on, becomes the one receiving input events. Unsetting focus removes
 *          the focus from @a obj, passing it back to the previous element in the
 *          focus chain list.
 *
 * @param[in] obj The Elementary object to operate on
 * @param[in] focus If @c EINA_TRUE the focus to a given object is set,
 *              otherwise @c EINA_FALSE to unset the focus to a given object
 *
 * @see elm_object_focus_get()
 * @see elm_object_focus_custom_chain_get()
 */
EAPI void                 elm_object_focus_set(Evas_Object *obj, Eina_Bool focus);

/**
 * @brief Sets the ability for an Elementary object to be focused.
 *
 * @details This sets whether the object @a obj is able to take focus or
 *          not. Unfocusable objects do nothing when programmatically
 *          focused, being the nearest focusable parent object, the one
 *          really getting focus. Also, when they receive mouse input, they
 *          get the event, but do not take away the focus from where it
 *          is previously.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary object to operate on
 * @param[in] enable If @c EINA_TRUE the object can be focused,
 *               otherwise @c EINA_FALSE if it cannot (and on errors)
 */
EAPI void                 elm_object_focus_allow_set(Evas_Object *obj, Eina_Bool enable);

/**
 * @brief Gets whether an Elementary object is focusable.
 *
 * @since_tizen 2.3
 *
 * @remarks Objects which are meant to be interacted with by input
 *          events are created to be able to be focused, by default. All the
 *          others are not.
 *
 * @param[in] obj The Elementary object to operate on
 * @return @c EINA_TRUE if the object is allowed to be focused,
 *         otherwise @c EINA_FALSE if it is not (and on errors)
 */
EAPI Eina_Bool            elm_object_focus_allow_get(const Evas_Object *obj);

/**
 * @brief Sets the custom focus chain.
 *
 * @details This function overwrites any previous custom focus chain within
 *          the list of objects. The previous list is deleted and this list
 *          is managed by elementary. After it is set, don't modify it.
 *
 * @since_tizen 2.3
 *
 * @remarks On the focus cycle, there are only evaluated children of this container.
 *
 * @param[in] obj The container object
 * @param[in] objs The chain of objects to pass focus to
 */
EAPI void                 elm_object_focus_custom_chain_set(Evas_Object *obj, Eina_List *objs);

/**
 * @brief Unsets a custom focus chain on a given Elementary widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Any focus chain previously set on @a obj (for its child objects)
 *          is removed entirely after this call.
 *
 * @param[in] obj The container object to remove the focus chain from
 */
EAPI void                 elm_object_focus_custom_chain_unset(Evas_Object *obj);

/**
 * @brief Gets the custom focus chain.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The container object
 * @return  The focus custom chain list
 */
EAPI const Eina_List     *elm_object_focus_custom_chain_get(const Evas_Object *obj);

/**
 * @brief Appends an object to the custom focus chain.
 *
 * @since_tizen 2.3
 *
 * @remarks If @a relative_child is equal to @c NULL or not in the custom chain, the object
 *          is added in the end.
 *
 * @remarks On the focus cycle, there are only evaluated children of this container.
 *
 * @param[in] obj The container object
 * @param[in] child The child to be added in the custom chain
 * @param[in] relative_child The relative object to position the child
 */
EAPI void                 elm_object_focus_custom_chain_append(Evas_Object *obj, Evas_Object *child, Evas_Object *relative_child);

/**
 * @brief Prepends an object to the custom focus chain.
 *
 * @since_tizen 2.3
 *
 * @remarks If @a relative_child is equal to @c NULL or not in custom chain, the object
 *          is added in the beginning.
 *
 * @remarks On the focus cycle, there are only evaluated children of this container.
 *
 * @param[in] obj The container object
 * @param[in] child The child to be added in the custom chain
 * @param[in] relative_child The relative object to position the child
 */
EAPI void                 elm_object_focus_custom_chain_prepend(Evas_Object *obj, Evas_Object *child, Evas_Object *relative_child);

/**
 * @brief Gives focus to the next object in the object tree.
 *
 * @details This gives focus to the next object in the focus chain of one the object sub-tree.
 *          If the last object of the chain already has focus, the focus goes to the
 *          first object of the chain.
 *
 * @param[in] obj The object root of the sub-tree
 * @param[in] dir The direction to move the focus
 *
 * @see elm_object_focus_next_object_get()
 * @see elm_object_focus_next_object_set()
 */
EAPI void                 elm_object_focus_next(Evas_Object *obj, Elm_Focus_Direction dir);

/**
 * @brief Gets the next object that is set with a specific focus direction.
 *
 * @details This gets the next object that is set by elm_object_focus_next_object_set()
 *          with a specific focus direction.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary object
 * @param[in] dir The focus direction
 * @return The focus next object, otherwise @c NULL if there is no focus next object
 *
 * @see elm_object_focus_next_object_set()
 * @see elm_object_focus_next()
 */
EAPI Evas_Object *        elm_object_focus_next_object_get(const Evas_Object *obj, Elm_Focus_Direction dir);

/**
 * @brief Sets the next object with a specific focus direction.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks When the focus next object is set with a specific focus direction, this object
 *          is the first candidate when finding the next focusable object.
 *          The focus next object can be registered with six directions that are previous,
 *          next, up, down, right, and left.
 *
 * @param[in] obj The Elementary object
 * @param[in] next The focus next object
 * @param[in] dir The focus direction
 *
 * @see elm_object_focus_next_object_get()
 * @see elm_object_focus_next()
 */
EAPI void                 elm_object_focus_next_object_set(Evas_Object *obj, Evas_Object *next, Elm_Focus_Direction dir);

/**
 * @brief Gets the focused object in the object tree.
 *
 * @details This function returns the current focused object in one object sub-tree.
 *
 * @since 1.8
 *
 * @param obj The object root of the sub-tree
 * @return The current focused object, otherwise @c NULL if there is no focused object
 */
EAPI Evas_Object         *elm_object_focused_object_get(const Evas_Object *obj);

/**
 * @brief Makes the elementary object and its children to be focusable
 *        (or unfocusable).
 *
 * @details This sets whether the object @a obj and its children objects
 *          are able to take focus or not. If the tree is set as unfocusable, the
 *          newest focused object which is not in this tree gets focus.
 *          This API can be helpful for an object to be deleted.
 *          When an object is deleted soon, the object and its children may not
 *          want to get focus (by focus reverting or by other focus controls).
 *          Then, just use this API before deleting.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary object to operate on
 * @param[in] focusable If @c EINA_TRUE it is focusable,
 *                  otherwise @c EINA_FALSE if it is unfocusable
 *
 * @see elm_object_tree_focus_allow_get()
 *
 */
EAPI void                 elm_object_tree_focus_allow_set(Evas_Object *obj, Eina_Bool focusable);

/**
 * @brief Gets whether an Elementary object and its children are focusable.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Elementary object to get the information from
 * @return @c EINA_TRUE, if the tree is focusable,
 *         otherwise @c EINA_FALSE if it is not (and on errors)
 *
 * @see elm_object_tree_focus_allow_set()
 */
EAPI Eina_Bool            elm_object_tree_focus_allow_get(const Evas_Object *obj);

/**
 * @}
 */
