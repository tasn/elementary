/**
 * @defgroup Caches Caches
 * @ingroup elm_infra_group
 *
 * @brief These are functions which let one fine-tune some cache values for
 *        Elementary applications, thus allowing for performance adjustments.
 *
 * @{
 */

/**
 * @brief Flushes all caches.
 *
 * @details This frees all data that is in the cache and is not currently being used, in order to reduce
 *          memory usage. This frees Edje's, Evas', and Eet's cache. This is equivalent
 *          to calling all of the following functions:
 *          @li edje_file_cache_flush()
 *          @li edje_collection_cache_flush()
 *          @li eet_clearcache()
 *          @li evas_image_cache_flush()
 *          @li evas_font_cache_flush()
 *          @li evas_render_dump()
 *
 * @since_tizen 2.3
 *
 * @remarks Evas caches are flushed for every canvas associated with a window.
 */
EAPI void      elm_cache_all_flush(void);

/**
 * @}
 */
