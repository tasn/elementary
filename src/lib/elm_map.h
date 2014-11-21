/**
 * @defgroup Map Map
 * @ingroup elm_widget_group
 *
 * @image html map_inheritance_tree.png
 * @image latex map_inheritance_tree.eps
 *
 * @brief This is a widget specifically for displaying a map.
 *
 * It basically uses the OpenStreetMap provider http://www.openstreetmap.org/,
 * but custom providers can be added.
 *
 * It supports some basic yet nice features:
 * @li Zooming and scrolling
 * @li Markers with content to be displayed when a user clicks on them
 * @li Group of markers and
 * @li Routes
 *
 * This widget implements the elm-scrollable-interface
 * interface, so that all (non-deprecated) functions for the base @ref
 * Scroller widget also work for map objects.
 *
 * Smart callbacks that one can listen to:
 * - @c "clicked" - This is called when a user has clicked the map without
 *                  dragging it around.
 * - @c "clicked,double" - This is called when a user has double-clicked
 *                         the map.
 * - @c "press" - This is called when a user has pressed down on the map.
 * - @c "longpressed" - This is called when a user has pressed down on the map
 *      for a long time without dragging it around.
 * - @c "scroll" - The content has been scrolled (moved).
 * - @c "scroll,drag,start" - Dragging the content around has started.
 * - @c "scroll,drag,stop" - Dragging the content around has stopped.
 * - @c "scroll,anim,start" - Scrolling animation has started.
 * - @c "scroll,anim,stop" - Scrolling animation has stopped.
 * - @c "zoom,start" - Zoom animation has started.
 * - @c "zoom,stop" - Zoom animation has stopped.
 * - @c "zoom,change" - Zoom changed when using the auto zoom mode.
 * - @c "tile,load" - A map tile image load begins.
 * - @c "tile,loaded" -  A map tile image load ends.
 * - @c "tile,loaded,fail" -  A map tile image load fails.
 * - @c "route,load" - Route request begins.
 * - @c "route,loaded" - Route request ends.
 * - @c "route,loaded,fail" - Route request fails.
 * - @c "name,load" - Name request begins.
 * - @c "name,loaded" - Name request ends.
 * - @c "name,loaded,fail" - Name request fails.
 * - @c "overlay,clicked" - An overlay is clicked.
 * - @c "loaded" - The map is finally loaded. @since 1.7
 * - @c "language,changed" - The program's language has changed.
 *
 * Available style for map widgets:
 * - @c "default"
 *
 * Available styles for markers:
 * - @c "radio"
 * - @c "radio2"
 * - @c "empty"
 *
 * Available style for marker bubbles:
 * - @c "default"
 *
 * @{
 */

/**
 * @brief Sets the map's zoom behavior. It can be set to manual or automatic.
 *
 * @remarks The default value is #ELM_MAP_ZOOM_MODE_MANUAL.
 *
 * @remarks Values <b> don't </b> work as bitmask, only one can be chosen.
 *
 * @remarks Valid sizes are 2^zoom, consequently the map may be smaller
 *          than the scroller view.
 *
 * @see elm_map_zoom_mode_set()
 * @see elm_map_zoom_mode_get()
 */
typedef enum
{
   ELM_MAP_ZOOM_MODE_MANUAL,      /**< Zoom controlled manually by elm_map_zoom_set(). It's set by default */
   ELM_MAP_ZOOM_MODE_AUTO_FIT,    /**< Zoom until the map fits inside the scroll frame with no pixels outside this area */
   ELM_MAP_ZOOM_MODE_AUTO_FILL,   /**< Zoom until the map fills the scroll, ensuring that no pixels are left unfilled */
   ELM_MAP_ZOOM_MODE_LAST
} Elm_Map_Zoom_Mode;

/**
 * @brief Enumeration that sets the type of an external source (provider).
 *
 * @see elm_map_sources_get()
 * @see elm_map_source_get()
 * @see elm_map_source_set()
 */
typedef enum
{
   ELM_MAP_SOURCE_TYPE_TILE,   /**< Map tile provider */
   ELM_MAP_SOURCE_TYPE_ROUTE,  /**< Route service provider */
   ELM_MAP_SOURCE_TYPE_NAME,   /**< Name service provider */
   ELM_MAP_SOURCE_TYPE_LAST
} Elm_Map_Source_Type;

/**
 * @brief Enumeration that sets the type of transport used on a route.
 *
 * @see elm_map_route_add()
 */
typedef enum
{
   ELM_MAP_ROUTE_TYPE_MOTOCAR,   /**< Route should consider that an automobile is used */
   ELM_MAP_ROUTE_TYPE_BICYCLE,   /**< Route should consider that a bicycle is used by the user */
   ELM_MAP_ROUTE_TYPE_FOOT,      /**< Route should consider that the user is walking */
   ELM_MAP_ROUTE_TYPE_LAST
} Elm_Map_Route_Type;

/**
 * @brief Enumeration that sets the routing method. It decides what should be prioritized, time or distance.
 *
 * @see elm_map_route_add()
 */
typedef enum
{
   ELM_MAP_ROUTE_METHOD_FASTEST,  /**< Route should prioritize time */
   ELM_MAP_ROUTE_METHOD_SHORTEST, /**< Route should prioritize distance */
   ELM_MAP_ROUTE_METHOD_LAST
} Elm_Map_Route_Method;

/**
 * @brief Enumeration that sets the name search method.
 *
 * This is for the name module interface.
 */
typedef enum
{
   ELM_MAP_NAME_METHOD_SEARCH, 
   ELM_MAP_NAME_METHOD_REVERSE, 
   ELM_MAP_NAME_METHOD_LAST     
} Elm_Map_Name_Method;

/**
 * @brief Enumeration that sets the overlay type to be used. This type is resolved
 *        when the overlay is created.
 * @remarks You can get this value by elm_map_overlay_type_get().
 *
 * @see elm_map_overlay_type_get()
 * @see elm_map_overlay_add()
 * @see elm_map_overlay_class_add()
 * @see elm_map_overlay_bubble_add()
 */
typedef enum _Elm_Map_Overlay_Type
{
   ELM_MAP_OVERLAY_TYPE_NONE = 0,
   ELM_MAP_OVERLAY_TYPE_DEFAULT,
   ELM_MAP_OVERLAY_TYPE_CLASS,
   ELM_MAP_OVERLAY_TYPE_GROUP,
   ELM_MAP_OVERLAY_TYPE_BUBBLE,
   ELM_MAP_OVERLAY_TYPE_ROUTE,
   ELM_MAP_OVERLAY_TYPE_LINE,
   ELM_MAP_OVERLAY_TYPE_POLYGON,
   ELM_MAP_OVERLAY_TYPE_CIRCLE,
   ELM_MAP_OVERLAY_TYPE_SCALE,
   ELM_MAP_OVERLAY_TYPE_POLYLINE
} Elm_Map_Overlay_Type;

