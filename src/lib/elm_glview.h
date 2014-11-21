/**
 * @defgroup GLView GLView
 * @ingroup elm_widget_group
 *
 * @image html glview_inheritance_tree.png
 * @image latex glview_inheritance_tree.eps
 *
 * @brief A GLView widget allows for simple GL rendering in the elementary
 *        environment.
 *
 * GLView hides all the complicated evas_gl details so that the user only
 * has to deal with registering a few callback functions for rendering
 * to a surface using OpenGL APIs.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref GLView :
 * @li @c "language,changed" - The program's language is changed
 *
 * Please refer to the page @ref elm_opengl_page for a quick introduction
 * about OpenGL with EFL.
 *
 * @{
 */

typedef void (*Elm_GLView_Func_Cb)(Evas_Object *obj);

/**
 * @brief Selects the target surface properties
 *
 * An OR combination of @c Elm_GLView_Mode values should be passed to
 * @ref elm_glview_mode_set when setting up a GL widget. These flags will
 * specify the properties of the rendering target surface; in particular,
 * the mode can request the surface to support alpha, depth and stencil buffers.
 *
 * @note @c ELM_GLVIEW_CLIENT_SIDE_ROTATION is a special value that indicates
 *       to EFL that the application will handle the view rotation when the
 *       device is rotated. This is needed only when the application requests
 *       direct rendering. Please refer to @ref Evas_GL
 *       for more information about direct rendering.
 *
 * @see elm_glview_mode_set
 * @see @ref elm_opengl_page
 *
 * @since_tizen 2.3
 */
typedef enum _Elm_GLView_Mode
{
   ELM_GLVIEW_NONE    = 0,
   // 0x1 is reserved for future use
   ELM_GLVIEW_ALPHA   = (1<<1), /**< Alpha channel enabled rendering mode */
   ELM_GLVIEW_DEPTH   = (1<<2), /**< Depth buffer enabled rendering mode (24 bits by default) */
   ELM_GLVIEW_STENCIL = (1<<3), /**< Stencil buffer enabled rendering mode (8 bits by default) */
   ELM_GLVIEW_DIRECT  = (1<<4), /**< Request direct rendering, unless there must be a fallback */
   ELM_GLVIEW_CLIENT_SIDE_ROTATION = (1<<5), /**< Client will handle GL view rotation if direct rendering is enabled */
   // Depth buffer sizes (3 bits)
   ELM_GLVIEW_DEPTH_8  = ELM_GLVIEW_DEPTH | (1 << 6), /**< Request min. 8 bits for the depth buffer */
   ELM_GLVIEW_DEPTH_16 = ELM_GLVIEW_DEPTH | (2 << 6), /**< Request min. 16 bits for the depth buffer */
   ELM_GLVIEW_DEPTH_24 = ELM_GLVIEW_DEPTH | (3 << 6), /**< Request min. 24 bits for the depth buffer (default) */
   ELM_GLVIEW_DEPTH_32 = ELM_GLVIEW_DEPTH | (4 << 6), /**< Request min. 32 bits for the depth buffer */
   // Stencil buffer sizes (3 bits)
   ELM_GLVIEW_STENCIL_1  = ELM_GLVIEW_STENCIL | (1 << 9), /**< Request min. 1 bits for the stencil buffer */
   ELM_GLVIEW_STENCIL_2  = ELM_GLVIEW_STENCIL | (2 << 9), /**< Request min. 2 bits for the stencil buffer */
   ELM_GLVIEW_STENCIL_4  = ELM_GLVIEW_STENCIL | (3 << 9), /**< Request min. 4 bits for the stencil buffer */
   ELM_GLVIEW_STENCIL_8  = ELM_GLVIEW_STENCIL | (4 << 9), /**< Request min. 8 bits for the stencil buffer (default) */
   ELM_GLVIEW_STENCIL_16 = ELM_GLVIEW_STENCIL | (5 << 9), /**< Request min. 16 bits for the stencil buffer */
   // MSAA params (2 bits)
   ELM_GLVIEW_MULTISAMPLE_LOW  = (1 << 12), /**< MSAA with minimum number of samples */
   ELM_GLVIEW_MULTISAMPLE_MED  = (2 << 12), /**< MSAA with half the number of maximum samples */
   ELM_GLVIEW_MULTISAMPLE_HIGH = (3 << 12)  /**< MSAA with maximum number of samples */
} Elm_GLView_Mode;

