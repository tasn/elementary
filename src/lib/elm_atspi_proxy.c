#ifdef HAVE_CONFIG_H
  #include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED

#include <Elementary.h>
#include "elm_widget.h"
#include "elm_priv.h"


static Eina_List *_socket_list;

typedef struct _Elm_Atspi_Proxy_Data Elm_Atspi_Proxy_Data;

struct _Elm_Atspi_Proxy_Data
{
   Elm_Atspi_Proxy_Type type;
   const char *bus;
   const char *path;
};

EOLIAN static void
_elm_atspi_proxy_eo_base_destructor(Eo *obj, Elm_Atspi_Proxy_Data *_pd)
{
   if (_pd->type == ELM_ATSPI_PROXY_TYPE_SOCKET)
      _socket_list = eina_list_remove(_socket_list, obj);

   if (_pd->bus) eina_stringshare_del(_pd->bus);
   if (_pd->path) eina_stringshare_del(_pd->path);

   eo_do_super(obj, ELM_ATSPI_PROXY_CLASS, eo_destructor());
}

EOLIAN static void
_elm_atspi_proxy_constructor(Eo *obj, Elm_Atspi_Proxy_Data *_pd, Elm_Atspi_Proxy_Type type)
{
   Eo *parent;

   _pd->type = type;
   eo_do(obj, parent = eo_parent_get());
   if (!parent || !eo_isa(parent, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN))
     {
        CRI("Elm_Atspi_Proxy parent (%s) must implement ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN", eo_class_name_get(eo_class_get(parent)));
        abort();
     }
   if (type == ELM_ATSPI_PROXY_TYPE_SOCKET)
      _socket_list = eina_list_append(_socket_list, obj);
}

EOLIAN Elm_Atspi_Proxy_Type
_elm_atspi_proxy_type_get(Eo *obj EINA_UNUSED, Elm_Atspi_Proxy_Data *_pd)
{
   return _pd->type;
}

EOLIAN void
_elm_atspi_proxy_address_get(Eo *obj EINA_UNUSED, Elm_Atspi_Proxy_Data *_pd, const char **bus, const char **path)
{
   if (bus) *bus = _pd->bus;
   if (path) *path = _pd->path;
}

EOLIAN void
_elm_atspi_proxy_address_set(Eo *obj EINA_UNUSED, Elm_Atspi_Proxy_Data *_pd, const char *bus, const char *path)
{
   if (bus) eina_stringshare_replace(&_pd->bus, bus);
   if (path) eina_stringshare_replace(&_pd->path, path);
}

EOLIAN Eina_List*
_elm_atspi_proxy_elm_interface_atspi_accessible_children_get(Eo *obj EINA_UNUSED, Elm_Atspi_Proxy_Data *_pd)
{
   Eina_List *ret = NULL;
   if (_pd->type == ELM_ATSPI_PROXY_TYPE_SOCKET)
     {
        Eo *parent;
        eo_do(obj, parent = eo_parent_get());
        ret = eina_list_append(ret, parent);
     }
   return ret;
}

EOLIAN Eo*
_elm_atspi_proxy_elm_interface_atspi_accessible_parent_get(Eo *obj EINA_UNUSED, Elm_Atspi_Proxy_Data *_pd)
{
   Eo *ret = NULL;
   if (_pd->type == ELM_ATSPI_PROXY_TYPE_PLUG)
     {
        eo_do(obj, ret = eo_parent_get());
     }
   return ret;
}

Eina_List*
_elm_atspi_proxy_socket_list_get(void)
{
   return eina_list_clone(_socket_list);
}

#include "elm_atspi_proxy.eo.c"
