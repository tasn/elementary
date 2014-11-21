/**
 * @defgroup Entry Entry
 * @ingroup elm_widget_group
 *
 * @image html entry_inheritance_tree.png
 * @image latex entry_inheritance_tree.eps
 *
 * @brief An entry is a convenience widget that shows a box in which the user
 *       can enter text.
 *
 * Entries by default don't scroll, so they grow to accommodate the entire
 * text, resizing the parent window as needed. This can be changed with the
 * elm_entry_scrollable_set() function.
 *
 * They can also be single line or multi line (the default) and when set
 * to the multi line mode they support text wrapping in any of the modes
 * indicated by #Elm_Wrap_Type.
 *
 * Other features include the password mode, filtering of inserted text with
 * elm_entry_markup_filter_append() and related functions, inline "items" and
 * formatted markup text.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for entry objects (@since 1.8).
 *
 * This widget implements the elm-scrollable-interface
 * interface, so that all (non-deprecated) functions for the base
 * @ref Scroller widget also work for entries (@since 1.8).
 *
 * Some calls on the entry's API are marked as @b deprecated, as they
 * just wrap the scrollable widgets counterpart functions. Use the
 * ones mentioned for each case of deprecation here.
 * Eventually the deprecated ones are discarded (next major
 * release).
 *
 * @section entry-markup Formatted text
 *
 * The markup tags supported by the Entry are defined by the theme, but
 * even while writing new themes or extensions it's a good idea to stick to
 * a sane default, to maintain coherency and avoid application breakages.
 * Currently the tags defined by the default theme are as follows:
 * @li \<br\>: Inserts a line break.
 * @li \<ps\>: Inserts a paragraph separator. This is preferred over line
 * breaks.
 * @li \<tab\>: Inserts a tab.
 * @li \<em\>...\</em\>: Emphasizes text. Sets the @em oblique style for the
 * enclosed text.
 * @li \<b\>...\</b\>: Sets the @b bold style for the enclosed text.
 * @li \<link\>...\</link\>: Underlines the enclosed text.
 * @li \<hilight\>...\</hilight\>: Highlights the enclosed text.
 *
 * @section entry-special Special markups
 *
 * Besides those used to format text, entries support two special markup
 * tags used to insert clickable portions of text or items inlined within
 * text.
 *
 * @subsection entry-anchors Anchors
 *
 * Anchors are similar to HTML anchors. Text can be surrounded by \<a\> and
 * \</a\> and an event is generated when this text is clicked,
 * for example:
 *
 * @code
 * This text is outside <a href=anc-01>but this one is an anchor</a>
 * @endcode
 *
 * The @c href attribute in the opening tag gives the name that is
 * used to identify the anchor. It can be any valid UTF8 string.
 *
 * When an anchor is clicked, an @c "anchor,clicked" signal is emitted with
 * #Elm_Entry_Anchor_Info in the @a event_info parameter for the
 * callback function. The same applies for the "anchor,in" (mouse in), "anchor,out"
 * (mouse out), "anchor,down" (mouse down), and "anchor,up" (mouse up) events on
 * an anchor.
 *
 * @subsection entry-items Items
 *
 * Inlined in the text, any other @c Evas_Object can be inserted by using
 * \<item\> tags, for example:
 *
 * @code
 * <item size=16x16 vsize=full href=emoticon/haha></item>
 * @endcode
 *
 * Just like with anchors, the @c href identifies each item. However, these need
 * to additionally indicate their size, which is done using either the
 * @c size, @c absize, or @c relsize attribute. These attributes take their
 * value in the WxH format, where W is the width and H is the height of the
 * item.
 *
 * @li absize: Absolute pixel size for the item. Whatever value is set
 * becomes the item's size regardless of any scale value the object may have
 * been set to. The final line height is adjusted to fit larger items.
 * @li size: Similar to @c absize, but it's adjusted to the scale value set
 * for the object.
 * @li relsize: Size is adjusted for the item to fit within the current
 * line height.
 *
 * Besides their size, items are given a @c vsize value that affects
 * how their final size and position is calculated. The possible values
 * are:
 * @li ascent: Item is placed within the line's baseline and its
 * ascent. That is, the height between the line where all characters are
 * positioned and the highest point in the line. For @c size and @c absize
 * items, the descent value is added to the total line height to make
 * them fit. @c relsize items are adjusted to fit within this space.
 * @li full: Items are placed between the descent and ascent, or the
 * lowest and highest point in the line.
 *
 * The next image shows different configurations of items and how
 * the previously mentioned options affect their sizes. In all cases,
 * the green line indicates the ascent, blue indicates the baseline, and red indicates
 * the descent.
 *
 * @image html entry_item.png
 * @image latex entry_item.eps "entry item" width=\textwidth
 *
 * And another one to show how size differs from @c absize. In the first one,
 * the scale value is set to @c 1.0, while the second one is using @c 2.0.
 *
 * @image html entry_item_scale.png
 * @image latex entry_item_scale.eps "entry item scale" width=\textwidth
 *
 * After the size for an item is calculated, the entry requests an
 * object to place in its space. For this, the functions set with
 * elm_entry_item_provider_append() and other related functions are called
 * in order until one of them returns a non-NULL value. If no providers
 * are available, or all of them return @c NULL, then the entry falls back
 * to one of the internal defaults, provided the name matches one of
 * them.
 *
 * All of the following are currently supported:
 *
 * - emoticon/angry
 * - emoticon/angry-shout
 * - emoticon/crazy-laugh
 * - emoticon/evil-laugh
 * - emoticon/evil
 * - emoticon/goggle-smile
 * - emoticon/grumpy
 * - emoticon/grumpy-smile
 * - emoticon/guilty
 * - emoticon/guilty-smile
 * - emoticon/haha
 * - emoticon/half-smile
 * - emoticon/happy-panting
 * - emoticon/happy
 * - emoticon/indifferent
 * - emoticon/kiss
 * - emoticon/knowing-grin
 * - emoticon/laugh
 * - emoticon/little-bit-sorry
 * - emoticon/love-lots
 * - emoticon/love
 * - emoticon/minimal-smile
 * - emoticon/not-happy
 * - emoticon/not-impressed
 * - emoticon/omg
 * - emoticon/opensmile
 * - emoticon/smile
 * - emoticon/sorry
 * - emoticon/squint-laugh
 * - emoticon/surprised
 * - emoticon/suspicious
 * - emoticon/tongue-dangling
 * - emoticon/tongue-poke
 * - emoticon/uh
 * - emoticon/unhappy
 * - emoticon/very-sorry
 * - emoticon/what
 * - emoticon/wink
 * - emoticon/worried
 * - emoticon/wtf
 *
 * Alternatively, an item may reference an image by its path, using
 * the URI form @c file:///path/to/an/image.png and the entry then
 * uses that image for the item.
 *
 * @section entry-style-set Setting entry's style
 *
 * There are 2 major ways to change the entry's style:
 * - Theme - Set the "base" field to the desired style.
 * - User style - Pushing overrides the theme style to the textblock object by using evas_object_textblock_style_user_push().
 *
 * You should modify the theme when you want to change the style for
 * aesthetic reasons. While the user style should be changed when you want
 * to change the style to something specifically defined at run-time, e.g,
 * setting font or font size in a text editor.
 *
 * @section entry-files Loading and saving files
 *
 * Entries have convenience functions to load text from a file and save
 * changes back to it after a short delay. The automatic saving is enabled
 * by default, but can be disabled with elm_entry_autosave_set(). Files
 * can be loaded directly as plain text or can have any recognized markup in them.
 * See elm_entry_file_set() for more details.
 *
 * @section entry-signals Emitted signals
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li "changed": The text within the entry is changed.
 * @li "changed,user": The text within the entry is changed because of user interaction.
 * @li "activated": The enter key is pressed on a single line entry.
 * @li "aborted": The escape key is pressed on a single line entry. (since 1.7)
 * @li "press": A mouse button has been pressed on the entry.
 * @li "longpressed": A mouse button has been pressed and held for a couple of
 * seconds.
 * @li "clicked": The entry has been clicked (mouse press and release).
 * @li "clicked,double": The entry has been double clicked.
 * @li "clicked,triple": The entry has been triple clicked.
 * @li "focused": The entry has received focus.
 * @li "unfocused": The entry has lost focus.
 * @li "selection,paste": A paste of the clipboard contents has been requested.
 * @li "selection,copy": A copy of the selected text into the clipboard has been
 * requested.
 * @li "selection,cut": A cut of the selected text into the clipboard has been
 * requested.
 * @li "selection,start": A selection has begun and no previous selection
 * existed.
 * @li "selection,changed": The current selection has changed.
 * @li "selection,cleared": The current selection has been cleared.
 * @li "cursor,changed": The cursor has changed position.
 * @li "anchor,clicked": An anchor has been clicked. The @a event_info
 * parameter for the callback is #Elm_Entry_Anchor_Info.
 * @li "anchor,in": The mouse cursor has moved into an anchor. The @a event_info
 * parameter for the callback is #Elm_Entry_Anchor_Info.
 * @li "anchor,out": The mouse cursor has moved out of an anchor. The @a event_info
 * parameter for the callback is #Elm_Entry_Anchor_Info.
 * @li "anchor,up": The mouse button has been unpressed on an anchor. The @a event_info
 * parameter for the callback is #Elm_Entry_Anchor_Info.
 * @li "anchor,down": The muse button has been pressed on an anchor. The @a event_info
 * parameter for the callback is #Elm_Entry_Anchor_Info.
 * @li "preedit,changed": The preedit string has changed.
 * @li "language,changed": The program language has changed.
 *
 * The default content parts of the entry items that you can use are:
 * @li "icon" - An icon in the entry
 * @li "end" - Content at the end of the entry
 *
 * The default text parts of the entry that you can use are:
 * @li "default" - Text of the entry
 *
 * Supported common elm_object APIs.
 * @li @ref elm_object_signal_emit
 * @li @ref elm_object_part_text_set
 * @li @ref elm_object_part_text_get
 * @li @ref elm_object_part_content_set
 * @li @ref elm_object_part_content_get
 * @li @ref elm_object_part_content_unset
 * @li @ref elm_object_signal_callback_add
 * @li @ref elm_object_signal_callback_del
 * @li @ref elm_object_disabled_set
 * @li @ref elm_object_disabled_get
 *
 * @{
 */

