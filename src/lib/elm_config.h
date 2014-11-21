/**
 * @defgroup Config Elementary Config
 * @ingroup elm_infra_group
 * @brief Elementary configuration is formed by a set of options bounded to a
 *        given @ref Profile profile, like @ref Theme theme, @ref Fingers
 *        "finger size", etc.
 *
 * These are functions with which one can synchronize
 * the changes made to those values into the configuration storing files, de
 * facto. You most probably don't want to use the functions in this
 * group unless you're writing an elementary configuration manager.
 *
 * @{
 */

/**
 * @brief Saves back Elementary's configuration, so that it persists on
 *        future sessions.
 *
 * @details This function takes effect so do I/O immediately. Use
 *          it when you want to save all the configuration changes at once. The
 *          current configuration set gets saved onto the current profile
 *          configuration file.
 *
 * @since_tizen 2.3
 *
 * @return @c EINA_TRUE if successful, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool elm_config_save(void);

/**
 * @brief Reloads Elementary's configuration, bounded to the current selected
 *        profile.
 *
 * @since_tizen 2.3
 *
 * @remarks It is useful when you want to force reloading of the configuration values for
 *          a profile. If one removes the user custom configuration directories,
 *          it forces a reload with system values instead.
 *
 * @return @c EINA_TRUE if successful, otherwise @c EINA_FALSE
 */
EAPI void      elm_config_reload(void);

/**
 * @brief Flushes all config settings and then applies those settings to all applications
 *        using elementary on the current display.
 *
 * @since_tizen 2.3
 */
EAPI void      elm_config_all_flush(void);

/**
 * @}
 */

/**
 * @defgroup Profile Elementary Profile
 * @ingroup elm_infra_group *
 * @brief Profiles are pre-set options that affect the whole look-and-feel of
 * Elementary-based applications.
 *
 * There are, for example, profiles aimed at desktop computer applications and
 * others aimed at mobile, touchscreen-based ones. You most probably don't want
 * to use the functions in this group unless you're writing an elementary
 * configuration manager.
 *
 * @{
 */

/**
 * @brief Gets Elementary's profile in use.
 *
 * @details This gets the global profile that is applied to all Elementary
 *          applications.
 *
 * @since_tizen 2.3
 *
 * @return The profile name
 */
EAPI const char *elm_config_profile_get(void);

/**
 * @brief Gets an Elementary's profile directory path in the filesystem. One
 *        may want to fetch a system profile dir or a user one (fetched
 *        inside $HOME).
 *
 * @since_tizen 2.3
 *
 * @remarks You must free it using elm_config_profile_dir_free().
 *
 * @param[in] profile The profile name
 * @param[in] is_user The boolean value that indicates whether to lookup for a user profile (@c EINA_TRUE)
 *                or a system one (@c EINA_FALSE)
 * @return The profile's directory path
 */
EAPI const char *elm_config_profile_dir_get(const char *profile, Eina_Bool is_user);

/**
 * @brief Frees an Elementary's profile directory path, as returned by
 *        elm_config_profile_dir_get().
 *
 * @since_tizen 2.3
 *
 * @param[in] p_dir The profile path
 *
 */
EAPI void        elm_config_profile_dir_free(const char *p_dir);

/**
 * @brief Gets an Elementary's list of available profiles.
 *
 * @since_tizen 2.3
 *
 * @remarks One must free this list, after usage, using the function
 *          elm_config_profile_list_free().
 *
 * @return The profiles list \n
 *         List node data are the profile name strings.
 */
EAPI Eina_List  *elm_config_profile_list_get(void);

/**
 * @brief Frees an Elementary's list of available profiles.
 *
 * @since_tizen 2.3
 *
 * @param[in] l The profile list, as returned by elm_config_profile_list_get()
 */
EAPI void        elm_config_profile_list_free(Eina_List *l);

/**
 * @brief Sets an Elementary's profile.
 *
 * @details This sets the global profile that is applied to Elementary
 *          applications. Only the process that the call comes from is
 *          affected.
 *
 * @since_tizen 2.3
 *
 * @param[in] profile The profile name
 */
EAPI void        elm_config_profile_set(const char *profile);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Checks whether the given Elementary's profile exists.
 *
 * @param[in] profile The profile name
 * @return @c EINA_TRUE if the profile exists, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool   elm_config_profile_exists(const char *profile);

/**
 * @}
 */

/**
 * @defgroup Scrolling Elementary Scrolling
 * @ingroup elm_infra_group
 * @brief These are functions that set how scrollable views in Elementary
 *        widgets should behave on user interaction.
 *
 * @{
 */

/**
 * @brief Gets whether scrollers should bounce when they reach their
 *        viewport's edge during a scroll.
 *
 * @since_tizen 2.3
 *
 * @remarks This is the default behavior for touch screens, in general.
 *
 * @return The thumb scroll bouncing state
 */
EAPI Eina_Bool    elm_config_scroll_bounce_enabled_get(void);

/**
 * @brief Sets whether scrollers should bounce when they reach their
 *        viewport's edge during a scroll.
 *
 * @since_tizen 2.3
 *
 * @param[in] enabled The thumb scroll bouncing state
 *
 * @see elm_config_scroll_bounce_enabled_get()
 */
EAPI void         elm_config_scroll_bounce_enabled_set(Eina_Bool enabled);

/**
 * @brief Gets the amount of inertia a scroller imposes during bounce
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll bounce friction
 */
EAPI double       elm_config_scroll_bounce_friction_get(void);

/**
 * @brief Sets the amount of inertia a scroller imposes during bounce
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @param[in] friction The thumb scroll bounce friction
 *
 * @see elm_config_scroll_bounce_friction_get()
 */
EAPI void         elm_config_scroll_bounce_friction_set(double friction);

