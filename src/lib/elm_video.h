/**
 * @internal
 * @defgroup Video Video
 * @ingroup elm_widget_group
 *
 * @image html video_inheritance_tree.png
 * @image latex video_inheritance_tree.eps
 *
 * @image html player_inheritance_tree.png
 * @image latex player_inheritance_tree.eps
 *
 * @brief This widget can display a video by using Emotion.
 *
 * Elementary comes with two objects that help design applications that need
 * to display a video.
 *
 * The first one, Elm_Video, displays a video by using Emotion.
 * It embeds the video inside an Edje object, so that you can do some
 * animation depending on the video state change. It also implements a
 * resource management policy to remove this burden from the application.
 *
 * The second one,
 * Elm_Player is a video player that needs to be linked with an Elm_Video.
 * It takes care of updating its content according to the Emotion event and provides a
 * way to theme itself. It also automatically raises the priority of the
 * linked Elm_Video so it uses the video decoder, if available. It also
 * activates the "remember" function on the linked Elm_Video object.
 *
 * Both widgets inherit from the @ref Layout one, so that all the
 * functions acting on it also work for video objects.
 *
 * The player widget emits the following signals, besides the ones
 * sent from @ref Layout :
 *  - @c "forward,clicked" - The user clicked the forward button.
 *  - @c "info,clicked" - The user clicked the info button.
 *  - @c "next,clicked" - The user clicked the next button.
 *  - @c "pause,clicked" - The user clicked the pause button.
 *  - @c "play,clicked" - The user clicked the play button.
 *  - @c "prev,clicked" - The user clicked the prev button.
 *  - @c "rewind,clicked" - The user clicked the rewind button.
 *  - @c "stop,clicked" - The user clicked the stop button.
 *
 * The default content parts of the player widget that you can use are:
 * @li "video" - A video of the player.
 *
 * @{
 */

/**
 * @brief Adds a new Elm_Player object to the given parent Elementary (container) object.
 *
 * @details This function inserts a new player widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return A new player widget handle, otherwise @c NULL in case of an error
 *
 * @see elm_object_part_content_set()
 */
EAPI Evas_Object         *elm_player_add(Evas_Object *parent);

/**
 * @brief Adds a new Elm_Video object to the given parent Elementary (container) object.
 *
 * @details This function inserts a new video widget on the canvas.
 *
 * @param[in] parent The parent object
 * @return A new video widget handle, otherwise @c NULL in case of an error
 *
 * @see elm_video_file_set()
 */
EAPI Evas_Object         *elm_video_add(Evas_Object *parent);

/**
 * @brief Defines the file or URI that is the video source.
 *
 * @details This function explicitly defines a file or URI as a source
 *          for the video of the Elm_Video object.
 *
 * @param[in] video The video object to define the file or URI for the video
 *              of the Elm_Video object
 *
 * @param[in] filename The file or URI to target
 *                 Local files can be specified using file:// or by using full file paths.
 *                 URI could be a remote source of video, like http:// or a local source like
 *                 WebCam (v4l2://). (You can use Emotion API to request and list
 *                 the available Webcam on your system).
 *
 * @return @c EINA_TRUE on success, otherwise @c EINA_FALSE on failure
 *
 * @see elm_video_add()
 * @see elm_player_add()
 */
EAPI Eina_Bool            elm_video_file_set(Evas_Object *video, const char *filename);

/**
 * @brief Get the underlying Emotion object.
 *
 * @param[in] video The video object to proceed the request on
 * @return The underlying Emotion object
 */
EAPI Evas_Object         *elm_video_emotion_get(const Evas_Object *video);

/**
 * @brief Starts to play the video.
 *
 * @details This starts to play the video and cancel the all suspend state.
 *
 * @param[in] video The video object to proceed the request on
 */
EAPI void                 elm_video_play(Evas_Object *video);

/**
 * @brief Pauses the video.
 *
 * @details This pauses the video and starts a timer to trigger the suspend mode.
 *
 * @param[in] video The video object to proceed the request on
 */
EAPI void                 elm_video_pause(Evas_Object *video);

/**
 * @brief Stops the video.
 *
 * @details This stops the video and puts the emotion in the deep sleep mode.
 *
 * @param[in] video The video object to proceed the request on
 */
EAPI void                 elm_video_stop(Evas_Object *video);

/**
 * @brief Gets whether the video is actually playing.
 *
 * @remarks You should consider watching an event on the object instead of polling
 *          the object state.
 *
 * @param[in] video The video object to proceed the request on
 * @return @c EINA_TRUE if the video is actually playing,
 *         otherwise @c EINA_FALSE
 */
EAPI Eina_Bool            elm_video_is_playing_get(const Evas_Object *video);

/**
 * @brief Gets whether it is possible to seek inside the video.
 *
 * @param[in] video The video object to proceed the request on
 * @return #EINA_TRUE if it is possible to seek inside the video,
 *         otherwise #EINA_FALSE
 */
EAPI Eina_Bool            elm_video_is_seekable_get(const Evas_Object *video);

/**
 * @brief Gets whether the audio is muted.
 *
 * @param[in] video The video object to proceed the request on
 * @return The current audio level
 */
EAPI double               elm_video_audio_level_get(const Evas_Object *video);

/**
 * @brief Sets the audio level of an Elm_Video object.
 *
 * @param[in] video The video object to proceed the request on
 * @param[in] volume The new audio volume
 */
EAPI void                 elm_video_audio_level_set(Evas_Object *video, double volume);

/**
 * @brief Gets the current position (in seconds) being played in the
 *        Elm_Video object.
 *
 * @param[in] video The video object
 * @return The time (in seconds) since the beginning of the media file
 */
EAPI double               elm_video_play_position_get(const Evas_Object *video);

/**
 * @brief Sets the current position (in seconds) to be played in the
 *        Elm_Video object.
 *
 * @param[in] video The video object
 * @param[in] position The time (in seconds) since the beginning of the media file
 */
EAPI void                 elm_video_play_position_set(Evas_Object *video, double position);

/**
 * @brief Gets the total playing time (in seconds) of the Elm_Video object.
 *
 * @param[in] video The video object
 * @return The total duration (in seconds) of the media file
 */
EAPI double               elm_video_play_length_get(const Evas_Object *video);

/**
 * @brief Sets whether the object can remember the last played position.
 *
 * @remarks This API only serves as an indication. System support is required.
 *
 * @param[in] video The video object
 * @param[in] remember The boolean value that indicates the last played position of the Elm_Video object
 */
EAPI void                 elm_video_remember_position_set(Evas_Object *video, Eina_Bool remember);

/**
 * @brief Sets whether the object can remember the last played position.
 *
 * @remarks This API only serves as an indication. System support is required.
 *
 * @param[in] video The video object
 * @return The boolean value that indicates whether the object remembers the last played position (@c EINA_TRUE)
 */
EAPI Eina_Bool            elm_video_remember_position_get(const Evas_Object *video);

/**
 * @brief Gets the title (for instance DVD title) from this emotion object.
 *
 * @remarks This function is only useful when playing a DVD.
 *
 * @remarks Don't change or free the string returned by this function.
 *
 * @param[in] video The Elm_Video object
 * @return A string containing the title
 */
EAPI const char          *elm_video_title_get(const Evas_Object *video);

/**
 * @}
 */