/**
 * @typedef Elm_Text_Format
 *
 * @brief Enumeration that defines the text format types.
 *
 * @see elm_entry_file_set()
 */
typedef enum
{
   ELM_TEXT_FORMAT_PLAIN_UTF8,  /**< Plain UTF8 type */
   ELM_TEXT_FORMAT_MARKUP_UTF8  /**< Markup UTF8 type */
} Elm_Text_Format;

/**
 * @typedef Elm_Wrap_Type
 *
 * @brief Enumeration that defines the line wrapping types.
 *
 * @see elm_entry_line_wrap_set()
 */
typedef enum
{
   ELM_WRAP_NONE = 0, /**< No wrap - value is zero */
   ELM_WRAP_CHAR,     /**< Char wrap - wrap between characters */
   ELM_WRAP_WORD,     /**< Word wrap - wrap within the allowed wrapping points (as defined in the unicode standard) */
   ELM_WRAP_MIXED,    /**< Mixed wrap - Word wrap, if that fails, char wrap */
   ELM_WRAP_LAST
} Elm_Wrap_Type; /**< Type of word or character wrapping to use */

/**
 * @typedef Elm_Input_Panel_Layout
 *
 * @brief Enumeration that defines the input panel (virtual keyboard) layout types.
 *
 * @see elm_entry_input_panel_layout_set()
 */
typedef enum
{
   ELM_INPUT_PANEL_LAYOUT_NORMAL,      /**< Default layout */
   ELM_INPUT_PANEL_LAYOUT_NUMBER,      /**< Number layout */
   ELM_INPUT_PANEL_LAYOUT_EMAIL,       /**< Email layout */
   ELM_INPUT_PANEL_LAYOUT_URL,         /**< URL layout */
   ELM_INPUT_PANEL_LAYOUT_PHONENUMBER, /**< Phone number layout */
   ELM_INPUT_PANEL_LAYOUT_IP,          /**< IP layout */
   ELM_INPUT_PANEL_LAYOUT_MONTH,       /**< Month layout */
   ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,  /**< Number only layout */
   ELM_INPUT_PANEL_LAYOUT_INVALID,     /**< Invalid layout(not recommended) */
   ELM_INPUT_PANEL_LAYOUT_HEX,         /**< Hexadecimal layout */
   ELM_INPUT_PANEL_LAYOUT_TERMINAL,    /**< Command-line terminal layout including the esc, alt, ctrl key and so on (no auto-correct, no auto-capitalization) */
   ELM_INPUT_PANEL_LAYOUT_PASSWORD,    /**< Like normal, but no auto-correct, no auto-capitalization */
   ELM_INPUT_PANEL_LAYOUT_DATETIME,    /**< Date and time layout @since 1.8 */
   ELM_INPUT_PANEL_LAYOUT_EMOTICON     /**< Emoticon layout @since 1.10 */
} Elm_Input_Panel_Layout; /**< Type of input panel (virtual keyboard) to use - this is a hint and may not provide exactly what is desired */

/**
 * @typedef Elm_Input_Hints
 * @brief Enumeration that defines the types of Input Hints.
 * @since 1.12
 */
typedef enum
{
   ELM_INPUT_HINT_NONE                = 0,        /**< No active hints @since 1.12 */
   ELM_INPUT_HINT_AUTO_COMPLETE       = 1 << 0,   /**< suggest word auto completion @since 1.12 */
   ELM_INPUT_HINT_SENSITIVE_DATA      = 1 << 1,   /**< typed text should not be stored. @since 1.12 */
} Elm_Input_Hints;

enum
{
   ELM_INPUT_PANEL_LAYOUT_NORMAL_VARIATION_NORMAL,          /**< The plain normal layout @since 1.12 */
   ELM_INPUT_PANEL_LAYOUT_NORMAL_VARIATION_FILENAME,        /**< Filename layout. Symbols such as '/' should be disabled. @since 1.12 */
   ELM_INPUT_PANEL_LAYOUT_NORMAL_VARIATION_PERSON_NAME      /**< The name of a person. @since 1.12 */
};

enum
{
   ELM_INPUT_PANEL_LAYOUT_NUMBERONLY_VARIATION_NORMAL,              /**< The plain normal number layout @since 1.8 */
   ELM_INPUT_PANEL_LAYOUT_NUMBERONLY_VARIATION_SIGNED,              /**< The number layout to allow a positive or negative sign at the start @since 1.8 */
   ELM_INPUT_PANEL_LAYOUT_NUMBERONLY_VARIATION_DECIMAL,             /**< The number layout to allow decimal point to provide fractional value @since 1.8 */
   ELM_INPUT_PANEL_LAYOUT_NUMBERONLY_VARIATION_SIGNED_AND_DECIMAL   /**< The number layout to allow decimal point and negative sign @since 1.8 */
};

enum
{
   ELM_INPUT_PANEL_LAYOUT_PASSWORD_VARIATION_NORMAL,    /**< The normal password layout @since 1.12 */
   ELM_INPUT_PANEL_LAYOUT_PASSWORD_VARIATION_NUMBERONLY /**< The password layout to allow only number @since 1.12 */
};

/**
 * @typedef Elm_Input_Panel_Lang
 *
 * @brief Enumeration that defines the input panel (virtual keyboard) language modes.
 *
 * @see elm_entry_input_panel_language_set()
 */
typedef enum
{
   ELM_INPUT_PANEL_LANG_AUTOMATIC,    /**< Automatic language mode */
   ELM_INPUT_PANEL_LANG_ALPHABET      /**< Alphabet language mode */
} Elm_Input_Panel_Lang;

/**
 * @typedef Elm_Autocapital_Type
 *
 * @brief Enumeration that defines the autocapitalization types.
 *
 * @see elm_entry_autocapital_type_set()
 */
typedef enum
{
   ELM_AUTOCAPITAL_TYPE_NONE,         /**< No autocapitalization when typing */
   ELM_AUTOCAPITAL_TYPE_WORD,         /**< Autocapitalize each typed word */
   ELM_AUTOCAPITAL_TYPE_SENTENCE,     /**< Autocapitalize the start of each sentence */
   ELM_AUTOCAPITAL_TYPE_ALLCHARACTER, /**< Autocapitalize all letters */
} Elm_Autocapital_Type; /**< Choose a method of autocapitalization */

/**
 * @typedef Elm_Input_Panel_Return_Key_Type
 *
 * @brief Enumeration that defines the "Return" key types on the input panel (virtual keyboard).
 *
 * @see elm_entry_input_panel_return_key_type_set()
 */
