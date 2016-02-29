#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "elm_widget_photocam.h"
#include "elm_interface_scrollable.h"

#define MY_PAN_CLASS ELM_PHOTOCAM_PAN_CLASS

#define MY_PAN_CLASS_NAME "Elm_Photocam_Pan"
#define MY_PAN_CLASS_NAME_LEGACY "elm_photocam_pan"

#define MY_CLASS ELM_PHOTOCAM_CLASS

#define MY_CLASS_NAME "Elm_Photocam"
#define MY_CLASS_NAME_LEGACY "elm_photocam"

/*
 * TODO (maybe - optional future stuff):
 *
 * 1. wrap photo in theme edje so u can have styling around photo (like white
 *    photo bordering).
 * 2. exif handling
 * 3. rotation flags in exif handling (nasty! should have rot in evas)
 */

static const char SIG_CLICKED[] = "clicked";
static const char SIG_PRESS[] = "press";
static const char SIG_LONGPRESSED[] = "longpressed";
static const char SIG_CLICKED_DOUBLE[] = "clicked,double";
static const char SIG_LOAD[] = "load";
static const char SIG_LOADED[] = "loaded";
static const char SIG_LOAD_DETAIL[] = "load,detail";
static const char SIG_LOADED_DETAIL[] = "loaded,detail";
static const char SIG_ZOOM_START[] = "zoom,start";
static const char SIG_ZOOM_STOP[] = "zoom,stop";
static const char SIG_ZOOM_CHANGE[] = "zoom,change";
static const char SIG_SCROLL[] = "scroll";
static const char SIG_SCROLL_ANIM_START[] = "scroll,anim,start";
static const char SIG_SCROLL_ANIM_STOP[] = "scroll,anim,stop";
static const char SIG_SCROLL_DRAG_START[] = "scroll,drag,start";
static const char SIG_SCROLL_DRAG_STOP[] = "scroll,drag,stop";
static const char SIG_DOWNLOAD_START[] = "download,start";
static const char SIG_DOWNLOAD_PROGRESS[] = "download,progress";
static const char SIG_DOWNLOAD_DONE[] = "download,done";
static const char SIG_DOWNLOAD_ERROR[] = "download,error";
static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_CLICKED, ""},
   {SIG_PRESS, ""},
   {SIG_LONGPRESSED, ""},
   {SIG_CLICKED_DOUBLE, ""},
   {SIG_LOAD, ""},
   {SIG_LOADED, ""},
   {SIG_LOAD_DETAIL, ""},
   {SIG_LOADED_DETAIL, ""},
   {SIG_ZOOM_START, ""},
   {SIG_ZOOM_STOP, ""},
   {SIG_ZOOM_CHANGE, ""},
   {SIG_SCROLL, ""},
   {SIG_SCROLL_ANIM_START, ""},
   {SIG_SCROLL_ANIM_STOP, ""},
   {SIG_SCROLL_DRAG_START, ""},
   {SIG_SCROLL_DRAG_STOP, ""},
   {SIG_DOWNLOAD_START, ""},
   {SIG_DOWNLOAD_PROGRESS, ""},
   {SIG_DOWNLOAD_DONE, ""},
   {SIG_DOWNLOAD_ERROR, ""},
   {SIG_WIDGET_FOCUSED, ""}, /**< handled by elm_widget */
   {SIG_WIDGET_UNFOCUSED, ""}, /**< handled by elm_widget */
   {NULL, NULL}
};

static Eina_Error PHOTO_FILE_LOAD_ERROR_GENERIC;
static Eina_Error PHOTO_FILE_LOAD_ERROR_DOES_NOT_EXIST;
static Eina_Error PHOTO_FILE_LOAD_ERROR_PERMISSION_DENIED;
static Eina_Error PHOTO_FILE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED;
static Eina_Error PHOTO_FILE_LOAD_ERROR_CORRUPT_FILE;
static Eina_Error PHOTO_FILE_LOAD_ERROR_UNKNOWN_FORMAT;

static Eina_Bool _key_action_move(Evas_Object *obj, const char *params);
static Eina_Bool _key_action_zoom(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"move", _key_action_move},
   {"zoom", _key_action_zoom},
   {NULL, NULL}
};

static inline void
_photocam_image_file_set(Evas_Object *obj, Elm_Photocam_Data *sd)
{
   if (sd->f)
     evas_object_image_mmap_set(obj, sd->f, NULL);
   else
     evas_object_image_file_set(obj, sd->file, NULL);
}

static void
_sizing_eval(Evas_Object *obj)
{
   Evas_Coord minw = 0, minh = 0, maxw = -1, maxh = -1;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   evas_object_size_hint_max_get
     (wd->resize_obj, &maxw, &maxh);
   evas_object_size_hint_min_set(obj, minw, minh);
   evas_object_size_hint_max_set(obj, maxw, maxh);
}

static void
_calc_job_cb(void *data)
{
   Evas_Object *obj = data;
   ELM_PHOTOCAM_DATA_GET(obj, sd);
   Evas_Coord minw, minh;

   minw = sd->size.w;
   minh = sd->size.h;
   if (sd->resized)
     {
        sd->resized = EINA_FALSE;
        if (sd->mode != ELM_PHOTOCAM_ZOOM_MODE_MANUAL)
          {
             double tz = sd->zoom;
             sd->zoom = 0.0;
             elm_photocam_zoom_set(obj, tz);
          }
     }
   if ((minw != sd->minw) || (minh != sd->minh))
     {
        sd->minw = minw;
        sd->minh = minh;

        eo_do(sd->pan_obj, eo_event_callback_call(ELM_PAN_EVENT_CHANGED, NULL));
        _sizing_eval(obj);
     }
   sd->calc_job = NULL;
   evas_object_smart_changed(sd->pan_obj);
}

EOLIAN static void
_elm_photocam_pan_evas_object_smart_move(Eo *obj EINA_UNUSED, Elm_Photocam_Pan_Data *psd, Evas_Coord x EINA_UNUSED, Evas_Coord y EINA_UNUSED)
{
   ecore_job_del(psd->wsd->calc_job);
   psd->wsd->calc_job = ecore_job_add(_calc_job_cb, psd->wobj);
}

EOLIAN static void
_elm_photocam_pan_evas_object_smart_resize(Eo *obj, Elm_Photocam_Pan_Data *psd, Evas_Coord w, Evas_Coord h)
{
   Evas_Coord ow, oh;

   evas_object_geometry_get(obj, NULL, NULL, &ow, &oh);
   if ((ow == w) && (oh == h)) return;

   psd->wsd->resized = EINA_TRUE;
   ecore_job_del(psd->wsd->calc_job);
   psd->wsd->calc_job = ecore_job_add(_calc_job_cb, psd->wobj);
}

static void
_image_place(Evas_Object *obj,
             Evas_Coord px,
             Evas_Coord py,
             Evas_Coord ox,
             Evas_Coord oy,
             Evas_Coord ow,
             Evas_Coord oh)
{
   Evas_Coord ax, ay, gw, gh;

   ELM_PHOTOCAM_DATA_GET(obj, sd);

   ax = 0;
   ay = 0;
   gw = sd->size.w;
   gh = sd->size.h;
   if (!sd->zoom_g_layer)
     {
        if (ow > gw) ax = (ow - gw) / 2;
        if (oh > gh) ay = (oh - gh) / 2;
     }
   evas_object_move(sd->img, ox + 0 - px + ax, oy + 0 - py + ay);
   evas_object_resize(sd->img, gw, gh);

   if (sd->show.show)
     {
        sd->show.show = EINA_FALSE;
        eo_do(obj, elm_interface_scrollable_content_region_show
              (sd->show.x, sd->show.y, sd->show.w, sd->show.h));
     }
}