typedef struct _Elm_Map_Marker       Elm_Map_Marker;       /**< A marker to be shown at a specific point on the map. Can be created with elm_map_marker_add() and deleted with elm_map_marker_remove() */
typedef struct _Elm_Map_Marker_Class Elm_Map_Marker_Class; /**< Each marker must be associated to a class. It's required to add a mark. The class defines the style of the marker when a marker is displayed alone (not grouped). A new class can be created with elm_map_marker_class_new() */
typedef struct _Elm_Map_Group_Class  Elm_Map_Group_Class;  /**< Each marker must be associated to a group class. It's required to add a mark. The group class defines the style of the marker when a marker is grouped to other markers. Markers with the same group are grouped if they are close. A new group class can be created with elm_map_marker_group_class_new() */
typedef struct _Elm_Map_Route        Elm_Map_Route;        /**< A route to be shown on the map. Can be created with elm_map_route_add() and deleted with elm_map_route_del() */
typedef struct _Elm_Map_Name         Elm_Map_Name;         /**< A handle for specific coordinates */
typedef struct _Elm_Map_Overlay      Elm_Map_Overlay;      /**< An overlay to be shown at a specific point on the map. This can be created by elm_map_overlay_add() and other similar functions, and deleted by elm_map_overlay_del() */
typedef struct _Elm_Map_Region       Elm_Map_Region;       /**< A geographical point specified by latitude, longitude and altitude values.*/

typedef Evas_Object               *(*Elm_Map_Marker_Get_Func)(Evas_Object *obj, Elm_Map_Marker *marker, void *data); /**< Bubble content fetching class function for marker classes. When the user clicks on a marker, a bubble is displayed with content */
typedef void                       (*Elm_Map_Marker_Del_Func)(Evas_Object *obj, Elm_Map_Marker *marker, void *data, Evas_Object *o); /**< Function to delete bubble content for marker classes */
typedef Evas_Object               *(*Elm_Map_Marker_Icon_Get_Func)(Evas_Object *obj, Elm_Map_Marker *marker, void *data); /**< Icon fetching class function for marker classes */
typedef Evas_Object               *(*Elm_Map_Group_Icon_Get_Func)(Evas_Object *obj, void *data); /**< Icon fetching class function for marker group classes */

typedef void                       (*Elm_Map_Overlay_Get_Cb)(void *data, Evas_Object *map, Elm_Map_Overlay *overlay);   /**< Get callback function for the overlay */
typedef void                       (*Elm_Map_Overlay_Del_Cb)(void *data, Evas_Object *map, Elm_Map_Overlay *overlay);   /**< Del callback function for the overlay @since 1.7 */
typedef void                       (*Elm_Map_Name_Cb)(void *data, Evas_Object *map, Elm_Map_Name *name);                /**< Async-callback function for the name request */
typedef void                       (*Elm_Map_Name_List_Cb)(void *data, Evas_Object *map, Eina_List *name_list);                /**< Async-callback function for the name list request */
typedef void                       (*Elm_Map_Route_Cb)(void *data, Evas_Object *map, Elm_Map_Route *route);             /**< Async-callback function for the route request */

typedef void                       (*Elm_Map_Capture_Result_Cb)(Evas_Object*, void*);/** Async ofscreen map image capture function */

/**
 * @brief Adds a new map widget to the given parent Elementary (container) object.
 *
 * @details This function inserts a new map widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new map widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object          *elm_map_add(Evas_Object *parent);

/**
 * @brief Sets the zoom level of the map.
 *
 * @details This sets the zoom level.
 *
 * @since_tizen 2.3
 *
 * @remarks It respects the limits defined by elm_map_zoom_min_set() and
 *          elm_map_zoom_max_set().
 *
 * @remarks By default these values are @c 0 (world map) and @c 18 (maximum zoom).
 *
 * @remarks This function should be used when the zoom mode is set to #ELM_MAP_ZOOM_MODE_MANUAL.
 *          This is the default mode, and can be set with elm_map_zoom_mode_set().
 *
 *
 * @param[in] obj The map object
 * @param[in] zoom The zoom level to set
 *
 * @see elm_map_zoom_mode_set()
 * @see elm_map_zoom_get()
 */
EAPI void                  elm_map_zoom_set(Evas_Object *obj, int zoom);

/**
 * @brief Gets the zoom level of the map.
 *
 * @details This returns the current zoom level of the map object.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that if you set the fill mode to a value other than #ELM_MAP_ZOOM_MODE_MANUAL
 *          (which is the default), the zoom level may be changed at any time by the
 *          map object itself in order to account for the map size and map viewport size.
 *
 * @param[in] obj The map object
 * @return The current zoom level
 *
 * @see elm_map_zoom_set()
 */
EAPI int                   elm_map_zoom_get(const Evas_Object *obj);

/**
 * @brief Sets the zoom mode used by the map object.
 *
 * @since_tizen 2.3
 *
 * @details This sets the zoom mode to manual or one of the automatic levels.
 *          Manual (#ELM_MAP_ZOOM_MODE_MANUAL) means that zoom is set manually by
 *          elm_map_zoom_set() and stays at that level until changed by code
 *          or until the zoom mode is changed. This is the default mode.
 *
 * @remarks The Automatic modes allow the map object to automatically
 *          adjust the zoom mode based on properties. #ELM_MAP_ZOOM_MODE_AUTO_FIT
 *          adjusts the zoom so that the map fits inside the scroll frame with no pixels
 *          outside this area. #ELM_MAP_ZOOM_MODE_AUTO_FILL is similar but
 *          ensure that no pixels within the frame are left unfilled. Do not forget that
 *          the valid sizes are 2^zoom. Consequently, the map may be smaller than
 *          the scroller view.
 *
 * @param[in] obj The map object
 * @param[in] mode The zoom mode of the map, one from #ELM_MAP_ZOOM_MODE_MANUAL
 *             (default), #ELM_MAP_ZOOM_MODE_AUTO_FIT, or #ELM_MAP_ZOOM_MODE_AUTO_FILL
 *
 * @see elm_map_zoom_set()
 */
EAPI void                  elm_map_zoom_mode_set(Evas_Object *obj, Elm_Map_Zoom_Mode mode);

/**
 * @brief Gets the zoom mode used by the map object.
 *
 * @details This function returns the current zoom mode used by the map object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @return The zoom mode of the map, one from #ELM_MAP_ZOOM_MODE_MANUAL
 *         (default), #ELM_MAP_ZOOM_MODE_AUTO_FIT, or #ELM_MAP_ZOOM_MODE_AUTO_FILL
 *
 * @see elm_map_zoom_mode_set()
 */
EAPI Elm_Map_Zoom_Mode     elm_map_zoom_mode_get(const Evas_Object *obj);

/**
 * @brief Sets the minimum zoom of the source.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] zoom The new minimum zoom value to be used
 *
 * @see elm_map_zoom_min_get()
 */
EAPI void                  elm_map_zoom_min_set(Evas_Object *obj, int zoom);

/**
 * @brief Gets the minimum zoom of the source.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @return The minimum zoom of the source
 *
 * @see elm_map_zoom_min_set()
 */
EAPI int                   elm_map_zoom_min_get(const Evas_Object *obj);

/**
 * @brief Sets the maximum zoom of the source.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] zoom The new maximum zoom value to be used
 *
 * @see elm_map_zoom_max_get()
 */
EAPI void                  elm_map_zoom_max_set(Evas_Object *obj, int zoom);

/**
 * @brief Gets the maximum zoom of the source.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @return The maximum zoom of the source
 *
 * @see elm_map_zoom_max_set()
 */
