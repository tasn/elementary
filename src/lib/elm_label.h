/**
 * @defgroup Label Label
 * @ingroup elm_widget_group
 *
 * @image html label_inheritance_tree.png
 * @image latex label_inheritance_tree.eps
 *
 * @brief Widget to display text, with simple html-like markup.
 *
 * The Label widget @b doesn't allow text to overflow its boundaries, if the
 * text doesn't fit the geometry of the label, it is ellipsized or
 * cut. Elementary provides several styles for this widget:
 * @li default - No animation.
 * @li marker - Centers the text in the label and makes it bold by default.
 * @li slide_long - The entire text appears from the right of the screen and
 * slides until it disappears in the left of the screen(reappearing on the
 * right again).
 * @li slide_short - The text appears in the left of the label and slides to
 * the right to show the overflow. When all of the text has been shown the
 * position is reset.
 * @li slide_bounce - The text appears in the left of the label and slides to
 * the right to show the overflow. When all of the text has been shown the
 * animation reverses, moving the text to the left.
 *
 * Custom themes can of course invent new markup tags and style them in any way
 * they like.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for label objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li @c "language,changed": The program's language is changed.
 * @li @c "slide,end": The slide has ended.
 *
 * @{
 */

/**
 * Enumeration of Elm Label Slide Mode type
 */
typedef enum
{
   ELM_LABEL_SLIDE_MODE_NONE = 0, /**< No slide effect */
   ELM_LABEL_SLIDE_MODE_AUTO, /**< Slide only if the label area is bigger than the text width length */
   ELM_LABEL_SLIDE_MODE_ALWAYS /**< Slide always */
} Elm_Label_Slide_Mode;

typedef struct _Elm_Label_Anchor_Info Elm_Label_Anchor_Info;
struct _Elm_Label_Anchor_Info
{
   char                 *name;
};

/**
 * @typedef Elm_Label_Item_Provider_Cb
 * @brief Called to provide items.
 *
 * @since_tizen 2.3
 *
 * @remarks If it returns an object handle other than @c NULL (it should create an
 *          object to do this), then this object is used to replace the current item.
 *          If not, the next provider is called until one provides an item object, or the
 *          default provider in the label does.
 * @param[in] data The data specified as the last parameter when adding the provider
 * @param[in] label The label object
 * @param[in] text A pointer to the item href string in the text
 * @return The object to be placed in the label like an icon, or another element
 * @see elm_label_item_provider_append
 * @see elm_label_item_provider_prepend
 * @see elm_label_item_provider_remove
 */
typedef Evas_Object * (*Elm_Label_Item_Provider_Cb)(void *data, Evas_Object * label, const char *item);

/**
 * @internal
 * @remarks Tizen only feature 2014.06.28
 *
 * @typedef Elm_Label_Anchor_Access_Provider_Cb
 * @brief This callback type is used to provide TTS string of an anchor.
 * @remarks If it returns a string other than NULL,
 *          then this string is used to replace the current anchor's TTS string.
 *          If not the next provider is called until one provides a string, or the
 *          default string will be read.
 *
 * @param data The data specified as the last param when adding the provider
 * @param label The label object
 * @param name A pointer to the anchor href string in the text
 * @param text A pointer to the text inside of the anchor's range.
 * @return TTS string for the anchor.
 *
 * @see elm_label_anchor_access_provider_append
 * @see elm_label_anchor_access_provider_prepend
 * @see elm_label_anchor_access_provider_remove
 */
typedef char * (*Elm_Label_Anchor_Access_Provider_Cb)(void *data, Evas_Object * label, const char *name, const char *text);
//

/**
 * @brief Adds a new label to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object                *elm_label_add(Evas_Object *parent);

/**
 * @brief Sets the wrapping behavior of the label.
 *
 * @since_tizen 2.3
 *
 * @remarks By default no wrapping is done. Possible values for @a wrap are:
 *          @li ELM_WRAP_NONE - No wrapping.
 *          @li ELM_WRAP_CHAR - Wrap between characters.
 *          @li ELM_WRAP_WORD - Wrap between words.
 *          @li ELM_WRAP_MIXED - Word wrap, and if that fails, char wrap.
 *
 * @param[in] obj The label object
 * @param[in] wrap The boolean that indicates whether to wrap the text
 */