static void
_grid_load(Evas_Object *obj,
           Elm_Phocam_Grid *g)
{
   int x, y;
   Evas_Coord ox, oy, ow, oh, cvx, cvy, cvw, cvh, gw, gh, tx, ty;

   ELM_PHOTOCAM_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   evas_object_geometry_get(sd->pan_obj, &ox, &oy, &ow, &oh);
   evas_output_viewport_get(evas_object_evas_get(obj), &cvx, &cvy, &cvw, &cvh);

   gw = sd->size.w;
   gh = sd->size.h;
   for (y = 0; y < g->gh; y++)
     {
        for (x = 0; x < g->gw; x++)
          {
             int tn, xx, yy, ww, hh;
             Eina_Bool visible = EINA_FALSE;

             tn = (y * g->gw) + x;
             xx = g->grid[tn].out.x;
             yy = g->grid[tn].out.y;
             ww = g->grid[tn].out.w;
             hh = g->grid[tn].out.h;
             if ((gw != g->w) && (g->w > 0))
               {
                  tx = xx;
                  xx = (gw * xx) / g->w;
                  ww = ((gw * (tx + ww)) / g->w) - xx;
               }
             if ((gh != g->h) && (g->h > 0))
               {
                  ty = yy;
                  yy = (gh * yy) / g->h;
                  hh = ((gh * (ty + hh)) / g->h) - yy;
               }
             if (ELM_RECTS_INTERSECT(xx - sd->pan_x + ox,
                                     yy - sd->pan_y + oy,
                                     ww, hh, cvx, cvy, cvw, cvh))
               visible = EINA_TRUE;
             if ((visible) && (!g->grid[tn].have) && (!g->grid[tn].want))
               {
                  g->grid[tn].want = 1;
                  evas_object_hide(g->grid[tn].img);
                  evas_object_image_file_set(g->grid[tn].img, NULL, NULL);
                  evas_object_image_load_scale_down_set
                    (g->grid[tn].img, g->zoom);
                  evas_object_image_load_region_set
                    (g->grid[tn].img, g->grid[tn].src.x, g->grid[tn].src.y,
                    g->grid[tn].src.w, g->grid[tn].src.h);
                  _photocam_image_file_set(g->grid[tn].img, sd);
                  evas_object_image_preload(g->grid[tn].img, 0);
                  sd->preload_num++;
                  if (sd->preload_num == 1)
                    {
                       edje_object_signal_emit
                         (wd->resize_obj,
                         "elm,state,busy,start", "elm");
                       eo_do(obj, eo_event_callback_call
                        (ELM_PHOTOCAM_EVENT_LOAD_DETAIL, NULL));
                    }
               }
             else if ((g->grid[tn].want) && (!visible))
               {
                  sd->preload_num--;
                  if (!sd->preload_num)
                    {
                       edje_object_signal_emit
                         (wd->resize_obj,
                         "elm,state,busy,stop", "elm");
                       eo_do(obj, eo_event_callback_call
                        (ELM_PHOTOCAM_EVENT_LOADED_DETAIL, NULL));
                    }
                  g->grid[tn].want = 0;
                  evas_object_hide(g->grid[tn].img);
                  evas_object_image_preload(g->grid[tn].img, 1);
                  evas_object_image_file_set(g->grid[tn].img, NULL, NULL);
               }
             else if ((g->grid[tn].have) && (!visible))
               {
                  g->grid[tn].have = 0;
                  evas_object_hide(g->grid[tn].img);
                  evas_object_image_preload(g->grid[tn].img, 1);
                  evas_object_image_file_set(g->grid[tn].img, NULL, NULL);
               }
          }
     }
}

static void
_grid_place(Evas_Object *obj,
            Elm_Phocam_Grid *g,
            Evas_Coord px,
            Evas_Coord py,
            Evas_Coord ox,
            Evas_Coord oy,
            Evas_Coord ow,
            Evas_Coord oh)
{
   Evas_Coord ax, ay, gw, gh, tx, ty;
   int x, y;

   ELM_PHOTOCAM_DATA_GET(obj, sd);

   ax = 0;
   ay = 0;
   gw = sd->size.w;
   gh = sd->size.h;
   if (!sd->zoom_g_layer)
     {
        if (ow > gw) ax = (ow - gw) / 2;
        if (oh > gh) ay = (oh - gh) / 2;
     }
   for (y = 0; y < g->gh; y++)
     {
        for (x = 0; x < g->gw; x++)
          {
             int tn, xx, yy, ww, hh;

             tn = (y * g->gw) + x;
             xx = g->grid[tn].out.x;
             yy = g->grid[tn].out.y;
             ww = g->grid[tn].out.w;
             hh = g->grid[tn].out.h;
             if ((gw != g->w) && (g->w > 0))
               {
                  tx = xx;
                  xx = (gw * xx) / g->w;
                  ww = ((gw * (tx + ww)) / g->w) - xx;
               }
             if ((gh != g->h) && (g->h > 0))
               {
                  ty = yy;
                  yy = (gh * yy) / g->h;
                  hh = ((gh * (ty + hh)) / g->h) - yy;
               }
             evas_object_move(g->grid[tn].img,
                              ox + xx - px + ax,
                              oy + yy - py + ay);
             evas_object_resize(g->grid[tn].img, ww, hh);
          }
     }
}

EOLIAN static void
_elm_photocam_pan_evas_object_smart_calculate(Eo *obj, Elm_Photocam_Pan_Data *psd)
{
   Elm_Phocam_Grid *g;
   Eina_List *l;
   Evas_Coord ox, oy, ow, oh;

   ELM_WIDGET_DATA_GET_OR_RETURN(psd->wobj, wd);

   evas_object_geometry_get(obj, &ox, &oy, &ow, &oh);
   _image_place(
       wd->obj, psd->wsd->pan_x, psd->wsd->pan_y,
       ox - psd->wsd->g_layer_zoom.imx, oy - psd->wsd->g_layer_zoom.imy, ow,
       oh);

   EINA_LIST_FOREACH(psd->wsd->grids, l, g)
     {
        _grid_load(wd->obj, g);
        _grid_place(
              wd->obj, g, psd->wsd->pan_x,
             psd->wsd->pan_y, ox - psd->wsd->g_layer_zoom.imx,
             oy - psd->wsd->g_layer_zoom.imy, ow, oh);
     }
}

EOLIAN static void
_elm_photocam_pan_elm_pan_pos_set(Eo *obj, Elm_Photocam_Pan_Data *psd, Evas_Coord x, Evas_Coord y)
{
   if ((x == psd->wsd->pan_x) && (y == psd->wsd->pan_y)) return;
   psd->wsd->pan_x = x;
   psd->wsd->pan_y = y;
   evas_object_smart_changed(obj);
}

EOLIAN static void
_elm_photocam_pan_elm_pan_pos_get(Eo *obj EINA_UNUSED, Elm_Photocam_Pan_Data *psd, Evas_Coord *x, Evas_Coord *y)
{
   if (x) *x = psd->wsd->pan_x;
   if (y) *y = psd->wsd->pan_y;
}

EOLIAN static void
_elm_photocam_pan_elm_pan_pos_max_get(Eo *obj, Elm_Photocam_Pan_Data *psd, Evas_Coord *x, Evas_Coord *y)
{
   Evas_Coord ow, oh;

   evas_object_geometry_get(obj, NULL, NULL, &ow, &oh);
   ow = psd->wsd->minw - ow;
   if (ow < 0) ow = 0;
   oh = psd->wsd->minh - oh;
   if (oh < 0) oh = 0;
   if (x) *x = ow;
   if (y) *y = oh;
}

EOLIAN static void
_elm_photocam_pan_elm_pan_pos_min_get(Eo *obj EINA_UNUSED, Elm_Photocam_Pan_Data *_pd EINA_UNUSED, Evas_Coord *x, Evas_Coord *y)
{
   if (x) *x = 0;
   if (y) *y = 0;
}

EOLIAN static void
_elm_photocam_pan_elm_pan_content_size_get(Eo *obj EINA_UNUSED, Elm_Photocam_Pan_Data *psd, Evas_Coord *w, Evas_Coord *h)
{
   if (w) *w = psd->wsd->minw;
   if (h) *h = psd->wsd->minh;
}

EOLIAN static void
_elm_photocam_pan_eo_base_destructor(Eo *obj, Elm_Photocam_Pan_Data *psd)
{
   eo_data_unref(psd->wobj, psd->wsd);
   eo_do_super(obj, MY_PAN_CLASS, eo_destructor());
}

static void
_elm_photocam_pan_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_PAN_CLASS_NAME_LEGACY, klass);
}

#include "elm_photocam_pan.eo.c"

static int
_nearest_pow2_get(int num)
{
   unsigned int n = num - 1;

   n |= n >> 1;
   n |= n >> 2;
   n |= n >> 4;
   n |= n >> 8;
   n |= n >> 16;

   return n + 1;
}

static void
_grid_clear(Evas_Object *obj,
            Elm_Phocam_Grid *g)
{
   int x, y;

   ELM_PHOTOCAM_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   if (!g->grid) return;
   for (y = 0; y < g->gh; y++)
     {
        for (x = 0; x < g->gw; x++)
          {
             int tn;

             tn = (y * g->gw) + x;
             evas_object_del(g->grid[tn].img);
             if (g->grid[tn].want)
               {
                  sd->preload_num--;
                  if (!sd->preload_num)
                    {
                       edje_object_signal_emit
                         (wd->resize_obj,
                         "elm,state,busy,stop", "elm");
                       eo_do(obj, eo_event_callback_call
                         (ELM_PHOTOCAM_EVENT_LOAD_DETAIL, NULL));
                    }
               }
          }
     }

   ELM_SAFE_FREE(g->grid, free);
   g->gw = 0;
   g->gh = 0;
}

