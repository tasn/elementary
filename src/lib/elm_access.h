/**
 * @internal
 * @defgroup Access Access
 * @ingroup elm_infra_group
 *
 * TODO: description
 *
 * @{
 */

#define MSG_DOMAIN_CONTROL_ACCESS (int)ECORE_X_ATOM_E_ILLUME_ACCESS_CONTROL

enum _Elm_Access_Info_Type
{
   ELM_ACCESS_INFO_FIRST = -1,
   ELM_ACCESS_INFO,         /* Next read is info - this is
                             * normally the label */
   ELM_ACCESS_TYPE,         /* When reading out the widget or item
                             * this is read first */
   ELM_ACCESS_STATE,        /* If there is a state (eg checkbox)
                             * then read the state out */
   ELM_ACCESS_CONTEXT_INFO, /* To give contextual information */
   ELM_ACCESS_INFO_LAST
};

/**
 * @typedef Elm_Access_Info_Type
 */
typedef enum _Elm_Access_Info_Type Elm_Access_Info_Type;

typedef char *(*Elm_Access_Info_Cb)(void *data, Evas_Object *obj);
typedef void (*Elm_Access_Activate_Cb)(void *data, Evas_Object *part_obj, Elm_Object_Item *item);

/**
 * @enum _Elm_Access_Action_Type
 * @brief Enumerations of the supported access action types.
 */
enum _Elm_Access_Action_Type
{
   ELM_ACCESS_ACTION_FIRST = -1,

   ELM_ACCESS_ACTION_HIGHLIGHT, /**< Highlight an object */
   ELM_ACCESS_ACTION_UNHIGHLIGHT, /**< Unhighlight an object */
   ELM_ACCESS_ACTION_HIGHLIGHT_NEXT, /**< Set highlight to the next object */
   ELM_ACCESS_ACTION_HIGHLIGHT_PREV, /**< Set highlight to the previous object */
   ELM_ACCESS_ACTION_ACTIVATE, /**< Activate a highlight object */
   ELM_ACCESS_ACTION_VALUE_CHANGE, /**< TODO: deprecate this */
   ELM_ACCESS_ACTION_SCROLL, /**< Scroll if one of the highlight object parents
                              * is scrollable */
   ELM_ACCESS_ACTION_ZOOM, /**< Send the signal of the zoom gesture
                            * by a callback function */
   ELM_ACCESS_ACTION_MOUSE, /**< Give the mouse event to the highlight object */
   ELM_ACCESS_ACTION_UP, /**< Change the value up of the highlight object */
   ELM_ACCESS_ACTION_DOWN, /**< Change the value down of the highlight object */
   ELM_ACCESS_ACTION_BACK, /**< Go back to a previous view
                              ex: pop naviframe item */
   ELM_ACCESS_ACTION_OVER, /**< Mouse over an object */
   ELM_ACCESS_ACTION_READ, /**< Highlight an object */
   ELM_ACCESS_ACTION_ENABLE, /**< Enable highlight and read ability */
   ELM_ACCESS_ACTION_DISABLE, /**< Disable highlight and read ability */

   ELM_ACCESS_ACTION_LAST
};

/**
 * @typedef Elm_Access_Action_Type
 */
typedef enum _Elm_Access_Action_Type Elm_Access_Action_Type;

enum _Elm_Highlight_Direction
{
	ELM_HIGHLIGHT_DIR_FIRST = -1,
	ELM_HIGHLIGHT_DIR_NEXT,
	ELM_HIGHLIGHT_DIR_PREVIOUS
};
/**
 * @typedef Elm_Access_Action_Type
 */
typedef enum _Elm_Highlight_Direction Elm_Highlight_Direction;

enum _Elm_Access_Sound_Type
{
   ELM_ACCESS_SOUND_FIRST = -1,
   ELM_ACCESS_SOUND_HIGHLIGHT,
   ELM_ACCESS_SOUND_SCROLL,
   ELM_ACCESS_SOUND_END,
   ELM_ACCESS_SOUND_LAST
};
/**
 * @typedef Elm_Access_Sound_Type
 */