typedef enum
{
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT, /**< Default key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE,    /**< Done key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_GO,      /**< Go key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_JOIN,    /**< Join key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_LOGIN,   /**< Login key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT,    /**< Next key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH,  /**< Search string or magnifier icon key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEND,    /**< Send key type */
   ELM_INPUT_PANEL_RETURN_KEY_TYPE_SIGNIN   /**< Sign-in key type @since 1.8 */
} Elm_Input_Panel_Return_Key_Type;

/**
 * @typedef Elm_Entry_Anchor_Info
 *
 * @brief The information sent in the callback for the "anchor,clicked" signals emitted
 *        by entries.
 */
typedef struct _Elm_Entry_Anchor_Info Elm_Entry_Anchor_Info;

/**
 * @struct _Elm_Entry_Anchor_Info
 *
 * @brief The information sent in the callback for the "anchor,clicked" signals emitted
 *        by entries.
 */
struct _Elm_Entry_Anchor_Info
{
   const char *name; /**< Name of the anchor, as stated in its href */
   int         button; /**< Mouse button used to click on it */
   Evas_Coord  x, /**< Anchor geometry, relative to canvas */
               y, /**< Anchor geometry, relative to canvas */
               w, /**< Anchor geometry, relative to canvas */
               h; /**< Anchor geometry, relative to canvas */
};

/**
 * @typedef Elm_Entry_Anchor_Hover_Info
 *
 * @brief The information sent in the callback for "anchor,clicked" signals emitted by
 *        the Anchor_Hover widget.
 */
typedef struct _Elm_Entry_Anchor_Hover_Info Elm_Entry_Anchor_Hover_Info;

/**
 * @struct _Elm_Entry_Anchor_Hover_Info
 *
 * @brief The information sent in the callback for "anchor,clicked" signals emitted by
 *        the Anchor_Hover widget.
 */
struct _Elm_Entry_Anchor_Hover_Info
{
   const Elm_Entry_Anchor_Info *anchor_info; /**< Actual anchor info */
   Evas_Object *hover; /**< Hover object to use for the popup */
   struct
   {
      Evas_Coord x, y, w, h;
   } hover_parent; /**< Geometry of the object used as the parent by the
                        hover */
   Eina_Bool    hover_left : 1; /**< Hint indicating whether there's space
                                     for content on the left side of
                                     the hover. Before calling the
                                     callback, the widget makes the
                                     necessary calculations to check
                                     which sides are fit to be set with
                                     content, based on the position where
                                     hover is activated and its distance
                                     to the edges of its parent object
                                 */
   Eina_Bool    hover_right : 1; /**< Hint indicating whether content fits on
                                      the right side of the hover.
                                      See @ref hover_left */
   Eina_Bool    hover_top : 1; /**< Hint indicating whether content fits on top
                                    of the hover. See @ref hover_left */
   Eina_Bool    hover_bottom : 1; /**< Hint indicating whether content fits
                                       below the hover. See @ref
                                       hover_left */
};

/**
 * @typedef Elm_Entry_Item_Provider_Cb
 * @brief Called to provide items.
 *
 * @since_tizen 2.3
 *
 * @remarks If it returns an object handle other than @c NULL (it should create an
 *          object to do this), then this object is used to replace the current item.
 *          If not, then the next provider is called until it provides an item object, or the
 *          default provider in entry does.
 *
 * @param[in] data The data specified as the last parameter when adding the provider
 * @param[in] entry The entry object
 * @param[in] text A pointer to the item href string in the text
 *
 * @return The object to be placed in the entry like an icon, or another element
 *
 * @see elm_entry_item_provider_append
 * @see elm_entry_item_provider_prepend
 * @see elm_entry_item_provider_remove
 */
typedef Evas_Object * (*Elm_Entry_Item_Provider_Cb)(void *data, Evas_Object *entry, const char *item);

/**
 * @typedef Elm_Entry_Filter_Cb
 * @brief Called by entry filters to modify text.
 *
 * @since_tizen 2.3
 *
 * @param data The data specified as the last parameter when adding the filter
 * @param entry The entry object
 * @param text A pointer to the location of the text being filtered \n
 *             The type of text is always markup \n
 *             This data can be modified, but any additional allocations must be managed by the user.
 *
 * @see elm_entry_markup_filter_append
 * @see elm_entry_markup_filter_prepend
 * @see elm_entry_markup_filter_remove
 */
typedef void (*Elm_Entry_Filter_Cb)(void *data, Evas_Object *entry, char **text);

/**
 * @internal
 * @remarks Tizen Only Feature (20140625)
 *
 * @typedef Elm_Entry_Anchor_Access_Provider_Cb
 * @brief This callback type is used to provide TTS string of an anchor.
 *
 * @since_tizen 2.3
 *
 * @remarks If it returns a string other than NULL,
 *          then this string is used to replace the current anchor's TTS string.
 * @remarks If not the next provider is called until one provides a string, or the
 *          default string will be read.
 *
 * @param data The data specified as the last param when adding the provider
 * @param entry The entry object
 * @param name A pointer to the anchor href string in the text
 * @param text A pointer to the text inside of the anchor's range.
 * @return TTS string for the anchor.
 *
 * @see elm_entry_anchor_access_provider_append
 * @see elm_entry_anchor_access_provider_prepend
 * @see elm_entry_anchor_access_provider_remove
 */
typedef char * (*Elm_Entry_Anchor_Access_Provider_Cb)(void *data, Evas_Object * entry, const char *name, const char *text);

/**
 * @typedef Elm_Entry_Change_Info
 * @brief This corresponds to Edje_Entry_Change_Info. It includes information about
 *        a change in the entry.
 */
typedef Edje_Entry_Change_Info Elm_Entry_Change_Info;

/**
 * @brief Adds an entry to the @a parent object.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, entries are as follows:
 *          @li not scrolled
 *          @li multi-line
 *          @li word wrapped
 *          @li autosave is enabled
 *
 * @param[in] parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object       *elm_entry_add(Evas_Object *parent);

/**
 * @brief Pushes the style to the top of the user style stack.
 *
 * @since_tizen 2.3
 *
 * @remarks If there are styles in the user style stack, the properties in the top style
 *          of the user style stack replace the properties in the current theme.
 *          The input style is specified in the format tag='property=value' (i.e. DEFAULT='font=Sans font_size=60'hilight=' + font_weight=Bold').
 *
 * @param[in] obj The entry object
 * @param[in] style The style user to push
 *
 * @since 1.7
 */
EAPI void      elm_entry_text_style_user_push(Evas_Object *obj, const char *style);

/**
 * @brief Removes the style at the top of the user style stack.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param [in] obj The entry object
 *
 * @see elm_entry_text_style_user_push()
 */
EAPI void     elm_entry_text_style_user_pop(Evas_Object *obj);

/**
 * @brief Retrieves the style on the top of the user style stack.
 *
 * @since 1.7
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The style on the top of the user style stack if it exists, otherwise @c NULL
 *
 * @see elm_entry_text_style_user_push()
 */
EAPI const char*      elm_entry_text_style_user_peek(const Evas_Object *obj);

/**
 * @brief Sets the entry to the single line mode.
 *
 * @since_tizen 2.3
 *
 * @remarks In the single line mode, entries don't wrap when the text reaches the
 *          edge, instead they keep growing horizontally. Pressing the Enter
 *          key generates an @c "activate" event instead of adding a new line.
 *
 * @remarks When @a single_line is @c EINA_FALSE, line wrapping takes effect again
 *          and pressing enter breaks the text into a different line
 *          without generating any events.
 *
 * @param[in] obj The entry object
 * @param[in] single_line If @c true, the text in the entry is on a single line,
 *                    otherwise @c false
 */
EAPI void               elm_entry_single_line_set(Evas_Object *obj, Eina_Bool single_line);

/**
 * @brief Gets whether the entry is set to be on a single line.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return single_line @c true if the text in the entry is set to display on a single line,
 *                     otherwise @c false
 *
 * @see elm_entry_single_line_set()
 */
EAPI Eina_Bool          elm_entry_single_line_get(const Evas_Object *obj);

/**
 * @brief Sets the entry to the password mode.
 *
 * @since_tizen 2.3
 *
 * @remarks In the password mode, entries are implicitly on a single line and the display of
 *          any text in them is replaced with asterisks (*).
 *
 * @param[in] obj The entry object
 * @param[in] password If @c true the password mode is enabled,
 *                 otherwise @c false to disable it
 */
EAPI void               elm_entry_password_set(Evas_Object *obj, Eina_Bool password);

/**
 * @brief Gets whether the entry is set to the password mode.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c true if the entry is set to display all characters
 *         as asterisks (*), otherwise @c false
 *
 * @see elm_entry_password_set()
 */
