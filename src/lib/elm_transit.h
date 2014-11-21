/**
 * @defgroup Transit Transit
 * @ingroup elm_infra_group
 * @brief This group provides functions to handle the effects of transition
 *        of Elementary widgets.
 *
 * Transit is designed to apply various animated transition effects to @c
 * Evas_Object, such as translation, rotation, etc. For using these
 * effects, create @ref Elm_Transit and add the desired transition effects.
 *
 * Once the effects are added into transit, they are automatically
 * managed (their callback is called for the set duration and
 * they are deleted upon completion).
 *
 * Example:
 * @code
 * Elm_Transit *trans = elm_transit_add();
 * elm_transit_object_add(trans, obj);
 * elm_transit_effect_translation_add(trans, 0, 0, 280, 280
 * elm_transit_duration_set(transit, 1);
 * elm_transit_auto_reverse_set(transit, EINA_TRUE);
 * elm_transit_tween_mode_set(transit, ELM_TRANSIT_TWEEN_MODE_DECELERATE);
 * elm_transit_repeat_times_set(transit, 3);
 * @endcode
 *
 * Some transition effects are used to change the properties of objects. They
 * are:
 * @li @ref elm_transit_effect_translation_add
 * @li @ref elm_transit_effect_color_add
 * @li @ref elm_transit_effect_rotation_add
 * @li @ref elm_transit_effect_wipe_add
 * @li @ref elm_transit_effect_zoom_add
 * @li @ref elm_transit_effect_resizing_add
 *
 * Other transition effects are used to make one object disappear and another
 * object appear on its place. These effects are:
 *
 * @li @ref elm_transit_effect_flip_add
 * @li @ref elm_transit_effect_resizable_flip_add
 * @li @ref elm_transit_effect_fade_add
 * @li @ref elm_transit_effect_blend_add
 *
 * It's also possible to make a transition chain with @ref
 * elm_transit_chain_transit_add.
 *
 * @remarks We strongly recommend to use elm_transit just when edje cannot do
 * the trick. Edje is better at handling transitions than Elm_Transit.
 * Edje has more flexibility and animations can be manipulated inside the theme.
 *
 * @{
 */

/**
 * @enum Elm_Transit_Tween_Mode
 *
 * @brief Enumeration of acceleration used in the transition.
 *
 * @since_tizen 2.3
 */
typedef enum
{
   ELM_TRANSIT_TWEEN_MODE_LINEAR, /**< Constant speed */
   ELM_TRANSIT_TWEEN_MODE_SINUSOIDAL, /**< Starts slow, increases speed
                                         over time, then decreases again
                                         and stops slowly, v1 being a power factor */
   ELM_TRANSIT_TWEEN_MODE_DECELERATE, /**< Starts fast and decreases
                                         speed over time, v1 being a power factor */
   ELM_TRANSIT_TWEEN_MODE_ACCELERATE, /**< Starts slow and increases speed
                                         over time, v1 being a power factor */
   ELM_TRANSIT_TWEEN_MODE_DIVISOR_INTERP, /**< Starts at gradient v1,
                                             interpolated via power of the v2 curve */
   ELM_TRANSIT_TWEEN_MODE_BOUNCE, /**< Starts at @c 0.0 then a "drop" like a ball
                                     bouncing to the ground at @c 1.0, and
                                     bounce v2 times, with decay factor of v1 */
   ELM_TRANSIT_TWEEN_MODE_SPRING /**< Starts at @c 0.0 then a "wobble" like a spring
                                    at rest position @c 1.0, and wobble v2 times,
                                    with decay factor of v1 */
} Elm_Transit_Tween_Mode;

/**
 * @enum Elm_Transit_Effect_Flip_Axis
 *
 * @brief Enumeration of the axis along which flip effect should be applied.
 *
 * @since_tizen 2.3
 */
typedef enum
{
   ELM_TRANSIT_EFFECT_FLIP_AXIS_X, /**< Flip on X axis */
   ELM_TRANSIT_EFFECT_FLIP_AXIS_Y /**< Flip on Y axis */
} Elm_Transit_Effect_Flip_Axis;

