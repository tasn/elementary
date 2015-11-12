/*
 * TODO:
 *  - child elements focusing support
 *  - user defined icon/label cb
 *  - show/hide/add buttons ???
 *  - Pattern Filter support
 */
#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#include <Elementary.h>
#include "elm_priv.h"
#include "elm_interface_fileselector.h"
#include "elm_widget_fileselector.h"

#define MY_CLASS ELM_FILESELECTOR_CLASS

#define MY_CLASS_NAME "Elm_Fileselector"
#define MY_CLASS_NAME_LEGACY "elm_fileselector"

/* FIXME: need a way to find a gap between the size of item and thumbnail */
#define GENGRID_PADDING 16

static Elm_Genlist_Item_Class *list_itc[ELM_FILE_LAST];
static Elm_Gengrid_Item_Class *grid_itc[ELM_FILE_LAST];

#define ELM_PRIV_FILESELECTOR_SIGNALS(cmd) \
   cmd(SIG_ACTIVATED, "activated", "s") \
   cmd(SIG_DIRECTORY_OPEN, "directory,open", "s") \
   cmd(SIG_DONE, "done", "s") \
   cmd(SIG_SELECTED, "selected", "s") \
   cmd(SIG_SELECTED_INVALID, "selected,invalid", "s")

ELM_PRIV_FILESELECTOR_SIGNALS(ELM_PRIV_STATIC_VARIABLE_DECLARE);

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   ELM_PRIV_FILESELECTOR_SIGNALS(ELM_PRIV_SMART_CALLBACKS_DESC)
   {SIG_LAYOUT_FOCUSED, ""}, /**< handled by elm_layout */
   {SIG_LAYOUT_UNFOCUSED, ""}, /**< handled by elm_layout */
   {NULL, NULL}
};
#undef ELM_PRIV_FILESELECTOR_SIGNALS

static Eina_Bool _key_action_select(Evas_Object *obj, const char *params);
static Eina_Bool _key_action_escape(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"select", _key_action_select},
   {"escape", _key_action_escape},
   {NULL, NULL}
};