EAPI Eina_Bool          elm_entry_password_get(const Evas_Object *obj);

/**
 * @brief Sets the text displayed within the entry to @a entry.
 *
 * @since_tizen 2.3
 *
 * @remarks Using this function bypasses text filters
 *
 * @param[in] obj The entry object
 * @param[in] entry The text to be displayed
 */
EAPI void               elm_entry_entry_set(Evas_Object *obj, const char *entry);

/**
 * @brief Returns the text currently shown in the object @a entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The currently displayed text, otherwise @c NULL on failure
 *
 * @see elm_entry_entry_set()
 */
EAPI const char        *elm_entry_entry_get(const Evas_Object *obj);

/**
 * @brief Appends @a entry to the text of the entry.
 *
 * @details This adds the text in @a entry to the end of any text already present in the
 *          widget.
 *
 * @since_tizen 2.3
 *
 * @remarks The appended text is subject to any filters set for the widget.
 *
 * @param[in] obj The entry object
 * @param[in] entry The text to be displayed
 *
 * @see elm_entry_markup_filter_append()
 */
EAPI void               elm_entry_entry_append(Evas_Object *obj, const char *entry);

/**
 * @brief Gets whether the entry is empty.
 *
 * @since_tizen 2.3
 *
 * @remarks Empty means no text at all. If there are any markup tags, like an item
 *          tag for which no provider finds anything, and no text is displayed, this
 *          function still returns @c EINA_FALSE.
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE if the entry is empty, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool          elm_entry_is_empty(const Evas_Object *obj);

/**
 * @brief Gets any selected text within the entry.
 *
 * @since_tizen 2.3
 *
 * @remarks If there's any selected text in the entry, this function returns it as
 *          a string in the markup format. @c NULL is returned if no selection exists or
 *          if an error occurs.
 *
 * @remarks The returned value points to an internal string should not be freed
 *          or modified in any way. If the @a entry object is deleted or its
 *          contents are changed, the returned pointer should be considered invalid.
 *
 * @param[in] obj The entry object
 * @return The selected text within the entry, otherwise @c NULL on failure
 */
EAPI const char        *elm_entry_selection_get(const Evas_Object *obj);

/**
 * @brief Returns the actual textblock object of the entry.
 *
 * @details This function exposes the internal textblock object that actually
 *          contains and draws the text. This should be used for low-level
 *          manipulations that are otherwise not possible.
 *
 * @since_tizen 2.3
 *
 * @remarks Changing the textblock directly from here does not notify edje/elm to
 *          recalculate the textblock size automatically, so any modifications
 *          done to the textblock and returned by this function should be followed by
 *          a call to elm_entry_calc_force().
 *
 * @remarks The return value is marked as const so as to serve as an additional warning.
 *          One should not use the returned object with any of the generic evas
 *          functions (geometry_get/resize/move and so on), but only with the textblock
 *          functions; the former either does not work at all, or breaks the correct
 *          functionality.
 *
 * @remarks IMPORTANT: Many functions may change (i.e delete and create a new one)
 *          the internal textblock object. Do NOT cache the returned object, and try
 *          not to mix calls on this object with regular elm_entry calls (which may
 *          change the internal textblock object). This applies to all cursors
 *          returned from textblock calls, and all the other derivative values.
 *
 * @param[in] obj The entry object
 * @return The textblock object
 */
EAPI Evas_Object *      elm_entry_textblock_get(Evas_Object *obj);

/**
 * @brief Forces calculation of the entry size and text layouting.
 *
 * @since_tizen 2.3
 *
 * @remarks This should be used after modifying the textblock object directly. See
 *          elm_entry_textblock_get() for more information.
 *
 * @param[in] obj The entry object
 *
 * @see elm_entry_textblock_get()
 */
EAPI void               elm_entry_calc_force(Evas_Object *obj);

/**
 * @brief Inserts the given text into the entry at the current cursor position.
 *
 * @details This inserts text at the cursor position as if it is typed
 *          by the user (note that this also allows markup which a user
 *          can't just "type" as it would be converted to escaped text, so this
 *          call can be used to insert things like emoticon items or bold push/pop
 *          tags, other font and color change tags etc.)
 *
 * @since_tizen 2.3
 *
 * @remarks If any selection exists, it is replaced by the inserted text.
 *
 * @remarks The inserted text is subject to any filters set for the widget.
 *
 * @param[in] obj The entry object
 * @param[in] entry The text to insert
 *
 * @see elm_entry_markup_filter_append()
 */
EAPI void               elm_entry_entry_insert(Evas_Object *obj, const char *entry);

/**
 * @brief Sets the line wrap type to use on multi-line entries.
 *
 * @details This sets the wrap type used by the entry of any of the specified
 *          Elm_Wrap_Type. This tells how the text is implicitly cut into a new
 *          line (without inserting a line break or paragraph separator) when it
 *          reaches the far edge of the widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that this only makes sense for multi-line entries. A widget set
 *          to be a single line never wraps.
 *
 * @param[in] obj The entry object
 * @param[in] wrap The wrap mode to use \n
 *             For more details, see Elm_Wrap_Type
 */
EAPI void               elm_entry_line_wrap_set(Evas_Object *obj, Elm_Wrap_Type wrap);

/**
 * @brief Gets the wrap mode that the entry is set to use.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The wrap type
 *
 * @see elm_entry_line_wrap_set()
 */
EAPI Elm_Wrap_Type      elm_entry_line_wrap_get(const Evas_Object *obj);

/**
 * @brief Sets whether the entry is editable.
 *
 * @since_tizen 2.3
 *
 * @remarks By default, entries are editable and when focused, any text input by the
 *          user is inserted at the current cursor position. But calling this
 *          function with @a editable as @c EINA_FALSE prevents the user from
 *          inputing text into the entry.
 *
 * @remarks The only way to change the text of a non-editable entry is to use
 *          elm_object_text_set(), elm_entry_entry_insert(), and other related
 *          functions.
 *
 * @param[in] obj The entry object
 * @param[in] editable If @c EINA_TRUE user input is inserted in the entry,
 *                 otherwise @c EINA_FALSE if the entry is read-only and no user input is allowed
 */
EAPI void               elm_entry_editable_set(Evas_Object *obj, Eina_Bool editable);

/**
 * @brief Gets whether the entry is editable.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c true if the entry is editable by the user,
 *         otherwise @ false if it is not editable by the user
 *
 * @see elm_entry_editable_set()
 */
EAPI Eina_Bool          elm_entry_editable_get(const Evas_Object *obj);

/**
 * @brief Drops any existing text selection within the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_select_none(Evas_Object *obj);

/**
 * @brief Selects all the text within the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_select_all(Evas_Object *obj);

/**
 * @brief Moves the cursor by one position to the right within the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool          elm_entry_cursor_next(Evas_Object *obj);

/**
 * @brief Moves the cursor one place to the left within the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool          elm_entry_cursor_prev(Evas_Object *obj);

/**
 * @brief Moves the cursor one line up within the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool          elm_entry_cursor_up(Evas_Object *obj);

/**
 * @brief Moves the cursor one line down within the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool          elm_entry_cursor_down(Evas_Object *obj);

/**
 * @brief Moves the cursor to the beginning of the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_cursor_begin_set(Evas_Object *obj);

/**
 * @brief Moves the cursor to the end of the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_cursor_end_set(Evas_Object *obj);

/**
 * @brief Moves the cursor to the beginning of the current line.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_cursor_line_begin_set(Evas_Object *obj);

/**
 * @brief Moves the cursor to the end of the current line.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_cursor_line_end_set(Evas_Object *obj);

/**
 * @brief Begins a selection within the entry as though
 *        the user were holding down the mouse button to make a selection.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_cursor_selection_begin(Evas_Object *obj);

/**
 * @brief Ends a selection within the entry as though
 *        the user had just released the mouse button while making a selection.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_cursor_selection_end(Evas_Object *obj);

/**
 * @brief Gets whether a format node exists at the current cursor position.
 *
 * @since_tizen 2.3
 *
 * @remarks A format node is anything that defines how the text is rendered. It can
 *          be a visible format node, such as a line break or a paragraph separator,
 *          or an invisible one, such as bold begin or end tag.
 *          This function returns whether any format node exists at the current
 *          cursor position.
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE if the current cursor position contains a format node,
 *         otherwise @c EINA_FALSE
 *
 * @see elm_entry_cursor_is_visible_format_get()
 */