/**
 * @brief Gets the amount of inertia a <b>paged</b> scroller imposes during
 *        page fitting animations.
 *
 * @since_tizen 2.3
 *
 * @return The page scroll friction
 */
EAPI double       elm_config_scroll_page_scroll_friction_get(void);

/**
 * @brief Sets the amount of inertia a <b>paged</b> scroller imposes during
 *        page fitting animations.
 *
 * @since_tizen 2.3
 *
 * @param[in] friction The page scroll friction
 *
 * @see elm_config_scroll_page_scroll_friction_get()
 */
EAPI void         elm_config_scroll_page_scroll_friction_set(double friction);

/**
 * @brief Gets the amount of inertia a scroller imposes during region bring
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @return The bring in scroll friction
 */
EAPI double       elm_config_scroll_bring_in_scroll_friction_get(void);

/**
 * @brief Sets the amount of inertia a scroller imposes during region bring
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @param[in] friction The bring in scroll friction
 *
 * @see elm_config_scroll_bring_in_scroll_friction_get()
 */
EAPI void         elm_config_scroll_bring_in_scroll_friction_set(double friction);

/**
 * @brief Gets the amount of inertia scrollers impose during animations
 *        triggered by Elementary widgets' zooming API.
 *
 * @since_tizen 2.3
 *
 * @return The zoom friction
 */
EAPI double       elm_config_scroll_zoom_friction_get(void);

/**
 * @brief Sets the amount of inertia scrollers impose during animations
 *        triggered by Elementary widgets' zooming API.
 *
 * @since_tizen 2.3
 *
 * @param[in] friction The zoom friction
 *
 * @see elm_config_scroll_zoom_friction_get()
 */
EAPI void         elm_config_scroll_zoom_friction_set(double friction);

/**
 * @brief Gets whether scrollers should be draggable from any point in their
 *        views.
 *
 * @since_tizen 2.3
 *
 * @remarks This is the default behavior for touch screens, in general.
 * @remarks All other functions namespaced with "thumbscroll" are only
 *          going to have effect if this mode is enabled.
 *
 * @return The thumb scroll state
 */
EAPI Eina_Bool    elm_config_scroll_thumbscroll_enabled_get(void);

/**
 * @brief Sets whether scrollers should be draggable from any point in their
 *        views.
 *
 * @since_tizen 2.3
 *
 * @param[in] enabled The thumb scroll state
 *
 * @see elm_config_scroll_thumbscroll_enabled_get()
 */
EAPI void         elm_config_scroll_thumbscroll_enabled_set(Eina_Bool enabled);

/**
 * @brief Gets the number of pixels one should travel while dragging a
 *        scroller's view to actually trigger scrolling.
 *
 * @since_tizen 2.3
 *
 * @remarks One would use higher values for touch screens, in general, because
 *          of their inherent imprecision.
 *
 * @return The thumb scroll threshold
 */
EAPI unsigned int elm_config_scroll_thumbscroll_threshold_get(void);

/**
 * @brief Sets the number of pixels one should travel while dragging a
 *        scroller's view to actually trigger scrolling.
 *
 * @since_tizen 2.3
 *
 * @param[in] threshold The thumb scroll threshold
 *
 * @see elm_config_thumbscroll_threshold_get()
 */
EAPI void         elm_config_scroll_thumbscroll_threshold_set(unsigned int threshold);

/**
 * @brief Gets the number of pixels in the range that can be scrolled,
 *        while the scroller is held.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll hold threshold
 */
EAPI unsigned int elm_config_scroll_thumbscroll_hold_threshold_get(void);

/**
 * @brief Sets the number of pixels in the range that can be scrolled,
 *        while the scroller is held.
 *
 * @since_tizen 2.3
 *
 * @param[in] threshold The thumb scroll hold threshold
 *
 * @see elm_config_thumbscroll_hold_threshold_get()
 */
EAPI void         elm_config_scroll_thumbscroll_hold_threshold_set(unsigned int threshold);

/**
 * @brief Gets the minimum speed of the mouse cursor movement that triggers
 *        the list self scrolling animation after a mouse up event
 *        (pixels/second).
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll momentum threshold
 */
EAPI double       elm_config_scroll_thumbscroll_momentum_threshold_get(void);

/**
 * @brief Sets the minimum speed of the mouse cursor movement that triggers
 *        the list self scrolling animation after a mouse up event
 *        (pixels/second).
 *
 * @since_tizen 2.3
 *
 * @param[in] threshold The thumb scroll momentum threshold
 *
 * @see elm_config_thumbscroll_momentum_threshold_get()
 */
EAPI void         elm_config_scroll_thumbscroll_momentum_threshold_set(double threshold);

/**
 * @brief Gets the number of pixels by which the maximum distance can be flicked.
 *        If it is flicked more than this,
 *        the flick distance is same as the maximum distance.
 *
 * @since_tizen 2.3
 *
 * @return The maximum thumb scroll flick distance
 */
EAPI unsigned int elm_config_scroll_thumbscroll_flick_distance_tolerance_get(void);

/**
 * @brief Sets the number of pixels by which the maximum distance can be flicked.
 *        If it is flicked more than this,
 *        the flick distance is same as the maximum distance.
 *
 * @since_tizen 2.3
 *
 * @param[in] distance The maximum thumb scroll flick distance
 *
 * @see elm_config_thumbscroll_flick_distance_tolerance_get()
 */
EAPI void         elm_config_scroll_thumbscroll_flick_distance_tolerance_set(unsigned int distance);

/**
 * @brief Gets the amount of inertia a scroller imposes during self scrolling
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll friction
 */
EAPI double       elm_config_scroll_thumbscroll_friction_get(void);

