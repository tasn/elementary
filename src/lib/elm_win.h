/**
 * @defgroup Win Win
 * @ingroup elm_widget_group
 *
 * @image html win_inheritance_tree.png
 * @image latex win_inheritance_tree.eps
 *
 * @brief The window class of Elementary contains functions to manipulate
 *        Windows.
 *
 * The Evas engine that is used to render the window contents is specified
 * in the system or user elementary config files (whichever is found last),
 * and can be overridden by using the ELM_ENGINE environment variable for
 * testing.  Engines that may be supported (depending on Evas and Ecore-Evas
 * compilation setup and modules that are actually installed at runtime) are
 * (listed in order of best supported and most likely to be complete and work 
 * to lowest quality).
 *
 * @li "x11", "x", "software-x11", "software_x11" (Software rendering in X11)
 * @li "gl", "opengl", "opengl-x11", "opengl_x11" (OpenGL or OpenGL-ES2
 * rendering in X11)
 * @li "shot:..." (Virtual screenshot renderer - renders to output file and
 * exits)
 * @li "fb", "software-fb", "software_fb" (Linux framebuffer direct software
 * rendering)
 * @li "sdl", "software-sdl", "software_sdl" (SDL software rendering to SDL
 * buffer)
 * @li "gl-sdl", "gl_sdl", "opengl-sdl", "opengl_sdl" (OpenGL or OpenGL-ES2
 * rendering using SDL as the buffer)
 * @li "gdi", "software-gdi", "software_gdi" (Windows WIN32 rendering via
 * GDI with software)
 * @li "dfb", "directfb" (Rendering to a DirectFB window)
 * @li "x11-8", "x8", "software-8-x11", "software_8_x11" (Rendering in
 * grayscale using a dedicated 8bit software engine in X11)
 * @li "x11-16", "x16", "software-16-x11", "software_16_x11" (Rendering in
 * X11 using a 16bit software engine)
 * @li "wince-gdi", "software-16-wince-gdi", "software_16_wince_gdi"
 * (Windows CE rendering via GDI with a 16bit software renderer)
 * @li "sdl-16", "software-16-sdl", "software_16_sdl" (Rendering to SDL
 * buffer with a 16bit software renderer)
 * @li "ews" (rendering to EWS - Ecore + Evas Single Process Windowing System)
 * @li "gl-cocoa", "gl_cocoa", "opengl-cocoa", "opengl_cocoa" (OpenGL rendering in Cocoa)
 * @li "psl1ght" (PS3 rendering using PSL1GHT)
 *
 * All engines use a simple string to select the engine to render, EXCEPT
 * the "shot" engine. This actually encodes the output of the virtual
 * screenshot and the time for which to delay the engine string. The engine string
 * is encoded in the following way:
 *
 *   "shot:[delay=XX][:][repeat=DDD][:][file=XX]"
 *
 * Where options are separated by a ":" char, if more than one option is
 * given with delay, if provided being the first option and file the last
 * (order is important). The delay specifies how long to wait after the
 * window is displayed before doing the virtual "in memory" rendering and then
 * saving the output to the file specified by the file option (and then exiting).
 * If no delay is given, the default value is @c 0.5 seconds. If no file is given the
 * default output file is "out.png". The repeat option is for continuous
 * capturing of screenshots. The repeat range is from @c 1 to @c 999 and the filename is
 * fixed to "out001.png" Some examples of using the shot engine are:
 *
 *   ELM_ENGINE="shot:delay=1.0:repeat=5:file=elm_test.png" elementary_test
 *   ELM_ENGINE="shot:delay=1.0:file=elm_test.png" elementary_test
 *   ELM_ENGINE="shot:file=elm_test2.png" elementary_test
 *   ELM_ENGINE="shot:delay=2.0" elementary_test
 *   ELM_ENGINE="shot:" elementary_test
 *
 * Signals for which you can add callbacks are:
 *
 * @li "delete,request": The user requested to close the window. See
 * elm_win_autodel_set().
 * @li "focus,in": The window got focus.
 * @li "focus,out": The window lost focus.
 * @li "moved": The window that holds the canvas is moved.
 * @li "withdrawn": The window is still being managed normally but is removed from the view.
 * @li "iconified": The window is minimized (perhaps into an icon or a taskbar).
 * @li "normal": The window is in a normal state (not withdrawn or iconified).
 * @li "stick": The window has become sticky (shows on all desktops).
 * @li "unstick": The window has stopped being sticky.
 * @li "fullscreen": The window has become fullscreen.
 * @li "unfullscreen": The window has stopped being fullscreen.
 * @li "maximized": The window has been maximized.
 * @li "unmaximized": The window has stopped being maximized.
 * @li "wm,rotation,changed": The rotation of the window has been changed by the Windows Manager.
 * @li "ioerr": A low-level I/O error has occurred in the display system.
 *
 * @{
 */
/**
 * @brief Enumeration that defines the types of windows that can be created.
 *
 * @since_tizen 2.3
 *
 * @remarks These are hints set on the window so that a running Windows Manager knows
 *          how the window should be handled and/or what kind of decorations it
 *          should have.
 *
 * @remarks Currently, only the X11 backed engines use them.
 */
typedef enum
{
   ELM_WIN_UNKNOWN, /**< Unknown window type */
   ELM_WIN_BASIC, /**< A normal window. It indicates a normal, top-level
                     window. Almost every window is created with this
                     type */
   ELM_WIN_DIALOG_BASIC, /**< Used for simple dialog windows */
   ELM_WIN_DESKTOP, /**< Used for special desktop windows, like a background
                       window holding desktop icons */
   ELM_WIN_DOCK, /**< The window is used as a dock or panel. Usually would
                    be kept on top of any other window by the Windows
                    Manager */
   ELM_WIN_TOOLBAR, /**< The window is used to hold a floating toolbar, or
                       something similar */
   ELM_WIN_MENU, /**< This is similar to #ELM_WIN_TOOLBAR */
   ELM_WIN_UTILITY, /**< A persistent utility window, like a toolbox or
                       palette */
   ELM_WIN_SPLASH, /**< A splash window for an application that is starting up */
   ELM_WIN_DROPDOWN_MENU, /**< The window is a dropdown menu, when an
                             entry in a menubar is clicked. Typically used
                             with elm_win_override_set(). This hint exists
                             for completion only, as the EFL way of
                             implementing a menu would not normally use a
                             separate window for its contents */
   ELM_WIN_POPUP_MENU, /**< Like #ELM_WIN_DROPDOWN_MENU, but used for the menu
                          triggered by right-clicking an object */
   ELM_WIN_TOOLTIP, /**< The window is a tooltip. A short piece of
                       explanatory text that typically appears after the
                       mouse cursor hovers over an object for a while.
                       Typically used with elm_win_override_set() and also
                       not very commonly used in EFL */
   ELM_WIN_NOTIFICATION, /**< A notification window, like a warning about
                            battery life or a new email that is received */
   ELM_WIN_COMBO, /**< A window holding the contents of a combo box. Not
                     usually used in EFL */
   ELM_WIN_DND, /**< Used to indicate that the window is a representation of an
                   object being dragged across different windows, or even
                   applications. Typically used with
                   elm_win_override_set() */
   ELM_WIN_INLINED_IMAGE, /**< The window is rendered onto an image
                             buffer. No actual window is created for this
                             type, instead the window and all of its
                             contents are rendered to an image buffer.
                             This allows to have a children window inside a
                             parent one just like any other object would
                             be, and do other things like applying @c
                             Evas_Map effects to it. This is the only type
                             of window that requires the @a parent
                             parameter of elm_win_add() to be a valid @c
                             Evas_Object */
   ELM_WIN_SOCKET_IMAGE,/**< The window is rendered onto an image buffer
			     and can be shown another process's plug image object.
			     No actual window is created for this type,
			     instead the window and all of its contents are
			     rendered to an image buffer and can be shown
			     another process's plug image object */
   // Tizen Only(20131219): Dynamic Box is only supported in the Tizen
   ELM_WIN_DYNAMIC_BOX,
} Elm_Win_Type;

