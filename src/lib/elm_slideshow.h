/**
 * @internal
 * @defgroup Slideshow Slideshow
 * @ingroup elm_widget_group
 *
 * @image html slideshow_inheritance_tree.png
 * @image latex slideshow_inheritance_tree.eps
 *
 * @image html img/widget/slideshow/preview-00.png
 * @image latex img/widget/slideshow/preview-00.eps
 *
 * @brief A Slideshow provide a slideshow for pre-made images.
 *
 * This widget, as the name indicates, is a pre-made image
 * slideshow panel, with API functions acting on the (child) image
 * items presentation. Between those actions, are:
 * - advance to next/previous image.
 * - select the style of image transition animation.
 * - set the exhibition time for each image,
 * - start/stop the slideshow.
 *
 * The transition animations are defined in the widget's theme.
 * Consequently, new animations can be added without having to
 * update the widget's code.
 *
 * @section Slideshow_Items Slideshow items
 *
 * For slideshow items, just like for @ref Genlist "genlist" ones,
 * the user defines @b classes, specifying functions that are
 * called on the item's creation and deletion times.
 *
 * The #Elm_Slideshow_Item_Class structure contains the following
 * members:
 *
 * - @c func.get - When an item is displayed, this function is
 *   called, and it's where one should create the item object, de
 *   facto. For example, the object can be a pure Evas image object
 *   or an Elementary @ref Photocam "photocam" widget.
 *   See #SlideshowItemGetFunc.
 * - @c func.del - When an item is no more displayed, this function
 *   is called, where the user must delete any data associated to
 *   the item. See #SlideshowItemDelFunc.
 *
 * @section Slideshow_Caching Slideshow caching
 *
 * The slideshow provides facilities to have items adjacent to the
 * one being displayed <b>already "realized"</b> (i.e. loaded) for
 * you, so that the system does not have to decode the image data
 * anymore when it has to actually switch images on its
 * viewport. The user is able to set the number of items to be
 * cached @b before and @b after the current item, in the widget's
 * item list.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for slideshow objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - When the slideshow switches its view to a new
 *   item. The @a event_info parameter in the callback contains the current visible item
 * - @c "transition,end" - When a slide transition ends. The @a event_info parameter
 *   in the callback contains the current visible item.
 *
 * @{
 */

/**
 * @brief typedef to struct _Elm_Slideshow_Item_Class
 */
typedef struct _Elm_Slideshow_Item_Class      Elm_Slideshow_Item_Class;
typedef struct _Elm_Slideshow_Item_Class_Func Elm_Slideshow_Item_Class_Func;    /**< Class functions for slideshow item classes */
typedef Evas_Object                        *(*SlideshowItemGetFunc)(void *data, Evas_Object *obj); /**< Image fetching class function for slideshow item classes */
typedef void                                (*SlideshowItemDelFunc)(void *data, Evas_Object *obj); /**< Deletion class function for slideshow item classes */

/**
 * @internal
 * @struct _Elm_Slideshow_Item_Class
 *
 * @brief The structure type for slideshow item class definition. See @ref Slideshow_Items for
 *        field details.
 */
struct _Elm_Slideshow_Item_Class
{
   struct _Elm_Slideshow_Item_Class_Func
     {
        SlideshowItemGetFunc get;
        SlideshowItemDelFunc del;
     } func; 
};

/**
 * @brief Adds a new slideshow widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new slideshow widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return A new slideshow widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object          *elm_slideshow_add(Evas_Object *parent);

/**
 * @brief Adds (appends) a new item to a given slideshow widget.
 *
 * @details This adds a new item to the @a obj internal list of items, appending it.
 *          The item's class must contain the function that really fetches the
 *          image object to show for this item, which could be an Evas image
 *          object or an Elementary photo, for example. The @a data
 *          parameter is going to be passed to both class functions of the
 *          item.
 *
 * @param[in] obj The slideshow object
 * @param[in] itc The item class for the item
 * @param[in] data The item's data
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see #Elm_Slideshow_Item_Class
 * @see elm_slideshow_item_sorted_insert()
 * @see elm_object_item_data_set()
 */
EAPI Elm_Object_Item      *elm_slideshow_item_add(Evas_Object *obj, const Elm_Slideshow_Item_Class *itc, const void *data);

