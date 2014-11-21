EINA_DEPRECATED EAPI Evas_Object *elm_scrolled_entry_add(Evas_Object *parent);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_single_line_set(Evas_Object *obj, Eina_Bool single_line);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_single_line_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_password_set(Evas_Object *obj, Eina_Bool password);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_password_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_entry_set(Evas_Object *obj, const char *entry);
EINA_DEPRECATED EAPI const char  *elm_scrolled_entry_entry_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_entry_append(Evas_Object *obj, const char *entry);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_is_empty(const Evas_Object *obj);
EINA_DEPRECATED EAPI const char  *elm_scrolled_entry_selection_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_entry_insert(Evas_Object *obj, const char *entry);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_line_wrap_set(Evas_Object *obj, Elm_Wrap_Type wrap);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_editable_set(Evas_Object *obj, Eina_Bool editable);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_editable_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_select_none(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_select_all(Evas_Object *obj);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_cursor_next(Evas_Object *obj);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_cursor_prev(Evas_Object *obj);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_cursor_up(Evas_Object *obj);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_cursor_down(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cursor_begin_set(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cursor_end_set(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cursor_line_begin_set(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cursor_line_end_set(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cursor_selection_begin(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cursor_selection_end(Evas_Object *obj);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_cursor_is_format_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_cursor_is_visible_format_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI const char  *elm_scrolled_entry_cursor_content_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cursor_pos_set(Evas_Object *obj, int pos);
EINA_DEPRECATED EAPI int          elm_scrolled_entry_cursor_pos_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_selection_cut(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_selection_copy(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_selection_paste(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_context_menu_clear(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_context_menu_item_add(Evas_Object *obj, const char *label, const char *icon_file, Elm_Icon_Type icon_type, Evas_Smart_Cb func, const void *data);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_context_menu_disabled_set(Evas_Object *obj, Eina_Bool disabled);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_context_menu_disabled_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_scrollbar_policy_set(Evas_Object *obj, Elm_Scroller_Policy h, Elm_Scroller_Policy v);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_bounce_get(const Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_icon_set(Evas_Object *obj, Evas_Object *icon);
EINA_DEPRECATED EAPI Evas_Object *elm_scrolled_entry_icon_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI Evas_Object *elm_scrolled_entry_icon_unset(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_icon_visible_set(Evas_Object *obj, Eina_Bool setting);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_end_set(Evas_Object *obj, Evas_Object *end);
EINA_DEPRECATED EAPI Evas_Object *elm_scrolled_entry_end_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI Evas_Object *elm_scrolled_entry_end_unset(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_end_visible_set(Evas_Object *obj, Eina_Bool setting);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_item_provider_append(Evas_Object *obj, Evas_Object * (*func)(void *data, Evas_Object * entry, const char *item), void *data);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_item_provider_prepend(Evas_Object *obj, Evas_Object * (*func)(void *data, Evas_Object * entry, const char *item), void *data);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_item_provider_remove(Evas_Object *obj, Evas_Object * (*func)(void *data, Evas_Object * entry, const char *item), void *data);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_text_filter_append(Evas_Object *obj, void (*func)(void *data, Evas_Object *entry, char **text), void *data);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_text_filter_prepend(Evas_Object *obj, void (*func)(void *data, Evas_Object *entry, char **text), void *data);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_text_filter_remove(Evas_Object *obj, void (*func)(void *data, Evas_Object *entry, char **text), void *data);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_file_set(Evas_Object *obj, const char *file, Elm_Text_Format format);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_file_get(const Evas_Object *obj, const char **file, Elm_Text_Format *format);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_file_save(Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_autosave_set(Evas_Object *obj, Eina_Bool autosave);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_autosave_get(const Evas_Object *obj);
EINA_DEPRECATED EAPI void         elm_scrolled_entry_cnp_textonly_set(Evas_Object *obj, Eina_Bool textonly);
EINA_DEPRECATED EAPI Eina_Bool    elm_scrolled_entry_cnp_textonly_get(Evas_Object *obj);

/**
 * @brief Converts a pixel coordinate (x,y) into a geographic coordinate
 *        (longitude, latitude).
 *
 * @remarks The origin pixel point is the top left corner of the viewport.
 *          Map zoom and size are taken into account.
 *
 * @param obj The map object
 * @param x The coordinate
 * @param y The coordinate
 * @param size The size in pixels of the map \n
 *             The map is a square and generally its size is : pow(2.0, zoom)*256.
 * @param lon The pointer to store the longitude that corresponds to @a x
 * @param lat The pointer to store the latitude that corresponds to @a y
 *
 * @deprecated Use elm_map_canvas_to_geo_convert() instead
 *
 * @see elm_map_utils_convert_geo_into_coord() if you need the inverse.
 */
EINA_DEPRECATED EAPI void                  elm_map_utils_convert_coord_into_geo(const Evas_Object *obj, int x, int y, int size, double *lon, double *lat);

/**
 * @brief Converts a geographic coordinate (longitude, latitude) into a pixel
 *        coordinate (x, y).
 *
 * @remarks The origin pixel point is the top left corner of the viewport.
 *          Map zoom and size are taken into account.
 *
 * @param obj The map object
 * @param lon The longitude
 * @param lat The latitude
 * @param size The size in pixels of the map \n
 *             The map is a square
 *             and generally its size is : pow(2.0, zoom)*256.
 * @param x The pointer to store the horizontal pixel coordinate that
 *          corresponds to the longitude
 * @param y The pointer to store the vertical pixel coordinate that
 *          corresponds to the latitude.
 *
 * @deprecated Use Use elm_map_canvas_to_geo_convert() instead
 *
 * @see elm_map_utils_convert_coord_into_geo() if you need the inverse.
 */
EINA_DEPRECATED EAPI void                  elm_map_utils_convert_geo_into_coord(const Evas_Object *obj, double lon, double lat, int size, int *x, int *y);

/**
 * @brief Gets information on the downloading status.
 *
 * @details This gets the current downloading status for the map object, the number
 *          of tiles being downloaded, and the number of tiles already downloaded.
 *
 * @param obj The map object
 * @param try_num The pointer to store the number of tiles being downloaded
 * @param finish_num The pointer to store the number of tiles that are successfully downloaded
 *
 * @deprecated Use Use elm_map_tile_load_status_get() instead.
 */
EINA_DEPRECATED EAPI void                  elm_map_utils_downloading_status_get(const Evas_Object *obj, int *try_num, int *finish_num);

/**
 * @brief Converts a geographic coordinate (longitude, latitude) into a name
 *        (address).
 *
 * @remarks To get the string for this address, elm_map_name_address_get()
 *          should be used.
 *
 * @param obj The map object
 * @param lon The longitude
 * @param lat The latitude
 * @return name A #Elm_Map_Name handle for this coordinate
 *
 * @deprecated Use elm_map_name_add() instead.
 *
 * @see elm_map_utils_convert_name_into_coord() if you need to do an inverse.
 *
 */
EINA_DEPRECATED EAPI Elm_Map_Name         *elm_map_utils_convert_coord_into_name(const Evas_Object *obj, double lon, double lat);

/**
 * @brief Converts a name (address) into a geographic coordinate
 *        (longitude, latitude).
 *
 * @remarks To get the longitude and latitude, elm_map_name_region_get()
 *          should be used.
 *
 * @param obj The map object
 * @param address The address
 * @return name A #Elm_Map_Name handle for this address
 *
 * @deprecated Use elm_map_name_geo_request() instead.
 *
 * @see elm_map_utils_convert_coord_into_name() if you need to do an inverse.
 *
 */
EINA_DEPRECATED EAPI Elm_Map_Name         *elm_map_utils_convert_name_into_coord(const Evas_Object *obj, char *address);

/**
 * @brief Adds a new marker to the map object.
 *
 * @remarks A marker is created and shown at a specific point of the map, defined
 *          by @a lon and @a lat.
 *
 * @remarks It is displayed using a style defined by @a clas when this marker
 *          is displayed alone (not grouped). A new class can be created with
 *          elm_map_marker_class_new().
 *
 * @remarks If the marker is grouped to other markers, it is displayed with a
 *          style defined by the @a clas_group. Markers with the same group are grouped
 *          if they are close. A new group class can be created with
 *          elm_map_marker_group_class_new().
 *
 * @remarks Markers created with this method can be deleted with
 *          elm_map_marker_remove().
 *
 * @remarks A marker can have associated content displayed as a bubble,
 *          when a user clicks on it, or as an icon. These objects
 *          are fetched using class' callback functions.
 *
 * @param obj The map object
 * @param lon The longitude of the marker
 * @param lat The latitude of the marker
 * @param clas The class to use when the marker @b isn't grouped to others
 * @param clas_group The class group to use when the marker is grouped to others
 * @param data The data passed to the callbacks
 *
 * @return The created marker, otherwise @c NULL on failure
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_class_new()
 * @see elm_map_marker_group_class_new()
 * @see elm_map_marker_remove()
 */
EINA_DEPRECATED EAPI Elm_Map_Marker       *elm_map_marker_add(Evas_Object *obj, double lon, double lat, Elm_Map_Marker_Class *clas, Elm_Map_Group_Class *clas_group, void *data);

/**
 * @brief Removes a marker from the map.
 *
 * @param marker The marker to remove
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_add()
 */
EINA_DEPRECATED EAPI void                  elm_map_marker_remove(Elm_Map_Marker *marker);

/**
 * @brief Gets the current coordinates of the marker.
 *
 * @remarks These values are set when adding markers using the function
 *          elm_map_marker_add().
 *
 * @param marker The marker
 * @param lat The pointer to store the marker's latitude
 * @param lon The pointer to store the marker's longitude
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_add()
 */
EINA_DEPRECATED EAPI void                  elm_map_marker_region_get(const Elm_Map_Marker *marker, double *lon, double *lat);

/**
 * @brief Animatedly brings the given marker to the center of the map.
 *
 * @remarks This causes the map to jump to the given @a marker coordinates
 *          and display it (by scrolling) in the center of the viewport, if it is not
 *          already centered. This uses animation to do so and takes a period
 *           of time to complete.
 *
 * @param marker The marker to bring to the center
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_show() for a function to avoid animation.
 * @see elm_map_marker_region_get()
 */
EINA_DEPRECATED EAPI void                  elm_map_marker_bring_in(Elm_Map_Marker *marker);

/**
 * @brief Shows the given marker at the center of the map, @b immediately.
 *
 * @remarks This causes the map to @b redraw its viewport's contents to the
 *          region containing the given @a marker coordinates, that are
 *          moved to the center of the map.
 *
 * @param marker The marker to show at the center
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_bring_in() for a function to move with animation.
 * @see elm_map_markers_list_show() if more than one marker needs to be
 *      displayed.
 * @see elm_map_marker_region_get()
 */
EINA_DEPRECATED EAPI void                  elm_map_marker_show(Elm_Map_Marker *marker);

/**
 * @brief Moves and zooms the map to display a list of markers.
 *
 * @remarks The map is centered at the center point of the markers in the list.
 *          Then the map is zoomed in order to fit the markers using the maximum
 *          zoom which allows display of all the markers.
 *
 * @remarks All the markers should belong to the same map object.
 *
 * @param markers A list of #Elm_Map_Marker handles
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_show() to show a single marker.
 * @see elm_map_marker_bring_in()
 */
EINA_DEPRECATED EAPI void                  elm_map_markers_list_show(Eina_List *markers);

/**
 * @brief Gets the Evas object returned by the Elm_Map_Marker_Get_Func callback.
 *
 * @remarks To set callback function #Elm_Map_Marker_Get_Func for the marker class,
 *          elm_map_marker_class_get_cb_set() should be used.
 *
 * @remarks This content is what is inside the bubble that is displayed
 *          when a user clicks over the marker.
 *
 * @remarks This returns the actual Evas object to be placed inside
 *          the bubble. This may be @c NULL, as it may
 *          not have been created or may have been deleted, at any time, by
 *          the map. <b>Do not modify this object</b> (move, resize,
 *          show, hide, etc.), as the map is controlling it. This
 *          function is for querying, emitting custom signals or hooking
 *          lower level callbacks for events on that object. Do not delete
 *          this object under any circumstances.
 *
 * @param marker The marker with content to be returned
 * @return The evas object if it exists, otherwise @c NULL
 *
 * @deprecated Use Elm_Map_Overlay instead
 */
EINA_DEPRECATED EAPI Evas_Object          *elm_map_marker_object_get(const Elm_Map_Marker *marker);

/**
 * @brief Updates the marker.
 *
 * @remarks If content is set for this marker, it calls the function #Elm_Map_Marker_Del_Func to delete it,
 *          and then it fetches the content again with #Elm_Map_Marker_Get_Func.
 *
 * @remarks These functions are set for the marker class with
 *          elm_map_marker_class_get_cb_set() and elm_map_marker_class_del_cb_set().
 *
 * @param marker The marker to be updated
 *
 * @deprecated Use Elm_Map_Overlay instead
 */
EINA_DEPRECATED EAPI void                  elm_map_marker_update(Elm_Map_Marker *marker);

/**
 * @brief Creates a new group class.
 *
 * @remarks Each marker must be associated to a group class. Markers in the same
 *          group are grouped if they are close.
 *
 * @remarks The group class defines the style of the marker when a marker is grouped
 *          to others markers. When it is alone, another class is used.
 *
 * @remarks A group class needs to be provided when creating a marker with
 *          elm_map_marker_add().
 *
 * @remarks Some properties and functions that can be set by the class are:
 *          - style - With elm_map_group_class_style_set().
 *          - data - To be associated to the group class. It can be set using
 *                   elm_map_group_class_data_set().
 *          - min zoom - To display markers, set using
 *                       elm_map_group_class_zoom_displayed_set().
 *          - max zoom - To group markers, set using
 *                       elm_map_group_class_zoom_grouped_set().
 *          - visibility - To set if markers are visible or not, set using
 *                         elm_map_group_class_hide_set().
 *          - #Elm_Map_Group_Icon_Get_Func - Used to fetch the icon for the markers group classes.
 *                                           It can be set using elm_map_group_class_icon_cb_set().
 *
 * @param obj The map object
 * @return The new group class
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_add()
 * @see elm_map_group_class_style_set()
 * @see elm_map_group_class_data_set()
 * @see elm_map_group_class_zoom_displayed_set()
 * @see elm_map_group_class_zoom_grouped_set()
 * @see elm_map_group_class_hide_set()
 * @see elm_map_group_class_icon_cb_set()
 */
EINA_DEPRECATED EAPI Elm_Map_Group_Class  *elm_map_group_class_new(Evas_Object *obj);

/**
 * @brief Creates a new marker class.
 *
 * @remarks Each marker must be associated to a class.
 *
 * @remarks The marker class defines the style of the marker when a marker is
 *          displayed alone, i.e., not grouped to others markers. When grouped
 *          it uses the group class style.
 *
 * @remarks A marker class needs to be provided when creating a marker using
 *          elm_map_marker_add().
 *
 * @remarks Some properties and functions that can be set by the class are:
 *          - style - With elm_map_marker_class_style_set().
 *          - #Elm_Map_Marker_Icon_Get_Func - Used to fetch the icon for the markers classes.
 *                                            It can be set using elm_map_marker_class_icon_cb_set().
 *          - #Elm_Map_Marker_Get_Func - Used to fetch bubble content for marker classes.
 *                                       Set using elm_map_marker_class_get_cb_set().
 *          - #Elm_Map_Marker_Del_Func - Used to delete bubble content for marker classes.
 *                                       Set using elm_map_marker_class_del_cb_set().
 *
 * @param obj The map object
 * @return The new group class
 *
 * @deprecated Use Elm_Map_Overlay instead
 *
 * @see elm_map_marker_add()
 * @see elm_map_marker_class_style_set()
 * @see elm_map_marker_class_icon_cb_set()
 * @see elm_map_marker_class_get_cb_set()
 * @see elm_map_marker_class_del_cb_set()
 */
EINA_DEPRECATED EAPI Elm_Map_Marker_Class *elm_map_marker_class_new(Evas_Object *obj);

/**
 * @brief Removes a route from the map.
 *
 * @param route The route to remove
 *
 * @deprecated Use elm_map_route_del() instead
 *
 * @see elm_map_route_add()
 *
 */
EINA_DEPRECATED EAPI void                  elm_map_route_remove(Elm_Map_Route *route);

/**
 * @deprecated Use elm_object_item_data_get instead.
 */
EINA_DEPRECATED EAPI void * elm_multibuttonentry_item_data_get(const Elm_Object_Item *it);

/**
 * @deprecated Use elm_object_item_data_set instead.
 */
EINA_DEPRECATED EAPI void elm_multibuttonentry_item_data_set(Elm_Object_Item *it, void *data);

/**
 * @deprecated Use elm_calendar_select_mode_set instead.
 */
EINA_DEPRECATED EAPI void                 elm_calendar_day_selection_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @deprecated Use elm_calendar_select_mode_get instead.
 */
EINA_DEPRECATED EAPI Eina_Bool            elm_calendar_day_selection_disabled_get(const Evas_Object *obj);

/**
 * @brief Enumeration of values that should be used in conjunction with elm_notify_orient_set() to
 *        set the position in which the notification should appear(relative to its parent)
 *        and in conjunction with elm_notify_orient_get() to know where the notification
 *        is appearing.
 *
 * @deprecated Possible orient values for a notification.
 */
typedef enum
{
   ELM_NOTIFY_ORIENT_TOP, /**< Notification should appear at the top of the parent, default */
   ELM_NOTIFY_ORIENT_CENTER, /**< Notification should appear in the center of the parent */
   ELM_NOTIFY_ORIENT_BOTTOM, /**< Notification should appear at the bottom of the parent */
   ELM_NOTIFY_ORIENT_LEFT, /**< Notification should appear on the left of the parent */
   ELM_NOTIFY_ORIENT_RIGHT, /**< Notification should appear on the right of the parent */
   ELM_NOTIFY_ORIENT_TOP_LEFT, /**< Notification should appear in the top left corner of the parent */
   ELM_NOTIFY_ORIENT_TOP_RIGHT, /**< Notification should appear in the top right corner of the parent */
   ELM_NOTIFY_ORIENT_BOTTOM_LEFT, /**< Notification should appear in the bottom left corner of the parent */
   ELM_NOTIFY_ORIENT_BOTTOM_RIGHT, /**< Notification should appear in the bottom right corner of the parent */
   ELM_NOTIFY_ORIENT_LAST /**< Sentinel value, @b don't use */
} Elm_Notify_Orient;


/**
 * @deprecated Use elm_notify_align_set instead.
 */
EINA_DEPRECATED EAPI void                         elm_notify_orient_set(Evas_Object *obj, Elm_Notify_Orient orient);

/**
 * @deprecated Use elm_notify_align_get instead.
 */
EINA_DEPRECATED EAPI Elm_Notify_Orient            elm_notify_orient_get(const Evas_Object *obj);

/**
 * @brief Sets the slide effect for the label widget.
 *
 * @remarks If set to @c true, the text of the label slides/scrolls through the length of the
 *          label.
 *
 * @remarks This only works with the themes "slide_short", "slide_long", and
 *          "slide_bounce".
 *
 * @remarks This doesn't work if the line wrap(elm_label_line_wrap_set()) or
 *          ellipsis(elm_label_ellipsis_set()) is set.
 *
 * @param obj The label object
 * @param slide If @c true slide effect is shown, otherwise @c false
 *
 * @deprecated see elm_label_slide_mode_set() instead.
 */
EINA_DEPRECATED EAPI void                        elm_label_slide_set(Evas_Object *obj, Eina_Bool slide);

/**
 * @brief Gets the whether slide effect is shown.
 *
 * @param obj The label object
 * @return @c true if the slide effect is shown,
 *         otherwise @c false
 *
 * @deprecated use elm_label_slide_mode_get() instead.
 *
 * @see elm_label_slide_set()
 */
EINA_DEPRECATED EAPI Eina_Bool                   elm_label_slide_get(const Evas_Object *obj);

/**
 * @brief Sets the text for an object's part, marking it as translatable.
 *
 * @remarks The string to set as @a text must be the original one. Do not pass the
 *          return of gettext() here. Elementary translates the string
 *          internally and sets it on the object using elm_object_part_text_set(),
 *          also storing the original string so that it can be automatically
 *          translated when the language is changed with elm_language_set().
 *
 * @remarks The @a domain is also stored to find the translation in the
 *          correct catalog. It can be @c NULL, in which case it uses whatever
 *          domain is set by the application with textdomain(). This is useful
 *          in case you are building a library on top of Elementary that is going to have
 *          its own translatable strings, that should not be mixed with those of
 *          programs using the library.
 *
 * @param obj The object containing the text part
 * @param part The name of the part to set
 * @param domain The translation domain to use
 * @param text The original, non-translated text to set
 *
 * @deprecated Use elm_object_domain_translatable_part_text_set() instead.
 */
EINA_DEPRECATED EAPI void      elm_object_domain_translatable_text_part_set(Evas_Object *obj, const char *part, const char *domain, const char *text);

/**
 * @brief Gets the original string set as translatable for an object.
 *
 * @remarks When setting translated strings, the function elm_object_part_text_get()
 *          returns the translation returned by gettext(). To get the
 *          original string use this function.
 *
 * @param obj The object
 * @param part The name of the part that is set
 *
 * @return The original, untranslated string
 *
 * @deprecated Use elm_object_translatable_part_text_get() instead.
 */
EINA_DEPRECATED EAPI const char *elm_object_translatable_text_part_get(const Evas_Object *obj, const char *part);

/**
 * @brief Shows or hides the title area.
 *
 * @remarks When the title area is invisible, then the controls are hidden so as
 *          to expand the content area to full-size.
 *
 * @param it The naviframe item
 * @param visible If @c EINA_TRUE, the title area is visible, otherwise if @c EINA_FALSE it is hidden
 *
 * @deprecated Use elm_naviframe_item_title_enabled_set() instead.
 *
 * @see also elm_naviframe_item_title_visible_get()
 * @see also elm_naviframe_item_title_enabled_get()
 */
EINA_DEPRECATED EAPI void      elm_naviframe_item_title_visible_set(Elm_Object_Item *it, Eina_Bool visible);

/**
 * @brief Gets a value that indicates whether the title area is visible.
 *
 * @param it The naviframe item
 * @return @c EINA_TRUE if the title area is visible,
 *         otherwise @c EINA_FALSE
 *
 * @deprecated Use elm_naviframe_item_title_enabled_get() instead.
 *
 * @see elm_naviframe_item_title_visible_set()
 */
EINA_DEPRECATED EAPI Eina_Bool elm_naviframe_item_title_visible_get(const Elm_Object_Item *it);

