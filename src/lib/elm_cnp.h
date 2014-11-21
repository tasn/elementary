#ifndef _ELM_CNP_H
#define _ELM_CNP_H
/**
 * @internal
 * @defgroup CopyPaste CopyPaste
 * @ingroup elm_infra_group
 *
 * Implements the following functionality:
 *    a. select, copy/cut, and paste
 *    b. clipboard
 *    c. drag and drop
 * in order to share data across application windows.
 *
 * Contains functions to select text or a portion of data,
 * send it to a buffer, and paste the data into a target.
 *
 * elm_cnp provides a generic copy and paste facility based on its windowing system.
 * It is not necessary to know the details of each windowing system,
 * but some terms and behavior are common.
 * Currently the X11 window system is widely used, and only X11 functionality is implemented.
 *
 * In the X11R6 window system, CopyPaste works like peer-to-peer communication.
 * Copying is an operation on an object in an X server.
 * X11 calls those objects as 'selections' which have names.
 * Generally, two selection types are needed for copy and paste:
 * The Primary selection and the Clipboard selection.
 * Primary selection is for selecting text (that means highlighted text).
 * Clipboard selection is for explicit copying behavior
 * (such as ctrl+c, or 'copy' in a menu).
 * Thus, in applications, most cases only use the clipboard selection.
 * As stated before, taking ownership of a selection doesn't move any actual data.
 * Copying and Pasting is described as follows:
 *  1. Copy text in Program A : Program A takes ownership of the selection.
 *  2. Paste text in Program B : Program B notes that Program A owns the selection.
 *  3. Program B asks A for the text.
 *  4. Program A responds and sends the text to Program B.
 *  5. Program B pastes the response.
 * More information is on
 *  - http://www.jwz.org/doc/x-cut-and-paste.html
 *  - X11R6 Inter-Client Communication Conventions Manual, section 2
 *
 * TODO: Add for other window system.
 *
 * @{
 */

/**
 * @brief Enumeration that defines the types of selection property names.
 * @see http://www.x.org/docs/X11/xlib.pdf for more details.
 */
typedef enum
{
   ELM_SEL_TYPE_PRIMARY, /**< Primary text selection (highlighted or selected text) */
   ELM_SEL_TYPE_SECONDARY, /**< Used when primary selection is in use */
   ELM_SEL_TYPE_XDND, /**< Drag 'n' Drop */
   ELM_SEL_TYPE_CLIPBOARD, /**< Clipboard selection (ctrl+C) */
} Elm_Sel_Type;

/**
 * @brief Enumeration that defines the types of content.
 */
typedef enum
{
   /** For matching every possible item */
   ELM_SEL_FORMAT_TARGETS =   -1,
   /** Content is from outside of Elementary */
   ELM_SEL_FORMAT_NONE    =  0x0,
   /** Plain unformatted text: Used for things that don't want rich markup */
   ELM_SEL_FORMAT_TEXT    = 0x01,
   /** Edje textblock markup, including inline images */
   ELM_SEL_FORMAT_MARKUP  = 0x02,
   /** Images */
   ELM_SEL_FORMAT_IMAGE   = 0x04,
   /** Vcards */
   ELM_SEL_FORMAT_VCARD   = 0x08,
   /** Raw HTML-like data (eg. webkit) */
   ELM_SEL_FORMAT_HTML    = 0x10,
} Elm_Sel_Format;

/**
 * @brief Enumeration that defines the kind of action associated with drop data, if for XDND.
 * @since 1.8
 */
typedef enum
{
   ELM_XDND_ACTION_UNKNOWN, /**< Action type is unknown */
   ELM_XDND_ACTION_COPY, /**< Copy the data */
   ELM_XDND_ACTION_MOVE, /**< Move the data */
   ELM_XDND_ACTION_PRIVATE, /**< Private action type */
   ELM_XDND_ACTION_ASK, /**< Ask the user what to do */
   ELM_XDND_ACTION_LIST, /**< List the data */
   ELM_XDND_ACTION_LINK, /**< Link the data */
   ELM_XDND_ACTION_DESCRIPTION /**< Describe the data */
} Elm_Xdnd_Action;

/**
 * @brief The structure type holding information about selected data.
 */