/**
 * @brief Sets the amount of inertia a scroller imposes during self scrolling
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @param[in] friction The thumb scroll friction
 *
 * @see elm_config_thumbscroll_friction_get()
 */
EAPI void         elm_config_scroll_thumbscroll_friction_set(double friction);

/**
 * @brief Gets the minimum amount of inertia a scroller imposes durin self scrolling
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @return The minimum thumb scroll friction
 */
EAPI double       elm_config_scroll_thumbscroll_min_friction_get(void);

/**
 * @brief Sets the minimum amount of inertia a scroller imposes during self scrolling
 *        animations.
 *
 * @since_tizen 2.3
 *
 * @param[in] friction The minimum thumb scroll friction
 *
 * @see elm_config_thumbscroll_min_friction_get()
 */
EAPI void         elm_config_scroll_thumbscroll_min_friction_set(double friction);

/**
 * @brief Gets the standard velocity of the scroller. The scroll animation time is
 *        same as the thumbscroll friction, if the velocity is same as the standard
 *        velocity.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll friction
 */
EAPI double       elm_config_scroll_thumbscroll_friction_standard_get(void);

/**
 * @brief Sets the standard velocity of the scroller. The scroll animation time is
 *        same as the thumbscroll friction, if the velocity is same as the standard
 *        velocity.
 *
 * @since_tizen 2.3
 *
 * @param[in] standard The standard thumb scroll friction
 *
 * @see elm_config_thumbscroll_friction_standard_get()
 */
EAPI void         elm_config_scroll_thumbscroll_friction_standard_set(double standard);

/**
 * @brief Gets the amount of lag between your actual mouse cursor dragging
 *        movement and a scroller's view movement itself, while pushing it
 *        into the bounce state manually.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll border friction
 */
EAPI double       elm_config_scroll_thumbscroll_border_friction_get(void);

/**
 * @brief Sets the amount of lag between your actual mouse cursor dragging
 *        movement and a scroller's view movement itself, while pushing it
 *        into the bounce state manually.
 *
 * @since_tizen 2.3
 *
 * @remarks The parameter value gets bound to the 0.0 - 1.0 interval at all times.
 *
 * @param[in] friction The thumb scroll border friction \n
 *                 @c 0.0 for perfect synchrony between two movements,
 *                 @c 1.0 for maximum lag.
 *
 * @see elm_config_thumbscroll_border_friction_get()
 */
EAPI void         elm_config_scroll_thumbscroll_border_friction_set(double friction);

/**
 * @brief Gets the amount of sensitivity that is to be multiplied by the length of
 *        mouse dragging.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll sensitivity friction
 */
EAPI double       elm_config_scroll_thumbscroll_sensitivity_friction_get(void);

/**
 * @brief Sets the amount of sensitivity that is be multiplied by the length of
 *        mouse dragging.
 *
 * @since_tizen 2.3
 *
 * @remarks The parameter value gets bound to the 0.1 - 1.0 interval at all times
 *
 * @param[in] friction The thumb scroll sensitivity friction \n
 *                 @c 0.1 for minimum sensitivity,
 *                 @c 1.0 for maximum sensitivity, @c 0.25 is proper.
 *
 * @see elm_config_thumbscroll_sensitivity_friction_get()
 */
EAPI void         elm_config_scroll_thumbscroll_sensitivity_friction_set(double friction);

/**
 * @brief Gets the minimum speed of the mouse cursor movement that accelerates
 *        scrolling velocity after a mouse up event
 *        (pixels/second).
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll acceleration threshold
 */
EAPI double       elm_config_scroll_thumbscroll_acceleration_threshold_get(void);

/**
 * @brief Sets the minimum speed of the mouse cursor movement that accelerates
 *        scrolling velocity after a mouse up event
 *        (pixels/second).
 *
 * @since_tizen 2.3
 *
 * @param[in] threshold The thumb scroll acceleration threshold
 *
 * @see elm_config_thumbscroll_acceleration_threshold_get()
 */
EAPI void         elm_config_scroll_thumbscroll_acceleration_threshold_set(double threshold);

/**
 * @brief Gets the time limit for accelerating velocity.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll acceleration time limit
 */
EAPI double       elm_config_scroll_thumbscroll_acceleration_time_limit_get(void);

/**
 * @brief Sets the time limit for accelerating velocity.
 *
 * @since_tizen 2.3
 *
 * @param[in] time_limit The thumb scroll acceleration time limit
 *
 * @see elm_config_thumbscroll_acceleration_time_limit_get()
 */
EAPI void         elm_config_scroll_thumbscroll_acceleration_time_limit_set(double time_limit);

/**
 * @brief Gets the weight for acceleration.
 *
 * @since_tizen 2.3
 *
 * @return The thumb scroll acceleration weight
 */
EAPI double       elm_config_scroll_thumbscroll_acceleration_weight_get(void);

/**
 * @brief Sets the weight for acceleration.
 *
 * @since_tizen 2.3
 *
 * @param[in] weight The thumb scroll acceleration weight
 *
 * @see elm_config_thumbscroll_acceleration_weight_get()
 */
EAPI void         elm_config_scroll_thumbscroll_acceleration_weight_set(double weight);

/**
 * @}
 */

/**
 * @defgroup longpress_group Longpress
 * @ingroup elm_infra_group
 *
 * @brief Configuration for longpress events.
 *
 * @{
 */

/**
 * @brief Gets the duration for the occurrence of a long press event.
 *
 * @since_tizen 2.3
 *
 * @return The timeout for a long press event
 */
EAPI double       elm_config_longpress_timeout_get(void);

/**
 * @brief Sets the duration for the occurrence of a long press event.
 *
 * @since_tizen 2.3
 *
 * @param[in] longpress_timeout The timeout for a long press event
 */