/**
 * @enum Elm_Transit_Effect_Wipe_Dir
 *
 * @brief Enumeration of the direction in which the wipe effect should occur.
 *
 * @since_tizen 2.3
 */
typedef enum
{
   ELM_TRANSIT_EFFECT_WIPE_DIR_LEFT, /**< Wipe to the left */
   ELM_TRANSIT_EFFECT_WIPE_DIR_RIGHT, /**< Wipe to the right */
   ELM_TRANSIT_EFFECT_WIPE_DIR_UP, /**< Wipe up */
   ELM_TRANSIT_EFFECT_WIPE_DIR_DOWN /**< Wipe down */
} Elm_Transit_Effect_Wipe_Dir;

/** @enum Elm_Transit_Effect_Wipe_Type
 *
 * @brief Enumeration that indicates whether the wipe effect should show or hide the object.
 *
 * @since_tizen 2.3
 */
typedef enum
{
   ELM_TRANSIT_EFFECT_WIPE_TYPE_HIDE, /**< Hide the object during the
                                         animation */
   ELM_TRANSIT_EFFECT_WIPE_TYPE_SHOW /**< Show the object during the
                                        animation */
} Elm_Transit_Effect_Wipe_Type;

/**
 * @typedef Elm_Transit
 *
 * @brief The Transit structure type created with elm_transit_add(). This type has the information
 *        about the objects to which the transition is applied, and the
 *        transition effects that are used. It also contains info about
 *        duration, number of repetitions, auto-reverse, etc.
 *
 * @since_tizen 2.3
 */
typedef struct _Elm_Transit Elm_Transit;
typedef void                Elm_Transit_Effect;

/**
 * @typedef Elm_Transit_Effect_Transition_Cb
 *
 * @brief Called for this effect on each transition iteration.
 *
 * @since_tizen 2.3
 */
typedef void (*Elm_Transit_Effect_Transition_Cb)(Elm_Transit_Effect *effect, Elm_Transit *transit, double progress);

/**
 * Elm_Transit_Effect_End_Cb
 *
 * @brief Called for this effect when the transition is over.
 *
 * @since_tizen 2.3
 */
typedef void (*Elm_Transit_Effect_End_Cb)(Elm_Transit_Effect *effect, Elm_Transit *transit);

/**
 * Elm_Transit_Del_Cb
 *
 * @brief Called when the transit is deleted.
 *
 * @since_tizen 2.3
 */
typedef void (*Elm_Transit_Del_Cb)(void *data, Elm_Transit *transit);

/**
 * @brief Creates a new transit.
 *
 * @since_tizen 2.3
 *
 * @remarks It is not necessary to delete the transit object, it is deleted at
 *          the end of its operation.
 * @remarks The transit starts playing when the program enters the main loop.
 *
 * @return The transit object
 */
EAPI Elm_Transit           *elm_transit_add(void);

/**
 * @brief Stops the animation and deletes the @a transit object.
 *
 * @since_tizen 2.3
 *
 * @remarks Call this function if you want to stop the animation before the
 *          transit time. Make sure the @a transit object is still alive using the
 *          elm_transit_del_cb_set() function.
 *          All added effects are deleted, calling its respective data_free_cb
 *          functions. The function set by elm_transit_del_cb_set() is called.
 *
 * @param[in] transit The transit object to be deleted
 *
 * @see elm_transit_del_cb_set()
 */
EAPI void                   elm_transit_del(Elm_Transit *transit);

/**
 * @brief Adds a new effect to the transit.
 *
 * @since_tizen 2.3
 *
 * @remarks The @a cb function and @a data are the key to the effect.
 *          If you try to add an existing effect, nothing is done.
 * @remarks After the first addition of an effect to @a transit, if its
 *          effect list become empty again, the @a transit is killed by the
 *          elm_transit_del(transit) function.
 *
 * Example:
 * @code
 * Elm_Transit *transit = elm_transit_add();
 * elm_transit_effect_add(transit,
 *                        elm_transit_effect_blend_op,
 *                        elm_transit_effect_blend_context_new(),
 *                        elm_transit_effect_blend_context_free);
 * @endcode
 *
 * @remarks The transit frees the context data at the end of the
 *          transition with the @a data_free_cb function.
 * @remarks Do not share the context data in between different transit objects.
 *
 * @param[in] transit The transit object
 * @param[in] transition_cb The operation function \n
 *        It is called when the
 *        animation begins, it is the function that actually performs the animation \n
 *        It is called with the @a data, @a transit and the time progression of the
 *        animation (a double value between @c 0.0 and @c 1.0).
 * @param[in] effect The context data of the effect
 * @param[in] end_cb The function to free the context data, it is called
 *        at the end of the effect, it must finalize the animation and free the
 *        @a data.
 */
