/**
 * @defgroup Theme Theme
 * @ingroup elm_infra_group
 * @brief This group provides functions to handle look and feel of Elementary
 *        Widgets.
 *
 * Elementary uses Edje to theme its widgets, naturally. But for most of the
 * part this is hidden behind a simpler interface that lets the user set
 * extensions and choose the style of widgets in a much easier way.
 *
 * Instead of thinking in terms of paths to Edje files and their groups
 * each time you want to change the appearance of a widget, Elementary
 * works so you can add any theme file with extensions or replace the
 * main theme at one point in the application, and then just set the style
 * of widgets with elm_object_style_set() and related functions. Elementary
 * then looks in its list of themes for a matching group and applies it,
 * and when the theme changes midway through the application, all widgets
 * are updated accordingly.
 *
 * There are three concepts you need to know to understand how Elementary
 * themes work: default theme, extensions, and overlays.
 *
 * Default theme, obviously, is the one that provides the default
 * look for all widgets. End users can change the theme used by Elementary
 * by setting the @c ELM_THEME environment variable before running an
 * application, or globally for all programs using the @c elementary_config
 * utility. Applications can change the default theme using elm_theme_set(),
 * but this can go against the user's wishes, so it's not an advised practice.
 *
 * Ideally, applications should find everything they need in the already
 * provided theme, but there may be occasions when that's not enough and
 * custom styles are required to correctly express the idea. For such
 * cases, Elementary has extensions.
 *
 * Extensions allow the application developer to write styles of its own
 * to apply to some widgets. This requires knowledge of how each widget
 * is themed, as extensions always replace the entire group used by
 * the widget, so important signals and parts need to be there for the
 * object to behave properly (see documentation of Edje for details).
 * Once the theme for the extension is done, the application needs to add
 * it to the list of themes that Elementary will look into, using
 * elm_theme_extension_add(), and set the style of the desired widgets as
 * he would normally do using elm_object_style_set().
 *
 * Overlays, on the other hand, can replace the look of all the widgets by
 * overriding the default style. Like extensions, it's up to the application
 * developer to write the theme for the widgets it wants, the difference
 * being that when looking for the theme, Elementary checks first the
 * list of overlays, then the set theme, and lastly the list of extensions,
 * so with overlays it's possible to replace the default view and every
 * widget is affected. This is very much similar to setting the whole
 * theme for the application and probably clashing with the end user
 * options, not to mention the risk of ending up with non-matching styles
 * across the program. Unless there's a very special reason to use them,
 * overlays should be avoided for the reasons mentioned previously.
 *
 * All these theme lists are handled by ::Elm_Theme instances. Elementary
 * keeps one default theme internally and every function that receives one of
 * these can be called with @c NULL to refer to this default theme(except for
 * elm_theme_free()). It's possible to create a new instance of a
 * ::Elm_Theme to set other themes for a specific widget (and all of its
 * children), but this is as bad as, if not more than, using
 * overlays. Don't use this unless you really know what you are doing.
 *
 * @{
 */
/**
 * @typedef Elm_Theme
 *
 * @brief The structure type which is an opaque handler for the list of themes that Elementary looks for when
 *        rendering widgets.
 *
 * @since_tizen 2.3
 *
 * @remarks Avoid this unless you really know what you are doing. For most
 *          cases, sticking to the default is all a developer needs.
 */
typedef struct _Elm_Theme Elm_Theme;

/**
 * @brief Creates a new specific theme.
 *
 * @details This creates an empty specific theme that only uses the default theme. A
 *          specific theme has its own private set of extensions and overlays
 *          (which are empty by default). Specific themes do not fall back to themes
 *          of parent objects. They are not intended for this use. Use styles, overlays,
 *          and extensions when needed, but avoid specific themes unless there is no
 *          other way (example: you want to have a preview of a new theme you are
 *          selecting in a "theme selector" window. The preview is inside a scroller
 *          and should display how the theme you selected will look, but should not
 *          actually apply it. The child of the scroller have a specific
 *          theme set to show this preview before the user decides to apply it to all
 *          applications).
 *
 * @return A new theme
 *
 * @since_tizen 2.3
 */
EAPI Elm_Theme       *elm_theme_new(void);

/**
 * @brief Frees a specific theme.
 *
 * @details This frees a theme created with elm_theme_new().
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme to free
 */
EAPI void             elm_theme_free(Elm_Theme *th);

/**
 * @brief Copies the theme from the source to the destination theme.
 *
 * @since_tizen 2.3
 *
 * @remarks This makes a one-time static copy of all the theme config, extensions
 *          and overlays from @a th to @a thdst. If @a th references a theme, then
 *          @a thdst is also set to reference it, with all the theme settings,
 *          overlays, and extensions that @a th has.
 *
 * @param[in] th The source theme to copy from
 * @param[out] thdst The destination theme to copy data to
 */
EAPI void             elm_theme_copy(Elm_Theme *th, Elm_Theme *thdst);