/**
 * @brief Enumeration that defines a policy for the GLView resizing.
 *
 * @remarks The resizing policy tells GLView what to do with the underlying
 *          surface when resize happens. @c ELM_GLVIEW_RESIZE_POLICY_RECREATE
 *          destroys the current surface and recreates the surface to the
 *          new size. @c ELM_GLVIEW_RESIZE_POLICY_SCALE instead keeps the
 *          current surface but only displays the result at the desired size
 *          scaled.
 *
 * @remarks Default is @c ELM_GLVIEW_RESIZE_POLICY_RECREATE
 *
 * @since_tizen 2.3
 */
typedef enum
{
   ELM_GLVIEW_RESIZE_POLICY_RECREATE = 1, /**< Resize the internal surface along with the image */
   ELM_GLVIEW_RESIZE_POLICY_SCALE    = 2  /**< Only resize the internal image and not the surface */
} Elm_GLView_Resize_Policy;

/**
 * @brief Enumeration that defines a policy for gl rendering.
 *
 * @remarks The rendering policy tells GLView where to run the gl rendering code.
 *          @c ELM_GLVIEW_RENDER_POLICY_ON_DEMAND tells GLView to call the rendering
 *          calls on demand, which means that the rendering code gets called
 *          only when it is visible.
 *
 * @remarks Default is @c ELM_GLVIEW_RENDER_POLICY_ON_DEMAND
 *
 * @since_tizen 2.3
 */
typedef enum
{
   ELM_GLVIEW_RENDER_POLICY_ON_DEMAND = 1, /**< Render only when there is a need for redrawing */
   ELM_GLVIEW_RENDER_POLICY_ALWAYS    = 2  /**< Render always even when it is not visible */
} Elm_GLView_Render_Policy;

/**
 * @brief Adds a new GLView to the parent.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, GLView creates an OpenGL-ES 2.0 context, so only
 *          OpenGL-ES 2.0 APIs can be used (and supported extensions).
 *          Please use @ref elm_glview_version_add instead to create OpenGL-ES 1.1
 *          contexts.
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @see elm_glview_version_add
 */
EAPI Evas_Object *elm_glview_add(Evas_Object *parent);

/**
 * @brief Adds a new GLView to the parent, given an OpenGL-ES context version number.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @param[in] version Requested GL ES version number (default is 2.x, 1.x may also be supported)
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @see elm_glview_add
 */
EAPI Evas_Object *elm_glview_version_add(Evas_Object *parent, Evas_GL_Context_Version version);

/**
 * @brief Sets the size of the GLView.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The GLView object
 * @param[in] w The width of the GLView object
 * @param[in] h The height of the GLView object
 */
EAPI void         elm_glview_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h);

/**
 * @brief Gets the size of the GLView.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that this function returns the actual image size of the
 *          GLView. This means that when the scale policy is set to
 *          @c ELM_GLVIEW_RESIZE_POLICY_SCALE, it returns the non-scaled
 *          size.
 *
 * @param[in] obj The GLView object
 * @param[out] w The width of the GLView object
 * @param[out] h The height of the GLView object
 */