EAPI void                   elm_transit_effect_add(Elm_Transit *transit, Elm_Transit_Effect_Transition_Cb transition_cb, Elm_Transit_Effect *effect, Elm_Transit_Effect_End_Cb end_cb);

/**
 * @brief Deletes an added effect.
 *
 * @details This function removes the effect from the @a transit, calling the
 *          @a data_free_cb to free the @a data.
 *
 * @since_tizen 2.3
 *
 * @remarks If the effect is not found, nothing is done.
 * @remarks If the effect list becomes empty, this function calls
 *          elm_transit_del(transit), i.e., it kills the @a transit.
 *
 * @param[in] transit The transit object
 * @param[in] transition_cb The operation function
 * @param[in] effect The context data of the effect
 *
 * @see elm_transit_effect_add()
 */
EAPI void                   elm_transit_effect_del(Elm_Transit *transit, Elm_Transit_Effect_Transition_Cb transition_cb, Elm_Transit_Effect *effect);

/**
 * @brief Adds a new object to apply the effects.
 *
 * @since_tizen 2.3
 *
 * @remarks After the first addition of an object to @a transit, if its
 *          object list becomes empty again, the @a transit is killed by the
 *          elm_transit_del(transit) function.
 * @remarks If the @a obj belongs to another transit, the @a obj is
 *          removed from it and it only belongs to the other @a transit.
 *          If the old transit stays without objects, it dies.
 * @remarks When you add an object into @a transit, its state from
 *          evas_object_pass_events_get(obj) is saved, and it is applied when the
 *          transit ends, if you change this state using evas_object_pass_events_set()
 *          after adding the object, this state changes again when @a transit stops.
 * @remarks It is not allowed to add a new object after transit begins.
 *
 * @param[in] transit The transit object
 * @param[in] obj The object to be animated
 */
EAPI void                   elm_transit_object_add(Elm_Transit *transit, Evas_Object *obj);

/**
 * @brief Removes an added object from the transit.
 *
 * @since_tizen 2.3
 *
 * @remarks If the @a obj is not in the @a transit, nothing is done.
 * @remarks If the list becomes empty, this function calls
 *          elm_transit_del(transit), i.e., it kills the @a transit.
 * @remarks It is not allowed to remove objects after transit begins.
 *
 * @param[in] transit The transit object
 * @param[in] obj The object to be removed from @a transit
 */
EAPI void                   elm_transit_object_remove(Elm_Transit *transit, Evas_Object *obj);

/**
 * @brief Gets the objects of the transit.
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @return a The Eina_List with the objects from the transit
 */
EAPI const Eina_List       *elm_transit_objects_get(const Elm_Transit *transit);

/**
 * @brief Enables or disables keeping up the object's states.
 *        If it is not kept, the object's states are reset when transition ends.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL.
 * @remarks One state includes geometry, color, and map data.
 *
 * @param[in] transit The transit object
 * @param[in] state_keep The boolean value that indicates whether to retain the state
 */
EAPI void                   elm_transit_objects_final_state_keep_set(Elm_Transit *transit, Eina_Bool state_keep);

/**
 * @brief Gets a value that indicates whether the object's states are reset.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL
 *
 * @param[in] transit The transit object
 * @return @c EINA_TRUE indicates that the states of the objects are reset \n
 *         If @a transit is @c NULL, @c EINA_FALSE is returned
 *
 * @see elm_transit_objects_final_state_keep_set()
 */
EAPI Eina_Bool              elm_transit_objects_final_state_keep_get(const Elm_Transit *transit);