/**
 * @brief Enumeration that defines the different layouts that can be requested for the virtual keyboard.
 *
 * @since_tizen 2.3
 *
 * @remarks When the application window is being managed by Illume, it may request
 *          any of the following layouts for the virtual keyboard.
 */
typedef enum
{
   ELM_WIN_KEYBOARD_UNKNOWN, /**< Unknown keyboard state */
   ELM_WIN_KEYBOARD_OFF, /**< Request to deactivate the keyboard */
   ELM_WIN_KEYBOARD_ON, /**< Enable keyboard with default layout */
   ELM_WIN_KEYBOARD_ALPHA, /**< Alpha (a-z) keyboard layout */
   ELM_WIN_KEYBOARD_NUMERIC, /**< Numeric keyboard layout */
   ELM_WIN_KEYBOARD_PIN, /**< PIN keyboard layout */
   ELM_WIN_KEYBOARD_PHONE_NUMBER, /**< Phone keyboard layout */
   ELM_WIN_KEYBOARD_HEX, /**< Hexadecimal numeric keyboard layout */
   ELM_WIN_KEYBOARD_TERMINAL, /**< Full (QWERTY) keyboard layout */
   ELM_WIN_KEYBOARD_PASSWORD, /**< Password keyboard layout */
   ELM_WIN_KEYBOARD_IP, /**< IP keyboard layout */
   ELM_WIN_KEYBOARD_HOST, /**< Host keyboard layout */
   ELM_WIN_KEYBOARD_FILE, /**< File keyboard layout */
   ELM_WIN_KEYBOARD_URL, /**< URL keyboard layout */
   ELM_WIN_KEYBOARD_KEYPAD, /**< Keypad layout */
   ELM_WIN_KEYBOARD_J2ME /**< J2ME keyboard layout */
} Elm_Win_Keyboard_Mode;

/**
 * @brief Enumeration that defines the different indicator states.
 *
 * @since_tizen 2.3
 *
 * @remarks In some environments, like phones, you may have an indicator that
 *          shows the battery status, reception, time etc. This is the indicator.
 *
 * @remarks Sometimes you don't want it because you provide the same functionality
 *          inside your app, so this requests that the indicator be hidden in
 *          such a circumstance if you use @c ELM_ILLUME_INDICATOR_HIDE. The default
 *          is to have the indicator shown.
 */
typedef enum
{
   ELM_WIN_INDICATOR_UNKNOWN, /**< Unknown indicator state */
   ELM_WIN_INDICATOR_HIDE, /**< Hides the indicator */
   ELM_WIN_INDICATOR_SHOW /**< Shows the indicator */
} Elm_Win_Indicator_Mode;

/**
 * @brief Enumeration that defines the opacity modes of an indicator that can be shown.
 *
 * @since_tizen 2.3
 */

typedef enum
{
   ELM_WIN_INDICATOR_OPACITY_UNKNOWN, /**< Unknown indicator opacity mode */
   ELM_WIN_INDICATOR_OPAQUE, /**< Opacifies the indicator */
   ELM_WIN_INDICATOR_TRANSLUCENT, /**< Makes the indicator translucent */
   ELM_WIN_INDICATOR_TRANSPARENT, /**< Transparentizes the indicator */
   ELM_WIN_INDICATOR_BG_TRANSPARENT /**< Transparentizes the indicator background*/
} Elm_Win_Indicator_Opacity_Mode;

/**
 * @brief Enumeration that defines the type modes of an indicator that can be shown.
 *
 * @since_tizen 2.3
 *
 * @remarks If the indicator can support several types,
 *          you can use this enum value to deal with different types of indicators.
 */

typedef enum
{
   ELM_WIN_INDICATOR_TYPE_UNKNOWN, /**< Unknown indicator type mode */
   ELM_WIN_INDICATOR_TYPE_1, /**< Type 0 indicator */
   ELM_WIN_INDICATOR_TYPE_2, /**< Type 1 indicator */
} Elm_Win_Indicator_Type_Mode;

/**
 * @brief Enumeration that defines the available commands that can be sent to the Illume manager.
 *
 * @since_tizen 2.3
 *
 * @remarks When running under an Illume session, a window may send commands to the
 *          Illume manager to perform different actions.
 */
typedef enum
{
   ELM_ILLUME_COMMAND_FOCUS_BACK, /**< Reverts focus to the previous window */
   ELM_ILLUME_COMMAND_FOCUS_FORWARD, /**< Sends focus to the next window in the list */
   ELM_ILLUME_COMMAND_FOCUS_HOME, /**< Hides all windows to show the Home screen */
   ELM_ILLUME_COMMAND_CLOSE, /**< Closes the currently active window */
} Elm_Illume_Command;

/**
 * @brief Adds a window object. If this is the first window created, pass @c NULL as
 *        @a parent.
 *
 * @since_tizen 2.3
 *
 * @remarks The @a parent parameter can be @c NULL for every window @a type
 *          except @c ELM_WIN_INLINED_IMAGE, which needs a parent to retrieve the
 *          canvas on which the image object is created.
 *
 * @param[in] parent The parent object to add the window to, otherwise @c NULL
 * @param[in] name The name of the window
 * @param[in] type The window type, one from #Elm_Win_Type
 *
 * @return The created object, otherwise @c NULL on failure
 */
EAPI Evas_Object          *elm_win_add(Evas_Object *parent, const char *name, Elm_Win_Type type);

/**
 * @brief Gets the type of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object for which to get the type
 *
 * @return The type of a window object \n
 *         If the object is not a window object, return @c -1.
 */
EAPI Elm_Win_Type          elm_win_type_get(Evas_Object *obj);

/**
 * @brief Adds a window object with a standard setup.
 *
 * @since_tizen 2.3
 *
 * @remarks This creates a window like elm_win_add(), but also puts in a standard
 *          background using elm_bg_add() as well as setting the window title to
 *          @a title. The window type created is of type @c ELM_WIN_BASIC with @c NULL
 *          as the parent widget.
 *
 * @param[in] name The name of the window
 * @param[in] title The title of the window
 *
 * @return The created object, otherwise @c NULL on failure
 *
 * @see elm_win_add()
 */
EAPI Evas_Object          *elm_win_util_standard_add(const char *name, const char *title);

/**
 * @brief Adds @a subobj as a resize object of the window @a obj.
 *
 * @since_tizen 2.3
 *
 * @remarks Setting an object as a resize object of the window means that the
 *          @a subobj child's size and position is controlled by the window
 *          directly. That is, the object is resized to match the window size
 *          and should never be moved or resized manually by the developer.
 *
 * @remarks In addition, resize objects of the window control the minimum size
 *          of it as well as whether it can or cannot be resized by the user.
 *
 * @remarks For the end user to be able to resize a window by dragging the handles
 *          or borders provided by the Windows Manager, or using any other similar
 *          mechanism, all of the resize objects in the window should have their
 *          evas_object_size_hint_weight_set() set to EVAS_HINT_EXPAND.
 *
 * @remarks Also notice that the window can get resized to the current size of the
 *          object if EVAS_HINT_EXPAND is set @b after the call to
 *          elm_win_resize_object_add(). So if the object should get resized to the
 *          size of the window, set this hint @b before adding it as a resize object
 *          (this happens because the size of the window and the object are evaluated
 *          as soon as the object is added to the window).
 *
 * @param[in] obj The window object
 * @param[in] subobj The resize object to add
 */