EAPI Eina_Bool          elm_entry_cursor_is_format_get(const Evas_Object *obj);

/**
 * @brief Gets if the current cursor position holds a visible format node.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE if the current cursor is a visible format,
 *         otherwise @c EINA_FALSE if it's an invisible one or no format exists
 *
 * @see elm_entry_cursor_is_format_get()
 */
EAPI Eina_Bool          elm_entry_cursor_is_visible_format_get(const Evas_Object *obj);

/**
 * @brief Gets the character pointed by the cursor at its current position.
 *
 * @details This function returns a string with the UTF8 character stored at the
 *          current cursor position.
 *          Only the text is returned, any format that may exist is not going to be a part
 *          of the return value. The string must be released with free() by you.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The text pointed by the cursor
 */
EAPI char              *elm_entry_cursor_content_get(const Evas_Object *obj);

/**
 * @brief Returns the geometry of the cursor.
 *
 * @since_tizen 2.3
 *
 * @remarks It's useful if you want to draw something on the cursor (or where it is),
 *          or for example in case of a scrolled entry where you want to show the
 *          cursor.
 *
 * @param[in] obj The entry object
 * @param[out] x The returned geometry
 * @param[out] y The returned geometry
 * @param[out] w The returned geometry
 * @param[out] h The returned geometry
 * @return @c EINA_TRUE upon success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool          elm_entry_cursor_geometry_get(const Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);

/**
 * @brief Sets the cursor position in the entry to the given value.
 *
 * @since_tizen 2.3
 *
 * @remarks The value in @a pos is the index of the character position within the
 *          content of the string as returned by elm_entry_cursor_pos_get().
 *
 * @param[in] obj The entry object
 * @param[in] pos The position of the cursor
 */
EAPI void               elm_entry_cursor_pos_set(Evas_Object *obj, int pos);

/**
 * @brief Retrieves the current position of the cursor in the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The cursor position
 */
EAPI int                elm_entry_cursor_pos_get(const Evas_Object *obj);

/**
 * @brief Executes a "cut" action on the selected text in the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_selection_cut(Evas_Object *obj);

/**
 * @brief Executes a "copy" action on the selected text in the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_selection_copy(Evas_Object *obj);

/**
 * @brief Executes a "paste" action in the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_selection_paste(Evas_Object *obj);

/**
 * @brief Clears and frees the items in a entry's contextual (longpress)
 * menu.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 *
 * @see elm_entry_context_menu_item_add()
 */
EAPI void               elm_entry_context_menu_clear(Evas_Object *obj);

/**
 * @brief Adds an item to the entry's contextual menu.
 *
 * @since_tizen 2.3
 *
 * @remarks A longpress on an entry makes the contextual menu show up, if this
 *          hasn't been disabled with elm_entry_context_menu_disabled_set().
 *          By default, this menu provides a few options like enabling the selection mode,
 *          which is useful on embedded devices that need to be explicit about it,
 *          and when a selection exists, it also shows the copy and cut actions.
 *
 * @remarks With this function, developers can add other options to this menu to
 *          perform any action they deem necessary.
 *
 * @param[in] obj The entry object
 * @param[in] label The item's text label
 * @param[in] icon_file The item's icon file
 * @param[in] icon_type The item's icon type
 * @param[in] func The callback to execute when the item is clicked
 * @param[in] data The data to associate with the item for related functions
 */
EAPI void               elm_entry_context_menu_item_add(Evas_Object *obj, const char *label, const char *icon_file, Elm_Icon_Type icon_type, Evas_Smart_Cb func, const void *data);

/**
 * @brief Disables the entry's contextual (longpress) menu.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] disabled If @c true the menu is disabled, 
 *                 otherwise @c false to enable it
 */
EAPI void               elm_entry_context_menu_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Returns whether the entry's contextual (longpress) menu is
 *        disabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c true if the menu is disabled, 
 *         otherwise @c false
 */
EAPI Eina_Bool          elm_entry_context_menu_disabled_get(const Evas_Object *obj);

/**
 * @brief Appends a custom item provider to the list for that entry.
 *
 * @details This appends the given callback. The list is walked through from beginning till the end
 *          with each function being called, given the item href string in the text. If the
 *          function returns an object handle other than @c NULL (it should create an
 *          object to do this), then this object is used to replace that item. If
 *          not, then the next provider is called until it provides an item object, or the
 *          default provider in the entry does.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] func The function that is called to provide the item object
 * @param[in] data The data passed to @a func
 *
 * @see @ref entry-items
 */
EAPI void               elm_entry_item_provider_append(Evas_Object *obj, Elm_Entry_Item_Provider_Cb func, void *data);

/**
 * @brief Prepends a custom item provider to the list for that entry.
 *
 * @details This prepends the given callback. For more details, see elm_entry_item_provider_append().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] func The function that is called to provide the item object
 * @param[in] data The data passed to @a func
 *
 * @see  elm_entry_item_provider_append()
 */
EAPI void               elm_entry_item_provider_prepend(Evas_Object *obj, Elm_Entry_Item_Provider_Cb func, void *data);

/**
 * @brief Removes a custom item provider to the list for that entry.
 *
 * @details This removes the given callback. For more details, see elm_entry_item_provider_append().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] func The function that is called to provide the item object
 * @param[in] data The data passed to @a func
 */
EAPI void               elm_entry_item_provider_remove(Evas_Object *obj, Elm_Entry_Item_Provider_Cb func, void *data);

/**
 * @brief Appends a markup filter function for text inserted in the entry.
 *
 * @details This appends the given callback to the list. This functions is called
 *          whenever any text is inserted into the entry, provided the text to be inserted
 *          is a parameter. The type of the given text is always markup.
 *          The callback function is free to alter the text in any way it wants, but
 *          it must remember to free the given pointer and update it.
 *          If the new text is to be discarded, the function can free it and set its
 *          text parameter to @c NULL. This prevents any future filters from
 *          being called.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] func The function to use as a text filter
 * @param[in] data The data to pass to @a func
 */
EAPI void               elm_entry_markup_filter_append(Evas_Object *obj, Elm_Entry_Filter_Cb func, void *data);

/**
 * @brief Prepends a markup filter function for text inserted in the entry.
 *
 * @details This prepends the given callback to the list. For more details, see elm_entry_markup_filter_append().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] func The function to use as a text filter
 * @param[in] data The data to pass to @a func
 */
EAPI void               elm_entry_markup_filter_prepend(Evas_Object *obj, Elm_Entry_Filter_Cb func, void *data);

/**
 * @brief Removes a markup filter from the list.
 *
 * @details This removes the given callback from the filter list. For more details,
 *          see elm_entry_markup_filter_append().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] func The filter function to remove
 * @param[in] data The data to pass to @a func
 */
EAPI void               elm_entry_markup_filter_remove(Evas_Object *obj, Elm_Entry_Filter_Cb func, void *data);

/**
 * @brief Converts a markup (HTML-like) string into UTF-8.
 *
 * @since_tizen 2.3
 *
 * @remarks The returned string is a malloc'ed buffer and it should be freed when
 *          not needed.
 *
 * @param[in] s The string (in markup) to be converted
 * @return The converted string (in UTF-8) \n
 *         It should be freed.
 */
EAPI char              *elm_entry_markup_to_utf8(const char *s);

/**
 * @brief Converts a UTF-8 string into markup (HTML-like).
 *
 * @since_tizen 2.3
 *
 * @remarks The returned string is a malloc'ed buffer and it should be freed when
 *          not needed.
 *
 * @param[in] s The string (in UTF-8) to be converted
 * @return The converted string (in markup) \n
 *         It should be freed.
 */
EAPI char              *elm_entry_utf8_to_markup(const char *s);

/**
 * @brief Sets the file (and implicitly loads it) for the text to display and
 *        then allow edit. All changes are written back to the file after a short delay if
 *        the entry object is set to autosave (which is the default).
 *
 * @since_tizen 2.3
 *
 * @remarks If the entry has any other file set previously, any changes made to it
 *          are saved if the autosave feature is enabled, otherwise, the file
 *          is silently discarded and any non-saved changes are lost.
 *
 * @param[in] obj The entry object
 * @param[in] file The path to the file to load and save
 * @param[in] format The file format
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 */
EAPI Eina_Bool          elm_entry_file_set(Evas_Object *obj, const char *file, Elm_Text_Format format);

/**
 * @brief Gets the file being edited by the entry.
 *
 * @details This function can be used to retrieve any file set on the entry for
 *          edition, along with the format used to load and save it.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[out] file The path to the file to load and save
 * @param[out] format The file format
 */