static Eina_Bool _ok(void *data,
      Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _canc(void *data,
      Eo *obj, const Eo_Event_Description *desc, void *event_info);

/* final routine on deletion */
static void
_elm_fileselector_smart_del_do(Elm_Fileselector_Data *sd)
{
   eina_stringshare_del(sd->path);
   eina_stringshare_del(sd->selection);
   free(ecore_idler_del(sd->populate_idler));
   ecore_idler_del(sd->path_entry_idler);

   eo_do_super(sd->obj, MY_CLASS, evas_obj_smart_del());
}

EOLIAN static void
_elm_fileselector_elm_layout_sizing_eval(Eo *obj, Elm_Fileselector_Data *sd EINA_UNUSED)
{
   Evas_Coord minw = -1, minh = -1;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   edje_object_size_min_restricted_calc
     (wd->resize_obj, &minw, &minh, minw, minh);
   evas_object_size_hint_min_set(obj, minw, minh);
}

static void
_mirrored_set(Evas_Object *obj, Eina_Bool rtl)
{
   ELM_FILESELECTOR_DATA_GET(obj, sd);

   elm_widget_mirrored_set(sd->cancel_button, rtl);
   elm_widget_mirrored_set(sd->ok_button, rtl);
   elm_widget_mirrored_set(sd->files_view, rtl);
   elm_widget_mirrored_set(sd->up_button, rtl);
   elm_widget_mirrored_set(sd->home_button, rtl);
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_widget_theme_apply(Eo *obj, Elm_Fileselector_Data *sd)
{
   const char *style;
   const char *data;
   char buf[1024];

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, EINA_FALSE);

   Eina_Bool int_ret = EINA_FALSE;
   eo_do_super(obj, MY_CLASS, int_ret = elm_obj_widget_theme_apply());
   if (!int_ret) return EINA_FALSE;

   style = elm_widget_style_get(obj);
   _mirrored_set(obj, elm_widget_mirrored_get(obj));

   data = edje_object_data_get
       (wd->resize_obj, "path_separator");
   if (data) sd->path_separator = data;
   else sd->path_separator = "/";

   snprintf(buf, sizeof(buf), "fileselector/%s", style);

#define SWALLOW(part_name, object_ptn)                          \
  if (object_ptn)                                               \
    {                                                           \
       elm_widget_style_set(object_ptn, buf);                   \
       if (!elm_layout_content_set(obj, part_name, object_ptn)) \
         evas_object_hide(object_ptn);                          \
    }

   SWALLOW("elm.swallow.up", sd->up_button);
   SWALLOW("elm.swallow.home", sd->home_button);
   SWALLOW("elm.swallow.spinner", sd->spinner);
   elm_object_style_set(sd->spinner, "wheel");

   if (!elm_layout_content_set(obj, "elm.swallow.files", sd->files_view))
     evas_object_hide(sd->files_view);

   SWALLOW("elm.swallow.path", sd->path_entry);
   SWALLOW("elm.swallow.filename", sd->name_entry);
   SWALLOW("elm.swallow.search", sd->search_entry);

   snprintf(buf, sizeof(buf), "fileselector/actions/%s", style);
   SWALLOW("elm.swallow.filters", sd->filter_hoversel);
   SWALLOW("elm.swallow.cancel", sd->cancel_button);
   SWALLOW("elm.swallow.ok", sd->ok_button);

#undef SWALLOW

   edje_object_message_signal_process(wd->resize_obj);

   elm_layout_sizing_eval(obj);

   return EINA_TRUE;
}

static Eina_Bool
_key_action_select(Evas_Object *obj, const char *params EINA_UNUSED)
{
   _ok(obj, NULL, NULL, NULL);
   return EINA_TRUE;
}

static Eina_Bool
_key_action_escape(Evas_Object *obj, const char *params EINA_UNUSED)
{
   _canc(obj, NULL, NULL, NULL);
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_widget_event(Eo *obj, Elm_Fileselector_Data *sd EINA_UNUSED, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   (void) src;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

/***  GENLIST/GENGRID "MODEL"  ***/
static char *
_itc_text_get(void *data,
              Evas_Object *obj   EINA_UNUSED,
              const char *source EINA_UNUSED)
{
   return elm_entry_utf8_to_markup
            (ecore_file_file_get(data)); /* NOTE this will be free()'d by
                                          * the caller */
}

static Evas_Object *
_itc_icon_folder_get(void *data EINA_UNUSED,
                     Evas_Object *obj,
                     const char *source)
{
   Evas_Object *ic, *grid, *f;

   if (strcmp(source, "elm.swallow.icon")) return NULL;

   f = evas_object_data_get(obj, "parent");
   ELM_FILESELECTOR_DATA_GET(f, sd);

   ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "folder");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   evas_object_show(ic);

   grid = elm_grid_add(obj);
   elm_grid_size_set(grid, 1, 1);
   elm_grid_pack(grid, ic, 0, 0, 1, 1);
   evas_object_size_hint_min_set(grid, sd->thumbnail_size.w, sd->thumbnail_size.h);
   evas_object_show(grid);

   return grid;
}

static Evas_Object *
_itc_icon_image_get(void *data,
                    Evas_Object *obj,
                    const char *source)
{
   const char *filename = data;
   Evas_Object *ic, *grid, *f;

   if (strcmp(source, "elm.swallow.icon")) return NULL;

   f = evas_object_data_get(obj, "parent");
   ELM_FILESELECTOR_DATA_GET(f, sd);

   ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "image");
   elm_icon_thumb_set(ic, filename, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   evas_object_show(ic);

   grid = elm_grid_add(obj);
   elm_grid_size_set(grid, 1, 1);
   elm_grid_pack(grid, ic, 0, 0, 1, 1);
   evas_object_size_hint_min_set(grid, sd->thumbnail_size.w, sd->thumbnail_size.h);
   evas_object_show(grid);

   return grid;
}

static Evas_Object *
_itc_icon_file_get(void *data EINA_UNUSED,
                   Evas_Object *obj,
                   const char *source)
{
   Evas_Object *ic, *grid, *f;

   if (strcmp(source, "elm.swallow.icon")) return NULL;

   f = evas_object_data_get(obj, "parent");
   ELM_FILESELECTOR_DATA_GET(f, sd);

   ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "file");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   evas_object_show(ic);

   grid = elm_grid_add(obj);
   elm_grid_size_set(grid, 1, 1);
   elm_grid_pack(grid, ic, 0, 0, 1, 1);
   evas_object_size_hint_min_set(grid, sd->thumbnail_size.w, sd->thumbnail_size.h);
   evas_object_show(grid);

   return grid;
}

static Eina_Bool
_itc_state_get(void *data         EINA_UNUSED,
               Evas_Object *obj   EINA_UNUSED,
               const char *source EINA_UNUSED)
{
   return EINA_FALSE;
}

static void
_itc_del(void *data,
         Evas_Object *obj EINA_UNUSED)
{
   eina_stringshare_del(data);
}

static void
_anchors_do(Evas_Object *obj,
            const char *path)
{
   char **tok, buf[PATH_MAX * 3], *s;
   int i, j;

   ELM_FILESELECTOR_DATA_GET(obj, sd);

   s = elm_entry_utf8_to_markup(path);
   if (!s) return;

   buf[0] = '\0';
   tok = eina_str_split(s, "/", 0);
   free(s);

   eina_strlcat(buf, "<a href='/'>root</a>", sizeof(buf));
   for (i = 0; tok[i]; i++)
     {
        if ((!tok[i]) || (!tok[i][0])) continue;
        eina_strlcat(buf, sd->path_separator, sizeof(buf));
        eina_strlcat(buf, "<a href=", sizeof(buf));
        for (j = 0; j <= i; j++)
          {
             if (strlen(tok[j]) < 1) continue;
             eina_strlcat(buf, "/", sizeof(buf));
             eina_strlcat(buf, tok[j], sizeof(buf));
          }
        eina_strlcat(buf, ">", sizeof(buf));
        eina_strlcat(buf, tok[i], sizeof(buf));
        eina_strlcat(buf, "</a>", sizeof(buf));
     }
   free(tok[0]);
   free(tok);

   elm_object_text_set(sd->path_entry, buf);
}

static Eina_Bool
_mime_type_matched(const char *mime_filter, const char *mime_type)
{
   int i = 0;

   while (mime_filter[i] != '\0')
     {
        if (mime_filter[i] != mime_type[i])
          {
             if (mime_filter[i] == '*' && mime_filter[i + 1] == '\0')
               return EINA_TRUE;

             return EINA_FALSE;
          }
        i++;
     }

   if (mime_type[i] != '\0') return EINA_FALSE;

   return EINA_TRUE;
}

static Eina_Bool
_check_mime_type_filter(const Elm_Fileselector_Filter *filter,
                        const char *file_name)
{
   const char *mime_type = NULL;
   int i;

   mime_type = efreet_mime_type_get(file_name);

   if (!mime_type) return EINA_FALSE;

   for (i = 0; filter->filter.mime_types[i]; ++i)
     {
        if (_mime_type_matched(filter->filter.mime_types[i], mime_type))
          return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eina_Bool
_ls_filter_cb(void *data,
              Eio_File *handler EINA_UNUSED,
              const Eina_File_Direct_Info *info)
{
   Listing_Request *lreq = data;
   Elm_Fileselector_Filter *cf;
   Eina_Bool dir = EINA_FALSE;
   char *pch = NULL, *temp = NULL;
   char temp_path[EINA_PATH_MAX];
   ELM_FILESELECTOR_DATA_GET(lreq->obj, sd);

   if (!sd) return EINA_FALSE;
   if (!sd->hidden_visible && info->path[info->name_start] == '.')
     return EINA_FALSE;

   if (info->type == EINA_FILE_DIR)
     dir = EINA_TRUE;

   if (sd->only_folder && !dir)
     return EINA_FALSE;

   //Search entry filter
   if ((sd->search_string) && (sd->search_string[0] != '\0'))
     {
        strcpy(temp_path, info->path);
        pch = strchr(temp_path, '/');
        while (pch != NULL)
          {
             temp = pch;
             pch = strchr(pch + 1, '/');
          }
        temp++;
        if ((temp) && (sd->search_string) &&
            (!strstr(temp, sd->search_string)))
          return EINA_FALSE;
     }

   cf = sd->current_filter;
   if (!cf)
     return EINA_TRUE;

   switch (cf->filter_type)
     {
      case ELM_FILESELECTOR_MIME_FILTER:
         return dir || _check_mime_type_filter(cf, info->path);
      case ELM_FILESELECTOR_CUSTOM_FILTER:
         return cf->filter.custom->func(info->path, dir,
                                        cf->filter.custom->data);
      default:
         return EINA_FALSE;
     }

   return EINA_FALSE;
}

static const char *
_file_type(const char *a)
{
   char *p = strrchr(a, '.');
   if (!p) return "";

   return p;
}

static int
_strcoll_rev(const char *a, const char *b)
{
   return strcoll(b, a);
}

static int
_strcoll_type(const char *a, const char *b)
{
   return strcoll(_file_type(a), _file_type(b));
}

static int
_strcoll_type_rev(const char *a, const char *b)
{
   return _strcoll_type(b, a);
}

static int
_size_cmp(const char *a, const char *b)
{
   return ecore_file_size(a) - ecore_file_size(b);
}

static int
_size_cmp_rev(const char *a, const char *b)
{
   return _size_cmp(b, a);
}

static int
_modified_cmp(const char *a, const char *b)
{
   if (ecore_file_mod_time(a) > ecore_file_mod_time(b))
     return 1;

   return -1;
}

static int
_modified_cmp_rev(const char *a, const char *b)
{
   return _modified_cmp(b, a);
}

static int
_file_grid_cmp(const void *a, const void *b)
{
   Evas_Object *f;

   const Elm_Object_Item *ga = a;
   const Elm_Object_Item *gb = b;
   const Elm_Gengrid_Item_Class *ca = elm_gengrid_item_item_class_get(ga);
   const Elm_Gengrid_Item_Class *cb = elm_gengrid_item_item_class_get(gb);

   if (ca == grid_itc[ELM_DIRECTORY])
     {
        if (cb != grid_itc[ELM_DIRECTORY])
          return -1;
     }
   else if (cb == grid_itc[ELM_DIRECTORY])
     {
        return 1;
     }

   f = evas_object_data_get(elm_object_item_widget_get(ga), "parent");
   ELM_FILESELECTOR_DATA_GET(f, sd);
   return sd->sort_method(elm_object_item_data_get(ga),
                          elm_object_item_data_get(gb));
}

static int
_file_list_cmp(const void *a, const void *b)
{
   Evas_Object *f;

   const Elm_Object_Item *la = a;
   const Elm_Object_Item *lb = b;
   const Elm_Genlist_Item_Class *ca = elm_genlist_item_item_class_get(la);
   const Elm_Genlist_Item_Class *cb = elm_genlist_item_item_class_get(lb);

   if (ca == list_itc[ELM_DIRECTORY])
     {
        if (cb != list_itc[ELM_DIRECTORY])
          return -1;
     }
   else if (cb == list_itc[ELM_DIRECTORY])
     {
        return 1;
     }

   f = evas_object_data_get(elm_object_item_widget_get(la), "parent");
   ELM_FILESELECTOR_DATA_GET(f, sd);
   return sd->sort_method(elm_object_item_data_get(la),
                          elm_object_item_data_get(lb));
}

static void
_signal_first(Listing_Request *lreq)
{
   ELM_FILESELECTOR_DATA_GET(lreq->obj, sd);

   if (!lreq->first) return;
   if (!sd) return;

   if (sd->multi)
     {
        char *path;
        EINA_LIST_FREE(sd->paths, path) free(path);
     }

   eo_do(lreq->obj, eo_event_callback_call
     (ELM_FILESELECTOR_EVENT_DIRECTORY_OPEN, (void *)lreq->path));

   if (!lreq->parent_it)
     {
        if (sd->mode == ELM_FILESELECTOR_LIST)
          elm_genlist_clear(sd->files_view);
        else
          elm_gengrid_clear(sd->files_view);
        eina_stringshare_replace(&sd->path, lreq->path);
        _anchors_do(lreq->obj, lreq->path);
     }

   lreq->first = EINA_FALSE;
}

static void
_ls_main_cb(void *data,
            Eio_File *handler,
            const Eina_File_Direct_Info *info)
{
   Listing_Request *lreq = data;
   Elm_Object_Item *item;
   int itcn = ELM_FILE_UNKNOW;
   ELM_FILESELECTOR_DATA_GET(lreq->obj, sd);
   
   if (eio_file_check(handler)) return;
   if (!sd) return;

   if (!sd->files_view || sd->current != handler)
     {
        eio_file_cancel(handler);
        return;
     }

   _signal_first(lreq);

   if (info->type == EINA_FILE_DIR)
     itcn = ELM_DIRECTORY;
   else
     {
        if (evas_object_image_extension_can_load_get
              (info->path + info->name_start))
          itcn = ELM_FILE_IMAGE;
     }

   if (sd->mode == ELM_FILESELECTOR_LIST)
     {
        item = elm_genlist_item_sorted_insert(sd->files_view, list_itc[itcn],
                                              eina_stringshare_add(info->path),
                                              lreq->parent_it,
                                              ((sd->expand) && (itcn == ELM_DIRECTORY))
                                              ? ELM_GENLIST_ITEM_TREE : ELM_GENLIST_ITEM_NONE,
                                              _file_list_cmp, NULL, NULL);

        if (lreq->selected && !strcmp(info->path, lreq->selected))
          {
             elm_genlist_item_selected_set(item, EINA_TRUE);
             elm_object_text_set(sd->name_entry, ecore_file_file_get(info->path));
          }
     }
   else if (sd->mode == ELM_FILESELECTOR_GRID)
     {
        item = elm_gengrid_item_sorted_insert(sd->files_view, grid_itc[itcn],
                                              eina_stringshare_add(info->path),
                                              _file_grid_cmp, NULL, NULL);

        if (lreq->selected && !strcmp(info->path, lreq->selected))
          {
             elm_gengrid_item_selected_set(item, EINA_TRUE);
             elm_object_text_set(sd->name_entry, ecore_file_file_get(info->path));
          }
     }
}

static void
_listing_request_cleanup(Listing_Request *lreq)
{
   eina_stringshare_del(lreq->path);
   eina_stringshare_del(lreq->selected);
   free(lreq);
}

static void
_ls_done_cb(void *data, Eio_File *handler EINA_UNUSED)
{
   Listing_Request *lreq = data;
   ELM_FILESELECTOR_DATA_GET(lreq->obj, sd);

   _signal_first(lreq);
   if (sd)
     {
        elm_progressbar_pulse(sd->spinner, EINA_FALSE);
        elm_layout_signal_emit(lreq->obj, "elm,action,spinner,hide", "elm");
        sd->current = NULL;
     }

   _listing_request_cleanup(lreq);
}

static void
_ls_error_cb(void *data, Eio_File *handler, int error EINA_UNUSED)
{
   Listing_Request *lreq = data;
   ELM_FILESELECTOR_DATA_GET(lreq->obj, sd);

   if (sd)
     {
        elm_progressbar_pulse(sd->spinner, EINA_FALSE);
        elm_layout_signal_emit(lreq->obj, "elm,action,spinner,hide", "elm");
        if (sd->current == handler) sd->current = NULL;
     }
   _listing_request_cleanup(lreq);
}

static void
_populate(Evas_Object *obj,
          const char *path,
          Elm_Object_Item *parent_it,
          const char *selected)
{
   ELM_FILESELECTOR_DATA_GET(obj, sd);
   if (!path) return;

   Listing_Request *lreq;

   if (sd->expand && sd->current) return;

   if (sd->monitor) eio_monitor_del(sd->monitor);
   if (sd->current) eio_file_cancel(sd->current);
   sd->monitor = NULL;
   sd->current = NULL;

   lreq = malloc(sizeof (Listing_Request));
   if (!lreq) return;

   lreq->parent_it = parent_it; /* FIXME: should we refcount the parent_it ? */
   lreq->obj = obj;
   lreq->path = eina_stringshare_add(path);
   lreq->first = EINA_TRUE;

   if (selected)
     lreq->selected = eina_stringshare_add(selected);
   else
     lreq->selected = NULL;

   /* TODO: sub directory should be monitored for expand mode */
   sd->monitor = eio_monitor_add(path);
   sd->current = eio_file_stat_ls(path, _ls_filter_cb, _ls_main_cb,
                                  _ls_done_cb, _ls_error_cb, lreq);
   elm_progressbar_pulse(sd->spinner, EINA_TRUE);
   elm_layout_signal_emit(lreq->obj, "elm,action,spinner,show", "elm");

   // Clear name entry not in case of save mode.
   if (elm_object_disabled_get(sd->name_entry))
     elm_object_text_set(sd->name_entry, "");

}

static Eina_Bool
_on_list_expanded(void *data,
                  Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                  void *event_info)
{
   Elm_Object_Item *it = event_info;
   const char *path = elm_object_item_data_get(it);

   _populate(data, path, it, NULL);

   return EINA_TRUE;
}

static Eina_Bool
_on_list_contracted(void *data EINA_UNUSED,
                    Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                    void *event_info)
{
   Elm_Object_Item *it = event_info;

   elm_genlist_item_subitems_clear(it);

   return EINA_TRUE;
}

static Eina_Bool
_on_list_expand_req(void *data EINA_UNUSED,
                    Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                    void *event_info)
{
   Elm_Object_Item *it = event_info;

   elm_genlist_item_expanded_set(it, EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_on_list_contract_req(void *data EINA_UNUSED,
                      Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                      void *event_info)
{
   Elm_Object_Item *it = event_info;

   elm_genlist_item_expanded_set(it, EINA_FALSE);

   return EINA_TRUE;
}

static Eina_Bool
_populate_do(void *data)
{
   struct sel_data *sdata = data;
   ELM_FILESELECTOR_DATA_GET(sdata->fs, sd);

   _populate(sdata->fs, sdata->path, NULL, sdata->selected);
   eina_stringshare_del(sdata->path);
   eina_stringshare_del(sdata->selected);

   sd->populate_idler = NULL;

   free(sdata);
   return ECORE_CALLBACK_CANCEL;
}

static void
_schedule_populate(Evas_Object *fs,
                   Elm_Fileselector_Data *sd,
                   Eina_Stringshare *path,
                   Eina_Stringshare *selected)
{
   struct sel_data *sdata;
   sdata = malloc(sizeof(*sdata));
   if (!sdata) return;

   sdata->fs = fs;
   sdata->path = path;
   sdata->selected = selected;

   if (sd->populate_idler)
     {
        struct sel_data *old_sdata;
        old_sdata = ecore_idler_del(sd->populate_idler);
        eina_stringshare_del(old_sdata->path);
        eina_stringshare_del(old_sdata->selected);
        free(old_sdata);
     }
   sd->populate_idler = ecore_idler_add(_populate_do, sdata);
}

static Eina_Bool
_on_item_activated(void *data,
                   Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                   void *event_info)
{
   //This event_info could be a list or gengrid item
   Elm_Object_Item *it = event_info;
   const char *path;
   Eina_Bool is_dir;

   ELM_FILESELECTOR_DATA_GET(data, sd);

   path = elm_object_item_data_get(it);
   if (!path) return EINA_TRUE;

   is_dir = ecore_file_is_dir(path);
   if (!is_dir)
     {
        eo_do(data, eo_event_callback_call
          (ELM_FILESELECTOR_EVENT_ACTIVATED, (void *)path));
        return EINA_TRUE;
     }

   if (!sd->double_tap_navigation) return EINA_TRUE;

   _schedule_populate(data, sd, eina_stringshare_add(path), NULL);

   return EINA_TRUE;
}

static void
_clear_selections(Elm_Fileselector_Data *sd, Elm_Object_Item *last_selected)
{
   Eina_List *items;
   Elm_Object_Item *sel;

   if (sd->mode == ELM_FILESELECTOR_LIST)
     {
        items = eina_list_clone(elm_genlist_selected_items_get(sd->files_view));

        EINA_LIST_FREE(items, sel)
          {
            if (sel == last_selected) continue;
            elm_genlist_item_selected_set(sel, EINA_FALSE);
          }
     }
   else if (sd->mode == ELM_FILESELECTOR_GRID)
     {
        items = eina_list_clone(elm_gengrid_selected_items_get(sd->files_view));

        EINA_LIST_FREE(items, sel)
          {
            if (sel == last_selected) continue;
            elm_gengrid_item_selected_set(sel, EINA_FALSE);
          }
     }
}

static Eina_Bool
_on_item_selected(void *data,
                  Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                  void *event_info)
{
   //This event_info could be a list or gengrid item
   Elm_Object_Item *it = event_info;
   const char *path;
   char *parent_path;
   Eina_Bool is_dir;

   ELM_FILESELECTOR_DATA_GET(data, sd);

   path = elm_object_item_data_get(it);
   if (!path) return EINA_TRUE;

   is_dir = ecore_file_is_dir(path);

   /* We need to send callback when:
    * - path is dir and mode is ONLY FOLDER
    * - path is file and mode is NOT ONLY FOLDER */
   if (is_dir == sd->only_folder)
     {
        if (sd->multi)
          {
             Eina_List *li;
             const char *p;
             Eina_Strbuf *buf;

             if (sd->dir_selected)
               {
                  _clear_selections(sd, it);
                  sd->dir_selected = EINA_FALSE;
               }

             buf = eina_strbuf_new();
             EINA_LIST_FOREACH(sd->paths, li, p)
               {
                  eina_strbuf_append(buf, ecore_file_file_get(p));
                  eina_strbuf_append_length(buf, ", ", 2);
               }

             sd->paths = eina_list_append(sd->paths, strdup(path));
             eina_strbuf_append(buf, ecore_file_file_get(path));

             elm_object_text_set(sd->name_entry, eina_strbuf_string_get(buf));
             eina_strbuf_free(buf);
          }
        else
          elm_object_text_set(sd->name_entry, ecore_file_file_get(path));

        eo_do(data, eo_event_callback_call
          (EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, (void *)path));
     }
   else if (sd->multi && is_dir && sd->double_tap_navigation)
     {
        _clear_selections(sd, it);
        sd->dir_selected = EINA_TRUE;
     }

   /* We need to populate, if path is directory and:
    * - mode is GRID;
    * - mode is LIST and 'not expand mode';
    *   in other cases update anchors. */
   if (!is_dir) return EINA_TRUE;

   if (sd->expand && sd->mode == ELM_FILESELECTOR_LIST)
     {
        if (sd->only_folder)
          {
             parent_path = ecore_file_dir_get(path);
             eina_stringshare_replace(&sd->path, parent_path);
             _anchors_do(data, parent_path);
             free(parent_path);
          }
        else
          {
             eina_stringshare_replace(&sd->path, path);
             _anchors_do(data, path);
          }
        return EINA_TRUE;
     }

   if (sd->double_tap_navigation) return EINA_TRUE;

   _schedule_populate(data, sd, eina_stringshare_add(path), NULL);

   return EINA_TRUE;
}

static Eina_Bool
_on_item_unselected(void *data,
                    Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                    void *event_info)
{
   Eina_List *li, *l;
   char *path;
   const char *unselected_path;
   Eina_Strbuf *buf;
   Elm_Object_Item *it = event_info;
   Eina_Bool first = EINA_TRUE;

   ELM_FILESELECTOR_DATA_GET(data, sd);

   if (!sd->multi) return EINA_TRUE;

   unselected_path = elm_object_item_data_get(it);
   if (!unselected_path) return EINA_TRUE;

   buf = eina_strbuf_new();
   EINA_LIST_FOREACH_SAFE(sd->paths, li, l, path)
     {
        if (!strcmp(path, unselected_path))
          {
             sd->paths = eina_list_remove_list(sd->paths, li);
             free(path);
          }
        else
          {
             if (!first)
               eina_strbuf_append_length(buf, ", ", 2);
             else
               first = EINA_FALSE;

             eina_strbuf_append(buf, ecore_file_file_get(path));
          }
     }

   elm_object_text_set(sd->name_entry, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);

   return EINA_TRUE;
}

static Eina_Bool
_on_dir_up(void *data,
           Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
           void *event_info EINA_UNUSED)
{
   Evas_Object *fs = data;
   char *parent;

   ELM_FILESELECTOR_DATA_GET(fs, sd);

   parent = ecore_file_dir_get(sd->path);
   _populate(fs, parent, NULL, NULL);
   free(parent);

   return EINA_TRUE;
}

static Eina_Bool
_home(void *data,
      Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
      void *event_info EINA_UNUSED)
{
   Evas_Object *fs = data;

   _populate(fs, eina_environment_home_get(), NULL, NULL);

   return EINA_TRUE;
}

static void
_current_filter_changed(void *data,
                        Evas_Object *obj,
                        void *event_info EINA_UNUSED)
{
   Elm_Fileselector_Filter *filter = data;

   if (filter->sd->current_filter == filter) return;

   elm_object_text_set(obj, filter->filter_name);
   filter->sd->current_filter = filter;

   _populate(filter->sd->obj, filter->sd->path, NULL, NULL);
}

static Eina_Bool
_ok(void *data,
    Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
    void *event_info EINA_UNUSED)
{
   const char *name;
   const char *selection = NULL;
   Evas_Object *fs = data;
   ELM_FILESELECTOR_DATA_GET(fs, sd);

   if (!sd->path)
     {
        eo_do(fs, eo_event_callback_call(ELM_FILESELECTOR_EVENT_DONE, NULL));
         return EINA_TRUE;
     }

   name = elm_object_text_get(sd->name_entry);
   if (name && name[0] != '\0')
     {
        int len = eina_stringshare_strlen(sd->path);
        if (sd->path[len - 1] == '/')
          selection = eina_stringshare_printf("%s%s", sd->path, name);
        else
          selection = eina_stringshare_printf("%s/%s", sd->path, name);
     }
   else
     selection = eina_stringshare_add(elm_fileselector_selected_get(fs));

   eo_do(fs, eo_event_callback_call
     (ELM_FILESELECTOR_EVENT_DONE, (void *)selection));
   eina_stringshare_del(selection);

   return EINA_TRUE;
}

static Eina_Bool
_canc(void *data,
      Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
      void *event_info EINA_UNUSED)
{
   Evas_Object *fs = data;

   eo_do(fs, eo_event_callback_call(ELM_FILESELECTOR_EVENT_DONE, NULL));

   return EINA_TRUE;
}

static Eina_Bool
_on_text_activated(void *data,
                   Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                   void *event_info EINA_UNUSED)
{
   Evas_Object *fs = data;
   const char *p, *path;
   char *dir;

   ELM_FILESELECTOR_DATA_GET(fs, sd);

   path = elm_widget_part_text_get(obj, NULL);

   if (!ecore_file_exists(path))
     {
        eo_do(fs, eo_event_callback_call
          (EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, (void *)path));
        eo_do(fs, eo_event_callback_call
          (ELM_FILESELECTOR_EVENT_SELECTED_INVALID, (void *)path));
        goto end;
     }

   if (ecore_file_is_dir(path))
     {
        // keep a ref to path 'couse it will be destroyed by _populate
        p = eina_stringshare_add(path);
        _populate(fs, p, NULL, NULL);
        eina_stringshare_del(p);

        if (sd->only_folder)
          eo_do(fs, eo_event_callback_call
            (EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, (void *)path));

        goto end;
     }

   dir = ecore_file_dir_get(path);
   if (!dir) goto end;

   if (strcmp(dir, sd->path))
     {
        _populate(fs, dir, NULL, path);

        if (sd->only_folder)
          eo_do(fs, eo_event_callback_call
            (EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, (void *)path));
     }
   else
     {
        if (sd->mode == ELM_FILESELECTOR_LIST)
          {
             Elm_Object_Item *item = elm_genlist_first_item_get(sd->files_view);
             while (item)
               {
                  const char *item_path = WIDGET_ITEM_DATA_GET(item);
                  if (!strcmp(item_path, path))
                    {
                       elm_genlist_item_selected_set(item, EINA_TRUE);
                       elm_widget_part_text_set(sd->name_entry, NULL,
                                                ecore_file_file_get(path));
                       break;
                    }
                  item = elm_genlist_item_next_get(item);
               }
          }
        else
          {
             Elm_Object_Item *item = elm_gengrid_first_item_get(sd->files_view);
             while (item)
               {
                  const char *item_path = elm_object_item_data_get(item);
                  if (!strcmp(item_path, path))
                    {
                       elm_gengrid_item_selected_set(item, EINA_TRUE);
                       elm_widget_part_text_set(sd->name_entry, NULL,
                                                ecore_file_file_get(path));
                       break;
                    }
                  item = elm_gengrid_item_next_get(item);
               }
          }
     }
   free(dir);

end:
   elm_object_focus_set(obj, EINA_FALSE);

   return EINA_TRUE;
}

static Eina_Bool
_anchors_undo(void *data)
{
   ELM_FILESELECTOR_DATA_GET(data, sd);

   elm_entry_entry_set(sd->path_entry, sd->path);
   elm_entry_cursor_pos_set(sd->path_entry, eina_stringshare_strlen(sd->path));

   sd->path_entry_idler = NULL;

   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_on_text_focused(void *data,
                 Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                 void *event_info EINA_UNUSED)
{
   ELM_FILESELECTOR_DATA_GET(data, sd);

   if (!sd->path_entry_idler)
       sd->path_entry_idler = ecore_idler_add(_anchors_undo, data);

   return EINA_TRUE;
}

static Eina_Bool
_on_text_unfocused(void *data,
                   Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                   void *event_info EINA_UNUSED)
{
   ELM_FILESELECTOR_DATA_GET(data, sd);
   _anchors_do(data, sd->path);

   return EINA_TRUE;
}

static Eina_Bool
_anchor_clicked(void *data,
                Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED,
                void *event_info)
{
   Elm_Entry_Anchor_Info *info = event_info;
   Evas_Object *fs = data;
   const char *p;

   ELM_FILESELECTOR_DATA_GET(fs, sd);

   // keep a ref to path 'couse it will be destroyed by _populate
   p = eina_stringshare_add(info->name);
   _populate(fs, p, NULL, NULL);
   eina_stringshare_del(p);
   /* After anchor was clicked, entry will be focused, and will be editable.
    * It's wrong. So remove focus. */
   elm_object_focus_set(obj, EINA_FALSE);

   if (sd->path_entry_idler) {
       ecore_idler_del(sd->path_entry_idler);
       sd->path_entry_idler = NULL;
   }

   return EINA_TRUE;
}

static Eina_Bool
_files_key_down(void *data,
      Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
     Evas_Event_Key_Down *ev = event_info;
     Evas_Object *par, *searchbar;

     par = data;
     searchbar = evas_object_data_get(par, "search");

     if (!searchbar) return EINA_TRUE;

     if (((ev->string) && *(ev->string) &&
               (isalpha(*ev->string) ||
                isdigit(*ev->string))))
       {

          elm_entry_entry_append(searchbar, ev->string);
       }
     else if (ev->string &&
              *(ev->string) &&
              !strcmp(ev->key, "BackSpace"))
       {
          char buf[PATH_MAX];
          const char *en;
          en = elm_entry_entry_get(searchbar);
          if (strlen(en) > 0)
            {
               memmove(buf, en, strlen(en) -1);
               buf[strlen(en) -1] = '\0';
               elm_entry_entry_set(searchbar, buf);
            }
       }

   return EINA_TRUE;
}

static Evas_Object *
_files_list_add(Evas_Object *obj)
{
   Evas_Object *li;

   li = elm_genlist_add(obj);
   evas_object_data_set(li, "parent", obj);
   elm_widget_mirrored_automatic_set(li, EINA_FALSE);
   evas_object_size_hint_align_set(li, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_min_set(li, 100, 100);

   eo_do(li, eo_event_callback_add
     (EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, _on_item_selected, obj));
   eo_do(li, eo_event_callback_add
     (EVAS_SELECTABLE_INTERFACE_EVENT_UNSELECTED, _on_item_unselected, obj));
   eo_do(li, eo_event_callback_add
     (ELM_GENLIST_EVENT_ACTIVATED, _on_item_activated, obj));
   eo_do(li, eo_event_callback_add
     (ELM_GENLIST_EVENT_EXPAND_REQUEST, _on_list_expand_req, obj));
   eo_do(li, eo_event_callback_add
     (ELM_GENLIST_EVENT_CONTRACT_REQUEST, _on_list_contract_req, obj));
   eo_do(li, eo_event_callback_add
     (ELM_GENLIST_EVENT_EXPANDED, _on_list_expanded, obj));
   eo_do(li, eo_event_callback_add
     (ELM_GENLIST_EVENT_CONTRACTED, _on_list_contracted, obj));
   eo_do(li, eo_event_callback_add
     (EVAS_OBJECT_EVENT_KEY_DOWN, _files_key_down, obj));

   elm_widget_sub_object_add(obj, li);

   return li;
}

static Evas_Object *
_files_grid_add(Evas_Object *obj)
{
   Evas_Object *grid;

   ELM_FILESELECTOR_DATA_GET(obj, sd);

   grid = elm_gengrid_add(obj);
   evas_object_data_set(grid, "parent", obj);
   elm_widget_mirrored_automatic_set(grid, EINA_FALSE);
   evas_object_size_hint_align_set(grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   elm_gengrid_item_size_set(grid, sd->thumbnail_size.w + GENGRID_PADDING, sd->thumbnail_size.h + GENGRID_PADDING);

   elm_gengrid_align_set(grid, 0.0, 0.0);

   eo_do(grid, eo_event_callback_add
     (EVAS_SELECTABLE_INTERFACE_EVENT_SELECTED, _on_item_selected, obj));
   eo_do(grid, eo_event_callback_add
     (EVAS_SELECTABLE_INTERFACE_EVENT_UNSELECTED, _on_item_unselected, obj));
   eo_do(grid, eo_event_callback_add
     (ELM_GENGRID_EVENT_ACTIVATED, _on_item_activated, obj));
   eo_do(grid, eo_event_callback_add
     (EVAS_OBJECT_EVENT_KEY_DOWN, _files_key_down, obj));

   elm_widget_sub_object_add(obj, grid);

   return grid;
}

static Eina_Bool
_resource_created(void *data, int type, void *ev)
{
   Evas_Object *obj = data;
   Eio_Monitor_Event *event = ev;
   int itcn = ELM_FILE_UNKNOW;
   Eina_Bool dir = EINA_FALSE;

   ELM_FILESELECTOR_DATA_GET(obj, sd);

   if (type == EIO_MONITOR_DIRECTORY_CREATED)
     dir = EINA_TRUE;

   Elm_Fileselector_Filter *cf = sd->current_filter;
   if (cf)
     {
        switch (cf->filter_type)
          {
           case ELM_FILESELECTOR_MIME_FILTER:
              if (!dir && !_check_mime_type_filter(cf, event->filename))
                return ECORE_CALLBACK_PASS_ON;
              break;
           case ELM_FILESELECTOR_CUSTOM_FILTER:
              if (!cf->filter.custom->func(event->filename, dir, cf->filter.custom->data))
                return ECORE_CALLBACK_PASS_ON;
              break;
           default:
              break;
          }
     }

   if (dir)
     itcn = ELM_DIRECTORY;
   else
     {
        if (evas_object_image_extension_can_load_get(event->filename))
          itcn = ELM_FILE_IMAGE;
     }

   if (sd->mode == ELM_FILESELECTOR_LIST)
     elm_genlist_item_sorted_insert(sd->files_view, list_itc[itcn],
                                    eina_stringshare_add(event->filename),
                                    NULL,
                                    (sd->expand && itcn == ELM_DIRECTORY)
                                    ? ELM_GENLIST_ITEM_TREE : ELM_GENLIST_ITEM_NONE,
                                    _file_list_cmp, NULL, NULL);
   else
     elm_gengrid_item_sorted_insert(sd->files_view, grid_itc[itcn],
                                    eina_stringshare_add(event->filename),
                                    _file_grid_cmp, NULL, NULL);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_resource_deleted(void *data, int type EINA_UNUSED, void *ev)
{
   Evas_Object *obj = data;
   Eio_Monitor_Event *event = ev;
   Elm_Object_Item *it = NULL;
   Eina_Bool selected = EINA_FALSE;

   ELM_FILESELECTOR_DATA_GET(obj, sd);

   if (sd->mode == ELM_FILESELECTOR_LIST)
     {
        it = elm_genlist_first_item_get(sd->files_view);
        while (it)
          {
             if (!strcmp(elm_object_item_data_get(it), event->filename))
               {
                  selected = elm_genlist_item_selected_get(it);
                  break;
               }
             it = elm_genlist_item_next_get(it);
          }
     }
   else
     {
        it = elm_gengrid_first_item_get(sd->files_view);
        while (it)
          {
             if (!strcmp(elm_object_item_data_get(it), event->filename))
               {
                  selected = elm_genlist_item_selected_get(it);
                  break;
               }
             it = elm_gengrid_item_next_get(it);
          }
     }

   if (it) eo_do(it, elm_wdg_item_del());

   if (selected)
     {
        if (sd->multi)
          {
             Eina_List *li, *l;
             char *path;
             Eina_Strbuf *buf;
             Eina_Bool first = EINA_TRUE;

             buf = eina_strbuf_new();
             EINA_LIST_FOREACH_SAFE(sd->paths, li, l, path)
               {
                  if (!strcmp(path, event->filename))
                    {
                       sd->paths = eina_list_remove_list(sd->paths, li);
                       free(path);
                    }
                  else
                    {
                       if (!first)
                         eina_strbuf_append_length(buf, ", ", 2);
                       else
                         first = EINA_FALSE;

                       eina_strbuf_append(buf, ecore_file_file_get(path));
                    }
               }

             elm_object_text_set(sd->name_entry, eina_strbuf_string_get(buf));
             eina_strbuf_free(buf);
          }
        else
          elm_object_text_set(sd->name_entry, "");
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_preedit_cb(void *data,
      Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   ELM_FILESELECTOR_DATA_GET(data, sd);

   sd->search_string = elm_entry_entry_get(obj);

   if (sd->search_string && sd->path)
     _populate(data, sd->path, NULL, NULL);

   return EINA_TRUE;
}

EOLIAN static void
_elm_fileselector_evas_object_smart_add(Eo *obj, Elm_Fileselector_Data *priv)
{
   Evas_Object *ic, *bt, *en, *pb;

   EINA_REFCOUNT_INIT(priv);

   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());
   elm_widget_sub_object_parent_add(obj);
   elm_widget_can_focus_set(obj, EINA_TRUE);

   priv->expand = !!_elm_config->fileselector_expand_enable;
   priv->double_tap_navigation = !!_elm_config->fileselector_double_tap_navigation_enable;

   if (!elm_layout_theme_set
       (obj, "fileselector", "base", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   // up btn
   ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "arrow_up");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   bt = elm_button_add(obj);
   elm_widget_mirrored_automatic_set(bt, EINA_FALSE);
   elm_object_part_content_set(bt, "icon", ic);
   elm_object_domain_translatable_text_set(bt, PACKAGE, N_("Up"));
   evas_object_size_hint_align_set(bt, 0.0, 0.0);
   eo_do(bt, eo_event_callback_add
     (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, _on_dir_up, obj));

   elm_widget_sub_object_add(obj, bt);

   priv->up_button = bt;

   // home btn
   ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "home");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   bt = elm_button_add(obj);
   elm_widget_mirrored_automatic_set(bt, EINA_FALSE);
   elm_object_part_content_set(bt, "icon", ic);
   elm_object_domain_translatable_text_set(bt, PACKAGE, N_("Home"));
   evas_object_size_hint_align_set(bt, 0.0, 0.0);
   eo_do(bt, eo_event_callback_add
     (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, _home, obj));

   priv->home_button = bt;

   // spinner
   pb = elm_progressbar_add(obj);
   elm_widget_sub_object_add(obj, pb);
   priv->spinner = pb;

   // XXX: will fail for dynamic finger size changing
   priv->thumbnail_size.w = elm_config_finger_size_get() * 2 - GENGRID_PADDING;
   priv->thumbnail_size.h = priv->thumbnail_size.w;

   priv->sort_type = ELM_FILESELECTOR_SORT_BY_FILENAME_ASC;
   priv->sort_method = strcoll;

   // files_view
   priv->files_view = _files_list_add(obj);

   // path entry
   en = elm_entry_add(obj);
   elm_entry_scrollable_set(en, EINA_TRUE);
   elm_widget_mirrored_automatic_set(en, EINA_FALSE);
   elm_entry_single_line_set(en, EINA_TRUE);
   elm_entry_line_wrap_set(en, ELM_WRAP_CHAR);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, EVAS_HINT_FILL);

   eo_do(en, eo_event_callback_add
     (ELM_ENTRY_EVENT_ANCHOR_CLICKED, _anchor_clicked, obj));
   eo_do(en, eo_event_callback_add
     (ELM_WIDGET_EVENT_FOCUSED, _on_text_focused, obj));
   eo_do(en, eo_event_callback_add
     (ELM_WIDGET_EVENT_UNFOCUSED, _on_text_unfocused, obj));
   eo_do(en, eo_event_callback_add
     (ELM_ENTRY_EVENT_ACTIVATED, _on_text_activated, obj));

   elm_widget_sub_object_add(obj, en);
   priv->path_entry = en;

   // name entry
   en = elm_entry_add(obj);
   elm_entry_scrollable_set(en, EINA_TRUE);
   elm_widget_mirrored_automatic_set(en, EINA_FALSE);
   elm_entry_editable_set(en, EINA_TRUE);
   elm_entry_single_line_set(en, EINA_TRUE);
   elm_entry_line_wrap_set(en, ELM_WRAP_CHAR);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, EVAS_HINT_FILL);

   elm_widget_sub_object_add(obj, en);
   priv->name_entry = en;

   //search entry
   ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "search");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   en = elm_entry_add(obj);
   elm_entry_scrollable_set(en, EINA_TRUE);
   elm_widget_mirrored_automatic_set(en, EINA_FALSE);
   elm_entry_editable_set(en, EINA_TRUE);
   elm_entry_single_line_set(en, EINA_TRUE);
   elm_entry_line_wrap_set(en, ELM_WRAP_CHAR);
   elm_object_domain_translatable_part_text_set(en, "guide",
                                                PACKAGE, N_("Search"));
   elm_object_part_content_set(en, "icon", ic);
   elm_entry_icon_visible_set(en, EINA_TRUE);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, EVAS_HINT_FILL);
   eo_do(en, eo_event_callback_add
     (ELM_ENTRY_EVENT_CHANGED, _preedit_cb, obj));
   evas_object_data_set(obj, "search", en);

   elm_widget_sub_object_add(obj, en);
   priv->search_entry = en;

   elm_fileselector_buttons_ok_cancel_set(obj, EINA_TRUE);
   elm_fileselector_is_save_set(obj, EINA_FALSE);

#define HANDLER_ADD(e, fn) \
   priv->handlers = eina_list_append(priv->handlers, \
                                     ecore_event_handler_add(e, fn, obj));

   HANDLER_ADD(EIO_MONITOR_FILE_CREATED, _resource_created);
   HANDLER_ADD(EIO_MONITOR_DIRECTORY_CREATED, _resource_created);

   HANDLER_ADD(EIO_MONITOR_FILE_DELETED, _resource_deleted);
   HANDLER_ADD(EIO_MONITOR_DIRECTORY_DELETED, _resource_deleted);
#undef HANDLER_ADD

   eo_do(obj, elm_obj_widget_theme_apply());
}

EOLIAN static void
_elm_fileselector_evas_object_smart_del(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   Elm_Fileselector_Filter *filter;
   char *path;
   Ecore_Event_Handler *h;

   if (sd->monitor) eio_monitor_del(sd->monitor);
   if (sd->current) eio_file_cancel(sd->current);
   sd->monitor = NULL;
   sd->current = NULL;

   EINA_LIST_FREE(sd->handlers, h)
     {
        ecore_event_handler_del(h);
     }

   EINA_LIST_FREE(sd->filter_list, filter)
     {
        eina_stringshare_del(filter->filter_name);

        if (filter->filter_type == ELM_FILESELECTOR_MIME_FILTER)
          {
             free(filter->filter.mime_types[0]);
             free(filter->filter.mime_types);
          }
        else
          free(filter->filter.custom);

        free(filter);
     }

   EINA_LIST_FREE(sd->paths, path)
     free(path);

   sd->files_view = NULL;

   /* this one matching EINA_REFCOUNT_INIT() */
   EINA_REFCOUNT_UNREF(sd) _elm_fileselector_smart_del_do(sd);
}

EAPI Evas_Object *
elm_fileselector_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   Evas_Object *obj = eo_add(MY_CLASS, parent);
   return obj;
}

EOLIAN static Eo *
_elm_fileselector_eo_base_constructor(Eo *obj, Elm_Fileselector_Data *sd)
{
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());
   sd->obj = obj;
   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY),
         evas_obj_smart_callbacks_descriptions_set(_smart_callbacks),
         elm_interface_atspi_accessible_role_set(ELM_ATSPI_ROLE_FILE_CHOOSER));

   return obj;
}

EAPI void
elm_fileselector_is_save_set(Evas_Object *obj,
                             Eina_Bool is_save)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_is_save_set(is_save));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_is_save_set(Eo *obj, Elm_Fileselector_Data *sd, Eina_Bool is_save)
{
   elm_object_disabled_set(sd->name_entry, !is_save);

   if (is_save) elm_layout_signal_emit(obj, "elm,state,save,on", "elm");
   else elm_layout_signal_emit(obj, "elm,state,save,off", "elm");
}

EAPI Eina_Bool
elm_fileselector_is_save_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, EINA_FALSE);
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_is_save_get());
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_is_save_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return !elm_object_disabled_get(sd->name_entry);
}