static void
_tile_preloaded_cb(void *data,
                   Evas *e EINA_UNUSED,
                   Evas_Object *o EINA_UNUSED,
                   void *event_info EINA_UNUSED)
{
   Elm_Photocam_Grid_Item *git = data;
   ELM_PHOTOCAM_DATA_GET(git->obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(git->obj, wd);

   if (git->want)
     {
        git->want = 0;
        evas_object_show(git->img);
        git->have = 1;
        sd->preload_num--;
        if (!sd->preload_num)
          {
             edje_object_signal_emit
               (wd->resize_obj, "elm,state,busy,stop",
               "elm");
             eo_do(wd->obj, eo_event_callback_call
               (ELM_PHOTOCAM_EVENT_LOADED_DETAIL, NULL));
          }
     }
}

static int
_grid_zoom_calc(double zoom)
{
   int z = zoom;

   if (z < 1) z = 1;
   return _nearest_pow2_get(z);
}

static Elm_Phocam_Grid *
_grid_create(Evas_Object *obj)
{
   int x, y;
   Elm_Phocam_Grid *g;

   ELM_PHOTOCAM_DATA_GET(obj, sd);

   g = calloc(1, sizeof(Elm_Phocam_Grid));
   if (!g) return NULL;

   g->zoom = _grid_zoom_calc(sd->zoom);
   g->tsize = sd->tsize;
   g->iw = sd->size.imw;
   g->ih = sd->size.imh;

   g->w = g->iw / g->zoom;
   g->h = g->ih / g->zoom;
   if (g->zoom >= 8)
     {
        free(g);

        return NULL;
     }
   if (sd->do_region)
     {
        g->gw = (g->w + g->tsize - 1) / g->tsize;
        g->gh = (g->h + g->tsize - 1) / g->tsize;
     }
   else
     {
        g->gw = 1;
        g->gh = 1;
     }

   g->grid = calloc(1, sizeof(Elm_Photocam_Grid_Item) * g->gw * g->gh);
   if (!g->grid)
     {
        g->gw = 0;
        g->gh = 0;

        return g;
     }

   for (y = 0; y < g->gh; y++)
     {
        for (x = 0; x < g->gw; x++)
          {
             int tn;

             tn = (y * g->gw) + x;
             g->grid[tn].src.x = x * g->tsize;
             if (x == (g->gw - 1))
               g->grid[tn].src.w = g->w - ((g->gw - 1) * g->tsize);
             else
               g->grid[tn].src.w = g->tsize;
             g->grid[tn].src.y = y * g->tsize;
             if (y == (g->gh - 1))
               g->grid[tn].src.h = g->h - ((g->gh - 1) * g->tsize);
             else
               g->grid[tn].src.h = g->tsize;

             g->grid[tn].out.x = g->grid[tn].src.x;
             g->grid[tn].out.y = g->grid[tn].src.y;
             g->grid[tn].out.w = g->grid[tn].src.w;
             g->grid[tn].out.h = g->grid[tn].src.h;

             g->grid[tn].obj = obj;
             g->grid[tn].img =
               evas_object_image_add(evas_object_evas_get(obj));
             evas_object_image_load_orientation_set(g->grid[tn].img, EINA_TRUE);
             evas_object_image_orient_set(g->grid[tn].img, sd->orient);
             evas_object_image_scale_hint_set
               (g->grid[tn].img, EVAS_IMAGE_SCALE_HINT_DYNAMIC);
             evas_object_pass_events_set(g->grid[tn].img, EINA_TRUE);

             /* XXX: check this */
             evas_object_smart_member_add(g->grid[tn].img, sd->pan_obj);
             elm_widget_sub_object_add(obj, g->grid[tn].img);
             evas_object_image_filled_set(g->grid[tn].img, 1);
             evas_object_event_callback_add
               (g->grid[tn].img, EVAS_CALLBACK_IMAGE_PRELOADED,
               _tile_preloaded_cb, &(g->grid[tn]));
          }
     }

   return g;
}

static void
_grid_clear_all(Evas_Object *obj)
{
   Elm_Phocam_Grid *g;

   ELM_PHOTOCAM_DATA_GET(obj, sd);

   EINA_LIST_FREE(sd->grids, g)
     {
        _grid_clear(obj, g);
        free(g);
     }
}

static void
_smooth_update(Evas_Object *obj)
{
   Elm_Phocam_Grid *g;
   int x, y;
   Eina_List *l;

   ELM_PHOTOCAM_DATA_GET(obj, sd);

   EINA_LIST_FOREACH(sd->grids, l, g)
     {
        for (y = 0; y < g->gh; y++)
          {
             for (x = 0; x < g->gw; x++)
               {
                  int tn;

                  tn = (y * g->gw) + x;
                  evas_object_image_smooth_scale_set
                    (g->grid[tn].img, (!sd->no_smooth));
               }
          }
     }

   evas_object_image_smooth_scale_set(sd->img, (!sd->no_smooth));
}

static void
_grid_raise(Elm_Phocam_Grid *g)
{
   int x, y;

   for (y = 0; y < g->gh; y++)
     {
        for (x = 0; x < g->gw; x++)
          {
             int tn;

             tn = (y * g->gw) + x;
             evas_object_raise(g->grid[tn].img);
          }
     }
}

static Eina_Bool
_scroll_timeout_cb(void *data)
{
   ELM_PHOTOCAM_DATA_GET(data, sd);

   sd->no_smooth--;
   if (!sd->no_smooth) _smooth_update(data);

   sd->scr_timer = NULL;

   return ECORE_CALLBACK_CANCEL;
}

static void
_main_img_preloaded_cb(void *data,
                       Evas *e EINA_UNUSED,
                       Evas_Object *o EINA_UNUSED,
                       void *event_info EINA_UNUSED)
{
   Evas_Object *obj = data;
   Elm_Phocam_Grid *g;

   ELM_PHOTOCAM_DATA_GET(data, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(data, wd);

   evas_object_show(sd->img);
   sd->main_load_pending = 0;
   g = _grid_create(obj);
   if (g)
     {
        sd->grids = eina_list_prepend(sd->grids, g);
        _grid_load(obj, g);
     }
   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job_cb, data);
   eo_do(data, eo_event_callback_call(ELM_PHOTOCAM_EVENT_LOADED, NULL));
   sd->preload_num--;
   if (!sd->preload_num)
     {
        edje_object_signal_emit
          (wd->resize_obj, "elm,state,busy,stop", "elm");
        eo_do(obj, eo_event_callback_call
          (ELM_PHOTOCAM_EVENT_LOADED_DETAIL, NULL));
     }
}

static Eina_Bool
_zoom_do(Evas_Object *obj,
         double t)
{
   Evas_Coord xx, yy, ow = 0, oh = 0;

   ELM_PHOTOCAM_DATA_GET(obj, sd);

   sd->size.w = (sd->size.ow * (1.0 - t)) + (sd->size.nw * t);
   sd->size.h = (sd->size.oh * (1.0 - t)) + (sd->size.nh * t);
   eo_do(obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &ow, &oh));
   xx = (sd->size.spos.x * sd->size.w) - (ow / 2);
   yy = (sd->size.spos.y * sd->size.h) - (oh / 2);
   if (xx < 0) xx = 0;
   else if (xx > (sd->size.w - ow))
     xx = sd->size.w - ow;
   if (yy < 0) yy = 0;
   else if (yy > (sd->size.h - oh))
     yy = sd->size.h - oh;

   sd->show.show = EINA_TRUE;
   sd->show.x = xx;
   sd->show.y = yy;
   sd->show.w = ow;
   sd->show.h = oh;

   if (sd->orientation_changed)
     {
        evas_object_smart_member_del(sd->img);
        elm_widget_sub_object_del(obj, sd->img);
        evas_object_smart_member_add(sd->img, sd->pan_obj);
        elm_widget_sub_object_add(obj, sd->img);
     }
   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job_cb, obj);
   if (t >= 1.0)
     {
        Eina_List *l, *l_next;
        Elm_Phocam_Grid *g;

        EINA_LIST_FOREACH_SAFE(sd->grids, l, l_next, g)
          {
             if (g->dead)
               {
                  sd->grids = eina_list_remove_list(sd->grids, l);
                  _grid_clear(obj, g);
                  free(g);
               }
          }
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

static Eina_Bool
_zoom_anim_cb(void *data, const Eo_Event *event EINA_UNUSED)
{
   double t;
   Evas_Object *obj = data;

   ELM_PHOTOCAM_DATA_GET(obj, sd);

   t = ecore_loop_time_get();
   if (t >= sd->t_end)
     t = 1.0;
   else if (sd->t_end > sd->t_start)
     t = (t - sd->t_start) / (sd->t_end - sd->t_start);
   else
     t = 1.0;
   t = 1.0 - t;
   t = 1.0 - (t * t);
   if (!_zoom_do(obj, t))
     {
        sd->no_smooth--;
        if (!sd->no_smooth) _smooth_update(data);
        eo_do(obj,
              eo_event_callback_del(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _zoom_anim_cb, obj));
        eo_do(obj, eo_event_callback_call(EVAS_ZOOMABLE_INTERFACE_EVENT_ZOOM_STOP, NULL));
     }

   return EO_CALLBACK_CONTINUE;
}

static Eina_Bool
_long_press_cb(void *data)
{
   ELM_PHOTOCAM_DATA_GET(data, sd);

   sd->long_timer = NULL;
   sd->longpressed = EINA_TRUE;
   eo_do(data, eo_event_callback_call
     (EVAS_CLICKABLE_INTERFACE_EVENT_LONGPRESSED, NULL));

   return ECORE_CALLBACK_CANCEL;
}

static void
_mouse_down_cb(void *data,
               Evas *evas EINA_UNUSED,
               Evas_Object *obj EINA_UNUSED,
               void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;

   ELM_PHOTOCAM_DATA_GET(data, sd);

   if (ev->button != 1) return;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) sd->on_hold = EINA_TRUE;
   else sd->on_hold = EINA_FALSE;
   if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
     eo_do(data, eo_event_callback_call
       (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED_DOUBLE, NULL));
   else
     eo_do(data, eo_event_callback_call(ELM_PHOTOCAM_EVENT_PRESS, NULL));
   sd->longpressed = EINA_FALSE;
   ecore_timer_del(sd->long_timer);
   sd->long_timer = ecore_timer_add
       (_elm_config->longpress_timeout, _long_press_cb, data);
}