EAPI void                        elm_label_line_wrap_set(Evas_Object *obj, Elm_Wrap_Type wrap);

/**
 * @brief Gets the wrapping behavior of the label.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The label object
 * @return The wrap type
 *
 * @see elm_label_line_wrap_set()
 */
EAPI Elm_Wrap_Type               elm_label_line_wrap_get(const Evas_Object *obj);

/**
 * @brief Sets the wrap width of the label.
 *
 * @details This function sets the maximum width size hint of the label.
 *
 * @since_tizen 2.3
 *
 * @remarks This is only relevant if the label is inside a container.
 *
 * @param[in] obj The label object
 * @param[in] w The wrap width in pixels at a minimum where words need to wrap
 */
EAPI void                        elm_label_wrap_width_set(Evas_Object *obj, Evas_Coord w);

/**
 * @brief Gets the wrap width of the label.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The label object
 * @return The wrap width in pixels at a minimum where words need to wrap
 *
 * @see elm_label_wrap_width_set()
 */
EAPI Evas_Coord                  elm_label_wrap_width_get(const Evas_Object *obj);

/**
 * @brief Sets the ellipsis behavior of the label.
 *
 * @since_tizen 2.3
 *
 * @remarks If set to @c true and the text doesn't fit in the label, ellipsis("...")
 *          are shown at the end of the widget.
 *
 * @remarks This doesn't work with slide(elm_label_slide_set()) or if the
 *          chosen wrap method is #ELM_WRAP_WORD.
 *
 * @param[in] obj The label object
 * @param[in] ellipsis The boolean value that indicates whether to add ellipsis to text
 */
EAPI void                        elm_label_ellipsis_set(Evas_Object *obj, Eina_Bool ellipsis);

/**
 * @brief Gets the ellipsis behavior of the label.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The label object
 * @return @c true if ellipsis are shown at the end of the label area,
 *         otherwise @c false
 *
 * @see elm_label_ellipsis_set()
 */
EAPI Eina_Bool                   elm_label_ellipsis_get(const Evas_Object *obj);

/**
 * @brief Sets the slide duration (speed) of the label.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The label object
 * @param[in] duration The duration in seconds in moving text from slide begin position
 *                 to slide end position
 */
EAPI void                        elm_label_slide_duration_set(Evas_Object *obj, double duration);

/**
 * @brief Gets the slide duration(speed) of the label.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The label object
 * @return The duration time in moving text from slide begin position to slide end position
 *
 * @see elm_label_slide_duration_set()
 */
EAPI double                      elm_label_slide_duration_get(const Evas_Object *obj);

/**
 * @brief Sets the slide mode of the label widget.
 *
 * @since_tizen 2.3
 *
 * @remarks elm_label_slide_mode_set() changes the label slide mode.
 *          By default, the slide mode is none. Possible values for @a mode are:
 *          @li ELM_LABEL_SLIDE_MODE_NONE - No slide effect.
 *          @li ELM_LABEL_SLIDE_MODE_AUTO - Slide only if the label area is bigger than
 *                                          the text width length.
 *          @li ELM_LABEL_SLIDE_MODE_ALWAYS -Slide always.
 *
 * @remarks @c ELM_LABEL_SLIDE_MODE_AUTO and @c ELM_LABEL_SLIDE_MODE_ALWAYS only works
 *          with the themes "slide_short", "slide_long", and "slide_bounce".
 * @remarks @c ELM_LABEL_SLIDE_MODE_AUTO and @c ELM_LABEL_SLIDE_MODE_ALWAYS don't work
 *          if the line wrap(elm_label_line_wrap_set()) or
 *          ellipsis(elm_label_ellipsis_set()) is set.
 *
 * @param[in] obj The label object
 * @param[in] mode The slide mode
 *
 * @see elm_label_slide_mode_get()
 */