typedef enum _Elm_Access_Sound_Type Elm_Access_Sound_Type;

struct _Elm_Access_Action_Info
{
   double       zoom;
   double       angle;
   Evas_Coord   x;
   Evas_Coord   y;
   unsigned int mouse_type; /**< 0: mouse down
                               1: mouse move
                               2: mouse up   */

   Elm_Access_Action_Type action_type;
   Elm_Access_Action_Type action_by;
   Eina_Bool              highlight_cycle : 1;
   Eina_Bool              highlight_end : 1;
};

typedef struct _Elm_Access_Action_Info Elm_Access_Action_Info;

/**
 * @typedef Elm_Access_Action_Cb
 * @brief Called to make an access object do specific actions.
 *
 * @param[in] data The user data
 * @param[in] action_info The information to classify the action
 * @return @c EINA_TRUE on success, otherwise @c EINA FALSE
 *
 */
typedef Eina_Bool (*Elm_Access_Action_Cb)(void *data, Evas_Object *obj, Elm_Access_Action_Info *action_info);

/**
 * @brief Registers the evas object as an accessible object.
 *
 * @since 1.8
 *
 * @param[in] obj The evas object to register as an accessible object
 * @param[in] parent The elementary object which is used for creating
 *               an accessible object
 */
EAPI Evas_Object *elm_access_object_register(Evas_Object *obj, Evas_Object *parent);

/**
 * @brief Unregister the accessible object.
 *
 * @since 1.8
 *
 * @param[in] obj The Evas object to unregister an accessible object
 */
EAPI void elm_access_object_unregister(Evas_Object *obj);

/**
 * @brief Gets an accessible object of the evas object.
 *
 * @since 1.8
 *
 * @param[in] obj The evas object
 * @return The accessible object of the evas object, otherwise @c NULL in case of an error
 */
EAPI Evas_Object *elm_access_object_get(const Evas_Object *obj);

/**
 * @brief Sets text to give information for a specific type.
 *
 * @since 1.8
 *
 * @param[in] obj The accessible object
 * @param[in] type The type of content that is read
 * @param[in] text The text information that is read
 *
 * @see elm_access_info_cb_set
 */
EAPI void elm_access_info_set(Evas_Object *obj, int type, const char *text);

/**
 * @brief Sets text to give information for a specific type.
 *
 * @since 1.8
 *
 * @param[in] obj The accessible object
 * @param[in] type The type of content that is read
 *
 * @see elm_access_info_cb_set
 */
EAPI char *elm_access_info_get(const Evas_Object *obj, int type);

/**
 * @brief Sets a content callback to give information for a specific type.
 *
 * @since 1.8
 *
 * @remarks The type would be one from @c ELM_ACCESS_TYPE, @c ELM_ACCESS_INFO,
 *          @c ELM_ACCESS_STATE, or @c ELM_ACCESS_CONTEXT_INFO.
 *
 * @remarks In the case of button widget, the content of @c ELM_ACCESS_TYPE would be
 *          "button". The label of the button such as "ok", "cancel" is for @c ELM_ACCESS_INFO.
 *          If the button is disabled, content of @c ELM_ACCESS_STATE would be "disabled".
 *          And if there is contextual information, use @c ELM_ACCESS_CONTEXT_INFO.
 *
 * @param[in] obj The accessible object
 * @param[in] type The type of content that is read
 * @param[in] func The function to be called when the content is read
 * @param[in] data The data pointer to be passed to @a func
 */
EAPI void elm_access_info_cb_set(Evas_Object *obj, int type, Elm_Access_Info_Cb func, const void *data);

/**
 * @brief Sets an activate callback to activate the highlight object.
 *
 * @since 1.8
 *
 * @param[in] obj The accessible object
 * @param[in] func The function to be called when the activate gesture is detected
 * @param[in] data The data pointer to be passed to @a func
 */
EAPI void elm_access_activate_cb_set(Evas_Object *obj, Elm_Access_Activate_Cb func, void *data);