EAPI void                  elm_win_resize_object_add(Evas_Object *obj, Evas_Object *subobj);

/**
 * @brief Deletes @a subobj as a resize object of the window @a obj.
 *
 * @details This function removes the object @a subobj from the resize objects of
 *          the window @a obj. It does not delete the object itself, which is
 *          left unmanaged and should be deleted by the developer, manually handled
 *          or set as the child of some other container.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] subobj The resize object to add
 */
EAPI void                  elm_win_resize_object_del(Evas_Object *obj, Evas_Object *subobj);

/**
 * @brief Sets the title of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] title The title to set
 */
EAPI void                  elm_win_title_set(Evas_Object *obj, const char *title);

/**
 * @brief Gets the title of the window.
 *
 * @since_tizen 2.3
 *
 * @remarks The returned string is an internal one and should not be freed or
 *          modified. It should also be rendered invalid if a new title is set or if
 *          the window is destroyed.
 *
 * @param[in] obj The window object
 * @return The title
 */
EAPI const char           *elm_win_title_get(const Evas_Object *obj);

/**
 * @brief Sets the icon name of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] icon_name The icon name to set
 */
EAPI void                  elm_win_icon_name_set(Evas_Object *obj, const char *icon_name);

/**
 * @brief Gets the icon name of the window.
 *
 * @since_tizen 2.3
 *
 * @remarks The returned string is an internal one and should not be freed or
 *          modified. It should also be rendered invalid if a new icon name is set or if
 *          the window is destroyed.
 *
 * @param[in] obj The window object
 * @return The icon name
 */
EAPI const char           *elm_win_icon_name_get(const Evas_Object *obj);

/**
 * @brief Sets the role of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] role The role to set
 */
EAPI void                  elm_win_role_set(Evas_Object *obj, const char *role);

/**
 * @brief Gets the role of the window.
 *
 * @since_tizen 2.3
 *
 * @remarks The returned string is an internal one and should not be freed or
 *          modified. It should also be rendered invalid if a new role is set or if
 *          the window is destroyed.
 *
 * @param[in] obj The window object
 * @return The role
 */
EAPI const char           *elm_win_role_get(const Evas_Object *obj);

/**
 * @brief Sets a window object icon.
 *
 * @details This sets an image to be used as the icon for the given window, in
 *          the Windows Manager decoration part. The exact pixel dimensions of
 *          the object (not object size) is used, and the image pixels
 *          are used as-is when this function is called. If the image
 *          object has been updated, then call this function again to source
 *          the image pixels and put them on the window's icon. Note that
 *          <b>only Evas image objects are allowed</b>.
 *
 * @since_tizen 2.3
 *
 * @remarks Example of usage:
 * @code
 *  icon = evas_object_image_add(evas_object_evas_get(elm_window));
 *  evas_object_image_file_set(icon, "/path/to/the/icon", NULL);
 *  elm_win_icon_object_set(elm_window, icon);
 *  evas_object_show(icon);
 * @endcode
 *
 * @param[in] obj The window object used to get an icon
 * @param[in] icon The Evas image object used as an icon
 */
EAPI void                  elm_win_icon_object_set(Evas_Object *obj, Evas_Object *icon);

/**
 * @brief Gets the icon object used for the window.
 *
 * @since_tizen 2.3
 *
 * @remarks The object returned is the one marked by elm_win_icon_object_set() as the
 *          object to use for the window icon.
 *
 * @param[in] obj The window object
 * @return The icon object to set
 */
EAPI const Evas_Object    *elm_win_icon_object_get(const Evas_Object *obj);

/**
 * @brief Sets the window autodel state.
 *
 * @since_tizen 2.3
 *
 * @remarks When closing the window in any way outside of the program control, like
 *          pressing the X button in the titlebar or using a command from the
 *          Windows Manager, a @c "delete,request" signal is emitted to indicate that
 *          this event has occurred and the developer can take an action, which may or may not
 *          include destroying the window object.
 *
 * @remarks When the @a autodel parameter is set, the window is automatically
 *          destroyed when this event occurs, after the signal is emitted.
 *          If @a autodel is @c EINA_FALSE, then the window is not destroyed
 *          and it is up to the program to decide when to do so if required.
 *
 * @param[in] obj The window object
 * @param[in] autodel If @c true the window automatically deletes itself when closed,
 *                otherwise @c false
 */
EAPI void                  elm_win_autodel_set(Evas_Object *obj, Eina_Bool autodel);

/**
 * @brief Gets the window autodel state.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window automatically deletes itself when closed,
 *         otherwise @c false
 *
 * @see elm_win_autodel_set()
 */
EAPI Eina_Bool             elm_win_autodel_get(const Evas_Object *obj);

/**
 * @brief Activates the window object.
 *
 * @details This function sends a request to the Windows Manager to activate the
 *          window pointed by @a obj. If honored by the WM, the window receives
 *          the keyboard focus.
 *
 * @since_tizen 2.3
 *
 * @remarks This is just a request that a Window Manager may ignore, so calling
 *          this function does not ensure in any way that the window is going to be the
 *          active one after it.
 *
 * @param[in] obj The window object
 */
EAPI void                  elm_win_activate(Evas_Object *obj);

/**
 * @brief Lowers the window object.
 *
 * @since_tizen 2.3
 *
 * @remarks Places the window pointed by @a obj at the bottom of the stack, so that
 *          no other window is covered by it.
 *
 * @remarks If elm_win_override_set() is not set, the Windows Manager may ignore this
 *          request.
 *
 * @param[in] obj The window object
 */
EAPI void                  elm_win_lower(Evas_Object *obj);

/**
 * @brief Raises the window object.
 *
 * @since_tizen 2.3
 *
 * @remarks Places the window pointed by @a obj at the top of the stack, so that it's
 *          not covered by any other window.
 *
 * @remarks If elm_win_override_set() is not set, the Windows Manager may ignore this
 *          request.
 *
 * @param[in] obj The window object
 */
EAPI void                  elm_win_raise(Evas_Object *obj);

/**
 * @brief Centers a window on its screen.
 *
 * @details This function centers a window @a obj horizontally and/or vertically based on the values
 *          of @a h and @a v.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] h If @c true it centers horizontally,
 *            otherwise @c false if it does not change the horizontal location
 * @param[in] v If @c true it centers vertically, 
 *            otherwise @c false if it does not change the vertical location
 */
EAPI void                  elm_win_center(Evas_Object *obj, Eina_Bool h, Eina_Bool v);

/**
 * @brief Sets the borderless state of a window.
 *
 * @details This function requests the Windows Manager to not draw any decoration
 *          around the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] borderless If @c true the window is borderless,
 *                   otherwise @c false
 */
EAPI void                  elm_win_borderless_set(Evas_Object *obj, Eina_Bool borderless);

