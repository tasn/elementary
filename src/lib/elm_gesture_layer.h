/**
 * @defgroup Elm_Gesture_Layer Gesture Layer
 * @ingroup elm_infra_group
 *
 * @brief This provides basic gesture detection.
 *
 * @image html gesture_layer_inheritance_tree.png
 * @image latex gesture_layer_inheritance_tree.eps
 *
 * Gesture Layer Usage:
 *
 * Use Gesture Layer to detect gestures.
 * The advantage is that you don't have to implement
 * gesture detection, just set callbacks of the gesture state.
 * By using the gesture layer we make a standard interface.
 *
 * In order to use Gesture Layer you start with @ref elm_gesture_layer_add
 * and a parent object parameter.
 * Next 'activate' the gesture layer with a call to @ref elm_gesture_layer_attach
 * Usually with the same object as target (2nd parameter).
 *
 * Now you need to tell the gesture layer what gestures you follow.
 * This is done with @ref elm_gesture_layer_cb_set call.
 * By setting the callback you are actually telling the gesture layer:
 * I would like to know when the gesture @ref Elm_Gesture_Type
 * switches to the state @ref Elm_Gesture_State.
 *
 * Next, you need to implement the actual action that follows the input
 * in your callback.
 *
 * Note that if you like to stop being reported about a gesture, just set
 * all callbacks referring to this gesture to @c NULL.
 * (again with @ref elm_gesture_layer_cb_set)
 *
 * The information reported by the gesture layer to your callback is depending
 * on @ref Elm_Gesture_Type :
 * @ref Elm_Gesture_Taps_Info is the info reported for tap gestures:
 * @ref ELM_GESTURE_N_TAPS, @ref ELM_GESTURE_N_LONG_TAPS,
 * @ref ELM_GESTURE_N_DOUBLE_TAPS, @ref ELM_GESTURE_N_TRIPLE_TAPS.
 *
 * @ref Elm_Gesture_Momentum_Info is the info reported for momentum gestures:
 * @ref ELM_GESTURE_MOMENTUM.
 *
 * @ref Elm_Gesture_Line_Info is the info reported for line gestures:
 * (this also contains the @ref Elm_Gesture_Momentum_Info internal structure)
 * @ref ELM_GESTURE_N_LINES, @ref ELM_GESTURE_N_FLICKS.
 * Note that we consider a flick as a line-gesture that should be completed
 * in a flick-time-limit as defined in @ref Config.
 *
 * @ref Elm_Gesture_Zoom_Info is the info reported for the @ref ELM_GESTURE_ZOOM gesture.
 *
 * @ref Elm_Gesture_Rotate_Info is the info reported for the @ref ELM_GESTURE_ROTATE gesture.
 *
 *
 * Gesture Layer Tweaks:
 *
 * Note that line, flick, gestures can start without the need to remove fingers from the surface.
 * When the user's fingers rest on the same-spot, the gesture ends and starts again when the fingers are moved.
 *
 * Setting glayer_continues_enable to @c false in @ref Config changes this behavior,
 * so the gesture starts when the user touches (a *DOWN event) touch-surface
 * and ends when no fingers touch the surface (a *UP event).
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 *
 * @{
 *
 */

/**
 * @enum _Elm_Gesture_Type
 * @brief Enumeration of supported gesture types.
 */
enum _Elm_Gesture_Type
{
   ELM_GESTURE_FIRST = 0,

   ELM_GESTURE_N_TAPS, /**< N fingers single taps */
   ELM_GESTURE_N_LONG_TAPS, /**< N fingers single long-taps */
   ELM_GESTURE_N_DOUBLE_TAPS, /**< N fingers double-single taps */
   ELM_GESTURE_N_TRIPLE_TAPS, /**< N fingers triple-single taps */

   ELM_GESTURE_MOMENTUM, /**< Reports momentum in the direction of movement */

   ELM_GESTURE_N_LINES, /**< N fingers line gesture */
   ELM_GESTURE_N_FLICKS, /**< N fingers flick gesture */

   ELM_GESTURE_ZOOM, /**< Zoom */
   ELM_GESTURE_ROTATE, /**< Rotate */

   ELM_GESTURE_LAST
};

/**
 * @typedef Elm_Gesture_Type
 * @brief This is a convenient macro around #_Elm_Gesture_Type.
 */
typedef enum _Elm_Gesture_Type Elm_Gesture_Type;

/**
 * @enum _Elm_Gesture_State
 * @brief Enumeration of gesture states.
 */