/**
 * @brief Sets the source theme to reference the ref theme.
 *
 * @since_tizen 2.3
 *
 * @remarks This clears @a th to be empty and then sets it to refer to @a thref
 *          so @a th acts as an override to @a thref, but where its overriding
 *          doesn't apply, it falls through to @a thref for configuration.
 *
 * @param[in] th The theme that does the referencing
 * @param[out] thref The theme that is the reference source
 */
EAPI void             elm_theme_ref_set(Elm_Theme *th, Elm_Theme *thref);

/**
 * @brief Gets the theme referred to.
 *
 * @details This gets the theme set as the reference theme by elm_theme_ref_set().
 *          If no theme is set as a reference, @c NULL is returned.
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme to get the reference from
 * @return The referenced theme handle
 */
EAPI Elm_Theme       *elm_theme_ref_get(Elm_Theme *th);

/**
 * @brief Gets the default theme.
 *
 * @since_tizen 2.3
 *
 * @remarks This returns the internal default theme setup handle that all widgets
 *          use implicitly unless a specific theme is set. This is also often used
 *          as a shorthand for @c NULL.
 *
 * @return The default theme handle
 */
EAPI Elm_Theme       *elm_theme_default_get(void);

/**
 * @brief Prepends a theme overlay to the list of overlays.
 *
 * @since_tizen 2.3
 *
 * @remarks Use this if your application needs to provide some custom overlay theme
 *          (An Edje file that replaces some default styles of widgets) where adding
 *          new styles, or changing system theme configuration is not possible. Do
 *          NOT use this instead of a proper system theme configuration. Use proper
 *          configuration files, profiles, environment variables etc., to set a theme
 *          so that the theme can be altered by simple configuration by a user. Using
 *          this call to achieve that effect is overusing the API and creates lots
 *          of issues.
 *
 * @param[in] th The theme to add to, otherwise @c NULL for the default theme
 * @param[in] item The Edje file path to be used
 *
 * @see elm_theme_extension_add()
 */
EAPI void             elm_theme_overlay_add(Elm_Theme *th, const char *item);

/**
 * @brief Deletes a theme overlay from the list of overlays.
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme to delete, otherwise @c NULL for the default theme
 * @param[in] item The name of the theme overlay
 *
 * @see elm_theme_overlay_add()
 */
EAPI void             elm_theme_overlay_del(Elm_Theme *th, const char *item);

/**
 * @brief Gets the list of registered overlays for the given theme.
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme from which to get the overlays
 * @return The list of theme overlays, do not free it
 *
 * @see elm_theme_overlay_add()
 */
EAPI const Eina_List *elm_theme_overlay_list_get(const Elm_Theme *th);

/**
 * @brief Appends a theme extension to the list of extensions.
 *
 * @since_tizen 2.3
 *
 * @remarks This is intended when an application needs more styles of widgets or new
 *          widget themes that the default does not provide (or may not provide). The
 *          application has "extended" usage by coming up with new custom style names
 *          for widgets for specific uses, but as these are not "standard", they are
 *          not guaranteed to be provided by a default theme. This means the
 *          application is required to provide these extra elements itself in specific
 *          Edje files. This call adds one of those Edje files to the theme search
 *          path to be search after the default theme. The use of this call is
 *          encouraged when default styles do not meet the needs of the application.
 *          Use this call instead of elm_theme_overlay_add() for almost all cases.
 *
 * @param[in] th The theme to add, otherwise @c NULL for the default theme
 * @param[in] item The Edje file path to be used
 *
 * @see elm_object_style_set()
 */
EAPI void             elm_theme_extension_add(Elm_Theme *th, const char *item);

/**
 * @brief Deletes a theme extension from the list of extensions.
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme to delete, otherwise @c NULL for the default theme
 * @param[in] item The name of the theme extension
 *
 * @see elm_theme_extension_add()
 */
EAPI void             elm_theme_extension_del(Elm_Theme *th, const char *item);

/**
 * @brief Gets the list of registered extensions for the given theme.
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme from which to get the extensions
 * @return The list of theme extensions, do not free it
 *
 * @see elm_theme_extension_add()
 */
EAPI const Eina_List *elm_theme_extension_list_get(const Elm_Theme *th);

/**
 * @brief Sets the theme search order for the given theme.
 *
 * @details This sets the search string for the theme in the path-notation from the first
 *          theme to search, till the last, delimited by the : character. Example:
 *
 *          "shiny:/path/to/file.edj:default"
 *
 * @since_tizen 2.3
 *
 * @remarks See the ELM_THEME environment variable for more information.
 *
 * @param[in] th The theme to set the search order for, otherwise @c NULL for the default theme
 * @param[in] theme The theme search string
 *
 * @see elm_theme_get()
 * @see elm_theme_list_get()
 */
EAPI void             elm_theme_set(Elm_Theme *th, const char *theme);

/**
 * @brief Gets the theme search order.
 *
 * @since_tizen 2.3
 *
 * @remarks This function returns a colon separated string of theme elements as
 *          returned by elm_theme_list_get().
 *
 * @param[in] th The theme to get the search order for, otherwise @c NULL for the default theme
 * @return The internal search order path
 *
 * @see elm_theme_set()
 * @see elm_theme_list_get()
 */
