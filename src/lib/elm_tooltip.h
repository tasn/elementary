/**
 * @defgroup Tooltips Tooltips
 * @ingroup elm_infra_group
 *
 * @brief The Tooltip is an (internal, for now) smart object used to show a
 *        content in a frame on mouse hover of objects(or widgets), with
 *        tips/information about them.
 *
 * @{
 */

/**
 * @brief Called back when a widget's tooltip is activated and needs content.
 *
 * @since_tizen 2.3
 *
 * @param[in] data The user-data given to elm_object_tooltip_content_cb_set()
 * @param[in] obj The owner widget
 * @param[in] tooltip The tooltip object (affix content to this!)
 */
typedef Evas_Object *(*Elm_Tooltip_Content_Cb)(void *data, Evas_Object *obj, Evas_Object *tooltip);

/**
 * @brief Called back when a widget's item tooltip is activated and needs content.
 *
 * @since_tizen 2.3
 *
 * @param[in] data The user-data given to elm_object_tooltip_content_cb_set()
 * @param[in] obj The owner widget
 * @param[in] tooltip The tooltip object (affix content to this!)
 * @param[in] item The context dependent item. As an example, if tooltip was
 *             set on elm_list item, then it is of this type.
 */
typedef Evas_Object *(*Elm_Tooltip_Item_Content_Cb)(void *data, Evas_Object *obj, Evas_Object *tooltip, void *item);

/**
 * @brief Force show tooltip of object
 *
 * @details Force show the tooltip and disable hide on mouse_out.
 *          If another content is set as tooltip, the visible tooltip will hididen and
 *          showed again with new content.
 *          This can force show more than one tooltip at a time.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Target object
 */
EAPI void        elm_object_tooltip_show(Evas_Object *obj);

/**
 * @brief Force hide tooltip of object
 *
 * @details Force hide the tooltip and (re)enable future mouse interations.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Target object
 */
EAPI void        elm_object_tooltip_hide(Evas_Object *obj);

/**
 * @brief Set the text to be displayed inside the tooltip.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The tooltip object
 * @param[in] text The text to be displayed
 *
 * @ingroup Tooltips
 */
EAPI void        elm_object_tooltip_text_set(Evas_Object *obj, const char *text);
EAPI void        elm_object_tooltip_domain_translatable_text_set(Evas_Object *obj, const char *domain, const char *text);
#define elm_object_tooltip_translatable_text_set(obj, text) elm_object_tooltip_domain_translatable_text_set((obj), NULL, (text))

/**
 * @brief Set the content to be shown in the tooltip object
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The object being attached a tooltip.
 * @param[in] func The function used to create the tooltip contents.
 * @param[in] data What to provide to @a func as callback data/context.
 * @param[in] del_cb Function called when data is not needed anymore, either when
 *        another callback replaces @p func, the tooltip is unset with
 *        elm_object_tooltip_unset() or the owner object @a obj
 *        dies. This callback receives as the first parameter the
 *        given @a data, and @c event_info is NULL.
 *
 * @details Setup the tooltip to object. The object can have only one tooltip,
 *          so any previous tooltip data is removed. @p func(with @p data) will
 *          be called every time that need show the tooltip and it should
 *          return a valid Evas_Object. This object is then managed fully by
 *          tooltip system and is deleted when the tooltip is gone.
 */
EAPI void        elm_object_tooltip_content_cb_set(Evas_Object *obj, Elm_Tooltip_Content_Cb func, const void *data, Evas_Smart_Cb del_cb);

/**
 * @brief Unset tooltip from object
 *
 * @since_tizen 2.3
 *
 * @param[in] obj Target object
 *
 * Remove tooltip from object. The callback provided as del_cb to
 * elm_object_tooltip_content_cb_set() will be called to notify it is
 * not used anymore.
 *
 * @see elm_object_tooltip_content_cb_set()
 */
EAPI void        elm_object_tooltip_unset(Evas_Object *obj);

/**
 * @brief Sets a different style for this object tooltip.
 *
 * @since_tizen 2.3
 *
 * @remarks Before you set a style you should define a tooltip with
 *          elm_object_tooltip_content_cb_set() or
 *          elm_object_tooltip_text_set().
 *
 * @param[in] obj an object with tooltip already set
 * @param[in] style The theme style to use (default, transparent, ...)
 */
EAPI void        elm_object_tooltip_style_set(Evas_Object *obj, const char *style);

/**
 * @brief Gets the style for this object tooltip.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj An object with tooltip already set
 * @return style The theme style in use, defaults to "default" \n
 *               If the object does not have a tooltip set, then @c NULL is returned.
 */
EAPI const char *elm_object_tooltip_style_get(const Evas_Object *obj);

/**
 * @brief Disables size restrictions on an object's tooltip.
 *
 * @details This function allows a tooltip to expand beyond its parent window's canvas.
 *          It will instead be limited only by the size of the display.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The tooltip's anchor object
 * @param[in] disable If @c EINA_TRUE, size restrictions are disabled,
 *                otherwise @c EINA_FALSE
 * @return  @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool   elm_object_tooltip_window_mode_set(Evas_Object *obj, Eina_Bool disable);

/**
 * @brief Retrieves size restriction state of an object's tooltip
 * @details This function returns whether a tooltip is allowed to expand beyond
 *          its parent window's canvas.
 *          It will instead be limited only by the size of the display.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The tooltip's anchor object
 * @return @c EINA_TRUE if size restrictions are disabled,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool   elm_object_tooltip_window_mode_get(const Evas_Object *obj);

/**
 * @}
 */