static void
_mouse_up_cb(void *data,
             Evas *evas EINA_UNUSED,
             Evas_Object *obj EINA_UNUSED,
             void *event_info)
{
   Evas_Event_Mouse_Up *ev = event_info;

   ELM_PHOTOCAM_DATA_GET(data, sd);

   if (ev->button != 1) return;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) sd->on_hold = EINA_TRUE;
   else sd->on_hold = EINA_FALSE;
   ELM_SAFE_FREE(sd->long_timer, ecore_timer_del);
   if (!sd->on_hold)
     eo_do(data, eo_event_callback_call
       (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, NULL));
   sd->on_hold = EINA_FALSE;
}

EOLIAN static Eina_Bool
_elm_photocam_elm_widget_on_focus(Eo *obj, Elm_Photocam_Data *_pd EINA_UNUSED, Elm_Object_Item *item EINA_UNUSED)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);
   Eina_Bool int_ret = EINA_FALSE;

   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_on_focus(NULL));
   if (!int_ret) return EINA_FALSE;

   if (elm_widget_focus_get(obj))
     {
        edje_object_signal_emit
          (wd->resize_obj, "elm,action,focus", "elm");
        evas_object_focus_set(wd->resize_obj, EINA_TRUE);
     }
   else
     {
        edje_object_signal_emit
          (wd->resize_obj, "elm,action,unfocus", "elm");
        evas_object_focus_set(wd->resize_obj, EINA_FALSE);
     }

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_photocam_elm_widget_theme_apply(Eo *obj, Elm_Photocam_Data *sd EINA_UNUSED)
{
   Eina_Bool int_ret = EINA_FALSE;
   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_theme_apply());
   if (!int_ret) return EINA_FALSE;

   _sizing_eval(obj);

   return EINA_TRUE;
}

static void
_scroll_animate_start_cb(Evas_Object *obj,
                         void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call
     (EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_ANIM_START, NULL));
}

static void
_scroll_animate_stop_cb(Evas_Object *obj,
                        void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call
     (EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_ANIM_STOP, NULL));
}

static void
_scroll_drag_start_cb(Evas_Object *obj,
                      void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call
     (EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_DRAG_START, NULL));
}

static void
_scroll_drag_stop_cb(Evas_Object *obj,
                     void *data EINA_UNUSED)
{
   eo_do(obj, eo_event_callback_call
     (EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL_DRAG_STOP, NULL));
}

static void
_scroll_cb(Evas_Object *obj,
           void *data EINA_UNUSED)
{
   ELM_PHOTOCAM_DATA_GET(obj, sd);

   if (!sd->scr_timer)
     {
        sd->no_smooth++;
        if (sd->no_smooth == 1) _smooth_update(obj);
     }

   ecore_timer_del(sd->scr_timer);
   sd->scr_timer = ecore_timer_add(0.5, _scroll_timeout_cb, obj);

   eo_do(obj, eo_event_callback_call
     (EVAS_SCROLLABLE_INTERFACE_EVENT_SCROLL, NULL));
}

static Eina_Bool
_key_action_move(Evas_Object *obj, const char *params)
{
   const char *dir = params;

   Evas_Coord x = 0;
   Evas_Coord y = 0;
   Evas_Coord v_h = 0;
   Evas_Coord step_x = 0;
   Evas_Coord step_y = 0;
   Evas_Coord page_x = 0;
   Evas_Coord page_y = 0;

   eo_do(obj,
         elm_interface_scrollable_content_pos_get(&x, &y),
         elm_interface_scrollable_step_size_get(&step_x, &step_y),
         elm_interface_scrollable_page_size_get(&page_x, &page_y),
         elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, NULL, &v_h));

   if (!strcmp(dir, "left"))
     {
        x -= step_x;
     }
   else if (!strcmp(dir, "right"))
     {
        x += step_x;
     }
   else if (!strcmp(dir, "up"))
     {
        y -= step_y;
     }
   else if (!strcmp(dir, "down"))
     {
        y += step_y;
     }
   else if (!strcmp(dir, "prior"))
     {
        if (page_y < 0)
          y -= -(page_y * v_h) / 100;
        else
          y -= page_y;
     }
   else if (!strcmp(dir, "next"))
     {
        if (page_y < 0)
          y += -(page_y * v_h) / 100;
        else
          y += page_y;
     }
   else return EINA_FALSE;

   eo_do(obj, elm_interface_scrollable_content_pos_set(x, y, EINA_TRUE));
   return EINA_TRUE;
}

