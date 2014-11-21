/**
 * @internal
 * @defgroup Actionslider Actionslider
 * @ingroup Elementary
 *
 * @image html actionslider_inheritance_tree.png
 * @image latex actionslider_inheritance_tree.eps
 *
 * @image html img/widget/actionslider/preview-00.png
 * @image latex img/widget/actionslider/preview-00.eps
 *
 * An actionslider is a switcher for @c 2 or @c 3 labels with customizable magnet
 * properties. The user drags and releases the indicator, to choose a label.
 *
 * Labels occupy the following positions:
 * a. Left
 * b. Right
 * c. Center
 *
 * Positions can be enabled or disabled.
 *
 * Magnets can be set on the above positions.
 *
 * When the indicator is released, it moves to its nearest
 * "enabled and magnetized" position.
 *
 * By default, all positions are set as enabled.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for actionslider objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li @c "selected" - When the user selects an enabled position (the
 *              label is passed as event info).
 * @li @c "pos_changed" - When the indicator reaches any of the
 *                 positions("left", "right" or "center").
 *
 * The default text parts of the actionslider widget that you can use are:
 * @li "indicator" - An indicator label of the actionslider.
 * @li "left" - A left label of the actionslider.
 * @li "right" - A right label of the actionslider.
 * @li "center" - A center label of the actionslider.
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 *
 * @{
 */
typedef enum
{
   ELM_ACTIONSLIDER_NONE = 0,
   ELM_ACTIONSLIDER_LEFT = 1 << 0,
   ELM_ACTIONSLIDER_CENTER = 1 << 1,
   ELM_ACTIONSLIDER_RIGHT = 1 << 2,
   ELM_ACTIONSLIDER_ALL = (1 << 3) - 1
} Elm_Actionslider_Pos;

/**
 * @brief Adds a new actionslider to the parent.
 *
 * @param[in] parent The parent object
 * @return The new actionslider object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object                *elm_actionslider_add(Evas_Object *parent);

/**
 * @brief Gets the actionslider selected label.
 *
 * @param[in] obj The actionslider object
 * @return The selected label
 */
EAPI const char                 *elm_actionslider_selected_label_get(const Evas_Object *obj);

/**
 * @brief Sets the actionslider indicator position.
 *
 * @param[in] obj The actionslider object
 * @param[in] pos The position of the indicator
 */
EAPI void                        elm_actionslider_indicator_pos_set(Evas_Object *obj, Elm_Actionslider_Pos pos);

/**
 * @brief Gets the actionslider indicator position.
 *
 * @param[in] obj The actionslider object
 * @return The position of the indicator
 */
EAPI Elm_Actionslider_Pos        elm_actionslider_indicator_pos_get(const Evas_Object *obj);

/**
 * @brief Sets the actionslider magnet position. To make multiple positions magnets as enabled @c or
 *        them together(e.g.: @c ELM_ACTIONSLIDER_LEFT | @c ELM_ACTIONSLIDER_RIGHT).
 *
 * @param[in] obj The actionslider object
 * @param[in] pos The bit mask indicating the magnet positions
 */
EAPI void                        elm_actionslider_magnet_pos_set(Evas_Object *obj, Elm_Actionslider_Pos pos);

/**
 * @brief Gets the actionslider magnet position.
 *
 * @param[in] obj The actionslider object
 * @return The positions with the magnet property
 */
EAPI Elm_Actionslider_Pos        elm_actionslider_magnet_pos_get(const Evas_Object *obj);

/**
 * @brief Sets the actionslider enabled position. To set multiple positions as enabled OR
 *        them together(e.g.: @c ELM_ACTIONSLIDER_LEFT | @c ELM_ACTIONSLIDER_RIGHT).
 *
 * @remarks All the positions are enabled by default.
 *
 * @param[in] obj The actionslider object
 * @param[in] pos The bit mask indicating the enabled positions
 */
EAPI void                        elm_actionslider_enabled_pos_set(Evas_Object *obj, Elm_Actionslider_Pos pos);

/**
 * @brief Gets the actionslider enabled position.
 *
 * @param[in] obj The actionslider object
 * @return The enabled positions
 */
EAPI Elm_Actionslider_Pos        elm_actionslider_enabled_pos_get(const Evas_Object *obj);

/**
 * @}
 */