/**
 * @brief Sets the events that are enabled when transit is operating.
 *
 * @since_tizen 2.3
 *
 * @remarks If @a enabled is @c EINA_TRUE, the objects of the transit receive
 *          events from a mouse and keyboard during the animation.
 * @remarks When you add an object with elm_transit_object_add(), its state from
 *          evas_object_freeze_events_get(obj) is saved, and it is applied when the
 *          transit ends. If you change this state using evas_object_freeze_events_set()
 *          after adding the object, this state changes again when @a transit stops
 *          to run.
 *
 * @param[in] transit The transit object
 * @param[in] enabled Events are received when enabled is @c EINA_TRUE,
 *                otherwise it is ignored
 */
EAPI void                   elm_transit_event_enabled_set(Elm_Transit *transit, Eina_Bool enabled);

/**
 * @brief Gets the value of the event enabled status.
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @return @c EINA_TRUE if the event is enabled,
 *         otherwise @c EINA_FALSE is returned if @a transit is @c NULL
 *
 * @see elm_transit_event_enabled_set()
 */
EAPI Eina_Bool              elm_transit_event_enabled_get(const Elm_Transit *transit);

/**
 * @brief Sets the user-callback function when the transit is deleted.
 *
 * @since_tizen 2.3
 *
 * @remarks Using this function twice overwrites the first function set.
 * @remarks The @a transit object is deleted after calling the @a cb function.
 *
 * @param[in] transit The transit object
 * @param[in] cb The callback function pointer \n
 *           This function is called before the deletion of the transit.
 * @param[in] data The callback function for user data \n
 *             It is the @a op parameter.
 */
EAPI void                   elm_transit_del_cb_set(Elm_Transit *transit, Elm_Transit_Del_Cb cb, void *data);

/**
 * @brief Sets the reverse effect automatically.
 *
 * @since_tizen 2.3
 *
 * @remarks If auto reverse is set, after running the effects with the progress
 *          parameter from @c 0 to @c 1, it calls the effects again with the progress
 *          from @c 1 to @c 0. The transit lasts for a time equal to (2 * duration * repeat),
 *          where the duration is set using the function elm_transit_add and
 *          the repeat using the function elm_transit_repeat_times_set().
 *
 * @param[in] transit The transit object
 * @param[in] reverse If @c EINA_TRUE the auto_reverse is on, otherwise @c EINA_FALSE
 */
EAPI void                   elm_transit_auto_reverse_set(Elm_Transit *transit, Eina_Bool reverse);

/**
 * @brief Gets whether the auto reverse is on.
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @return @c EINA_TRUE if the auto reverse is on,
 *         otherwise @c EINA_FALSE is returned if @a transit is @c NULL
 *
 * @see elm_transit_auto_reverse_set()
 */
EAPI Eina_Bool              elm_transit_auto_reverse_get(const Elm_Transit *transit);

/**
 * @brief Sets the transit repeat count. The effect is repeated as per the repeat count.
 *
 * @details This function sets the number of repetitions that the transit runs after
 *          the first one, i.e., if @a repeat is @c 1, the transit runs @c 2 times.
 *          If @a repeat is a negative number, it repeats infinite times.
 *
 * @since_tizen 2.3
 *
 * @remarks If this function is called during the transit execution, the transit
 *          runs @a repeat times, ignoring the times it already performed.
 *
 * @param[in] transit The transit object
 * @param[in] repeat The repeat count
 */
EAPI void                   elm_transit_repeat_times_set(Elm_Transit *transit, int repeat);

/**
 * @brief Gets the transit repeat count.
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @return The repeat count \n
 *         If @a transit is @c NULL, @c 0 is returned
 *
 * @see elm_transit_repeat_times_set()
 */
EAPI int                    elm_transit_repeat_times_get(const Elm_Transit *transit);