EAPI void         elm_config_longpress_timeout_set(double longpress_timeout);

/**
 * @}
 */

/**
 * @defgroup softcursor_group SotfCursor
 * @ingroup elm_infra_group
 *
 * @brief Configuration for softcursor.
 *
 * @{
 */

/**
 * @brief Enumeration of Elm Softcursor Mode
 */
typedef enum _Elm_Softcursor_Mode
{
   ELM_SOFTCURSOR_MODE_AUTO, /**< Auto-detect if a software cursor should be used (default) */
   ELM_SOFTCURSOR_MODE_ON, /**< Always use a softcursor */
   ELM_SOFTCURSOR_MODE_OFF /**< Never use a softcursor */
} Elm_Softcursor_Mode; /**< @since 1.7 */

/**
 * @brief Sets the mode used for software provided mouse cursors inline with the window
 *        canvas.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @remarks A software rendered cursor can be provided for rendering inline inside the
 *          canvas window in the event that the native display system does not provide one
 *          or the native one is not needed.
 *
 * @param[in] mode The mode used for software cursor
 *
 * @see elm_config_softcursor_mode_get()
 */
EAPI void         elm_config_softcursor_mode_set(Elm_Softcursor_Mode mode);

/**
 * @brief Gets the software cursor mode.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @return The mode used for software cursors
 *
 * @see elm_config_softcursor_mode_set()
 */
EAPI Elm_Softcursor_Mode elm_config_softcursor_mode_get(void);

/**
 * @}
 */

/**
 * @ingroup Tooltips
 * @{
 */

/**
 * @brief Gets the duration after which a tooltip is shown.
 *
 * @since_tizen 2.3
 *
 * @return The duration after which a tooltip is shown
 */
EAPI double      elm_config_tooltip_delay_get(void);

/**
 * @brief Sets the duration after which a tooltip is shown.
 *
 * @since_tizen 2.3
 *
 * @param[in] delay The delay duration
 * @return @c EINA_TRUE if the value is set, otherwise @c EINA_FALSE
 */
EAPI void        elm_config_tooltip_delay_set(double delay);

/**
 * @}
 */

/**
 * @ingroup Cursors
 * @{
 */

/**
 * @brief Gets only the usage of the configured cursor engine.
 *
 * @details This gets the globally configured exclusive usage of the engine cursors.
 *
 * @since_tizen 2.3
 *
 * @return @c 1 if only engine cursors should be used,
 *         otherwise @c 0
 */
EAPI Eina_Bool   elm_config_cursor_engine_only_get(void);

/**
 * @brief Sets only the usage of the configured cursor engine.
 *
 * @details This sets the globally configured exclusive usage of the engine cursors.
 *          It won't affect the cursors set until this value is changed.
 *
 * @since_tizen 2.3
 *
 * @param[in] engine_only If @c 1 only engine cursors are enabled,
 *                    otherwise @c 0 if they are looked for on the theme
 */
EAPI void        elm_config_cursor_engine_only_set(Eina_Bool engine_only);

/**
 * @}
 */

/**
 * @ingroup Scaling
 * @{
 */

/**
 * @brief Gets the global scaling factor.
 *
 * @details This gets the globally configured scaling factor that is applied to all
 *          objects.
 *
 * @since_tizen 2.3
 *
 * @return The scaling factor
 */
EAPI double elm_config_scale_get(void);

/**
 * @brief Sets the global scaling factor.
 *
 * @details This sets the globally configured scaling factor that is applied to all
 *          objects.
 *
 * @since_tizen 2.3
 *
 * @param[in] scale The scaling factor to set
 */
EAPI void   elm_config_scale_set(double scale);

/**
 * @}
 */

/**
 * @defgroup Password_last_show Password show last
 * @ingroup elm_infra_group
 *
 * @brief Showing the last feature of the password mode enables users to view
 *        the last input entered for a few seconds before it is masked.
 *
 * These functions allow to set this feature in the password mode
 * of the entry widget and also allow to manipulate the duration
 * for which the input has to be visible.
 *
 * @{
 */

/**
 * @brief Gets the "show last" setting of the password mode.
 *
 * @details This gets the "show last" setting of the password mode which might be
 *          enabled or disabled.
 *
 * @since_tizen 2.3
 *
 * @return @c EINA_TRUE if the "show last" setting is enabled,
 *         otherwise @c EINA_FALSE  if it's disabled
 */
EAPI Eina_Bool elm_config_password_show_last_get(void);

/**
 * @brief Sets the show last setting of the password mode.
 *
 * @details This enables or disables the show last setting of the password mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] password_show_last If @c EINA_TRUE it enables "show last" in the password mode,
 *                           otherwise @c EINA_FALSE
 * @see elm_config_password_show_last_timeout_set()
 */
EAPI void      elm_config_password_show_last_set(Eina_Bool password_show_last);

/**
 * @brief Gets the timeout value of the "show last" password mode.
 *
 * @details This gets the timeout value for which the last input entered in the password
 *          mode is visible.
 *
 * @since_tizen 2.3
 *
 * @return The timeout value of the "show last" password mode
 */
EAPI double    elm_config_password_show_last_timeout_get(void);

/**
 * @brief Sets the timeout value of the "show last" password mode.
 *
 * @details This sets the timeout value for which the last input entered in the password
 *          mode is visible.
 *
 * @since_tizen 2.3
 *
 * @param[in] password_show_last_timeout The timeout value
 * @see elm_config_password_show_last_set()
 */
EAPI void      elm_config_password_show_last_timeout_set(double password_show_last_timeout);

/**
 * @}
 */