EAPI void
elm_fileselector_folder_only_set(Evas_Object *obj,
                                 Eina_Bool only)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_folder_only_set(only));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_folder_only_set(Eo *obj, Elm_Fileselector_Data *sd, Eina_Bool only)
{
   if (sd->only_folder == only) return;

   sd->only_folder = !!only;
   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }
}

EAPI Eina_Bool
elm_fileselector_folder_only_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, EINA_FALSE);
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_folder_only_get());
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_folder_only_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->only_folder;
}

EOLIAN static void
_elm_fileselector_buttons_ok_cancel_set(Eo *obj, Elm_Fileselector_Data *sd, Eina_Bool visible)
{
   Evas_Object *bt;

   if (visible && (!sd->ok_button))
     {
        // cancel btn
        bt = elm_button_add(obj);
        elm_widget_mirrored_automatic_set(bt, EINA_FALSE);
        elm_object_domain_translatable_text_set(bt, PACKAGE, N_("Cancel"));

        eo_do(bt, eo_event_callback_add
          (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, _canc, obj));

        sd->cancel_button = bt;

        // ok btn
        bt = elm_button_add(obj);
        elm_widget_mirrored_automatic_set(bt, EINA_FALSE);
        elm_object_domain_translatable_text_set(bt, PACKAGE, N_("OK"));

        eo_do(bt, eo_event_callback_add
          (EVAS_CLICKABLE_INTERFACE_EVENT_CLICKED, _ok, obj));

        sd->ok_button = bt;

        eo_do(obj, elm_obj_widget_theme_apply());
     }
   else if (!visible)
     {
        ELM_SAFE_FREE(sd->cancel_button, evas_object_del);
        ELM_SAFE_FREE(sd->ok_button, evas_object_del);
     }
}