enum _Elm_Gesture_State
{
   ELM_GESTURE_STATE_UNDEFINED = -1, /**< Gesture not STARTed */
   ELM_GESTURE_STATE_START, /**< Gesture STARTed     */
   ELM_GESTURE_STATE_MOVE, /**< Gesture is ongoing  */
   ELM_GESTURE_STATE_END, /**< Gesture has completed   */
   ELM_GESTURE_STATE_ABORT /**< Ongoing gesture has been ABORTed */
};

/**
 * @typedef Elm_Gesture_State
 * @brief This is a convenient macro around #_Elm_Gesture_State
 */
typedef enum _Elm_Gesture_State Elm_Gesture_State;

/**
 * @struct _Elm_Gesture_Taps_Info
 * @brief The structure type that holds taps info for the user.
 */
struct _Elm_Gesture_Taps_Info
{
   Evas_Coord   x, y; /**< Holds center point between fingers */
   unsigned int n; /**< Number of fingers tapped           */
   unsigned int timestamp; /**< Event timestamp       */
};

/**
 * @typedef Elm_Gesture_Taps_Info
 * @brief The structure type that holds taps info for the user.
 */
typedef struct _Elm_Gesture_Taps_Info Elm_Gesture_Taps_Info;

/**
 * @struct _Elm_Gesture_Momentum_Info
 * @brief The structure type that holds momentum info for the user.
 *        x1 and y1 are not necessarily in sync
 *        x1 holds the x value of the x direction starting point
 *        and same holds for y1.
 *        This is noticeable when doing V-shape movement.
 */
struct _Elm_Gesture_Momentum_Info /* Report line ends, timestamps, and momentum computed        */
{Evas_Coord   x1; /**< Final-swipe direction with starting point on X */
 Evas_Coord   y1; /**< Final-swipe direction with starting point on Y */
 Evas_Coord   x2; /**< Final-swipe direction with ending point on X   */
 Evas_Coord   y2; /**< Final-swipe direction with ending point on Y   */

 unsigned int tx; /**< Timestamp of the start of the final x-swipe */
 unsigned int ty; /**< Timestamp of the start of the final y-swipe */

 Evas_Coord   mx; /**< Momentum on X */
 Evas_Coord   my; /**< Momentum on Y */

 unsigned int n; /**< Number of fingers */
};

/**
 * @typedef Elm_Gesture_Momentum_Info
 * @brief The structure type that holds momentum info for the user.
 */
typedef struct _Elm_Gesture_Momentum_Info Elm_Gesture_Momentum_Info;

/**
 * @struct _Elm_Gesture_Line_Info
 * @brief The structure type that holds line info for the user.
 */
struct _Elm_Gesture_Line_Info   /* Report line ends, timestamps, and momentum computed      */
{Elm_Gesture_Momentum_Info momentum; /**< Line momentum info */
 double                    angle; /**< Angle (direction) of the lines  */
};

/**
 * @typedef Elm_Gesture_Line_Info
 * @brief The structure type that holds line info for the user.
 */
typedef struct _Elm_Gesture_Line_Info Elm_Gesture_Line_Info;

/**
 * @struct _Elm_Gesture_Zoom_Info
 * @brief The structure type that holds zoom info for the user.
 */
struct _Elm_Gesture_Zoom_Info
{
   Evas_Coord x, y; /**< Holds zoom center point reported to the user  */
   Evas_Coord radius; /**< Holds radius between fingers reported to the user */
   double     zoom; /**< Zoom value: @c 1.0 means no zoom             */
   double     momentum; /**< Zoom momentum: zoom growth per second (NOT YET SUPPORTED) */
};

/**
 * @typedef Elm_Gesture_Zoom_Info
 * @brief The structure type that holds zoom info for the user.
 */
typedef struct _Elm_Gesture_Zoom_Info Elm_Gesture_Zoom_Info;

/**
 * @struct _Elm_Gesture_Rotate_Info
 * @brief The structure type that holds rotation info for the user.
 */
struct _Elm_Gesture_Rotate_Info
{
   Evas_Coord x, y; /**< Holds zoom center point reported to the user      */
   Evas_Coord radius; /**< Holds radius between fingers reported to the user */
   double     base_angle; /**< Holds start-angle */
   double     angle; /**< Rotation value: @c 0.0 means no rotation         */
   double     momentum; /**< Rotation momentum: rotation done per second (NOT YET SUPPORTED) */
};

/**
 * @typedef Elm_Gesture_Rotate_Info
 * @brief The structure type that holds rotation info for the user.
 */
typedef struct _Elm_Gesture_Rotate_Info Elm_Gesture_Rotate_Info;

/**
 * @typedef Elm_Gesture_Event_Cb
 * @brief User callback used to stream gesture info from the gesture layer.
 *
 * @remarks You should probably return EVAS_EVENT_FLAG_ON_HOLD if your widget acted
 *          upon the event, in an irreversible way.
 *
 * @param data The user data
 * @param event_info The gesture report info
 * @return The flag field to be applied on the causing event
 *
 */