EAPI void         elm_glview_size_get(const Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Gets the GL API struct for GL rendering.
 *
 * @details Elementary GLView uses Evas GL internally to create a rendering context
 *          and target surfaces. Please refer to the Evas GL documentation for more
 *          information about this GL API structure.
 *
 * @note When using an OpenGL-ES 1.1 context, @c elm_glview_gl_api_get will
 *       return a GL-ES 1.1 API.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The GLView object
 * @return The API object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_GL_API *elm_glview_gl_api_get(const Evas_Object *obj);

/**
 * @brief Sets the mode of the GLView. Supports alpha, depth and stencil.
 *
 * @since_tizen 2.3
 *
 * @remarks If direct rendering is enabled, Evas GL will render directly to the back
 *          buffer of the window, unless the window has been rotated. If the window
 *          is rotated and the flag DIRECT is set, the flag CLIENT_SIDE_ROTATION
 *          can be used to avoid falling back to a framebuffer.
 *
 * @remarks Here are some conditions that will disable direct rendering and force a
 *          fallback to indirect rendering in a framebuffer:
 *
 * @li if the object's color is not (255,255,255,255),
 * @li if the object has an evas map,
 * @li if the object size is different from the viewport, (RESIZE_POLICY_SCALE)
 * @li if the window is rotated and CLIENT_SIDE_ROTATION is not set,
 * @li if the GLView policy is set to ALWAYS render.
 *
 * @param[in] obj The GLView object
 * @param[in] mode The mode Options OR'ed enabling Alpha, Depth, Stencil, Direct and Client Side Rotation
 * @return @c true if set properly,
 *         otherwise @c false
 */
EAPI Eina_Bool    elm_glview_mode_set(Evas_Object *obj, Elm_GLView_Mode mode);

/**
 * @brief Sets the resize policy for the GLView object.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, the resize policy is set to @c ELM_GLVIEW_RESIZE_POLICY_RECREATE.
 *          When resize is called it destroys the previous surface and recreates the
 *          newly specified size. If the policy is set to
 *          @c ELM_GLVIEW_RESIZE_POLICY_SCALE, however, GLView only scales the image
 *          object and not the underlying GL Surface.
 *
 * @param[in] obj The GLView object
 * @param[in] policy The scaling policy
 * @return #EINA_TRUE if succeed, otherwise #EINA_FALSE
 */
EAPI Eina_Bool    elm_glview_resize_policy_set(Evas_Object *obj, Elm_GLView_Resize_Policy policy);

/**
 * @brief Sets the render policy for the GLView object.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, the render policy is set to @c ELM_GLVIEW_RENDER_POLICY_ON_DEMAND.
 *          This policy is set such that during the render loop, GLView is only
 *          redrawn if it needs to be redrawn. (i.e. when it is visible). If the policy
 *          is set to @c ELM_GLVIEWW_RENDER_POLICY_ALWAYS, it redraws regardless of
 *          whether it is visible or needs redrawing.
 *
 * @param[in] obj The GLView object
 * @param[in] policy The render policy
 * @return #EINA_TRUE if succeed, otherwise #EINA_FALSE
 */
EAPI Eina_Bool    elm_glview_render_policy_set(Evas_Object *obj, Elm_GLView_Render_Policy policy);

/**
 * @brief Sets the init function that runs once in the main loop.
 *
 * @since_tizen 2.3
 *
 * @remarks The registered init function gets called once during the render loop.
 *          This function allows GLView to hide all the rendering context/surface
 *          details and have the user just call the GL calls that they desire
 *          for initialization GL calls.
 *
 * @param[in] obj The GLView object
 * @param[in] func The init function to be registered
 */
EAPI void         elm_glview_init_func_set(Evas_Object *obj, Elm_GLView_Func_Cb func);

/**
 * @brief Sets the delete function that runs when the GLView gets deleted.
 *
 * @since_tizen 2.3
 *
 * @remarks The registered del function gets called when GLView object is deleted.
 *          This function allows GLView to hide all the rendering context/surface
 *          details and have the user just call the GL calls that they desire
 *          when delete happens. Called from the main loop.
 *
 * @param[in] obj The GLView object
 * @param[in] func The delete function to be registered
 */
EAPI void         elm_glview_del_func_set(Evas_Object *obj, Elm_GLView_Func_Cb func);

/**
 * @brief Sets the resize function that gets called when resize happens.
 *
 * @since_tizen 2.3
 *
 * @remarks The resize function gets called during the render loop.
 *          This function allows GLView to hide all the rendering context/surface
 *          details and have the user just call the GL calls that they desire
 *          when resize happens.
 *
 * @param[in] obj The GLView object
 * @param[in] func The resize function to be registered
 */
EAPI void         elm_glview_resize_func_set(Evas_Object *obj, Elm_GLView_Func_Cb func);

/**
 * @brief Sets the render function that runs in the main loop.
 *
 * @since_tizen 2.3
 *
 * @remarks The render function gets called in the main loop, but whether it runs
 *          depends on the rendering policy and whether elm_glview_changed_set()
 *          gets called.
 *
 * @param[in] obj The GLView object
 * @param[in] func The render function to be registered
 */
EAPI void         elm_glview_render_func_set(Evas_Object *obj, Elm_GLView_Func_Cb func);

/**
 * @brief Notifies that there have been changes in the GLView.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The GLView object
 */
EAPI void         elm_glview_changed_set(Evas_Object *obj);

/**
 * @brief Get the internal Evas GL attached to this view.
 *
 * @note The returned Evas_GL must not be destroyed as it is still owned
 * by the view. But this pointer can be used then to call all the evas_gl_
 * functions.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The GLView object
 *
 * @return The Evas_GL used by this GLView.
 */
EAPI Evas_GL     *elm_glview_evas_gl_get(Evas_Object *obj);

/**
 * @}
 */