static Eina_Bool
_key_action_zoom(Evas_Object *obj, const char *params)
{
   const char *dir = params;
   double zoom;

   if (!strcmp(dir, "in"))
     {
        zoom = elm_photocam_zoom_get(obj);
        zoom -= 0.5;
        elm_photocam_zoom_mode_set(obj, ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
        elm_photocam_zoom_set(obj, zoom);
     }
   else if (!strcmp(dir, "out"))
     {
        zoom = elm_photocam_zoom_get(obj);
        zoom += 0.5;
        elm_photocam_zoom_mode_set(obj, ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
        elm_photocam_zoom_set(obj, zoom);
     }
   else return EINA_FALSE;

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_photocam_elm_widget_event(Eo *obj, Elm_Photocam_Data *_pd EINA_UNUSED, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
   (void) src;
   Evas_Event_Key_Down *ev = event_info;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, MY_CLASS_NAME, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

static Eina_Bool
_bounce_eval(void *data, const Eo_Event *event EINA_UNUSED)
{
   Evas_Object *obj = data;
   ELM_PHOTOCAM_DATA_GET(obj, sd);
   double t, tt;

   if ((sd->g_layer_zoom.imx == sd->g_layer_zoom.bounce.x_end) &&
       (sd->g_layer_zoom.imy == sd->g_layer_zoom.bounce.y_end))
     goto on_end;

   t = ecore_loop_time_get();
   tt = (t - sd->g_layer_zoom.bounce.t_start) /
     (sd->g_layer_zoom.bounce.t_end -
      sd->g_layer_zoom.bounce.t_start);
   tt = 1.0 - tt;
   tt = 1.0 - (tt * tt);

   if (t > sd->g_layer_zoom.bounce.t_end)
     {
        _zoom_do(obj, 1.0);
        goto on_end;
     }

   if (sd->g_layer_zoom.imx != sd->g_layer_zoom.bounce.x_end)
     sd->g_layer_zoom.imx =
       sd->g_layer_zoom.bounce.x_start * (1.0 - tt) +
       sd->g_layer_zoom.bounce.x_end * tt;

   if (sd->g_layer_zoom.imy != sd->g_layer_zoom.bounce.y_end)
     sd->g_layer_zoom.imy =
       sd->g_layer_zoom.bounce.y_start * (1.0 - tt) +
       sd->g_layer_zoom.bounce.y_end * tt;

   _zoom_do(obj, 1.0 - (1.0 - tt));

   return EO_CALLBACK_CONTINUE;

 on_end:
   sd->g_layer_zoom.imx = 0;
   sd->g_layer_zoom.imy = 0;
   sd->zoom_g_layer = EINA_FALSE;

   eo_do(obj, elm_interface_scrollable_freeze_set(EINA_FALSE));

   eo_do(obj,
         eo_event_callback_del(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _bounce_eval, obj));
   return EO_CALLBACK_CONTINUE;
}

static void
_elm_photocam_bounce_reset(Eo *obj, Elm_Photocam_Data *sd EINA_UNUSED)
{
   Eina_Bool r;

   eo_do(obj,
         r = eo_event_callback_del(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _bounce_eval, obj));
   if (r) _zoom_do(obj, 1.0);
}

static void
_elm_photocam_zoom_reset(Eo *obj, Elm_Photocam_Data *sd)
{
   Eina_Bool r;

   eo_do(obj,
         r = eo_event_callback_del(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _zoom_anim_cb, obj));
   if (r)
     {
        sd->no_smooth--;
        if (!sd->no_smooth) _smooth_update(obj);
        _zoom_do(obj, 1.0);
        eo_do(obj, eo_event_callback_call(EVAS_ZOOMABLE_INTERFACE_EVENT_ZOOM_STOP, NULL));
     }
}

static void
_g_layer_zoom_do(Evas_Object *obj,
                 Evas_Coord px,
                 Evas_Coord py,
                 Elm_Gesture_Zoom_Info *g_layer)
{
   int regx, regy, regw, regh, ix, iy, iw, ih;
   Evas_Coord rx, ry, rw = 0, rh = 0;
   int xx, yy;

   ELM_PHOTOCAM_DATA_GET(obj, sd);
   sd->mode = ELM_PHOTOCAM_ZOOM_MODE_MANUAL;
   sd->zoom = sd->g_layer_start / g_layer->zoom;
   sd->size.ow = sd->size.w;
   sd->size.oh = sd->size.h;
   eo_do(obj, elm_interface_scrollable_content_pos_get(&rx, &ry));
   eo_do(obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &rw, &rh));
   if ((rw <= 0) || (rh <= 0)) return;

   sd->size.nw = (double)sd->size.imw / sd->zoom;
   sd->size.nh = (double)sd->size.imh / sd->zoom;

   elm_photocam_image_region_get(obj, &regx, &regy, &regw, &regh);
   evas_object_geometry_get(sd->img, &ix, &iy, &iw, &ih);

   sd->pvx = g_layer->x;
   sd->pvy = g_layer->y;

   xx = (px / sd->zoom) - sd->pvx;
   yy = (py / sd->zoom) - sd->pvy;
   sd->g_layer_zoom.imx = 0;
   sd->g_layer_zoom.imy = 0;

   if ((xx < 0) || (rw > sd->size.nw))
     {
        sd->g_layer_zoom.imx = xx;
        xx = 0;
     }
   else if ((xx + rw) > sd->size.nw)
     {
        sd->g_layer_zoom.imx = xx + rw - sd->size.nw;
        xx = sd->size.nw - rw;
     }

   if ((yy < 0) || (rh > sd->size.nh))
     {
        sd->g_layer_zoom.imy = yy;
        yy = 0;
     }
   else if ((yy + rh) > sd->size.nh)
     {
        sd->g_layer_zoom.imy = yy + rh - sd->size.nh;
        yy = sd->size.nh - rh;
     }

   sd->size.spos.x = (double)(xx + (rw / 2)) / (double)(sd->size.nw);
   sd->size.spos.y = (double)(yy + (rh / 2)) / (double)(sd->size.nh);

   _zoom_do(obj, 1.0);
}

static Evas_Event_Flags
_g_layer_zoom_start_cb(void *data,
                       void *event_info)
{
   Evas_Object *obj = data;
   Elm_Gesture_Zoom_Info *p = event_info;
   ELM_PHOTOCAM_DATA_GET(obj, sd);
   double marginx = 0, marginy = 0;
   Evas_Coord rw = 0, rh = 0;
   int x, y, w, h;

   _elm_photocam_bounce_reset(obj, sd);
   sd->zoom_g_layer = EINA_TRUE;

   eo_do(obj, elm_interface_scrollable_freeze_set(EINA_TRUE));

   elm_photocam_image_region_get(obj, &x, &y, &w, &h);
   eo_do(obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &rw, &rh));

   if (rw > sd->size.nw)
     marginx = (rw - sd->size.nw) / 2;
   if (rh > sd->size.nh)
     marginy = (rh - sd->size.nh) / 2;

   sd->g_layer_start = sd->zoom;

   sd->zoom_point_x = x + ((p->x - marginx) * sd->zoom) +
     sd->g_layer_zoom.imx;
   sd->zoom_point_y = y + ((p->y - marginy) * sd->zoom) +
     sd->g_layer_zoom.imy;

   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
_g_layer_zoom_move_cb(void *data,
                      void *event_info)
{
   Elm_Photocam_Data *sd = eo_data_scope_get(data, MY_CLASS);
   Elm_Gesture_Zoom_Info *p = event_info;

   _g_layer_zoom_do(data, sd->zoom_point_x, sd->zoom_point_y, p);

   return EVAS_EVENT_FLAG_NONE;
}

static Evas_Event_Flags
_g_layer_zoom_end_cb(void *data,
                     void *event_info EINA_UNUSED)
{
   Evas_Object *obj = data;
   ELM_PHOTOCAM_DATA_GET(obj, sd);
   Evas_Coord rw, rh;

   eo_do(obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &rw, &rh));
   sd->g_layer_start = 1.0;

   if (sd->g_layer_zoom.imx || sd->g_layer_zoom.imy)
     {
        double t;

        t = ecore_loop_time_get();
        sd->g_layer_zoom.bounce.x_start = sd->g_layer_zoom.imx;
        sd->g_layer_zoom.bounce.y_start = sd->g_layer_zoom.imy;
        sd->g_layer_zoom.bounce.x_end = 0;
        sd->g_layer_zoom.bounce.y_end = 0;

        if (rw > sd->size.nw &&
            rh > sd->size.nh)
          {
             Evas_Coord pw, ph;
             double z;

             if ((sd->size.imw < rw) && (sd->size.imh < rh))
               {
                  sd->zoom = 1;
                  sd->size.nw = sd->size.imw;
                  sd->size.nh = sd->size.imh;
               }
             else
               {
                  ph = (sd->size.imh * rw) / sd->size.imw;
                  if (ph > rh)
                    {
                       pw = (sd->size.imw * rh) / sd->size.imh;
                       ph = rh;
                    }
                  else
                    {
                       pw = rw;
                    }
                  if (sd->size.imw > sd->size.imh)
                    z = (double)sd->size.imw / pw;
                  else
                    z = (double)sd->size.imh / ph;

                  sd->zoom = z;
                  sd->size.nw = pw;
                  sd->size.nh = ph;
               }
             sd->g_layer_zoom.bounce.x_end = (sd->size.nw - rw) / 2;
             sd->g_layer_zoom.bounce.y_end = (sd->size.nh - rh) / 2;
          }
        else
          {
             int xx, yy;

             xx = (sd->zoom_point_x / sd->zoom) - sd->pvx;
             yy = (sd->zoom_point_y / sd->zoom) - sd->pvy;

             if (xx < 0) xx = 0;
             if (yy < 0) yy = 0;

             if (rw > sd->size.nw)
               sd->g_layer_zoom.bounce.x_end = (sd->size.nw - rw) / 2;
             if ((xx + rw) > sd->size.nw)
               xx = sd->size.nw - rw;

             if (rh > sd->size.nh)
               sd->g_layer_zoom.bounce.y_end = (sd->size.nh - rh) / 2;
             if ((yy + rh) > sd->size.nh)
               yy = sd->size.nh - rh;

             sd->size.spos.x = (double)(xx + (rw / 2)) / (double)(sd->size.nw);
             sd->size.spos.y = (double)(yy + (rh / 2)) / (double)(sd->size.nh);
          }

        sd->g_layer_zoom.bounce.t_start = t;
        sd->g_layer_zoom.bounce.t_end = t +
          _elm_config->page_scroll_friction;

        eo_do(obj,
              eo_event_callback_add(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _bounce_eval, obj));
     }
   else
     {
        eo_do(obj, elm_interface_scrollable_freeze_set(EINA_FALSE));
        sd->zoom_g_layer = EINA_FALSE;
     }

   return EVAS_EVENT_FLAG_NONE;
}

static void
_orient_do(Evas_Object *obj, Elm_Photocam_Data *sd)
{
   evas_object_smart_member_del(sd->img);
   elm_widget_sub_object_del(obj, sd->img);
   evas_object_smart_member_add(sd->img, sd->pan_obj);
   elm_widget_sub_object_add(obj, sd->img);
   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job_cb, obj);
}

EOLIAN static void
_elm_photocam_image_orient_set(Eo *obj, Elm_Photocam_Data *sd, Evas_Image_Orient orient)
{
   int iw, ih;
   Eina_List *l;
   Elm_Phocam_Grid *g, *g_orient = NULL;

   if (sd->orient == orient) return;

   sd->orientation_changed = EINA_TRUE;
   sd->orient = orient;
   g = _grid_create(obj);
   if (g)
     {
        if (eina_list_count(sd->grids) > 1)
          {
             g_orient = eina_list_last(sd->grids)->data;
             sd->grids = eina_list_remove(sd->grids, g_orient);
             _grid_clear(obj, g_orient);
             free(g_orient);
             EINA_LIST_FOREACH(sd->grids, l, g_orient)
               {
                  g_orient->dead = 1;
               }
          }
        sd->grids = eina_list_prepend(sd->grids, g);
     }
   else
     {
        EINA_LIST_FREE(sd->grids, g)
          {
             _grid_clear(obj, g);
             free(g);
          }
     }

   evas_object_image_orient_set(sd->img, orient);
   evas_object_image_size_get(sd->img, &iw, &ih);
   sd->size.imw = iw;
   sd->size.imh = ih;
   sd->size.w = iw / sd->zoom;
   sd->size.h = ih / sd->zoom;
   _orient_do(obj, sd);
}

EOLIAN static Evas_Image_Orient
_elm_photocam_image_orient_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd)
{
   return sd->orient;
}

EOLIAN static void
_elm_photocam_evas_object_smart_add(Eo *obj, Elm_Photocam_Data *priv)
{
   Eina_Bool bounce = _elm_config->thumbscroll_bounce_enable;
   Elm_Photocam_Pan_Data *pan_data;
   Evas_Object *edje;
   Evas_Coord minw, minh;

   elm_widget_sub_object_parent_add(obj);

   edje = edje_object_add(evas_object_evas_get(obj));
   elm_widget_resize_object_set(obj, edje, EINA_TRUE);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());

   elm_widget_theme_object_set
      (obj, edje, "photocam", "base", elm_widget_style_get(obj));

   priv->hit_rect = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_smart_member_add(priv->hit_rect, obj);
   elm_widget_sub_object_add(obj, priv->hit_rect);

   /* common scroller hit rectangle setup */
   evas_object_color_set(priv->hit_rect, 0, 0, 0, 0);
   evas_object_show(priv->hit_rect);
   evas_object_repeat_events_set(priv->hit_rect, EINA_TRUE);

   elm_widget_can_focus_set(obj, EINA_TRUE);

   eo_do(obj, elm_interface_scrollable_objects_set(edje, priv->hit_rect));

   eo_do(obj,
         elm_interface_scrollable_animate_start_cb_set(_scroll_animate_start_cb),
         elm_interface_scrollable_animate_stop_cb_set(_scroll_animate_stop_cb),
         elm_interface_scrollable_drag_start_cb_set(_scroll_drag_start_cb),
         elm_interface_scrollable_drag_stop_cb_set(_scroll_drag_stop_cb),
         elm_interface_scrollable_scroll_cb_set(_scroll_cb));

   eo_do(obj, elm_interface_scrollable_bounce_allow_set(bounce, bounce));

   priv->pan_obj = eo_add(MY_PAN_CLASS, evas_object_evas_get(obj));
   pan_data = eo_data_scope_get(priv->pan_obj, MY_PAN_CLASS);
   eo_data_ref(obj, NULL);
   pan_data->wobj = obj;
   pan_data->wsd = priv;

   eo_do(obj, elm_interface_scrollable_extern_pan_set(priv->pan_obj));

   priv->g_layer_start = 1.0;
   priv->zoom = 1;
   priv->mode = ELM_PHOTOCAM_ZOOM_MODE_MANUAL;
   priv->tsize = 512;

   priv->img = evas_object_image_add(evas_object_evas_get(obj));
   evas_object_image_load_orientation_set(priv->img, EINA_TRUE);
   evas_object_image_scale_hint_set(priv->img, EVAS_IMAGE_SCALE_HINT_DYNAMIC);
   evas_object_event_callback_add
     (priv->img, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, obj);
   evas_object_event_callback_add
     (priv->img, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb, obj);
   evas_object_image_scale_hint_set(priv->img, EVAS_IMAGE_SCALE_HINT_STATIC);

   /* XXX: mmm... */
   evas_object_smart_member_add(priv->img, priv->pan_obj);

   elm_widget_sub_object_add(obj, priv->img);
   evas_object_image_filled_set(priv->img, EINA_TRUE);
   evas_object_event_callback_add
     (priv->img, EVAS_CALLBACK_IMAGE_PRELOADED, _main_img_preloaded_cb, obj);

   edje_object_size_min_calc(edje, &minw, &minh);
   evas_object_size_hint_min_set(obj, minw, minh);

   _sizing_eval(obj);

}