EOLIAN static Eina_Bool
_elm_fileselector_buttons_ok_cancel_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->ok_button ? EINA_TRUE : EINA_FALSE;
}

EAPI void
elm_fileselector_expandable_set(Evas_Object *obj,
                                Eina_Bool expand)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_expandable_set(expand));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_expandable_set(Eo *obj, Elm_Fileselector_Data *sd, Eina_Bool expand)
{
   sd->expand = !!expand;

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }
}

EAPI Eina_Bool
elm_fileselector_expandable_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, EINA_FALSE);
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_expandable_get());
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_expandable_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->expand;
}

EAPI void
elm_fileselector_path_set(Evas_Object *obj,
                          const char *_path)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_path_set(_path));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_path_set(Eo *obj, Elm_Fileselector_Data *sd, const char *_path)
{
   char *path;

   path = ecore_file_realpath(_path);
   _schedule_populate(obj, sd, eina_stringshare_add(path), NULL);
   free(path);
}

EAPI const char *
elm_fileselector_path_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, NULL);
   const char *ret = NULL;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_path_get());
   return ret;
}

EOLIAN static const char*
_elm_fileselector_elm_interface_fileselector_path_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->path;
}

EAPI void
elm_fileselector_mode_set(Evas_Object *obj,
                          Elm_Fileselector_Mode mode)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_mode_set(mode));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_mode_set(Eo *obj, Elm_Fileselector_Data *sd, Elm_Fileselector_Mode mode)
{
   Evas_Object *old;

   if (mode == sd->mode) return;

   old = elm_layout_content_unset(obj, "elm.swallow.files");

   if (mode == ELM_FILESELECTOR_LIST)
     {
        sd->files_view = _files_list_add(obj);
        if (sd->multi)
          elm_genlist_multi_select_set(sd->files_view, EINA_TRUE);
     }
   else
     {
        sd->files_view = _files_grid_add(obj);
        if (sd->multi)
          elm_gengrid_multi_select_set(sd->files_view, EINA_TRUE);
     }

   elm_layout_content_set(obj, "elm.swallow.files", sd->files_view);

   evas_object_del(old);

   sd->mode = mode;

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }
}

