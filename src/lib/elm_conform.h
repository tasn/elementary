/**
 * @defgroup Conformant Conformant
 * @ingroup elm_widget_group
 *
 * @image html conformant_inheritance_tree.png
 * @image latex conformant_inheritance_tree.eps
 *
 * @image html img/conformant.png
 * @image latex img/conformant.eps "conformant" width=\textwidth
 *
 * @brief The aim is to provide a widget that can be used in elementary apps to
 *        account for space taken up by the indicator, virtual keypad & softkey
 *        windows when running the illume2 module of E17.
 *
 * So conformant content is sized and positioned considering the
 * space required for such stuff, and when they popup, as a keyboard
 * shows when an entry is selected, conformant content won't change.
 *
 * This widget inherits from the @ref Layout one, so that all the
 * functions acting on it also work for conformant objects.
 *
 * This widget emits the following signals, besides the ones sent from
 * @ref Layout :
 * @li "virtualkeypad,size,changed": This is called when the virtualkeypad size is
 * changed. @c event_info parameter is the virtualkeypad size in
 * Evas_Coord_Rectangle structure. (@since 1.8)
 *
 * Available styles for it:
 * - @c "default"
 *
 * The default content parts of the conformant widget that you can use are:
 * @li "default" - Content of the conformant.
 *
 * @{
 */

/**
 * @brief Adds a new conformant widget to the given parent Elementary
 *        (container) object.
 *
 * @details This function inserts a new conformant widget on the canvas.
 *
 * @since_tizen 2.3
 *
 * @param[in] parent The parent object
 * @return A new conformant widget handle, otherwise @c NULL in case of an error
 */
EAPI Evas_Object                 *elm_conformant_add(Evas_Object *parent);

/**
 * @}
 */