/**
 * @brief Gets the borderless state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is borderless,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_borderless_get(const Evas_Object *obj);

/**
 * @brief Sets the shaped state of a window.
 *
 * @since_tizen 2.3
 *
 * @remarks Shaped windows, when supported, render the parts of the window that
 *          has no content and is transparent.
 *
 * @remarks If @a shaped is @c EINA_FALSE, then it is strongly advised to have some
 *          background object or cover the entire window in any other way, otherwise the
 *          parts of the canvas that have no data show framebuffer artifacts.
 *
 * @param[in] obj The window object
 * @param[in] shaped If @c true the window is shaped,
 *               otherwise @c false
 *
 * @see elm_win_alpha_set()
 */
EAPI void                  elm_win_shaped_set(Evas_Object *obj, Eina_Bool shaped);

/**
 * @brief Gets the shaped state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is shaped,
 *         otherwise @c false
 *
 * @see elm_win_shaped_set()
 */
EAPI Eina_Bool             elm_win_shaped_get(const Evas_Object *obj);

/**
 * @brief Sets the alpha channel state of a window.
 *
 * @since_tizen 2.3
 *
 * @remarks If @a alpha is @c EINA_TRUE, the alpha channel of the canvas is enabled
 *          possibly making parts of the window completely or partially transparent.
 *          This is also subject to the underlying system supporting it, like for
 *          example, running under a compositing manager. If no compositing is
 *          available, enabling this option results in using shaped
 *          windows with elm_win_shaped_set().
 *
 * @param[in] obj The window object
 * @param[in] alpha If @c true the window has an alpha channel,
 *              otherwise @c false
 *
 * @see elm_win_alpha_set()
 */
EAPI void                  elm_win_alpha_set(Evas_Object *obj, Eina_Bool alpha);

/**
 * @brief Gets the alpha channel state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window has an alpha channel,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_alpha_get(const Evas_Object *obj);

/**
 * @brief Sets the override state of a window.
 *
 * @since_tizen 2.3
 *
 * @remarks A window with @a override set to @c EINA_TRUE is not managed by the
 *          Windows Manager. This means that no decorations of any kind are shown
 *          for it, moving and resizing must be handled by the application as well
 *          as the window visibility.
 *
 * @remarks This should not be used for normal windows, and even for not so normal
 *          ones, it should only be used with a lot of care and
 *          when there's a good reason. Mishandling override windows may result in situations that
 *          disrupt the normal workflow of the end user.
 *
 * @param[in] obj The window object
 * @param[in] override If @c true the window is overridden,
 *                 otherwise @c false
 */
EAPI void                  elm_win_override_set(Evas_Object *obj, Eina_Bool override);

/**
 * @brief Gets the override state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is overridden,
 *         otherwise @c false
 *
 * @see elm_win_override_set()
 */
EAPI Eina_Bool             elm_win_override_get(const Evas_Object *obj);

/**
 * @brief Sets the fullscreen state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param fullscreen If @c true the window is fullscreen,
 *                   otherwise @c false
 */
EAPI void                  elm_win_fullscreen_set(Evas_Object *obj, Eina_Bool fullscreen);

/**
 * @brief Geta the fullscreen state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is fullscreen,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_fullscreen_get(const Evas_Object *obj);

/**
 * @brief Sets the maximized state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] maximized If @c true the window is maximized,
 *                  otherwise @c false
 */
EAPI void                  elm_win_maximized_set(Evas_Object *obj, Eina_Bool maximized);

/**
 * @brief Gets the maximized state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is maximized,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_maximized_get(const Evas_Object *obj);

/**
 * @brief Sets the iconified state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] iconified If @c true the window is iconified,
 *                  otherwise @c false
 */
EAPI void                  elm_win_iconified_set(Evas_Object *obj, Eina_Bool iconified);

/**
 * @brief Gets the iconified state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is iconified,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_iconified_get(const Evas_Object *obj);

/**
 * @brief Sets the withdrawn state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] withdrawn If @c true the window is withdrawn,
 *                  otherwise @c false
 */
EAPI void                  elm_win_withdrawn_set(Evas_Object *obj, Eina_Bool withdrawn);

/**
 * @brief Gets the withdrawn state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is withdrawn,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_withdrawn_get(const Evas_Object *obj);

/**
 * @brief Sets the profile list of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] profiles The list of profile names
 * @param[in] num_profiles The number of profile names
 */
EAPI void                  elm_win_profiles_set(Evas_Object *obj, const char **profiles, unsigned int num_profiles);

/**
 * @brief Gets the profile of a window.
 *
 * @since_tizen 2.3
 *
 * @remarks The returned string is an internal one and should not be freed or
 *          modified. It is also rendered invalid if a new role is set or if
 *          the window is destroyed.
 *
 * @param[in] obj The window object
 * @return The profile name
 */
EAPI const char           *elm_win_profile_get(const Evas_Object *obj);

/**
 * @brief Sets the urgent state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] urgent If @c true the window is urgent,
 *               otherwise @c false
 */
EAPI void                  elm_win_urgent_set(Evas_Object *obj, Eina_Bool urgent);

/**
 * @brief Gets the urgent state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is urgent,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_urgent_get(const Evas_Object *obj);

/**
 * @brief Sets the @a demand_attention state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] demand_attention If @c true the window is in the @a demand_attention state,
 *                         otherwise @c false
 */
EAPI void                  elm_win_demand_attention_set(Evas_Object *obj, Eina_Bool demand_attention);

/**
 * @brief Gets the @a demand_attention state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is in the @a demand_attention state,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_demand_attention_get(const Evas_Object *obj);

/**
 * @brief Sets the @a modal state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] modal If @c true the window is @a modal,
 *              otherwise @c false
 */
EAPI void                  elm_win_modal_set(Evas_Object *obj, Eina_Bool modal);

/**
 * @brief Gets the @a modal state of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return If @c true the window is @a modal,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_modal_get(const Evas_Object *obj);

/**
 * @brief Sets the aspect ratio of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] aspect If @c 0 the window has no aspect limits,
 *               otherwise it is width divided by height
 */
EAPI void                  elm_win_aspect_set(Evas_Object *obj, double aspect);

/**
 * @brief Gets the aspect ratio of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The set aspect ratio (@c 0 by default)
 */
EAPI double                elm_win_aspect_get(const Evas_Object *obj);

/**
 * @brief Sets the base window size used with stepping calculation.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks Base size + stepping is what is calculated for window sizing restrictions.
 *
 * @param[in] obj The window object
 * @param[in] w The base width
 * @param[in] h The base height
 *
 * @see elm_win_size_step_set
 * @see elm_win_size_base_get
 */
EAPI void                  elm_win_size_base_set(Evas_Object *obj, int w, int h);

/**
 * @brief Gets the base size of a window.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[out] w The pointer that stores the returned base width
 * @param[out] h The pointer that stores the returned base height
 *
 * @see elm_win_size_base_set
 * @see elm_win_size_step_set
 */
EAPI void                  elm_win_size_base_get(Evas_Object *obj, int *w, int *h);

/**
 * @brief Sets the window stepping used with sizing calculation.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks Base size + stepping is what is calculated for window sizing restrictions.
 *
 * @param[in] obj The window object
 * @param[in] w The stepping width (@c 0 disables)
 * @param[in] h The stepping height (@c 0 disables)
 *
 * @see elm_win_size_step_get
 * @see elm_win_size_base_set
 */
EAPI void                  elm_win_size_step_set(Evas_Object *obj, int w, int h);

/**
 * @brief Gets the stepping of a window.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[out] w The pointer that stores the returned stepping width
 * @param[out] h The pointer that stores the returned stepping height
 *
 * @see elm_win_size_base_set
 * @see elm_win_size_step_set
 */