/**
 * @brief Inserts a new item into the given slideshow widget, using the @a func
 *        function to sort items (by item handles).
 *
 * @remarks It adds a new item to the @a obj internal list of items, in a position
 *          determined by the @a func comparing function. The item's class
 *          must contain the function that really fetches the image object to
 *          show for this item, which could be an Evas image object or an
 *          Elementary photo, for example. The @a data parameter is going to
 *          be passed to both class functions of the item.
 *
 * @param[in] obj The slideshow object
 * @param[in] itc The item class for the item
 * @param[in] data The item data
 * @param[in] func The comparing function to be used to sort the slideshow
 *             items <b>by #Elm_Slideshow_Item_Class item handles</b>
 * @return The slideshow item handle on success,
 *         otherwise @c NULL in case of an error
 *
 * @see #Elm_Slideshow_Item_Class
 * @see elm_slideshow_item_add()
 */
EAPI Elm_Object_Item      *elm_slideshow_item_sorted_insert(Evas_Object *obj, const Elm_Slideshow_Item_Class *itc, const void *data, Eina_Compare_Cb func);

/**
 * @brief Displays a given slideshow widget's item, programmatically.
 *
 * @remarks The change between the current item and @a it uses the
 *          transition @a obj that is set to use (@see
 *          elm_slideshow_transition_set()).
 *
 * @param[in] it The item to display on the @a obj viewport
 */
EAPI void                  elm_slideshow_item_show(Elm_Object_Item *it);

/**
 * @brief Slides to the @b next item, in a given slideshow widget.
 *
 * @remarks The sliding animation @a obj is set to use the
 *          transition effect that is used after this call is issued.
 *
 * @remarks If the end of the slideshow's internal list of items is
 *          reached, it wraps around the list's beginning, again.
 *
 * @param[in] obj The slideshow object
 */
EAPI void                  elm_slideshow_next(Evas_Object *obj);

/**
 * @brief Slides to the @b previous item, in a given slideshow widget.
 *
 * @remarks The sliding animation @a obj is set to use the
 *          transition effect used after this call is issued.
 *
 * @remarks If the beginning of the slideshow's internal list of items
 *          is reached, it wraps around the list's end, again.
 *
 * @param[in] obj The slideshow object
 */
EAPI void                  elm_slideshow_previous(Evas_Object *obj);

/**
 * @brief Returns the list of sliding transition/effect names available, for a
 *        given slideshow widget.
 *
 * @remarks The transitions, which come from the @a obj theme, must be an EDC
 *          data item named @c "transitions" on the theme file, with (prefix)
 *          names of EDC programs actually implementing them.
 *
 * @remarks The available transitions for slideshows on the default theme are:
 *          - @c "fade" - The current item fades out, while the new one
 *                        fades in to the slideshow's viewport.
 *          - @c "black_fade" - The current item fades to black, and just
 *                              then, the new item fades in.
 *          - @c "horizontal" - The current item slides horizontally, until
 *                              it gets out of the slideshow's viewport, while the new item
 *                              comes from the left to take its place.
 *          - @c "vertical" - The current item slides vertically, until it
 *                            gets out of the slideshow's viewport, while the new item comes
 *                            from the bottom to take its place.
 *          - @c "square" - The new item starts to appear from the middle of
 *                          the current one, but with a tiny size, growing until its
 *                          target (full) size and covering the old one.
 *
 * @remarks The stringshared strings get no new references
 *          exclusive to the user grabbing the list, here, so if you would like
 *          to use them out of this call's context, you would better
 *          eina_stringshare_ref() them. Also the list is an internal list and
 *          is only valid for as long as the slideshow object is valid, and
 *          has not internally changed its list for some reason, so make a
 *          copy if you need it.
 *
 * @param[in] obj The slideshow object
 * @return The list of transitions (list of @b stringshared strings
 *         as data)
 *
 * @see elm_slideshow_transition_set()
 */
EAPI const Eina_List      *elm_slideshow_transitions_get(const Evas_Object *obj);

/**
 * @brief Sets the current slide transition/effect in use for a given
 *        slideshow widget.
 *
 * @remarks If @a transition is implemented in the @a obj theme (i.e., is
 *          contained in the list returned by
 *          elm_slideshow_transitions_get()), this new sliding effect is
 *          used on the widget.
 *
 * @param[in] obj The slideshow object
 * @param[in] transition The new transition's name string
 *
 * @see elm_slideshow_transitions_get()
 */
EAPI void                  elm_slideshow_transition_set(Evas_Object *obj, const char *transition);

/**
 * @brief Gets the current slide transition/effect in use for a given
 *        slideshow widget.
 *
 * @param[in] obj The slideshow object
 * @return The current transition's name
 *
 * @see elm_slideshow_transition_set()
 */
EAPI const char           *elm_slideshow_transition_get(const Evas_Object *obj);