typedef Evas_Event_Flags (*Elm_Gesture_Event_Cb)(void *data, void *event_info);

/**
 * @brief Sets callbacks to be notified about the
 *        change of state of a gesture.
 *
 * @since_tizen 2.3
 *
 * @remarks When a user registers a callback with this function
 *          it means this gesture has to be tested.
 *
 * @remarks When ALL callbacks for a gesture are set to @c NULL
 *          it means the user isn't interested in gesture-state
 *          and it is not tested.
 *
 * @param[in] obj The gesture-layer
 * @param[in] idx The gesture whose state is tracked
 * @param[in] cb The callback function pointer
 * @param[in] cb_type The event type this callback tracks: START, MOVE, END, ABORT
 * @param[in] data The user info to be sent to the callback (usually, Smart Data)
 *
 */
EAPI void         elm_gesture_layer_cb_set(Evas_Object *obj, Elm_Gesture_Type idx, Elm_Gesture_State cb_type, Elm_Gesture_Event_Cb cb, void *data);

/**
 * @brief Called to get repeat-events settings.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 *
 * @return The repeat events settings
 * @see elm_gesture_layer_hold_events_set()
 */
EAPI Eina_Bool    elm_gesture_layer_hold_events_get(const Evas_Object *obj);

/**
 * @brief Makes gesture-layer repeat events.
 *
 * @since_tizen 2.3
 *
 * @remarks Set this if you like to get the raw events only if gestures were not
 *          detected.
 * @remarks Clear this if you like the gesture layer to forward events as testing gestures.
 *
 * @param[in] obj The gesture layer
 * @param[in] hold_events The boolean value that indicates whether events are held
 *
 */
EAPI void         elm_gesture_layer_hold_events_set(Evas_Object *obj, Eina_Bool hold_events);

/**
 * @brief Sets the step-value for zoom action.
 *
 * @since_tizen 2.3
 *
 * @remarks Set the step to any positive value.
 * @remarks Cancel step setting by setting it to @c 0.
 *
 * @param[in] obj The gesture-layer
 * @param[in] step The new zoom step value
 *
 * @see elm_gesture_layer_zoom_step_get()
 */
EAPI void         elm_gesture_layer_zoom_step_set(Evas_Object *obj, double step);

/**
 * @brief Gets the step-value for zoom action.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The zoom step value
 *
 * @see elm_gesture_layer_zoom_step_set()
 */
EAPI double       elm_gesture_layer_zoom_step_get(const Evas_Object *obj);

/**
 * @brief Sets the step-value for rotate action.
 *
 * @since_tizen 2.3
 *
 * @remarks Set the step to any positive value.
 * @remarks Cancel the step setting by setting to @c 0.
 *
 * @param[in] obj The gesture-layer
 * @param[in] step The new rotate step value
 *
 */
EAPI void         elm_gesture_layer_rotate_step_set(Evas_Object *obj, double step);

/**
 * @brief Gets the step-value for rotate action.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The rotate step value
 *
 */
EAPI double       elm_gesture_layer_rotate_step_get(const Evas_Object *obj);

/**
 * @brief Attaches a given gesture layer widget to an Evas object, thus setting
 *        the widget's @b target.
 *
 * @since_tizen 2.3
 *
 * @remarks A gesture layer target may be whichever Evas object one
 *          chooses. This is the object @a obj that listens to all mouse and key
 *          events and reports the gestures made upon it.
 *
 *
 * @param[in] obj The gesture layer to attach an object to
 * @param[in] target The object to attach to @a obj (target)
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 */
EAPI Eina_Bool    elm_gesture_layer_attach(Evas_Object *obj, Evas_Object *target);

/**
 * @brief Called to construct a new gesture-layer object.
 *
 * @since_tizen 2.3
 *
 * @remarks This does not activate the gesture layer. You have to
 *          call elm_gesture_layer_attach() in order to 'activate' gesture-layer.
 *
 * @param[in] parent The gesture layer's parent widget
 *
 * @return A new gesture layer object
 *
 */
EAPI Evas_Object *elm_gesture_layer_add(Evas_Object *parent);

/**
 * @brief Sets the gesture layer line min length of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] line_min_length The length
 *
 */
EAPI void elm_gesture_layer_line_min_length_set(Evas_Object *obj, int line_min_length);

/**
 * @brief Gets the gesture layer line min length of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The length
 *
 */
EAPI int elm_gesture_layer_line_min_length_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer zoom distance tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] zoom_distance_tolerance The zoom distance tolerance
 *
 */
