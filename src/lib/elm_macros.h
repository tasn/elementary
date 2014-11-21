/* Handy macros */
#define ELM_RECTS_INTERSECT(x, y, w, h, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && (((x) + (w)) > (xx)) && (((y) + (h)) > (yy)))
#define ELM_PI 3.14159265358979323846
#define ELM_ITEM_HIGHLIGHT_TIMER 0.1
#define ELM_SCALE_SIZE(x) x / elm_app_base_scale_get() * elm_config_scale_get()

// Checks if the point(xx, yy) stays out of the rectangle(x, y, w, h) area.
#define ELM_RECTS_POINT_OUT(x, y, w, h, xx, yy) (((xx) < (x)) || ((yy) < (y)) || ((xx) > ((x) + (w))) || ((yy) > ((y) + (h))))