EAPI int                   elm_map_zoom_max_get(const Evas_Object *obj);

/**
 * @brief Gets the current geographic coordinates of the map.
 *
 * @details This gets the current center coordinates of the map object. It can be
 *          set by elm_map_region_bring_in() and elm_map_region_show().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[out] lon The pointer that stores the longitude
 * @param[out] lat The pointer that stores the latitude
 *
 * @see elm_map_region_bring_in()
 * @see elm_map_region_show()
 */
EAPI void                  elm_map_region_get(const Evas_Object *obj, double *lon, double *lat);

/**
 * @brief Animatedly brings the given coordinates to the center of the map.
 *
 * @since_tizen 2.3
 *
 * @remarks This causes the map to jump to the given @a lat and @a lon coordinates
 *          and display it (by scrolling) in the center of the viewport, if it is not
 *          already centered. This uses animation to do so and takes a period
 *          of time to complete.
 *
 * @param[in] obj The map object
 * @param[in] lon The longitude to bring to the center
 * @param[in] lat The latitude to bring to the center
 *
 * @see elm_map_region_show() for a function to avoid animation.
 * @see elm_map_region_get()
 */
EAPI void                  elm_map_region_bring_in(Evas_Object *obj, double lon, double lat);

/**
 * @brief Shows the given coordinates at the center of the map, @b immediately.
 *
 * @since_tizen 2.3
 *
 * @remarks This causes the map to @b redraw its viewport's contents to the
 *          region containing the given @a lat and @a lon, that are moved to the
 *          center of the map.
 *
 * @param[in] obj The map object
 * @param[in] lon The longitude to show at the center
 * @param[in] lat The latitude to show at the center
 *
 * @see elm_map_region_bring_in() for a function to move with animation.
 * @see elm_map_region_get()
 */
EAPI void                  elm_map_region_show(Evas_Object *obj, double lon, double lat);

/**
 * @brief Converts the canvas coordinates into geographic coordinates
 *        (longitude, latitude).
 *
 * @details This gets the longitude and latitude from the x, y coordinates of the canvas. The canvas
 *          coordinates mean x, y coordinates from the current viewport.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] x   The horizontal coordinate of the point to convert
 * @param[in] y   The vertical coordinate of the point to convert
 * @param[in] lon A pointer to the longitude
 * @param[in] lat A pointer to the latitude
 *
 * see elm_map_region_to_canvas_convert()
 */
EAPI void                  elm_map_canvas_to_region_convert(const Evas_Object *obj, const Evas_Coord x, const Evas_Coord y, double *lon, double *lat);

/**
 * @brief Converts the geographic coordinates (longitude, latitude)
 *        into canvas coordinates.
 *
 * @details This gets x, y coordinates of the canvas from the longitude and latitude. The canvas
 *          coordinates mean x, y coordinates from the current viewport.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] lon The longitude to convert
 * @param[in] lat The latitude to convert
 * @param[out] x   A pointer to the horizontal coordinate
 * @param[out] y   A pointer to the vertical coordinate
 *
 * see elm_map_canvas_to_region_convert()
 */
EAPI void                  elm_map_region_to_canvas_convert(const Evas_Object *obj, double lon, double lat, Evas_Coord *x, Evas_Coord *y);

/**
 * @brief Pauses or unpauses the map.
 *
 * @details This sets the paused state to on (@c EINA_TRUE) or off (@c EINA_FALSE)
 *          for the map.
 *
 * @since_tizen 2.3
 *
 * @remarks The default is off.
 *
 * @remarks This stops zooming using animation, changing zoom levels
 *          change instantly. This stops any existing animations that are running.
 *
 * @param[in] obj The map object
 * @param[in] paused If @c EINA_TRUE the map @a obj is paused,
 *               otherwise @c EINA_FALSE if it is unpaused
 *
 * @see elm_map_paused_get()
 */
EAPI void                  elm_map_paused_set(Evas_Object *obj, Eina_Bool paused);

/**
 * @brief Gets a value that indicates whether the map is paused.
 *
 * @details This gets the current paused state for the map object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @return @c EINA_TRUE means that the map is paused, otherwise @c EINA_FALSE indicates
 *         that it is not
 *
 * @see elm_map_paused_set()
 */
EAPI Eina_Bool             elm_map_paused_get(const Evas_Object *obj);

/**
 * @brief Rotates the map.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] degree The angle from @c 0.0 to @c 360.0 to rotate around the Z axis
 * @param[in] cx The rotation's center horizontal position
 * @param[in] cy The rotation's center vertical position
 *
 * @see elm_map_rotate_get()
 */
EAPI void                  elm_map_rotate_set(Evas_Object *obj, double degree, Evas_Coord cx, Evas_Coord cy);

/**
 * @brief Gets the rotate degree of the map.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[out] degree The pointer that stores degrees from @c 0.0 to @c 360.0
 *               to rotate around the Z axis
 * @param[out] cx The pointer that stores the rotation's center horizontal position
 * @param[out] cy The pointer that stores the rotation's center vertical position
 *
 * @see elm_map_rotate_set() to set map rotation.
 */
EAPI void                  elm_map_rotate_get(const Evas_Object *obj, double *degree, Evas_Coord *cx, Evas_Coord *cy);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Enables or disables the perspective feature for 3D view.
 *
 * @param[in] obj The map object
 * @param[in] enabled If @c EINA_TRUE the perspective feature is enabled,
 *                 otherwise @c EINA_FALSE if it is disabled
 *
 * @see elm_map_perspective_enabled_get()
 */
EAPI void                  elm_map_perspective_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets a value that indicates whether the perspective feature is enabled.
 *
 * @param[in] obj The map object
 * @return @c EINA_TRUE indicates that the perspective feature is enabled, otherwise @c EINA_FALSE indicates
 *         that it is disabled
 *
 * @see elm_map_perspective_enabled_set()
 */
EAPI Eina_Bool             elm_map_perspective_enabled_get(const Evas_Object *obj);

/**
 * @brief Enables or disables the mouse wheel to be used to zoom in / out the map.
 *
 * @since_tizen 2.3
 *
 * @remarks Mouse wheel can be used for the user to zoom in or zoom out of the map.
 *
 * @remarks It's disabled by default.
 *
 * @param[in] obj The map object
 * @param[in] disabled If @c EINA_TRUE the mouse wheel is disabled,
 *                 otherwise @c EINA_FALSE if it is enabled
 *
 * @see elm_map_wheel_disabled_get()
 */
EAPI void                  elm_map_wheel_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Gets a value that indicates whether the mouse wheel is enabled.
 *
 * @since_tizen 2.3
 *
 * @remarks Mouse wheel can be used for the user to zoom in or zoom out of the map.
 *
 * @param[in] obj The map object
 * @return @c EINA_TRUE indicates that the map is disabled, otherwise @c EINA_FALSE indicates
 *         that it is enabled
 *
 * @see elm_map_wheel_disabled_set()
 */
EAPI Eina_Bool             elm_map_wheel_disabled_get(const Evas_Object *obj);

