/**
 * @defgroup Fonts Elementary Fonts
 * @ingroup elm_infra_group
 *
 * @brief These are functions dealing with font rendering, selection, and the
 *        like for Elementary applications. One might fetch which system
 *        fonts are there to use and set custom fonts for individual classes
 *        of UI items containing text (text classes).
 *
 * @{
 */

/**
 * @brief Structure of Elm Font Property
 */
typedef struct _Elm_Font_Properties
{
   const char *name;
   Eina_List  *styles;
} Elm_Font_Properties;

/**
 * @brief Translates a font (family) name string in the fontconfig's font names
 *        syntax into an #Elm_Font_Properties struct.
 *
 * @since_tizen 2.3
 *
 * @remarks The reverse translation can be achieved with
 *          elm_font_fontconfig_name_get(), for one style only (single font
 *          instance, not family).
 *
 * @param[in] font The font name and styles string
 * @return The font properties struct
 */
EAPI Elm_Font_Properties *elm_font_properties_get(const char *font);

/**
 * @brief Frees the font properties returned by elm_font_properties_get().
 *
 * @since_tizen 2.3
 *
 * @param[in] efp The font properties struct
 */
EAPI void             elm_font_properties_free(Elm_Font_Properties *efp);

/**
 * @brief Translates a font name, bound to a style, into the fontconfig's font names
 *        syntax.
 *
 * @since_tizen 2.3
 *
 * @remarks The reverse translation can be achieved with
 *          elm_font_properties_get(), for one style only (single font
 *          instance, not family).
 *
 * @param[in] name The font (family) name
 * @param[in] style The given style (may be @c NULL)
 *
 * @return The font name and style string
 */
EAPI char      *elm_font_fontconfig_name_get(const char *name, const char *style);

/**
 * @brief Frees the font string returned by elm_font_fontconfig_name_get().
 *
 * @since_tizen 2.3
 *
 * @param[in] name The font properties struct
 */
EAPI void             elm_font_fontconfig_name_free(char *name);

/**
 * @brief Creates a font hash table of available system fonts.
 *
 * @since_tizen 2.3
 *
 * @remarks One must call it with @a list being the return value of
 *          evas_font_available_list(). The hash is indexed by font
 *          (family) names that are its values @c Elm_Font_Properties blobs.
 *
 * @remarks The user is supposed to get it populated at least with @c 3
 *          default font families (Sans, Serif, Monospace), which should be
 *          present on most systems.
 *
 * @param[in] list The list of available system fonts, as returned by
 *             evas_font_available_list()
 * @return The font hash
 */
EAPI Eina_Hash       *elm_font_available_hash_add(Eina_List *list);

/**
 * @brief Frees the hash returned by elm_font_available_hash_add().
 *
 * @since_tizen 2.3
 *
 * @param[in] hash The hash to be freed
 */
EAPI void             elm_font_available_hash_del(Eina_Hash *hash);

/**
 * @}
 */