/**
 * @brief Sets the transit animation acceleration type.
 *
 * @details This function sets the tween mode of the transit which can be:
 *          ELM_TRANSIT_TWEEN_MODE_LINEAR - The default mode.
 *          ELM_TRANSIT_TWEEN_MODE_SINUSOIDAL - Starts in the accelerate mode and ends
 *                                              decelerating with factor.
 *          ELM_TRANSIT_TWEEN_MODE_DECELERATE - The animation is slowed over time
 *                                              with factor.
 *          ELM_TRANSIT_TWEEN_MODE_ACCELERATE - The animation accelerates over time
 *                                              with factor.
 *          ELM_TRANSIT_TWEEN_MODE_DIVISOR_INTERP - Starts at gradient v1, interpolated
 *                                                  via power of the v2 curve.
 *          ELM_TRANSIT_TWEEN_MODE_BOUNCE - Starts at @c 0.0 then a "drop" like a ball bouncing
 *                                          to the ground at @c 1.0, and bounce v2 times, with decay factor of v1.
 *          ELM_TRANSIT_TWEEN_MODE_SPRING - Starts at @c 0.0 then a "wobble" like a spring in rest
 *                                          position @c 1.0, and wobble v2 times, with decay factor of v1.
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @param[in] tween_mode The tween type
 */
EAPI void                   elm_transit_tween_mode_set(Elm_Transit *transit, Elm_Transit_Tween_Mode tween_mode);

/**
 * @brief Gets the transit animation acceleration type.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL.
 *
 * @param[in] transit The transit object
 * @return The tween type \n
 *         If @a transit is @c NULL, @c ELM_TRANSIT_TWEEN_MODE_LINEAR is returned.
 */
EAPI Elm_Transit_Tween_Mode elm_transit_tween_mode_get(const Elm_Transit *transit);

/**
 * @brief Sets the transit animation acceleration factor.
 *
 * @details This function sets the tween mode factor of the transit which can be:
 *          If you use the below tween modes, you have to set the factor using this API.
 *          ELM_TRANSIT_TWEEN_MODE_SINUSOIDAL - Starts slow, speeds up then slows down
 *                                              at the end, v1 being a power factor, @c 0.0 being linear, @c 1.0 being
 *          ELM_TRANSIT_TWEEN_MODE_SINUSOIDAL - Default, @c 2.0 being a much more pronounced
 *                                              sinusoidal(squared), @c 3.0 being cubed, etc.
 *          ELM_TRANSIT_TWEEN_MODE_DECELERATE - Starts fast then slows down, v1 being a
 *                                              power factor, @c 0.0 being linear, @c 1.0 being ELM_TRANSIT_TWEEN_MODE_DECELERATE
 *                                              default, @c 2.0 being a much more pronounced decelerate (squared), @c 3.0 being
 *                                              cubed, etc.
 *          ELM_TRANSIT_TWEEN_MODE_ACCELERATE - Starts slow then speeds up, v1 being a
 *                                              power factor, @c 0.0 being linear, @c 1.0 being ELM_TRANSIT_TWEEN_MODE_ACCELERATE
 *                                              default, @c 2.0 being a much more pronounced accelerate (squared), @c 3.0 being
 *                                              cubed, etc.
 *          ELM_TRANSIT_TWEEN_MODE_DIVISOR_INTERP - Starts at gradient * v1, interpolated
 *                                                  via power of the v2 curve.
 *          ELM_TRANSIT_TWEEN_MODE_BOUNCE - Starts at @c 0.0 then a "drop" like a ball bouncing
 *                                          to the ground at @c 1.0, and bounce v2 times, with decay factor of v1.
 *          ELM_TRANSIT_TWEEN_MODE_SPRING - Starts at @c 0.0 then a "wobble" like a spring in rest
 *                                          position @c 1.0, and wobble v2 times, with decay factor of v1.
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @param[in] v1 A parameter used by the mapping (default is @c 1.0)
 * @param[in] v2 A parameter used by the mapping (default is @c 0.0)
 *
 * @see elm_transit_tween_mode_factor_get()
 */
EAPI void                   elm_transit_tween_mode_factor_set(Elm_Transit *transit, double v1, double v2);

/**
 * @brief Gets the transit animation acceleration factor.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL.
 *
 * @param[in] transit The transit object
 * @param[out] v1      The pointer to a double in which to store the factor value
 * @param[out] v2      The pointer to a double in which to store the factor value2
 *
 * @see elm_transit_tween_mode_factor_set()
 */
