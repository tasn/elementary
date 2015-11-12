#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#include <Elementary.h>

/* We zoom out to this value so we'll be able to use map and have a nice
 * resolution when zooming in. */
#define BASE_ZOOM 0.5
/* The amount of zoom to do when "lifting" objects. */
#define LIFT_FACTOR 1.3
/* The base size of the shadow image. */
#define SHADOW_W 118
#define SHADOW_H 118

static double zoom_out_animation_duration = 0.4;

struct _Photo_Object {
     Evas_Object *ic, *shadow;
     Evas_Object *hit;
     Evas_Object *gl;
     Elm_Transit *zoom_out;
     /* bx, by - current wanted coordinates of the photo object.
      * bw, bh - original size of the "ic" object.
      * dx, dy - Used to indicate the distance between the center point
      * where we put down our fingers (when started moving the item) to
      * the coords of the object, so we'll be able to calculate movement
      * correctly. */
     Evas_Coord bx, by, bw, bh, dx, dy;
     /* Because gesture layer only knows the amount of rotation/zoom we do
      * per gesture, we have to keep the current rotate/zoom factor and the
      * one that was before we started the gesture. */
     int base_rotate, rotate;
     double base_zoom, zoom;
     double shadow_zoom;
};
typedef struct _Photo_Object Photo_Object;

/* This function applies the information from the Photo_Object to the actual
 * evas objects. Zoom/rotate factors and etc. */
static void
apply_changes(Photo_Object *po)
{
   Evas_Map *map;

   map = evas_map_new(4);
   evas_map_point_coord_set(map, 0, po->bx, po->by, 0);
   evas_map_point_coord_set(map, 1, po->bx + po->bw, po->by, 0);
   evas_map_point_coord_set(map, 2, po->bx + po->bw, po->by + po->bh, 0);
   evas_map_point_coord_set(map, 3, po->bx, po->by + po->bh, 0);
   evas_map_point_image_uv_set(map, 0, 0, 0);
   evas_map_point_image_uv_set(map, 1, po->bw, 0);
   evas_map_point_image_uv_set(map, 2, po->bw, po->bh);
   evas_map_point_image_uv_set(map, 3, 0, po->bh);
   evas_map_util_rotate(map, po->rotate,
         po->bx + po->bw / 2, po->by + po->bh /2);
   evas_map_util_zoom(map, po->zoom, po->zoom,
         po->bx + po->bw / 2, po->by + po->bh /2);
   evas_object_map_enable_set(po->ic, EINA_TRUE);
   evas_object_map_set(po->ic, map);

     {
        Evas_Map *shadow_map = evas_map_new(4);
        evas_map_point_coord_set(shadow_map, 0, po->bx, po->by, 0);
        evas_map_point_coord_set(shadow_map, 1, po->bx + po->bw, po->by, 0);
        evas_map_point_coord_set(shadow_map, 2, po->bx + po->bw, po->by + po->bh, 0);
        evas_map_point_coord_set(shadow_map, 3, po->bx, po->by + po->bh, 0);
        evas_map_point_image_uv_set(shadow_map, 0, 0, 0);
        evas_map_point_image_uv_set(shadow_map, 1, SHADOW_W, 0);
        evas_map_point_image_uv_set(shadow_map, 2, SHADOW_W, SHADOW_H);
        evas_map_point_image_uv_set(shadow_map, 3, 0, SHADOW_H);
        evas_map_util_rotate(shadow_map, po->rotate,
              po->bx + po->bw / 2, po->by + po->bh /2);
        evas_map_util_zoom(shadow_map, po->zoom * po->shadow_zoom,
              po->zoom * po->shadow_zoom,
              po->bx + (po->bw / 2), po->by + (po->bh / 2));
        evas_object_map_enable_set(po->shadow, EINA_TRUE);
        evas_object_map_set(po->shadow, shadow_map);
        evas_map_free(shadow_map);
     }

   /* Update the position of the hit box */
     {
        Evas_Coord minx, miny, maxx, maxy;
        int i;
        evas_object_polygon_points_clear(po->hit);
        evas_map_point_coord_get(map, 0, &minx, &miny, NULL);
        maxx = minx;
        maxy = miny;
        evas_object_polygon_point_add(po->hit, minx, miny);
        for (i = 1 ; i <= 3 ; i++)
          {
             Evas_Coord x, y;
             evas_map_point_coord_get(map, i, &x, &y, NULL);
             evas_object_polygon_point_add(po->hit, x, y);
             if (x < minx)
                minx = x;
             else if (x > maxx)
                maxx = x;

             if (y < miny)
                miny = y;
             else if (y > maxy)
                maxy = y;
          }
     }

   evas_object_raise(po->shadow);
   evas_object_raise(po->ic);
   evas_object_raise(po->hit);
   evas_map_free(map);
}