EAPI Elm_Fileselector_Mode
elm_fileselector_mode_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, ELM_FILESELECTOR_LAST);
   Elm_Fileselector_Mode ret = ELM_FILESELECTOR_LAST;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_mode_get());
   return ret;
}

EOLIAN static Elm_Fileselector_Mode
_elm_fileselector_elm_interface_fileselector_mode_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->mode;
}

EAPI void
elm_fileselector_multi_select_set(Evas_Object *obj, Eina_Bool multi)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_multi_select_set(multi));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_multi_select_set(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, Eina_Bool multi)
{
   char *path;

   multi = !!multi;
   if (sd->multi == multi) return;
   sd->multi = multi;

   if (sd->mode == ELM_FILESELECTOR_LIST)
     elm_genlist_multi_select_set(sd->files_view, multi);
   else
     elm_gengrid_multi_select_set(sd->files_view, multi);

   if (!sd->multi)
     {
        _clear_selections(sd, NULL);

        EINA_LIST_FREE(sd->paths, path)
          free(path);
     }
   else
     {
        const Eina_List *selected_items, *li;
        const Elm_Object_Item *it;

        if (sd->mode == ELM_FILESELECTOR_LIST)
          selected_items = elm_genlist_selected_items_get(sd->files_view);
        else
          selected_items = elm_gengrid_selected_items_get(sd->files_view);

        EINA_LIST_FOREACH(selected_items, li, it)
          {
             path = elm_object_item_data_get(it);
             sd->paths = eina_list_append(sd->paths, strdup(path));
          }
     }
}

