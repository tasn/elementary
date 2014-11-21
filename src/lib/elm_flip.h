/**
 * @defgroup Flip Flip
 * @ingroup elm_widget_group
 *
 * @image html flip_inheritance_tree.png
 * @image latex flip_inheritance_tree.eps
 *
 * @brief This widget holds @c 2 content objects(Evas_Object): one on the front
 *        and one on the back. It allows you to flip from front to back and 
 *        vice-versa using various animations.
 *
 * If either the front or back contents are not set, the flip treats that
 * as transparent. So if you were to set the front content but not the back,
 * and then call elm_flip_go() you would see whatever is below the flip.
 *
 * For a list of supported animations see elm_flip_go().
 *
 * Signals that you can add callbacks for are:
 * "animate,begin" - When a flip animation is started
 * "animate,done" - When a flip animation is finished
 *
 * The default content parts of the flip widget that you can use are:
 * @li "front" - The front content of the flip.
 * @li "back" - The back content of the flip.
 *
 * The functions meant to act on it works for mapbuf objects:
 *
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 *
 * @{
 */
typedef enum
{
   ELM_FLIP_ROTATE_Y_CENTER_AXIS,
   ELM_FLIP_ROTATE_X_CENTER_AXIS,
   ELM_FLIP_ROTATE_XZ_CENTER_AXIS,
   ELM_FLIP_ROTATE_YZ_CENTER_AXIS,
   ELM_FLIP_CUBE_LEFT,
   ELM_FLIP_CUBE_RIGHT,
   ELM_FLIP_CUBE_UP,
   ELM_FLIP_CUBE_DOWN,
   ELM_FLIP_PAGE_LEFT,
   ELM_FLIP_PAGE_RIGHT,
   ELM_FLIP_PAGE_UP,
   ELM_FLIP_PAGE_DOWN
} Elm_Flip_Mode;

typedef enum
{
   ELM_FLIP_INTERACTION_NONE,
   ELM_FLIP_INTERACTION_ROTATE,
   ELM_FLIP_INTERACTION_CUBE,
   ELM_FLIP_INTERACTION_PAGE
} Elm_Flip_Interaction;

/**
 * Enumeration of Elm Flip Direction type
 */
typedef enum
{
   ELM_FLIP_DIRECTION_UP, /**< Allows interaction with the top of the widget */
   ELM_FLIP_DIRECTION_DOWN, /**< Allows interaction with the bottom of the widget */
   ELM_FLIP_DIRECTION_LEFT, /**< Allows interaction with the left portion of the widget */
   ELM_FLIP_DIRECTION_RIGHT /**< Allows interaction with the right portion of the widget */
} Elm_Flip_Direction;

/**
 * @brief Adds a new flip to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object *elm_flip_add(Evas_Object *parent);

/**
 * @brief Gets the flip front visibility state.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The flip object
 * @return @c EINA_TRUE if the front is showing, otherwise @c EINA_FALSE if the back is
 *         showing
 */
Eina_Bool elm_flip_front_visible_get(const Evas_Object *obj);

/**
 * @brief Sets the flip perspective.
 *
 * @since_tizen 2.3
 *
 * @remarks This function currently does nothing.
 *
 * @param[in] obj The flip object
 * @param[in] foc The coordinate to set the focus on
 * @param[in] x The X coordinate
 * @param[in] y The Y coordinate
 */
EAPI void                 elm_flip_perspective_set(Evas_Object *obj, Evas_Coord foc, Evas_Coord x, Evas_Coord y);