EAPI void                   elm_transit_tween_mode_factor_get(const Elm_Transit *transit, double *v1, double *v2);

/**
 * @brief Sets the transit animation time.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL.
 *
 * @param[in] transit The transit object
 * @param[in] duration The animation time
 */
EAPI void                   elm_transit_duration_set(Elm_Transit *transit, double duration);

/**
 * @brief Gets the transit animation time.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL.
 *
 * @param[in] transit The transit object
 *
 * @return The transit animation time
 */
EAPI double                 elm_transit_duration_get(const Elm_Transit *transit);

/**
 * @brief Starts the transition.
 *
 * @since_tizen 2.3
 *
 * @remarks Once this API is called, the transit begins to measure the time.
 *
 * @remarks @a transit cannot be @c NULL.
 *
 * @param[in] transit The transit object
 */
EAPI void                   elm_transit_go(Elm_Transit *transit);

/**
 * @brief Pauses and resumes the transition.
 *
 * @since_tizen 2.3
 *
 * @remarks If you call elm_transit_go() again, the transit is started from the
 *          beginning, and is played.
 *
 * @remarks @a transit cannot be @c NULL
 *
 * @param[in] transit The transit object
 * @param[in] paused The boolean value that indicates whether the transition should be paused
 */
EAPI void                   elm_transit_paused_set(Elm_Transit *transit, Eina_Bool paused);

/**
 * @brief Gets the value of the paused status.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL.
 *
 * @param[in] transit The transit object
 * @return @c EINA_TRUE if the transition is paused,
 *         otherwise @c EINA_FALSE is returned if @a transit is @c NULL
 *
 * @see elm_transit_paused_set()
 */
EAPI Eina_Bool              elm_transit_paused_get(const Elm_Transit *transit);

/**
 * @brief Gets the time progression of the animation (a double value between @c 0.0 and @c 1.0).
 *
 * @since_tizen 2.3
 *
 * @remarks The value returned is a fraction (current time / total time). It
 *          represents the progression position relative to the total.
 *
 * @remarks @a transit cannot be @c NULL
 *
 * @param[in] transit The transit object
 *
 * @return The time progression value \n
 *         If @a transit is @c NULL, @c 0 is returned
 */
EAPI double                 elm_transit_progress_value_get(const Elm_Transit *transit);

/**
 * @brief Makes the chain relationship between two transits.
 *
 * @since_tizen 2.3
 *
 * @remarks This function adds the @a chain_transit transition to a chain after the @a
 *          transit, and is started as soon as @a transit ends.
 *
 * @remarks @a transit cannot be @c NULL. Transit would have multiple chain transits.
 * @remarks @p chain_transit cannot be @c NULL. Chain transits could be chained to only one transit.
 *
 * @param[in] transit The transit object
 * @param[in] chain_transit The chain transit object \n 
 *                      This transit is operated after transit is done.
 */
EAPI void                   elm_transit_chain_transit_add(Elm_Transit *transit, Elm_Transit *chain_transit);

/**
 * @brief Cuts off the chain relationship between two transits.
 *
 * @since_tizen 2.3
 *
 * @remarks This function removes the @a chain_transit transition from the @a transit.
 *
 * @remarks @a transit cannot be @c NULL. Transit would have a chain relationship with @a chain transit.
 * @remarks @a chain_transit cannot be @c NULL. Chain transits should be chained to the @a transit.
 *
 * @param[in] transit The transit object
 * @param[in] chain_transit The chain transit object
 */
EAPI void                   elm_transit_chain_transit_del(Elm_Transit *transit, Elm_Transit *chain_transit);

/**
 * @brief Gets the current chain transit list.
 *
 * @since_tizen 2.3
 *
 * @remarks @a transit cannot be @c NULL.
 *
 * @param[in] transit The transit object
 * @return The chain transit list
 */
EAPI Eina_List             *elm_transit_chain_transits_get(const Elm_Transit *transit);