EAPI Eina_Bool
elm_fileselector_multi_select_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, EINA_FALSE);
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_multi_select_get());
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_multi_select_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->multi;
}

EAPI const char *
elm_fileselector_selected_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, NULL);
   const char *ret = NULL;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_selected_get());
   return ret;
}

EOLIAN static const char*
_elm_fileselector_elm_interface_fileselector_selected_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   const char *fp;
   if (!sd->path) return NULL;

   fp = elm_object_text_get(sd->path_entry);
   if (ecore_file_exists(fp))
     {
        eina_stringshare_replace(&sd->selection, fp);

        return sd->selection;
     }

   if (sd->mode == ELM_FILESELECTOR_LIST)
     {
        Elm_Object_Item *gl_it = elm_genlist_selected_item_get(sd->files_view);

        if (gl_it) return elm_object_item_data_get(gl_it);
     }
   else
     {
        Elm_Object_Item *gg_it = elm_gengrid_selected_item_get(sd->files_view);

        if (gg_it) return elm_object_item_data_get(gg_it);
     }


   return sd->path;
}

EAPI Eina_Bool
elm_fileselector_selected_set(Evas_Object *obj,
                              const char *_path)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, EINA_FALSE);
   Eina_Bool ret = EINA_FALSE;
   eo_do(obj, ret = elm_interface_fileselector_selected_set(_path));
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_selected_set(Eo *obj, Elm_Fileselector_Data *sd, const char *_path)
{
   Eina_Bool ret = EINA_TRUE;
   char *dir;
   char *path;

   path = ecore_file_realpath(_path);

   if (ecore_file_is_dir(path))
     _schedule_populate(obj, sd, eina_stringshare_add(path), NULL);
   else
     {
        if (!ecore_file_exists(path))
          {
             ret = EINA_FALSE;
             goto clean_up;
          }

        dir = ecore_file_dir_get(path);
        eina_stringshare_replace(&sd->selection, path);
        eina_stringshare_ref(sd->selection);
        _schedule_populate(obj, sd, eina_stringshare_add(dir), sd->selection);
        free(dir);
     }

clean_up:
   free(path);

   return ret;
}