/**
 * @brief Runs the flip animation.
 *
 * @since_tizen 2.3
 *
 * @remarks Flips the front and back contents using the @a mode animation. This
 *          effectively hides the currently visible content and shows the hidden one.
 *
 * @remarks There are a number of possible animations to use for flipping:
 * @li ELM_FLIP_ROTATE_X_CENTER_AXIS - Rotates the currently visible content
 *                                     around a horizontal axis in the middle of its height, the other content
 *                                     is shown as the other side of the flip.
 * @li ELM_FLIP_ROTATE_Y_CENTER_AXIS - Rotates the currently visible content
 *                                     around a vertical axis in the middle of its width, the other content is
 *                                     shown as the other side of the flip.
 * @li ELM_FLIP_ROTATE_XZ_CENTER_AXIS - Rotates the currently visible content
 *                                      around a diagonal axis in the middle of its width, the other content is
 *                                      shown as the other side of the flip.
 * @li ELM_FLIP_ROTATE_YZ_CENTER_AXIS - Rotates the currently visible content
 *                                      around a diagonal axis in the middle of its height, the other content is
 *                                      shown as the other side of the flip.
 * @li ELM_FLIP_CUBE_LEFT - Rotates the currently visible content to the left
 *                          as if the flip is a cube, the other content is shown as the right face of
 *                          the cube.
 * @li ELM_FLIP_CUBE_RIGHT - Rotates the currently visible content to the
 *                           right as if the flip is a cube, the other content is shown as the left
 *                           face of the cube.
 * @li ELM_FLIP_CUBE_UP - Rotates the currently visible content up as if the
 *                        flip is a cube, the other content is shown as the bottom face of the cube.
 * @li ELM_FLIP_CUBE_DOWN - Rotates the currently visible content down as if
 *                          the flip is a cube, the other content is shown as the upper face of the
 *                          cube.
 * @li ELM_FLIP_PAGE_LEFT - Moves the currently visible content to the left as
 *                          if the flip is a book, the other content is shown as the page below that.
 * @li ELM_FLIP_PAGE_RIGHT - Moves the currently visible content to the right
 *                           as if the flip is a book, the other content is shown as the page below
 *                           that.
 * @li ELM_FLIP_PAGE_UP - Moves the currently visible content up as if the
 *                        flip is a book, the other content is shown as the page below that.
 * @li ELM_FLIP_PAGE_DOWN - Moves the currently visible content down as if the
 *                          flip is a book, the other content is shown as the page below that.
 *
 * @image html elm_flip.png
 * @image latex elm_flip.eps "elm flip" width=\textwidth
 *
 * @param[in] obj The flip object
 * @param[in] mode The mode type
 *
 * @see elm_flip_go_to()
 */
EAPI void                 elm_flip_go(Evas_Object *obj, Elm_Flip_Mode mode);

/**
 * @brief Runs the flip animation to the front or back.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks Flips the front and back contents using the @a mode animation. This
 *          effectively hides the currently visible content and shows the hidden one.
 *
 * @remarks There are a number of possible animations to use for flipping:
 * @li ELM_FLIP_ROTATE_X_CENTER_AXIS - Rotates the currently visible content
 *                                     around a horizontal axis in the middle of its height, the other content
 *                                     is shown as the other side of the flip.
 * @li ELM_FLIP_ROTATE_Y_CENTER_AXIS - Rotates the currently visible content
 *                                     around a vertical axis in the middle of its width, the other content is
 *                                     shown as the other side of the flip.
 * @li ELM_FLIP_ROTATE_XZ_CENTER_AXIS - Rotates the currently visible content
 *                                      around a diagonal axis in the middle of its width, the other content is
 *                                      shown as the other side of the flip.
 * @li ELM_FLIP_ROTATE_YZ_CENTER_AXIS - Rotates the currently visible content
 *                                      around a diagonal axis in the middle of its height, the other content is
 *                                      shown as the other side of the flip.
 * @li ELM_FLIP_CUBE_LEFT - Rotates the currently visible content to the left
 *                          as if the flip is a cube, the other content is shown as the right face of
 *                          the cube.
 * @li ELM_FLIP_CUBE_RIGHT - Rotates the currently visible content to the
 *                           right as if the flip is a cube, the other content is shown as the left
 *                           face of the cube.
 * @li ELM_FLIP_CUBE_UP - Rotates the currently visible content up as if the
 *                        flip is a cube, the other content is shown as the bottom face of the cube.
 * @li ELM_FLIP_CUBE_DOWN - Rotates the currently visible content down as if
 *                          the flip is a cube, the other content is shown as the upper face of the
 *                          cube.
 * @li ELM_FLIP_PAGE_LEFT - Moves the currently visible content to the left as
 *                          if the flip is a book, the other content is shown as the page below that.
 * @li ELM_FLIP_PAGE_RIGHT - Moves the currently visible content to the right
 *                           as if the flip is a book, the other content is shown as the page below
 *                           that.
 * @li ELM_FLIP_PAGE_UP - Moves the currently visible content up as if the
 *                        flip is a book, the other content is shown as the page below that.
 * @li ELM_FLIP_PAGE_DOWN - Moves the currently visible content down as if the
 *                          flip is a book, the other content is shown as the page below that.
 *
 * @image html elm_flip.png
 * @image latex elm_flip.eps "elm flip" width=\textwidth
 *
 * @param[in] obj The flip object
 * @param[in] front If @c EINA_TRUE it makes the front visible,
 *              otherwise @c EINA_FALSE to make the back visible
 * @param[in] mode The mode type
 */