/**
 * @brief Sets the user agent used by the map object to access routing services.
 *
 * @since_tizen 2.3
 *
 * @remarks User agent is a client application implementing a network protocol used
 *          in communications within a client–server distributed computing system.
 *
 * @remarks The @a user_agent identification string is transmitted in a header
 *          field @c User-Agent.
 *
 * @param[in] obj The map object
 * @param[in] user_agent The user agent to be used by the map
 *
 * @see elm_map_user_agent_get()
 */
EAPI void                  elm_map_user_agent_set(Evas_Object *obj, const char *user_agent);

/**
 * @brief Gets the user agent used by the map object.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @return The user agent identification string used by the map
 *
 * @see elm_map_user_agent_set()
 */
EAPI const char           *elm_map_user_agent_get(const Evas_Object *obj);


/**
 * @brief Adds a new overlay to the map object. This overlay has a default type.
 *
 * @since_tizen 2.3
 *
 * @remarks An overlay is created and shown at a specific point on the map, defined
 *          by @a lon and @a lat.
 *
 * @remarks The created overlay has a default style layout before the content or
 *          icon is set.
 *
 * @remarks If the content or icon is set, they are displayed instead of the default style
 *          layout.
 *          You can set this by using elm_map_overlay_content_set() or
 *          elm_map_overlay_icon_set(). If @c NULL is set, the default style
 *          is shown again.
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] obj The map object to add a new overlay
 * @param[in] lon The longitude of the overlay
 * @param[in] lat The latitude of the overlay
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_del()
 * @see elm_map_overlay_class_add()
 * @see elm_map_overlay_bubble_add()
 * @see elm_map_overlay_content_set()
 * @see elm_map_overlay_icon_set()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_add(Evas_Object *obj, double lon, double lat);

/**
 * @brief Returns all overlays in the map object.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks This list includes group overlays also.
 *          So this can be changed dynamically while zooming and panning.
 *
 * @param[in] obj The map object to return overlays
 * @return The list of all overlays, otherwise @c NULL on failure
 */
EAPI EAPI Eina_List *      elm_map_overlays_get(Evas_Object *obj);

/**
 * @brief Deletes an overlay from the map. This function can delete all types
 *        of overlays.
 *
 * @param[in] overlay The overlay to be deleted
 *
 * @see elm_map_overlay_add()
 * @see elm_map_overlay_class_add()
 * @see elm_map_overlay_bubble_add()
 */
EAPI void                  elm_map_overlay_del(Elm_Map_Overlay *overlay);

/**
 * @brief Gets the overlay type.
 *
 * @since_tizen 2.3
 *
 * @remarks This type is resolved when the overlay is created.
 *
 * @param[in] overlay The overlay type
 * @return The overlay type
 *
 * @see elm_map_overlay_add()
 * @see elm_map_overlay_class_add()
 * @see elm_map_overlay_bubble_add()
 */
EAPI Elm_Map_Overlay_Type  elm_map_overlay_type_get(const Elm_Map_Overlay *overlay);

 /**
 * @brief Sets a pointer to the user data of an overlay.
 *
 * @since_tizen 2.3
 *
 * @param[in] overlay The overlay to own the user data
 * @param[in] data A pointer to the user data
 *
 * @see elm_map_overlay_data_get()
 */
EAPI void                  elm_map_overlay_data_set(Elm_Map_Overlay *overlay, void *data);

/**
 * @brief Gets the user data stored on a overlay.
 *
 * @since_tizen 2.3
 *
 * @param[in] overlay The overlay to return the user data
 * @return A pointer to the data stored using elm_map_overlay_data_set(),
 *         otherwise @c NULL if no data has been set
 *
 * @see elm_map_overlay_data_set()
 */
EAPI void *                elm_map_overlay_data_get(const Elm_Map_Overlay *overlay);

/**
 * @brief Sets whether the overlay is hidden.
 *
 * @since_tizen 2.3
 *
 * @param[in] overlay The overlay to hide
 * @param[in] hide If @c EINA_TRUE the overlay is hidden,
 *             otherwise @c EINA_FALSE if it is shown
 * @see elm_map_overlay_hide_get()
 */
EAPI void                  elm_map_overlay_hide_set(Elm_Map_Overlay *overlay, Eina_Bool hide);

/**
 * @brief Gets a value that indicates whether the overlay is hidden.
 *
 * @details This gets the current hidden state for the overlay.
 *
 * @since_tizen 2.3
 *
 * @param[in] overlay The overlay with the hidden state
 * @return @c EINA_TRUE indicates that the overlay is hidden, otherwise @c EINA_FALSE indicates
 *         that it is not
 *
 * @see elm_map_overlay_hide_set()
 */
EAPI Eina_Bool             elm_map_overlay_hide_get(const Elm_Map_Overlay *overlay);

/**
 * @brief Sets the minimum zoom from where the overlay is displayed.
 *
 * @since_tizen 2.3
 *
 * @remarks The overlay is only displayed when the map is displayed at @a zoom
 *          or bigger.
 *
 * @param[in] overlay The overlay to set to minimum zoom
 * @param[in] zoom The minimum zoom
 *
 * @see elm_map_overlay_displayed_zoom_min_get()
 */
EAPI void                  elm_map_overlay_displayed_zoom_min_set(Elm_Map_Overlay *overlay, int zoom);

/**
 * @brief Gets the minimum zoom from where the overlay is displayed.
 *
 * @since_tizen 2.3
 *
 * @param[in] overlay The overlay with minimum zoom
 * @return zoom The minimum zoom
 *
 * @see elm_map_overlay_displayed_zoom_min_set()
 */
EAPI int                   elm_map_overlay_displayed_zoom_min_get(const Elm_Map_Overlay *overlay);

/**
 * @brief Pauses or unpauses the overlay.
 *
 * @details This sets the paused state to on (@c EINA_TRUE) or off (@c EINA_FALSE)
 *          for the overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks The default is off.
 *
 * @remarks This stops moving the overlay coordinates instantly,
 *          even if the map is being scrolled or zoomed.
 *
 * @param[in] overlay The overlay to be paused
 * @param[in] paused If @c EINA_TRUE the @a overlay is paused,
 *               otherwise @c EINA_FALSE if it is unpaused
 *
 * @see elm_map_overlay_paused_get()
 */
EAPI void                  elm_map_overlay_paused_set(Elm_Map_Overlay *overlay, Eina_Bool paused);

/**
 * @brief Gets a value that indicates whether the overlay is paused.
 *
 * @details This gets the current paused state for the overlay.
 *
 * @since_tizen 2.3
 *
 * @param[in] overlay The overlay in the paused state
 * @return @c EINA_TRUE indicates that the overlay is paused, otherwise @c EINA_FALSE indicates
 *         that it is not
 *
 * @see elm_map_overlay_paused_set()
 */
EAPI Eina_Bool             elm_map_overlay_paused_get(const Elm_Map_Overlay *overlay);

/**
 * @brief Gets a value that indicates whether the overlay is visible.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks The visibility of the overlay cannot be set.
 * @remarks This value can be changed dynamically while zooming and panning.
 *
 * @param[in] overlay The overlay in the visible state
 * @return @c EINA_TRUE indicates that the overlay is visible, otherwise @c EINA_FALSE indicates
 *         that it is not
 */
EAPI Eina_Bool             elm_map_overlay_visible_get(const Elm_Map_Overlay *overlay);