EAPI void                  elm_win_size_step_get(Evas_Object *obj, int *w, int *h);

/**
 * @brief Sets the layer of the window.
 *
 * @since_tizen 2.3
 *
 * @remarks What this means exactly depends on the underlying engine used.
 *
 * @remarks In case of X11 backed engines, the value in @a layer has one of the
 *          following meanings:
 *          @li < 3: The window is placed below all others.
 *          @li > 5: The window is placed above all others.
 *          @li other: The window is placed in the default layer.
 *
 * @param[in] obj The window object
 * @param[in] layer The layer of the window
 */
EAPI void                  elm_win_layer_set(Evas_Object *obj, int layer);

/**
 * @brief Gets the layer of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The layer of the window
 *
 * @see elm_win_layer_set()
 */
EAPI int                   elm_win_layer_get(const Evas_Object *obj);

/**
 * @brief Pushes (incriments) the norender counter on the window.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks There are some occasions where you wish to suspend rendering on a window.
 *          You may be "sleeping" and may have nothing to update and do not want animations
 *          or other theme side-effects causing rendering to the window while "asleep".
 *          You can push (and pop) the norender mode to make this work.
 *
 * @remarks If combined with evas_render_dump(), evas_image_cache_flush(), and
 *          evas_font_cache_flush() (and maybe edje_file_cache_flush() and 
 *          edje_collection_cache_flush()), you can minimize memory footprint
 *          significantly while "asleep", and the pausing of rendering ensures that
 *          evas does not re-load data into memory until needed. When rendering is
 *          resumed, data is re-loaded as needed, which may result in some
 *          lag, but does save memory.
 *
 * @param[in] obj The window object
 *
 * @see elm_win_norender_pop()
 * @see elm_win_norender_get()
 * @see elm_win_render()
 */
EAPI void                  elm_win_norender_push(Evas_Object *obj);

/**
 * @brief Pops (decrements) the norender counter on the window.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks Once norender has gone back to @c 0, automatic rendering continues
 *          in the given window. If it is already at @c 0, this has no effect.
 *
 * @param[in] obj The window object
 *
 * @see elm_win_norender_push()
 * @see elm_win_norender_get()
 * @see elm_win_render()
 */
EAPI void                  elm_win_norender_pop(Evas_Object *obj);

/**
 * @brief Returns the number of times norender has been pushed on the window.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The number of times norender has been pushed
 *
 * @see elm_win_norender_push()
 * @see elm_win_norender_pop()
 * @see elm_win_render()
 */
EAPI int                   elm_win_norender_get(Evas_Object *obj);

/**
 * @brief Manually asks evas to render the window immediately.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks You should NEVER call this unless you really know what you are doing and
 *          why. Never call this unless you are asking for performance degredation
 *          and possibly weird behavior. Windows gets automatically rendered when the
 *          application goes idle so there is never a need to call this UNLESS you
 *          have enabled the "norender" mode.
 *
 * @param[in] obj The window object
 *
 * @see elm_win_norender_push()
 * @see elm_win_norender_pop()
 * @see elm_win_norender_get()
 */
EAPI void                  elm_win_render(Evas_Object *obj);

/**
 * @brief Sets the rotation of the window.
 *
 * @since_tizen 2.3
 *
 * @remarks Most engines only work with multiples of @c 90.
 *
 * @remarks This function is used to set the orientation of the window @a obj to
 *          match that of the screen. The window itself is resized to adjust
 *          to the new geometry of its contents. If you want to keep the window size,
 *
 * @param[in] obj The window object
 * @param[in] rotation The rotation of the window, in degrees (0-360),
 *                 counter-clockwise
 *
 * @see elm_win_rotation_with_resize_set()
 */
EAPI void                  elm_win_rotation_set(Evas_Object *obj, int rotation);

/**
 * @brief Rotates the window and resizes it.
 *
 * @since_tizen 2.3
 *
 * @remarks Like elm_win_rotation_set(), but it also resizes the window's contents so
 *          that they fit inside the current window geometry.
 *
 * @param[in] obj The window object
 * @param[in] rotation The rotation of the window, in degrees (0-360),
 *                 counter-clockwise
 */
EAPI void                  elm_win_rotation_with_resize_set(Evas_Object *obj, int rotation);

/**
 * @brief Gets the rotation of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The rotation of the window in degrees (0-360)
 *
 * @see elm_win_rotation_set()
 * @see elm_win_rotation_with_resize_set()
 */
EAPI int                   elm_win_rotation_get(const Evas_Object *obj);

/**
 * @brief Sets the sticky state of the window.
 *
 * @since_tizen 2.3
 *
 * @remarks Hints the Windows Manager that the window in @a obj should be left fixed
 *          at its position even when the virtual desktop that it's on moves or changes.
 *
 * @param[in] obj The window object
 * @param[in] sticky If @c true the window's sticky state is enabled,
 *               otherwise @c false
 */
EAPI void                  elm_win_sticky_set(Evas_Object *obj, Eina_Bool sticky);

/**
 * @brief Gets the sticky state of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return If @c true the window's sticky state is enabled,
 *         otherwise @c false
 *
 * @see elm_win_sticky_set()
 */
EAPI Eina_Bool             elm_win_sticky_get(const Evas_Object *obj);

/**
 * @brief Sets whether this window is an illume conformant window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] conformant The conformant flag (@c 1 = conformant, @c 0 = non-conformant)
 */
EAPI void                  elm_win_conformant_set(Evas_Object *obj, Eina_Bool conformant);

/**
 * @brief Gets whether this window is an illume conformant window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The boolean value that indicates whether this window is illume conformant
 */
EAPI Eina_Bool             elm_win_conformant_get(const Evas_Object *obj);

/**
 * @brief Sets a window to be an illume quickpanel window.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, window objects are not quickpanel windows.
 *
 * @param[in] obj The window object
 * @param[in] quickpanel The quickpanel flag (@c 1 = quickpanel, @c 0 = normal window)
 */
EAPI void                  elm_win_quickpanel_set(Evas_Object *obj, Eina_Bool quickpanel);

/**
 * @brief Gets whether this window is a quickpanel.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return A boolean value that indicates whether this window is a quickpanel
 */
EAPI Eina_Bool             elm_win_quickpanel_get(const Evas_Object *obj);

/**
 * @brief Sets the major priority of a quickpanel window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] priority The major priority for this quickpanel
 */
EAPI void                  elm_win_quickpanel_priority_major_set(Evas_Object *obj, int priority);

/**
 * @brief Gets the major priority of a quickpanel window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The major priority of this quickpanel
 */
EAPI int                   elm_win_quickpanel_priority_major_get(const Evas_Object *obj);

/**
 * @brief Sets the minor priority of a quickpanel window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] priority The minor priority for this quickpanel
 */
EAPI void                  elm_win_quickpanel_priority_minor_set(Evas_Object *obj, int priority);

/**
 * @brief Gets the minor priority of a quickpanel window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The minor priority of this quickpanel
 */
EAPI int                   elm_win_quickpanel_priority_minor_get(const Evas_Object *obj);

/**
 * @brief Sets the zone in which this quickpanel should appear.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] zone The requested zone for this quickpanel
 */
EAPI void                  elm_win_quickpanel_zone_set(Evas_Object *obj, int zone);

/**
 * @brief Gets the zone in which this should appear.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The requested zone for this quickpanel
 */
EAPI int                   elm_win_quickpanel_zone_get(const Evas_Object *obj);