/**
 * @brief Sets the smooth effect for a transit.
 *
 * @details This sets smoothing for transit map rendering. If the object added in a
 *          transit is of a type that has its own smoothing settings, then both the smooth
 *          settings for this object and the map must be turned off. By default, smooth
 *          maps are enabled.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @param[in] smooth enabled The boolean value that enables or disables smooth map rendering
 *
 * @see evas_map_smooth_set()
 */
EAPI void                   elm_transit_smooth_set(Elm_Transit *transit, Eina_Bool smooth);

/**
 * @brief Gets smooth scaling for transit map rendering.
 *
 * @details This gets smooth scaling for transit map rendering.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] transit The transit object
 * @return @c EINA_TRUE if smooth scaling is enabled, otherwise @c EINA_FALSE
 *
 * @see elm_transit_smooth_set()
 *
 */
Eina_Bool                   elm_transit_smooth_get(const Elm_Transit *transit);

/**
 * @brief Adds the resizing effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates resizing effect context
 *          and adds its required APIs to elm_transit_effect_add().
 *
 * @param[in] transit The transit object
 * @param[in] from_w The object width size when the effect begins
 * @param[in] from_h The object height size when the effect begins
 * @param[in] to_w The object width size when the effect ends
 * @param[in] to_h The object height size when the effect ends
 * @return The resizing effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_resizing_add(Elm_Transit *transit, Evas_Coord from_w, Evas_Coord from_h, Evas_Coord to_w, Evas_Coord to_h);

/**
 * @brief Adds the translation effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates translation effect context
 *          and adds its required APIs to elm_transit_effect_add().
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window to which the objects of the transit belong has already been created.
 *          This is because this effect needs the geometry information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @param[in] from_dx The x position variation when the effect begins
 * @param[in] from_dy The y position variation when the effect begins
 * @param[in] to_dx The x position variation when the effect ends
 * @param[in] to_dy The y position variation when the effect ends
 * @return The translation effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_translation_add(Elm_Transit *transit, Evas_Coord from_dx, Evas_Coord from_dy, Evas_Coord to_dx, Evas_Coord to_dy);

/**
 * @brief Adds the zoom effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates zoom effect context
 *          and adds its required APIs to elm_transit_effect_add().
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window to which the objects of the transit belong has already been created.
 *          This is because this effect needs the geometry information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @param[in] from_rate The scale rate when the effect begins (@c 1 is the current rate)
 * @param[in] to_rate The scale rate when the effect ends
 * @return The zoom effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_zoom_add(Elm_Transit *transit, float from_rate, float to_rate);

/**
 * @brief Adds the flip effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates flip effect context
 *          and adds its required APIs to elm_transit_effect_add().
 * @remarks This effect is applied to each pair of objects in the order they are listed
 *          in the transit list of objects. The first object in the pair is the
 *          "front" object and the second is the "back" object.
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window to which the objects of the transit belong has already been created.
 *          This is because this effect needs the geometry information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @param[in] axis The flipping axis(x or y)
 * @param[in] cw The flipping direction \n
 *           @c EINA_TRUE is clock-wise.
 * @return The flip effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_flip_add(Elm_Transit *transit, Elm_Transit_Effect_Flip_Axis axis, Eina_Bool cw);

/**
 * @brief Adds the resizeable flip effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates resizable flip effect context
 *          and adds its required APIs to elm_transit_effect_add().
 * @remarks This effect is applied to each pair of objects in the order they are listed
 *          in the transit list of objects. The first object in the pair is the
 *          "front" object and the second is the "back" object.
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window to which the objects of the transit belong has already been created.
 *          This is because this effect needs the geometry information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @param[in] axis The flipping axis(x or y)
 * @param[in] cw The flipping direction \n
 *           @c EINA_TRUE is clock-wise.
 * @return The resizeable flip effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_resizable_flip_add(Elm_Transit *transit, Elm_Transit_Effect_Flip_Axis axis, Eina_Bool cw);

/**
 * @brief Adds the wipe effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates wipe effect context
 *          and adds its required APIs to elm_transit_effect_add().
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window to which the objects of the transit belong has already been created.
 *          This is because this effect needs the geometry information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @param[in] type The wipe type which is either hide or show
 * @param[in] dir The wipe direction
 * @return The wipe effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_wipe_add(Elm_Transit *transit, Elm_Transit_Effect_Wipe_Type type, Elm_Transit_Effect_Wipe_Dir dir);

/**
 * @brief Adds the color effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates color effect context
 *          and adds its required APIs to elm_transit_effect_add().
 *
 *
 * @param[in] transit        The transit object
 * @param[in]  from_r        The RGB from R when the effect begins
 * @param[in]  from_g        The RGB from G when the effect begins
 * @param[in]  from_b        The RGB from B when the effect begins
 * @param[in]  from_a        The RGB from A when the effect begins
 * @param[in]  to_r          The RGB to R when the effect ends
 * @param[in]  to_g          The RGB to G when the effect ends
 * @param[in]  to_b          The RGB to B when the effect ends
 * @param[in]  to_a          The RGB to A when the effect ends
 * @return               The color effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_color_add(Elm_Transit *transit, unsigned int from_r, unsigned int from_g, unsigned int from_b, unsigned int from_a, unsigned int to_r, unsigned int to_g, unsigned int to_b, unsigned int to_a);

/**
 * @brief Adds the fade effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates fade effect context
 *          and adds its required APIs to elm_transit_effect_add().
 * @remarks This effect is applied to each pair of objects in the order they are listed
 *          in the transit list of objects. The first object in the pair is the
 *          "before" object and the second is the "after" object.
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window to which the objects of the transit belong has already been created.
 *          This is because this effect needs the color information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @return The fade effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_fade_add(Elm_Transit *transit);

/**
 * @brief Adds the blend effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates blend effect context
 *          and adds its required APIs to elm_transit_effect_add().
 * @remarks This effect is applied to each pair of objects in the order they are listed
 *          in the transit list of objects. The first object in the pair is the
 *          "before" object and the second is the "after" object.
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window that the objects of the transit belongs has already been created.
 *          This is because this effect needs the color information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @return The blend effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_blend_add(Elm_Transit *transit);

/**
 * @brief Adds the rotation effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates rotation effect context
 *          and adds its required APIs to elm_transit_effect_add().
 *
 * @remarks It is highly recommended to just create a transit with this effect when
 *          the window to which the objects of the transit belong has already been created.
 *          This is because this effect needs the geometry information about the objects,
 *          and if the window is not created yet, it can get wrong information.
 *
 * @param[in] transit The transit object
 * @param[in] from_degree The degree when the effect begins
 * @param[in] to_degree The degree when the effect is ends
 * @return The rotation effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_rotation_add(Elm_Transit *transit, float from_degree, float to_degree);

/**
 * @brief Adds the image animation effect to Elm_Transit.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is one of the facades. It creates image animation effect context
 *          and adds its required APIs to elm_transit_effect_add.
 *          The @a images parameter is a list images paths. This list and
 *          its contents is deleted at the end of the effect by the
 *          elm_transit_effect_image_animation_context_free() function.
 *
 * Example:
 * @code
 * char buf[PATH_MAX];
 * Eina_List *images = NULL;
 * Elm_Transit *transi = elm_transit_add();
 *
 * snprintf(buf, sizeof(buf), "%s/images/icon_11.png", PACKAGE_DATA_DIR);
 * images = eina_list_append(images, eina_stringshare_add(buf));
 *
 * snprintf(buf, sizeof(buf), "%s/images/logo_small.png", PACKAGE_DATA_DIR);
 * images = eina_list_append(images, eina_stringshare_add(buf));
 * elm_transit_effect_image_animation_add(transi, images);
 *
 * @endcode
 *
 *
 * @param[in] transit The transit object
 * @param[in] images The Eina_List of images file paths \n
 *               This list and its contents are deleted at the end of the effect by the
 *               elm_transit_effect_image_animation_context_free() function.
 *
 * @return The image animation effect context data
 *
 * @see elm_transit_effect_add()
 */
EAPI Elm_Transit_Effect    *elm_transit_effect_image_animation_add(Elm_Transit *transit, Eina_List *images);

/**
 * @}
 */