EOLIAN static void
_elm_photocam_evas_object_smart_del(Eo *obj, Elm_Photocam_Data *sd)
{
   Elm_Phocam_Grid *g;

   EINA_LIST_FREE(sd->grids, g)
     {
        free(g->grid);
        free(g);
     }
   ELM_SAFE_FREE(sd->pan_obj, evas_object_del);

   if (sd->f) eina_file_close(sd->f);
   free(sd->remote_data);
   if (sd->remote) _elm_url_cancel(sd->remote);
   eina_stringshare_del(sd->file);
   ecore_job_del(sd->calc_job);
   ecore_timer_del(sd->scr_timer);
   ecore_timer_del(sd->long_timer);
   eo_do(obj,
         eo_event_callback_del(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _zoom_anim_cb, obj),
         eo_event_callback_del(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _bounce_eval, obj));

   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

EOLIAN static void
_elm_photocam_evas_object_smart_move(Eo *obj, Elm_Photocam_Data *sd, Evas_Coord x, Evas_Coord y)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_move(x, y));

   evas_object_move(sd->hit_rect, x, y);
}

EOLIAN static void
_elm_photocam_evas_object_smart_resize(Eo *obj, Elm_Photocam_Data *sd, Evas_Coord w, Evas_Coord h)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_resize(w, h));

   evas_object_resize(sd->hit_rect, w, h);
}

EOLIAN static void
_elm_photocam_evas_object_smart_member_add(Eo *obj, Elm_Photocam_Data *sd, Evas_Object *member)
{

   eo_do_super(obj, MY_CLASS, evas_obj_smart_member_add(member));

   if (sd->hit_rect)
     evas_object_raise(sd->hit_rect);
}

EAPI Evas_Object *
elm_photocam_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

EOLIAN static Eo *
_elm_photocam_eo_base_constructor(Eo *obj, Elm_Photocam_Data *_pd EINA_UNUSED)
{
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());
   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_IMAGE));

   return obj;
}

static void
_internal_file_set(Eo *obj, Elm_Photocam_Data *sd, const char *file, Eina_File *f, Evas_Load_Error *ret)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   Evas_Load_Error err;
   int w, h;
   double tz;

   // It is actually to late, we have lost the reference to the previous
   // file descriptor already, so we can't know if the file changed. To
   // be safe we do for now just force a full reload on file_set and hope
   // on evas to catch it, if there is no change.
   eina_stringshare_replace(&sd->file, file);
   sd->f = eina_file_dup(f);

   evas_object_image_smooth_scale_set(sd->img, (sd->no_smooth == 0));
   evas_object_image_file_set(sd->img, NULL, NULL);
   evas_object_image_load_scale_down_set(sd->img, 0);
   _photocam_image_file_set(sd->img, sd);
   err = evas_object_image_load_error_get(sd->img);
   if (err != EVAS_LOAD_ERROR_NONE)
     {
        ERR("Things are going bad for '%s' (%p) : %i", file, sd->img, err);
        if (ret) *ret = err;
        return;
     }
   evas_object_image_size_get(sd->img, &w, &h);

   sd->do_region = evas_object_image_region_support_get(sd->img);
   sd->size.imw = w;
   sd->size.imh = h;
   sd->size.w = sd->size.imw / sd->zoom;
   sd->size.h = sd->size.imh / sd->zoom;
   evas_object_image_file_set(sd->img, NULL, NULL);
   _photocam_image_file_set(sd->img, sd);
   err = evas_object_image_load_error_get(sd->img);
   if (err != EVAS_LOAD_ERROR_NONE)
     {
        ERR("Things are going bad for '%s' (%p)", file, sd->img);
        if (ret) *ret = err;
        return;
     }

   evas_object_image_preload(sd->img, 0);
   sd->main_load_pending = EINA_TRUE;

   sd->calc_job = ecore_job_add(_calc_job_cb, obj);
   eo_do(obj, eo_event_callback_call(ELM_PHOTOCAM_EVENT_LOAD, NULL));
   sd->preload_num++;
   if (sd->preload_num == 1)
     {
        edje_object_signal_emit
          (wd->resize_obj, "elm,state,busy,start", "elm");
        eo_do(obj, eo_event_callback_call(ELM_PHOTOCAM_EVENT_LOAD_DETAIL, NULL));
     }

   tz = sd->zoom;
   sd->zoom = 0.0;
   elm_photocam_zoom_set(obj, tz);
   sd->orient = EVAS_IMAGE_ORIENT_NONE;
   sd->orientation_changed = EINA_FALSE;

   if (ret) *ret = evas_object_image_load_error_get(sd->img);
}

static void
_elm_photocam_download_done(void *data, Elm_Url *url EINA_UNUSED, Eina_Binbuf *download)
{
   Eo *obj = data;
   Elm_Photocam_Data *sd = eo_data_scope_get(obj, MY_CLASS);
   Eina_File *f;
   size_t length;
   Evas_Load_Error ret = EVAS_LOAD_ERROR_NONE;

   free(sd->remote_data);
   length = eina_binbuf_length_get(download);
   sd->remote_data = eina_binbuf_string_steal(download);
   f = eina_file_virtualize(_elm_url_get(url),
                            sd->remote_data, length,
                            EINA_FALSE);
   _internal_file_set(obj, sd, _elm_url_get(url), f, &ret);
   eina_file_close(f);

   if (ret != EVAS_LOAD_ERROR_NONE)
     {
        Elm_Photocam_Error err = { 0, EINA_TRUE };

        free(sd->remote_data);
        sd->remote_data = NULL;
        eo_do(obj, eo_event_callback_call
          (ELM_PHOTOCAM_EVENT_DOWNLOAD_ERROR, &err));
     }
   else
     {
        eo_do(obj, eo_event_callback_call
          (ELM_PHOTOCAM_EVENT_DOWNLOAD_DONE, NULL));
     }

   sd->remote = NULL;
}

static void
_elm_photocam_download_cancel(void *data, Elm_Url *url EINA_UNUSED, int error)
{
   Eo *obj = data;
   Elm_Photocam_Data *sd = eo_data_scope_get(obj, MY_CLASS);
   Elm_Photocam_Error err = { error, EINA_FALSE };

   eo_do(obj, eo_event_callback_call(ELM_PHOTOCAM_EVENT_DOWNLOAD_ERROR, &err));

   sd->remote = NULL;
}

static void
_elm_photocam_download_progress(void *data, Elm_Url *url EINA_UNUSED, double now, double total)
{
   Eo *obj = data;
   Elm_Photocam_Progress progress;

   progress.now = now;
   progress.total = total;
   eo_do(obj, eo_event_callback_call
     (ELM_PHOTOCAM_EVENT_DOWNLOAD_PROGRESS, &progress));
}

static const char *remote_uri[] = {
  "http://", "https://", "ftp://"
};

static Evas_Load_Error
_elm_photocam_file_set_internal(Eo *obj, Elm_Photocam_Data *sd, const char *file)
{
   Evas_Load_Error ret = EVAS_LOAD_ERROR_NONE;
   unsigned int i;

   _grid_clear_all(obj);
   _elm_photocam_zoom_reset(obj, sd);
   _elm_photocam_bounce_reset(obj, sd);
   sd->no_smooth--;
   if (sd->no_smooth == 0) _smooth_update(obj);

   ecore_job_del(sd->calc_job);
   evas_object_hide(sd->img);
   if (sd->f) eina_file_close(sd->f);
   sd->f = NULL;

   free(sd->remote_data);
   if (sd->remote) _elm_url_cancel(sd->remote);
   sd->remote = NULL;
   sd->preload_num = 0;

   for (i = 0; i < sizeof (remote_uri) / sizeof (remote_uri[0]); ++i)
     if (!strncmp(remote_uri[i], file, strlen(remote_uri[i])))
       {
          // Found a remote target !
          sd->remote = _elm_url_download(file,
                                        _elm_photocam_download_done,
                                        _elm_photocam_download_cancel,
                                        _elm_photocam_download_progress,
                                        obj);
          if (sd->remote)
            {
               eo_do(obj, eo_event_callback_call
                 (ELM_PHOTOCAM_EVENT_DOWNLOAD_START, NULL));
               return ret;
            }
          break;
       }

   _internal_file_set(obj, sd, file, NULL, &ret);

   return ret;
}