/**
 * @brief Sets the content object of the overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks Only default and class type overlay support this function.
 *
 * @remarks The content should be resized or size hints should be set in advance for the overlay.
 *          <b> Do not modify this object</b> (move, show, hide, del, etc.),
 *          after setting.
 *          You can only resize this.
 *
 * @remarks This content is what is inside the overlay and is going to be displayed.
 *          If content is set, the icon and default style layout are no more used unless
 *          the content is deleted.
 *
 * @remarks If @a obj is @c NULL, the content inside the overlay is deleted.
 *
 * @param[in] overlay The overlay whose content is set
 * @param[in] obj The evas object is used to display the overlay
 *
 * @see elm_map_overlay_content_get()
 */
EAPI void                  elm_map_overlay_content_set(Elm_Map_Overlay *overlay, Evas_Object *obj);

/**
 * @brief Gets the content object.
 *
 * @since_tizen 2.3
 *
 * @remarks Only default and class type overlay support this function.
 *
 * @remarks The returned content is what is inside the overlay and is going to be displayed.
 *
 * @remarks <b> Do not modify this object</b> (move, show, hide, del, etc.).
 *          You can only resize this.
 *
 * @remarks The content can be set by elm_map_overlay_content_set().
 *
 * @param[in] overlay The overlay to return the content
 * @return The evas object if it exists, otherwise @c NULL
 *
 * @see elm_map_overlay_content_set()
 */
EAPI const Evas_Object *   elm_map_overlay_content_get(const Elm_Map_Overlay *overlay);

/**
 * @brief Sets an icon for the overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks Only default and class type overlay support this function.
 *
 * @remarks <b> Do not modify this object</b> (move, show, hide, resize, del, etc.),
 *          after it is set.
 *
 * @remarks If the @a icon is set, the default style layout is not used.
 *
 * @remarks If @a icon is @c NULL, the icon inside the overlay is deleted.
 *
 * @param[in] overlay The overlay to set the icon
 * @param[in] icon The icon used to display the overlay
 *
 * @see elm_map_overlay_icon_get()
 */
EAPI void                  elm_map_overlay_icon_set(Elm_Map_Overlay *overlay, Evas_Object *icon);

/**
 * @brief Gets the icon object.
 *
 * @since_tizen 2.3
 *
 * @remarks Only default and class type overlay support this function.
 *
 * @remarks The returned icon is what is inside the overlay and is going to be displayed.
 *
 * @remarks <b> Do not modify this icon </b> (move, show, hide, resize, del, etc.).
 *
 * @remarks The icon can be set by elm_map_overlay_icon_set().
 *
 * @param[in] overlay The overlay to return the icon
 * @return The icon object if it exists, otherwise @c NULL
 *
 * @see elm_map_overlay_icon_set()
 */
EAPI const Evas_Object *   elm_map_overlay_icon_get(const Elm_Map_Overlay *overlay);

/**
 * @brief Sets the geographic coordinates of the overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks Only default and bubble type overlay support this function.
 *
 * @remarks This sets the center coordinates of the overlay. It can be
 *          obtained by elm_map_overlay_region_get().
 *
 * @param[in] overlay The overlay for which the geographic coordinates are set
 * @param[in] lon The longitude to be set
 * @param[in] lat The latitude to be set
 *
 * @see elm_map_overlay_region_get()
 */
EAPI void                  elm_map_overlay_region_set(Elm_Map_Overlay *overlay, double lon, double lat);

/**
 * @brief Gets the geographic coordinates of the overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks Only default and bubble type overlay support this function.
 *
 * @remarks This returns the center coordinates of the overlay. It can be
 *          set by elm_map_overlay_region_set().
 *
 * @param[in] overlay The overlay to return geographic coordinates
 * @param[out] lon The pointer to store the longitude
 * @param[out] lat The pointer to store the latitude
 *
 * @see elm_map_overlay_region_set()
 */
EAPI void                  elm_map_overlay_region_get(const Elm_Map_Overlay *overlay, double *lon, double *lat);


/**
 * @brief Sets the object color of the overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks It uses an additive color model, so each color channel represents
 *          how much of each primary colors must be used. @c 0 represents
 *          the absence of this color, so if all of the three are set to @c 0,
 *          the color is black.
 *
 * @remarks These component values should be integers in the range from @c 0 to @c 255,
 *          (single 8-bit byte).
 *
 * @remarks This sets the color used for the overlay. By default, it is set to
 *          solid red (r = 255, g = 0, b = 0, a = 255).
 *
 * @remarks For an alpha channel, @c 0 represents complete transparency, and @c 255 represents opacity.
 *
 * @remarks This function supports only the @c ELM_MAP_OVERLAY_TYPE_CLASS, @c ELM_MAP_OVERLAY_TYPE_DEFAULT,
 *          and @c ELM_MAP_OVERLAY_TYPE_ROUTE Elm_Map_Overlay_Type types.
 *
 * @param[in] overlay The overlay for which to set color
 * @param[in] r The red channel value, from @c 0 to @c 255
 * @param[in] g The green channel value, from @c 0 to @c 255
 * @param[in] b The blue channel value, from @c 0 to @c 255
 * @param[in] a The alpha channel value, from @c 0 to @c 255
 *
 * @see elm_map_overlay_color_get()
 */
EAPI void                  elm_map_overlay_color_set(Elm_Map_Overlay *overlay, int r, int g, int b, int a);

/**
 * @brief Gets the object color of the overlay.
 *
 * @since_tizen 2.3
 *
 * @param[in] overlay The overlay to return color
 * @param[out] r The pointer that stores the red channel value
 * @param[out] g The pointer that stores the green channel value
 * @param[out] b The pointer that stores the blue channel value
 * @param[out] a The pointer that stores the alpha channel value
 *
 * @see elm_map_overlay_color_set()
 */
EAPI void                  elm_map_overlay_color_get(const Elm_Map_Overlay *overlay, int *r, int *g, int *b, int *a);

/**
 * @brief Shows the given overlay at the center of the map, immediately.
 *
 * @since_tizen 2.3
 *
 * @remarks This causes the map to @b redraw its viewport's contents in the
 *          region containing the given @a overlay coordinates, that are
 *          moved to the center of the map.
 *
 * @param[in] overlay The overlay to show at the center
 *
 * @see elm_map_overlays_show() if more than one overlay needs to be displayed.
 */
EAPI void                  elm_map_overlay_show(Elm_Map_Overlay *overlay);

/**
 * @brief Moves and zooms the map to display a list of overlays.
 *
 * @since_tizen 2.3
 *
 * @remarks The map is centered on the center point of the overlay in the list.
 *          Then the map is zoomed in order to fit the overlay using the maximum
 *          zoom which allows display of all the overlays.
 *
 * @remarks All the overlays should belong to the same map object.
 *
 * @param[in] overlays A list of #Elm_Map_Overlay handles
 *
 * @see elm_map_overlay_show() to show a single overlay.
 */
EAPI void                  elm_map_overlays_show(Eina_List *overlays);

/**
 * @brief Sets the get callback function of the overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks If the overlay is clicked, the callback is called.
 *          The clicked overlay is returned by callback.
 *
 * @remarks You can add a callback to the class overlay. If one of the group overlays in this class
 *          is clicked, callback is called and a virtual group overlay is returned.
 *
 * @remarks You can delete this callback function by setting @c NULL.
 *
 * @param[in] overlay The overlay to own the get callback function
 * @param[in] get_cb The callback function
 * @param[in] data The user callback data
 */