struct _Elm_Selection_Data
{
   Evas_Coord       x, y;
   Elm_Sel_Format   format;
   void            *data;
   size_t           len;
   Elm_Xdnd_Action  action; /**< The action to perform with the data @since 1.8 */
};
typedef struct _Elm_Selection_Data Elm_Selection_Data;

/**
 * @brief Called when the selected data is 'dropped' at its destination.
 *
 * @remarks FIXME: This should probably be a smart callback.
 *
 * @param[in] data The application specific data
 * @param[in] obj The evas object where the selected data is 'dropped'
 * @param[in] ev The struct holding information about the selected data
 */
typedef Eina_Bool (*Elm_Drop_Cb)(void *data, Evas_Object *obj, Elm_Selection_Data *ev);

/**
 * @brief Called to find out what object is under (x,y) coordinates.
 *
 * @param[in] obj The container object
 * @param[in] x The coordinate to check
 * @param[in] y The coordinate to check
 * @param[in] xposret The position relative to the item (left (-1), middle (0), right (1))
 * @param[in] yposret The position relative to the item (upper (-1), middle (0), bottom (1))
 * @return The object under x,y cordinates, otherwise @c NULL if not found
 */
typedef Elm_Object_Item *(*Elm_Xy_Item_Get_Cb)(Evas_Object *obj, Evas_Coord x, Evas_Coord y, int *xposret, int *yposret);

/**
 * @brief Called when the selection ownership for a given selection is lost.
 *
 * @since 1.7
 *
 * @param[in] data The application specific data
 * @param[in] selection The selection that is lost
 */
typedef void (*Elm_Selection_Loss_Cb)(void *data, Elm_Sel_Type selection);

/**
 * @brief Called to create a drag icon object.
 *
 * @since 1.8
 *
 * @param[in] data The application specific data
 * @param[in] win The window to create the relative objects
 * @param[out] xoff The return coordinate for the X offset at which to place the drag icon object relative to the source drag object
 * @param[out] yoff The return coordinate for the Y offset at which to place the drag icon object relative to the source drag object
 * @return The object to fill the drag window with, otherwise @c NULL if not needed
 */
typedef Evas_Object *(*Elm_Drag_Icon_Create_Cb) (void *data, Evas_Object *win, Evas_Coord *xoff, Evas_Coord *yoff);

/**
 * @brief Called when a drag finishes, enters, or leaves an object.
 * 
 * @since 1.8
 *
 * @param[in] data The application specific data
 * @param[in] obj The object where the drag started
 */
typedef void (*Elm_Drag_State) (void *data, Evas_Object *obj);

/**
 * @brief Called when a drag is finished.
 *
 * @since 1.8
 *
 * @param[in] data The application specific data
 * @param[in] obj The object where the drag started
 * @param[in] accepted If @c true the dropped data is accepted on drop, 
 *                 otherwise @c false
 */
typedef void (*Elm_Drag_Done) (void *data, Evas_Object *obj, Eina_Bool accepted);

/**
 * @brief Called when a drag is responded to with an accept or deny.
 *
 * @since 1.8
 *
 * @param[in] data The application specific data
 * @param[in] obj The object where the drag started
 * @param[in] doaccept The boolean value that indicates whether the target accepts the drag
 */
typedef void (*Elm_Drag_Accept) (void *data, Evas_Object *obj, Eina_Bool doaccept);

/**
 * @brief Called when a drag is over an object, and gives object-relative coordinates.
 * 
 * @since 1.8
 *
 * @param[in] data The application specific data
 * @param[in] obj The object where the drag started
 * @param[in] x The X coordinate relative to the top-left corner of the object
 * @param[in] y The Y coordinate relative to the top-left corner of the object
 */
typedef void (*Elm_Drag_Pos) (void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y, Elm_Xdnd_Action action);

/**
 * @brief Called when a drag starts from an item container.
 *
 * @since 1.8
 *
 * @param[in] data The application specific data
 * @param[in] obj The object where the drag started
 */
typedef void (*Elm_Drag_Start) (void *data, Evas_Object *obj);

/**
 * @brief Sets the copy data for a widget.
 *
 * @details This sets the copy data and takes ownership of the selection. Format is used for specifying the selection type,
 *          and this is used during pasting.
 *
 * @param[in] selection The selection type for copying and pasting
 * @param[in] obj The source widget pointer
 * @param[in] format The selection format
 * @param[in] buf The data selected
 * @param[in] buflen The size of @a buf
 * @return @c EINA_TRUE if setting data is successful,
 *         otherwise @c EINA_FALSE
 *
 * @ingroup CopyPaste
 *
 */