EOLIAN static Eina_Bool
_elm_photocam_efl_file_file_set(Eo *obj, Elm_Photocam_Data *sd, const char *file, const char *key EINA_UNUSED)
{
   Evas_Load_Error ret = _elm_photocam_file_set_internal(obj, sd, file);

   if (ret == EVAS_LOAD_ERROR_NONE) return EINA_TRUE;

   eina_error_set(
     ret == EVAS_LOAD_ERROR_DOES_NOT_EXIST             ? PHOTO_FILE_LOAD_ERROR_DOES_NOT_EXIST :
     ret == EVAS_LOAD_ERROR_PERMISSION_DENIED          ? PHOTO_FILE_LOAD_ERROR_PERMISSION_DENIED :
     ret == EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED ? PHOTO_FILE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED :
     ret == EVAS_LOAD_ERROR_CORRUPT_FILE               ? PHOTO_FILE_LOAD_ERROR_CORRUPT_FILE :
     ret == EVAS_LOAD_ERROR_UNKNOWN_FORMAT             ? PHOTO_FILE_LOAD_ERROR_UNKNOWN_FORMAT :
     PHOTO_FILE_LOAD_ERROR_GENERIC
   );
   return EINA_FALSE;
}

EAPI Evas_Load_Error
elm_photocam_file_set(Elm_Photocam *obj, const char *file)
{
   Eina_Bool ret;

   ELM_PHOTOCAM_CHECK(obj) EVAS_LOAD_ERROR_NONE;
   EINA_SAFETY_ON_NULL_RETURN_VAL(file, EVAS_LOAD_ERROR_NONE);
   if (eo_do_ret(obj, ret, efl_file_set(file, NULL))) return EVAS_LOAD_ERROR_NONE;

   Eina_Error err = eina_error_get();
   return err == PHOTO_FILE_LOAD_ERROR_DOES_NOT_EXIST ?
            EVAS_LOAD_ERROR_DOES_NOT_EXIST :
          err == PHOTO_FILE_LOAD_ERROR_PERMISSION_DENIED ?
            EVAS_LOAD_ERROR_PERMISSION_DENIED :
          err == PHOTO_FILE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED ?
            EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED :
          err == PHOTO_FILE_LOAD_ERROR_CORRUPT_FILE ?
            EVAS_LOAD_ERROR_CORRUPT_FILE :
          err == PHOTO_FILE_LOAD_ERROR_UNKNOWN_FORMAT ?
            EVAS_LOAD_ERROR_UNKNOWN_FORMAT :
          EVAS_LOAD_ERROR_GENERIC;
}

EOLIAN static void
_elm_photocam_efl_file_file_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd, const char **file, const char **key)
{
   if (file) *file = sd->file;
   if (key) *key = NULL;
}

EAPI const char*
elm_photocam_file_get(const Elm_Photocam *obj)
{
   const char *ret = NULL;
   eo_do(obj, efl_file_get(&ret, NULL));
   return ret;
}

EOLIAN static void
_elm_photocam_zoom_set(Eo *obj, Elm_Photocam_Data *sd, double zoom)
{
   double z;
   Eina_List *l;
   Elm_Phocam_Grid *g, *g_zoom = NULL;
   Evas_Coord pw, ph, rx, ry, rw, rh;
   int zoom_changed = 0, started = 0;
   Eina_Bool an = EINA_FALSE;

   if (zoom <= (1.0 / 256.0)) zoom = (1.0 / 256.0);
   if (zoom == sd->zoom) return;

   sd->zoom = zoom;
   sd->size.ow = sd->size.w;
   sd->size.oh = sd->size.h;
   eo_do(obj, elm_interface_scrollable_content_pos_get(&rx, &ry));
   eo_do(obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &rw, &rh));
   if ((rw <= 0) || (rh <= 0)) return;

   if (sd->mode == ELM_PHOTOCAM_ZOOM_MODE_MANUAL)
     {
        sd->size.nw = (double)sd->size.imw / sd->zoom;
        sd->size.nh = (double)sd->size.imh / sd->zoom;
     }
   else if (sd->mode == ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT)
     {
        if ((sd->size.imw < 1) || (sd->size.imh < 1))
          {
             sd->size.nw = 0;
             sd->size.nh = 0;
          }
        else
          {
             ph = (sd->size.imh * rw) / sd->size.imw;
             if (ph > rh)
               {
                  pw = (sd->size.imw * rh) / sd->size.imh;
                  ph = rh;
               }
             else
               {
                  pw = rw;
               }
             if (sd->size.imw > sd->size.imh)
               z = (double)sd->size.imw / pw;
             else
               z = (double)sd->size.imh / ph;
             if (z != sd->zoom)
               zoom_changed = 1;
             sd->zoom = z;
             sd->size.nw = pw;
             sd->size.nh = ph;
          }
     }
   else if (sd->mode == ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL)
     {
        if ((sd->size.imw < 1) || (sd->size.imh < 1))
          {
             sd->size.nw = 0;
             sd->size.nw = 0;
          }
        else
          {
             ph = (sd->size.imh * rw) / sd->size.imw;
             if (ph < rh)
               {
                  pw = (sd->size.imw * rh) / sd->size.imh;
                  ph = rh;
               }
             else
               {
                  pw = rw;
               }
             if (sd->size.imw > sd->size.imh)
               z = (double)sd->size.imw / pw;
             else
               z = (double)sd->size.imh / ph;
             if (z != sd->zoom)
               zoom_changed = 1;
             sd->zoom = z;
             sd->size.nw = pw;
             sd->size.nh = ph;
          }
     }
   else if (sd->mode == ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT_IN)
     {
        if ((sd->size.imw < 1) || (sd->size.imh < 1))
          {
             sd->size.nw = 0;
             sd->size.nh = 0;
          }
        else if ((sd->size.imw < rw) && (sd->size.imh < rh))
          {
             if (1 != sd->zoom) zoom_changed = 1;
             sd->zoom = 1;
             sd->size.nw = sd->size.imw;
             sd->size.nh = sd->size.imh;
          }
        else
          {
             ph = (sd->size.imh * rw) / sd->size.imw;
             if (ph > rh)
               {
                  pw = (sd->size.imw * rh) / sd->size.imh;
                  ph = rh;
               }
             else
               pw = rw;
             if (sd->size.imw > sd->size.imh)
               z = (double)sd->size.imw / pw;
             else
               z = (double)sd->size.imh / ph;
             if (z != sd->zoom)
               zoom_changed = 1;
             sd->zoom = z;
             sd->size.nw = pw;
             sd->size.nh = ph;
          }
     }

   if (sd->main_load_pending)
     {
        sd->size.w = sd->size.nw;
        sd->size.h = sd->size.nh;

        goto done;
     }

   EINA_LIST_FOREACH(sd->grids, l, g)
     {
        if (g->zoom == _grid_zoom_calc(sd->zoom))
          {
             sd->grids = eina_list_remove(sd->grids, g);
             sd->grids = eina_list_prepend(sd->grids, g);
             _grid_raise(g);
             goto done;
          }
     }

   g = _grid_create(obj);
   if (g)
     {
        if (eina_list_count(sd->grids) > 1)
          {
             g_zoom = eina_list_last(sd->grids)->data;
             sd->grids = eina_list_remove(sd->grids, g_zoom);
             _grid_clear(obj, g_zoom);
             free(g_zoom);
             EINA_LIST_FOREACH(sd->grids, l, g_zoom)
               {
                  g_zoom->dead = 1;
               }
          }
        sd->grids = eina_list_prepend(sd->grids, g);
     }
   else
     {
        EINA_LIST_FREE(sd->grids, g)
          {
             _grid_clear(obj, g);
             free(g);
          }
     }

done:
   sd->t_start = ecore_loop_time_get();
   sd->t_end = sd->t_start + _elm_config->zoom_friction;
   if ((sd->size.w > 0) && (sd->size.h > 0))
     {
        sd->size.spos.x = (double)(rx + (rw / 2)) / (double)sd->size.w;
        sd->size.spos.y = (double)(ry + (rh / 2)) / (double)sd->size.h;
     }
   else
     {
        sd->size.spos.x = 0.5;
        sd->size.spos.y = 0.5;
     }
   if (rw > sd->size.w) sd->size.spos.x = 0.5;
   if (rh > sd->size.h) sd->size.spos.y = 0.5;
   if (sd->size.spos.x > 1.0) sd->size.spos.x = 1.0;
   if (sd->size.spos.y > 1.0) sd->size.spos.y = 1.0;

   if (sd->paused)
     {
        _zoom_do(obj, 1.0);
     }
   else
     {
        eo_do(obj,
              an = eo_event_callback_del(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _zoom_anim_cb, obj),
              eo_event_callback_add(EFL_CORE_ANIMATOR_EVENT_ANIMATOR_TICK, _zoom_anim_cb, obj));
        if (!an)
          {
             sd->no_smooth++;
             if (sd->no_smooth == 1) _smooth_update(obj);
             started = 1;
          }
     }

   if (an)
     {
        // FIXME: If one day we do support partial animator in photocam, this would require change
        Eo_Event event = {0};
        event.obj = evas_object_evas_get(obj);
        if (!_zoom_anim_cb(obj, &event))
          {
             _elm_photocam_bounce_reset(obj, sd);
             an = 0;
          }
     }

   ecore_job_del(sd->calc_job);
   sd->calc_job = ecore_job_add(_calc_job_cb, obj);
   if (!sd->paused)
     {
        if (started)
          eo_do(obj, eo_event_callback_call(EVAS_ZOOMABLE_INTERFACE_EVENT_ZOOM_START, NULL));
        if (!an)
          eo_do(obj, eo_event_callback_call(EVAS_ZOOMABLE_INTERFACE_EVENT_ZOOM_STOP, NULL));
     }
   if (zoom_changed)
     eo_do(obj, eo_event_callback_call(EVAS_ZOOMABLE_INTERFACE_EVENT_ZOOM_CHANGE, NULL));
}