EAPI void                  elm_map_overlay_get_cb_set(Elm_Map_Overlay *overlay, Elm_Map_Overlay_Get_Cb get_cb, void *data);

/**
 * @brief Sets the get callback function to call when the overlay is deleted.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks If the overlay is deleted, the callback is called.
 *          The deleted overlay is returned by the callback.
 *
 * @remarks You can delete this callback function by setting @c NULL.
 *
 * @param[in] overlay The overlay to own the del callback function
 * @param[in] del_cb The callback function
 * @param[in] data The user callback data
 */
EAPI void                  elm_map_overlay_del_cb_set(Elm_Map_Overlay *overlay, Elm_Map_Overlay_Del_Cb del_cb, void *data);

/**
 * @brief Adds a new class overlay to the map object.
 *        This overlay has a class type.
 *
 * @since_tizen 2.3
 *
 * @remarks This overlay is not shown unless overlay members are appended.
 *          If overlay members in the same class are close, group overlays
 *          are created. If they are far away, group overlays are hidden.
 *          When group overlays are shown, they have default style layouts at first.
 *
 * @remarks You can change the state (hidden, paused, etc.) or set the content
 *          or icon of the group overlays by chaning the state of the class overlay.
 *          Do not modify the group overlay.
 *
 * @remarks Also these changes have an influence on the overlays in the same class
 *          even if each overlay is alone and is not grouped.
 *
 * @param[in] obj The map object to add a new overlay
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_del()
 * @see elm_map_overlay_add()
 * @see elm_map_overlay_bubble_add()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_class_add(Evas_Object *obj);

/**
 * @brief Adds a new overlay member to the class overlay.
 *
 * @since_tizen 2.3
 *
 * @param[in] clas The class overlay to which to add a new overlay
 * @param[in] overlay The overlay to be added to the class overlay
 *
 * @see elm_map_overlay_class_remove()
 */
EAPI void                  elm_map_overlay_class_append(Elm_Map_Overlay *clas, Elm_Map_Overlay *overlay);

/**
 * @brief Removes an overlay from the class.
 *
 * @since_tizen 2.3
 *
 * @param[in] clas The class overlay from which to delete the overlay
 * @param[in] overlay The overlay to be deleted from the class overlay
 *
 * @see elm_map_overlay_class_append()
 */
EAPI void                  elm_map_overlay_class_remove(Elm_Map_Overlay *clas, Elm_Map_Overlay *overlay);

/**
 * @brief Sets the maximum zoom from where the overlay members in the class can be
 *        grouped.
 *
 * @since_tizen 2.3
 *
 * @remarks Overlay members in the class are only grouped when the map
 *          is displayed at less than @a zoom.
 *
 * @param[in] clas The overlay class having overlay members
 * @param[in] zoom The maximum zoom
 *
 * @see elm_map_overlay_class_zoom_max_get()
 */
EAPI void                  elm_map_overlay_class_zoom_max_set(Elm_Map_Overlay *clas, int zoom);

/**
 * @brief Gets the maximum zoom from where the overlay members in the class can be
 *        grouped.
 *
 * @since_tizen 2.3
 *
 * @param[in] clas The overlay class having overlay members
 *
 * @return The maximum zoom
 *
 * @see elm_map_overlay_class_zoom_max_set()
 */
EAPI int                   elm_map_overlay_class_zoom_max_get(const Elm_Map_Overlay *clas);

/**
 * @brief Gets the overlay members of the group overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks The group overlays are virtual overlays. They are shown and hidden dynamically.
 *          You can add a callback to the class overlay. If one of the group overlays in this class
 *          is clicked, callback is called and a virtual group overlays is returned.
 *
 * @remarks You can change the state (hidden, paused, etc.) or set the content
 *          or icon of the group overlays by changing the state of the class overlay.
 *          Do not modify the group overlay.
 *
 * @param[in] grp The group overlay having overlay members
 *
 * @return The list of group overlay members
 *
 * @see elm_map_overlay_class_add()
 */
EAPI Eina_List *      elm_map_overlay_group_members_get(const Elm_Map_Overlay *grp);

/**
 * @brief Adds a new bubble overlay to the map object.
 *        This overlay has a bubble type.
 *
 * @since_tizen 2.3
 *
 * @remarks A bubble is not displayed unless the geographic coordinates are set or
 *          any other overlays are followed.
 *
 * @remarks This overlay has a bubble style layout and icon or the content cannot
 *          be set.
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] obj The map object to add a new overlay
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_del()
 * @see elm_map_overlay_add()
 * @see elm_map_overlay_class_add()
 * @see elm_map_overlay_region_set()
 * @see elm_map_overlay_bubble_follow()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_bubble_add(Evas_Object *obj);

/**
 * @brief Follows another overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks The bubble overlay follows the parent overlay's movement (hide, show, move).
 *
 * @param[in] bubble The bubble overlay to follow a parent overlay
 * @param[in] parent The parent overlay to be followed by the bubble overlay
 *
 * @see elm_map_overlay_bubble_add()
 */
EAPI void                  elm_map_overlay_bubble_follow(Elm_Map_Overlay *bubble, const Elm_Map_Overlay *parent);

/**
 * @brief Adds a content object to the bubble overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks The added content is displayed inside the bubble overlay.
 *
 * @param[in] bubble The bubble overlay to add content
 * @param[in] content The content to be added to the bubble overlay
 *
 * @see elm_map_overlay_bubble_content_clear()
 */
EAPI void                  elm_map_overlay_bubble_content_append(Elm_Map_Overlay *bubble, Evas_Object *content);

/**
 * @brief Clears all the content inside the bubble overlay.
 *
 * @details This deletes all the content inside the bubble overlay.
 *
 * @since_tizen 2.3
 *
 * @param[in] bubble The bubble overlay whose content is cleared
 *
 * @see elm_map_overlay_bubble_content_append()
 */
EAPI void                  elm_map_overlay_bubble_content_clear(Elm_Map_Overlay *bubble);

/**
 * @brief Adds a new route overlay to the map object.
 *        This overlay has a route type.
 *
 * @since_tizen 2.3
 *
 * @remarks This overlay has a route style layout and icon or content cannot
 *          be set.
 *
 * @remarks The color scheme can be changed by elm_map_overlay_content_set().
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] obj The map object to add a new overlay
 * @param[in] route The route object to make an overlay
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_del()
 * @see elm_map_overlay_class_add()
 * @see elm_map_overlay_content_set()
 * @see elm_map_overlay_content_get()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_route_add(Evas_Object *obj, const Elm_Map_Route *route);

/**
 * @brief Adds a new line overlay to the map object.
 *        This overlay has a line type.
 *
 * @since_tizen 2.3
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] obj The map object to add a new overlay
 * @param[in] flon The start longitude
 * @param[in] flat The start latitude
 * @param[in] tlon The destination longitude
 * @param[in] tlat The destination latitude
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_del()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_line_add(Evas_Object *obj, double flon, double flat, double tlon, double tlat);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Add a new polyline overlay to the map object.
 *        This overlay has a polyline type.
 *
 * @remarks At least 2 regions should be added to show the polyline overlay.
 *
 * @remarks Overlay created with this method can be deleted with elm_map_overlay_del().
 *
 * @param obj The map object to add a new overlay.
 * @return The created overlay or @c NULL upon failure.
 *
 * @see elm_map_overlay_del()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_polyline_add(Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Add a geographic coordinates to the polyline overlay.
 *
 * @remarks At least 2 regions should be added to show the polyline overlay.
 *
 * @remarks Overlay created with this method can be deleted with elm_map_overlay_del().
 *
 * @param overlay The polyline overlay to add geographic coordinates
 * @param lon The longitude.
 * @param lat The latitude.
 *
 * @see elm_map_overlay_polyline_add()
 * @see elm_map_overlay_del()
 */