/* Zoom out animation */
static void
zoom_out_animation_operation(void *_po, Elm_Transit *transit EINA_UNUSED,
      double progress)
{
   Photo_Object *po = (Photo_Object *) _po;
   po->zoom = BASE_ZOOM + ((po->base_zoom - BASE_ZOOM) * (1.0 - progress));
   apply_changes(po);
}

static void
zoom_out_animation_end(void *_po, Elm_Transit *transit EINA_UNUSED)
{
   Photo_Object *po = (Photo_Object *) _po;

   po->base_zoom = po->zoom = BASE_ZOOM;
   apply_changes(po);

   po->zoom_out = NULL;
}

static Evas_Event_Flags
rotate_move(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Rotate_Info *p = (Elm_Gesture_Rotate_Info *) event_info;
   printf("rotate move <%d,%d> base=<%f> <%f>\n", p->x, p->y, p->base_angle, p->angle);
   po->rotate = po->base_rotate + (int) p->angle - p->base_angle;
   if (po->rotate < 0)
      po->rotate += 360;
   apply_changes(po);
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
rotate_end(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Rotate_Info *p = (Elm_Gesture_Rotate_Info *) event_info;
   printf("rotate end/abort <%d,%d> base=<%f> <%f>\n", p->x, p->y, p->base_angle, p->angle);
   po->base_rotate += (int) p->angle - p->base_angle;
   if (po->rotate < 0)
      po->rotate += 360;
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
zoom_start(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   printf("zoom start <%d,%d> <%f>\n", p->x, p->y, p->zoom);

   /* If there's an active animator, stop it */
   if (po->zoom_out)
     {
        elm_transit_del(po->zoom_out);
        po->zoom_out = NULL;
     }

   /* Give it a "lift" effect right from the start */
   po->base_zoom = BASE_ZOOM * LIFT_FACTOR;
   po->zoom = po->base_zoom;
   po->shadow_zoom = 1.7;

   apply_changes(po);
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
zoom_move(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   printf("zoom move <%d,%d> <%f>\n", p->x, p->y, p->zoom);
   po->zoom = po->base_zoom * p->zoom;
   apply_changes(po);
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
zoom_end(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   printf("zoom end/abort <%d,%d> <%f>\n", p->x, p->y, p->zoom);

   /* Apply the zoom out animator */
   po->shadow_zoom = 1.3;
   po->base_zoom = po->zoom;
   po->zoom_out = elm_transit_add();
   elm_transit_duration_set(po->zoom_out, zoom_out_animation_duration);
   elm_transit_effect_add(po->zoom_out, zoom_out_animation_operation, po, zoom_out_animation_end);
   elm_transit_go(po->zoom_out);
   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
momentum_start(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Momentum_Info *p = (Elm_Gesture_Momentum_Info *) event_info;
   printf("momentum_start <%d,%d>\n", p->x2, p->y2);

   po->dx = p->x2 - po->bx;
   po->dy = p->y2 - po->by;
   apply_changes(po);

   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
momentum_move(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Momentum_Info *p = (Elm_Gesture_Momentum_Info *) event_info;
   printf("momentum move <%d,%d>\n", p->x2, p->y2);

   po->bx = p->x2 - po->dx;
   po->by = p->y2 - po->dy;
   apply_changes(po);

   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
momentum_end(void *_po, void *event_info)
{
   Photo_Object *po = (Photo_Object *) _po;
   Elm_Gesture_Momentum_Info *p = (Elm_Gesture_Momentum_Info *) event_info;
   printf("momentum end/abort <%d,%d> <%d,%d>\n", p->x2, p->y2, p->mx, p->my);
   (void) po;
   (void) p;
   /* Make sure middle is in the screen, if not, fix it. */
     {
        /* FIXME: Use actual window sizes instead of the hardcoded
         * values */
        Evas_Coord mx, my;
        mx = po->bx + (po->bw / 2);
        my = po->by + (po->bh / 2);
        if (mx < 0)
           po->bx = 0 - (po->bw / 2);
        else if (mx > 480)
           po->bx = 480 - (po->bw / 2);

        if (my < 0)
           po->by = 0 - (po->bw / 2);
        else if (my > 800)
           po->by = 800 - (po->bh / 2);
     }
   apply_changes(po);

   return EVAS_EVENT_FLAG_NONE;
}

static void
_win_del_req(void *data, Evas_Object *obj EINA_UNUSED,
      void *event_info EINA_UNUSED)
{
   Photo_Object **photo_array = (Photo_Object **) data;

   if (!photo_array)
      return;

   /* The content of the photo object is automatically deleted when the win
    * is deleted. */
   for ( ; *photo_array ; photo_array++)
      free(*photo_array);

   free(data);
}

Photo_Object *
photo_object_add(Evas_Object *parent, Evas_Object *ic, const char *icon, Evas_Coord x,
      Evas_Coord y, Evas_Coord w, Evas_Coord h, int angle)
{
   char buf[PATH_MAX];
   Photo_Object *po;
   po = calloc(1, sizeof(*po));
   po->base_zoom = po->zoom = BASE_ZOOM;

   if (ic)
     {
        po->ic = ic;
     }
   else
     {
        po->ic = elm_icon_add(parent);
        elm_image_file_set(po->ic, icon, NULL);
     }

   po->bx = x;
   po->by = y;
   po->bw = w;
   po->bh = h;

   /* Add shadow */
     {
        po->shadow = elm_icon_add(po->ic);
        snprintf(buf, sizeof(buf), "%s/images/pol_shadow.png", elm_app_data_dir_get());
        elm_image_file_set(po->shadow, buf, NULL);
        evas_object_resize(po->shadow, SHADOW_W, SHADOW_H);
        evas_object_show(po->shadow);
     }

   po->hit = evas_object_polygon_add(evas_object_evas_get(parent));
   evas_object_precise_is_inside_set(po->hit, EINA_TRUE);
   evas_object_repeat_events_set(po->hit, EINA_TRUE);
   evas_object_color_set(po->hit, 0, 0, 0, 0);

   evas_object_move(po->ic, 0, 0);
   evas_object_resize(po->ic, po->bw, po->bh);
   evas_object_show(po->ic);

   evas_object_show(po->hit);

   po->gl = elm_gesture_layer_add(po->ic);
   elm_gesture_layer_hold_events_set(po->gl, EINA_TRUE);
   elm_gesture_layer_attach(po->gl, po->hit);

   /* FIXME: Add a po->rotate start so we take the first angle!!!! */
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_ROTATE, ELM_GESTURE_STATE_MOVE, rotate_move, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_ROTATE, ELM_GESTURE_STATE_END, rotate_end, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_ROTATE, ELM_GESTURE_STATE_ABORT, rotate_end, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_START, zoom_start, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_MOVE, zoom_move, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_END, zoom_end, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_ABORT, zoom_end, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_MOMENTUM, ELM_GESTURE_STATE_START, momentum_start, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_MOMENTUM, ELM_GESTURE_STATE_MOVE, momentum_move, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_MOMENTUM, ELM_GESTURE_STATE_END, momentum_end, po);
   elm_gesture_layer_cb_set(po->gl, ELM_GESTURE_MOMENTUM, ELM_GESTURE_STATE_ABORT, momentum_end, po);

   po->rotate = po->base_rotate = angle;
   po->shadow_zoom = 1.3;

   apply_changes(po);
   return po;
}

void
test_gesture_layer(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
      void *event_info EINA_UNUSED)
{
   Evas_Coord w, h;
   Evas_Object *win, *bg;
   char buf[PATH_MAX];
   int ind = 0;
   Photo_Object **photo_array;
   photo_array = calloc(sizeof(*photo_array), 4);

   w = 480;
   h = 800;

   win = elm_win_add(NULL, "gesture-layer", ELM_WIN_BASIC);
   elm_win_title_set(win, "Gesture Layer");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_resize(win, w, h);

   bg = elm_bg_add(win);
   snprintf(buf, sizeof(buf), "%s/images/wood_01.jpg", elm_app_data_dir_get());
   elm_bg_file_set(bg, buf, NULL);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   snprintf(buf, sizeof(buf), "%s/images/pol_sky.png", elm_app_data_dir_get());
   photo_array[ind++] = photo_object_add(win, NULL, buf, 200, 200, 365, 400, 0);
   snprintf(buf, sizeof(buf), "%s/images/pol_twofish.png", elm_app_data_dir_get());
   photo_array[ind++] = photo_object_add(win, NULL, buf, 40, 300, 365, 400, 45);

   Evas_Object *en = elm_entry_add(win);
   elm_object_text_set(en, "<font_size=40>You can use whatever object you want, "
         "even entries like this.</font_size>");
   elm_entry_line_wrap_set(en, ELM_WRAP_MIXED);

   Evas_Object *postit = elm_layout_add(win);
   snprintf(buf, sizeof(buf), "%s/objects/postit_ent.edj", elm_app_data_dir_get());
   elm_layout_file_set(postit, buf, "main");
   elm_object_part_content_set(postit, "ent", en);

   photo_array[ind++] = photo_object_add(win, postit, NULL, 50, 50, 382, 400, 355);

   photo_array[ind] = NULL;
   evas_object_smart_callback_add(win, "delete,request", _win_del_req,
         photo_array);
   evas_object_show(win);
}