/**
 * @defgroup Engine Elementary Engine
 * @ingroup elm_infra_group
 *
 * @brief These are functions that set and query the rendering engine that
 *        Elementary uses for drawing its windows' pixels.
 *
 * The following are the available engines:
 * @li "fb"
 * @li "buffer"
 * @li "ews"
 * @li NULL - no engine config
 *
 * @note Please use @ref elm_config_accel_preference_override_set() instead
 *
 * @{
 */

/**
 * @brief Gets Elementary's rendering engine in use.
 *
 * @details This gets the global rendering engine that is applied to all Elementary
 *          applications.
 *
 * @since_tizen 2.3
 *
 * @remarks There's no need to free the returned string, here.
 *
 *
 * @return The rendering engine name
 *
 * @see elm_config_engine_set()
 */
EAPI const char *elm_config_engine_get(void);

/**
 * @brief Sets Elementary's rendering engine for use.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that it takes effect only on Elementary windows created after
 *          this is called.
 *
 * @param[in] engine The rendering engine name
 *
 * @see elm_win_add()
 */
EAPI void        elm_config_engine_set(const char *engine);

/**
 * @brief Gets Elementary's preferred engine to use.
 *
 * @details This gets the global rendering engine that is applied to all Elementary
 *          applications and is PREFERRED by the application. This can (and will)
 *          override the engine that is configured for all applications.
 *
 * @since_tizen 2.3
 *
 * @remarks There's no need to free the returned string, here.
 *
 * @return The rendering engine name
 *
 * @see elm_config_preferred_engine_set()
 */
EAPI const char *elm_config_preferred_engine_get(void);

/**
 * @brief Sets Elementary's preferred rendering engine for use.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that it takes effect only on Elementary windows created after
 *          this is called. This overrides the engine set by the configuration at
 *          application startup. Note that it is a hint and may not be honored.
 *
 * @param[in] engine The rendering engine name
 *
 * @see elm_win_add()
 */
EAPI void        elm_config_preferred_engine_set(const char *engine);

/**
 * @brief Get Elementary's preferred engine to use.
 *
 * @return The acceleration preference hint string
 * @note there's no need to free the returned string, here.
 *
 * @details See elm_config_accel_preference_set() for more information, but
 *          this simply returns what was set by this call, nothing more.
 *
 * @since_tizen 2.3
 *
 * @see elm_config_accel_preference_set()
 * @since 1.10
 */
EAPI const char *elm_config_accel_preference_get(void);

/**
 * @brief Set Elementary's acceleration preferences for new windows.
 *
 * @param[in] pref The preference desired as a normal C string
 *
 * @details Note that it will take effect only to Elementary windows created after
 *          this is called. The @p pref string is a freeform C string that indicates
 *          what kind of acceleration is preferred. This may or may not be honored,
 *          but a best attempt will be made. Known strings are as follows:
 * @li "gl", "opengl" - try use opengl.
 * @li "3d" - try and use a 3d acceleration unit.
 * @li "hw", "hardware", "accel" - try any acceleration unit (best possible)
 *
 * @note This takes precedence over engine preferences set with
 *       @ref elm_config_preferred_engine_set().
 *
 * @since_tizen 2.3
 *
 * @see elm_win_add()
 * @since 1.10
 */
EAPI void        elm_config_accel_preference_set(const char *pref);

/**
 * @brief Get the acceleration override preference flag
 *
 * @since_tizen 2.3
 *
 * @details This gets the acceleration override preference. This is a flag that
 *          has the global system acceleration preference configuration forcibly
 *          override whatever acceleration preference the application may want.
 *
 * @return If acceleration override is enabled
 *
 * @since 1.11
 */
EAPI Eina_Bool  elm_config_accel_preference_override_get(void);

/**
 * @brief Set the acceleration override preference flag
 *
 * @since_tizen 2.3
 *
 * @details This sets the acceleration override preference. This is a flag that
 *          has the global system acceleration preference configuration forcibly
 *          override whatever acceleration preference the application may want.
 *
 * @param[in] enabled This should be @c EINA_TRUE if enabled, or @c EINA_FALSE if
 *                    not.
 *
 * @since 1.11
 */
EAPI void       elm_config_accel_preference_override_set(Eina_Bool enabled);

/**
 * @}
 */

typedef struct _Elm_Text_Class
{
   const char *name;
   const char *desc;
} Elm_Text_Class;

/**
 * @brief Structure of Elm Font Overlay
 * @ingroup Fonts
 */
typedef struct _Elm_Font_Overlay
{
   const char    *text_class;
   const char    *font;
   Evas_Font_Size size;
} Elm_Font_Overlay;

/**
 * @ingroup Fonts
 *
 * @{
 */

/**
 * @brief Gets Elementary's list of supported text classes.
 *
 * @since_tizen 2.3
 *
 * @remarks Release the list with elm_text_classes_list_free().
 *
 * @return The text classes list with @c Elm_Text_Class blobs as data
 */
EAPI Eina_List *elm_config_text_classes_list_get(void);

/**
 * @brief Frees Elementary's list of supported text classes.
 *
 * @param[in] list The text classes list to be freed.
 *
 * @since_tizen 2.3
 *
 * @see elm_config_text_classes_list_get().
 */
EAPI void elm_config_text_classes_list_free(Eina_List *list);

/**
 * @brief Gets Elementary's list of font overlays, set using
 *        elm_config_font_overlay_set().
 *
 * @since_tizen 2.3
 *
 * @remarks For each text class, one can set a <b>font overlay</b>,
 *          overriding the default font properties for that class coming from
 *          the theme in use. There is no need to free this list.
 *
 * @return The font overlays list with #Elm_Font_Overlay blobs as data
 *
 * @see elm_config_font_overlay_set()
 * @see elm_config_font_overlay_unset()
 */
EAPI const Eina_List *elm_config_font_overlay_list_get(void);