EAPI void elm_gesture_layer_zoom_distance_tolerance_set(Evas_Object *obj, Evas_Coord zoom_distance_tolerance);

/**
 * @brief Gets the gesture layer zoom distance tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The zoom distance tolerance
 *
 */
EAPI Evas_Coord elm_gesture_layer_zoom_distance_tolerance_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer line distance tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] line_distance_tolerance The line distance tolerance
 *
 */
EAPI void elm_gesture_layer_line_distance_tolerance_set(Evas_Object *obj, Evas_Coord line_distance_tolerance);

/**
 * @brief Gets the gesture layer line distance tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The line distance tolerance
 *
 */
EAPI Evas_Coord elm_gesture_layer_line_distance_tolerance_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer line angular tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] line_angular_tolerance The line angular tolerance
 *
 */
EAPI void elm_gesture_layer_line_angular_tolerance_set(Evas_Object *obj, double line_angular_tolerance);

/**
 * @brief Gets the gesture layer line angular tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The line angular tolerance
 *
 */
EAPI double elm_gesture_layer_line_angular_tolerance_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer zoom wheel factor of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] zoom_wheel_factor The zoom wheel factor
 *
 */
EAPI void elm_gesture_layer_zoom_wheel_factor_set(Evas_Object *obj, double zoom_wheel_factor);

/**
 * @brief Gets the gesture layer zoom wheel factor of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The zoom wheel factor
 *
 */
EAPI double elm_gesture_layer_zoom_wheel_factor_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer zoom finger factor of an object
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] zoom_finger_factor The zoom finger factor
 *
 */
EAPI void elm_gesture_layer_zoom_finger_factor_set(Evas_Object *obj, double zoom_finger_factor);

/**
 * @brief Gets the gesture layer zoom finger factor of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The zoom finger factor
 *
 */
EAPI double elm_gesture_layer_zoom_finger_factor_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer rotate angular tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] rotate_angular_tolerance The rotate angular tolerance
 *
 */
EAPI void elm_gesture_layer_rotate_angular_tolerance_set(Evas_Object *obj, double rotate_angular_tolerance);

/**
 * @brief Gets the gesture layer rotate angular tolerance of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The rotate angular tolerance
 *
 */
EAPI double elm_gesture_layer_rotate_angular_tolerance_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer flick time limit (in ms) of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] flick_time_limit_ms The flick time limit (in ms)
 *
 */
EAPI void elm_gesture_layer_flick_time_limit_ms_set(Evas_Object *obj, unsigned int flick_time_limit_ms);

/**
 * @brief Gets the gesture layer flick time limit (in ms) of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The flick time limit (in ms)
 *
 */
EAPI unsigned int elm_gesture_layer_flick_time_limit_ms_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer long tap start timeout of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] long_tap_start_timeout The long tap start timeout
 *
 */
EAPI void elm_gesture_layer_long_tap_start_timeout_set(Evas_Object *obj, double long_tap_start_timeout);

/**
 * @brief Gets the gesture layer long tap start timeout of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The long tap start timeout
 *
 */
EAPI double elm_gesture_layer_long_tap_start_timeout_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer continues enable of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] continues_enable The continues enable
 *
 */
EAPI void elm_gesture_layer_continues_enable_set(Evas_Object *obj, Eina_Bool continues_enable);

/**
 * @brief Gets the gesture layer continues enable of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The continues enable
 *
 */
EAPI Eina_Bool elm_gesture_layer_continues_enable_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer double tap timeout of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @param[in] double_tap_timeout The double tap timeout
 *
 */
EAPI void elm_gesture_layer_double_tap_timeout_set(Evas_Object *obj, double double_tap_timeout);

/**
 * @brief Gets the gesture layer double tap timeout of an object.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The double tap timeout
 *
 */
EAPI double elm_gesture_layer_double_tap_timeout_get(const Evas_Object *obj);

/**
 * @brief Sets the gesture layer finger-size for taps.
 *
 * @since_tizen 2.3
 *
 * @remarks If not set, this size is taken from elm_config.
 *          Set to ZERO if you want GLayer to use system finger size value (default)
 *          It is recommended to not set a very big or small value to avoid weird
 *          behaviors.
 *
 * @param[in] obj The gesture-layer
 * @param[in] sz The finger size
 *
 */
EAPI void elm_gesture_layer_tap_finger_size_set(Evas_Object *obj, Evas_Coord sz);

/**
 * @brief Gets the gesture layer finger-size for taps.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The gesture-layer
 * @return The finger size that is currently used by the Gesture Layer for taps
 *
 */
EAPI Evas_Coord elm_gesture_layer_tap_finger_size_get(const Evas_Object *obj);

/**
 * @}
 */
