/**
 * @internal
 * @brief Requests that your elementary application needs Efreet.
 *
 * @details This initializes the Efreet library when called and if support exists
 *          it returns @c EINA_TRUE, otherwise it returns @c EINA_FALSE. This must be called
 *          before any efreet calls.
 *
 * @since_tizen 2.3
 *
 * @return @c EINA_TRUE if support exists and initialization succeeds,
 *         otherwise @c EINA_FALSE
 *
 * @ingroup Efreet
 */
EAPI Eina_Bool elm_need_efreet(void);

/**
 * @internal
 *
 * @brief Requests that your elementary application needs e_dbus.
 *
 * @details This initializes the E_dbus library when called and if support exists
 *          it returns @c EINA_TRUE, otherwise it returns @c EINA_FALSE. This must be called
 *          before any e_dbus calls.
 *
 * @return @c EINA_TRUE if support exists and initialization succeeds,
 *         otherwise @c EINA_FALSE
 *
 * @ingroup E_dbus
 */
EAPI Eina_Bool elm_need_e_dbus(void);

/**
 * @internal
 * @brief Requests that your elementary application needs ethumb.
 *
 * @details This initializes the Ethumb library when called and if support exists
 *          it returns @c EINA_TRUE, otherwise it returns @c EINA_FALSE.
 *          This must be called before any other function that deals with
 *          elm_thumb objects or ethumb_client instances is called.
 *
 * @since_tizen 2.3
 *
 * @ingroup Thumb
 */
EAPI Eina_Bool elm_need_ethumb(void);

/**
 * @internal
 * @brief Requests that your elementary application needs web support.
 *
 * @details This initializes the Ewebkit library when called and if support exists
 *          it returns @c EINA_TRUE, otherwise it returns @c EINA_FALSE.
 *          This must be called before any other function that deals with
 *          elm_web objects or ewk_view instances is called.
 *
 * @since_tizen 2.3
 *
 * @ingroup Web
 */
EAPI Eina_Bool elm_need_web(void);
