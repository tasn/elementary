/**
 * @defgroup Panel Panel
 * @ingroup elm_widget_group
 *
 * @image html panel_inheritance_tree.png
 * @image latex panel_inheritance_tree.eps
 *
 * @brief A panel is an animated side-panel that contains a
 *        sub-object. It can be expanded or contracted by clicking the
 *        button on its edge.
 *
 * Orientations are as follows:
 * @li @c ELM_PANEL_ORIENT_TOP
 * @li @c ELM_PANEL_ORIENT_LEFT
 * @li @c ELM_PANEL_ORIENT_RIGHT
 * @li @c ELM_PANEL_ORIENT_BOTTOM
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for panel objects (@since 1.8).
 *
 * The default content parts of the panel widget that you can use are:
 * @li @c "default" - Content of the panel.
 *
 * This widget emits the following signals, besides the ones sent from
 * @li @c "scroll" - The content has been scrolled (moved).
 *
 * @{
 */


/**
 * @brief Enumeration of Elm Panel Orient type
 */
typedef enum
{
   ELM_PANEL_ORIENT_TOP, /**< Panel (dis)appears from the top */
   ELM_PANEL_ORIENT_BOTTOM, /**< Panel (dis)appears from the bottom */
   ELM_PANEL_ORIENT_LEFT, /**< Panel (dis)appears from the left */
   ELM_PANEL_ORIENT_RIGHT, /**< Panel (dis)appears from the right */
} Elm_Panel_Orient;

/**
 * @typedef Elm_Panel_Scroll_Info
 *
 * @brief The structure type when the panel content is scrolled, if the panel object is scrollable,
 *        this information is passed by the @a event_info argument in the
 *        @c "scroll" smart callback function.
 */
typedef struct _Elm_Panel_Scroll_Info
{
   double rel_x;   /**< Content scrolled position (0.0 ~ 1.0) in the panel */
   double rel_y;   /**< Content scrolled position (0.0 ~ 1.0) in the panel */

} Elm_Panel_Scroll_Info;

/**
 * @brief Adds a panel object.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 *
 * @return The panel object, otherwise @c NULL on failure
 */
EAPI Evas_Object                 *elm_panel_add(Evas_Object *parent);

/**
 * @brief Sets the orientation of the panel.
 *
 * @details This sets the location from where the panel (dis)appears.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panel object
 * @param[in] orient The panel orientation, can be one of the following:
 *               @li ELM_PANEL_ORIENT_TOP
 *               @li ELM_PANEL_ORIENT_LEFT
 *               @li ELM_PANEL_ORIENT_RIGHT
 */
EAPI void                         elm_panel_orient_set(Evas_Object *obj, Elm_Panel_Orient orient);

/**
 * @brief Gets the orientation of the panel.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panel object
 * @return The Elm_Panel_Orient, otherwise @c ELM_PANEL_ORIENT_LEFT on failure
 */
EAPI Elm_Panel_Orient             elm_panel_orient_get(const Evas_Object *obj);

/**
 * @brief Sets the state of the panel.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panel object
 * @param[in] hidden If @c true the panel runs the animation to disappear,
 *               otherwise @c false
 */
EAPI void                         elm_panel_hidden_set(Evas_Object *obj, Eina_Bool hidden);

/**
 * @brief Gets the state of the panel.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panel object
 * @return @c EINA_TRUE if it is the hidden state,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool                    elm_panel_hidden_get(const Evas_Object *obj);

/**
 * @brief Toggles the hidden state of the panel from the code.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The panel object
 */
EAPI void                         elm_panel_toggle(Evas_Object *obj);

/**
 * @remarks Tizen only feature but not internal (will be patched into upstream)
 *
 * @brief Enables or disables scrolling in the panel.
 *
 * @since_tizen 2.3
 *
 * @remarks Normally the panel is not scrollable unless you enable it with this call.
 *
 * @param[in] obj The panel object
 * @param[in] scrollable If @c EINA_TRUE it is scrollable, otherwise @c EINA_FALSE
 */
EAPI void                         elm_panel_scrollable_set(Evas_Object *obj, Eina_Bool scrollable);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the scrollable state of the panel.
 *
 * @remarks Normally the panel is not scrollable. This gets the scrollable state
 *          of the panel.
 *
 * @param obj The panel object
 * @return The scrollable state
 *
 * @see elm_panel_scrollable_set()
 */
EAPI Eina_Bool          elm_panel_scrollable_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Set the size of the content of a scrollable panel
 *
 * @param obj The panel object
 * @param ratio The ratio of the content to the panel. This value ranges from 0 to 1.
 */
EAPI void               elm_panel_scrollable_content_size_set(Evas_Object *obj, double ratio);

/**
 * @}
 */