/**
 * @brief Sets the window to be skipped by keyboard focus.
 *
 * @details This sets the window to be skipped by normal keyboard input. This means
 *          the Windows Manager is asked to not focus this window as well as omit
 *          it from things like the taskbar, pager, "alt-tab" list, etc.
 *
 * @since_tizen 2.3
 *
 * @remarks Call this and enable it on a window BEFORE you show it for the first time,
 *          otherwise it may have no effect.
 *
 * @remarks Use this for windows that have only output information or might only be
 *          interacted with by a mouse or fingers, and never for typing input.
 *          Be careful that this may have side-effects like making the window
 *          non-accessible in some cases unless the window is specially handled. Use
 *          this with care.
 *
 * @param[in] obj The window object
 * @param[in] skip The skip flag state (@c EINA_TRUE if it is to be skipped)
 */
EAPI void                  elm_win_prop_focus_skip_set(Evas_Object *obj, Eina_Bool skip);

/**
 * @brief Sends a command to the windowing environment.
 *
 * @since_tizen 2.3
 *
 * @remarks This is intended to work in touchscreen or small screen device
 *          environments where there is a more simplistic window management policy in
 *          place. This uses the window object indicated to select which part of the
 *          environment to control (the part that this window lives in), and provides
 *          a command and an optional parameter structure (use @c NULL for this if not
 *          needed).
 *
 * @param[in] obj The window object that lives in the environment to control
 * @param[in] command The command to send
 * @param[in] params The optional parameters for the command
 */
EAPI void                  elm_win_illume_command_send(Evas_Object *obj, Elm_Illume_Command command, void *params);

/**
 * @brief Gets the inlined image object handle.
 *
 * @since_tizen 2.3
 *
 * @remarks When you create a window with elm_win_add() of type @c ELM_WIN_INLINED_IMAGE,
 *          then the window is in fact an evas image object inlined in the parent
 *          canvas. You can get this object (be careful to not manipulate it as it
 *          is under the control of elementary), and use it to do things like get pixel
 *          data, save the image to a file, etc.
 *
 * @param[in] obj The window object from which to get the inlined image
 * @return The inlined image object, otherwise @c NULL if none exist
 */
EAPI Evas_Object          *elm_win_inlined_image_object_get(Evas_Object *obj);

/**
 * @brief Determines whether a window has focus.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window to query
 * @return @c EINA_TRUE if the window exists and has focus, 
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool             elm_win_focus_get(const Evas_Object *obj);

/**
 * @brief Limits the maximum width and height of a window to the width and height of its screen.
 *
 * @since_tizen 2.3
 *
 * @remarks When @a constrain is @c true, @a obj never resizes to a size larger than the screen.
 * @param[in] obj The window object
 * @param[in] constrain @c EINA_TRUE to restrict the window's maximum size, 
 *                  otherwise @c EINA_FALSE to disable restriction
 */
EAPI void                  elm_win_screen_constrain_set(Evas_Object *obj, Eina_Bool constrain);

/**
 * @brief Retrieves the constraints on the maximum width and height of a window relative to the width and height of its screen.
 *
 * @since_tizen 2.3
 *
 * @remarks When this function returns @c true, @a obj never resizes to a size larger than the screen.
 * @param[in] obj The window object
 * @return @c EINA_TRUE to restrict the window's maximum size,
 *         otherwise @c EINA_FALSE to disable restriction
 */
EAPI Eina_Bool             elm_win_screen_constrain_get(Evas_Object *obj);

/**
 * @brief Gets the screen geometry details for the screen that a window is on.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window to query
 * @param[out] x The coordinate at which to return the horizontal offset value \n
 *          This may be @c NULL.
 * @param[out] y The coordinate at which to return the vertical offset value \n
 *          This may be @c NULL.
 * @param[out] w The coordinate at which to return the width value \n
 *          This may be @c NULL.
 * @param[out] h The coordinate at which to return the height value \n
 *          This may be @c NULL.
 */
EAPI void                  elm_win_screen_size_get(const Evas_Object *obj, int *x, int *y, int *w, int *h);

/**
 * @brief Gets the screen dpi for the screen that a window is on.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window to query
 * @param[out] xdpi The pointer to the value that stores the returned horizontal dpi \n
 *             This may be @c NULL.
 * @param[out] ydpi The pointer to the value that stores the returned vertical dpi \n
 *             This may be @c NULL.
 */
EAPI void                  elm_win_screen_dpi_get(const Evas_Object *obj, int *xdpi, int *ydpi);

/**
 * @brief Sets the enabled status for the focus highlight in a window.
 *
 * @details This function enables or disables the focus highlight only for the
 *          given window, regardless of the global setting for it.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window for which to enable highlight
 * @param[in] enabled The enabled value for the highlight
 */
EAPI void                  elm_win_focus_highlight_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Gets the enabled value of the focus highlight for this window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window in which to check if the focus highlight is enabled
 *
 * @return @c EINA_TRUE if enabled,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool             elm_win_focus_highlight_enabled_get(const Evas_Object *obj);

/**
 * @brief Sets the style for the focus highlight on this window.
 *
 * @details This sets the style to use for theming the highlight of focused objects on
 *          the given window. If @a style is @c NULL, the default value is used.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window for which to set the style
 * @param[in] style The style to set
 */
EAPI void                  elm_win_focus_highlight_style_set(Evas_Object *obj, const char *style);

/**
 * @brief Gets the style set for the focus highlight object.
 *
 * @details This gets the style set for this windows highlight object, otherwise @c NULL if no style
 *          is set.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window from which to retrieve the highlight style
 *
 * @return The style set, otherwise @c NULL if no style is set \n
 *         The default value is used in that case.
 */
EAPI const char           *elm_win_focus_highlight_style_get(const Evas_Object *obj);

/**
 * @brief Sets the keyboard mode of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] mode The mode to set, one from #Elm_Win_Keyboard_Mode
 */
EAPI void                  elm_win_keyboard_mode_set(Evas_Object *obj, Elm_Win_Keyboard_Mode mode);

/**
 * @brief Gets the keyboard mode of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The mode, one from #Elm_Win_Keyboard_Mode
 */
EAPI Elm_Win_Keyboard_Mode elm_win_keyboard_mode_get(const Evas_Object *obj);

/**
 * @brief Sets whether the window is a keyboard.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] is_keyboard If @c true the window is a virtual keyboard,
 *                    otherwise @c false
 */
EAPI void                  elm_win_keyboard_win_set(Evas_Object *obj, Eina_Bool is_keyboard);

/**
 * @brief Gets whether the window is a keyboard.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is a virtual keyboard,
 *         otherwise @c false
 */
EAPI Eina_Bool             elm_win_keyboard_win_get(const Evas_Object *obj);

/**
 * @brief Sets the indicator mode of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] mode The mode to set, one from #Elm_Win_Indicator_Mode
 */
EAPI void                  elm_win_indicator_mode_set(Evas_Object *obj, Elm_Win_Indicator_Mode mode);

/**
 * @brief Gets the indicator mode of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The mode, one from #Elm_Win_Indicator_Mode
 */
EAPI Elm_Win_Indicator_Mode elm_win_indicator_mode_get(const Evas_Object *obj);

/**
 * @brief Sets the indicator opacity mode of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] mode The mode to set, one from #Elm_Win_Indicator_Opacity_Mode
 */
EAPI void                  elm_win_indicator_opacity_set(Evas_Object *obj, Elm_Win_Indicator_Opacity_Mode mode);

