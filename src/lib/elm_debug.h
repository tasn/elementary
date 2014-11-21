/**
 * @defgroup Debug Debug
 * @ingroup elm_infra_group
 * @brief Don't use it unless you are sure.
 *
 * @{
 */

/**
 * @brief Prints the Tree object hierarchy in stdout.
 *
 * @since_tizen 2.3
 *
 * @param[in] top The root object
 */
EAPI void elm_object_tree_dump(const Evas_Object *top);

/**
 * @brief Prints the Elm Objects tree hierarchy in a file as dot(graphviz) syntax.
 *
 * @since_tizen 2.3
 *
 * @param[in] top The root object
 * @param[in] file The path of the output file
 */
EAPI void elm_object_tree_dot_dump(const Evas_Object *top, const char *file);

/**
 * @}
 */
