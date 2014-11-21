/**
 * @internal
 * @defgroup Route Route
 * @ingroup Elementary
 *
 * @image html route_inheritance_tree.png
 * @image latex route_inheritance_tree.eps
 *
 * @brief For displaying a route on the map widget.
 *
 * @{
 */

/**
 * @internal
 *
 * @brief Adds a new route object to the parent's canvas.
 *
 * @param parent The parent object
 * @return The new object, otherwise @c NULL if it cannot be created
 *
 * @ingroup Route
 */
EAPI Evas_Object *elm_route_add(Evas_Object *parent);

#ifdef ELM_EMAP
EAPI void         elm_route_emap_set(Evas_Object *obj, EMap_Route *emap);
#endif

/**
 * @internal
 *
 * @brief Gets the minimum and maximum values along the longitude.
 *
 * @remarks If only one value is needed, the other pointer can be passed
 *          as @c NULL.
 *
 * @param obj The route object
 * @param min A pointer to store the minimum value
 * @param max A pointer to store the maximum value
 *
 * @ingroup Route
 */
EAPI void        elm_route_longitude_min_max_get(const Evas_Object *obj, double *min, double *max);

/**
 * @internal
 *
 * @brief Gets the minimum and maximum values along the latitude.
 *
 * @remarks If only one value is needed, the other pointer can be passed
 *          as @c NULL.
 *
 * @param obj The route object
 * @param min A pointer to store the minimum value
 * @param max A pointer to store the maximum value
 *
 * @ingroup Route
 */
EAPI void        elm_route_latitude_min_max_get(const Evas_Object *obj, double *min, double *max);

/**
 * @}
 */