/**
 * @brief Gets the indicator opacity mode of the window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The mode, one from #Elm_Win_Indicator_Opacity_Mode
 */
EAPI Elm_Win_Indicator_Opacity_Mode elm_win_indicator_opacity_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sets the indicator type mode of the window.
 *
 * @param obj The window object
 * @param mode The mode to set, one from #Elm_Win_Indicator_Type_Mode
 */
EAPI void                  elm_win_indicator_type_set(Evas_Object *obj, Elm_Win_Indicator_Type_Mode mode);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the indicator type mode of the window.
 *
 * @param obj The window object
 * @return The mode, one from #Elm_Win_Indicator_Type_Mode
 */
EAPI Elm_Win_Indicator_Type_Mode elm_win_indicator_type_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Enable the indicator fixed style of the window.
 *
 * @since_tizen 2.3
 *
 * @remarks It's disabled by default.
 *
 * @param[in] obj object to enable the fixed style on
 * @param[in] enabled enabled state
 */
EAPI void                 elm_win_indicator_fixed_style_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Get the indicator fixed style enabled state.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The indicator fixed style enabled state
 */
EAPI Eina_Bool            elm_win_indicator_fixed_style_get(const Evas_Object *obj);

/**
 * @brief Get the screen position of a window.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[out] x The integer that stores the x coordinate
 * @param[out] y The integer that stores the y coordinate
 */
EAPI void                  elm_win_screen_position_get(const Evas_Object *obj, int *x, int *y);

/**
 * @brief Creates a socket to provide the service for a Plug widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] svcname The name of the service to be advertised \n
 *                Ensure that it is unique (when combined with @a svcnum), otherwise the creation may fail.
 * @param[in] svcnum A number (any value, @c 0 being the common default) to differentiate multiple instances of services with the same name
 * @param[in] svcsys If @c true it specifies to create a system-wide service that all users can connect to, 
 *               otherwise @c false if the service is private to the user ID that created the service
 * @return The boolean value that indicates whether the socket creation is successful
 */
EAPI Eina_Bool             elm_win_socket_listen(Evas_Object *obj, const char *svcname, int svcnum, Eina_Bool svcsys);

/* X specific calls - won't work on non-x engines (return 0) */
/**
 * @internal
 * @remarks X11 only feature
 *
 * @brief Gets the Ecore_X_Window of an Evas_Object.
 *
 * @param obj The object
 *
 * @return The Ecore_X_Window of @a obj
 */
EAPI Ecore_X_Window elm_win_xwindow_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Wayland only feature
 *
 * @brief Gets the Ecore_Wl_Window of an Evas_Object.
 *
 * @remarks Wayland specific call - returns NULL on non-Wayland engines
 *
 * @param obj The object
 * @return The Ecore_Wl_Window of @a obj
 */
EAPI Ecore_Wl_Window *elm_win_wl_window_get(const Evas_Object *obj);

/**
 * @typedef Elm_Win_Trap
 *
 * @brief Trap can be set with elm_win_trap_set() and intercepts the
 *        calls to internal ecore_evas with the same name and parameters. If
 *        there is a trap and it returns @c EINA_TRUE then the call is
 *        allowed, otherwise it is ignored.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 */
typedef struct _Elm_Win_Trap Elm_Win_Trap;
struct _Elm_Win_Trap
{
#define ELM_WIN_TRAP_VERSION (1UL)
   unsigned long version;
   void *(*add)(Evas_Object *o); /**< Object is just added. The returned pointer is handled to every other trap call */
   void (*del)(void *data, Evas_Object *o); /**< Object is deleted */
   Eina_Bool (*hide)(void *data, Evas_Object *o);
   Eina_Bool (*show)(void *data, Evas_Object *o);
   Eina_Bool (*move)(void *data, Evas_Object *o, int x, int y);
   Eina_Bool (*resize)(void *data, Evas_Object *o, int w, int h);
   Eina_Bool (*center)(void *data, Evas_Object *o); /* not in ecore_evas, but nice to trap */
   Eina_Bool (*lower)(void *data, Evas_Object *o);
   Eina_Bool (*raise)(void *data, Evas_Object *o);
   Eina_Bool (*activate)(void *data, Evas_Object *o);
   Eina_Bool (*alpha_set)(void *data, Evas_Object *o, Eina_Bool alpha);
   Eina_Bool (*aspect_set)(void *data, Evas_Object *o, double aspect);
   Eina_Bool (*avoid_damage_set)(void *data, Evas_Object *o, Ecore_Evas_Avoid_Damage_Type on);
   Eina_Bool (*borderless_set)(void *data, Evas_Object *o, Eina_Bool on);
   Eina_Bool (*demand_attention_set)(void *data, Evas_Object *o, Eina_Bool on);
   Eina_Bool (*focus_skip_set)(void *data, Evas_Object *o, Eina_Bool skip);
   Eina_Bool (*fullscreen_set)(void *data, Evas_Object *o, Eina_Bool on);
   Eina_Bool (*iconified_set)(void *data, Evas_Object *o, Eina_Bool on);
   Eina_Bool (*layer_set)(void *data, Evas_Object *o, int layer);
   Eina_Bool (*manual_render_set)(void *data, Evas_Object *o, Eina_Bool manual_render);
   Eina_Bool (*maximized_set)(void *data, Evas_Object *o, Eina_Bool on);
   Eina_Bool (*modal_set)(void *data, Evas_Object *o, Eina_Bool on);
   Eina_Bool (*name_class_set)(void *data, Evas_Object *o, const char *n, const char *c);
   Eina_Bool (*object_cursor_set)(void *data, Evas_Object *o, Evas_Object *obj, int layer, int hot_x, int hot_y);
   Eina_Bool (*override_set)(void *data, Evas_Object *o, Eina_Bool on);
   Eina_Bool (*rotation_set)(void *data, Evas_Object *o, int rot);
   Eina_Bool (*rotation_with_resize_set)(void *data, Evas_Object *o, int rot);
   Eina_Bool (*shaped_set)(void *data, Evas_Object *o, Eina_Bool shaped);
   Eina_Bool (*size_base_set)(void *data, Evas_Object *o, int w, int h);
   Eina_Bool (*size_step_set)(void *data, Evas_Object *o, int w, int h);
   Eina_Bool (*size_min_set)(void *data, Evas_Object *o, int w, int h);
   Eina_Bool (*size_max_set)(void *data, Evas_Object *o, int w, int h);
   Eina_Bool (*sticky_set)(void *data, Evas_Object *o, Eina_Bool sticky);
   Eina_Bool (*title_set)(void *data, Evas_Object *o, const char *t);
   Eina_Bool (*urgent_set)(void *data, Evas_Object *o, Eina_Bool urgent);
   Eina_Bool (*withdrawn_set)(void *data, Evas_Object *o, Eina_Bool withdrawn);
};

/**
 * @brief Sets the trap to be used for internal @c Ecore_Evas management.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks This is an advanced feature that you should avoid using.
 *
 * @param[in] trap The trap to be used, otherwise @c NULL to remove traps \n
 *             The pointer is not modified or copied, it is kept alive.
 * @return @c EINA_TRUE on success, 
 *         otherwise @c EINA_FALSE if there is a
 *         problem, such as an invalid version number
 */
EAPI Eina_Bool elm_win_trap_set(const Elm_Win_Trap *trap);

/**
 * @brief Sets the floating mode of a window.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] floating If @c true the window is in the floating mode,
 *                 otherwise @c false
 *
 * @see elm_win_floating_mode_get()
 */