EAPI Eina_Bool elm_cnp_selection_set(Evas_Object *obj, Elm_Sel_Type selection,
                                     Elm_Sel_Format format,
                                     const void *buf, size_t buflen);

/**
 * @brief Gets data from a widget that has a selection.
 *
 * @details This gets the current selection data from a widget.
 *          The widget input here is usually elm_entry,
 *          in which case @a datacb and @a udata can be @c NULL.
 *          If a different widget is passed, @a datacb and @a udata are used for retrieving data.
 *
 * @param[in] selection The selection type for copying and pasting
 * @param[in] format The selection format
 * @param[in] obj The source widget
 * @param[in] datacb The user data callback if the target widget isn't elm_entry
 * @param[in] udata The user data pointer for @a datacb
 * @return @c EINA_TRUE if getting the selection data is successful,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_cnp_selection_set()
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_cnp_selection_get(Evas_Object *obj, Elm_Sel_Type selection,
                                     Elm_Sel_Format format,
                                     Elm_Drop_Cb datacb, void *udata);

/**
 * @brief Clears the selection data of a widget.
 *
 * @details It clears all the data from the selection which is owned by a widget.
 *
 * @param[in] obj The source widget
 * @param[in] selection The selection type for copying and pasting
 * @return @c EINA_TRUE if clearing data is successful,
 *         otherwise @c EINA_FALSE
 *
 * @see also elm_cnp_selection_set()
 * @ingroup CopyPaste
 *
 */
EAPI Eina_Bool elm_object_cnp_selection_clear(Evas_Object *obj,
                                              Elm_Sel_Type selection);


/**
 * @brief Sets a function to be called when a selection is lost.
 *
 * @since 1.7
 *
 * @remarks The function @a func is set to be called when selection @a selection is lost
 *          to another process or when elm_cnp_selection_set() is called. If @a func
 *          is @c NULL then it is not called. @a data is passed as the data parameter to
 *          the callback functions and @a selection is passed in as the selection that
 *          has been lost.
 * 
 * @remarks elm_cnp_selection_set() and elm_object_cnp_selection_clear() automatically
 *          set this callback to @c NULL when called. If you wish to take the selection
 *          and then be notified of loss please this (for example):
 * 
 * @code
 * elm_cnp_selection_set(obj, ELM_SEL_TYPE_PRIMARY, ELM_SEL_FORMAT_TEXT, "hello", strlen(hello));
 * elm_cnp_selection_loss_callback_set(obj, ELM_SEL_TYPE_PRIMARY, loss_cb, NULL);
 * @endcode
 *
 *
 * @param[in] obj The object to indicate the window target/display system
 * @param[in] selection The selection to be notified of loss
 * @param[in] func The function to call
 * @param[in] data The data pointer passed to the function
 *
 * @see also elm_cnp_selection_set()
 * @ingroup CopyPaste
 */
EAPI void elm_cnp_selection_loss_callback_set(Evas_Object *obj, Elm_Sel_Type selection, Elm_Selection_Loss_Cb func, const void *data);