EAPI void                        elm_label_slide_mode_set(Evas_Object *obj, Elm_Label_Slide_Mode mode);

/**
 * @brief Gets the slide mode of the label widget.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The label object
 * @return The slide mode
 *
 * @see elm_label_slide_mode_set()
 */
EAPI Elm_Label_Slide_Mode        elm_label_slide_mode_get(const Evas_Object *obj);

/**
 * @brief Starts the slide effect.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The label object
 *
 * @see elm_label_slide_mode_set()
 */
EAPI void                        elm_label_slide_go(Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature 2013.10.28: Support item, anchor formats
 *
 * @brief Appends a custom item provider to the list for that label.
 *
 * @details This appends the given callback. The list is walked from beginning to end
 *          with each function called, given the item href string in the text. If the
 *          function returns an object handle other than @c NULL (it should create an
 *          object to do this), then this object is used to replace that item. If
 *          not, the next provider is called until one provides an item object, or the
 *          default provider in the label does.
 *
 * @param obj The label object
 * @param func The function called to provide the item object
 * @param data The data passed to @a func
 */
EAPI void               elm_label_item_provider_append(Evas_Object *obj, Elm_Label_Item_Provider_Cb func, void *data);

/**
 * @internal
 * @remarks Tizen only feature 2013.10.28: Support item, anchor formats
 *
 * @brief Prepends a custom item provider to the list for that label.
 *
 * @details This prepends the given callback. See elm_label_item_provider_append() for
 *          more information.
 *
 * @param obj The label object
 * @param func The function called to provide the item object
 * @param data The data passed to @a func
 */
EAPI void               elm_label_item_provider_prepend(Evas_Object *obj, Elm_Label_Item_Provider_Cb func, void *data);

/**
 * @internal
 * @remarks Tizen only feature 2013.10.28: Support item, anchor formats
 *
 * @brief Removes a custom item provider to the list for that label.
 *
 * @details This removes the given callback. See elm_label_item_provider_append() for
 *          more information.
 *
 * @param obj The label object
 * @param func The function called to provide the item object
 * @param data The data passed to @a func
 */
EAPI void               elm_label_item_provider_remove(Evas_Object *obj, Elm_Label_Item_Provider_Cb func, void *data);

/**
 * @internal
 * @remarks Tizen only feature 2014.06.28
 *
 * @brief This appends a custom anchor access provider to the list for that label
 *
 * @remarks This appends the given callback. The list is walked from beginning to end
 *          with each function called given the anchor href string in the text. If the
 *          function returns a string other than NULL, then this string is used
 *          to replace that TTS string.
 *          If not the next provider is called until one provides a string, or the
 *          default TTS string will be read.
 *
 * @param obj The label object
 * @param func The function called to provide the TTS string
 * @param data The data passed to @p func
 */
EAPI void               elm_label_anchor_access_provider_append(Evas_Object *obj, Elm_Label_Anchor_Access_Provider_Cb func, void *data);

/**
 * @internal
 * @remarks Tizen only feature 2014.06.28
 *
 * @brief This prepends a custom anchor access provider to the list for that label
 *
 * @remarks This prepends the given callback.
 *
 * @param obj The label object
 * @param func The function called to provide the TTS string
 * @param data The data passed to @p func
 *
 * @see elm_label_anchor_access_provider_append().
 */
EAPI void               elm_label_anchor_access_provider_prepend(Evas_Object *obj, Elm_Label_Anchor_Access_Provider_Cb func, void *data);

/**
 * @internal
 * @remarks Tizen only feature 2014.06.28
 *
 * @brief This removes a custom anchor access provider to the list for that label
 *
 * @remarks This removes the given callback. 
 *
 * @see elm_label_anchor_access_provider_append().
 *
 * @param obj The label object
 * @param func The function called to provide the TTS string
 * @param data The data passed to @p func
 */
EAPI void               elm_label_anchor_access_provider_remove(Evas_Object *obj, Elm_Label_Anchor_Access_Provider_Cb func, void *data);

/**
 * @}
 */