EAPI const Eina_List *
elm_fileselector_selected_paths_get(const Evas_Object* obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, NULL);
   const Eina_List *ret = NULL;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_selected_paths_get());
   return ret;
}

EOLIAN static const Eina_List*
_elm_fileselector_elm_interface_fileselector_selected_paths_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   if (sd->multi)
     return sd->paths;
   else
     return NULL;
}

EAPI const char *
elm_fileselector_current_name_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, NULL);
   const char *ret = NULL;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_current_name_get());
   return ret;
}

EOLIAN static const char *
_elm_fileselector_elm_interface_fileselector_current_name_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return elm_object_text_get(sd->name_entry);
}

EAPI void
elm_fileselector_current_name_set(Evas_Object *obj,
                                  const char *name)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do((Eo *) obj, elm_interface_fileselector_current_name_set(name));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_current_name_set(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, const char *name)
{
   elm_object_text_set(sd->name_entry, name);
}

static Elm_Fileselector_Filter *
_filter_add(Elm_Fileselector_Data *sd, const char *filter_name)
{
   Elm_Fileselector_Filter *ff;
   ff = malloc(sizeof(Elm_Fileselector_Filter));
   if (!ff) return NULL;

   ff->filter_name = eina_stringshare_add(filter_name);
   ff->sd = sd;

   return ff;
}

EAPI Eina_Bool
elm_fileselector_mime_types_filter_append(Evas_Object *obj, const char *mime_type, const char *filter_name)
{
   ELM_FILESELECTOR_CHECK(obj) EINA_FALSE;
   Eina_Bool ret = EINA_FALSE;
   eo_do(obj, ret = elm_interface_fileselector_mime_types_filter_append(mime_type, filter_name));
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_mime_types_filter_append(Eo *obj, Elm_Fileselector_Data *sd, const char *mime_types, const char *filter_name)
{
   Elm_Fileselector_Filter *ff;
   Eina_Bool need_theme = EINA_FALSE;

   if (!mime_types) return EINA_FALSE;

   ff = _filter_add(sd, filter_name ? filter_name : mime_types);
   if (!ff) return EINA_FALSE;

   ff->filter_type = ELM_FILESELECTOR_MIME_FILTER;

   ff->filter.mime_types = eina_str_split(mime_types, ",", 0);

   if (!sd->filter_list)
     {
        sd->current_filter = ff;
        sd->filter_hoversel = elm_hoversel_add(obj);
        elm_object_text_set(sd->filter_hoversel, ff->filter_name);
        need_theme = EINA_TRUE;
     }
   elm_hoversel_item_add(sd->filter_hoversel, ff->filter_name, NULL, ELM_ICON_NONE, _current_filter_changed, ff);

   sd->filter_list = eina_list_append(sd->filter_list, ff);

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }

   if (need_theme)
     eo_do(obj, elm_obj_widget_theme_apply());

   return EINA_TRUE;
}

EAPI Eina_Bool
elm_fileselector_custom_filter_append(Evas_Object *obj, Elm_Fileselector_Filter_Func func, void *data, const char *filter_name)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, EINA_FALSE);
   Eina_Bool ret = EINA_FALSE;
   eo_do(obj, ret = elm_interface_fileselector_custom_filter_append(func, data, filter_name));
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_custom_filter_append(Eo *obj, Elm_Fileselector_Data *sd, Elm_Fileselector_Filter_Func func, void *data, const char *filter_name)
{
   Elm_Fileselector_Filter *ff;
   Elm_Fileselector_Custom_Filter *custom_filter;
   Eina_Bool need_theme = EINA_FALSE;

   if (!func) return EINA_FALSE;

   custom_filter = malloc(sizeof(Elm_Fileselector_Custom_Filter));
   if (!custom_filter) return EINA_FALSE;

   ff = _filter_add(sd, filter_name ? filter_name : "custom");
   if (!ff)
     {
        free(custom_filter);
        return EINA_FALSE;
     }

   ff->filter_type = ELM_FILESELECTOR_CUSTOM_FILTER;
   ff->filter.custom = custom_filter;
   ff->filter.custom->func = func;
   ff->filter.custom->data = data;

   if (!sd->filter_list)
     {
        sd->current_filter = ff;
        sd->filter_hoversel = elm_hoversel_add(obj);
        elm_object_text_set(sd->filter_hoversel, ff->filter_name);
        need_theme = EINA_TRUE;
     }
   elm_hoversel_item_add(sd->filter_hoversel, ff->filter_name, NULL, ELM_ICON_NONE, _current_filter_changed, ff);

   sd->filter_list = eina_list_append(sd->filter_list, ff);

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }

   if (need_theme)
     eo_do(obj, elm_obj_widget_theme_apply());

   return EINA_TRUE;
}

EAPI void
elm_fileselector_filters_clear(Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_filters_clear());
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_filters_clear(Eo *obj, Elm_Fileselector_Data *sd)
{
   Elm_Fileselector_Filter *filter;

   EINA_LIST_FREE(sd->filter_list, filter)
     {
        eina_stringshare_del(filter->filter_name);

        if (filter->filter_type == ELM_FILESELECTOR_MIME_FILTER)
          {
             free(filter->filter.mime_types[0]);
             free(filter->filter.mime_types);
          }
        else
          free(filter->filter.custom);

        free(filter);
     }

   ELM_SAFE_FREE(sd->filter_hoversel, evas_object_del);

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }
}