EAPI void                             elm_map_overlay_polyline_region_add(Elm_Map_Overlay *overlay, double lon, double lat);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sets the width of the polyline overlay.
 *
 * @param overlay The polyline overlay to set a width.
 * @param width The line width in pixels
 *
 * @see elm_map_overlay_polyline_width_get()
 */
EAPI void                             elm_map_overlay_polyline_width_set(Elm_Map_Overlay *overlay, int width);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the width of the polyline overlay.
 *
 * @param overlay The polyline overlay to get a width.
 * @return The line width in pixels
 *
 * @see elm_map_overlay_polyline_width_set()
 */
EAPI int                 elm_map_overlay_polyline_width_get(Elm_Map_Overlay *overlay);

/**
 * @brief Adds a new polygon overlay to the map object.
 *        This overlay has a polygon type.
 *
 * @since_tizen 2.3
 *
 * @remarks At least 3 regions should be added to show the polygon overlay.
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] obj The map object to add a new overlay
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_polygon_region_add()
 * @see elm_map_overlay_del()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_polygon_add(Evas_Object *obj);

/**
 * @brief Adds geographic coordinates to the polygon overlay.
 *
 * @since_tizen 2.3
 *
 * @remarks At least 3 regions should be added to show the polygon overlay.
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] overlay The polygon overlay to add geographic coordinates
 * @param[in] lon The longitude
 * @param[in] lat The latitude
 *
 * @see elm_map_overlay_polygon_add()
 * @see elm_map_overlay_del()
 */
EAPI void                             elm_map_overlay_polygon_region_add(Elm_Map_Overlay *overlay, double lon, double lat);

/**
 * @brief Adds a new circle overlay to the map object.
 *        This overlay has a circle type.
 *
 * @since_tizen 2.3
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] obj The map object to add a new circle overlay
 * @param[in] lon The center longitude
 * @param[in] lat The center latitude
 * @param[in] radius The pixel length of the radius
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_del()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_circle_add(Evas_Object *obj, double lon, double lat, double radius);

/**
 * @brief Adds a new scale overlay to the map object.
 *        This overlay has a scale type.
 *
 * @since_tizen 2.3
 *
 * @remarks The scale overlay shows the ratio of a distance on the map to the corresponding distance.
 *
 * @remarks Overlay created with this method can be deleted by elm_map_overlay_del().
 *
 * @param[in] obj The map object to add a new scale overlay
 * @param[in] x  The horizontal pixel coordinate
 * @param[in] y  The vertical pixel coordinate
 * @return The created overlay, otherwise @c NULL on failure
 *
 * @see elm_map_overlay_del()
 */
EAPI Elm_Map_Overlay *     elm_map_overlay_scale_add(Evas_Object *obj, Evas_Coord x, Evas_Coord y);

/**
 * @brief Gets information on the tile load status.
 *
 * @since_tizen 2.3
 *
 * @remarks This gets the current tile loaded status for the map object.
 *
 * @param[in] obj The map object
 * @param[out] try_num The pointer that stores the number of tile download requests
 * @param[out] finish_num The pointer that stores the number of tiles successfully downloaded
 */
EAPI void                  elm_map_tile_load_status_get(const Evas_Object *obj, int *try_num, int *finish_num);

/**
 * @brief Gets the names of available sources for a specific type.
 *
 * @since_tizen 2.3
 *
 * @remarks It provides a list with all available sources.
 *          Current source can be set by elm_map_source_set(), or obtained by
 *          elm_map_source_get().
 *
 * @remarks Available sources of tile type:
 * @li "Mapnik"
 * @li "Osmarender"
 * @li "CycleMap"
 * @li "Maplint"
 *
 * @remarks Available sources of route type:
 * @li "Yours"
 *
 * @remarks Available sources of name type:
 * @li "Nominatim"
 *
 * @param[in] obj The map object
 * @param[in] type The source type
 * @return The char pointer array of source names
 *
 * @see elm_map_source_set()
 * @see elm_map_source_get()
 */
EAPI const char          **elm_map_sources_get(const Evas_Object *obj, Elm_Map_Source_Type type);

/**
 * @brief Sets the current source of the map for a specific type.
 *
 * @since_tizen 2.3
 *
 * @remarks Map widget retrieves tile images that compose of the map from a web service.
 *          This web service can be set by this method
 *          for the @c ELM_MAP_SOURCE_TYPE_TILE type.
 *          A different service can return a different map with different
 *          information and it can use different zoom values.
 *
 * @remarks Map widget provides route data based on an external web service.
 *          This web service can be set with this method
 *          for the @c ELM_MAP_SOURCE_TYPE_ROUTE type.
 *
 * @remarks Map widget also provides geoname data based on a external web service.
 *          This web service can be set with this method
 *          for the @c ELM_MAP_SOURCE_TYPE_NAME type.
 *
 * @remarks The @a source_name needs to match one of the names provided by
 *          elm_map_sources_get().
 *
 * @remarks The current source can be obtained by elm_map_source_get().
 *
 * @param[in] obj The map object
 * @param[in] type The source type
 * @param[in] source_name The source to be used
 *
 * @see elm_map_sources_get()
 * @see elm_map_source_get()
 */
EAPI void                  elm_map_source_set(Evas_Object *obj, Elm_Map_Source_Type type, const char *source_name);

/**
 * @brief Gets the name of the currently used source for a specific type.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] type The source type
 * @return The name of the source in use
 *
 * @see elm_map_sources_get()
 * @see elm_map_source_set()
 */
EAPI const char           *elm_map_source_get(const Evas_Object *obj, Elm_Map_Source_Type type);

/**
 * @internal
 * @remarks Tizen only
 *
 * @brief Gets the names of available engines.
 *
 * @remarks It provides a list with all the available map engines.
 *          Current engine can be set by elm_map_engine_set(), or obtained by
 *          elm_map_engine_get().
 *
 * @remarks By default, the tile engine is implemented.
 *
 * @param obj The map object
 * @return The char pointer array of engine names
 *
 * @see elm_map_engine_set()
 * @see elm_map_engine_get()
 */
EAPI const char          **elm_map_engines_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sets the current engine of the map.
 *
 * @remarks The @a engine_name needs to match one of the names provided by
 *          elm_map_engines_get().
 *
 * @remarks The current engine can be obtained by elm_map_engine_get().
 *
 * @remarks By default, the tile engine is used.
 *
 * @param obj The map object
 * @param engine_name The engine to be used
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_map_engines_get()
 * @see elm_map_engine_get()
 */
