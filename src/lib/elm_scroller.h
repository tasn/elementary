/**
 * @defgroup Scroller Scroller
 * @ingroup elm_widget_group
 *
 * @image html scroller_inheritance_tree.png
 * @image latex scroller_inheritance_tree.eps
 *
 * @brief A Scroller can contain a scrollable object.
 *
 * A scroller holds (and clips) a single object and "scrolls it
 * around". This means that it allows the user to use a scroll bar (or
 * a finger) to drag the viewable region around, moving through a much
 * larger object that is contained in the scroller. The scroller is
 * always having a small minimum size by default as it is not limited
 * by the contents of the scroller.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for scroller objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @li @c "edge,left" - The left edge of the content has been reached.
 * @li @c "edge,right" - The right edge of the content has been reached.
 * @li @c "edge,top" - The top edge of the content has been reached.
 * @li @c "edge,bottom" - The bottom edge of the content has been reached.
 * @li @c "scroll" - The content has been scrolled (moved).
 * @li @c "scroll,anim,start" - Scrolling animation has started.
 * @li @c "scroll,anim,stop" - Scrolling animation has stopped.
 * @li @c "scroll,drag,start" - Dragging the contents around has started.
 * @li @c "scroll,drag,stop" - Dragging the contents around has stopped.
 *
 * This widget implements the elm-scrollable-interface interface.
 * Its (non-deprecated) API functions, except for elm_scroller_add(),
 * which gives basic scroller objects, are meant to be a basis for all
 * other scrollable widgets (i.e. widgets implementing 
 * elm-scrollable-interface). So, they work on both pristine
 * scroller widgets and on other "specialized" scrollable widgets.
 *
 * The @c "scroll,anim,*" and @c "scroll,drag,*" signals are
 * only emitted by user intervention.
 *
 * When Elementary is under its default profile and theme (meant
 * for touch interfaces), scroll bars are @b not draggable,
 * their function is merely to indicate how much has been scrolled.
 *
 * When Elementary is under its desktop/standard profile and
 * theme, the thumb scroll (a.k.a. finger scroll) won't work.
 *
 * Default content parts of the scroller widget that you can use are:
 * @li @c "default" - Content of the scroller.
 *
 * @{
 */

/**
 * @brief Enumeration that defines types that control when scrollbars should appear.
 *
 * @since_tizen 2.3
 *
 * @see elm_scroller_policy_set()
 */
typedef enum
{
   ELM_SCROLLER_POLICY_AUTO = 0, /**< Show scrollbars as needed */
   ELM_SCROLLER_POLICY_ON, /**< Always show scrollbars */
   ELM_SCROLLER_POLICY_OFF, /**< Never show scrollbars */
   ELM_SCROLLER_POLICY_LAST
} Elm_Scroller_Policy;

/**
 * @brief Enumeration that defines types that control how the content is scrolled.
 *
 * @since_tizen 2.3
 *
 * @see elm_scroller_single_direction_set()
 */
typedef enum
{
   ELM_SCROLLER_SINGLE_DIRECTION_NONE = 0, /**< Scroll in every direction */
   ELM_SCROLLER_SINGLE_DIRECTION_SOFT, /**< Scroll in a single direction if the direction is certain */
   ELM_SCROLLER_SINGLE_DIRECTION_HARD, /**< Scroll only in a single direction */
   ELM_SCROLLER_SINGLE_DIRECTION_LAST
} Elm_Scroller_Single_Direction;

/**
 * @brief Enumeration that defines types that block the scroll movement in one or more directions.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @see elm_scroller_movement_block()
 */
typedef enum
{
    ELM_SCROLLER_MOVEMENT_NO_BLOCK = 1 << 0, /**< Do not block movements */
    ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL = 1 << 1, /**< Block vertical movements */
    ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL = 1 << 2 /**< Block horizontal movements */
} Elm_Scroller_Movement_Block;

/**
 * @brief Adds a new scroller to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object                  *elm_scroller_add(Evas_Object *parent);

/**
 * @internal
 *
 * @brief Sets the custom theme elements for the scroller.
 *
 * @param obj The scroller object
 * @param widget The widget name to use (default is "scroller")
 * @param base The base name to use (default is "base")
 *
 * @deprecated Use elm_layout_theme_set() instead.
 */
EINA_DEPRECATED EAPI void          elm_scroller_custom_widget_base_theme_set(Evas_Object *obj, const char *widget, const char *base);