/**
 * @brief Sets a font overlay for a given Elementary text class.
 *
 * @since_tizen 2.3
 *
 * @remarks @a font has to be in the format returned by elm_font_fontconfig_name_get().
 *
 * @param[in] text_class The text class name
 * @param[in] font The font name and style string
 * @param[in] size The font size
 *
 * @see elm_config_font_overlay_list_get()
 * @see elm_config_font_overlay_unset()
 * @see edje_object_text_class_set()
 */
EAPI void             elm_config_font_overlay_set(const char *text_class, const char *font, Evas_Font_Size size);

/**
 * @}
 */

/**
 * @ingroup Access
 * @{
 */

/**
 * @brief Gets the access mode.
 *
 * @since 1.7
 *
 * @return The access mode bouncing state
 *
 * @see elm_config_access_set()
 */
EAPI Eina_Bool        elm_config_access_get(void);

/**
 * @brief Sets the access mode.
 *
 * @since 1.7
 *
 * @remarks Elementary objects may have information (e.g. label on the elm_button)
 *          to be read. This information is read by the access module when an object
 *          receives the EVAS_CALLBACK_MOUSE_IN event.
 *
 * @param[in] is_access If @c EINA_TRUE it enables the access mode, otherwise @c EINA_FALSE
 *
 * @see elm_config_access_get()
 */
EAPI void             elm_config_access_set(Eina_Bool is_access);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets whether reading the password on accessibiliy is enabled.
 *
 * @since 1.8
 *
 * @return @c EINA_TRUE if the reading the password on accessibility is enabled,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_config_access_password_read_enabled_set()
 */
EAPI Eina_Bool        elm_config_access_password_read_enabled_get(void);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sets whether reading the password on accessibiliy is enabled.
 *
 * @since 1.8
 *
 * @param[in] enabled If @c EINA_TRUE it enables reading the password on accessibility,
 *                otherwise @c EINA_FALSE
 *
 * @see elm_config_access_password_read_enabled_get()
 */
EAPI void             elm_config_access_password_read_enabled_set(Eina_Bool enabled);

/**
 * @}
 */

/**
 * @defgroup Selection Elementary Selection
 * @ingroup elm_infra_group
 *
 * @brief Elementary Selection configuration
 *
 * @{
 */

/**
 * @brief Gets whether the selection should be cleared when entry widget is unfocused.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @return @c EINA_TRUE if the selection would be cleared when unfocused,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_config_selection_unfocused_clear_set()
 */
EAPI Eina_Bool        elm_config_selection_unfocused_clear_get(void);

/**
 * @brief Sets whether the selection should be cleared when entry widget is unfocused.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] enabled If @c EINA_TRUE clear the selection when unfocused,
 *                otherwise @c EINA_FALSE to not clear the selection when unfocused
 *
 * @see elm_config_selection_unfocused_clear_get()
 */
EAPI void             elm_config_selection_unfocused_clear_set(Eina_Bool enabled);

/**
 * @}
 */

/**
 * @ingroup Fonts
 * @{
 */

/**
 * @brief Unsets a font overlay for a given Elementary text class.
 *
 * @since_tizen 2.3
 *
 * @remarks This brings back text elements belonging to the text class
 *          @a text_class back to their default font settings.
 *
 * @param[in] text_class The text class name
 */
EAPI void             elm_config_font_overlay_unset(const char *text_class);

/**
 * @brief Applies the changes made with elm_config_font_overlay_set() and
 *        elm_config_font_overlay_unset() on the current Elementary window.
 *
 * @since_tizen 2.3
 *
 * @details This applies all font overlays set to all objects in the UI.
 */
EAPI void             elm_config_font_overlay_apply(void);

/**
 * @}
 */

/**
 * @ingroup Fingers
 * @{
 */

/**
 * @brief Gets the configured "finger size".
 *
 * @details This gets the globally configured finger size, <b>in pixels</b>.
 *
 * @since_tizen 2.3
 *
 * @return The finger size
 */
EAPI Evas_Coord elm_config_finger_size_get(void);

/**
 * @brief Sets the configured finger size.
 *
 * @details This sets the globally configured finger size in pixels.
 *
 * @since_tizen 2.3
 *
 * @param[in] size The finger size
 */
EAPI void       elm_config_finger_size_set(Evas_Coord size);

/**
 * @}
 */

/**
 * @ingroup Caches
 * @{
 */

/**
 * @brief Gets the configured cache flush interval time.
 *
 * @details This gets the globally configured cache flush interval time, in
 *          ticks.
 *
 * @since_tizen 2.3
 *
 * @return The cache flush interval time
 *
 * @see elm_cache_all_flush()
 */
EAPI int       elm_config_cache_flush_interval_get(void);

/**
 * @brief Sets the configured cache flush interval time.
 *
 * @details This sets the globally configured cache flush interval time, in ticks.
 *
 * @since_tizen 2.3
 *
 * @remarks The @a size must be greater than @c 0. If not, the cache flush is
 *          ignored.
 *
 * @param[in] size The cache flush interval time
 *
 * @see elm_cache_all_flush()
 */
EAPI void      elm_config_cache_flush_interval_set(int size);

/**
 * @brief Gets the configured cache flush enabled state.
 *
 * @details This gets the globally configured cache flush state - whether it is enabled
 *          or not. When cache flushing is enabled, elementary regularly
 *          (see elm_config_cache_flush_interval_get() ) flushes caches and dumps data out of
 *          memory and allows usage to re-seed caches and data in memory where it
 *          can do so. An idle application thus minimizes its memory usage as
 *          data is freed from memory and does not re-load as it is idle and
 *          not rendering or doing anything graphically right now.
 *
 * @since_tizen 2.3
 *
 * @return The cache flush state
 *
 * @see elm_cache_all_flush()
 */