EAPI const char      *elm_theme_get(Elm_Theme *th);

/**
 * @brief Gets a list of theme elements to be used in a theme.
 *
 * @details This returns the internal list of theme elements (will only be valid as
 *          long as the theme is not modified by elm_theme_set() or theme is not
 *          freed by elm_theme_free(). This is a list of strings which must not be
 *          altered as they are also internal. If @a th is @c NULL, then the default
 *          theme element list is returned.
 *
 * @since_tizen 2.3
 *
 * @remarks A theme element can consist of a full or relative path to a .edj file,
 *          or a name, without extension, for a theme to be searched in the known
 *          theme paths for Elementary.
 *
 * @param[in] th The theme to get the list of theme elements from
 * @return The internal list of theme elements
 *
 * @see elm_theme_set()
 * @see elm_theme_get()
 */
EAPI const Eina_List *elm_theme_list_get(const Elm_Theme *th);

/**
 * @brief Gets the full path for a theme element.
 *
 * @since_tizen 2.3
 *
 * @remarks This returns a string you should free with free() on success, @c NULL on
 *          failure. This searches for the given theme element, and if it is a
 *          full or relative path element or a simple searchable name. The returned
 *          path is the full path to the file, if searched, and the file exists, or it
 *          is simply the full path given in the element or a resolved path if
 *          relative to home. The @a in_search_path boolean that it is pointing to is set to
 *          @c EINA_TRUE if the file is a searchable file and is in the search path,
 *          otherwise the value is @c EINA_FALSE.
 *
 * @param[in] f The theme element name
 * @param[out] in_search_path The pointer to a boolean that indicate if the item is in the search path
 * @return The full path to the file found
 */
EAPI char            *elm_theme_list_item_path_get(const char *f, Eina_Bool *in_search_path);

/**
 * @brief Flushes the current theme.
 *
 * @details This flushes caches that let elementary know where to find theme elements
 *          in the given theme. If @a th is @c NULL, then the default theme is flushed.
 *          Call this function if the source theme data has changed in such a way so as to
 *          make any caches Elementary invalid.
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme to flush
 */
EAPI void             elm_theme_flush(Elm_Theme *th);

/**
 * @brief Flushes all themes (default and specific ones).
 *
 * @details This flushes all the themes in the current application context, by calling
 *          elm_theme_flush() on each of them.
 *
 * @since_tizen 2.3
 */
EAPI void             elm_theme_full_flush(void);

/**
 * @brief Returns a list of theme elements in the theme search path.
 *
 * @details This lists all the available theme files in the standard Elementary search path
 *          for the theme elements, and returns them in an alphabetical order as theme
 *          element names in a list of strings. Free this with
 *          elm_theme_name_available_list_free() when you are done with the list.
 *
 * @since_tizen 2.3
 *
 * @return The list of strings that are the theme element names
 */
EAPI Eina_List       *elm_theme_name_available_list_new(void);

/**
 * @brief Frees the list returned by elm_theme_name_available_list_new().
 *
 * @details This frees the list of themes returned by
 *          elm_theme_name_available_list_new(). Once freed the list should no longer
 *          be used. A new list must be created.
 *
 * @param[in] list The list to be freed
 *
 * @since_tizen 2.3
 */
EAPI void             elm_theme_name_available_list_free(Eina_List *list);

/**
 * @brief Sets a specific theme to be used for this object and its children.
 *
 * @details This sets a specific theme that is used for the given object and any
 *          child objects it has. If @a th is @c NULL then the theme to be used is
 *          cleared and the object inherits its theme from its parent (which
 *          ultimately uses the default theme if no specific themes are set).
 *
 * @since_tizen 2.3
 *
 * @remarks Use special themes with great care as this annoys users and makes
 *          configuration difficult. Avoid any custom themes if at all possible.
 *
 * @param[in] obj The object to set the theme on
 * @param[in] th The theme to set
 */
EAPI void             elm_object_theme_set(Evas_Object *obj, Elm_Theme *th);

/**
 * @brief Gets the specific theme to be used.
 *
 * @details This returns a specific theme set, otherwise @c NULL if no specific theme is
 *          set on that object. It returns inherited themes from parents, only
 *          the specific theme set for that specific object. See elm_object_theme_set()
 *          for more information.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object to get the specific theme from
 * @return The specific theme set
 */
EAPI Elm_Theme       *elm_object_theme_get(const Evas_Object *obj);

/**
 * @brief Gets a data item from a theme.
 *
 * @details This function is used to return data items from edc in @a th, an overlay, or an extension.
 *          It works in the same way as edje_file_data_get() except that the returned value is stringshared.
 *
 * @since_tizen 2.3
 *
 * @param[in] th The theme, otherwise @c NULL for the default theme
 * @param[in] key The data key to search with
 * @return The data value, otherwise @c NULL on failure
 */
EAPI const char      *elm_theme_data_get(Elm_Theme *th, const char *key);

/**
 * @}
 */