EAPI void               elm_entry_file_get(const Evas_Object *obj, const char **file, Elm_Text_Format *format);

/**
 * @brief Writes any changes made to the file that is set by
 *        elm_entry_file_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 */
EAPI void               elm_entry_file_save(Evas_Object *obj);

/**
 * @brief Sets the entry object to 'autosave' the loaded text file.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] autosave The boolean value that indicates whether the loaded file is autosaved
 *
 * @see elm_entry_file_set()
 */
EAPI void               elm_entry_autosave_set(Evas_Object *obj, Eina_Bool autosave);

/**
 * @brief Gets the entry object's 'autosave' status.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The boolean value that indicates whether the loaded file is autosaved
 *
 * @see elm_entry_file_set()
 */
EAPI Eina_Bool          elm_entry_autosave_get(const Evas_Object *obj);

/**
 * @brief Enables or disables scrolling in the entry.
 *
 * @since_tizen 2.3
 *
 * @remarks Normally, the entry is not scrollable unless you enable it with this call.
 *
 * @param[in] obj The entry object
 * @param[in] scroll If @c EINA_TRUE it is scrollable, otherwise @c EINA_FALSE
 */
EAPI void               elm_entry_scrollable_set(Evas_Object *obj, Eina_Bool scroll);

/**
 * @brief Gets the scrollable state of the entry.
 *
 * @since_tizen 2.3
 *
 * @remarks Normally, the entry is not scrollable. This gets the scrollable state
 *          of the entry. For more details, see elm_entry_scrollable_set().
 *
 * @param[in] obj The entry object
 * @return The scrollable state
 */
EAPI Eina_Bool          elm_entry_scrollable_get(const Evas_Object *obj);

/**
 * @brief Sets the visibility of the left-side widget of the entry that is
 *        set by elm_object_part_content_set().
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] setting If @c EINA_TRUE the object should be displayed,
 *                otherwise @c EINA_FALSE if not
 */
EAPI void               elm_entry_icon_visible_set(Evas_Object *obj, Eina_Bool setting);

/**
 * @internal
 * @remarks This API is unusable.
 *
 * @brief Sets the visibility of the end widget of the entry, set by
 *        elm_object_part_content_set(ent, "end", content).
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] setting @c EINA_TRUE if the object should be displayed,
 *                otherwise @c EINA_FALSE if not
 */
EAPI void               elm_entry_end_visible_set(Evas_Object *obj, Eina_Bool setting);

/**
 * @internal
 *
 * @brief Sets the entry's scrollbar policy (i.e. enabling/disabling
 *        them).
 *
 * @remarks Setting an entry to the single-line mode with elm_entry_single_line_set()
 *          automatically disables the display of scrollbars when the entry
 *          moves inside its scroller.
 *
 * @param obj The entry object
 * @param h The horizontal scrollbar policy to apply
 * @param v The vertical scrollbar policy to apply
 *
 * @deprecated Use elm_scroller_policy_set() instead.
 */
EINA_DEPRECATED EAPI void elm_entry_scrollbar_policy_set(Evas_Object *obj, Elm_Scroller_Policy h, Elm_Scroller_Policy v);

/**
 * @internal
 *
 * @brief Enables or disables bouncing within the entry.
 *
 * @remarks This function sets whether the entry bounces when scrolling reaches
 *          the end of the contained entry.
 *
 * @param obj The entry object
 * @param h_bounce The horizontal bounce state
 * @param v_bounce The vertical bounce state
 *
 * @deprecated Use elm_scroller_bounce_set() instead.
 */
EINA_DEPRECATED EAPI void elm_entry_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce);

/**
 * @internal
 *
 * @brief Gets the bounce mode.
 *
 * @param obj The entry object
 * @param h_bounce The boolean value that indicates whether horizontal bounce is allowed
 * @param v_bounce The boolean value that indicates whether vertical bounce is allowed
 *
 * @deprecated Use elm_scroller_bounce_get() instead.
 */
EINA_DEPRECATED EAPI void elm_entry_bounce_get(const Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce);

/**
 * @brief Sets the input panel layout of the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] layout The layout type
 */
EAPI void                   elm_entry_input_panel_layout_set(Evas_Object *obj, Elm_Input_Panel_Layout layout);

/**
 * @brief Gets the input panel layout of the entry.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The layout type
 *
 * @see elm_entry_input_panel_layout_set
 */
EAPI Elm_Input_Panel_Layout elm_entry_input_panel_layout_get(const Evas_Object *obj);

/**
 * @brief Sets the input panel layout variation of the entry.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] variation The layout variation type
 */
EAPI void                   elm_entry_input_panel_layout_variation_set(Evas_Object *obj, int variation);

/**
 * @brief Gets the input panel layout variation of the entry.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The layout variation type
 *
 * @see elm_entry_input_panel_layout_variation_set
 */
EAPI int                    elm_entry_input_panel_layout_variation_get(const Evas_Object *obj);


/**
 * @brief Sets the autocapitalization type on the immodule.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] autocapital_type The type of autocapitalization
 */
EAPI void                   elm_entry_autocapital_type_set(Evas_Object *obj, Elm_Autocapital_Type autocapital_type);

/**
 * @brief Retrieves the autocapitalization type on the immodule.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The autocapitalization type
 */
EAPI Elm_Autocapital_Type   elm_entry_autocapital_type_get(const Evas_Object *obj);

/**
 * @brief Sets the attribute to show the input panel automatically.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] enabled If @c true the input panel appears when the entry is clicked or has focus,
 *                otherwise @c false
 */
EAPI void                   elm_entry_input_panel_enabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Retrieves the attribute to show the input panel automatically.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE if the input panel appears when the entry is clicked or has a focus, 
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool              elm_entry_input_panel_enabled_get(const Evas_Object *obj);

/**
 * @brief Shows the input panel (virtual keyboard) based on the input panel property of the entry such as layout, autocapital types, and so on.
 *
 * @since_tizen 2.3
 *
 * @remarks Note that the input panel is shown or hidden automatically according to the focus state of the entry widget.
 *          This API can be used in case of manually controlling by using elm_entry_input_panel_enabled_set(en, EINA_FALSE).
 *
 * @param[in] obj The entry object
 */
EAPI void                   elm_entry_input_panel_show(Evas_Object *obj);

/**
 * @brief Hides the input panel (virtual keyboard).
 *
 * @since_tizen 2.3
 *
 * @remarks Note that the input panel is shown or hidden automatically according to the focus state of the entry widget.
 *          This API can be used in case of manually controlling by using elm_entry_input_panel_enabled_set(en, EINA_FALSE)
 *
 * @param[in] obj The entry object
 */
EAPI void                   elm_entry_input_panel_hide(Evas_Object *obj);

/**
 * @brief Sets the language mode of the input panel.
 *
 * @since_tizen 2.3
 *
 * @remarks This API can be used if you want to show the alphabet keyboard mode.
 *
 * @param[in] obj The entry object
 * @param[in] lang The language to be set for the input panel
 */
EAPI void                   elm_entry_input_panel_language_set(Evas_Object *obj, Elm_Input_Panel_Lang lang);

/**
 * @brief Gets the language mode of the input panel.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The input panel language type
 *
 * @see @ref elm_entry_input_panel_language_set
 */
EAPI Elm_Input_Panel_Lang   elm_entry_input_panel_language_get(const Evas_Object *obj);

/**
 * @brief Sets the input panel specific data to deliver to the input panel.
 *
 * @since_tizen 2.3
 *
 * @remarks This API is used by applications to deliver specific data to the input panel.
 *          The data format MUST be negotiated by both the application and the input panel.
 *          The size and format of data is defined by the input panel.
 *
 * @param[in] obj The entry object
 * @param[in] data The specific data to be set for the input panel
 * @param[in] len The length of the data, in bytes, to send to the input panel
 */
EAPI void                   elm_entry_input_panel_imdata_set(Evas_Object *obj, const void *data, int len);

/**
 * @brief Gets the specific data of the current input panel.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[out] data The specific data to be obtained from the input panel
 * @param[out] len The length of the data
 *
 * @see @ref elm_entry_input_panel_imdata_set
 */
EAPI void                   elm_entry_input_panel_imdata_get(const Evas_Object *obj, void *data, int *len);

/**
 * @brief Sets the "return" key type. This type is used to set the string or icon on the "return" key of the input panel.
 *
 * @since_tizen 2.3
 *
 * @remarks An input panel displays the string or icon associated with this type.
 *
 * @param[in] obj The entry object
 * @param[in] return_key_type The type of "return" key on the input panel
 */