/**
 * @brief Makes the scroller's minimum size limited to the minimum size of the content.
 *
 * @since_tizen 2.3
 *
 * @remarks By default the scroller is as small as its design allows,
 *          irrespective of its content. This makes the scroller's minimum size the
 *          right size horizontally and/or vertically to perfectly fit its content in
 *          that direction.
 *
 * @param[in] obj The scroller object
 * @param[in] w The boolean value that enables limiting minimum size horizontally
 * @param[in] h The boolean value that enables limiting minimum size vertically
 */
EAPI void                          elm_scroller_content_min_limit(Evas_Object *obj, Eina_Bool w, Eina_Bool h);

/**
 * @brief Shows a specific virtual region within the scroller content object.
 *
 * @since_tizen 2.3
 *
 * @remarks This ensures that all (or part if it does not fit) of the designated
 *          region in the virtual content object ((0, 0) starting at the top-left of the
 *          virtual content object) is shown within the scroller.
 *
 * @param[in] obj The scroller object
 * @param[in] x The x coordinate of the region
 * @param[in] y The y coordinate of the region
 * @param[in] w The width of the region
 * @param[in] h The height of the region
 */
EAPI void                          elm_scroller_region_show(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

/**
 * @brief Sets the scrollbar visibility policy.
 *
 * @details This sets the scrollbar visibility policy for the given scroller.
 *          @c ELM_SCROLLER_POLICY_AUTO means the scrollbar is made visible if it is
 *          needed, and otherwise kept hidden. @c ELM_SCROLLER_POLICY_ON turns it on all
 *          the time, and @c ELM_SCROLLER_POLICY_OFF always keeps it off. This applies
 *          respectively for the horizontal and vertical scrollbars.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[in] policy_h The horizontal scrollbar policy
 * @param[in] policy_v The vertical scrollbar policy
 */
EAPI void                          elm_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v);

/**
 * @brief Gets the scrollbar visibility policy.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[out] policy_h The horizontal scrollbar policy
 * @param[out] policy_v The vertical scrollbar policy
 *
 * @see elm_scroller_policy_set()
 */
EAPI void                          elm_scroller_policy_get(const Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v);

/**
 * @brief Sets the type of a single direction scroll.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[in] single_dir The type of single direction
 *
 * @see elm_scroller_single_direction_get()
 */
EAPI void                          elm_scroller_single_direction_set(Evas_Object *obj, Elm_Scroller_Single_Direction single_dir);

/**
 * @brief Gets the type of a single direction scroll.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @return The type of single direction
 *
 * @see elm_scroller_single_direction_get()
 */
EAPI Elm_Scroller_Single_Direction elm_scroller_single_direction_get(const Evas_Object *obj);

/**
 * @brief Gets the currently visible content region.
 *
 * @details This gets the current region in the content object that is visible through
 *          the scroller. The region co-ordinates are returned in the values that @a x, @a y, @a
 *          w, and @a h point to.
 *
 * @since_tizen 2.3
 *
 * @remarks All the coordinates are relative to the content.
 *
 * @param[in] obj The scroller object
 * @param[out] x The x coordinate of the region
 * @param[out] y The y coordinate of the region
 * @param[out] w The width of the region
 * @param[out] h The height of the region
 *
 * @see elm_scroller_region_show()
 */