/**
 * @brief Sets the given object as a target for drops from drag-and-drop.
 *
 * @since 1.8
 *
 * @param[in] obj The target object
 * @param[in] format The formats supported for dropping
 * @param[in] entercb The function to call when the object is entered with a drag
 * @param[in] enterdata The application data to pass to enterdata
 * @param[in] leavecb The function to call when the object is left with a drag
 * @param[in] leavedata The application data to pass to leavedata
 * @param[in] poscb The function to call when the object has a drag over it
 * @param[in] posdata The application data to pass to posdata
 * @param[in] dropcb The function to call when a drop has occurred
 * @param[in] dropdata The application data to pass to dropcb
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drop_target_add(Evas_Object *obj, Elm_Sel_Format format, 
                                   Elm_Drag_State entercb, void *enterdata,
                                   Elm_Drag_State leavecb, void *leavedata,
                                   Elm_Drag_Pos poscb, void *posdata,
                                   Elm_Drop_Cb dropcb, void *dropdata);

/**
 * @brief Deletes the drop target status of an object.
 *
 * @since 1.8
 *
 * @param[in] obj The target object
 * @param[in] format The formats supported for dropping
 * @param[in] entercb The function to call when the object is entered with a drag
 * @param[in] enterdata The application data to pass to @a enterdata
 * @param[in] leavecb The function to call when the object is left with a drag
 * @param[in] leavedata The application data to pass to @a leavedata
 * @param[in] poscb The function to call when the object has a drag over it
 * @param[in] posdata The application data to pass to @a posdata
 * @param[in] dropcb The function to call when a drop has occurred
 * @param[in] dropdata The application data to pass to @a dropcb
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drop_target_del(Evas_Object *obj, Elm_Sel_Format format,
                                   Elm_Drag_State entercb, void *enterdata,
                                   Elm_Drag_State leavecb, void *leavedata,
                                   Elm_Drag_Pos poscb, void *posdata,
                                   Elm_Drop_Cb dropcb, void *dropdata);

/**
 * @brief Begins a drag given a source object is provided.
 *
 * @since 1.8
 *
 * @param[in] obj The source object
 * @param[in] format The drag formats supported by the data
 * @param[in] data The drag data itself (a string)
 * @param[in] action The drag action to be done
 * @param[in] createicon The function to call to create a drag object, otherwise @c NULL if not wanted
 * @param[in] createdata The application data passed to @a createicon
 * @param[in] dragpos The function called with each position of the drag, x, y being screen coordinates if possible, and @a action being the current action
 * @param[in] dragdata The application data passed to @a dragpos
 * @param[in] acceptcb The function called indicating if the drop target accepts (or does not) the drop data while dragging
 *
 * @param[in] acceptdata The application data passed to @a acceptcb
 * @param[in] dragdone The function to call when drag is done
 * @param[in] donecbdata The application data to pass to @a dragdone
 * @return @c EINA_TRUE if successful,
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drag_start(Evas_Object *obj, Elm_Sel_Format format,
                              const char *data, Elm_Xdnd_Action action,
                              Elm_Drag_Icon_Create_Cb createicon,
                              void *createdata,
                              Elm_Drag_Pos dragpos, void *dragdata,
                              Elm_Drag_Accept acceptcb, void *acceptdata,
                              Elm_Drag_State dragdone, void *donecbdata);

/**
 * @brief Cancels the current drag operation.
 *
 * @since 1.9
 *
 * @remarks It can only be initiated from the source window.
 *
 * @param[in] obj The source of the current drag
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drag_cancel(Evas_Object *obj);

/**
 * @brief Changes the current drag action.
 *
 * @since 1.8
 *
 * @param[in] obj The source of a drag, if a drag is underway
 * @param[in] action The drag action to be done
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drag_action_set(Evas_Object *obj, Elm_Xdnd_Action action);

/**
 * @brief Called when a drag is over an object.
 *
 * @since 1.8
 *
 * @param[in] data The application specific data
 * @param[in] cont The container object where the drag started
 * @param[in] it The object item in the container where the mouse-over happened
 * @param[in] x The X coordinate relative to the top-left corner of the object
 * @param[in] y The Y coordinate relative to the top-left corner of the object
 * @param[in] xposret The position relative to the item (left (-1), middle (0), right (1))
 * @param[in] yposret The position relative to the item (upper (-1), middle (0), bottom (1))
 * @param[in] action The drag action to be done
 */
typedef void (*Elm_Drag_Item_Container_Pos) (void *data, Evas_Object *cont, Elm_Object_Item *it, Evas_Coord x, Evas_Coord y, int xposret, int yposret, Elm_Xdnd_Action action);

/**
 * @brief Called when the selected data is 'dropped' on the container.
 *
 * @param[in] data The application specific data
 * @param[in] obj The evas object where the selected data is 'dropped'
 * @param[in] it The item in the container where the drop-cords are present
 * @param[in] ev The struct holding information about the selected data
 * @param[in] xposret The position relative to the item (left (-1), middle (0), right (1))
 * @param[in] yposret The position relative to the item (upper (-1), middle (0), bottom (1))
 */
typedef Eina_Bool (*Elm_Drop_Item_Container_Cb)(void *data, Evas_Object *obj, Elm_Object_Item *it, Elm_Selection_Data *ev, int xposret, int yposret);