EOLIAN static double
_elm_photocam_zoom_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd)
{
   return sd->zoom;
}

EOLIAN static void
_elm_photocam_zoom_mode_set(Eo *obj, Elm_Photocam_Data *sd, Elm_Photocam_Zoom_Mode mode)
{
   double tz;
   if (sd->mode == mode) return;
   sd->mode = mode;

   tz = sd->zoom;
   sd->zoom = 0.0;
   elm_photocam_zoom_set(obj, tz);
}

EOLIAN static Elm_Photocam_Zoom_Mode
_elm_photocam_zoom_mode_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd)
{
   return sd->mode;
}

EOLIAN static void
_elm_photocam_image_size_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd, int *w, int *h)
{
   if (w) *w = sd->size.imw;
   if (h) *h = sd->size.imh;
}

EOLIAN static void
_elm_photocam_image_region_get(Eo *obj, Elm_Photocam_Data *sd, int *x, int *y, int *w, int *h)
{
   Evas_Coord sx, sy, sw, sh;

   eo_do((Eo *)obj, elm_interface_scrollable_content_pos_get(&sx, &sy));
   eo_do((Eo *)obj, elm_interface_scrollable_content_viewport_geometry_get
         (NULL, NULL, &sw, &sh));
   if (sd->size.w > 0)
     {
        if (x)
          {
             *x = (sd->size.imw * sx) / sd->size.w;
             if (*x > sd->size.imw) *x = sd->size.imw;
          }
        if (w)
          {
             *w = (sd->size.imw * sw) / sd->size.w;
             if (*w > sd->size.imw) *w = sd->size.imw;
             else if (*w < 0)
               *w = 0;
          }
     }
   else
     {
        if (x) *x = 0;
        if (w) *w = 0;
     }

   if (sd->size.h > 0)
     {
        if (y)
          {
             *y = (sd->size.imh * sy) / sd->size.h;
             if (*y > sd->size.imh) *y = sd->size.imh;
          }
        if (h)
          {
             *h = (sd->size.imh * sh) / sd->size.h;
             if (*h > sd->size.imh) *h = sd->size.imh;
             else if (*h < 0)
               *h = 0;
          }
     }
   else
     {
        if (y) *y = 0;
        if (h) *h = 0;
     }
}

EOLIAN static void
_elm_photocam_image_region_show(Eo *obj, Elm_Photocam_Data *sd, int x, int y, int w, int h)
{
   int rx, ry, rw, rh;

   if ((sd->size.imw < 1) || (sd->size.imh < 1)) return;
   rx = (x * sd->size.w) / sd->size.imw;
   ry = (y * sd->size.h) / sd->size.imh;
   rw = (w * sd->size.w) / sd->size.imw;
   rh = (h * sd->size.h) / sd->size.imh;
   if (rw < 1) rw = 1;
   if (rh < 1) rh = 1;
   if ((rx + rw) > sd->size.w) rx = sd->size.w - rw;
   if ((ry + rh) > sd->size.h) ry = sd->size.h - rh;

   _elm_photocam_bounce_reset(obj, sd);
   _elm_photocam_zoom_reset(obj, sd);

   eo_do(obj, elm_interface_scrollable_content_region_show(rx, ry, rw, rh));
}

EAPI void
elm_photocam_image_region_bring_in(Evas_Object *obj,
                                   int x,
                                   int y,
                                   int w,
                                   int h EINA_UNUSED)
{
   ELM_PHOTOCAM_CHECK(obj);
   eo_do(obj, elm_interface_scrollable_region_bring_in(x, y, w, h));
}

EOLIAN static void
_elm_photocam_elm_interface_scrollable_region_bring_in(Eo *obj, Elm_Photocam_Data *sd, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
   int rx, ry, rw, rh;

   if ((sd->size.imw < 1) || (sd->size.imh < 1)) return;
   rx = (x * sd->size.w) / sd->size.imw;
   ry = (y * sd->size.h) / sd->size.imh;
   rw = (w * sd->size.w) / sd->size.imw;
   rh = (h * sd->size.h) / sd->size.imh;
   if (rw < 1) rw = 1;
   if (rh < 1) rh = 1;
   if ((rx + rw) > sd->size.w) rx = sd->size.w - rw;
   if ((ry + rh) > sd->size.h) ry = sd->size.h - rh;

   _elm_photocam_bounce_reset(obj, sd);
   _elm_photocam_zoom_reset(obj, sd);

   eo_do_super(obj, MY_CLASS, elm_interface_scrollable_region_bring_in(rx, ry, rw, rh));
}

EOLIAN static void
_elm_photocam_paused_set(Eo *obj, Elm_Photocam_Data *sd, Eina_Bool paused)
{
   paused = !!paused;

   if (sd->paused == paused) return;
   sd->paused = paused;
   if (!sd->paused) return;

   _elm_photocam_bounce_reset(obj, sd);
   _elm_photocam_zoom_reset(obj, sd);
}

EOLIAN static Eina_Bool
_elm_photocam_paused_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd)
{
   return sd->paused;
}

EOLIAN static Evas_Object*
_elm_photocam_internal_image_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd)
{
   return sd->img;
}

EAPI void
elm_photocam_bounce_set(Evas_Object *obj,
                        Eina_Bool h_bounce,
                        Eina_Bool v_bounce)
{
   ELM_PHOTOCAM_CHECK(obj);

   eo_do(obj, elm_interface_scrollable_bounce_allow_set(h_bounce, v_bounce));
}

EAPI void
elm_photocam_bounce_get(const Evas_Object *obj,
                        Eina_Bool *h_bounce,
                        Eina_Bool *v_bounce)
{
   ELM_PHOTOCAM_CHECK(obj);

   eo_do((Eo *)obj, elm_interface_scrollable_bounce_allow_get(h_bounce, v_bounce));
}

EOLIAN static void
_elm_photocam_gesture_enabled_set(Eo *obj, Elm_Photocam_Data *sd, Eina_Bool gesture)
{
   gesture = !!gesture;

   if (sd->do_gesture == gesture) return;

   sd->do_gesture = gesture;

   ELM_SAFE_FREE(sd->g_layer, evas_object_del);

   if (!gesture) return;

   sd->g_layer = elm_gesture_layer_add(obj);
   if (!sd->g_layer) return;

   elm_gesture_layer_attach(sd->g_layer, obj);
   elm_gesture_layer_cb_set
     (sd->g_layer, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_START,
     _g_layer_zoom_start_cb, obj);
   elm_gesture_layer_cb_set
     (sd->g_layer, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_MOVE,
     _g_layer_zoom_move_cb, obj);
   elm_gesture_layer_cb_set
     (sd->g_layer, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_END,
     _g_layer_zoom_end_cb, obj);
   elm_gesture_layer_cb_set
     (sd->g_layer, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_ABORT,
     _g_layer_zoom_end_cb, obj);
}

EOLIAN static Eina_Bool
_elm_photocam_gesture_enabled_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *sd)
{
   return sd->do_gesture;
}

static void
_elm_photocam_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);

   PHOTO_FILE_LOAD_ERROR_GENERIC = eina_error_msg_static_register("Generic load error");
   PHOTO_FILE_LOAD_ERROR_DOES_NOT_EXIST = eina_error_msg_static_register("File does not exist");
   PHOTO_FILE_LOAD_ERROR_PERMISSION_DENIED = eina_error_msg_static_register("Permission denied to an existing file");
   PHOTO_FILE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED = eina_error_msg_static_register("Allocation of resources failure prevented load");
   PHOTO_FILE_LOAD_ERROR_CORRUPT_FILE = eina_error_msg_static_register("File corrupt (but was detected as a known format)");
   PHOTO_FILE_LOAD_ERROR_UNKNOWN_FORMAT = eina_error_msg_static_register("File is not a known format");
}

EOLIAN const Elm_Atspi_Action *
_elm_photocam_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Photocam_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "move,prior", "move", "prior", _key_action_move},
          { "move,next", "move", "next", _key_action_move},
          { "move,left", "move", "left", _key_action_move},
          { "move,right", "move", "right", _key_action_move},
          { "move,up", "move", "up", _key_action_move},
          { "move,down", "move", "down", _key_action_move},
          { "zoom,in", "zoom", "in", _key_action_zoom},
          { "zoom,out", "zoom", "out", _key_action_zoom},
          { NULL, NULL, NULL, NULL }
   };
   return &atspi_actions[0];
}

#include "elm_photocam.eo.c"