/**
 * @brief Sets the interval between each image transition on a given
 *        slideshow widget, <b>and the start of the slideshow, itself</b>
 *
 * @remarks After this call, the slideshow widget starts cycling its
 *          view, sequentially and automatically, with the images of the
 *          items that it has. The time between each new image displayed is going
 *          to be @a timeout, in @b seconds. If a different timeout is set
 *          previously and a slideshow is in progress, it continues
 *          with the new time between transitions, after this call.
 *
 * @remarks A value less than or equal to @c 0 on @a timeout disables
 *          the widget's internal timer, thus halting any slideshow which
 *          could be happening on @a obj.
 *
 * @param[in] obj The slideshow object
 * @param[in] timeout The new displaying timeout for images
 *
 * @see elm_slideshow_timeout_get()
 */
EAPI void                  elm_slideshow_timeout_set(Evas_Object *obj, double timeout);

/**
 * @brief Gets the interval set for image transitions on a given slideshow
 *        widget.
 *
 * @param[in] obj The slideshow object
 * @return The timeout set on it, otherwise @c -1.0 in case of an error
 *
 * @see elm_slideshow_timeout_set()
 */
EAPI double                elm_slideshow_timeout_get(const Evas_Object *obj);

/**
 * @brief Sets whether after a slideshow is started, for a given slideshow
 *        widget, its items should be displayed cyclically.
 *
 * @remarks elm_slideshow_next() and elm_slideshow_previous() @b
 *          ignore what is set by this functions, i.e., they @b always
 *          cycle through items. This affects only the "automatic"
 *          slideshow, as set by elm_slideshow_timeout_set().
 *
 * @param[in] obj The slideshow object
 * @param[in] loop If @c EINA_TRUE it cycles through items,
 *             otherwise @c EINA_FALSE for it to stop at the end of the @a obj internal
 *             list of items
 *
 * @see elm_slideshow_loop_get()
 */
EAPI void                  elm_slideshow_loop_set(Evas_Object *obj, Eina_Bool loop);

/**
 * @brief Gets whether after a slideshow is started, for a given slideshow
 *        widget, its items are to be displayed cyclically.
 *
 * @param[in] obj The slideshow object
 * @return @c EINA_TRUE if the items in @a obj are cycled,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_slideshow_loop_set()
 */
EAPI Eina_Bool             elm_slideshow_loop_get(const Evas_Object *obj);

/**
 * @brief Removes all items from a given slideshow widget.
 *
 * @details This removes (and deletes) all items in @a obj, leaving it
 *          empty.
 *
 * @param[in] obj The slideshow object
 *
 * @see elm_object_item_del(), to remove just one item.
 */
EAPI void                  elm_slideshow_clear(Evas_Object *obj);

/**
 * @brief Gets the internal list of items in a given slideshow widget.
 *
 * @remarks This list is @b not to be modified in any way and must not be
 *          freed. Use the list members with functions like
 *          elm_object_item_del() and elm_object_item_data_get().
 *
 * @remarks This list is only valid until @a obj object's internal
 *          items list is changed. It should be fetched again with another
 *          call to this function when changes happen.
 *
 * @param[in] obj The slideshow object
 * @return The list of items (#Elm_Object_Item as data),
 *         otherwise @c NULL in case of an error
 */
EAPI const Eina_List      *elm_slideshow_items_get(const Evas_Object *obj);

/**
 * @brief Returns the currently displayed item, in a given slideshow widget.
 *
 * @param[in] obj The slideshow object
 * @return A handle to the item being displayed in @a obj,
 *         otherwise @c NULL, if none are present (and on errors)
 */
EAPI Elm_Object_Item      *elm_slideshow_item_current_get(const Evas_Object *obj);

/**
 * @brief Gets the real Evas object created to implement the view of a
 *        given slideshow item.
 *
 * @details This returns the actual Evas object used to implement the
 *          specified slideshow item's view. This may be @c NULL, as it may
 *          not have been created or may have been deleted, at any time, by
 *          the slideshow. <b>Do not modify this object</b> (move, resize,
 *          show, hide, etc.), as the slideshow is controlling it. This
 *          function is for querying, emitting custom signals, or hooking
 *          lower level callbacks for events on that object. Do not delete
 *          this object under any circumstances.
 *
 * @param[in] it The slideshow item
 * @return The Evas object implementing this item's view
 *
 * @see elm_object_item_data_get()
 */
EAPI Evas_Object          *elm_slideshow_item_object_get(const Elm_Object_Item *it);

