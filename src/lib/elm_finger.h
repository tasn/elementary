/**
 * @defgroup Fingers Fingers
 * @ingroup elm_infra_group
 *
 * @brief Elementary is designed to be finger-friendly for touchscreens,
 *        and so in addition to scaling for display resolution, it can
 *        also scale based on finger "resolution" (or size). You can then
 *        customize the granularity of the areas meant to receive clicks
 *        on touchscreens.
 *
 * @remarks Different profiles may have pre-set values for finger sizes.
 *
 * @{
 */

/**
 * @brief Adjusts the size of an element for finger usage.
 *
 * @since_tizen 2.3
 *
 * @remarks This takes width and height sizes (in pixels) as input and a
 *          size multiple (which is how many fingers you want to place
 *          within the area, with "finger" as the size set by
 *          elm_finger_size_set()), and adjusts the size to be large enough
 *          to accommodate the resulting size, if it doesn't already
 *          accommodate it. On return the @a w and @a h sizes pointed to by
 *          these parameters are modified, on those conditions.
 *
 * @remarks This is a kind of low level Elementary call, most useful
 *          on size evaluation times for widgets. An external user wouldn't
 *          be calling, most of the time.
 *
 * @param[in] times_w The number of fingers that fit horizontally
 * @param[out] w A pointer to the width size to adjust
 * @param[in] times_h The number of fingers that fit vertically
 * @param[out] h A pointer to the height size to adjust
 */
EAPI void       elm_coords_finger_size_adjust(int times_w, Evas_Coord *w, int times_h, Evas_Coord *h);

/**
 * @}
 */