EAPI void                 elm_flip_go_to(Evas_Object *obj, Eina_Bool front, Elm_Flip_Mode mode);

/**
 * @brief Sets the interactive flip mode.
 *
 * @since_tizen 2.3
 *
 * @remarks This sets if the flip should be interactive (allow a user to click and
 *          drag a side of the flip to reveal the back page and cause it to flip).
 *          By default a flip is not interactive. You may also need to set which
 *          sides of the flip are "active" for flipping and how much space they use
 *          (a minimum of a finger size) with elm_flip_interaction_direction_enabled_set()
 *          and elm_flip_interaction_direction_hitsize_set().
 *
 * @remarks The four available modes of interaction are:
 *          @li ELM_FLIP_INTERACTION_NONE - No interaction is allowed.
 *          @li ELM_FLIP_INTERACTION_ROTATE - Interaction causes rotate animation.
 *          @li ELM_FLIP_INTERACTION_CUBE - Interaction causes cube animation.
 *          @li ELM_FLIP_INTERACTION_PAGE - Interaction causes page animation.
 *
 * @remarks ELM_FLIP_INTERACTION_ROTATE won't cause
 *          ELM_FLIP_ROTATE_XZ_CENTER_AXIS or ELM_FLIP_ROTATE_YZ_CENTER_AXIS to
 *          happen, they can only be achieved with elm_flip_go().
 *
 * @param[in] obj The flip object
 * @param[in] mode The interactive flip mode to use
 */
EAPI void                 elm_flip_interaction_set(Evas_Object *obj, Elm_Flip_Interaction mode);

/**
 * @brief Gets the interactive flip mode.
 *
 * @since_tizen 2.3
 *
 * @remarks This returns the interactive flip mode set by elm_flip_interaction_set().
 *
 * @param[in] obj The flip object
 * @return The interactive flip mode
 */
EAPI Elm_Flip_Interaction elm_flip_interaction_get(const Evas_Object *obj);

/**
 * @brief Sets which directions of the flip respond to the interactive flip.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, all the directions are disabled, so you may want to enable the
 *          desired directions for flipping if you need interactive flipping. You must
 *          call this function once for each direction that should be enabled.
 *
 * @param[in] obj The flip object
 * @param[in] dir The direction to change
 * @param[in] enabled The boolean value that indicates whether that direction is enabled
 *
 * @see elm_flip_interaction_set()
 */
EAPI void                 elm_flip_interaction_direction_enabled_set(Evas_Object *obj, Elm_Flip_Direction dir, Eina_Bool enabled);

/**
 * @brief Gets the enabled state of that flip direction.
 *
 * @details This gets the enabled state set by elm_flip_interaction_direction_enabled_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The flip object
 * @param[in] dir The direction to check
 * @return The boolean value that indicates whether that direction is enabled
 *
 * @see elm_flip_interaction_set()
 */
EAPI Eina_Bool            elm_flip_interaction_direction_enabled_get(Evas_Object *obj, Elm_Flip_Direction dir);

/**
 * @brief Sets the amount of the flip that is sensitive to the interactive flip.
 *
 * @details This sets the amount of the flip that is sensitive to the interactive flip, with @c 0
 *          representing no area in the flip and @c 1 representing the entire flip. There
 *          is however a consideration to be made. Basically, the area should never be
 *          smaller than the finger size set(as set in your Elementary configuration).
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The flip object
 * @param[in] dir The direction to modify
 * @param[in] hitsize The amount of that dimension (@c 0.0 to @c 1.0) to use
 *
 * @see elm_flip_interaction_set()
 */
EAPI void                 elm_flip_interaction_direction_hitsize_set(Evas_Object *obj, Elm_Flip_Direction dir, double hitsize);

/**
 * @brief Gets the amount of the flip that is sensitive to the interactive flip.
 *
 * @details This returns the amount of sensitive area set by
 *          elm_flip_interaction_direction_hitsize_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The flip object
 * @param[in] dir The direction to check
 * @return The size set for that direction
 */
EAPI double               elm_flip_interaction_direction_hitsize_get(Evas_Object *obj, Elm_Flip_Direction dir);

/**
 * @}
 */