/**
 * @brief Gets the item, in a given slideshow widget, placed at
 *        position @a nth, in its internal items list.
 *
 * @param[in] obj The slideshow object
 * @param[in] nth The number of the item to grab as a handle (@c 0 being
 *            the first)
 * @return The item stored in @a obj at position @a nth, otherwise @c NULL,
 *         if there's no item with that index (and on errors)
 */
EAPI Elm_Object_Item      *elm_slideshow_item_nth_get(const Evas_Object *obj, unsigned int nth);

/**
 * @brief Sets the current slide layout in use for a given slideshow widget.
 *
 * @remarks If @p layout is implemented in the @a obj theme (i.e., it is contained
 *          in the list returned by elm_slideshow_layouts_get()), this new
 *          images layout is used on the widget.
 *
 * @param[in] obj The slideshow object
 * @param[in] layout The new layout's name string
 *
 * @see elm_slideshow_layouts_get()
 */
EAPI void                  elm_slideshow_layout_set(Evas_Object *obj, const char *layout);

/**
 * @brief Gets the current slide layout in use for a given slideshow widget
 *
 * @param[in] obj The slideshow object
 * @return The current layout's name
 *
 * @see elm_slideshow_layout_set()
 */
EAPI const char           *elm_slideshow_layout_get(const Evas_Object *obj);

/**
 * @brief Returns the list of @b layout names available, for a given
 *        slideshow widget.
 *
 * @remarks Slideshow layouts change how the widget is to dispose each
 *          image item in its viewport, with regards to cropping, scaling,
 *          etc.
 *
 * @remarks The layouts, which come from the @a obj theme, must be an EDC
 *          data item name @c "layouts" on the theme file, with (prefix)
 *          names of EDC programs actually implementing them.
 *
 * @remarks The available layouts for slideshows on the default theme are:
 *          - @c "fullscreen" - Item images with original aspect, scaled to
 *            touch top and down slideshow borders or, if the image's height
 *            is not enough, left and right slideshow borders.
 *          - @c "not_fullscreen" - The same behavior as the @c "fullscreen"
 *            one, but always leaves 10% of the slideshow's dimensions of the
 *            distance between the item image's borders and the slideshow
 *            borders, for each axis.
 *
 * @remarks The stringshared strings get no new references
 *          exclusive to the user grabbing the list, here, so if you would like
 *          to use them out of this call's context, you would better
 *          eina_stringshare_ref() them.
 *
 * @param[in] obj The slideshow object
 * @return The list of layouts (list of @b stringshared strings
 *         as data)
 *
 * @see elm_slideshow_layout_set()
 */
EAPI const Eina_List      *elm_slideshow_layouts_get(const Evas_Object *obj);

/**
 * @brief Sets the number of items to cache, on a given slideshow widget,
 *        <b>before the current item</b>.
 *
 * @remarks The default value for this property is @c 2. See
 *          @ref Slideshow_Caching "slideshow caching" for more details.
 *
 * @param[in] obj The slideshow object
 * @param[in] count The number of items to cache before the current one
 *
 * @see elm_slideshow_cache_before_get()
 */
EAPI void                  elm_slideshow_cache_before_set(Evas_Object *obj, int count);

/**
 * @brief Retrieves the number of items to cache, on a given slideshow widget,
 *        <b>before the current item</b>.
 *
 * @param[in] obj The slideshow object
 * @return The number of items set to be cached before the current one
 *
 * @see elm_slideshow_cache_before_set()
 */
EAPI int                   elm_slideshow_cache_before_get(const Evas_Object *obj);

/**
 * @brief Sets the number of items to cache, on a given slideshow widget,
 *        <b>after the current item</b>.
 *
 * @remarks The default value for this property is @c 2. See
 *          @ref Slideshow_Caching "slideshow caching" for more details.
 *
 * @param[in] obj The slideshow object
 * @param[in] count The number of items to cache after the current one
 *
 * @see elm_slideshow_cache_after_get()
 */
EAPI void                  elm_slideshow_cache_after_set(Evas_Object *obj, int count);

/**
 * @brief Retrieves the number of items to cache, on a given slideshow widget,
 *        <b>after the current item</b>.
 *
 * @param[in] obj The slideshow object
 * @return The number of items set to be cached after the current one
 *
 * @see elm_slideshow_cache_after_set()
 */
EAPI int                   elm_slideshow_cache_after_get(const Evas_Object *obj);

/**
 * @brief Gets the number of items stored in a given slideshow widget.
 *
 * @param[in] obj The slideshow object
 * @return The number of items on @a obj at the time of this call
 */
EAPI unsigned int          elm_slideshow_count_get(const Evas_Object *obj);

/**
 * @}
 */