EAPI void                   elm_entry_input_panel_return_key_type_set(Evas_Object *obj, Elm_Input_Panel_Return_Key_Type return_key_type);

/**
 * @brief Gets the "return" key type.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The type of "return" key on the input panel
 *
 * @see elm_entry_input_panel_return_key_type_set()
 */
EAPI Elm_Input_Panel_Return_Key_Type elm_entry_input_panel_return_key_type_get(const Evas_Object *obj);

/**
 * @brief Sets the return key to be disabled on the input panel.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] disabled If @c EINA_TRUE the return key is disabled,
 *                 otherwise @c EINA_FALSE if it is enabled
 */
EAPI void                   elm_entry_input_panel_return_key_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @brief Gets whether the return key on the input panel should be disabled.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE if the return key should be disabled, 
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool              elm_entry_input_panel_return_key_disabled_get(const Evas_Object *obj);

/**
 * @brief Sets whether the return key on the input panel is disabled automatically when the entry has no text.
 *
 * @since_tizen 2.3
 *
 * @remarks If @a enabled is @c EINA_TRUE, the return key on the input panel is disabled when the entry has no text.
 *          The return key on the input panel is automatically enabled when the entry has text.
 *          The default value is @c EINA_FALSE.
 *
 * @param[in] obj The entry object
 * @param[in] enabled If @a enabled is @c EINA_TRUE, the return key is automatically disabled when the entry has no text,
 *                otherwise @c EINA_FALSE
 */
EAPI void                   elm_entry_input_panel_return_key_autoenabled_set(Evas_Object *obj, Eina_Bool enabled);

/**
 * @brief Sets the attribute to show the input panel in case of a user's explicit Mouse Up event.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @remarks It doesn't request to show the input panel even though it has focus.
 * @param[in] obj The entry object
 * @param[in] ondemand If @c true, the input panel is shown in case of a Mouse up event
 *                 (Focus event is ignored), otherwise @c false
 */
EAPI void                   elm_entry_input_panel_show_on_demand_set(Evas_Object *obj, Eina_Bool ondemand);

/**
 * @brief Gets the attribute to show the input panel in case of a user's explicit Mouse Up event.
 *
 * @since 1.8
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE if the input panel is shown in case of a Mouse up event, 
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool              elm_entry_input_panel_show_on_demand_get(const Evas_Object *obj);

/**
 * @brief Sets the input hint which allows input methods to fine-tune their behavior.
 *
 * @since 1.12
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] hints input hint
 */
EAPI void                   elm_entry_input_hint_set(Evas_Object *obj, Elm_Input_Hints hints);

/**
 * @brief Gets the value of input hint.
 *
 * @since 1.12
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return the value of input hint.
 */
EAPI Elm_Input_Hints        elm_entry_input_hint_get(const Evas_Object *obj);

/**
 * @brief Resets the input method context of the entry if needed.
 *
 * @since_tizen 2.3
 *
 * @remarks This can be necessary in a case where modifying the buffer would confuse an on-going input method behavior.
 *          This typically causes the Input Method Context to clear the pre-edit state.
 *
 * @param[in] obj The entry object
 */
EAPI void                   elm_entry_imf_context_reset(Evas_Object *obj);

/**
 * @brief Sets whether the entry should allow the use of text prediction.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] prediction The boolean value that indicates whether the entry should allow the use of text prediction
 */
EAPI void                   elm_entry_prediction_allow_set(Evas_Object *obj, Eina_Bool prediction);

/**
 * @brief Gets whether the entry should allow the use of text prediction.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return @c EINA_TRUE if it allows the use of text prediction, 
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool              elm_entry_prediction_allow_get(const Evas_Object *obj);

/* Pre-made filters for entries */

/**
 * @typedef Elm_Entry_Filter_Limit_Size
 *
 * @brief The data for the elm_entry_filter_limit_size() entry filter.
 */
typedef struct _Elm_Entry_Filter_Limit_Size Elm_Entry_Filter_Limit_Size;

/**
 * @struct _Elm_Entry_Filter_Limit_Size
 *
 * @brief The data for the elm_entry_filter_limit_size() entry filter.
 */
struct _Elm_Entry_Filter_Limit_Size
{
   int max_char_count;      /**< Maximum number of characters allowed */
   int max_byte_count;      /**< Maximum number of bytes allowed */
};

/**
 * @brief Filters the inserted text based on user defined character and byte limits.
 *
 * @details This adds this filter to an entry to limit the characters that it accepts
 *          based on the content of the provided #Elm_Entry_Filter_Limit_Size.
 *          The function works on the UTF-8 representation of the string, converting
 *          it from the set markup, thus not accounting for any format in it.
 *
 * @since_tizen 2.3
 *
 * @remarks The user must create an #Elm_Entry_Filter_Limit_Size structure and pass
 *          it as data when setting the filter. Within it, it's possible to set limits
 *          based character count or bytes (any of them is disabled if @c 0), and both can
 *          be set at the same time. In that case, it first checks for characters,
 *          then bytes. The #Elm_Entry_Filter_Limit_Size structure must be alive and
 *          valid for as long as the entry is alive AND the elm_entry_filter_limit_size
 *          filter is set.
 *
 * @remarks The function cuts the inserted text in order to allow only the first
 *          few characters that are still allowed. The cut is made in
 *          characters, even when limiting by bytes, in order to always contain
 *          valid ones and avoid half unicode characters.
 *
 * @remarks This filter, like any other, does not apply when setting the entry text
 *          directly using elm_object_text_set().
 *
 * @remarks Do not call this function directly. This function should be used for
 *          parameterfor as Elm_Entry_Filter_Cb.
 *
 * @param data The data specified as the last parameter when adding the filter
 * @param entry The entry object
 * @param text A pointer to the location of the text being filtered \n
 *             The type of text is always markup \n
 *             This data can be modified, but any additional allocations must be managed by the user
 *
 * @see elm_entry_markup_filter_append()
 */
EAPI void elm_entry_filter_limit_size(void *data, Evas_Object *entry, char **text);

/**
 * @typedef Elm_Entry_Filter_Accept_Set
 *
 * @brief The data for the elm_entry_filter_accept_set() entry filter.
 */
typedef struct _Elm_Entry_Filter_Accept_Set Elm_Entry_Filter_Accept_Set;

/**
 * @struct _Elm_Entry_Filter_Accept_Set
 *
 * @brief The data for the elm_entry_filter_accept_set() entry filter.
 */
struct _Elm_Entry_Filter_Accept_Set
{
   const char *accepted;      /**< Set of characters accepted in the entry */
   const char *rejected;      /**< Set of characters rejected from the entry */
};

/**
 * @brief Filters inserted text based on accepted or rejected sets of characters.
 *
 * @details This adds this filter to an entry to restrict the set of accepted characters
 *          based on sets in the provided #Elm_Entry_Filter_Accept_Set.
 *          This structure contains both accepted and rejected sets, but they are
 *          mutually exclusive. This structure must be available for as long as
 *          the entry is alive AND the elm_entry_filter_accept_set is being used.
 *
 * @since_tizen 2.3
 *
 * @remarks The @c accepted set takes preference, so if it is set, the filter
 *          only works based on the accepted characters, ignoring anything in the
 *          @c rejected value. If @c accepted is @c NULL, then @c rejected is used.
 *
 * @remarks In both cases, the function filters by matching UTF8 characters with the
 *          raw markup text, so it can be used to remove formatted tags.
 *
 * @remarks This filter, like any other, does not apply when setting the entry text
 *          directly using elm_object_text_set().
 *
 * @remarks Do not call this function directly. This function should be used for
 *          parameterfor as Elm_Entry_Filter_Cb.
 *
 * @param data The data specified as the last parameter when adding the filter
 * @param entry The entry object
 * @param text A pointer to the location of the text being filtered \n
 *             The type of text is always markup \n
 *             This data can be modified, but any additional allocations must be managed by the user
 *
 * @see elm_entry_markup_filter_append()
 */
EAPI void                   elm_entry_filter_accept_set(void *data, Evas_Object *entry, char **text);

/**
 * @brief Returns the input method context of the entry.
 *
 * @details This function exposes the internal input method context.
 *
 * @since_tizen 2.3
 *
 * @remarks IMPORTANT: Many functions may change (i.e delete and create a new one)
 *          the internal input method context. Do NOT cache the returned object.
 *
 * @param[in] obj The entry object
 * @return The input method context (Ecore_IMF_Context *) in the entry
 */