EAPI Eina_Bool elm_config_cache_flush_enabled_get(void);

/**
 * @brief Sets the configured cache flush enabled state.
 *
 * @details This sets the globally configured cache flush enabled state.
 *
 * @since_tizen 2.3
 *
 * @param[in] enabled The cache flush enabled state
 *
 * @see elm_cache_all_flush()
 */
EAPI void      elm_config_cache_flush_enabled_set(Eina_Bool enabled);

/**
 * @brief Gets the configured font cache size.
 *
 * @details This gets the globally configured font cache size, in bytes.
 *
 * @since_tizen 2.3
 *
 * @return The font cache size
 */
EAPI int       elm_config_cache_font_cache_size_get(void);

/**
 * @brief Sets the configured font cache size
 *
 * @details This sets the globally configured font cache size, in bytes
 *
 * @since_tizen 2.3
 *
 * @param[in] size The font cache size
 */
EAPI void      elm_config_cache_font_cache_size_set(int size);

/**
 * @brief Gets the configured image cache size.
 *
 * @details This gets the globally configured image cache size, in bytes
 *
 * @since_tizen 2.3
 *
 * @return The image cache size
 */
EAPI int       elm_config_cache_image_cache_size_get(void);

/**
 * @brief Sets the configured image cache size.
 *
 * @details This sets the globally configured image cache size, in bytes.
 *
 * @since_tizen 2.3
 *
 * @param[in] size The image cache size
 */
EAPI void       elm_config_cache_image_cache_size_set(int size);


/**
 * @brief Gets the configured edje file cache size.
 *
 * @details This gets the globally configured edje file cache size, in number
 *          of files.
 *
 * @since_tizen 2.3
 *
 * @return The edje file cache size
 */
EAPI int       elm_config_cache_edje_file_cache_size_get(void);

/**
 * @brief Sets the configured edje file cache size.
 *
 * @details This sets the globally configured edje file cache size, in number
 *          of files.
 *
 * @since_tizen 2.3
 *
 * @param[in] size The edje file cache size
 */
EAPI void       elm_config_cache_edje_file_cache_size_set(int size);

/**
 * @brief Gets the configured edje collections (groups) cache size.
 *
 * @details This gets the globally configured edje collections cache size, in
 *          number of collections.
 *
 * @since_tizen 2.3
 *
 * @return The edje collections cache size
 */
EAPI int       elm_config_cache_edje_collection_cache_size_get(void);

/**
 * @brief Sets the configured edje collections (groups) cache size.
 *
 * @details This sets the globally configured edje collections cache size, in
 *          number of collections.
 *
 * @since_tizen 2.3
 *
 * @param[in] size The edje collections cache size
 */
EAPI void       elm_config_cache_edje_collection_cache_size_set(int size);

/**
 * @}
 */

/**
 * @ingroup Focus
 * @{
 */

/**
 * @brief Gets the enable status of the focus highlight.
 *
 * @details This gets whether the highlight on the focused objects is enabled.
 *
 * @return The status of the focus highlight
 *
 * @since_tizen 2.3
 *
 * @see elm_config_focus_highlight_enabled_set()
 */
EAPI Eina_Bool            elm_config_focus_highlight_enabled_get(void);

/**
 * @brief Sets the enable status of the focus highlight.
 *
 * @details This sets whether to show the highlight on focused objects or not.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that it takes effect only on Elementary windows created after
 *          this is called.
 *
 * @param[in] enable If @c EINA_TRUE it enables highlight,
 *               otherwise @c EINA_FALSE disables highlight
 *
 * @see elm_win_add()
 */
EAPI void                 elm_config_focus_highlight_enabled_set(Eina_Bool enable);

/**
 * @brief Gets the enable status of the highlight animation.
 *
 * @details This gets whether the focus highlight, if enabled, animates its switch from
 *          one object to the next.
 *
 * @since_tizen 2.3
 *
 * @return The focus highlight mode set
 */
EAPI Eina_Bool            elm_config_focus_highlight_animate_get(void);

/**
 * @brief Sets the enable status of the highlight animation.
 *
 * @details This sets whether the focus highlight, if enabled, animates its switch from
 *          one object to the next.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that it takes effect only on Elementary windows created after
 *          this is called.
 *
 * @param[in] animate If @c EINA_TRUE it enables animation,
 *                otherwise @c EINA_FALSE disables it
 *
 * @see elm_win_add()
 */
EAPI void                 elm_config_focus_highlight_animate_set(Eina_Bool animate);

/**
 * @}
 */

/**
 * @brief Gets the system mirrored mode. This determines the default mirrored mode
 *        of widgets.
 *
 * @since_tizen 2.3
 *
 * @return @c EINA_TRUE if mirrored mode is set,
 *         otherwise @c EINA_FALSE
 * @ingroup Mirroring
 */
EAPI Eina_Bool elm_config_mirrored_get(void);

/**
 * @brief Sets the system mirrored mode. This determines the default mirrored mode
 *        of widgets.
 *
 * @since_tizen 2.3
 *
 * @param[in] mirrored If @c EINA_TRUE the mirrored mode is set,
 *                 otherwise @c EINA_FALSE to unset it
 * @ingroup Mirroring
 */
EAPI void      elm_config_mirrored_set(Eina_Bool mirrored);

/**
 * @brief Gets the indicator service name according to the rotation degree.
 *
 * @since_tizen 2.3
 *
 * @param[in] rotation The rotation that is related to the indicator service name, in degrees (0-360)
 *
 * @return The indicator service name according to the rotation degree
 * @ingroup Conformant
 */
EAPI const char *elm_config_indicator_service_get(int rotation);