/**
 * @brief The structure type describing user information for the drag process.
 *
 * @param format The drag formats supported by the data (output)
 * @param data The drag data itself (a string) (output)
 * @param icons If the value is not @c NULL, play default anim (output)
 * @param action The drag action to be done (output)
 * @param createicon The function to call to create a drag object, otherwise @c NULL if not wanted (output)
 * @param createdata The application data passed to @a createicon (output)
 * @param dragpos The function called with each position of the drag, x, y being screen coordinates if possible, and action being the current action (output)
 * @param dragdata The application data passed to @a dragpos (output)
 * @param acceptcb The function called indicating if the drop target accepts (or does not) the drop data while dragging (output)
 * @param acceptdata The application data passed to @a acceptcb (output)
 * @param dragdone The function to call when the drag is done (output)
 * @param donecbdata The application data to pass to @a dragdone (output)
 */
typedef struct _Elm_Drag_User_Info Elm_Drag_User_Info;

struct _Elm_Drag_User_Info
{
   Elm_Sel_Format format;
   const char *data;
   Eina_List *icons;
   Elm_Xdnd_Action action;
   Elm_Drag_Icon_Create_Cb createicon;
   void *createdata;
   Elm_Drag_Start dragstart;
   void *startcbdata;
   Elm_Drag_Pos dragpos;
   void *dragdata;
   Elm_Drag_Accept acceptcb;
   void *acceptdata;
   Elm_Drag_Done dragdone;
   void *donecbdata;
};

/**
 * @brief Called when starting to drag for a container.
 *
 * @param obj The container object
 * @param it The Elm_Object_Item pointer where the drag starts
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 */
typedef Eina_Bool (*Elm_Item_Container_Data_Get_Cb)(
      Evas_Object *obj,
      Elm_Object_Item *it,
      Elm_Drag_User_Info *info);

/**
 * @brief Sets an item container (list, genlist, grid) as the source of the drag.
 *
 * @since 1.8
 *
 * @param[in] obj The container object
 * @param[in] tm_to_anim The time period to wait before starting the animation
 * @param[in] tm_to_drag The time period to wait before starting dragging
 * @param[in] itemgetcb Callback to get the Evas_Object pointer for the item at (x,y)
 * @param[in] data_get  Callback to get drag information
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drag_item_container_add(Evas_Object *obj, double tm_to_anim, double tm_to_drag, Elm_Xy_Item_Get_Cb itemgetcb, Elm_Item_Container_Data_Get_Cb data_get);

/**
 * @brief Deletes a item container from the drag-source list.
 *
 * @since 1.8
 *
 * @param[in] obj The target object
 * @return @c EINA_TRUE if successful,
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drag_item_container_del(Evas_Object *obj);

/**
 * @brief Sets an item container (list, genlist, grid) as the target for the drop.
 *
 * @since 1.8
 *
 * @param obj The container object
 * @param format The formats supported for dropping
 * @param itemgetcb Callback to get the Evas_Object pointer for the item at (x,y)
 * @param entercb The function to call when the object is entered with a drag
 * @param enterdata The application data to pass to @a enterdata
 * @param leavecb The function to call when the object is left with a drag
 * @param leavedata The application data to pass to @a leavedata
 * @param poscb The function to call when the object has a drag over it
 * @param posdata The application data to pass to @a posdata
 * @param dropcb The function to call when a drop has occurred
 * @param dropdata The application data to pass to @a dropcb
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drop_item_container_add(Evas_Object *obj,
      Elm_Sel_Format format,
      Elm_Xy_Item_Get_Cb itemgetcb,
      Elm_Drag_State entercb, void *enterdata,
      Elm_Drag_State leavecb, void *leavedata,
      Elm_Drag_Item_Container_Pos poscb, void *posdata,
      Elm_Drop_Item_Container_Cb dropcb, void *dropdata);

/**
 * @brief Removes a container from the list of drop targets.
 *
 * @since 1.8
 *
 * @param[in] obj The container object
 * @return @c EINA_TRUE if successful, 
 *         otherwise @c EINA_FALSE if not
 *
 * @ingroup CopyPaste
 */
EAPI Eina_Bool elm_drop_item_container_del(Evas_Object *obj);

/**
 * @}
 */
#endif
