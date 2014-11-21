/**
 * @defgroup Colorselector Colorselector
 * @ingroup elm_widget_group
 *
 * @image html colorselector_inheritance_tree.png
 * @image latex colorselector_inheritance_tree.eps
 *
 * @brief A ColorSelector is a color selection widget.
 *
 * It allows an application to set a series of colors.It also allows to
 * load/save colors from/to config with a unique identifier, by default,
 * the colors are loaded/saved from/to config using a "default" identifier.
 * The colors  can be picked by the user from the color set by clicking on
 * individual color items on the palette or by selecting it from the selector.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for check objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * - @c "changed" - When the color value changes on the selector.
 *   @a event_info is @c NULL.
 * - @c "color,item,selected" - When the user clicks on a color item. The
 *   @a event_info parameter of the callback is the selected color
 *   item.
 * - @c "color,item,longpressed" - When user long presses on a color
 *   item. The @a event_info parameter of the callback contains the selected
 *   color item.
 *
 * @{
 */

typedef struct _Elm_Color_RGBA
{
   unsigned int r;
   unsigned int g;
   unsigned int b;
   unsigned int a;
   const char *color_name;
} Elm_Color_RGBA;

typedef struct _Elm_Custom_Palette
{
   const char *palette_name;
   Eina_List  *color_list;
} Elm_Custom_Palette;

/**
 * @enum Elm_Colorselector_Mode
 * @typedef Elm_Colorselector_Mode
 *
 * @brief Enumeration that defines the different modes supported by Colorselector.
 *
 * @see elm_colorselector_mode_set()
 * @see elm_colorselector_mode_get()
 */
typedef enum
{
   ELM_COLORSELECTOR_PALETTE = 0, /**< Only the color palette is displayed */
   ELM_COLORSELECTOR_COMPONENTS, /**< Only the color selector is displayed */
   ELM_COLORSELECTOR_BOTH, /**< Both the Palette and the selector is displayed, default */
   ELM_COLORSELECTOR_PICKER, /**< Only the color picker is displayed */
   ELM_COLORSELECTOR_PLANE, /**< Only the color plane is displayed */
   ELM_COLORSELECTOR_PALETTE_PLANE, /**< Both the palette and the plane is displayed */
   ELM_COLORSELECTOR_ALL /**< All possible color selectors are displayed */
} Elm_Colorselector_Mode;

/**
 * @brief Adds a new colorselector to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object *elm_colorselector_add(Evas_Object *parent);

/**
 * @brief Sets a color to the colorselector.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The colorselector object
 * @param[in] r The r-value of color
 * @param[in] g The g-value of color
 * @param[in] b The b-value of color
 * @param[in] a The a-value of color
 */
EAPI void elm_colorselector_color_set(Evas_Object *obj, int r, int g, int b, int a);

/**
 * @brief Gets the current color from the colorselector.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The colorselector object
 * @param[out] r An integer pointer for the r-value of color
 * @param[out] g An integer pointer for the g-value of color
 * @param[out] b An integer pointer for the b-value of color
 * @param[out] a An integer pointer for the a-value of color
 */
EAPI void elm_colorselector_color_get(const Evas_Object *obj, int *r, int *g, int *b, int *a);

/**
 * @brief Sets the Colorselector mode.
 *
 * @since_tizen 2.3
 *
 * @remarks Colorselector supports three modes: palette only, selector only, and both.
 *
 * @param[in] obj The colorselector object
 * @param[in] mode The Elm_Colorselector_Mode
 */
EAPI void elm_colorselector_mode_set(Evas_Object *obj, Elm_Colorselector_Mode mode);

/**
 * @brief Gets the Colorselector mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Colorselector object
 * @return mode The current mode of the colorselector
 */
EAPI Elm_Colorselector_Mode elm_colorselector_mode_get(const Evas_Object *obj);

/**
 * @brief Gets the Palette item's color.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The color palette item
 * @param[out] r An integer pointer for the r-value of color
 * @param[out] g An integer pointer for the g-value of color
 * @param[out] b An integer pointer for the b-value of color
 * @param[out] a An integer pointer for the a-value of color
 */
EAPI void elm_colorselector_palette_item_color_get(const Elm_Object_Item *it, int *r, int *g, int *b, int *a);

/**
 * @brief Sets the palette item's color.
 *
 * @since_tizen 2.3
 *
 * @param[in] it The color palette item
 * @param[in] r The r-value of color
 * @param[in] g The g-value of color
 * @param[in] b The b-value of color
 * @param[in] a The a-value of color
 */
EAPI void elm_colorselector_palette_item_color_set(Elm_Object_Item *it, int r, int g, int b, int a);

/**
 * @brief Adds a new color item to the palette.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Colorselector object
 * @param[in] r The r-value of color
 * @param[in] g The g-value of color
 * @param[in] b The b-value of color
 * @param[in] a The a-value of color
 * @return A new color palette Item
 */
EAPI Elm_Object_Item *elm_colorselector_palette_color_add(Evas_Object *obj, int r, int g, int b, int a);

/**
 * @brief Clears the palette items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Colorselector object
 */
EAPI void elm_colorselector_palette_clear(Evas_Object *obj);

/**
 * @brief Gets the list of palette items.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Colorselector object
 * @return The list of color items
 */
EAPI Eina_List *elm_colorselector_palette_items_get(const Evas_Object *obj);

/**
 * @brief Sets the current palette's name.
 *
 * @since_tizen 2.3
 *
 * @remarks When the colorpalette name is set, colors are loaded from and saved to the config
 *          using the set name. If no name is set then colors are loaded from or
 *          saved to the "default" config.
 *
 * @param[in] obj The Colorselector object
 * @param[in] palette_name The name of the palette
 */
EAPI void elm_colorselector_palette_name_set(Evas_Object *obj, const char *palette_name);

/**
 * @brief Gets the current palette's name.
 *
 * @since_tizen 2.3
 *
 * @details This returns the currently set palette name using which colors are
 *          saved/loaded into the config.
 *
 * @param[in] obj The Colorselector object
 * @return The name of the palette
 */
EAPI const char *elm_colorselector_palette_name_get(const Evas_Object *obj);

/**
 * @}
 */