EAPI Eina_Bool             elm_map_engine_set(Evas_Object *obj, const char *engine_name);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the name of the currently used engine.
 *
 * @param obj The map object
 * @return The name of the engine in use
 *
 * @see elm_map_engines_get()
 * @see elm_map_engine_set()
 */

EAPI const char           *elm_map_engine_get(const Evas_Object *obj);


/**
 * @brief Adds a new route to the map object.
 *
 * @since_tizen 2.3
 *
 * @remarks A route is traced by a point at the coordinates (@a flat, @a flon)
 *          to a point at the coordinates (@a tlat, @a tlon), using the route service
 *          set by elm_map_source_set().
 *
 * @remarks It takes @a type to be considered to define the route,
 *          depending on whether the user is walking or driving, the route may vary.
 *          One from #ELM_MAP_ROUTE_TYPE_MOTOCAR, #ELM_MAP_ROUTE_TYPE_BICYCLE,
 *          or #ELM_MAP_ROUTE_TYPE_FOOT need to be used.
 *
 * @remarks Another parameter is what the route should prioritize, minimum distance
 *          or less time spent on the route. So @a method should be one
 *          from #ELM_MAP_ROUTE_METHOD_SHORTEST or #ELM_MAP_ROUTE_METHOD_FASTEST.
 *
 * @remarks Routes created by this method can be deleted by
 *          elm_map_route_del(),
 *          and distance can be obtained by elm_map_route_distance_get().
 *
 * @param[in] obj The map object
 * @param[in] type The type of transport to be considered when tracing a route
 * @param[in] method The routing method that should be prioritized
 * @param[in] flon The start longitude
 * @param[in] flat The start latitude
 * @param[in] tlon The destination longitude
 * @param[in] tlat The destination latitude
 * @param[in] route_cb The route to be traced
 * @param[in] data A pointer to the user data
 *
 * @return The created route, otherwise @c NULL on failure
 *
 * @see elm_map_route_del()
 * @see elm_map_route_distance_get()
 * @see elm_map_source_set()
 */
EAPI Elm_Map_Route        *elm_map_route_add(Evas_Object *obj, Elm_Map_Route_Type type, Elm_Map_Route_Method method, double flon, double flat, double tlon, double tlat, Elm_Map_Route_Cb route_cb, void *data);

/**
 * @brief Removes a route from the map.
 *
 * @since_tizen 2.3
 *
 * @param[in] route The route to remove
 *
 * @see elm_map_route_add()
 */
EAPI void                  elm_map_route_del(Elm_Map_Route *route);

/**
 * @brief Gets the route distance in kilometers.
 *
 * @since_tizen 2.3
 *
 * @param[in] route The route object
 * @return The distance of the route (unit : km)
 */
EAPI double                elm_map_route_distance_get(const Elm_Map_Route *route);

/**
 * @brief Gets the information of the route nodes.
 *
 * @since_tizen 2.3
 *
 * @param[in] route The route object
 * @return A string with the nodes of the route
 */
EAPI const char           *elm_map_route_node_get(const Elm_Map_Route *route);

/**
 * @brief Gets the information of the route waypoint.
 *
 * @since_tizen 2.3
 *
 * @param[in] route The route object
 * @return A string with information about the waypoint of the route
 */
EAPI const char           *elm_map_route_waypoint_get(const Elm_Map_Route *route);

/**
 * @brief Requests an address or geographic coordinates(longitude, latitude)
 *        from a given address or geographic coordinates(longitude, latitude).
 *
 * @since_tizen 2.3
 *
 * @remarks If you want to get an address from the geographic coordinates, set the input @a address
 *          as @c NULL and set @a lon and @a lat as you want to convert.
 *          If the address is set to a value other than @c NULL, @a lon and @a lat are checked.
 *
 * @remarks To get the string for this address, elm_map_name_address_get()
 *          should be used after callback or the @c "name,loaded" signal is called.
 *
 * @remarks To get the longitude and latitude, elm_map_name_region_get()
 *          should be used.
 *
 * @param[in] obj The map object
 * @param[in] address The address
 * @param[in] lon The longitude
 * @param[in] lat The latitude
 * @param[in] name_cb The callback function
 * @param[in] data The user callback data
 * @return name A #Elm_Map_Name handle for this coordinate
 */
EAPI Elm_Map_Name         *elm_map_name_add(const Evas_Object *obj, const char *address, double lon, double lat, Elm_Map_Name_Cb name_cb, void *data);

/**
 * @brief Requests a list of addresses corresponding to a given name.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks This is used if you want to search the address from a name.
 *
 * @param[in] obj The map object
 * @param[in] address The address
 * @param[in] name_cb The callback function
 * @param[in] data The user callback data
 */
EAPI void                  elm_map_name_search(const Evas_Object *obj, const char *address, Elm_Map_Name_List_Cb name_cb, void *data);

/**
 * @brief Gets the address of the name.
 *
 * @details This gets the coordinates of the @a name created with one of the
 *          conversion functions.
 *
 * @since_tizen 2.3
 *
 * @param[in] name The name handle
 * @return The address string of @a name
 *
 * @see elm_map_name_add()
 */
EAPI const char           *elm_map_name_address_get(const Elm_Map_Name *name);

/**
 * @brief Gets the current coordinates of the name.
 *
 * @details This gets the coordinates of the @a name created with one of the
 *          conversion functions.
 *
 * @since_tizen 2.3
 *
 * @param[in] name The name handle
 * @param[out] lat The pointer to store the latitude
 * @param[out] lon The pointer to store The longitude
 *
 * @see elm_map_name_add()
 */
EAPI void                  elm_map_name_region_get(const Elm_Map_Name *name, double *lon, double *lat);

/**
 * @brief Removes a name from the map.
 *
 * @since_tizen 2.3
 *
 * @remarks Basically the struct handled by @a name is freed, so conversions
 *          between the address and coordinates is lost.
 *
 * @param[in] name The name to remove
 *
 * @see elm_map_name_add()
 */
EAPI void                  elm_map_name_del(Elm_Map_Name *name);

/**
 * @brief Adds a track on the map.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] emap The emap route object
 * @return The route object \n
 *         This is an elm object of type Route.
 *
 * @see elm_route_add()
 */
EAPI Evas_Object          *elm_map_track_add(Evas_Object *obj, void *emap);

/**
 * @brief Removes a track from the map.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The map object
 * @param[in] route The track to remove
 */
EAPI void                  elm_map_track_remove(Evas_Object *obj, Evas_Object *route);

/**
 * @internal
 * @remarks Tizen only feature
 */
EAPI void                  elm_map_engine_key_set(Evas_Object *obj, const char *engine_name, const char *key);

/**
 * @internal
 * @remarks Tizen only feature
 *
* @brief Get an offscreen rendered image of the map. The coordinates and zoom level must be set prior to
*        calling this function.
*
* @param obj The map object
* @param w   The width of the output image
* @param h   The height of the output image
* @param map_image_cb The callback function that is called when the map image is ready
* @param data The user callback data
* @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
*/
EAPI Eina_Bool              elm_map_image_get(Evas_Object *obj, int w, int h, Elm_Map_Capture_Result_Cb map_image_cb, void *data);

/**
 * @}
 */
