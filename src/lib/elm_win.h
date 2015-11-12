/**
 * @defgroup Win Win
 * @ingroup Elementary
 *
 * @image html win_inheritance_tree.png
 * @image latex win_inheritance_tree.eps
 *
 * @image html img/widget/win/preview-00.png
 * @image latex img/widget/win/preview-00.eps
 *
 * The window class of Elementary. Contains functions to manipulate
 * windows. The Evas engine used to render the window contents is specified
 * in the system or user elementary config files (whichever is found last),
 * and can be overridden with the ELM_ENGINE environment variable for
 * testing.  Engines that may be supported (depending on Evas and Ecore-Evas
 * compilation setup and modules actually installed at runtime) are (listed
 * in order of best supported and most likely to be complete and work to
 * lowest quality). Note that ELM_ENGINE is really only needed for special
 * cases and debugging. you should normally use ELM_DISPLAY and ELM_ACCEL
 * environment variables, or core elementary config. ELM_DISPLAY can be set to
 * "x11" or "wl" to indicate the target display system (as on Linux systems
 * you may have both display systems available, so this selects which to use).
 * ELM_ACCEL may also be set to indicate if you want accelerations and which
 * kind to use. see elm_config_accel_preference_set(0 for details on this
 * environment variable values.
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
 * @li "ddraw", "software-ddraw", "software_ddraw" (Windows WIN32 rendering via
 * DirectDraw with software)
 * @li "ews" (rendering to EWS - Ecore + Evas Single Process Windowing System)
 * @li "gl-cocoa", "gl_cocoa", "opengl-cocoa", "opengl_cocoa" (OpenGL rendering in Cocoa)
 * @li "wayland_shm" (Wayland client SHM rendering)
 * @li "wayland_egl" (Wayland client OpenGL/EGL rendering)
 * @li "drm" (Linux drm/kms etc. direct display)
 *
 * All engines use a simple string to select the engine to render, EXCEPT
 * the "shot" engine. This actually encodes the output of the virtual
 * screenshot and how long to delay in the engine string. The engine string
 * is encoded in the following way:
 *
 *   "shot:[delay=XX][:][repeat=DDD][:][file=XX]"
 *
 * Where options are separated by a ":" char if more than one option is
 * given, with delay, if provided being the first option and file the last
 * (order is important). The delay specifies how long to wait after the
 * window is shown before doing the virtual "in memory" rendering and then
 * save the output to the file specified by the file option (and then exit).
 * If no delay is given, the default is 0.5 seconds. If no file is given the
 * default output file is "out.png". Repeat option is for continuous
 * capturing screenshots. Repeat range is from 1 to 999 and filename is
 * fixed to "out001.png" Some examples of using the shot engine:
 *
 *   ELM_ENGINE="shot:delay=1.0:repeat=5:file=elm_test.png" elementary_test
 *   ELM_ENGINE="shot:delay=1.0:file=elm_test.png" elementary_test
 *   ELM_ENGINE="shot:file=elm_test2.png" elementary_test
 *   ELM_ENGINE="shot:delay=2.0" elementary_test
 *   ELM_ENGINE="shot:" elementary_test
 *
 * Signals that you can add callbacks for are:
 *
 * @li "delete,request": the user requested to close the window. See
 * elm_win_autodel_set() and elm_win_autohide_set().
 * @li "focus,in": window got focus (deprecated. use "focused" instead.)
 * @li "focus,out": window lost focus (deprecated. use "unfocused" instead.)
 * @li "moved": window that holds the canvas was moved
 * @li "withdrawn": window is still managed normally but removed from view
 * @li "iconified": window is minimized (perhaps into an icon or taskbar)
 * @li "normal": window is in a normal state (not withdrawn or iconified)
 * @li "stick": window has become sticky (shows on all desktops)
 * @li "unstick": window has stopped being sticky
 * @li "fullscreen": window has become fullscreen
 * @li "unfullscreen": window has stopped being fullscreen
 * @li "maximized": window has been maximized
 * @li "unmaximized": window has stopped being maximized
 * @li "ioerr": there has been a low-level I/O error with the display system
 * @li "indicator,prop,changed": an indicator's property has been changed
 * @li "rotation,changed": window rotation has been changed
 * @li "profile,changed": profile of the window has been changed
 * @li "focused" : When the win has received focus. (since 1.8)
 * @li "unfocused" : When the win has lost focus. (since 1.8)
 * @li "theme,changed" - The theme was changed. (since 1.13)
 *
 * Note that calling evas_object_show() after window contents creation is
 * recommended. It will trigger evas_smart_objects_calculate() and some backend
 * calls directly. For example, XMapWindow is called directly during
 * evas_object_show() in X11 engine.
 *
 * Examples:
 * @li @ref win_example_01
 *
 * @{
 */

#include <elm_win_common.h>
#ifdef EFL_EO_API_SUPPORT
#include <elm_win_eo.h>
#endif
#ifndef EFL_NOLEGACY_API_SUPPORT
#include <elm_win_legacy.h>
#endif

/**
 * @}
 */
