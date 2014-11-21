/**
 * @defgroup Plug Plug
 * @ingroup elm_widget_group
 *
 * @image html plug_inheritance_tree.png
 * @image latex plug_inheritance_tree.eps
 *
 * @brief An object that allows one to show an image which the other process created.
 *        It can be used anywhere like any other elementary widget.
 *
 * @{
 */

/**
 * @typedef Elm_Plug_Message
 *
 * @brief The structure type holding the message
 *        which elm plug received from ecore evas.
 *
 */
struct _Elm_Plug_Message
{
   int msg_domain;
   int msg_id;
   void *data;
   int size;
};

typedef struct _Elm_Plug_Message Elm_Plug_Message;

/**
 * @brief Adds a new plug image to the parent.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return The new plug image object, otherwise @c NULL if it cannot be created
 */
EAPI Evas_Object    *elm_plug_add(Evas_Object *parent);

/**
 * @brief Connects a plug widget to the service provided by the socket image.
 *
 * @since_tizen 2.3
 *
 * @param[in] obj The Evas_Object where the new image object lives
 * @param[in] svcname The service name to connect to the set up by the socket
 * @param[in] svcnum The service number to connect to (set up by socket)
 * @param[in] svcsys The boolean to set if the service is a system one or not (set up by socket)
 * @return (@c EINA_TRUE = success, @c EINA_FALSE = error)
 */
EAPI Eina_Bool       elm_plug_connect(Evas_Object *obj, const char *svcname, int svcnum, Eina_Bool svcsys);

/**
 * @brief Gets the basic Evas_Image object from this object (widget).
 *
 * @details This function allows one to get the underlying @c Evas_Object of type
 *          Image from this elementary widget. It can be useful to do things like getting
 *          the pixel data, saving the image to a file, etc.
 *
 * @since_tizen 2.3
 *
 * @remarks Be careful to not manipulate it, as it is under the control of
 *          elementary.
 *
 * @param[in] obj The image object to get the inlined image from
 * @return The inlined image object, otherwise @c NULL if none exist
 */
EAPI Evas_Object    *elm_plug_image_object_get(const Evas_Object *obj);

/**
 * @internal
 * @remarks Tizen only feature
 *
 * @brief Sends a message to the plug widget's socket.
 *
 * @since 1.8.0
 *
 * @remarks Support for this depends on the underlying windowing system.
 *
 * @param[in] obj The plug object to send a message to
 * @param[in] msg_domain The domain of the message
 * @param[in] msg_id The ID of the message
 * @param[in] data The data of the message
 * @param[in] size The size of the message data
 *
 */
EAPI Eina_Bool       elm_plug_msg_send(Evas_Object *obj, int msg_domain, int msg_id, void *data, int size); 

/**
 * @}
 */