/**
 * @ingroup Elm_Gesture_Layer
 * @{
 */

/**
 * @brief Gets the duration for occurrence of the long tap event of the gesture layer.
 *
 * @since_tizen 2.3
 *
 * @return The timeout for the long tap event of the gesture layer
 */
EAPI double   elm_config_glayer_long_tap_start_timeout_get(void);

/**
 * @brief Sets the duration for occurrence of the long tap event of the gesture layer.
 *
 * @since_tizen 2.3
 *
 * @param[in] long_tap_timeout The timeout for the long tap event of the gesture layer
 */
EAPI void   elm_config_glayer_long_tap_start_timeout_set(double long_tap_timeout);

/**
 * @brief Gets the duration for occurrence of the double tap event of the gesture layer.
 *
 * @since_tizen 2.3
 *
 * @return The timeout for the double tap event of the gesture layer
 */
EAPI double   elm_config_glayer_double_tap_timeout_get(void);

/**
 * @brief Sets the duration for occurrence of the double tap event of the gesture layer.
 *
 * @since_tizen 2.3
 *
 * @param[in] double_tap_timeout The timeout for the double tap event of the gesture layer
 */
EAPI void   elm_config_glayer_double_tap_timeout_set(double double_tap_timeout);

/**
 * @}
 */

/**
 * @defgroup colors_group Elementary Colors
 * @ingroup elm_infra_group
 * @brief Configuration for Elementary colors.
 *
 * @{
 */

typedef struct _Elm_Color_Class
{
   const char *name;
   const char *desc;
} Elm_Color_Class;

typedef struct _Elm_Color_Overlay
{
   const char *color_class;
   struct {
      unsigned char r, g, b, a;
   } color, outline, shadow;
} Elm_Color_Overlay;

/**
 * @brief Gets Elementary's list of supported color classes.
 *
 * @since 1.10
 *
 * @since_tizen 2.3
 *
 * @remarks Release the list with elm_color_classes_list_free().
 *
 * @return The color classes list with @c Elm_Color_Class blobs as data
 */
EAPI Eina_List *elm_config_color_classes_list_get(void);

/**
 * @brief Frees Elementary's list of supported color classes.
 *
 * @since 1.10
 *
 * @since_tizen 2.3
 *
 * @param[in] list The color classes list
 *
 * @see elm_config_color_classes_list_get().
 */
EAPI void      elm_config_color_classes_list_free(Eina_List *list);

/**
 * @brief Gets Elementary's list of color overlays, set with
 *        elm_config_color_overlay_set().
 *
 * @since 1.10
 *
 * @since_tizen 2.3
 *
 * @remarks For each color class, one can set a <b>color overlay</b> for it,
 *          overriding the default color properties for that class coming from
 *          the theme in use. There is no need to free this list.
 *
 * @return The color overlays list with @c Elm_Color_Overlay blobs as data
 *
 * @see elm_config_color_overlay_set()
 * @see elm_config_color_overlay_unset()
 */
EAPI const Eina_List *elm_config_color_overlay_list_get(void);

/**
 * @brief Sets a color overlay for a given Elementary color class.
 *
 * @since 1.10
 *
 * @since_tizen 2.3
 *
 * @remarks The first color is the object, the second is the text outline, and
 *          the third is the text shadow. (Note that the second two only apply
 *          to text parts).
 *
 * @remarks Setting color emits a signal "color_class,set" with source being
 *          the given color class in all edje objects.
 *
 * @remarks Unlike Evas, Edje colors are @b not pre-multiplied. That is,
 *          half-transparent white is 255 255 255 128.
 *
 * @param[in] color_class The color class name
 * @param[in] r The object red value
 * @param[in] g The object green value
 * @param[in] b The object blue value
 * @param[in] a The object alpha value
 * @param[in] r2 The outline red value
 * @param[in] g2 The outline green value
 * @param[in] b2 The outline blue value
 * @param[in] a2 The outline alpha value
 * @param[in] r3 The shadow red value
 * @param[in] g3 The shadow green value
 * @param[in] b3 The shadow blue value
 * @param[in] a3 The shadow alpha value
 *
 * @see elm_config_color_overlay_list_get()
 * @see elm_config_color_overlay_unset()
 * @see edje_color_class_set()
 */
EAPI void      elm_config_color_overlay_set(const char *color_class,
                                            int r, int g, int b, int a,
                                            int r2, int g2, int b2, int a2,
                                            int r3, int g3, int b3, int a3);

/**
 * @brief Unsets a color overlay for a given Elementary color class.
 *
 * @since 1.10
 *
 * @since_tizen 2.3
 *
 * @remarks This brings back color elements belonging to the color class
 *          @p color_class back to their default color settings.
 *
 * @param[in] color_class The color class name
 */
EAPI void      elm_config_color_overlay_unset(const char *color_class);

/**
 * @brief Applies the changes made with elm_config_color_overlay_set() and
 *        elm_config_color_overlay_unset() on the current Elementary window.
 *
 * @details This applies to all color overlays set to all objects in the UI.
 *
 * @since 1.10
 *
 * @since_tizen 2.3
 */
EAPI void      elm_config_color_overlay_apply(void);

/**
 * @}
 */

/**
 * @internal
 * @defgroup fps_group Elementary FPS
 * @ingroup elm_infra_group
 *
 * @{
 */

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Gets the FPS value for ecore_animator_frametime and edje_frametime calculation.
 *
 * @return The fps value
 */
EAPI double   elm_config_fps_get(void);

/**
 * @internal
 * @remarks Tizen only feature
 * @brief Sets the FPS value for ecore_animator_frametime and edje_frametime calculation.
 *
 * @param[in] fps The fps value to set
 */
EAPI void      elm_config_fps_set(double fps);

/**
 * @}
 */