EAPI void                  *elm_entry_imf_context_get(Evas_Object *obj);

/**
 * @typedef Elm_Cnp_Mode
 * @brief Enumeration that defines the entry's copy & paste policy.
 *
 * @see elm_entry_cnp_mode_set()
 * @see elm_entry_cnp_mode_get()
 */
typedef enum {
   ELM_CNP_MODE_MARKUP,   /**< Copy & paste text with markup tag */
   ELM_CNP_MODE_NO_IMAGE, /**< Copy & paste text without item(image) tag */
   ELM_CNP_MODE_PLAINTEXT /**< Copy & paste text without markup tag */
} Elm_Cnp_Mode;

/**
 * @brief Controls pasting of text and images for the widget.
 *
 * @since_tizen 2.3
 *
 * @remarks Normally the entry allows both text and images to be pasted.
 *          Setting cnp_mode to #ELM_CNP_MODE_NO_IMAGE, prevents images from being copied or pasted.
 *          Setting cnp_mode to #ELM_CNP_MODE_PLAINTEXT, removes all tags in the text.
 *
 * @param[in] obj The entry object
 * @param[in] cnp_mode One #Elm_Cnp_Mode: #ELM_CNP_MODE_MARKUP, #ELM_CNP_MODE_NO_IMAGE, #ELM_CNP_MODE_PLAINTEXT
 *
 * @remarks This only changes the behaviour of the text.
 */
EAPI void         elm_entry_cnp_mode_set(Evas_Object *obj, Elm_Cnp_Mode cnp_mode);

/**
 * @brief Gets the elm_entry text paste/drop mode.
 *
 * @since_tizen 2.3
 *
 * @remarks Normally the entry allows both text and images to be pasted.
 *          This gets the copy & paste mode of the entry.
 *
 * @param[in] obj The entry object
 * @return mode One #Elm_Cnp_Mode: #ELM_CNP_MODE_MARKUP, #ELM_CNP_MODE_NO_IMAGE, #ELM_CNP_MODE_PLAINTEXT.
 */
EAPI Elm_Cnp_Mode elm_entry_cnp_mode_get(const Evas_Object *obj);

/**
 * @brief Sets the parent of the hover popup.
 *
 * @details This sets the parent object to use the hover created by the entry
 *          when an anchor is clicked.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] parent The object to use as a parent for the hover
 *
 * @see Hover
 */
EAPI void                        elm_entry_anchor_hover_parent_set(Evas_Object *obj, Evas_Object *parent);

/**
 * @brief Gets the parent of the hover popup.
 *
 * @details This gets the object used as parent for the hover created by the entry
 *          widget.
 *          If no parent is set, the same entry object is used.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The object used as parent for the hover, otherwise @c NULL if none are set
 * @see Hover
 */
EAPI Evas_Object                *elm_entry_anchor_hover_parent_get(const Evas_Object *obj);

/**
 * @brief Sets the style that the hover should use.
 *
 * @since_tizen 2.3
 *
 * @remarks When creating the popup hover, the entry requests that it be
 *          themed according to @a style.
 *
 * @remarks Setting @a style to @c NULL means disabling automatic hover.
 *
 * @param[in] obj The entry object
 * @param[in] style The style to use for the underlying hover
 *
 * @see elm_object_style_set()
 */
EAPI void                        elm_entry_anchor_hover_style_set(Evas_Object *obj, const char *style);

/**
 * @brief Gets the style that the hover should use.
 *
 * @details This gets the style that the hover created by the entry uses.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @return The style to use by the hover \n
 *         @c NULL means the default is used.
 *
 * @see elm_object_style_set()
 */
EAPI const char                 *elm_entry_anchor_hover_style_get(const Evas_Object *obj);

/**
 * @brief Ends the hover popup in the entry.
 *
 * @since_tizen 2.3
 *
 * @remarks When an anchor is clicked, the entry widget creates a hover
 *          object to use as a popup with user provided content. This function
 *          terminates this popup, returning the entry to its normal state.
 *
 * @param[in] obj The entry object
 */
EAPI void                        elm_entry_anchor_hover_end(Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Enables selection in the entry.
 *
 * @param obj The entry object
 * @param allow If @c true selection is enabled,
 *              otherwise @c false if selection is disabled
 */
EAPI void               elm_entry_select_allow_set(Evas_Object *obj, Eina_Bool allow);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Returns whether selection in the entry is allowed.
 *
 * @param obj The entry object
 * @return @c true if selection is enabled,
 *         otherwise @c false if selection is disabled
 */
EAPI Eina_Bool               elm_entry_select_allow_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Disables the entry's cursor handler.
 *
 * @param obj The entry object
 * @param disabled If @c true the cursor handler is disabled,
 *                 otherwise @c false to enable it
 */
EAPI void               elm_entry_cursor_handler_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Returns whether the entry's cursor handler is disabled.
 *
 * @param obj The entry object
 * @return @c true if the cursor handler is disabled,
 *         otherwise @c false
 */
EAPI Eina_Bool          elm_entry_cursor_handler_disabled_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Disables the entry's magnifier feature.
 *
 * @param obj The entry object
 * @param disabled If @c true the magnifier is not displayed,
 *                 otherwise @c false
 */
EAPI void                        elm_entry_magnifier_disabled_set(Evas_Object *obj, Eina_Bool disabled);
/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Returns whether the entry's magnifier feature is disabled.
 *
 * @param obj The entry object
 * @return @c true if the feature is disabled,
 *         otherwise @c false
 */
EAPI Eina_Bool                   elm_entry_magnifier_disabled_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Disables the default drag action in the entry.
 *
 * @param obj The entry object
 * @param disabled If @c true, disable the default drag action in the entry,
 *                 otherwise @c false
 */
EAPI void                        elm_entry_drag_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Gets whether the default drag action in the entry is disabled.
 *
 * @param obj The entry object
 * @return @c true if the default drag action in the entry is disabled,
 *         otherwise @c false
 */
EAPI Eina_Bool                   elm_entry_drag_disabled_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Disables the default drop callback in the entry.
 *
 * @param obj The entry object
 * @param disabled If @c true the default drop callback in the entry is disabled,
 *                 otherwise @c false
 */
EAPI void                        elm_entry_drop_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @internal
 * @remarks Tizen Only Feature
 *
 * @brief Gets whether the default drop callback in the entry is disabled.
 *
 * @param obj The entry object
 * @return @c true if the default drop callback in the entry is disabled,
 *         otherwise @c false
 */
EAPI Eina_Bool                   elm_entry_drop_disabled_get(Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen Only Feature (20140625)
 *
 * This appends a custom anchor access provider to the list for that entry
 *
 * This appends the given callback. The list is walked from beginning to end
 * with each function called given the anchor href string in the text. If the
 * function returns a string other than NULL, then this string is used
 * to replace that TTS string.
 * If not the next provider is called until one provides a string, or the
 * default TTS string will be read.
 *
 * @param obj The entry object
 * @param func The function called to provide the TTS string
 * @param data The data passed to @p func
 *
 * @see @ref entry-anchors
 */
EAPI void               elm_entry_anchor_access_provider_append(Evas_Object *obj, Elm_Entry_Anchor_Access_Provider_Cb func, void *data);

/**
 * @internal
 * @remarks Tizen Only Feature (20140625)
 *
 * @brief This prepends a custom anchor access provider to the list for that entry
 *
 * @remarks This prepends the given callback. 
 *
 * @see elm_entry_anchor_access_provider_append() 
 *
 * @param obj The entry object
 * @param func The function called to provide the TTS string
 * @param data The data passed to @p func
 */
EAPI void               elm_entry_anchor_access_provider_prepend(Evas_Object *obj, Elm_Entry_Anchor_Access_Provider_Cb func, void *data);

/**
 * @internal
 * @remarks Tizen Only Feature(20140625)
 *
 * @brief This removes a custom anchor access provider to the list for that entry
 *
 * @remarks This removes the given callback. 
 *
 * @see elm_entry_anchor_access_provider_append()
 *
 * @param obj The entry object
 * @param func The function called to provide the TTS string
 * @param data The data passed to @p func
 */
EAPI void               elm_entry_anchor_access_provider_remove(Evas_Object *obj, Elm_Entry_Anchor_Access_Provider_Cb func, void *data);

/**
 * @brief This selects a region of text within the entry.
 *
 * @ingroup Entry
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The entry object
 * @param[in] start The starting position
 * @param[in] end The end position
 *
 */
EAPI void              elm_entry_select_region_set(Evas_Object *obj, int start, int end);


/**
 * @}
 */