/**
 * @brief Reads out text information directly.
 *
 * @since 1.8
 *
 * @remarks This function does not free the @a text internally.
 *
 * @param[in] text The text information that is read
 */
EAPI void elm_access_say(const char *text);

/**
 * @brief Read out text information forcibly and directly.
 * @since 1.11
 *
 * @param[in] text The text information that will be read
 *
 * This text isn't interupted any other elm_access_say.
 * Don't use this API continuous and repeatedly
 * This function will not free the @p text internally.
 *
 * @ingroup Access
 */
EAPI void elm_access_force_say(const char *text);

/**
 * @brief Gives the highlight to the object directly.
 *
 * @since 1.8
 *
 * @remarks The object should be an elementary object or an access object.
 *
 * @param[in] obj The object that has the highlight and its information to be read
 *
 * @see elm_access_object_get
 */
EAPI void elm_access_highlight_set(Evas_Object* obj);

/**
 * @brief Gives the highlight to the item directly.
 *
 * @since 1.8
 *
 * @remarks This API works like elm_access_highlight_set().
 *          Just the given parameter is different.
 *
 * @param[in] item The item that has the highlight and its information to be read
 *
 * @see elm_access_highlight_set
 */
EAPI void elm_access_item_highlight_set(Elm_Object_Item *item);

/**
 * @brief Sets the next access object for the highlight.
 *
 * @since 1.8
 *
 * @remarks Currently the focus chain is used for accessing the highlight direction. Use this API
 *          to customize the focus chain. If the focus chain is already established, you can
 *          change one object's highlight chain and not break the other object's
 *          focus chain. If you use elm_object_focus_custom_chain_append(), it
 *          resets another object's custom chain also.
 *
 * @param[in] obj  The object which is a previous access or widget object of the next object for highlight
 * @param[in] dir  The access direction which is same as the focus direction
 * @param[in] next The object which is a next access object of @a obj for highlight
 *
 * @see elm_object_focus_custom_chain_append
 */
EAPI void
elm_access_highlight_next_set(Evas_Object *obj, Elm_Highlight_Direction dir, Evas_Object *next);

/**
 * @brief Set the end of whole acces chain.
 * @since 1.9
 *
 * @remarks If the access highlight reaches to start or end of an access chain,
 *          it should notice the situation. In some case, the application can change
 *          the order of access chain. In that case, the application have to set
 *          the end of access chain.
 *
 * @param[in] obj  The object is in the end of access chain.
 * @param[in] dir  Access direction same as Focus direction
 */
EAPI void elm_access_chain_end_set(Evas_Object *obj, Elm_Highlight_Direction dir);

/**
 * @brief Do the accessibility action base on given object.
 * @since 1.8
 *
 * @param[in] obj The object that could be an any object. it would be useful to use a container widget.
 * @param[in] type The type of accessibility action.
 * @param[in] action_info The action information of action @p type to give more specific information.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise
 *
 * The return value would be useful, when the @p type is ELM_ACCESS_ACTION_HIGHLIGHT_NEXT
 * or ELM_ACCESS_ACTION_HIGHLIGHT_PREV. If there is no way to give a highlight,
 * @c EINA_FALSE will be returned.
 */
EAPI Eina_Bool elm_access_action(Evas_Object *obj, const Elm_Access_Action_Type type, void *action_info);

/**
 * @brief Set a callback function to a given accessibility action type
 * @since 1.8
 *
 * @param[in] obj The object to attach a callback to
 * @param[in] type The type of accessibility action.
 * @param[in] cb The callback function to be called when the accessibility action is triggered.
 * @param[in] data The data pointer to be passed to @p cb
 */
EAPI void elm_access_action_cb_set(Evas_Object *obj, const Elm_Access_Action_Type type, const Elm_Access_Action_Cb cb, const void *data);

//TODO: remvoe below - use elm_access_text_set(); or elm_access_cb_set();
EINA_DEPRECATED EAPI void elm_access_external_info_set(Evas_Object *obj, const char *text);
EINA_DEPRECATED EAPI char *elm_access_external_info_get(const Evas_Object *obj);

/**
 * @}
 */