EAPI void
elm_fileselector_hidden_visible_set(Evas_Object *obj, Eina_Bool visible)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_hidden_visible_set(visible));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_hidden_visible_set(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, Eina_Bool visible)
{
   visible = !!visible;
   if (sd->hidden_visible == visible) return;
   sd->hidden_visible = visible;

   _clear_selections(sd, NULL);

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }
}

EAPI Eina_Bool
elm_fileselector_hidden_visible_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, EINA_FALSE);
   Eina_Bool ret = EINA_FALSE;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_hidden_visible_get());
   return ret;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_interface_fileselector_hidden_visible_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->hidden_visible;
}

EAPI void
elm_fileselector_thumbnail_size_set(Evas_Object *obj,
                                    Evas_Coord w,
                                    Evas_Coord h)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_thumbnail_size_set(w, h));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_thumbnail_size_set(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, Evas_Coord w, Evas_Coord h)
{
   if (sd->thumbnail_size.w == w && sd->thumbnail_size.h == h) return;

   if (!w || !h)
     w = h = elm_config_finger_size_get() * 2 - GENGRID_PADDING;

   sd->thumbnail_size.w = w;
   sd->thumbnail_size.h = h;

   if (sd->mode == ELM_FILESELECTOR_GRID)
     elm_gengrid_item_size_set(sd->files_view, w + GENGRID_PADDING, h + GENGRID_PADDING);

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }
}

EAPI void
elm_fileselector_thumbnail_size_get(const Evas_Object *obj,
                                    Evas_Coord *w,
                                    Evas_Coord *h)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do((Eo *) obj, elm_interface_fileselector_thumbnail_size_get(w, h));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_thumbnail_size_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, Evas_Coord *w, Evas_Coord *h)
{
   if (w) *w = sd->thumbnail_size.w;
   if (h) *h = sd->thumbnail_size.h;
}

EAPI void
elm_fileselector_sort_method_set(Evas_Object *obj, Elm_Fileselector_Sort sort)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj);
   eo_do(obj, elm_interface_fileselector_sort_method_set(sort));
}

EOLIAN static void
_elm_fileselector_elm_interface_fileselector_sort_method_set(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, Elm_Fileselector_Sort sort)
{
   if (sd->sort_type == sort) return;
   sd->sort_type = sort;

   switch (sd->sort_type)
     {
      case ELM_FILESELECTOR_SORT_BY_FILENAME_ASC:
         sd->sort_method = strcoll;
         break;
      case ELM_FILESELECTOR_SORT_BY_FILENAME_DESC:
         sd->sort_method = _strcoll_rev;
         break;
      case ELM_FILESELECTOR_SORT_BY_TYPE_ASC:
         sd->sort_method = _strcoll_type;
         break;
      case ELM_FILESELECTOR_SORT_BY_TYPE_DESC:
         sd->sort_method = _strcoll_type_rev;
         break;
      case ELM_FILESELECTOR_SORT_BY_SIZE_ASC:
         sd->sort_method = _size_cmp;
         break;
      case ELM_FILESELECTOR_SORT_BY_SIZE_DESC:
         sd->sort_method = _size_cmp_rev;
         break;
      case ELM_FILESELECTOR_SORT_BY_MODIFIED_ASC:
         sd->sort_method = _modified_cmp;
         break;
      case ELM_FILESELECTOR_SORT_BY_MODIFIED_DESC:
         sd->sort_method = _modified_cmp_rev;
         break;
      case ELM_FILESELECTOR_SORT_LAST:
      default:
         sd->sort_method = strcoll;
     }

   if (sd->path)
     {
        eina_stringshare_ref(sd->path);
        _schedule_populate(obj, sd, sd->path, NULL);
     }
}

EAPI Elm_Fileselector_Sort
elm_fileselector_sort_method_get(const Evas_Object *obj)
{
   ELM_FILESELECTOR_INTERFACE_CHECK(obj, ELM_FILESELECTOR_SORT_LAST);
   Elm_Fileselector_Sort ret = ELM_FILESELECTOR_SORT_LAST;
   eo_do((Eo *) obj, ret = elm_interface_fileselector_sort_method_get());

   return ret;
}

EOLIAN static Elm_Fileselector_Sort
_elm_fileselector_elm_interface_fileselector_sort_method_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd)
{
   return sd->sort_type;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_widget_focus_next_manager_is(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd EINA_UNUSED)
{
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_widget_focus_next(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, Elm_Focus_Direction dir, Evas_Object **next, Elm_Object_Item **next_item)
{
   Eina_List *items = NULL;

   if (sd->up_button) items = eina_list_append(items, sd->up_button);
   if (sd->home_button) items = eina_list_append(items, sd->home_button);
   if (sd->files_view) items = eina_list_append(items, sd->files_view);
   if (sd->path_entry) items = eina_list_append(items, sd->path_entry);
   if (sd->name_entry) items = eina_list_append(items, sd->name_entry);
   if (sd->cancel_button) items = eina_list_append(items, sd->cancel_button);
   if (sd->ok_button) items = eina_list_append(items, sd->ok_button);  

   if (_elm_config->access_mode)
     return elm_widget_focus_list_next_get(obj, items, eina_list_data_get, dir, next, next_item);

   if (!elm_widget_focus_list_next_get(obj, items, eina_list_data_get, dir, next, next_item))
     *next = (Evas_Object *)obj;

   eina_list_free(items);

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_widget_focus_direction_manager_is(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd EINA_UNUSED)
{
   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_widget_focus_direction(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, const Evas_Object *base, double degree, Evas_Object **direction, Elm_Object_Item **direction_item, double *weight)
{
   Eina_List *items = NULL;

   if (sd->up_button) items = eina_list_append(items, sd->up_button);
   if (sd->home_button) items = eina_list_append(items, sd->home_button);
   if (sd->files_view) items = eina_list_append(items, sd->files_view);
   if (sd->path_entry) items = eina_list_append(items, sd->path_entry);
   if (sd->name_entry) items = eina_list_append(items, sd->name_entry);
   if (sd->cancel_button) items = eina_list_append(items, sd->cancel_button);
   if (sd->ok_button) items = eina_list_append(items, sd->ok_button);

   elm_widget_focus_list_direction_get
     (obj, base, items, eina_list_data_get, degree, direction, direction_item, weight);

   eina_list_free(items);

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_elm_fileselector_elm_layout_text_set(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *sd, const char *part, const char *label)
{
   if (!part) return EINA_FALSE;

   if (sd->ok_button && !strcmp(part, "ok"))
     {
        elm_object_text_set(sd->ok_button, label);
        return EINA_TRUE;
     }
   else if (sd->cancel_button && !strcmp(part, "cancel"))
     {
        elm_object_text_set(sd->cancel_button, label);
        return EINA_TRUE;
     }
   else
     {
        Eina_Bool int_ret = EINA_FALSE;
        eo_do_super(obj, MY_CLASS, int_ret = elm_obj_layout_text_set(part, label));
        return int_ret;
     }

   return EINA_FALSE;
}

EOLIAN static void
_elm_fileselector_class_constructor(Eo_Class *klass)
{
   unsigned int i;

   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);

   for (i = 0; i < ELM_FILE_LAST; ++i)
     {
        list_itc[i] = elm_genlist_item_class_new();
        grid_itc[i] = elm_gengrid_item_class_new();

        list_itc[i]->item_style = "default";
        list_itc[i]->func.text_get = grid_itc[i]->func.text_get =
            _itc_text_get;
        list_itc[i]->func.state_get = grid_itc[i]->func.state_get =
            _itc_state_get;
        list_itc[i]->func.del = grid_itc[i]->func.del = _itc_del;
     }

   list_itc[ELM_DIRECTORY]->func.content_get =
     grid_itc[ELM_DIRECTORY]->func.content_get = _itc_icon_folder_get;
   list_itc[ELM_FILE_IMAGE]->func.content_get =
     grid_itc[ELM_FILE_IMAGE]->func.content_get = _itc_icon_image_get;
   list_itc[ELM_FILE_UNKNOW]->func.content_get =
     grid_itc[ELM_FILE_UNKNOW]->func.content_get = _itc_icon_file_get;

}

EOLIAN static void
_elm_fileselector_class_destructor(Eo_Class *klass EINA_UNUSED)
{
   unsigned int i;

   for (i = 0; i < ELM_FILE_LAST; ++i)
     {
        elm_genlist_item_class_free(list_itc[i]);
        elm_gengrid_item_class_free(grid_itc[i]);
     }
}

EOLIAN const Elm_Atspi_Action *
_elm_fileselector_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Elm_Fileselector_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "select", "select", NULL, _key_action_select },
          { "escape", "escape", NULL, _key_action_escape},
          { NULL, NULL, NULL, NULL}
   };
   return &atspi_actions[0];
}

#include "elm_fileselector.eo.c"