EAPI void                  elm_win_floating_mode_set(Evas_Object *obj, Eina_Bool floating);

/**
 * @brief Gets the floating mode of a window.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the window is in the floating mode,
 *         otherwise @c false
 *
 * @see elm_win_floating_mode_set()
 */
EAPI Eina_Bool             elm_win_floating_mode_get(const Evas_Object *obj);

/**
 * @brief Queries whether Windows Manager supports window rotation.
 *
 * @since 1.9
 *
 * @since_tizen 2.3
 *
 * @remarks The Windows Manager rotation allows the WM to control the rotation of application windows.
 *          It is designed to support synchronized rotation for multiple application windows at the same time.
 *
 * @param[in] obj The window object
 * @return @c EINA_TRUE if the Windows Manager rotation is supported, 
 *         otherwise @c EINA_FALSE
 *
 * @see elm_win_wm_rotation_supported_get()
 * @see elm_win_wm_rotation_preferred_rotation_set()
 * @see elm_win_wm_rotation_preferred_rotation_get()
 * @see elm_win_wm_rotation_available_rotations_set()
 * @see elm_win_wm_rotation_available_rotations_get()
 * @see elm_win_wm_rotation_manual_rotation_done_set()
 * @see elm_win_wm_rotation_manual_rotation_done_get()
 * @see elm_win_wm_rotation_manual_rotation_done()
 */
EAPI Eina_Bool             elm_win_wm_rotation_supported_get(const Evas_Object *obj);

/**
 * @brief Set the preferred rotation value.
 *
 * @details This function is used to set the orientation of window @p obj to spicific angle fixed.
 *
 * @since 1.9
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] rotation The preferred rotation of the window in degrees (0-360),
 *        counter-clockwise.
 *
 * @see elm_win_wm_rotation_preferred_rotation_get()
 */
EAPI void                  elm_win_wm_rotation_preferred_rotation_set(Evas_Object *obj, const int rotation);

/**
 * @brief Gets the preferred rotation value.
 *
 * @details This function is used to get the preferred rotation value.
 *
 * @since 1.9
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return The preferred rotation of the window, in degrees (0-360),
 *         counter-clockwise
 *
 * @see elm_win_wm_rotation_preferred_rotation_set()
 */
EAPI int                   elm_win_wm_rotation_preferred_rotation_get(const Evas_Object *obj);

/**
 * @brief Sets the array of available window rotations.
 *
 * @details This function is used to set the available rotations to give hints to WM.
 *          WM refers these hints and sets the orientation of the window properly.
 *
 * @since 1.9
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] rotations The array of rotation values
 * @param[in] count The number of arrays of rotations
 *
 * @see elm_win_wm_rotation_available_rotations_get()
 */
EAPI void                  elm_win_wm_rotation_available_rotations_set(Evas_Object *obj, const int *rotations, unsigned int count);

/**
 * @brief Gets the array of available window rotations.
 *
 * @details This function is used to get the available rotations.
 *
 * @since 1.9
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[out] rotations The array of rotation values
 * @param[out] count The number of arrays of rotations
 * @return #EINA_TRUE if succeed, otherwise #EINA_FALSE
 *
 * @see elm_win_wm_rotation_available_rotations_set()
 */
EAPI Eina_Bool             elm_win_wm_rotation_available_rotations_get(const Evas_Object *obj, int **rotations, unsigned int *count);

/**
 * @brief Sets the manual rotation done mode of the window.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @param[in] set If @c true, the Windows Manager does not rotate the window until
 *            the rotation done event is received by elm_win_wm_rotation_manual_rotation_done,
 *            otherwise @c false if the manual rotation mode is disabled
 *
 * @see elm_win_wm_rotation_manual_rotation_done_get()
 * @see elm_win_wm_rotation_manual_rotation_done()
 */
EAPI void                  elm_win_wm_rotation_manual_rotation_done_set(Evas_Object *obj, Eina_Bool set);

/**
 * @brief Gets the manual rotation done mode of the window.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 * @return @c true if the manual rotation done mode is enabled,
 *         otherwise @c false
 *
 * @see elm_win_wm_rotation_manual_rotation_done_set()
 * @see elm_win_wm_rotation_manual_rotation_done()
 */
EAPI Eina_Bool             elm_win_wm_rotation_manual_rotation_done_get(const Evas_Object *obj);

/**
 * @brief Sets the rotation finish manually.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The window object
 *
 * @see elm_win_wm_rotation_manual_rotation_done_set()
 * @see elm_win_wm_rotation_manual_rotation_done_get()
 */
EAPI void                  elm_win_wm_rotation_manual_rotation_done(Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sets the desktop layout support state.
 *
 * @param obj The window object
 * @param support If @c true, the Windows Manager provides the desktop layout mode,
 *                otherwise @c false if the Windows Manager sets the window to the normal mode
 */
EAPI void                  elm_win_wm_desktop_layout_support_set(Evas_Object *obj, const Eina_Bool support);

/**
 * @internal
 * @remarks Tizen only feature 2013.12.07
 *
 * @brief For making unfocusable elm_window to be focusable.
 */
EAPI void                  elm_win_focus_allow_set(Evas_Object *obj, Eina_Bool enable);

/**
 * @internal
 * @remarks Tizen only feature 2013.12.07
 *
 * @brief For making unfocusable elm_window to be focusable.
 */
EAPI Eina_Bool             elm_win_focus_allow_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the list of supported auxiliary hint strings.
 *
 * @since 1.8
 *
 * @remarks Do not change the returned list of contents. Auxiliary hint
 *          strings are internal and should be considered constants, do not free or
 *          modify them.
 * @remarks Support for this depends on the underlying windowing system.
 *
 * @remarks The window auxiliary hint is the value that is used to decide which action should
 *          be made available to the user by the Windows Manager. If you want to set a specific hint
 *          to your window, then you should check whether it exists in the supported auxiliary
 *          hints that are registered in the root window by the Windows Manager. Once you have added
 *          an auxiliary hint, you can get a new ID which is used to change the value and delete the hint.
 *          The Windows Manager sends a response message to the application on receiving the auxiliary
 *          hint change event.
 *
 * @param obj The window object
 * @return The list of supported auxiliary hint strings
 */
EAPI const Eina_List      *elm_win_aux_hints_supported_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Creates an auxiliary hint of the window.
 *
 * @since 1.8
 *
 * @remarks Support for this depends on the underlying windowing system.
 *
 * @param obj The window object
 * @param hint The auxiliary hint string
 * @param val The value string
 * @return The ID of the created auxiliary hint,
 *         otherwise @c -1 on failure
 */
EAPI int                   elm_win_aux_hint_add(Evas_Object *obj, const char *hint, const char *val);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Deletes an auxiliary hint of the window.
 *
 * @since 1.8
 *
 * @remarks Support for this depends on the underlying windowing system.
 * @param obj The window object
 * @param id The ID of the auxiliary hint
 * @return @c EINA_TRUE if no error occurs, 
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool             elm_win_aux_hint_del(Evas_Object *obj, const int id);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Changes a value of the auxiliary hint.
 *
 * @since 1.8
 *
 * @remarks Support for this depends on the underlying windowing system.
 * @param obj The window object
 * @param id The auxiliary hint ID
 * @param val The value string to be set
 * @return @c EINA_TRUE if no error occurs,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool             elm_win_aux_hint_val_set(Evas_Object *obj, const int id, const char *val);

/**
 * @}
 */