EAPI void                          elm_scroller_region_get(const Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Gets the size of the content object.
 *
 * @details This gets the size of the content object of the scroller.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[out] w The width of the content object
 * @param[out] h The height of the content object
 */
EAPI void                          elm_scroller_child_size_get(const Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Sets the bounce behavior.
 *
 * @since_tizen 2.3
 *
 * @remarks When scrolling, the scroller may "bounce" when reaching an edge of the
 *          content object. This is a visual way to indicate that the end has been reached.
 *          This is enabled by default for both axis. This API is set if it is enabled
 *          for the given axis with the boolean parameters for each axis.
 *
 * @param[in] obj The scroller object
 * @param[in] h_bounce If @c EINA_TRUE horizontal bouncing is enabled,
 *                 otherwise @c EINA_FALSE to disable it
 * @param[in] v_bounce If @c EINA_TRUE vertical bouncing is enabled
 *                 otherwise @c EINA_FALSE to disable it
 */
EAPI void                          elm_scroller_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce);

/**
 * @brief Gets the bounce behaviour.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[out] h_bounce If @c EINA_TRUE horizontal bouncing is enabled
 *                 otherwise @c EINA_FALSE to disable it
 * @param[out] v_bounce If @c EINA_TRUE vertical bouncing is enabled
 *                 otherwise @c EINA_FALSE to disable it
 *
 * @see elm_scroller_bounce_set()
 */
EAPI void                          elm_scroller_bounce_get(const Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce);


/**
 * @internal
 * @remarks Tizen only feature
 */
EAPI void                          elm_scroller_origin_reverse_set(Evas_Object *obj, Eina_Bool origin_x, Eina_Bool origin_y);

/**
 * @internal
 * @remarks Tizen only feature
 */
EAPI void                          elm_scroller_origin_reverse_get(Evas_Object *obj, Eina_Bool *origin_x, Eina_Bool *origin_y);

/**
 * @brief Sets the scroll page size relative to the viewport size.
 *
 * @since_tizen 2.3
 *
 * @remarks The scroller is capable of limiting scrolling by the user to "pages". That
 *          is to jump by and only show a "whole page" at a time as if the continuous
 *          area of the scroller content is split into page sized pieces. This sets
 *          the size of a page relative to the viewport of the scroller. @c 1.0 is "1
 *          viewport" which is the size (horizontally or vertically). @c 0.0 turns it off in that
 *          axis. This is mutually exclusive with the page size
 *          (see elm_scroller_page_size_set()  for more information). Likewise @c 0.5
 *          is "half a viewport". Usable values are normally between @c 0.0 and @c 1.0
 *          including @c 1.0. If you only want @c 1 axis to be page "limited", use @c 0.0 for
 *          the other axis.
 *
 * @param[in] obj The scroller object
 * @param[in] h_pagerel The horizontal page relative size
 * @param[in] v_pagerel The vertical page relative size
 */
EAPI void                          elm_scroller_page_relative_set(Evas_Object *obj, double h_pagerel, double v_pagerel);

/**
 * @brief Gets a given scroller widget's scrolling page size, relative to
 *        its viewport size.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[out] h_pagerel The pointer to a variable in which to store the
 *                  horizontal page (relative) size
 * @param[out] v_pagerel The pointer to a variable in which to store the
 *                  vertical page (relative) size
 *
 * @see elm_scroller_page_relative_set()
 */
EAPI void                          elm_scroller_page_relative_get(const Evas_Object *obj, double *h_pagerel, double *v_pagerel);

/**
 * @brief Sets the scroll page size.
 *
 * @details This sets the page size to an absolute fixed value, with @c 0 turning it off
 *          for that axis.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[in] h_pagesize The horizontal page size
 * @param[in] v_pagesize The vertical page size
 *
 * @see elm_scroller_page_relative_set()
 * @see elm_scroller_page_size_get()
 */
EAPI void                          elm_scroller_page_size_set(Evas_Object *obj, Evas_Coord h_pagesize, Evas_Coord v_pagesize);

/**
 * @brief Retrieves a scroller widget's current page size.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[out] h_pagesize The location to store its horizontal page size
 * @param[out] v_pagesize The location to store its vertical page size
 *
 * @see elm_scroller_page_size_set() for more details
 * @see elm_scroller_page_relative_set()
 */
EAPI void                          elm_scroller_page_size_get(const Evas_Object *obj, Evas_Coord *h_pagesize, Evas_Coord *v_pagesize);

/**
 * @brief Sets the maximum limit of the movable page at flicking.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks The value of the maximum movable page should be more than @c 1.
 *
 * @param[in] obj The scroller object
 * @param[in] page_limit_h The maximum limit of the movable horizontal page
 * @param[in] page_limit_v The maximum limit of the movable vertical page
 *
 * @see elm_scroller_page_scroll_limit_get()
 */
EAPI void                          elm_scroller_page_scroll_limit_set(Evas_Object *obj, int page_limit_h, int page_limit_v);

/**
 * @brief Gets the maximum limit of the movable page at flicking.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[out] page_limit_h The maximum limit of the movable horizontal page
 * @param[out] page_limit_v The maximum limit of the movable vertical page
 *
 * @see elm_scroller_page_scroll_limit_set()
 */
EAPI void                          elm_scroller_page_scroll_limit_get(Evas_Object *obj, int *page_limit_h, int *page_limit_v);

/**
 * @brief Gets the scroll's current page number.
 *
 * @since_tizen 2.3
 *
 * @remarks The page number starts from @c 0. @c 0 is the first page.
 *          Current page means the page which meets the top-left corner of the viewport.
 *          If there are two or more pages in the viewport, it returns the number of the pages
 *          that meet the top-left corner of the viewport.
 *
 * @param[in] obj The scroller object
 * @param[out] h_pagenumber The horizontal page number
 * @param[out] v_pagenumber The vertical page number
 *
 * @see elm_scroller_last_page_get()
 * @see elm_scroller_page_show()
 * @see elm_scroller_page_bring_in()
 */
EAPI void                          elm_scroller_current_page_get(const Evas_Object *obj, int *h_pagenumber, int *v_pagenumber);

/**
 * @brief Gets the scroll's last page number.
 *
 * @details This returns the last page number among the pages.
 *
 * @since_tizen 2.3
 *
 * @remarks The page number starts from @c 0. @c 0 is the first page.
 *
 * @param[in] obj The scroller object
 * @param[out] h_pagenumber The horizontal page number
 * @param[out] v_pagenumber The vertical page number
 *
 * @see elm_scroller_current_page_get()
 * @see elm_scroller_page_show()
 * @see elm_scroller_page_bring_in()
 */
EAPI void                          elm_scroller_last_page_get(const Evas_Object *obj, int *h_pagenumber, int *v_pagenumber);

/**
 * @brief Shows a specific virtual region within the scroller content object by the page number.
 *
 * @since_tizen 2.3
 *
 * @remarks (0, 0) of the indicated page is located at the top-left corner of the viewport.
 *          This jumps to the page directly without animation.
 *
 * Example of usage:
 *
 * @code
 * sc = elm_scroller_add(win);
 * elm_object_content_set(sc, content);
 * elm_scroller_page_relative_set(sc, 1, 0);
 * elm_scroller_current_page_get(sc, &h_page, &v_page);
 * elm_scroller_page_show(sc, h_page + 1, v_page);
 * @endcode
 *
 * @param[in] obj The scroller object
 * @param[in] h_pagenumber The horizontal page number
 * @param[in] v_pagenumber The vertical page number
 *
 * @see elm_scroller_page_bring_in()
 */
EAPI void                          elm_scroller_page_show(Evas_Object *obj, int h_pagenumber, int v_pagenumber);

/**
 * @brief Shows a specific virtual region within the scroller content object by the page number.
 *
 * @since_tizen 2.3
 *
 * @remarks (0, 0) of the indicated page is located at the top-left corner of the viewport.
 *          This slides to the page with animation.
 *
 * Example of usage:
 *
 * @code
 * sc = elm_scroller_add(win);
 * elm_object_content_set(sc, content);
 * elm_scroller_page_relative_set(sc, 1, 0);
 * elm_scroller_last_page_get(sc, &h_page, &v_page);
 * elm_scroller_page_bring_in(sc, h_page, v_page);
 * @endcode
 *
 * @param[in] obj The scroller object
 * @param[in] h_pagenumber The horizontal page number
 * @param[in] v_pagenumber The vertical page number
 *
 * @see elm_scroller_page_show()
 */
EAPI void                          elm_scroller_page_bring_in(Evas_Object *obj, int h_pagenumber, int v_pagenumber);

/**
 * @brief Shows a specific virtual region within the scroller content object.
 *
 * @since_tizen 2.3
 *
 * @remarks This ensures that all (or part, if it does not fit) of the designated
 *          region in the virtual content object ((0, 0) starting at the top-left of the
 *          virtual content object) is shown within the scroller. Unlike
 *          elm_scroller_region_show(), this allows the scroller to "smoothly slide"
 *          to this location (if configuration in general calls for transitions). It
 *          may not jump immediately to the new location and may take a while and
 *          show other content along the way.
 *
 * @param[in] obj The scroller object
 * @param[in] x The x coordinate of the region
 * @param[in] y The y coordinate of the region
 * @param[in] w The width of the region
 * @param[in] h The height of the region
 *
 * @see elm_scroller_region_show()
 */
EAPI void                          elm_scroller_region_bring_in(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

/**
 * @brief Sets event propagation on a scroller.
 *
 * @details This enables or disables event propagation from the scroller
 *          content to the scroller and its parent. By default event
 *          propagation is @b enabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @param[in] propagation The boolean value that indicates whether propagation is enabled
 */
EAPI void                          elm_scroller_propagate_events_set(Evas_Object *obj, Eina_Bool propagation);

/**
 * @brief Gets event propagation for a scroller.
 *
 * @details This gets the event propagation for a scroller.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @return The propagation state
 *
 * @see elm_scroller_propagate_events_set()
 */
EAPI Eina_Bool                     elm_scroller_propagate_events_get(const Evas_Object *obj);

/**
 * @brief Sets the scrolling gravity on a scroller.
 *
 * @since_tizen 2.3
 *
 * @remarks The gravity, defines how the scroller adjusts its view
 *          when the size of the scroller content increases.
 *
 * @remarks The scroller adjusts the view to glue itself as follows:
 *
 *          x=0.0, for showing the left most region of the content.
 *          x=1.0, for showing the right most region of the content.
 *          y=0.0, for showing the bottom most region of the content.
 *          y=1.0, for showing the top most region of the content.
 *
 * @remarks Default value for x and y is @c 0.0.
 *
 * @param[in] obj The scroller object
 * @param[in] x The scrolling horizontal gravity
 * @param[in] y The scrolling vertical gravity
 */
EAPI void                          elm_scroller_gravity_set(Evas_Object *obj, double x, double y);

/**
 * @brief Gets scrolling gravity values for a scroller.
 *
 * @since_tizen 2.3
 *
 * @remarks This gets gravity values for a scroller.
 *
 * @param[in] obj The scroller object
 * @param[out] x The scrolling horizontal gravity
 * @param[out] y The scrolling vertical gravity
 *
 * @see elm_scroller_gravity_set()
 */
EAPI void                          elm_scroller_gravity_get(const Evas_Object *obj, double *x, double *y);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sets the mouse wheel event on a scroller.
 *
 * @details This enables or disables the mouse wheel event for a scroller.
 *          By default mouse wheel event is enabled.
 *
 * @param obj The scroller object
 * @param disabled The boolean value that indicates whether the wheel event is disabled
 */
EAPI void                          elm_scroller_wheel_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the mouse wheel event for a scroller.
 *
 * @remarks This gets whether the mouse wheel event is disabled.
 *
 * @param obj The scroller object
 *
 * @see elm_scroller_wheel_disabled_set()
 */
EAPI Eina_Bool                      elm_scroller_wheel_disabled_get(const Evas_Object *obj);

/**
 * @remarks Tizen only feature (but will be patched into upstream)
 *
 * @brief Sets the infinite loop for a scroller.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks This sets the infinite loop for a scroller.
 *
 * @param[in] obj The scroller object
 * @param[in] loop_h The scrolling horizontal loop
 * @param[in] loop_v The scrolling vertical loop
 */
EAPI void                          elm_scroller_loop_set(Evas_Object *obj, Eina_Bool loop_h, Eina_Bool loop_v);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the infinite loop for a scroller.
 *
 * @since 1.8
 *
 * @remarks This gets the infinite loop for a scroller.
 *
 * @param[in] obj The scroller object
 * @param[out] loop_h The scrolling horizontal loop
 * @param[out] loop_v The scrolling vertical loop
 *
 * @see elm_scroller_loop_set()
 */

EAPI void                          elm_scroller_loop_get(const Evas_Object *obj, Eina_Bool *loop_h, Eina_Bool *loop_v);

/**
 * @brief Sets blocking of scrolling (per axis) on a given scroller.
 *
 * @details This function blocks the scrolling movement (by input of a user) in
 *          a given direction. One can disable movements in the X axis, the Y
 *          axis, or both. The default value is @c ELM_SCROLLER_MOVEMENT_NO_BLOCK,
 *         where movements are allowed in both directions.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks What makes this function different from
 *          elm_object_scroll_freeze_push(), elm_object_scroll_hold_push(), and
 *          elm_object_scroll_lock_x_set() (or elm_object_scroll_lock_y_set())
 *          is that it @b doesn't propagate its effects to any parent or child
 *          widget of @a obj. Only the target scrollable widget is locked
 *          with regards to scrolling.
 *
 * @param[in] obj The scroller object
 * @param[in] block The axis to block
 */
EAPI void                         elm_scroller_movement_block_set(Evas_Object *obj, Elm_Scroller_Movement_Block block);

/**
 * @brief Gets a scroller's scroll blocking state.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The scroller object
 * @return The blocking state
 *
 * @see elm_scroller_movement_block_set()
 */
EAPI Elm_Scroller_Movement_Block  elm_scroller_movement_block_get(const Evas_Object *obj);


/**
 * @}
 */
