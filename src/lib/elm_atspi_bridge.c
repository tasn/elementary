#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_COMPONENT_PROTECTED
#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_ACTION_PROTECTED
#define ELM_INTERFACE_ATSPI_VALUE_PROTECTED
#define ELM_INTERFACE_ATSPI_IMAGE_PROTECTED
#define ELM_INTERFACE_ATSPI_SELECTION_PROTECTED
#define ELM_INTERFACE_ATSPI_TEXT_PROTECTED
#define ELM_INTERFACE_ATSPI_EDITABLE_TEXT_PROTECTED

#include "atspi/atspi-constants.h"

#include <stdint.h>
#include <Elementary.h>
#include "elm_priv.h"

/*
 * Accessibility Bus info not defined in atspi-constants.h
 */
#define A11Y_DBUS_NAME "org.a11y.Bus"
#define A11Y_DBUS_PATH "/org/a11y/bus"
#define A11Y_DBUS_INTERFACE "org.a11y.Bus"
#define A11Y_DBUS_STATUS_INTERFACE "org.a11y.Status"
#define ATSPI_DBUS_INTERFACE_EVENT_WINDOW "org.a11y.atspi.Event.Window"

#define CACHE_ITEM_SIGNATURE "((so)(so)(so)a(so)assusau)"
#define CACHE_INTERFACE_PATH "/org/a11y/atspi/cache"

#define ELM_ACCESS_OBJECT_PATH_ROOT "root"
#define ELM_ACCESS_OBJECT_PATH_PREFIX  "/org/a11y/atspi/accessible/"
#define ELM_ACCESS_OBJECT_PATH_PREFIX2  "/org/a11y/atspi/accessible"
#define ELM_ACCESS_OBJECT_REFERENCE_TEMPLATE ELM_ACCESS_OBJECT_PATH_PREFIX "%llu"

#define SIZE(x) sizeof(x)/sizeof(x[0])
#define ELM_ATSPI_BRIDGE_CLASS_NAME "__Elm_Atspi_Bridge"

#define ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(obj, sd) \
   Elm_Atspi_Bridge_Data *sd = eo_data_scope_get(obj, ELM_ATSPI_BRIDGE_CLASS); \
   if (!sd) return;

#define ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(obj, sd, val) \
   Elm_Atspi_Bridge_Data *sd = eo_data_scope_get(obj, ELM_ATSPI_BRIDGE_CLASS); \
   if (!sd) return val;

#define ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, class, msg, error) \
   if (!(obj) || !eo_isa(obj, class)) \
     { \
        *(error) = _dbus_invalid_ref_error_new(msg); \
        return EINA_FALSE; \
     }

#define ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, class, msg) \
   if (!(obj) || !eo_isa(obj, class)) \
     return _dbus_invalid_ref_error_new(msg);

typedef struct Key_Event_Info {
     Ecore_Event_Key event;
     int type;
     Eo *bridge;
} Key_Event_Info;

typedef struct _Elm_Atspi_Bridge_Data
{
   Eldbus_Connection *session_bus;
   Eldbus_Connection *a11y_bus;
   Eina_List *reemited_events;
   Eo *root;
   Eina_Hash *cache;
   Eldbus_Service_Interface *cache_interface;
   Eldbus_Signal_Handler *register_hdl;
   Eldbus_Signal_Handler *unregister_hdl;
   unsigned long object_broadcast_mask;
   unsigned long object_property_broadcast_mask;
   unsigned long object_children_broadcast_mask;
   unsigned long long object_state_broadcast_mask;
   unsigned long long window_signal_broadcast_mask;
   Ecore_Event_Filter *key_flr;
   Eldbus_Object *bus_obj;
   Eina_List *pending_requests;
   int id;
   Eina_Hash *state_hash;
   struct {
        Eldbus_Service_Interface *accessible;
        Eldbus_Service_Interface *application;
        Eldbus_Service_Interface *action;
        Eldbus_Service_Interface *component;
        Eldbus_Service_Interface *collection;
        Eldbus_Service_Interface *editable_text;
        Eldbus_Service_Interface *image;
        Eldbus_Service_Interface *selection;
        Eldbus_Service_Interface *text;
        Eldbus_Service_Interface *value;
   } interfaces;
   Elm_Atspi_Event_Handler *event_hdlr;
   Eina_Hash *event_hash;
   Eina_Bool connected : 1;
} Elm_Atspi_Bridge_Data;


struct collection_match_rule {
     Elm_Atspi_State_Set states;
     AtspiCollectionMatchType statematchtype;
     Eina_List *attributes;
     AtspiCollectionMatchType attributematchtype;
     uint64_t roles[2];
     AtspiCollectionMatchType rolematchtype;
     Eina_List *ifaces;
     AtspiCollectionMatchType interfacematchtype;
     Eina_Bool reverse : 1;
};

static Eo *_instance;
static int _init_count = 0;

// Object Event handlers
static Eina_Bool _state_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _property_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info);
static Eina_Bool _children_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info);
static Eina_Bool _window_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _visible_data_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _active_descendant_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _selection_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_text_inserted_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_text_removed_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_caret_moved_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_selection_changed_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info EINA_UNUSED);

// bridge private methods
static void _bridge_object_register(Eo *bridge, Eo *obj);
static void _bridge_object_unregister(Eo *bridge, Eo *obj);
static const char * _bridge_path_from_object(Eo *bridge, const Eo *eo);
static void _bridge_signal_send(Eo *bridge, Eo *obj, const char *ifc, const Eldbus_Signal *signal, const char *minor, unsigned int det1, unsigned int det2, const char *variant_sig, ...);
static Eo * _bridge_object_from_path(Eo *bridge, const char *path);
static void _bridge_iter_object_reference_append(Eo *bridge, Eldbus_Message_Iter *iter, const Eo *obj);

// utility functions
static void _iter_interfaces_append(Eldbus_Message_Iter *iter, const Eo *obj);
static Eina_Bool _elm_atspi_bridge_key_filter(void *data, void *loop, int type, void *event);
static void _object_desktop_reference_append(Eldbus_Message_Iter *iter);
static Eina_Bool _on_object_add(void *data, Eo *obj, const Eo_Event_Description *event EINA_UNUSED, void *event_info EINA_UNUSED);
static Eina_Bool _on_object_del(void *data, Eo *obj, const Eo_Event_Description *event EINA_UNUSED, void *event_info EINA_UNUSED);

typedef struct {
     const Eo_Event_Description *desc;
     const Eo_Event_Cb callback;
} Elm_Atspi_Bridge_Event_Handler;

static const Elm_Atspi_Bridge_Event_Handler event_handlers[] = {
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_CHILDREN_CHANGED, _children_changed_signal_send},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_PROPERTY_CHANGED, _property_changed_signal_send},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_STATE_CHANGED, _state_changed_signal_send},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_VISIBLE_DATA_CHANGED, _visible_data_changed_signal_send},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_ACTIVE_DESCENDANT_CHANGED, _active_descendant_changed_signal_send},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_ADDED, _on_object_add},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_REMOVED, _on_object_del},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_CREATED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_DESTROYED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_ACTIVATED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_DEACTIVATED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_MAXIMIZED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_MINIMIZED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_RESTORED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_SELECTION_EVENT_SELECTION_CHANGED, _selection_signal_send},
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_CARET_MOVED, _text_caret_moved_send },
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_INSERTED, _text_text_inserted_send },
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_REMOVED, _text_text_removed_send },
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_SELECTION_CHANGED, _text_selection_changed_send }
};

enum _Atspi_Object_Child_Event_Type
{
   ATSPI_OBJECT_CHILD_ADDED = 0,
   ATSPI_OBJECT_CHILD_REMOVED
};

enum _Atspi_Object_Property
{
   ATSPI_OBJECT_PROPERTY_NAME = 0,
   ATSPI_OBJECT_PROPERTY_DESCRIPTION,
   ATSPI_OBJECT_PROPERTY_VALUE,
   ATSPI_OBJECT_PROPERTY_ROLE,
   ATSPI_OBJECT_PROPERTY_PARENT,
   ATSPI_OBJECT_PROPERTY_LAST
};

enum _Atspi_Object_Signals {
   ATSPI_OBJECT_EVENT_PROPERTY_CHANGED = 0,
   ATSPI_OBJECT_EVENT_BOUNDS_CHANGED,
   ATSPI_OBJECT_EVENT_LINK_SELECTED,
   ATSPI_OBJECT_EVENT_STATE_CHANGED,
   ATSPI_OBJECT_EVENT_CHILDREN_CHANGED,
   ATSPI_OBJECT_EVENT_VISIBLE_DATA_CHANGED,
   ATSPI_OBJECT_EVENT_SELECTION_CHANGED,
   ATSPI_OBJECT_EVENT_MODEL_CHANGED,
   ATSPI_OBJECT_EVENT_ACTIVE_DESCENDANT_CHANGED,
   ATSPI_OBJECT_EVENT_ROW_INSERTED,
   ATSPI_OBJECT_EVENT_ROW_REORDERED,
   ATSPI_OBJECT_EVENT_ROW_DELETED,
   ATSPI_OBJECT_EVENT_COLUMN_INSERTED,
   ATSPI_OBJECT_EVENT_COLUMN_REORDERED,
   ATSPI_OBJECT_EVENT_COLUMN_DELETED,
   ATSPI_OBJECT_EVENT_TEXT_BOUNDS_CHANGED,
   ATSPI_OBJECT_EVENT_TEXT_SELECTION_CHANGED,
   ATSPI_OBJECT_EVENT_TEXT_CHANGED,
   ATSPI_OBJECT_EVENT_TEXT_ATTRIBUTES_CHANGED,
   ATSPI_OBJECT_EVENT_TEXT_CARET_MOVED,
   ATSPI_OBJECT_EVENT_ATTRIBUTES_CHANGED,
};

enum _Atspi_Window_Signals
{
   ATSPI_WINDOW_EVENT_PROPERTY_CHANGE = 0,
   ATSPI_WINDOW_EVENT_MINIMIZE,
   ATSPI_WINDOW_EVENT_MAXIMIZE,
   ATSPI_WINDOW_EVENT_RESTORE,
   ATSPI_WINDOW_EVENT_CLOSE,
   ATSPI_WINDOW_EVENT_CREATE,
   ATSPI_WINDOW_EVENT_REPARENT,
   ATSPI_WINDOW_EVENT_DESKTOPCREATE,
   ATSPI_WINDOW_EVENT_DESKTOPDESTROY,
   ATSPI_WINDOW_EVENT_DESTROY,
   ATSPI_WINDOW_EVENT_ACTIVATE,
   ATSPI_WINDOW_EVENT_DEACTIVATE,
   ATSPI_WINDOW_EVENT_RAISE,
   ATSPI_WINDOW_EVENT_LOWER,
   ATSPI_WINDOW_EVENT_MOVE,
   ATSPI_WINDOW_EVENT_RESIZE,
   ATSPI_WINDOW_EVENT_SHADE,
   ATSPI_WINDOW_EVENT_UUSHADE,
   ATSPI_WINDOW_EVENT_RESTYLE,
};

static const Eldbus_Signal _event_obj_signals[] = {
   [ATSPI_OBJECT_EVENT_PROPERTY_CHANGED] = {"PropertyChange", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_BOUNDS_CHANGED] = {"BoundsChange", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_LINK_SELECTED] = {"LinkSelected", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_STATE_CHANGED] = {"StateChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_CHILDREN_CHANGED] = {"ChildrenChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_VISIBLE_DATA_CHANGED] = {"VisibleDataChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_SELECTION_CHANGED] = {"SelectionChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_MODEL_CHANGED] = {"ModelChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_ACTIVE_DESCENDANT_CHANGED] = {"ActiveDescendantChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_ROW_INSERTED] = {"RowInserted", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_ROW_REORDERED] = {"RowReordered", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_ROW_DELETED] = {"RowDeleted", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_COLUMN_INSERTED] = {"ColumnInserted", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_COLUMN_REORDERED] = {"ColumnReordered", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_COLUMN_DELETED] = {"ColumnDeleted", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_TEXT_BOUNDS_CHANGED] = {"TextBoundsChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_TEXT_SELECTION_CHANGED] = {"TextSelectionChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_TEXT_CHANGED] = {"TextChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_TEXT_ATTRIBUTES_CHANGED] = {"TextAttributesChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_TEXT_CARET_MOVED] = {"TextCaretMoved", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_OBJECT_EVENT_ATTRIBUTES_CHANGED] = {"AttributesChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   {NULL, ELDBUS_ARGS({NULL, NULL}), 0}
};

static const Eldbus_Signal _window_obj_signals[] = {
   [ATSPI_WINDOW_EVENT_PROPERTY_CHANGE] = {"PropertyChange", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_MINIMIZE] = {"Minimize", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_MAXIMIZE] = {"Maximize", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_RESTORE] = {"Restore", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_CLOSE] = {"Close", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_CREATE] = {"Create", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_REPARENT] = {"Reparent", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_DESKTOPCREATE] = {"DesktopCreate", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_DESKTOPDESTROY] = {"DesktopDestroy", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_DESTROY] = {"Destroy", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_ACTIVATE] = {"Activate", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_DEACTIVATE] = {"Deactivate", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_RAISE] = {"Raise", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_LOWER] = {"Lower", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_MOVE] = {"Move", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_RESIZE] = {"Resize", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_SHADE] = {"Shade", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_UUSHADE] = {"uUshade", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   [ATSPI_WINDOW_EVENT_RESTYLE] = {"Restyle", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
   {NULL, ELDBUS_ARGS({NULL, NULL}), 0}
};

const int elm_roles_to_atspi_roles[][2] = {
   { ELM_ATSPI_ROLE_INVALID, ATSPI_ROLE_INVALID },
   { ELM_ATSPI_ROLE_ACCELERATOR_LABEL, ATSPI_ROLE_ACCELERATOR_LABEL },
   { ELM_ATSPI_ROLE_ALERT, ATSPI_ROLE_ALERT },
   { ELM_ATSPI_ROLE_ANIMATION, ATSPI_ROLE_ANIMATION },
   { ELM_ATSPI_ROLE_ARROW, ATSPI_ROLE_ARROW },
   { ELM_ATSPI_ROLE_CALENDAR, ATSPI_ROLE_CALENDAR },
   { ELM_ATSPI_ROLE_CANVAS, ATSPI_ROLE_CANVAS },
   { ELM_ATSPI_ROLE_CHECK_BOX, ATSPI_ROLE_CHECK_BOX },
   { ELM_ATSPI_ROLE_CHECK_MENU_ITEM, ATSPI_ROLE_CHECK_MENU_ITEM },
   { ELM_ATSPI_ROLE_COLOR_CHOOSER, ATSPI_ROLE_COLOR_CHOOSER },
   { ELM_ATSPI_ROLE_COLUMN_HEADER, ATSPI_ROLE_COLUMN_HEADER },
   { ELM_ATSPI_ROLE_COMBO_BOX, ATSPI_ROLE_COMBO_BOX },
   { ELM_ATSPI_ROLE_DATE_EDITOR, ATSPI_ROLE_DATE_EDITOR },
   { ELM_ATSPI_ROLE_DESKTOP_ICON, ATSPI_ROLE_DESKTOP_ICON },
   { ELM_ATSPI_ROLE_DESKTOP_FRAME, ATSPI_ROLE_DESKTOP_FRAME },
   { ELM_ATSPI_ROLE_DIAL, ATSPI_ROLE_DIAL },
   { ELM_ATSPI_ROLE_DIALOG, ATSPI_ROLE_DIALOG },
   { ELM_ATSPI_ROLE_DIRECTORY_PANE, ATSPI_ROLE_DIRECTORY_PANE },
   { ELM_ATSPI_ROLE_DRAWING_AREA, ATSPI_ROLE_DRAWING_AREA },
   { ELM_ATSPI_ROLE_FILE_CHOOSER, ATSPI_ROLE_FILE_CHOOSER },
   { ELM_ATSPI_ROLE_FILLER, ATSPI_ROLE_FILLER },
   { ELM_ATSPI_ROLE_FOCUS_TRAVERSABLE, ATSPI_ROLE_FOCUS_TRAVERSABLE },
   { ELM_ATSPI_ROLE_FONT_CHOOSER, ATSPI_ROLE_FONT_CHOOSER },
   { ELM_ATSPI_ROLE_FRAME, ATSPI_ROLE_FRAME },
   { ELM_ATSPI_ROLE_GLASS_PANE, ATSPI_ROLE_GLASS_PANE },
   { ELM_ATSPI_ROLE_HTML_CONTAINER, ATSPI_ROLE_HTML_CONTAINER },
   { ELM_ATSPI_ROLE_ICON, ATSPI_ROLE_ICON },
   { ELM_ATSPI_ROLE_IMAGE, ATSPI_ROLE_IMAGE },
   { ELM_ATSPI_ROLE_INTERNAL_FRAME, ATSPI_ROLE_INTERNAL_FRAME },
   { ELM_ATSPI_ROLE_LABEL, ATSPI_ROLE_LABEL },
   { ELM_ATSPI_ROLE_LAYERED_PANE, ATSPI_ROLE_LAYERED_PANE },
   { ELM_ATSPI_ROLE_LIST, ATSPI_ROLE_LIST },
   { ELM_ATSPI_ROLE_LIST_ITEM, ATSPI_ROLE_LIST_ITEM },
   { ELM_ATSPI_ROLE_MENU, ATSPI_ROLE_MENU },
   { ELM_ATSPI_ROLE_MENU_BAR, ATSPI_ROLE_MENU_BAR },
   { ELM_ATSPI_ROLE_MENU_ITEM, ATSPI_ROLE_MENU_ITEM },
   { ELM_ATSPI_ROLE_OPTION_PANE, ATSPI_ROLE_OPTION_PANE },
   { ELM_ATSPI_ROLE_PAGE_TAB, ATSPI_ROLE_PAGE_TAB },
   { ELM_ATSPI_ROLE_PAGE_TAB_LIST, ATSPI_ROLE_PAGE_TAB_LIST },
   { ELM_ATSPI_ROLE_PANEL, ATSPI_ROLE_PANEL },
   { ELM_ATSPI_ROLE_PASSWORD_TEXT, ATSPI_ROLE_PASSWORD_TEXT },
   { ELM_ATSPI_ROLE_POPUP_MENU, ATSPI_ROLE_POPUP_MENU },
   { ELM_ATSPI_ROLE_PROGRESS_BAR, ATSPI_ROLE_PROGRESS_BAR },
   { ELM_ATSPI_ROLE_PUSH_BUTTON, ATSPI_ROLE_PUSH_BUTTON },
   { ELM_ATSPI_ROLE_RADIO_BUTTON, ATSPI_ROLE_RADIO_BUTTON },
   { ELM_ATSPI_ROLE_RADIO_MENU_ITEM, ATSPI_ROLE_RADIO_MENU_ITEM },
   { ELM_ATSPI_ROLE_ROOT_PANE, ATSPI_ROLE_ROOT_PANE },
   { ELM_ATSPI_ROLE_ROW_HEADER, ATSPI_ROLE_ROW_HEADER },
   { ELM_ATSPI_ROLE_SCROLL_BAR, ATSPI_ROLE_SCROLL_BAR },
   { ELM_ATSPI_ROLE_SCROLL_PANE, ATSPI_ROLE_SCROLL_PANE },
   { ELM_ATSPI_ROLE_SEPARATOR, ATSPI_ROLE_SEPARATOR },
   { ELM_ATSPI_ROLE_SLIDER, ATSPI_ROLE_SLIDER },
   { ELM_ATSPI_ROLE_SPIN_BUTTON, ATSPI_ROLE_SPIN_BUTTON },
   { ELM_ATSPI_ROLE_SPLIT_PANE, ATSPI_ROLE_SPLIT_PANE },
   { ELM_ATSPI_ROLE_STATUS_BAR, ATSPI_ROLE_STATUS_BAR },
   { ELM_ATSPI_ROLE_TABLE, ATSPI_ROLE_TABLE },
   { ELM_ATSPI_ROLE_TABLE_CELL, ATSPI_ROLE_TABLE_CELL },
   { ELM_ATSPI_ROLE_TABLE_COLUMN_HEADER, ATSPI_ROLE_TABLE_COLUMN_HEADER },
   { ELM_ATSPI_ROLE_TABLE_ROW_HEADER, ATSPI_ROLE_TABLE_ROW_HEADER },
   { ELM_ATSPI_ROLE_TEAROFF_MENU_ITEM, ATSPI_ROLE_TEAROFF_MENU_ITEM },
   { ELM_ATSPI_ROLE_TERMINAL, ATSPI_ROLE_TERMINAL },
   { ELM_ATSPI_ROLE_TEXT, ATSPI_ROLE_TEXT },
   { ELM_ATSPI_ROLE_TOGGLE_BUTTON, ATSPI_ROLE_TOGGLE_BUTTON },
   { ELM_ATSPI_ROLE_TOOL_BAR, ATSPI_ROLE_TOOL_BAR },
   { ELM_ATSPI_ROLE_TOOL_TIP, ATSPI_ROLE_TOOL_TIP },
   { ELM_ATSPI_ROLE_TREE, ATSPI_ROLE_TREE },
   { ELM_ATSPI_ROLE_TREE_TABLE, ATSPI_ROLE_TREE_TABLE },
   { ELM_ATSPI_ROLE_UNKNOWN, ATSPI_ROLE_UNKNOWN },
   { ELM_ATSPI_ROLE_VIEWPORT, ATSPI_ROLE_VIEWPORT },
   { ELM_ATSPI_ROLE_WINDOW, ATSPI_ROLE_WINDOW },
   { ELM_ATSPI_ROLE_EXTENDED, ATSPI_ROLE_EXTENDED },
   { ELM_ATSPI_ROLE_HEADER, ATSPI_ROLE_HEADER },
   { ELM_ATSPI_ROLE_FOOTER, ATSPI_ROLE_FOOTER },
   { ELM_ATSPI_ROLE_PARAGRAPH, ATSPI_ROLE_PARAGRAPH },
   { ELM_ATSPI_ROLE_RULER, ATSPI_ROLE_RULER },
   { ELM_ATSPI_ROLE_APPLICATION, ATSPI_ROLE_APPLICATION },
   { ELM_ATSPI_ROLE_AUTOCOMPLETE, ATSPI_ROLE_AUTOCOMPLETE },
   { ELM_ATSPI_ROLE_EDITBAR, ATSPI_ROLE_EDITBAR },
   { ELM_ATSPI_ROLE_EMBEDDED, ATSPI_ROLE_EMBEDDED },
   { ELM_ATSPI_ROLE_ENTRY, ATSPI_ROLE_ENTRY },
   { ELM_ATSPI_ROLE_CHART, ATSPI_ROLE_CHART },
   { ELM_ATSPI_ROLE_CAPTION, ATSPI_ROLE_CAPTION },
   { ELM_ATSPI_ROLE_DOCUMENT_FRAME, ATSPI_ROLE_DOCUMENT_FRAME },
   { ELM_ATSPI_ROLE_HEADING, ATSPI_ROLE_HEADING },
   { ELM_ATSPI_ROLE_PAGE, ATSPI_ROLE_PAGE },
   { ELM_ATSPI_ROLE_SECTION, ATSPI_ROLE_SECTION },
   { ELM_ATSPI_ROLE_REDUNDANT_OBJECT, ATSPI_ROLE_REDUNDANT_OBJECT },
   { ELM_ATSPI_ROLE_FORM, ATSPI_ROLE_FORM },
   { ELM_ATSPI_ROLE_LINK, ATSPI_ROLE_LINK },
   { ELM_ATSPI_ROLE_INPUT_METHOD_WINDOW, ATSPI_ROLE_INPUT_METHOD_WINDOW },
   { ELM_ATSPI_ROLE_TABLE_ROW, ATSPI_ROLE_TABLE_ROW },
   { ELM_ATSPI_ROLE_TREE_ITEM, ATSPI_ROLE_TREE_ITEM },
   { ELM_ATSPI_ROLE_DOCUMENT_SPREADSHEET, ATSPI_ROLE_DOCUMENT_SPREADSHEET },
   { ELM_ATSPI_ROLE_DOCUMENT_PRESENTATION, ATSPI_ROLE_DOCUMENT_PRESENTATION },
   { ELM_ATSPI_ROLE_DOCUMENT_TEXT, ATSPI_ROLE_DOCUMENT_TEXT },
   { ELM_ATSPI_ROLE_DOCUMENT_WEB, ATSPI_ROLE_DOCUMENT_WEB },
   { ELM_ATSPI_ROLE_DOCUMENT_EMAIL, ATSPI_ROLE_DOCUMENT_EMAIL },
   { ELM_ATSPI_ROLE_COMMENT, ATSPI_ROLE_COMMENT },
   { ELM_ATSPI_ROLE_LIST_BOX, ATSPI_ROLE_LIST_BOX },
   { ELM_ATSPI_ROLE_GROUPING, ATSPI_ROLE_GROUPING },
   { ELM_ATSPI_ROLE_IMAGE_MAP, ATSPI_ROLE_IMAGE_MAP },
   { ELM_ATSPI_ROLE_NOTIFICATION, ATSPI_ROLE_NOTIFICATION },
   { ELM_ATSPI_ROLE_INFO_BAR, ATSPI_ROLE_INFO_BAR },
   { ELM_ATSPI_ROLE_LAST_DEFINED, ATSPI_ROLE_LAST_DEFINED },
};

struct atspi_state_desc
{
   Elm_Atspi_State_Type elm_state;
   AtspiStateType atspi_state;
   const char *name;
};

const struct atspi_state_desc elm_states_to_atspi_state[] = {
   { ELM_ATSPI_STATE_INVALID, ATSPI_STATE_INVALID, "invalid" },
   { ELM_ATSPI_STATE_ACTIVE, ATSPI_STATE_ACTIVE, "active" },
   { ELM_ATSPI_STATE_ARMED, ATSPI_STATE_ARMED, "armed" },
   { ELM_ATSPI_STATE_BUSY, ATSPI_STATE_BUSY, "busy" },
   { ELM_ATSPI_STATE_CHECKED, ATSPI_STATE_CHECKED, "checked" },
   { ELM_ATSPI_STATE_COLLAPSED, ATSPI_STATE_COLLAPSED, "collapsed" },
   { ELM_ATSPI_STATE_DEFUNCT, ATSPI_STATE_DEFUNCT, "defunct" },
   { ELM_ATSPI_STATE_EDITABLE, ATSPI_STATE_EDITABLE, "editable" },
   { ELM_ATSPI_STATE_ENABLED, ATSPI_STATE_ENABLED, "enabled" },
   { ELM_ATSPI_STATE_EXPANDABLE, ATSPI_STATE_EXPANDABLE, "expandable" },
   { ELM_ATSPI_STATE_EXPANDED, ATSPI_STATE_EXPANDED, "expanded" },
   { ELM_ATSPI_STATE_FOCUSABLE, ATSPI_STATE_FOCUSABLE, "focusable" },
   { ELM_ATSPI_STATE_FOCUSED, ATSPI_STATE_FOCUSED, "focused" },
   { ELM_ATSPI_STATE_HAS_TOOLTIP, ATSPI_STATE_HAS_TOOLTIP, "has-tooltip" },
   { ELM_ATSPI_STATE_HORIZONTAL, ATSPI_STATE_HORIZONTAL, "horizontal" },
   { ELM_ATSPI_STATE_ICONIFIED, ATSPI_STATE_ICONIFIED, "iconified" },
   { ELM_ATSPI_STATE_MODAL, ATSPI_STATE_MODAL, "modal" },
   { ELM_ATSPI_STATE_MULTI_LINE, ATSPI_STATE_MULTI_LINE, "multi-line" },
   { ELM_ATSPI_STATE_MULTISELECTABLE, ATSPI_STATE_MULTISELECTABLE, "multiselectable" },
   { ELM_ATSPI_STATE_OPAQUE, ATSPI_STATE_OPAQUE, "opaque" },
   { ELM_ATSPI_STATE_PRESSED, ATSPI_STATE_PRESSED, "pressed" },
   { ELM_ATSPI_STATE_RESIZABLE, ATSPI_STATE_RESIZABLE, "resizable" },
   { ELM_ATSPI_STATE_SELECTABLE, ATSPI_STATE_SELECTABLE, "selectable" },
   { ELM_ATSPI_STATE_SELECTED, ATSPI_STATE_SELECTED, "selected" },
   { ELM_ATSPI_STATE_SENSITIVE, ATSPI_STATE_SENSITIVE, "sensitive" },
   { ELM_ATSPI_STATE_SHOWING, ATSPI_STATE_SHOWING, "showing" },
   { ELM_ATSPI_STATE_SINGLE_LINE, ATSPI_STATE_SINGLE_LINE, "single-line" },
   { ELM_ATSPI_STATE_STALE, ATSPI_STATE_STALE, "stale" },
   { ELM_ATSPI_STATE_TRANSIENT, ATSPI_STATE_TRANSIENT, "transient" },
   { ELM_ATSPI_STATE_VERTICAL, ATSPI_STATE_VERTICAL, "vertical" },
   { ELM_ATSPI_STATE_VISIBLE, ATSPI_STATE_VISIBLE, "visible" },
   { ELM_ATSPI_STATE_MANAGES_DESCENDANTS, ATSPI_STATE_MANAGES_DESCENDANTS, "manages-descendants" },
   { ELM_ATSPI_STATE_INDETERMINATE, ATSPI_STATE_INDETERMINATE, "indeterminate" },
   { ELM_ATSPI_STATE_REQUIRED, ATSPI_STATE_REQUIRED, "required" },
   { ELM_ATSPI_STATE_TRUNCATED, ATSPI_STATE_TRUNCATED, "truncated" },
   { ELM_ATSPI_STATE_ANIMATED, ATSPI_STATE_ANIMATED, "animated" },
   { ELM_ATSPI_STATE_INVALID_ENTRY, ATSPI_STATE_INVALID_ENTRY, "invalid-entry" },
   { ELM_ATSPI_STATE_SUPPORTS_AUTOCOMPLETION, ATSPI_STATE_SUPPORTS_AUTOCOMPLETION, "supports-autocompletion" },
   { ELM_ATSPI_STATE_SELECTABLE_TEXT, ATSPI_STATE_SELECTABLE_TEXT, "selectable-text" },
   { ELM_ATSPI_STATE_IS_DEFAULT, ATSPI_STATE_IS_DEFAULT, "is-default" },
   { ELM_ATSPI_STATE_VISITED, ATSPI_STATE_VISITED, "visited" },
   { ELM_ATSPI_STATE_LAST_DEFINED, ATSPI_STATE_LAST_DEFINED, "last-defined" },
};

const int elm_relation_to_atspi_relation_mapping[] = {
   [ELM_ATSPI_RELATION_NULL] =  ATSPI_RELATION_NULL,
   [ELM_ATSPI_RELATION_LABEL_FOR] =  ATSPI_RELATION_LABEL_FOR,
   [ELM_ATSPI_RELATION_LABELLED_BY] = ATSPI_RELATION_LABELLED_BY,
   [ELM_ATSPI_RELATION_CONTROLLER_FOR] = ATSPI_RELATION_CONTROLLER_FOR,
   [ELM_ATSPI_RELATION_CONTROLLED_BY] = ATSPI_RELATION_CONTROLLED_BY,
   [ELM_ATSPI_RELATION_MEMBER_OF] = ATSPI_RELATION_MEMBER_OF,
   [ELM_ATSPI_RELATION_TOOLTIP_FOR] = ATSPI_RELATION_TOOLTIP_FOR,
   [ELM_ATSPI_RELATION_NODE_CHILD_OF] = ATSPI_RELATION_NODE_CHILD_OF,
   [ELM_ATSPI_RELATION_NODE_PARENT_OF] = ATSPI_RELATION_NODE_PARENT_OF,
   [ELM_ATSPI_RELATION_EXTENDED] = ATSPI_RELATION_EXTENDED,
   [ELM_ATSPI_RELATION_FLOWS_TO] = ATSPI_RELATION_FLOWS_TO,
   [ELM_ATSPI_RELATION_FLOWS_FROM] = ATSPI_RELATION_FLOWS_FROM,
   [ELM_ATSPI_RELATION_SUBWINDOW_OF] = ATSPI_RELATION_SUBWINDOW_OF,
   [ELM_ATSPI_RELATION_EMBEDS] = ATSPI_RELATION_EMBEDS,
   [ELM_ATSPI_RELATION_EMBEDDED_BY] = ATSPI_RELATION_EMBEDDED_BY,
   [ELM_ATSPI_RELATION_POPUP_FOR] = ATSPI_RELATION_POPUP_FOR,
   [ELM_ATSPI_RELATION_PARENT_WINDOW_OF] = ATSPI_RELATION_PARENT_WINDOW_OF,
   [ELM_ATSPI_RELATION_DESCRIPTION_FOR] = ATSPI_RELATION_DESCRIPTION_FOR,
   [ELM_ATSPI_RELATION_DESCRIBED_BY] = ATSPI_RELATION_DESCRIBED_BY,
   [ELM_ATSPI_RELATION_LAST_DEFINED] = ATSPI_RELATION_LAST_DEFINED,
};

static inline Eldbus_Message *_dbus_invalid_ref_error_new(const Eldbus_Message *msg)
{
  return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");
}

static AtspiRelationType _elm_relation_to_atspi_relation(Elm_Atspi_Relation_Type type)
{
   if ((type < ELM_ATSPI_RELATION_LAST_DEFINED) && (type > ELM_ATSPI_RELATION_NULL))
     return elm_relation_to_atspi_relation_mapping[type];
   return ATSPI_RELATION_NULL;
}

static Eldbus_Message *
_accessible_get_role(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   AtspiRole atspi_role = ATSPI_ROLE_INVALID;
   Elm_Atspi_Role role;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   eo_do(obj, role = elm_interface_atspi_accessible_role_get());

   Eldbus_Message *ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   atspi_role = role > ELM_ATSPI_ROLE_LAST_DEFINED ? ATSPI_ROLE_LAST_DEFINED : elm_roles_to_atspi_roles[role][1];
   eldbus_message_arguments_append(ret, "u", atspi_role);
   return ret;
}

static Eldbus_Message *
_accessible_get_role_name(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *role_name = NULL, *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   eo_do(obj, role_name = elm_interface_atspi_accessible_role_name_get());

   Eldbus_Message *ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);
   eldbus_message_arguments_append(ret, "s", role_name);

   return ret;
}

static Eldbus_Message *
_accessible_get_localized_role_name(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *l_role_name = NULL, *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   eo_do(obj, l_role_name = elm_interface_atspi_accessible_localized_role_name_get());
   EINA_SAFETY_ON_NULL_RETURN_VAL(l_role_name, NULL);

   Eldbus_Message *ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);
   eldbus_message_arguments_append(ret, "s", l_role_name);

   return ret;
}

static Eldbus_Message *
_accessible_get_children(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eina_List *children_list = NULL, *l;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eo *children;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   eo_do(obj, children_list = elm_interface_atspi_accessible_children_get());

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "(so)");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   EINA_LIST_FOREACH(children_list, l, children)
     {
        _bridge_iter_object_reference_append(bridge, iter_array, children);
        _bridge_object_register(bridge, children);
     }

   eldbus_message_iter_container_close(iter, iter_array);
   eina_list_free(children_list);

   return ret;

fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static Eldbus_Message *
_accessible_get_application(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   Eldbus_Message *ret;
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   Eldbus_Message_Iter *iter = eldbus_message_iter_get(ret);
   _bridge_iter_object_reference_append(bridge, iter, elm_atspi_bridge_root_get(bridge));

   return ret;
}

static Eldbus_Message *
_accessible_attributes_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   Eina_List *attrs, *l;
   Elm_Atspi_Attribute *attr;
   Eldbus_Message_Iter *iter, *iter_dict, *iter_entry;
   Eldbus_Message *ret;

   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, attrs = elm_interface_atspi_accessible_attributes_get());

   iter = eldbus_message_iter_get(ret);
   iter_dict = eldbus_message_iter_container_new(iter, 'a', "{ss}");
   EINA_SAFETY_ON_NULL_RETURN_VAL(iter_dict, NULL);

   EINA_LIST_FOREACH(attrs, l, attr)
     {
        iter_entry = eldbus_message_iter_container_new(iter_dict, 'e', NULL);
        EINA_SAFETY_ON_NULL_RETURN_VAL(iter_entry, NULL);
        eldbus_message_iter_arguments_append(iter_entry, "ss", attr->key, attr->value);
        eldbus_message_iter_container_close(iter_dict, iter_entry);
     }

   eldbus_message_iter_container_close(iter, iter_dict);
   elm_atspi_attributes_list_free(attrs);

   return ret;
}

static Eldbus_Message *
_accessible_interfaces_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter;
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   _iter_interfaces_append(iter, obj);

   return ret;
}

static uint64_t
_elm_atspi_state_set_to_atspi_state_set(Elm_Atspi_State_Set states)
{
   uint64_t ret = 0;
   unsigned int i = 0;

   for (i = 0; i < SIZE(elm_states_to_atspi_state); i++)
     {
        if (STATE_TYPE_GET(states, elm_states_to_atspi_state[i].elm_state))
          STATE_TYPE_SET(ret, elm_states_to_atspi_state[i].atspi_state);
     }
   return ret;
}

static Elm_Atspi_State_Set
_atspi_state_set_to_elm_atspi_state_set(uint64_t states)
{
   //Currently Elm_Atspi_State and Atspi_State_Set are binary compatible,
   //implement proper coversion when it will be needed.
   Elm_Atspi_State_Set ret = states;
   return ret;
}

static Eina_Hash*
_elm_atspi_state_hash_build(void)
{
   Eina_Hash *ret = eina_hash_string_superfast_new(NULL);
   unsigned int i = 0;

   for (i = 0; i < SIZE(elm_states_to_atspi_state); i++)
     eina_hash_add(ret, elm_states_to_atspi_state[i].name, &elm_states_to_atspi_state[i]);

   return ret;
}

static Eina_Hash*
_elm_atspi_event_hash_build(void)
{
   Eina_Hash *ret = eina_hash_pointer_new(NULL);
   unsigned int i = 0;

   for (i = 0; i < SIZE(event_handlers); i++)
     eina_hash_add(ret, &(event_handlers[i].desc), event_handlers[i].callback);

   return ret;
}

static Eldbus_Message *
_accessible_get_state(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Elm_Atspi_State_Set states;
   uint64_t atspi_states = 0;

   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "u");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   eo_do(obj, states = elm_interface_atspi_accessible_state_set_get());

   atspi_states = _elm_atspi_state_set_to_atspi_state_set(states);

   unsigned int s1 = atspi_states & 0xFFFFFFFF;
   unsigned int s2 = (atspi_states >> 32) & 0xFFFFFFFF;

   eldbus_message_iter_basic_append(iter_array, 'u', s1);
   eldbus_message_iter_basic_append(iter_array, 'u', s2);
   eldbus_message_iter_container_close(iter, iter_array);

   return ret;

fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static Eldbus_Message *
_accessible_get_index_in_parent(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int idx = -1;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, idx = elm_interface_atspi_accessible_index_in_parent_get());

   eldbus_message_arguments_append(ret, "i", idx);

   return ret;
}

static Eldbus_Message *
_accessible_child_at_index(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eo *child = NULL;
   Eina_List *children = NULL;
   int idx;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   eo_do(obj, children = elm_interface_atspi_accessible_children_get());

   child = eina_list_nth(children, idx);
   _bridge_iter_object_reference_append(bridge, iter, child);
   _bridge_object_register(bridge, child);
   eina_list_free(children);

   return ret;
}

static Eldbus_Message *
_accessible_get_relation_set(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *rel_obj, *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret = NULL;
   Eldbus_Message_Iter *iter = NULL, *iter_array = NULL, *iter_array2 = NULL, *iter_struct;
   Elm_Atspi_Relation *rel;
   Eina_List *l, *l2;
   Elm_Atspi_Relation_Set rels;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "(ua(so))");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   eo_do(obj, rels = elm_interface_atspi_accessible_relation_set_get());

   EINA_LIST_FOREACH(rels, l, rel)
     {
        iter_struct = eldbus_message_iter_container_new(iter_array, 'r', NULL);
        eldbus_message_iter_basic_append(iter_struct, 'u', _elm_relation_to_atspi_relation(rel->type));
        iter_array2 = eldbus_message_iter_container_new(iter_struct, 'a', "(so)");
        EINA_SAFETY_ON_NULL_GOTO(iter_array2, fail);
        EINA_LIST_FOREACH(rel->objects, l2, rel_obj)
          {
             _bridge_iter_object_reference_append(bridge, iter_array2, rel_obj);
             _bridge_object_register(bridge, rel_obj);
          }
        eldbus_message_iter_container_close(iter_struct, iter_array2);
        eldbus_message_iter_container_close(iter_array, iter_struct);
     }
   elm_atspi_relation_set_free(rels);
   eldbus_message_iter_container_close(iter, iter_array);

   return ret;

fail:
   return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Unable to get relation set.");
}

static const Eldbus_Method accessible_methods[] = {
   { "GetChildAtIndex", ELDBUS_ARGS({"i", "index"}), ELDBUS_ARGS({"(so)", "Accessible"}), _accessible_child_at_index, 0 },
   { "GetChildren", NULL, ELDBUS_ARGS({"a(so)", "children"}), _accessible_get_children, 0 },
   { "GetIndexInParent", NULL, ELDBUS_ARGS({"i", "index"}), _accessible_get_index_in_parent, 0 },
   { "GetRelationSet", NULL, ELDBUS_ARGS({"a(ua(so))", NULL}), _accessible_get_relation_set, 0 },
   { "GetRole", NULL, ELDBUS_ARGS({"u", "Role"}), _accessible_get_role, 0 },
   { "GetRoleName", NULL, ELDBUS_ARGS({"s", "Name"}), _accessible_get_role_name, 0 },
   { "GetLocalizedRoleName", NULL, ELDBUS_ARGS({"s", "LocalizedName"}), _accessible_get_localized_role_name, 0},
   { "GetState", NULL, ELDBUS_ARGS({"au", NULL}), _accessible_get_state, 0},
   { "GetApplication", NULL, ELDBUS_ARGS({"(so)", NULL}), _accessible_get_application, 0},
   { "GetAttributes", NULL, ELDBUS_ARGS({"a{ss}", NULL}), _accessible_attributes_get, 0},
   { "GetInterfaces", NULL, ELDBUS_ARGS({"as", NULL}), _accessible_interfaces_get, 0},
   { NULL, NULL, NULL, NULL, 0 }
};

static Eldbus_Message *
_selection_selected_child_get(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eo *child = NULL;

   int idx;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   eo_do(obj, child = elm_interface_atspi_selection_selected_child_get(idx));

   _bridge_iter_object_reference_append(bridge, iter, child);
   _bridge_object_register(bridge, child);

   return ret;
}

static Eldbus_Message *
_selection_child_select(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_child_select(idx));
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_selection_selected_child_deselect(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_selected_child_deselect(idx));
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_selection_is_child_selected(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_is_child_selected(idx));
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_selection_all_children_select(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eina_Bool result;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_all_children_select());
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_selection_clear(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eina_Bool result;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_clear());
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_selection_child_deselect(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_child_deselect(idx));
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static const Eldbus_Method selection_methods[] = {
   { "GetSelectedChild", ELDBUS_ARGS({"i", "selectedChildIndex"}), ELDBUS_ARGS({"(so)", "Accessible"}), _selection_selected_child_get, 0 },
   { "SelectChild", ELDBUS_ARGS({"i", "childIndex"}), ELDBUS_ARGS({"b", "result"}), _selection_child_select, 0 },
   { "DeselectSelectedChild", ELDBUS_ARGS({"i", "selectedChildIndex"}), ELDBUS_ARGS({"b", "result"}), _selection_selected_child_deselect, 0 },
   { "IsChildSelected", ELDBUS_ARGS({"i", "childIndex"}), ELDBUS_ARGS({"b", "result"}), _selection_is_child_selected, 0 },
   { "SelectAll", NULL, ELDBUS_ARGS({"b", "result"}), _selection_all_children_select, 0},
   { "ClearSelection", NULL, ELDBUS_ARGS({"b", "result"}), _selection_clear, 0},
   { "DeselectChild", ELDBUS_ARGS({"i", "childIndex"}), ELDBUS_ARGS({"b", "result"}), _selection_child_deselect, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static Eldbus_Message *
_action_description_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *description, *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, description = elm_interface_atspi_action_description_get(idx));
   description = description ? description : "";
   eldbus_message_arguments_append(ret, "s", description);

   return ret;
}

static Eldbus_Message *
_action_name_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *name, *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, name = elm_interface_atspi_action_name_get(idx));
   name = name ? name : "";
   eldbus_message_arguments_append(ret, "s", name);

   return ret;
}

static Eldbus_Message *
_action_localized_name_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *name, *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, name = elm_interface_atspi_action_localized_name_get(idx));
   name = name ? name : "";
   eldbus_message_arguments_append(ret, "s", name);

   return ret;
}

static Eldbus_Message *
_action_key_binding_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   char *key;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, key = elm_interface_atspi_action_keybinding_get(idx));
   eldbus_message_arguments_append(ret, "s", key ? key : "");
   if (key) free(key);

   return ret;
}

static Eldbus_Message *
_action_actions_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *action, *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eina_List *actions;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   EINA_SAFETY_ON_NULL_RETURN_VAL(iter, NULL);

   iter_array = eldbus_message_iter_container_new(iter, 'a', "sss");
   EINA_SAFETY_ON_NULL_RETURN_VAL(iter_array, NULL);

   eo_do(obj, actions = elm_interface_atspi_action_actions_get());

   int id = 0;
   EINA_LIST_FREE(actions, action)
     {
        const char *descr;
        char *key;
        eo_do(obj, key = elm_interface_atspi_action_keybinding_get(id));
        eo_do(obj, descr = elm_interface_atspi_action_description_get(id));
        descr = descr ? descr : "";
        eldbus_message_iter_arguments_append(iter_array, "sss", action, descr, key ? key : "");
        if (key) free(key);
        id++;
     }

  eldbus_message_iter_container_close(iter, iter_array);

   return ret;
}

static Eldbus_Message *
_action_action_do(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_action_do(idx));
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static const Eldbus_Method action_methods[] = {
   { "GetDescription", ELDBUS_ARGS({"i", "index"}), ELDBUS_ARGS({"s", "description"}), _action_description_get, 0 },
   { "GetName", ELDBUS_ARGS({"i", "index"}), ELDBUS_ARGS({"s", "name"}), _action_name_get, 0 },
   { "GetLocalizedName", ELDBUS_ARGS({"i", "index"}), ELDBUS_ARGS({"s", "name"}), _action_localized_name_get, 0 },
   { "GetKeyBinding", ELDBUS_ARGS({"i", "index"}), ELDBUS_ARGS({"s", "key"}), _action_key_binding_get, 0 },
   { "GetActions", NULL, ELDBUS_ARGS({"a(sss)", "actions"}), _action_actions_get, 0 },
   { "DoAction", ELDBUS_ARGS({"i", "index"}), ELDBUS_ARGS({"b", "result"}), _action_action_do, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static Eldbus_Message *
_image_extents_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   AtspiCoordType type;
   Eldbus_Message *ret;
   const char *obj_path = eldbus_message_path_get(msg);
   int x, y, w, h;
   Eina_Bool screen_coords;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   x = y = w = h = -1;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_IMAGE_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "u", &type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   screen_coords = type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, elm_interface_atspi_image_extents_get(screen_coords, &x, &y, &w, &h));
   eldbus_message_arguments_append(ret, "iiii", x, y, w, h);

   return ret;
}

static Eldbus_Message *
_image_position_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   AtspiCoordType type;
   Eldbus_Message *ret;
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x = -1, y = -1;
   Eina_Bool screen_coords;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_IMAGE_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "u", &type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   screen_coords = type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, elm_interface_atspi_image_extents_get(screen_coords, &x, &y, NULL, NULL));
   eldbus_message_arguments_append(ret, "i", x);
   eldbus_message_arguments_append(ret, "i", y);

   return ret;
}

static Eldbus_Message *
_image_size_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   Eldbus_Message *ret;
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int w = -1, h = -1;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_IMAGE_MIXIN, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, elm_interface_atspi_image_extents_get(EINA_FALSE, NULL, NULL, &w, &h));
   eldbus_message_arguments_append(ret, "i", w);
   eldbus_message_arguments_append(ret, "i", h);

   return ret;
}

static const Eldbus_Method image_methods[] = {
   { "GetImageExtents", ELDBUS_ARGS({"u", "coordType"}), ELDBUS_ARGS({"iiii", "extents"}), _image_extents_get, 0 },
   { "GetImagePosition", ELDBUS_ARGS({"u", "coordType"}), ELDBUS_ARGS({"i", "x"}, {"i", "y"}), _image_position_get, 0 },
   { "GetImageSize", NULL, ELDBUS_ARGS({"i", "width"}, {"i", "height"}), _image_size_get, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static Eldbus_Message *
_text_string_at_offset_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   char *str;
   Elm_Atspi_Text_Granularity gran;
   int start, end;
   Eldbus_Message *ret;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "iu", &start, &gran))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset and granularity expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, str = elm_interface_atspi_text_string_get(gran, &start, &end));
   str = str ? str : strdup("");

   eldbus_message_arguments_append(ret, "sii", str, start, end);
   free(str);

   return ret;
}

static Eldbus_Message *
_text_text_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   char *str;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "ii", &start, &end))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset and granularity expected.");

   Eldbus_Message *ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, str = elm_interface_atspi_text_get(start, end));
   str = str ? str : strdup("");

   eldbus_message_arguments_append(ret, "s", str);
   free(str);

   return ret;
}

static Eldbus_Message *
_text_caret_offset_set(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset;
   Eldbus_Message *ret;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &offset))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_text_caret_offset_set(offset));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_text_character_at_offset_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset;
   Eldbus_Message *ret;
   Eina_Unicode res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &offset))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_text_character_get(offset));

   eldbus_message_arguments_append(ret, "i", res);

   return ret;
}

static Eldbus_Message *
_text_attribute_value_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *name, *obj_path = eldbus_message_path_get(msg);
   char *value = NULL;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eldbus_Message *ret;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "is", &start, &name))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset and attribute name expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_text_attribute_get(name, &start, &end, &value));
   eldbus_message_arguments_append(ret, "siib", value ? value : "", start, end, res);

   if (value) free(value);
   return ret;
}

static Eldbus_Message *
_text_attributes_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eina_List *attrs;
   Elm_Atspi_Text_Attribute *attr;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &start))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "{ss}");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   eo_do(obj, attrs = elm_interface_atspi_text_attributes_get(&start, &end));

   EINA_LIST_FREE(attrs, attr)
    {
       eldbus_message_iter_arguments_append(iter_array, "ss", attr->name, attr->value);
       elm_atspi_text_text_attribute_free(attr);
    }

   eldbus_message_iter_container_close(iter, iter_array);
   eldbus_message_iter_arguments_append(iter, "ii", start, end);

   return ret;

fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static Eldbus_Message *
_text_default_attributes_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start = -1, end;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eina_List *attrs;
   Elm_Atspi_Text_Attribute *attr;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "{ss}");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   eo_do(obj, attrs = elm_interface_atspi_text_attributes_get(&start, &end));

   EINA_LIST_FREE(attrs, attr)
    {
      eldbus_message_iter_arguments_append(iter_array, "ss", attr->name, attr->value);
      elm_atspi_text_text_attribute_free(attr);
    }

   eldbus_message_iter_container_close(iter, iter_array);
   eldbus_message_iter_arguments_append(iter, "ii", start, end);

   return ret;

fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static Eldbus_Message *
_text_character_extents_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset;
   Eina_Rectangle rect;
   AtspiCoordType type;
   Eina_Bool screen_coords, res;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "iu", &offset, &type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset and coordinates type expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   screen_coords = type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;

   eo_do(obj, res = elm_interface_atspi_text_character_extents_get(offset, screen_coords, &rect));

   if (!res)
     {
        eldbus_message_unref(ret);
        return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Unable to get character extents.");
     }
   eldbus_message_arguments_append(ret, "iiii", rect.x, rect.y, rect.w, rect.h);

   return ret;
}

static Eldbus_Message *
_text_offset_at_point_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset, x, y;
   AtspiCoordType type;
   Eina_Bool screen_coords;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "iiu", &x, &y, &type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset and coordinates type expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   x = y = -1;
   screen_coords = type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;

   eo_do(obj, offset = elm_interface_atspi_text_offset_at_point_get(screen_coords, x, y));

   eldbus_message_arguments_append(ret, "i", offset);

   return ret;
}

static Eldbus_Message *
_text_n_selections_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int n;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, n = elm_interface_atspi_text_selections_count_get());

   eldbus_message_arguments_append(ret, "i", n);

   return ret;
}

static Eldbus_Message *
_text_selection_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int sel_num, start, end;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &sel_num))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Selection number expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, elm_interface_atspi_text_selection_get(sel_num, &start, &end));

   eldbus_message_arguments_append(ret, "ii", start, end);

   return ret;
}

static Eldbus_Message *
_text_selection_add(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eina_Bool res;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "ii", &start, &end))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Start and end text offset expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_text_selection_add(start, end));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_text_selection_remove(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int sel_num;
   Eina_Bool res;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &sel_num))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Selection number expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_text_selection_remove(sel_num));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_text_selection_set(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int sel_num, start, end;
   Eina_Bool res;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "iii", &sel_num, &start, &end))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Selection number expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_text_selection_set(sel_num, start, end));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_text_range_extents_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eina_Rectangle rect;
   Eina_Bool screen_coords, res;
   AtspiCoordType type;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "iiu", &start, &end, &type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Selection number expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   screen_coords = type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, res = elm_interface_atspi_text_range_extents_get(screen_coords, start, end, &rect));
   if (!res)
     {
        eldbus_message_unref(ret);
        return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Can't get range extents.");
     }

   eldbus_message_arguments_append(ret, "iiii", rect.x, rect.y, rect.w, rect.h);

   return ret;
}

static Eldbus_Message *
_text_bounded_ranges_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eina_Rectangle rect;
   Eina_Bool screen_coords;
   AtspiCoordType type;
   Elm_Atspi_Text_Clip_Type xclip, yclip;
   Eina_List *ranges;
   Eldbus_Message *ret;
   Elm_Atspi_Text_Range *range;
   Eldbus_Message_Iter *iter, *iter_array, *iter_struct, *iter_var;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "iiiiuuu", &rect.x, &rect.y, &rect.w, &rect.h, &type, &xclip, &yclip))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Expected (x,y,w,h) of bounding box, screen coord type and x, y text clip types.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "(iisv)");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   screen_coords = type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, ranges = elm_interface_atspi_text_bounded_ranges_get(screen_coords, rect, xclip, yclip));

   EINA_LIST_FREE(ranges, range)
     {
        iter_struct = eldbus_message_iter_container_new(iter_array, 'r', NULL);
        if (iter_struct)
          {
             eldbus_message_iter_basic_append(iter_struct, 'i', range->start_offset);
             eldbus_message_iter_basic_append(iter_struct, 'i', range->end_offset);
             range->content = range->content ? range->content : strdup("");
             eldbus_message_iter_basic_append(iter_struct, 's', range->content);
             /* AT-SPI specification requires variant type in return, however
              * ATK or other implementations as well as AT Clients don't use it .
              * To cover spec a dummy value will be returned */
             iter_var = eldbus_message_iter_container_new(iter_struct, 'v', "i");
             if (iter_var)
               {
                  eldbus_message_iter_basic_append(iter_var, 'i', 0);
                  eldbus_message_iter_container_close(iter_struct, iter_var);
               }
             eldbus_message_iter_container_close(iter_array, iter_struct);
          }
        if (range->content) free(range->content);
        free(range);
     }

   eldbus_message_iter_container_close(iter, iter_array);

   return ret;

fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static Eldbus_Message *
_text_run_attributes_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eina_List *attrs, *defaults;
   Elm_Atspi_Text_Attribute *attr;
   Eina_Bool incl_def;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "ib", &start, &incl_def))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Offset and include defaults flag expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "{ss}");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   eo_do(obj, attrs = elm_interface_atspi_text_attributes_get(&start, &end));

   if (incl_def)
     {
        eo_do(obj, defaults = elm_interface_atspi_text_default_attributes_get());
        attrs = eina_list_merge(attrs, defaults);
     }

   EINA_LIST_FREE(attrs, attr)
     {
        eldbus_message_iter_arguments_append(iter_array, "ss", attr->name, attr->value);
        elm_atspi_text_text_attribute_free(attr);
     }

   eldbus_message_iter_container_close(iter, iter_array);
   eldbus_message_iter_arguments_append(iter, "ii", start, end);

   return ret;

fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static const Eldbus_Method text_methods[] = {
   { "GetTextAtOffset", ELDBUS_ARGS({"i", "offset"}, {"u", "granularity"}), ELDBUS_ARGS({"s", "string"}, {"i", "startOffset"}, {"i", "endOffset"}), _text_string_at_offset_get, 0 },
   { "GetText", ELDBUS_ARGS({"i", "startOffset"}, {"i", "endOffset"}), ELDBUS_ARGS({"s", "string"}), _text_text_get, 0 },
   { "SetCaretOffset", ELDBUS_ARGS({"i", "offset"}), ELDBUS_ARGS({"b", NULL}), _text_caret_offset_set, 0 },
   { "GetCharacterAtOffset", ELDBUS_ARGS({"i", "offset"}), ELDBUS_ARGS({"i", NULL}), _text_character_at_offset_get, 0 },
   { "GetAttributeValue", ELDBUS_ARGS({"i", "offset"}, {"s", "attributeName"}), ELDBUS_ARGS({"s", NULL}, {"i", "startOffset"}, {"i", "endOffset"}, {"b", "defined"}), _text_attribute_value_get, 0 },
   { "GetAttributes", ELDBUS_ARGS({"i", "offset"}), ELDBUS_ARGS({"a(ss)", NULL}, {"i", "startOffset"}, {"i", "endOffset"}), _text_attributes_get, 0 },
   { "GetDefaultAttributes", NULL, ELDBUS_ARGS({"a(ss)", NULL}), _text_default_attributes_get, 0 },
   { "GetCharacterExtents", ELDBUS_ARGS({"i", "offset"}, {"u", "coordType"}), ELDBUS_ARGS({"i", "x"}, {"i", "y"}, {"i","w"}, {"i","h"}), _text_character_extents_get, 0 },
   { "GetOffsetAtPoint", ELDBUS_ARGS({"i", "x"}, {"i","y"}, {"u", "coordType"}), ELDBUS_ARGS({"i", NULL}), _text_offset_at_point_get, 0 },
   { "GetNSelections", NULL, ELDBUS_ARGS({"i", NULL}), _text_n_selections_get, 0 },
   { "GetSelection", ELDBUS_ARGS({"i", "selectionNum"}), ELDBUS_ARGS({"i", "startOffset"}, {"i", "endOffset"}), _text_selection_get, 0 },
   { "AddSelection", ELDBUS_ARGS({"i", "startOffset"}, {"i", "endOffset"}), ELDBUS_ARGS({"b", NULL}), _text_selection_add, 0 },
   { "RemoveSelection", ELDBUS_ARGS({"i", "selectionNum"}), ELDBUS_ARGS({"b", NULL}), _text_selection_remove, 0 },
   { "SetSelection", ELDBUS_ARGS({"i", "selectionNum"}, {"i", "startOffset"}, {"i", "endOffset"}), ELDBUS_ARGS({"b", NULL}), _text_selection_set, 0 },
   { "GetRangeExtents", ELDBUS_ARGS({"i", "startOffset"}, {"i", "endOffset"}, {"u", "coordType"}), ELDBUS_ARGS({"i", "x"}, {"i", "y"}, {"i","w"}, {"i","h"}), _text_range_extents_get, 0 },
   { "GetBoundedRanges", ELDBUS_ARGS({"i", "x"}, {"i", "y"}, {"i", "w"}, {"i", "h"}, {"u", "coordType"}, {"u", "xClipType"}, {"u", "yClipType"}), ELDBUS_ARGS({"a(issv)", NULL}), _text_bounded_ranges_get, 0 },
   { "GetAttributeRun", ELDBUS_ARGS({"i", "offset"}, {"b", "includeDefaults"}), ELDBUS_ARGS({"a(ss)", NULL}, {"i", "startOffset"}, {"i", "endOffset"}), _text_run_attributes_get, 0 },
   { "GetDefaultAttributeSet", NULL, ELDBUS_ARGS({"a(ss)", NULL}), _text_default_attributes_get, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static Eldbus_Message *
_editable_text_text_content_set(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   const char *content;
   Eldbus_Message *ret;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "s", &content))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "String expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_editable_text_content_set(content));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_editable_text_text_insert(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   const char *text;
   Eldbus_Message *ret;
   int pos, len;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "isi", &pos, &text, &len))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Postion, string, length expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_editable_text_insert(text, pos));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_editable_text_text_copy(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int start, end;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "ii", &start, &end))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Start and end index expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_editable_text_copy(start, end));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_editable_text_text_cut(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int start, end;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "ii", &start, &end))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Start and end index expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_editable_text_cut(start, end));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_editable_text_text_delete(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int start, end;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "ii", &start, &end))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Start and end index expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_editable_text_delete(start, end));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static Eldbus_Message *
_editable_text_text_paste(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int pos;
   Eina_Bool res;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE, msg);

   if (!eldbus_message_arguments_get(msg, "i", &pos))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Start and end index expected.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, res = elm_interface_atspi_editable_text_paste(pos));

   eldbus_message_arguments_append(ret, "b", res);

   return ret;
}

static const Eldbus_Method editable_text_methods[] = {
   { "SetTextContents", ELDBUS_ARGS({"s", "newcontents"}), ELDBUS_ARGS({"b", NULL}), _editable_text_text_content_set, 0 },
   { "InsertText", ELDBUS_ARGS({"i", "position"}, {"s", "text"}, {"i", "length"}), ELDBUS_ARGS({"b", NULL}), _editable_text_text_insert, 0 },
   { "CopyText", ELDBUS_ARGS({"i", "startPos"}, {"i", "endPos"}), NULL, _editable_text_text_copy, 0 },
   { "CutText", ELDBUS_ARGS({"i", "startPos"}, {"i", "endPos"}), ELDBUS_ARGS({"b", NULL}), _editable_text_text_cut, 0 },
   { "DeleteText", ELDBUS_ARGS({"i", "startPos"}, {"i", "endPos"}), ELDBUS_ARGS({"b", NULL}), _editable_text_text_delete, 0 },
   { "PasteText", ELDBUS_ARGS({"i", "position"}), ELDBUS_ARGS({"b", NULL}), _editable_text_text_paste, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static Eo *
_bridge_object_from_path(Eo *bridge, const char *path)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);
   unsigned long long eo_ptr = 0;
   Eo *eo = NULL;
   const char *tmp = path;
   Eo *ret;

   int len = strlen(ELM_ACCESS_OBJECT_PATH_PREFIX);
   if (strncmp(path, ELM_ACCESS_OBJECT_PATH_PREFIX, len))
     return NULL;

   tmp = path + len; /* Skip over the prefix */
   if (!strcmp(ELM_ACCESS_OBJECT_PATH_ROOT, tmp))
     return elm_atspi_bridge_root_get(bridge);

   sscanf(tmp, "%llu", &eo_ptr);
   eo = (Eo *) (uintptr_t) eo_ptr;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, NULL);
   if (!eina_hash_find(pd->cache, &eo))
     {
        WRN("Request for non-registered object: %s", path);
        return NULL;
     }

   ret = eo_isa(eo, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN) ? eo : NULL;

   return ret;
}

static const char *
_bridge_path_from_object(Eo *bridge, const Eo *eo)
{
   static char path[64];

   EINA_SAFETY_ON_NULL_RETURN_VAL(eo, ATSPI_DBUS_PATH_NULL);

   if (eo == elm_atspi_bridge_root_get(bridge))
     snprintf(path, sizeof(path), "%s%s", ELM_ACCESS_OBJECT_PATH_PREFIX, ELM_ACCESS_OBJECT_PATH_ROOT);
   else
     snprintf(path, sizeof(path), ELM_ACCESS_OBJECT_REFERENCE_TEMPLATE, (unsigned long long)(uintptr_t)eo);
   return path;
}

static Eina_Bool
_accessible_property_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg,
                         Eldbus_Message **error)
{
   const char *ret = NULL, *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *ret_obj = NULL, *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, request_msg, error);

   if (!strcmp(property, "Name"))
     {
        char *ret2;
        eo_do(obj, ret2 = elm_interface_atspi_accessible_name_get());
        if (!ret2)
          ret2 = strdup("");
        eldbus_message_iter_basic_append(iter, 's', ret2);
        free(ret2);
        return EINA_TRUE;
     }
   else if (!strcmp(property, "Description"))
     {
        eo_do(obj, ret = elm_interface_atspi_accessible_description_get());
        if (!ret)
          ret = "";
        eldbus_message_iter_basic_append(iter, 's', ret);
        return EINA_TRUE;
     }
   else if (!strcmp(property, "Parent"))
     {
       eo_do(obj, ret_obj = elm_interface_atspi_accessible_parent_get());
       Elm_Atspi_Role role = ELM_ATSPI_ROLE_INVALID;
       eo_do(obj, role = elm_interface_atspi_accessible_role_get());
       if ((!ret_obj) && (ELM_ATSPI_ROLE_APPLICATION == role))
         _object_desktop_reference_append(iter);
       else
         _bridge_iter_object_reference_append(bridge, iter, ret_obj);
       return EINA_TRUE;
     }
   else if (!strcmp(property, "ChildCount"))
     {
        Eina_List *l = NULL;
        eo_do(obj, l = elm_interface_atspi_accessible_children_get());
        eldbus_message_iter_basic_append(iter, 'i', eina_list_count(l));
        eina_list_free(l);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eina_Bool
_selection_property_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg,
                         Eldbus_Message **error)
{
   int n;
   const char *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE, request_msg, error);

   if (!strcmp(property, "NSelectedChildren"))
     {
        eo_do(obj, n = elm_interface_atspi_selection_selected_children_count_get());
        eldbus_message_iter_basic_append(iter, 'i', n);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eina_Bool
_action_property_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg,
                         Eldbus_Message **error)
{
   Eina_List *actions;
   const char *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN, request_msg, error);

   if (!strcmp(property, "NActions"))
     {
        eo_do(obj, actions = elm_interface_atspi_action_actions_get());
        eldbus_message_iter_basic_append(iter, 'i', eina_list_count(actions));
        eina_list_free(actions);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eldbus_Message*
_value_properties_set(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg)
{
   double value;
   Eina_Bool ret;
   const char *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_VALUE_INTERFACE, request_msg);

   if (!eldbus_message_iter_arguments_get(iter, "d", &value))
     {
       return eldbus_message_error_new(request_msg, "org.freedesktop.DBus.Error.InvalidArgs", "Expected value of type: double.");
     }

   if (!strcmp(property, "CurrentValue"))
     {
        eo_do(obj, ret = elm_interface_atspi_value_and_text_set(value, NULL));
        Eldbus_Message *answer = eldbus_message_method_return_new(request_msg);
        eldbus_message_arguments_append(answer, "b", ret);
        return answer;
     }

   return NULL;
}

static Eina_Bool
_value_properties_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg,
                         Eldbus_Message **error)
{
   double value;
   const char *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_VALUE_INTERFACE, request_msg, error);

   if (!strcmp(property, "CurrentValue"))
     {
        eo_do(obj, elm_interface_atspi_value_and_text_get(&value, NULL));
        eldbus_message_iter_basic_append(iter, 'd', value);
        return EINA_TRUE;
     }
   if (!strcmp(property, "MinimumValue"))
     {
        eo_do(obj, elm_interface_atspi_value_range_get(&value, NULL, NULL));
        eldbus_message_iter_basic_append(iter, 'd', value);
        return EINA_TRUE;
     }
   if (!strcmp(property, "MaximumValue"))
     {
        eo_do(obj, elm_interface_atspi_value_range_get(NULL, &value, NULL));
        eldbus_message_iter_basic_append(iter, 'd', value);
        return EINA_TRUE;
     }
   if (!strcmp(property, "MinimumIncrement"))
     {
        eo_do(obj, value = elm_interface_atspi_value_increment_get());
        eldbus_message_iter_basic_append(iter, 'd', value);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eina_Bool
_image_properties_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg,
                         Eldbus_Message **error)
{
   const char *value;
   const char *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_IMAGE_MIXIN, request_msg, error);

   if (!strcmp(property, "ImageDescription"))
     {
        eo_do(obj, value = elm_interface_atspi_image_description_get());
        value = value ? value : "";
        eldbus_message_iter_basic_append(iter, 's', value);
        return EINA_TRUE;
     }
   if (!strcmp(property, "ImageLocale"))
     {
        eo_do(obj, value = elm_interface_atspi_image_locale_get());
        value = value ? value : "";
        eldbus_message_iter_basic_append(iter, 's', value);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eina_Bool
_text_properties_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg,
                         Eldbus_Message **error)
{
   const char *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int val;

   ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE, request_msg, error);

   if (!strcmp(property, "CharacterCount"))
     {
        eo_do(obj, val = elm_interface_atspi_text_character_count_get());
        eldbus_message_iter_basic_append(iter, 'i', val);
        return EINA_TRUE;
     }
   if (!strcmp(property, "CaretOffset"))
     {
        eo_do(obj, val = elm_interface_atspi_text_caret_offset_get());
        eldbus_message_iter_basic_append(iter, 'i', val);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static Eldbus_Message*
_application_properties_set(const Eldbus_Service_Interface *iface, const char *property, Eldbus_Message_Iter *iter, const Eldbus_Message *input_msg)
{
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   const char *obj_path = eldbus_message_path_get(input_msg);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int value;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, NULL);
   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_ATSPI_APP_OBJECT_CLASS, input_msg);

   if (!eldbus_message_iter_arguments_get(iter, "i", &value))
     {
       return eldbus_message_error_new(input_msg, "org.freedesktop.DBus.Error.InvalidArgs", "Expected value of type: int.");
     }

   if (!strcmp(property, "Id"))
     {
        pd->id = value;
        Eldbus_Message *answer = eldbus_message_method_return_new(input_msg);
        eldbus_message_arguments_append(answer, "b", EINA_TRUE);
        return answer;
     }

   return NULL;
}

static Eina_Bool
_application_properties_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg,
                         Eldbus_Message **error)
{
   const char *obj_path = eldbus_message_path_get(request_msg);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, EINA_FALSE);
   ELM_ATSPI_PROPERTY_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_ATSPI_APP_OBJECT_CLASS, request_msg, error);

   if (!strcmp(property, "ToolkitName"))
     {
        eldbus_message_iter_basic_append(iter, 's', "elementary");
        return EINA_TRUE;
     }
   if (!strcmp(property, "Version"))
     {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d.%d", ELM_VERSION_MAJOR, ELM_VERSION_MINOR);
        eldbus_message_iter_basic_append(iter, 's', buf);
        return EINA_TRUE;
     }
   if (!strcmp(property, "Id"))
     {
        eldbus_message_iter_basic_append(iter, 'i', pd->id);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

static const Eldbus_Property accessible_properties[] = {
   { "Name", "s", NULL, NULL, 0 },
   { "Description", "s", NULL, NULL, 0 },
   { "Parent", "(so)", NULL, NULL, 0 },
   { "ChildCount", "i", NULL, NULL, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Property action_properties[] = {
   { "NActions", "i", _action_property_get, NULL, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Property value_properties[] = {
   { "MinimumValue", "d", NULL, NULL, 0 },
   { "MaximumValue", "d", NULL, NULL, 0 },
   { "MinimumIncrement", "d", NULL, NULL, 0 },
   { "CurrentValue", "d", NULL, NULL, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Property image_properties[] = {
   { "ImageDescription", "s", NULL, NULL, 0 },
   { "ImageLocale", "s", NULL, NULL, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Property selection_properties[] = {
   { "NSelectedChildren", "i", _selection_property_get, NULL, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Property text_properties[] = {
   { "CharacterCount", "i", NULL, NULL, 0 },
   { "CaretOffset", "i", NULL, NULL, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Property application_properties[] = {
   { "ToolkitName", "s", NULL, NULL, 0 },
   { "Version", "s", NULL, NULL, 0 },
   { "Id", "i", NULL, NULL, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Service_Interface_Desc accessible_iface_desc = {
   ATSPI_DBUS_INTERFACE_ACCESSIBLE, accessible_methods, NULL, accessible_properties, _accessible_property_get, NULL
};

static const Eldbus_Service_Interface_Desc action_iface_desc = {
   ATSPI_DBUS_INTERFACE_ACTION, action_methods, NULL, action_properties, NULL, NULL
};

static const Eldbus_Service_Interface_Desc value_iface_desc = {
   ATSPI_DBUS_INTERFACE_VALUE, NULL, NULL, value_properties, _value_properties_get, _value_properties_set
};

static const Eldbus_Service_Interface_Desc image_iface_desc = {
   ATSPI_DBUS_INTERFACE_IMAGE, image_methods, NULL, image_properties, _image_properties_get, NULL
};

static const Eldbus_Service_Interface_Desc selection_iface_desc = {
   ATSPI_DBUS_INTERFACE_SELECTION, selection_methods, NULL, selection_properties, NULL, NULL
};

static const Eldbus_Service_Interface_Desc text_iface_desc = {
   ATSPI_DBUS_INTERFACE_TEXT, text_methods, NULL, text_properties, _text_properties_get, NULL
};

static const Eldbus_Service_Interface_Desc editable_text_iface_desc = {
   ATSPI_DBUS_INTERFACE_EDITABLE_TEXT, editable_text_methods, NULL, NULL, NULL, NULL
};

static const Eldbus_Service_Interface_Desc application_iface_desc = {
   ATSPI_DBUS_INTERFACE_APPLICATION, NULL, NULL, application_properties, _application_properties_get, _application_properties_set
};

void
_collection_match_rule_free(struct collection_match_rule *rule)
{
   Elm_Atspi_Attribute *attr;
   eina_list_free(rule->ifaces);
   EINA_LIST_FREE(rule->attributes, attr)
     {
        eina_stringshare_del(attr->key);
        eina_stringshare_del(attr->value);
     }
}

static void
_collection_roles_convert(uint64_t roles[2])
{
   // Currently elm roles and atspi roles are binary compatible.
   // Implement this function when it will be needed.
   (void)roles;
}

static Eina_Bool
_collection_iter_match_rule_get(Eldbus_Message_Iter *iter, struct collection_match_rule *rule)
{
   Eldbus_Message_Iter *states_iter, *attrib_iter, *iter_arg, *role_iter, *ifc_iter;
   unsigned int *array;
   int array_count, state_match, attrib_match, role_match, ifc_match, reverse;
   const char *ifc_name;

   if (!eldbus_message_iter_arguments_get(iter, "aiia{ss}iaiiasib", &states_iter, &state_match, &attrib_iter, &attrib_match, &role_iter, &role_match, &ifc_iter, &ifc_match, &reverse))
     {
        ERR("Unable to get message arguments");
        return EINA_FALSE;
     }

   memset(rule, 0x0, sizeof(struct collection_match_rule));
   rule->statematchtype = state_match;
   rule->attributematchtype = attrib_match;
   rule->rolematchtype = role_match;
   rule->interfacematchtype = ifc_match;
   rule->reverse = reverse;

   if (!eldbus_message_iter_fixed_array_get(states_iter, 'i', &array, &array_count))
     return EINA_FALSE;

   //Roles according to libatspi impementation are transferred in 2-int element fixed bit array
   if (array_count != 2)
     {
        ERR("Unexpected states array size");
        return EINA_FALSE;
     }
   uint64_t states = ((uint64_t)array[0] | ((uint64_t)array[1] << 32));
   rule->states = _atspi_state_set_to_elm_atspi_state_set(states);

   //Roles according to libatspi impementation are transferred in 4-int element fixed bit array
   if (!eldbus_message_iter_fixed_array_get(role_iter, 'i', &array, &array_count))
     return EINA_FALSE;

   if (array_count != 4)
     {
        ERR("Unexpected roles array size");
        return EINA_FALSE;
     }

   //convert atspi roles to elm_roles
   rule->roles[0] = ((uint64_t)array[0] | ((uint64_t)array[1] << 32));
   rule->roles[1] = ((uint64_t)array[2] | ((uint64_t)array[3] << 32));

   _collection_roles_convert(rule->roles);

   //Get matching properties
   while (eldbus_message_iter_get_and_next(attrib_iter, '{', &iter_arg))
     {
        const char *key, *value;
        if (eldbus_message_iter_arguments_get(iter_arg, "ss", &key, &value))
          {
             Elm_Atspi_Attribute *attrib = calloc(sizeof(Elm_Atspi_Attribute), 1);
             attrib->key = eina_stringshare_add(key);
             attrib->value = eina_stringshare_add(value);
             rule->attributes = eina_list_append(rule->attributes, attrib);
          }
     }

   //Get interfaces to match
   while (eldbus_message_iter_get_and_next(ifc_iter, 's', &ifc_name))
     {
        const Eo_Class *class = NULL;
        if (!strcmp(ifc_name, "action"))
          class = ELM_INTERFACE_ATSPI_ACTION_MIXIN;
        else if (!strcmp(ifc_name, "component"))
          class = ELM_INTERFACE_ATSPI_COMPONENT_MIXIN;
        else if (!strcmp(ifc_name, "editabletext"))
          class = ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE;
        else if (!strcmp(ifc_name, "text"))
          class = ELM_INTERFACE_ATSPI_TEXT_INTERFACE;
        else if (!strcmp(ifc_name, "image"))
          class = ELM_INTERFACE_ATSPI_SELECTION_INTERFACE;
        else if (!strcmp(ifc_name, "value"))
          class = ELM_INTERFACE_ATSPI_VALUE_INTERFACE;

        if (class)
          rule->ifaces = eina_list_append(rule->ifaces, class);
        else
          {
             _collection_match_rule_free(rule);
             return EINA_FALSE;
          }
     }

   return EINA_TRUE;
}

static Eina_Bool
_collection_match_interfaces_helper(Eo *obj, Eina_List *ifcs, Eina_Bool condition, Eina_Bool ret_if_true, Eina_Bool ret_if_false)
{
   Eo_Class *class;
   Eina_List *l;

   EINA_LIST_FOREACH(ifcs, l, class)
     {
        if (eo_isa(obj, class) == condition)
          return ret_if_true;
     }
   return ret_if_false;
}

static Eina_Bool
_collection_match_interfaces_lookup(Eo *obj, struct collection_match_rule *rule)
{
   Eina_Bool ret = EINA_FALSE;

   switch (rule->interfacematchtype)
     {
        case ATSPI_Collection_MATCH_INVALID:
           ret = EINA_TRUE;
           break;
        case ATSPI_Collection_MATCH_ALL:
           ret = _collection_match_interfaces_helper(
              obj, rule->ifaces, EINA_FALSE, EINA_FALSE, EINA_TRUE);
           break;
        case ATSPI_Collection_MATCH_ANY:
           ret = _collection_match_interfaces_helper(
              obj, rule->ifaces, EINA_TRUE, EINA_TRUE, EINA_FALSE);
           break;
        case ATSPI_Collection_MATCH_NONE:
           ret = _collection_match_interfaces_helper(
              obj, rule->ifaces, EINA_TRUE, EINA_FALSE, EINA_TRUE);
           break;
        default:
           break;
     }
   return ret;
}

static Eina_Bool
_collection_match_states_lookup(Eo *obj, struct collection_match_rule *rule)
{
   Eina_Bool ret = EINA_FALSE;
   Elm_Atspi_State_Set ss;

   eo_do(obj, ss = elm_interface_atspi_accessible_state_set_get());

   switch (rule->statematchtype)
     {
        case ATSPI_Collection_MATCH_INVALID:
           ret = EINA_TRUE;
           break;
        case ATSPI_Collection_MATCH_ALL:
           ret = (ss & rule->states) == rule->states;
           break;
        case ATSPI_Collection_MATCH_ANY:
           ret = (ss & rule->states) > 0;
           break;
        case ATSPI_Collection_MATCH_NONE:
           ret = (ss & rule->states) == 0;
           break;
        default:
           break;
     }

   return ret;
}

static Eina_Bool
_collection_match_roles_lookup(Eo *obj, struct collection_match_rule *rule)
{
   Eina_Bool ret = EINA_FALSE;
   Elm_Atspi_Role role;
   int64_t role_set;

   eo_do(obj, role = elm_interface_atspi_accessible_role_get());

   if (role >= 64)
     {
        role -= 64;
        role_set = rule->roles[1];
     }
   else
     role_set = rule->roles[0];

   if (role >= 64)
     {
        ERR("Elm_Atspi_Role enum value exceeds 127. Unable to compare with roles bit field.");
        return EINA_FALSE;
     }

   switch (rule->rolematchtype)
     {
        case ATSPI_Collection_MATCH_INVALID:
           ret = EINA_TRUE;
           break;
        case ATSPI_Collection_MATCH_ALL:
        case ATSPI_Collection_MATCH_ANY:
           ret = (role_set & (1ULL << role)) > 0;
           break;
        case ATSPI_Collection_MATCH_NONE:
           ret = (role_set & (1ULL << role)) == 0;
           break;
        default:
           break;
     }

   return ret;
}

static Eina_Bool
_collection_match_attributes_helper(Eina_List *obj_attribs, Eina_List *attribs, Eina_Bool compare, Eina_Bool ret_if_compare, Eina_Bool ret_default)
{
   Eina_List *l, *l2;
   Elm_Atspi_Attribute *attr, *attr2;

   EINA_LIST_FOREACH(attribs, l, attr)
     {
        EINA_LIST_FOREACH(obj_attribs, l2, attr2)
          {
             if ((attr->key && attr2->key &&
                  attr->value && attr2->value &&
                  !strcmp(attr->key, attr2->key) &&
                  !strcmp(attr->value, attr2->value)) == compare)
               {
                  return ret_if_compare;
               }
          }
     }

   return ret_default;
}

static Eina_Bool
_collection_match_attributes_lookup(Eo *obj, struct collection_match_rule *rule)
{
   Eina_Bool ret = EINA_FALSE;
   Eina_List *obj_attribs;

   eo_do(obj, obj_attribs = elm_interface_atspi_accessible_attributes_get());

   switch (rule->attributematchtype)
     {
        case ATSPI_Collection_MATCH_INVALID:
           ret = EINA_TRUE;
           break;
        case ATSPI_Collection_MATCH_ALL:
           ret = _collection_match_attributes_helper(
              obj_attribs, rule->attributes, EINA_FALSE, EINA_FALSE, EINA_TRUE);
           break;
        case ATSPI_Collection_MATCH_ANY:
           ret = _collection_match_attributes_helper(
              obj_attribs, rule->attributes, EINA_TRUE, EINA_TRUE, EINA_FALSE);
           break;
        case ATSPI_Collection_MATCH_NONE:
           ret = _collection_match_attributes_helper(
              obj_attribs, rule->attributes, EINA_TRUE, EINA_FALSE, EINA_TRUE);
           break;
        default:
           break;
     }

   elm_atspi_attributes_list_free(obj_attribs);

   return ret;
}

static int
_collection_sort_order_canonical(struct collection_match_rule *rule, Eina_List **ls,
                      int count, int max,
                      Eo *obj, long index, Eina_Bool flag,
                      Eo *pobj, Eina_Bool recurse, Eina_Bool traverse)
{
   int i = index;
   Eina_List *children;
   eo_do(obj, children = elm_interface_atspi_accessible_children_get());
   long acount = eina_list_count(children);
   Eina_Bool prev = pobj ? EINA_TRUE : EINA_FALSE;

   for (; i < acount && (max == 0 || count < max); i++)
     {
        Eo *child = eina_list_nth(children, i);

        if (prev && child == pobj)
          {
             eina_list_free(children);
             return count;
          }

        if (flag && _collection_match_interfaces_lookup(child, rule)
            && _collection_match_states_lookup(child, rule)
            && _collection_match_roles_lookup(child, rule)
            && _collection_match_attributes_lookup(child, rule))
          {
             *ls = eina_list_append(*ls, child);
             count++;
          }

       if (!flag)
         flag = EINA_TRUE;

       if (recurse && traverse)
         count = _collection_sort_order_canonical(rule, ls, count,
                                                  max, child, 0, EINA_TRUE,
                                                  pobj, recurse, traverse);
     }
   eina_list_free(children);
   return count;
}

static int
_collection_sort_order_reverse_canonical(struct collection_match_rule *rule, Eina_List **ls,
                      int count, int max, Eo *obj, Eina_Bool flag, Eo *pobj)
{
  Eo *nextobj, *parent;
  long indexinparent;
  Eina_List *children;

  /* This breaks us out of the recursion. */
  if (!obj || obj == pobj)
    {
      return count;
    }

  /* Add to the list if it matches */
  if (flag && _collection_match_interfaces_lookup(obj, rule)
      && _collection_match_states_lookup(obj, rule)
      && _collection_match_roles_lookup(obj, rule)
      && _collection_match_attributes_lookup(obj, rule)
      && (max == 0 || count < max))
    {
       *ls = eina_list_append(*ls, obj);
       count++;
    }

  if (!flag)
    flag = EINA_TRUE;

  /* Get the current nodes index in it's parent and the parent object. */
  eo_do(obj,
        indexinparent = elm_interface_atspi_accessible_index_in_parent_get(),
        parent = elm_interface_atspi_accessible_parent_get());

  if ((indexinparent > 0) && ((max == 0) || (count < max)))
    {
       /* there are still some siblings to visit so get the previous sibling
          and get it's last descendant.
          First, get the previous sibling */
       eo_do(parent, children = elm_interface_atspi_accessible_children_get());
       nextobj = eina_list_nth(children, indexinparent - 1);
       eina_list_free(children);

       /* Now, drill down the right side to the last descendant */
       do {
            eo_do(nextobj, children = elm_interface_atspi_accessible_children_get());
            if (children) nextobj = eina_list_last_data_get(children);
            eina_list_free(children);
       } while (children);

       /* recurse with the last descendant */
       count = _collection_sort_order_reverse_canonical(rule, ls, count, max,
                                       nextobj, EINA_TRUE, pobj);
    }
  else if (max == 0 || count < max)
    {
      /* no more siblings so next node must be the parent */
      count = _collection_sort_order_reverse_canonical(rule, ls, count, max,
                                        parent, EINA_TRUE, pobj);

    }
  return count;
}

static int
_collection_inbackorder(Eo *collection, struct collection_match_rule *rule, Eina_List **list,
                        int max, Eo *obj)
{
   *list = eina_list_append(*list, obj);

   _collection_sort_order_reverse_canonical(rule, list, 0, max, obj, EINA_TRUE, collection);

   *list = eina_list_remove_list(*list, *list);

   return 0;
}

static int
_collection_inorder(Eo *collection, struct collection_match_rule *rule, Eina_List **list,
                    int count, int max, Eo *obj, Eina_Bool traverse)
{
   int idx = 0;

   count = _collection_sort_order_canonical(rule, list, count, max, obj, 0, EINA_TRUE, NULL, EINA_TRUE, traverse);

  while ((max == 0 || count < max) && obj && obj != collection)
    {
       Eo *parent;
       eo_do(obj,
             parent = elm_interface_atspi_accessible_parent_get(),
             idx = elm_interface_atspi_accessible_index_in_parent_get());
       count = _collection_sort_order_canonical(rule, list, count, max, parent,
                                     idx + 1, EINA_TRUE, NULL, EINA_TRUE, traverse);
       obj = parent;
    }

  if (max == 0 || count < max)
    count = _collection_sort_order_canonical(rule, list, count, max,
                                    obj, idx + 1, EINA_TRUE, NULL, EINA_TRUE, traverse);

  return count;
}

static int
_collection_query(struct collection_match_rule *rule, AtspiCollectionSortOrder sortby,
                         Eina_List **list, int count, int max, Eo *obj, long index,
                         Eina_Bool flag, Eo *pobj, Eina_Bool recurse, Eina_Bool traverse)
{
   switch (sortby)
     {
        case ATSPI_Collection_SORT_ORDER_CANONICAL:
           count = _collection_sort_order_canonical(rule, list, 0, max, obj, index, flag,
                                                    pobj, recurse, traverse);
           break;
        case ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL:
           count = _collection_sort_order_canonical(rule, list, 0, max, obj, index, flag,
                                                    pobj, recurse, traverse);
           *list = eina_list_reverse(*list);
           break;
        default:
          count = 0;
          WRN("Unhandled sort method");
          break;
     }
   return count;
}

static Eldbus_Message*
_collection_return_msg_from_list(Elm_Atspi_Bridge *bridge, const Eldbus_Message *msg, const Eina_List *objs)
{
   Eldbus_Message *ret;
   const Eina_List *l;
   Eldbus_Message_Iter *iter, *array_iter;
   Eo *obj;

   ret = eldbus_message_method_return_new(msg);
   if (!ret) return NULL;

   iter = eldbus_message_iter_get(ret);
   array_iter = eldbus_message_iter_container_new(iter, 'a', "(so)");

   EINA_LIST_FOREACH(objs, l, obj)
     {
        _bridge_object_register(bridge, obj);
        _bridge_iter_object_reference_append(bridge, array_iter, obj);
     }

   eldbus_message_iter_container_close(iter, array_iter);
   return ret;
}

static Eina_List*
_collection_get_matches_from_handle(Eo *collection, Eo *current, struct collection_match_rule *rule, AtspiCollectionSortOrder sortby, AtspiCollectionTreeTraversalType tree, int max, Eina_Bool traverse)
{
   Eina_List *result = NULL;
   Eo *parent;
   int idx;

   switch (tree)
     {
      case ATSPI_Collection_TREE_INORDER:
           _collection_inorder(collection, rule, &result, 0, max, current, traverse);
         if (sortby == ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL)
           result = eina_list_reverse(result);
         break;
      case ATSPI_Collection_TREE_RESTRICT_CHILDREN:
         eo_do(current,
               idx = elm_interface_atspi_accessible_index_in_parent_get(),
               parent = elm_interface_atspi_accessible_parent_get());
         _collection_query(rule, sortby, &result, 0, max, parent, idx, EINA_FALSE, NULL, EINA_TRUE, traverse);
         break;
      case ATSPI_Collection_TREE_RESTRICT_SIBLING:
         _collection_query(rule, sortby, &result, 0, max, current, 0, EINA_FALSE, NULL, EINA_TRUE, traverse);
         break;
      default:
         ERR("Tree parameter value not handled");
         break;
     }
   return result;
}

static Eldbus_Message*
_collection_get_matches_from(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg EINA_UNUSED)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *current, *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *rule_iter;
   struct collection_match_rule rule;
   int count;
   AtspiCollectionTreeTraversalType tree;
   Eina_Bool traverse;
   AtspiCollectionSortOrder sortby;
   Eina_List *result = NULL;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   iter = eldbus_message_iter_get(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(iter, NULL);

   if (!eldbus_message_iter_arguments_get(iter, "o(aiia{ss}iaiiasib)uuib", &obj_path, &rule_iter, &sortby, &tree, &count, &traverse))
     {
        return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Unable to get matchule, sortby, count or traverse values.");
     }

   current = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(current, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   if (!_collection_iter_match_rule_get(rule_iter, &rule))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Invalid match rule parameters.");

   result = _collection_get_matches_from_handle(obj, current, &rule, sortby, tree, count, traverse);
   ret = _collection_return_msg_from_list(bridge, msg, result);

   eina_list_free(result);
   _collection_match_rule_free(&rule);

   return ret;
}

static Eina_List*
_collection_get_matches_to_handle(Eo *obj, Eo *current, struct collection_match_rule *rule, AtspiCollectionSortOrder sortby, AtspiCollectionTreeTraversalType tree, Eina_Bool limit, int max, Eina_Bool traverse)
{
   Eina_List *result = NULL;
   Eo *collection = obj;

   if (limit)
     eo_do(obj, collection = elm_interface_atspi_accessible_parent_get());

   switch (tree)
     {
      case ATSPI_Collection_TREE_INORDER:
         _collection_inbackorder(obj, rule, &result, max, current);
         if (sortby == ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL)
           result = eina_list_reverse(result);
         break;
      case ATSPI_Collection_TREE_RESTRICT_CHILDREN:
         _collection_query(rule, sortby, &result, 0, max, collection, 0, EINA_FALSE, current, EINA_TRUE, traverse);
         break;
      case ATSPI_Collection_TREE_RESTRICT_SIBLING:
         _collection_query(rule, sortby, &result, 0, max, collection, 0, EINA_FALSE, current, EINA_TRUE, traverse);
         break;
      default:
         ERR("Tree parameter value not handled");
         break;
     }

   return result;
}

static Eldbus_Message*
_collection_get_matches_to(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg EINA_UNUSED)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *current, *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *rule_iter;
   struct collection_match_rule rule;
   int count;
   AtspiCollectionTreeTraversalType tree;
   Eina_Bool traverse;
   AtspiCollectionSortOrder sortby;
   Eina_List *result = NULL;
   Eina_Bool limit;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   iter = eldbus_message_iter_get(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(iter, NULL);

   if (!eldbus_message_iter_arguments_get(iter, "o(aiia{ss}iaiiasib)uubib", &obj_path, &rule_iter, &sortby, &tree, &limit, &count, &traverse))
     {
        return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Unable to get matchule, sortby, tree, limit count or traverse values.");
     }

   current = _bridge_object_from_path(bridge, obj_path);

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(current, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   if (!_collection_iter_match_rule_get(rule_iter, &rule))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Invalid match rule parameters.");

   result = _collection_get_matches_to_handle(obj, current, &rule, sortby, tree, limit, count, traverse);
   ret = _collection_return_msg_from_list(bridge, msg, result);

   eina_list_free(result);
   _collection_match_rule_free(&rule);

   return ret;
}

static Eldbus_Message*
_collection_get_matches(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *rule_iter;
   struct collection_match_rule rule;
   int count;
   Eina_Bool traverse;
   AtspiCollectionSortOrder sortby;
   Eina_List *result = NULL;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, msg);

   iter = eldbus_message_iter_get(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(iter, NULL);

   if (!eldbus_message_iter_arguments_get(iter, "(aiia{ss}iaiiasib)uib", &rule_iter, &sortby, &count, &traverse))
     {
        return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Unable to get matchule, sortby, count or traverse values.");
     }

   if (!_collection_iter_match_rule_get(rule_iter, &rule))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.Failed", "Invalid match rule parameters.");

   _collection_query(&rule, sortby, &result, 0, count, obj, 0, EINA_TRUE, NULL, EINA_TRUE, traverse);

   ret = _collection_return_msg_from_list(bridge, msg, result);

   eina_list_free(result);
   _collection_match_rule_free(&rule);

   return ret;
}

static const Eldbus_Method collection_methods[] = {
   { "GetMatchesFrom",
      ELDBUS_ARGS({"o", "current_object"}, {"(aiia{ss}iaiiasib)", "match_rule"},
                  {"u", "sortby"}, {"u", "tree"}, {"i", "count"}, {"b", "traverse"}),
      ELDBUS_ARGS({"a(so)", "objects"}), _collection_get_matches_from, 0 },
   { "GetMatchesTo",
      ELDBUS_ARGS({"o", "current_object"}, {"(aiia{ss}iaiiasib)", "match_rule"},
                  {"u", "sortby"}, {"u", "tree"}, {"b", "limit_scope"},
                  {"i", "count"}, {"b", "traverse"}),
      ELDBUS_ARGS({"a(so)", "objects"}), _collection_get_matches_to, 0 },
   { "GetMatches",
      ELDBUS_ARGS({"(aiia{ss}iaiiasib)", "match_rule"},
                  {"u", "sortby"}, {"i", "count"}, {"b", "traverse"}),
      ELDBUS_ARGS({"a(so)", "objects"}), _collection_get_matches, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Service_Interface_Desc collection_iface_desc = {
   ATSPI_DBUS_INTERFACE_COLLECTION, collection_methods, NULL, NULL, NULL, NULL
};

static void
_bridge_iter_object_reference_append(Eo *bridge, Eldbus_Message_Iter *iter, const Eo *obj)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);
   Eldbus_Message_Iter *iter_struct = eldbus_message_iter_container_new(iter, 'r', NULL);
   EINA_SAFETY_ON_NULL_RETURN(iter);
   const char *path = _bridge_path_from_object(bridge, obj);
   eldbus_message_iter_basic_append(iter_struct, 's', eldbus_connection_unique_name_get(pd->a11y_bus));
   eldbus_message_iter_basic_append(iter_struct, 'o', path);
   eldbus_message_iter_container_close(iter, iter_struct);
}

static void
_object_desktop_reference_append(Eldbus_Message_Iter *iter)
{
  Eldbus_Message_Iter *iter_struct = eldbus_message_iter_container_new(iter, 'r', NULL);
  EINA_SAFETY_ON_NULL_RETURN(iter);

  eldbus_message_iter_basic_append(iter_struct, 's', ATSPI_DBUS_NAME_REGISTRY);
  eldbus_message_iter_basic_append(iter_struct, 'o', ATSPI_DBUS_PATH_ROOT);
  eldbus_message_iter_container_close(iter, iter_struct);
}

static void
_iter_interfaces_append(Eldbus_Message_Iter *iter, const Eo *obj)
{
  Eldbus_Message_Iter *iter_array;
  iter_array = eldbus_message_iter_container_new(iter, 'a', "s");
  if (!iter_array) return;

  if (eo_isa(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN))
    {
       eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_ACCESSIBLE);
       eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_COLLECTION);
    }
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_ACTION);
  if (eo_isa(obj, ELM_ATSPI_APP_OBJECT_CLASS))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_APPLICATION);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_COMPONENT);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_EDITABLE_TEXT);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_IMAGE_MIXIN))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_IMAGE);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_SELECTION);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_TEXT);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_VALUE_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_VALUE);

  eldbus_message_iter_container_close(iter, iter_array);
}

static Eina_Bool
_cache_item_reference_append_cb(Eo *bridge, Eo *data, Eldbus_Message_Iter *iter_array)
{
  if (!eo_ref_get(data) || eo_destructed_is(data))
    return EINA_TRUE;

  Eldbus_Message_Iter *iter_struct, *iter_sub_array;
  Elm_Atspi_State_Set states;
  Elm_Atspi_Role role;
  Eo *root = elm_atspi_bridge_root_get(bridge);

  eo_do(data, role = elm_interface_atspi_accessible_role_get());

  iter_struct = eldbus_message_iter_container_new(iter_array, 'r', NULL);
  EINA_SAFETY_ON_NULL_RETURN_VAL(iter_struct, EINA_TRUE);

  /* Marshall object path */
  _bridge_iter_object_reference_append(bridge, iter_struct, data);

  /* Marshall application */
  _bridge_iter_object_reference_append(bridge, iter_struct, root);

  Eo *parent = NULL;
  eo_do(data, parent = elm_interface_atspi_accessible_parent_get());
  /* Marshall parent */
  if ((!parent) && (ELM_ATSPI_ROLE_APPLICATION == role))
    _object_desktop_reference_append(iter_struct);
  else
    _bridge_iter_object_reference_append(bridge, iter_struct, parent);

  /* Marshall children  */
  Eina_List *children_list = NULL, *l;
  Eo *child;

  eo_do(data, children_list = elm_interface_atspi_accessible_children_get());
  iter_sub_array = eldbus_message_iter_container_new(iter_struct, 'a', "(so)");
  EINA_SAFETY_ON_NULL_GOTO(iter_sub_array, fail);

  EINA_LIST_FOREACH(children_list, l, child)
     _bridge_iter_object_reference_append(bridge, iter_sub_array, child);

  eldbus_message_iter_container_close(iter_struct, iter_sub_array);
  eina_list_free(children_list);

  /* Marshall interfaces */
  _iter_interfaces_append(iter_struct, data);

  /* Marshall name */
  char *name = NULL;
  eo_do(data, name = elm_interface_atspi_accessible_name_get());
  if (!name)
    name = strdup("");

  eldbus_message_iter_basic_append(iter_struct, 's', name);
  free(name);

  /* Marshall role */
  eldbus_message_iter_basic_append(iter_struct, 'u', role);

  /* Marshall description */
  const char* descritpion = NULL;
  eo_do(data, descritpion = elm_interface_atspi_accessible_description_get());
  if (!descritpion)
    descritpion = "";
  eldbus_message_iter_basic_append(iter_struct, 's', descritpion);

  /* Marshall state set */
  iter_sub_array = eldbus_message_iter_container_new(iter_struct, 'a', "u");
  EINA_SAFETY_ON_NULL_GOTO(iter_sub_array, fail);

  eo_do(data, states = elm_interface_atspi_accessible_state_set_get());

  unsigned int s1 = states & 0xFFFFFFFF;
  unsigned int s2 = (states >> 32) & 0xFFFFFFFF;
  eldbus_message_iter_basic_append(iter_sub_array, 'u', s1);
  eldbus_message_iter_basic_append(iter_sub_array, 'u', s2);

  eldbus_message_iter_container_close(iter_struct, iter_sub_array);
  eldbus_message_iter_container_close(iter_array, iter_struct);

  return EINA_TRUE;

fail:
  if (iter_struct) eldbus_message_iter_del(iter_struct);
  return EINA_TRUE;
}

static Eldbus_Message *
_cache_get_items(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   Eldbus_Message_Iter *iter, *iter_array;
   Eldbus_Message *ret;
   Eina_List *to_process;
   Eo *root;

   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   if (!bridge) return NULL;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, NULL);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', CACHE_ITEM_SIGNATURE);
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   eo_do(bridge, root = elm_obj_atspi_bridge_root_get());
   to_process = eina_list_append(NULL, root);

   while (to_process)
     {
        Eo *obj = eina_list_data_get(to_process);
        to_process = eina_list_remove_list(to_process, to_process);
        _cache_item_reference_append_cb(bridge, obj, iter_array);
        _bridge_object_register(bridge, obj);

        Eina_List *children;
        eo_do(obj, children = elm_interface_atspi_accessible_children_get());
        to_process = eina_list_merge(to_process, children);
     }

   eldbus_message_iter_container_close(iter, iter_array);

   return ret;
fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static const Eldbus_Method cache_methods[] = {
   { "GetItems", NULL, ELDBUS_ARGS({CACHE_ITEM_SIGNATURE, "items"}), _cache_get_items, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Signal cache_signals[] = {
  [ATSPI_OBJECT_CHILD_ADDED] = { "AddAccessible", ELDBUS_ARGS({"((so)(so)a(so)assusau)", "added"}), 0},
  [ATSPI_OBJECT_CHILD_REMOVED] = { "RemoveAccessible", ELDBUS_ARGS({ "(so)", "removed" }), 0},
  {NULL, NULL, 0}
};

static const Eldbus_Service_Interface_Desc cache_iface_desc = {
   ATSPI_DBUS_INTERFACE_CACHE, cache_methods, cache_signals, NULL, NULL, NULL
};

// Component interface
static Eldbus_Message *
_component_contains(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eina_Bool contains = EINA_FALSE;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "iiu", &x, &y, &coord_type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   Eina_Bool type = coord_type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, contains = elm_interface_atspi_component_contains(type, x, y));

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "b", contains);

   return ret;
}

static Eldbus_Message *
_component_get_accessible_at_point(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eo *accessible = NULL;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "iiu", &x, &y, &coord_type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   Eina_Bool type = coord_type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, accessible = elm_interface_atspi_component_accessible_at_point_get(type, x, y));
   _bridge_iter_object_reference_append(bridge, iter, accessible);
   _bridge_object_register(bridge, accessible);

   return ret;
}

static Eldbus_Message *
_component_get_extents(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y, w, h;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_struct;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "u", &coord_type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);

   Eina_Bool type = coord_type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, elm_interface_atspi_component_extents_get(type, &x, &y, &w, &h));
   iter_struct = eldbus_message_iter_container_new(iter, 'r', NULL);
   EINA_SAFETY_ON_NULL_GOTO(iter_struct, fail);

   eldbus_message_iter_basic_append(iter_struct, 'i', x);
   eldbus_message_iter_basic_append(iter_struct, 'i', y);
   eldbus_message_iter_basic_append(iter_struct, 'i', w);
   eldbus_message_iter_basic_append(iter_struct, 'i', h);

   eldbus_message_iter_container_close(iter, iter_struct);

   return ret;
fail:
   if (ret) eldbus_message_unref(ret);
   return NULL;
}

static Eldbus_Message *
_component_get_position(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "u", &coord_type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   Eina_Bool type = coord_type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, elm_interface_atspi_component_position_get(type, &x, &y));

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "i", x);
   eldbus_message_arguments_append(ret, "i", y);

   return ret;
}

static Eldbus_Message *
_component_get_size(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   eo_do(obj, elm_interface_atspi_component_size_get(&x, &y));

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "i", x);
   eldbus_message_arguments_append(ret, "i", y);

   return ret;
}

static AtspiComponentLayer
_elm_layer_2_atspi_layer(int layer)
{
   if (layer <= ELM_OBJECT_LAYER_BACKGROUND) return ATSPI_LAYER_CANVAS;
   if (layer < ELM_OBJECT_LAYER_FOCUS) return ATSPI_LAYER_WIDGET;
   if (layer <= ELM_OBJECT_LAYER_TOOLTIP) return ATSPI_LAYER_POPUP;

   return ATSPI_LAYER_OVERLAY;
}

static Eldbus_Message *
_component_get_layer(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int layer = 0;
   Eldbus_Message *ret;
   AtspiComponentLayer atspi_layer;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   eo_do(obj, layer = elm_interface_atspi_component_layer_get());

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   atspi_layer = _elm_layer_2_atspi_layer(layer);
   eldbus_message_arguments_append(ret, "u", atspi_layer);

   return ret;
}

static Eldbus_Message *
_component_grab_focus(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eina_Bool focus = EINA_FALSE;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   eo_do(obj, focus = elm_interface_atspi_component_focus_grab());

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "b", focus);

   return ret;
}

static Eldbus_Message *
_component_get_alpha(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   double alpha = 0;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   eo_do(obj, alpha = elm_interface_atspi_component_alpha_get());

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "d", alpha);

   return ret;
}

static Eldbus_Message *
_component_set_extends(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y, w, h;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;
   Eina_Bool result = EINA_FALSE;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "iiiiu", &x, &y, &w, &h, &coord_type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   Eina_Bool type = coord_type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, result = elm_interface_atspi_component_extents_set(type, x, y, w, h));

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_component_set_position(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eina_Bool result = EINA_FALSE;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "iiu", &x, &y, &coord_type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   Eina_Bool type = coord_type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, result = elm_interface_atspi_component_position_set(type, x, y));

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_component_set_size(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_message_path_get(msg);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int w, h;
   Eina_Bool result;
   Eldbus_Message *ret;

   ELM_ATSPI_OBJ_CHECK_OR_RETURN_DBUS_ERROR(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN, msg);

   if (!eldbus_message_arguments_get(msg, "ii", &w, &h))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   eo_do(obj, result = elm_interface_atspi_component_size_set(w, h));

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static const Eldbus_Method component_methods[] = {
   { "Contains", ELDBUS_ARGS({"i", "x"}, {"i", "y"}, {"u", "coord_type"}), ELDBUS_ARGS({"b", "contains"}), _component_contains, 0 },
   { "GetAccessibleAtPoint", ELDBUS_ARGS({"i", "x"}, {"i", "y"}, {"u", "coord_type"}), ELDBUS_ARGS({"(so)", "accessible"}), _component_get_accessible_at_point, 0 },
   { "GetExtents", ELDBUS_ARGS({"u", "coord_type"}), ELDBUS_ARGS({"(iiii)", "extents"}), _component_get_extents, 0 },
   { "GetPosition", ELDBUS_ARGS({"u", "coord_type"}), ELDBUS_ARGS({"i", "x"}, {"i","y"}), _component_get_position, 0 },
   { "GetSize", NULL, ELDBUS_ARGS({"i", "w"}, {"i", "h"}), _component_get_size, 0 },
   { "GetLayer", NULL, ELDBUS_ARGS({"u", "layer"}), _component_get_layer, 0 },
//   { "GetMDIZOrder", NULL, ELDBUS_ARGS({"n", "MDIZOrder"}), _component_get_mdizorder, 0 },
   { "GrabFocus", NULL, ELDBUS_ARGS({"b", "focus"}), _component_grab_focus, 0 },
   { "GetAlpha", NULL, ELDBUS_ARGS({"d", "alpha"}), _component_get_alpha, 0 },
   { "SetExtents", ELDBUS_ARGS({"i", "x"}, {"i", "y"}, {"i", "width"}, {"i", "height"}, {"u", "coord_type"}), ELDBUS_ARGS({"b", "result"}), _component_set_extends, 0 },
   { "SetPosition", ELDBUS_ARGS({"i", "x"}, {"i", "y"}, {"u", "coord_type"}), ELDBUS_ARGS({"b", "result"}), _component_set_position, 0 },
   { "SetSize", ELDBUS_ARGS({"i", "width"}, {"i", "height"}), ELDBUS_ARGS({"b", "result"}), _component_set_size, 0 },
   { NULL, NULL, NULL, NULL, 0 }
};

static const Eldbus_Service_Interface_Desc component_iface_desc = {
   ATSPI_DBUS_INTERFACE_COMPONENT, component_methods, NULL, NULL, NULL, NULL
};

static void
_on_elm_atspi_bridge_app_register(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   const char *errname, *errmsg;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
        return;
     }
   DBG("Application successfuly registered at ATSPI2 bus.");
}

EAPI Eina_Bool
_elm_atspi_bridge_app_register(Eo *bridge)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, EINA_FALSE);

   Eldbus_Message *message = eldbus_message_method_call_new(ATSPI_DBUS_NAME_REGISTRY,
                                    ATSPI_DBUS_PATH_ROOT,
                                    ATSPI_DBUS_INTERFACE_SOCKET,
                                    "Embed");
   Eldbus_Message_Iter *iter = eldbus_message_iter_get(message);

   _bridge_iter_object_reference_append(bridge, iter, elm_atspi_bridge_root_get(bridge));
   eldbus_connection_send(pd->a11y_bus, message, _on_elm_atspi_bridge_app_register, NULL, -1);

   return EINA_TRUE;
}

EAPI Eina_Bool
_elm_atspi_bridge_app_unregister(Eo *bridge)
{
   Eo *root;
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, EINA_FALSE);

   root = elm_atspi_bridge_root_get(bridge);

   Eldbus_Message *message = eldbus_message_method_call_new(ATSPI_DBUS_NAME_REGISTRY,
                                    ATSPI_DBUS_PATH_ROOT,
                                    ATSPI_DBUS_INTERFACE_SOCKET,
                                    "Unembed");
   Eldbus_Message_Iter *iter = eldbus_message_iter_get(message);

   _bridge_iter_object_reference_append(bridge, iter, root);
   eldbus_connection_send(pd->a11y_bus, message, NULL, NULL, -1);

   return EINA_TRUE;
}

static void
_cache_register(Eo *obj)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(obj, pd);
   pd->cache_interface = eldbus_service_interface_register(pd->a11y_bus, CACHE_INTERFACE_PATH, &cache_iface_desc);
   eldbus_service_object_data_set(pd->cache_interface, ELM_ATSPI_BRIDGE_CLASS_NAME, obj);
}

static void
_set_broadcast_flag(const char *event, Eo *bridge)
{
   char **tokens;
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   tokens = eina_str_split(event, ":", 3);

   if (!tokens) return;

   if (!strcmp(tokens[0], "Object"))
     {
        if (!tokens[1] || *tokens[1] == '\0') return; // do not handle "Object:*"
        else if (!strcmp(tokens[1], "StateChanged"))
          {
             if (!tokens[2] || *tokens[2] == '\0')
               pd->object_state_broadcast_mask = -1; // broadcast all
             eina_str_tolower(&tokens[2]);
             struct atspi_state_desc *sd = eina_hash_find(pd->state_hash, tokens[2]);
             if (sd)
               STATE_TYPE_SET(pd->object_state_broadcast_mask, sd->elm_state);
          }
        else if (!strcmp(tokens[1], "PropertyChange"))
          {
             if (!tokens[2] || *tokens[2] == '\0')
               pd->object_property_broadcast_mask = -1; //broadcast all
             else if (!strcmp(tokens[2], "AccessibleValue"))
               STATE_TYPE_SET(pd->object_property_broadcast_mask, ATSPI_OBJECT_PROPERTY_VALUE);
             else if (!strcmp(tokens[2], "AccessibleName"))
               STATE_TYPE_SET(pd->object_property_broadcast_mask, ATSPI_OBJECT_PROPERTY_NAME);
             else if (!strcmp(tokens[2], "AccessibleDescription"))
               STATE_TYPE_SET(pd->object_property_broadcast_mask, ATSPI_OBJECT_PROPERTY_DESCRIPTION);
             else if (!strcmp(tokens[2], "AccessibleParent"))
               STATE_TYPE_SET(pd->object_property_broadcast_mask, ATSPI_OBJECT_PROPERTY_PARENT);
             else if (!strcmp(tokens[2], "AccessibleRole"))
               STATE_TYPE_SET(pd->object_property_broadcast_mask, ATSPI_OBJECT_PROPERTY_ROLE);
          }
        else if (!strcmp(tokens[1], "ChildrenChanged"))
          {
             if (!tokens[2] || *tokens[2] == '\0')
               pd->object_children_broadcast_mask = -1; // broadcast all
             else if (!strcmp(tokens[2], "add"))
               STATE_TYPE_SET(pd->object_children_broadcast_mask, ATSPI_OBJECT_CHILD_ADDED);
             else if (!strcmp(tokens[2], "remove"))
               STATE_TYPE_SET(pd->object_children_broadcast_mask, ATSPI_OBJECT_CHILD_REMOVED);
          }
        else if (!strcmp(tokens[1], "TextChanged"))
          STATE_TYPE_SET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CHANGED);
        else if (!strcmp(tokens[1], "TextCaretMoved"))
          STATE_TYPE_SET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CARET_MOVED);
        else if (!strcmp(tokens[1], "TextBoundsChanged"))
          STATE_TYPE_SET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_BOUNDS_CHANGED);
        else if (!strcmp(tokens[1], "TextSelectionChanged"))
          STATE_TYPE_SET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_SELECTION_CHANGED);
        else if (!strcmp(tokens[1], "TextAttributesChanged"))
          STATE_TYPE_SET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_ATTRIBUTES_CHANGED);
        else if (!strcmp(tokens[1], "VisibleDataChanged"))
          STATE_TYPE_SET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_VISIBLE_DATA_CHANGED);
        else if (!strcmp(tokens[1], "ActiveDescendantChanged"))
          STATE_TYPE_SET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_ACTIVE_DESCENDANT_CHANGED);
     }
   else if (!strcmp(tokens[0], "Window"))
     {
        if (!tokens[1] || *tokens[1] == '\0')
          pd->window_signal_broadcast_mask = -1; // broadcast all
        else if (!strcmp(tokens[1], "Create"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_CREATE);
        else if (!strcmp(tokens[1], "Destroy"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_DESTROY);
        else if (!strcmp(tokens[1], "Activate"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_ACTIVATE);
        else if (!strcmp(tokens[1], "Deactivate"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_DEACTIVATE);
        else if (!strcmp(tokens[1], "Maximize"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_MAXIMIZE);
        else if (!strcmp(tokens[1], "Minimize"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_MINIMIZE);
        else if (!strcmp(tokens[1], "Resize"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_RESIZE);
        else if (!strcmp(tokens[1], "Restore"))
          STATE_TYPE_SET(pd->window_signal_broadcast_mask, ATSPI_WINDOW_EVENT_RESTORE);
     }

   free(tokens[0]);
   free(tokens);
}

static void
_registered_listeners_get(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending)
{
   const char *event, *bus;
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(data, pd);
   pd->pending_requests = eina_list_remove(pd->pending_requests, pending);

   DBG("Updating registered ATSPI signals list.");
   pd->object_broadcast_mask = 0;
   pd->object_children_broadcast_mask = 0;
   pd->object_property_broadcast_mask = 0;
   pd->object_state_broadcast_mask = 0;
   pd->window_signal_broadcast_mask = 0;

   if (eldbus_message_error_get(msg, &event, &bus))
     {
        WRN("%s %s", event, bus);
        return;
     }
   Eldbus_Message_Iter *iter, *siter;
   if (!eldbus_message_arguments_get(msg, "a(ss)", &iter))
     {
        ERR("Invalid answer type from GetRegisteredEvents method call!");
        return;
     }
   while (eldbus_message_iter_get_and_next(iter, 'r', &siter))
     {
        eldbus_message_iter_arguments_get(siter, "ss", &bus, &event);
        _set_broadcast_flag(event, data);
     }

   if (!pd->connected)
      eo_do(data, eo_event_callback_call(ELM_ATSPI_BRIDGE_EVENT_CONNECTED, NULL));
   pd->connected = EINA_TRUE;
}

static void
_registered_events_list_update(Eo *bridge)
{
   Eldbus_Message *msg;
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);
   Eldbus_Pending *p;

   msg = eldbus_message_method_call_new(ATSPI_DBUS_NAME_REGISTRY, ATSPI_DBUS_PATH_REGISTRY, ATSPI_DBUS_INTERFACE_REGISTRY, "GetRegisteredEvents");
   p = eldbus_connection_send(pd->a11y_bus, msg, _registered_listeners_get, bridge, -1);
   pd->pending_requests = eina_list_append(pd->pending_requests, p);
}

static void
_handle_listener_change(void *data, const Eldbus_Message *msg EINA_UNUSED)
{
   _registered_events_list_update(data);
}

static Eina_Bool
_state_changed_signal_send(void *data, Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   Elm_Atspi_Event_State_Changed_Data *state_data = event_info;
   const char *type_desc;
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_state_broadcast_mask, state_data->type))
     return EINA_FALSE;

   if ((state_data->type > ELM_ATSPI_STATE_LAST_DEFINED) ||
        (int)state_data->type < 0)
     return EINA_FALSE;

   type_desc = elm_states_to_atspi_state[state_data->type].name;

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_STATE_CHANGED], type_desc, state_data->new_value, 0, NULL);
   return EINA_TRUE;
}

static Eina_Bool
_property_changed_signal_send(void *data, Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   const char *property = event_info;
   char *atspi_desc;
   enum _Atspi_Object_Property prop = ATSPI_OBJECT_PROPERTY_LAST;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);

   if (!strcmp(property, "parent"))
     {
        prop = ATSPI_OBJECT_PROPERTY_PARENT;
        atspi_desc = "accessible-parent";
     }
   else if (!strcmp(property, "name"))
     {
        prop = ATSPI_OBJECT_PROPERTY_NAME;
        atspi_desc = "accessible-name";
     }
   else if (!strcmp(property, "description"))
     {
        prop = ATSPI_OBJECT_PROPERTY_DESCRIPTION;
        atspi_desc = "accessible-description";
     }
   else if (!strcmp(property, "role"))
     {
        prop = ATSPI_OBJECT_PROPERTY_ROLE;
        atspi_desc = "accessible-role";
     }
   else if (!strcmp(property, "value"))
     {
        prop = ATSPI_OBJECT_PROPERTY_VALUE;
        atspi_desc = "accessible-value";
     }
   if (prop == ATSPI_OBJECT_PROPERTY_LAST)
     {
        ERR("Unrecognized property name!");
        return EINA_FALSE;
     }
   if (!STATE_TYPE_GET(pd->object_property_broadcast_mask, prop))
     {
        DBG("Masking property %s changed event.", property);
        return EINA_FALSE;
     }

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_PROPERTY_CHANGED], atspi_desc, 0, 0, NULL, NULL);
   return EINA_TRUE;
}

static Eina_Bool
_visible_data_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_children_broadcast_mask, ATSPI_OBJECT_EVENT_VISIBLE_DATA_CHANGED))
     return EINA_FALSE;

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_VISIBLE_DATA_CHANGED], "",
                       0, 0, NULL, NULL);

   return EINA_TRUE;
}

static Eina_Bool
_active_descendant_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   Eo *child = event_info;
   int idx;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_children_broadcast_mask, ATSPI_OBJECT_EVENT_ACTIVE_DESCENDANT_CHANGED))
     return EINA_FALSE;

   eo_do(child, idx = elm_interface_atspi_accessible_index_in_parent_get());

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_ACTIVE_DESCENDANT_CHANGED], "",
                       idx, 0, "(so)", eldbus_connection_unique_name_get(pd->a11y_bus), child);
   return EINA_TRUE;
}

static Eina_Bool
_children_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   const char *atspi_desc = NULL;
   Elm_Atspi_Event_Children_Changed_Data *ev_data = event_info;
   int idx;
   enum _Atspi_Object_Child_Event_Type type;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);

   type = ev_data->is_added ? ATSPI_OBJECT_CHILD_ADDED : ATSPI_OBJECT_CHILD_REMOVED;

   if (!STATE_TYPE_GET(pd->object_children_broadcast_mask, type))
     return EINA_FALSE;

   switch (type)
    {
     case ATSPI_OBJECT_CHILD_ADDED:
        atspi_desc = "add";
        eo_do(ev_data->child, idx = elm_interface_atspi_accessible_index_in_parent_get());
        break;
     case ATSPI_OBJECT_CHILD_REMOVED:
        atspi_desc = "remove";
        eo_do(ev_data->child, idx = elm_interface_atspi_accessible_index_in_parent_get());
        break;
    }

   if (!atspi_desc) return EINA_FALSE;

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_CHILDREN_CHANGED], atspi_desc,
                       idx, 0, "(so)", eldbus_connection_unique_name_get(pd->a11y_bus), ev_data->child);

   return EINA_TRUE;
}

static Eina_Bool
_window_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info EINA_UNUSED)
{
   enum _Atspi_Window_Signals type;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);

   if (desc == ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_CREATED)
     type = ATSPI_WINDOW_EVENT_CREATE;
   else if (desc == ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_DESTROYED)
     type = ATSPI_WINDOW_EVENT_DESTROY;
   else if (desc == ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_DEACTIVATED)
     type = ATSPI_WINDOW_EVENT_DEACTIVATE;
   else if (desc == ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_ACTIVATED)
     type = ATSPI_WINDOW_EVENT_ACTIVATE;
   else if (desc == ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_MAXIMIZED)
     type = ATSPI_WINDOW_EVENT_MAXIMIZE;
   else if (desc == ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_MINIMIZED)
     type = ATSPI_WINDOW_EVENT_MINIMIZE;
   else if (desc == ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_RESTORED)
     type = ATSPI_WINDOW_EVENT_RESTORE;
   else
     return EINA_FALSE;

   if (!STATE_TYPE_GET(pd->window_signal_broadcast_mask, type))
     return EINA_FALSE;

   if (!pd->a11y_bus)
     {
        ERR("A11Y connection closed. Unable to send ATSPI event.");
        return EINA_FALSE;
     }

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_WINDOW,
                       &_window_obj_signals[type], "", 0, 0, "i", 0);
   return EINA_TRUE;
}

static Eina_Bool
_selection_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_SELECTION_CHANGED))
     return EINA_FALSE;

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_SELECTION_CHANGED], "", 0, 0, "i", 0);
   return EINA_TRUE;
}

static void _bridge_signal_send(Eo *bridge, Eo *obj, const char *infc, const Eldbus_Signal *signal, const char *minor, unsigned int det1, unsigned int det2, const char *variant_sig, ...)
{
   Eldbus_Message *msg;
   Eldbus_Message_Iter *iter , *iter_stack[64], *iter_struct;
   va_list va;
   Eo *atspi_obj;
   const char *path;
   int top = 0;

   EINA_SAFETY_ON_NULL_RETURN(infc);
   EINA_SAFETY_ON_NULL_RETURN(signal);
   EINA_SAFETY_ON_NULL_RETURN(minor);
   EINA_SAFETY_ON_NULL_RETURN(obj);
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   path = _bridge_path_from_object(bridge, obj);

   msg = eldbus_message_signal_new(path, infc, signal->name);
   if (!msg) return;

   va_start(va, variant_sig);

   iter = eldbus_message_iter_get(msg);
   eldbus_message_iter_arguments_append(iter, "sii", minor, det1, det2);

   if (variant_sig)
     {
        iter_stack[top] = eldbus_message_iter_container_new(iter, 'v', variant_sig);

        const char *tmp = variant_sig;
        while (*tmp)
          {
             switch (*tmp)
               {
                case '(':
                   iter_stack[top + 1] = eldbus_message_iter_container_new(iter_stack[top], 'r', NULL);
                   top++;
                   break;
                case 's':
                   eldbus_message_iter_basic_append(iter_stack[top], 's', va_arg(va, char*));
                   break;
                case 'i':
                   eldbus_message_iter_basic_append(iter_stack[top], 'i', va_arg(va, int));
                   break;
                case 'o':
                   atspi_obj = va_arg(va, Eo*);
                   path = _bridge_path_from_object(bridge, atspi_obj);
                   eldbus_message_iter_basic_append(iter_stack[top], 'o', path);
                   break;
                case ')':
                   eldbus_message_iter_container_close(iter_stack[top - 1], iter_stack[top]);
                   top--;
                   break;
                default:
                   ERR("Not supported d-bus type: %c.", *tmp);
                   break;
               }
             tmp++;
          }
     }
   else // AT-SPI implementation forces checks on variant in signature even if not used.
     {
        iter_stack[top] = eldbus_message_iter_container_new(iter, 'v', "i");
        eldbus_message_iter_basic_append(iter_stack[top], 'i', 0);
     }

   va_end(va);
   if (top != 0)
     ERR("Invalid d-bus signature: () do not match.");

   eldbus_message_iter_container_close(iter, iter_stack[0]);

   iter_struct = eldbus_message_iter_container_new(iter, 'r', NULL);
   path = _bridge_path_from_object(bridge, elm_atspi_bridge_root_get(bridge));
   eldbus_message_iter_basic_append(iter_struct, 's', eldbus_connection_unique_name_get(pd->a11y_bus));
   eldbus_message_iter_basic_append(iter_struct, 'o', path);
   eldbus_message_iter_container_close(iter, iter_struct);

   eldbus_connection_send(pd->a11y_bus, msg, NULL, NULL, -1);
   DBG("Send %s.%s[%s,%d,%d]", infc, signal->name, minor, det1, det2);
}

static Eina_Bool
_text_caret_moved_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   int cursor_pos = 0;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CARET_MOVED))
     return EINA_TRUE;

   eo_do(obj, cursor_pos = elm_interface_atspi_text_caret_offset_get());

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_TEXT_CARET_MOVED], "", cursor_pos, 0, NULL, NULL);

   return EINA_TRUE;
}

static Eina_Bool
_text_text_inserted_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   Elm_Atspi_Text_Change_Info *info = event_info;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CHANGED))
     return EINA_TRUE;

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_TEXT_CHANGED], "insert", info->pos, info->len, "s", info->content);

   return EINA_TRUE;
}

static Eina_Bool
_text_text_removed_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   Elm_Atspi_Text_Change_Info *info = event_info;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CHANGED))
     return EINA_TRUE;

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_TEXT_CHANGED], "delete", info->pos, info->len, "s", info->content);

   return EINA_TRUE;
}

static Eina_Bool
_text_selection_changed_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_SELECTION_CHANGED))
     return EINA_TRUE;

   _bridge_signal_send(data, obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT,
                       &_event_obj_signals[ATSPI_OBJECT_EVENT_TEXT_SELECTION_CHANGED], "", 0, 0, NULL, NULL);

   return EINA_TRUE;
}

static void
_event_handlers_register(Eo *bridge)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   _registered_events_list_update(bridge);

   // register signal handlers in order to update list of registered listeners of ATSPI-Clients
   pd->register_hdl = eldbus_signal_handler_add(pd->a11y_bus, ATSPI_DBUS_NAME_REGISTRY, ATSPI_DBUS_PATH_REGISTRY, ATSPI_DBUS_INTERFACE_REGISTRY, "EventListenerRegistered", _handle_listener_change, bridge);
   pd->unregister_hdl = eldbus_signal_handler_add(pd->a11y_bus, ATSPI_DBUS_NAME_REGISTRY, ATSPI_DBUS_PATH_REGISTRY, ATSPI_DBUS_INTERFACE_REGISTRY, "EventListenerDeregistered", _handle_listener_change, bridge);

   pd->key_flr = ecore_event_filter_add(NULL, _elm_atspi_bridge_key_filter, NULL, bridge);
}

static void
_bridge_object_unregister(Eo *bridge, Eo *obj)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   eina_hash_del(pd->cache, &obj, obj);
}

static Eina_Bool
_on_object_add(void *data, Eo *obj, const Eo_Event_Description *event EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Eldbus_Message *sig;
   Eldbus_Message_Iter *iter;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);

   sig = eldbus_service_signal_new(pd->cache_interface, ATSPI_OBJECT_CHILD_ADDED);
   iter = eldbus_message_iter_get(sig);
   _cache_item_reference_append_cb(data, obj, iter);

   eldbus_service_signal_send(pd->cache_interface, sig);

   return EINA_TRUE;
}

static Eina_Bool
_on_object_del(void *data, Eo *obj, const Eo_Event_Description *event EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Eldbus_Message *sig;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);

   _bridge_object_unregister(data, obj);

   sig = eldbus_service_signal_new(pd->cache_interface, ATSPI_OBJECT_CHILD_REMOVED);
   Eldbus_Message_Iter *iter = eldbus_message_iter_get(sig);
   _bridge_iter_object_reference_append(data, iter, obj);
   eldbus_service_signal_send(pd->cache_interface, sig);

   return EINA_TRUE;
}

static void
_interfaces_unregister(Eo *bridge)
{
    ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

#define INTERFACE_SAFE_FREE(ifc) \
   if (ifc) \
      eldbus_service_interface_unregister(ifc); \
   ifc = NULL;

   INTERFACE_SAFE_FREE(pd->interfaces.accessible);
   INTERFACE_SAFE_FREE(pd->interfaces.application);
   INTERFACE_SAFE_FREE(pd->interfaces.action);
   INTERFACE_SAFE_FREE(pd->interfaces.component);
   INTERFACE_SAFE_FREE(pd->interfaces.collection);
   INTERFACE_SAFE_FREE(pd->interfaces.editable_text);
   INTERFACE_SAFE_FREE(pd->interfaces.image);
   INTERFACE_SAFE_FREE(pd->interfaces.selection);
   INTERFACE_SAFE_FREE(pd->interfaces.text);
   INTERFACE_SAFE_FREE(pd->interfaces.value);
}

static void
_a11y_connection_shutdown(Eo *bridge)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);
   Eldbus_Pending *pending;

   if (pd->connected)
      _elm_atspi_bridge_app_unregister(bridge);

   if (pd->cache)
     eina_hash_free(pd->cache);
   pd->cache = NULL;

   if (pd->cache_interface)
     eldbus_service_object_unregister(pd->cache_interface);
   pd->cache_interface = NULL;

   _interfaces_unregister(bridge);

   if (pd->key_flr) ecore_event_filter_del(pd->key_flr);
   pd->key_flr = NULL;

   if (pd->register_hdl) eldbus_signal_handler_del(pd->register_hdl);
   pd->register_hdl = NULL;

   if (pd->unregister_hdl) eldbus_signal_handler_del(pd->unregister_hdl);
   pd->unregister_hdl = NULL;

   EINA_LIST_FREE(pd->pending_requests, pending)
      eldbus_pending_cancel(pending);
   pd->pending_requests = NULL;

   if (pd->a11y_bus) eldbus_connection_unref(pd->a11y_bus);
   pd->a11y_bus = NULL;

   if (pd->state_hash) eina_hash_free(pd->state_hash);
   pd->state_hash = NULL;

   if (pd->event_hash) eina_hash_free(pd->event_hash);
   pd->event_hash = NULL;

   eo_do(ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, elm_interface_atspi_accessible_event_handler_del(pd->event_hdlr));
   pd->event_hdlr = NULL;

   eo_do(bridge, eo_event_callback_call(ELM_ATSPI_BRIDGE_EVENT_DISCONNECTED, NULL));
   pd->connected = EINA_FALSE;
}

static void _disconnect_cb(void *data, Eldbus_Connection *conn EINA_UNUSED, void *event_info EINA_UNUSED)
{
   _a11y_connection_shutdown(data);
}

static void
_interfaces_register(Eo *bridge)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   pd->interfaces.accessible =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &accessible_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.accessible, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.application =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &application_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.application, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.action =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &action_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.action, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.component =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &component_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.component, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.collection =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &collection_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.collection, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.editable_text =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &editable_text_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.editable_text, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.image =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &image_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.image, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.selection =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &selection_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.selection, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.text =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &text_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.text, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   pd->interfaces.value =
      eldbus_service_interface_fallback_register(pd->a11y_bus, ELM_ACCESS_OBJECT_PATH_PREFIX2, &value_iface_desc);
   eldbus_service_object_data_set(pd->interfaces.value, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
}

static Eina_Bool
_bridge_accessible_event_dispatch(void *data, Eo *accessible, const Eo_Event_Description *desc, void *event_info)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);

   _bridge_object_register(data, accessible);

   Eo_Event_Cb cb = eina_hash_find(pd->event_hash, &desc);
   return cb ? cb(data, accessible, desc, event_info) : EINA_TRUE;
}

static void
_a11y_bus_initialize(Eo *obj, const char *socket_addr)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(obj, pd);

   pd->a11y_bus = eldbus_private_address_connection_get(socket_addr);
   if (!pd->a11y_bus)
     return;

   eldbus_connection_event_callback_add(pd->a11y_bus, ELDBUS_CONNECTION_EVENT_DISCONNECTED, _disconnect_cb, obj);

   // init data structures
   pd->cache = eina_hash_pointer_new(NULL);
   pd->state_hash = _elm_atspi_state_hash_build();
   pd->event_hash = _elm_atspi_event_hash_build();

   // dbus init
   _cache_register(obj);
   _interfaces_register(obj);
   _event_handlers_register(obj);
   _elm_atspi_bridge_app_register(obj);

   // register accesible object event listener
   eo_do(ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN, pd->event_hdlr = elm_interface_atspi_accessible_event_handler_add(_bridge_accessible_event_dispatch, obj));
}

static void
_a11y_bus_address_get(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending)
{
   const char *errname, *errmsg, *sock_addr = NULL;
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(data, pd);

   pd->pending_requests = eina_list_remove(pd->pending_requests, pending);

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
        return;
     }

   if (!eldbus_message_arguments_get(msg, "s", &sock_addr) || !sock_addr)
     {
        ERR("Could not get A11Y Bus socket address.");
        return;
     }

   _a11y_bus_initialize((Eo*)data, sock_addr);
}

static void _a11y_connection_init(Eo *bridge)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);
   Eina_Bool is_connected;

   eo_do(bridge, is_connected = elm_obj_atspi_bridge_connected_get());

   if (is_connected) return;

   Eldbus_Message *m = eldbus_object_method_call_new(pd->bus_obj, A11Y_DBUS_INTERFACE, "GetAddress");
   Eldbus_Pending *p = eldbus_object_send(pd->bus_obj, m, _a11y_bus_address_get, bridge, 100);

   if (p)
      pd->pending_requests = eina_list_append(pd->pending_requests, p);
}

static void
_screen_reader_enabled_get(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(data, pd);
   const char *errname, *errmsg;
   Eina_Bool is_enabled;
   Eldbus_Message_Iter *variant;

   pd->pending_requests = eina_list_remove(pd->pending_requests, pending);

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        WRN("%s %s", errname, errmsg);
        return;
     }
   if (!eldbus_message_arguments_get(msg, "v", &variant))
     {
        ERR("'ScreenReaderEnabled' not packed into variant.");
        return;
     }
   if (!eldbus_message_iter_arguments_get(variant, "b", &is_enabled))
     {
        ERR("Could not get 'ScreenReaderEnabled' boolean property");
        return;
     }

   if (is_enabled)
     _a11y_connection_init(data);
   else
     DBG("AT-SPI2 stack not enabled.");
}

static void _bridge_object_register(Eo *bridge, Eo *obj)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   if (!eo_isa(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN))
     {
        WRN("Unable to register class w/o Elm_Interface_Atspi_Accessible!");
        return;
     }

   if (eina_hash_find(pd->cache, &obj))
        return;

   eina_hash_add(pd->cache, &obj, obj);
}

void
_elm_atspi_bridge_init(void)
{
   if (!_init_count)
     {
        _instance = eo_add(ELM_ATSPI_BRIDGE_CLASS, NULL);
        _init_count = 1;
     }
}

EAPI Eo*
_elm_atspi_bridge_get(void)
{
   return _instance;
}

void
_elm_atspi_bridge_shutdown(void)
{
   if (_init_count)
     {
        eo_del(_instance);
        _init_count = 0;
     }
}

static Key_Event_Info*
_key_event_info_new(int event_type, const Ecore_Event_Key *data, Eo *bridge)
{
   Key_Event_Info *ret;
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);

   ret = calloc(sizeof(Key_Event_Info), 1);

   ret->type = event_type;
   ret->event = *data;
   ret->bridge = bridge;

   ret->event.keyname = eina_stringshare_add(data->keyname);
   ret->event.key = eina_stringshare_add(data->key);
   ret->event.string = eina_stringshare_add(data->string);
   ret->event.compose = eina_stringshare_add(data->compose);

   // not sure why it is here, but explicite keep it NULLed.
   ret->event.data = NULL;

   return ret;
}

static void
_key_event_info_free(Key_Event_Info *data)
{
   EINA_SAFETY_ON_NULL_RETURN(data);

   eina_stringshare_del(data->event.keyname);
   eina_stringshare_del(data->event.key);
   eina_stringshare_del(data->event.string);
   eina_stringshare_del(data->event.compose);

   free(data);
}

static void
_iter_marshall_key_event(Eldbus_Message_Iter *iter, Key_Event_Info *data)
{
   Eldbus_Message_Iter *struct_iter;
   EINA_SAFETY_ON_NULL_RETURN(data);

   struct_iter = eldbus_message_iter_container_new(iter, 'r', NULL);

   const char *str = data->event.keyname ? data->event.keyname : "";
   int is_text = data->event.keyname ? 1 : 0;
   int type;
   if (data->type == ECORE_EVENT_KEY_DOWN)
     type = ATSPI_KEY_PRESSED_EVENT;
   else
     type = ATSPI_KEY_RELEASED_EVENT;

   eldbus_message_iter_arguments_append(struct_iter, "uiiiisb", type, 0, data->event.keycode, 0, data->event.timestamp, str, is_text);
   eldbus_message_iter_container_close(iter, struct_iter);
}

static void
_on_event_del(void *user_data, void *func_data EINA_UNUSED)
{
   Key_Event_Info *info = user_data;
   _key_event_info_free(info);
}

static void
_on_listener_answer(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   Key_Event_Info *info = data;
   const char *errname, *errmsg;
   Eina_Bool ret = EINA_TRUE;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(info->bridge, pd);

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
        goto reemit;
     }
   if (!eldbus_message_arguments_get(msg, "b", &ret))
     {
        ERR("Return message doen not contian return value");
        goto reemit;
     }
   if (ret)
     {
        _key_event_info_free(info);
        return;
     }
reemit:
   ecore_event_add(info->type, &info->event, _on_event_del, info);
   pd->reemited_events = eina_list_append(pd->reemited_events, &info->event);
}

static Eina_Bool
_elm_atspi_bridge_key_filter(void *data, void *loop EINA_UNUSED, int type, void *event)
{
   Eldbus_Message *msg;
   Eldbus_Message_Iter *iter;
   Ecore_Event_Key *key_event = event;
   Key_Event_Info *ke;
   Eo *bridge = data;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, EINA_TRUE);

   if ((type != ECORE_EVENT_KEY_DOWN) && (type != ECORE_EVENT_KEY_UP)) return EINA_TRUE;

   // check if reemited
   if (eina_list_data_find(pd->reemited_events, event))
     {
        pd->reemited_events = eina_list_remove(pd->reemited_events, event);
        return EINA_TRUE;
     }

   ke = _key_event_info_new(type, key_event, bridge);
   if (!ke) return EINA_TRUE;

   msg = eldbus_message_method_call_new(ATSPI_DBUS_NAME_REGISTRY, ATSPI_DBUS_PATH_DEC,
                                        ATSPI_DBUS_INTERFACE_DEC, "NotifyListenersSync");
   iter = eldbus_message_iter_get(msg);
   _iter_marshall_key_event(iter, ke);

   // timeout should be kept reasonaby low to avoid delays
   if (!eldbus_connection_send(pd->a11y_bus, msg, _on_listener_answer, ke, 100))
     return EINA_TRUE;

   return EINA_FALSE;
}

EOLIAN Eina_Bool
_elm_atspi_bridge_connected_get(Eo *obj EINA_UNUSED, Elm_Atspi_Bridge_Data *pd)
{
   return pd->connected;
}

EOLIAN Eo*
_elm_atspi_bridge_root_get(Eo *obj EINA_UNUSED, Elm_Atspi_Bridge_Data *pd)
{
   if (!pd->root)
     {
        pd->root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);
        elm_interface_atspi_accessible_added(pd->root);
     }

   return pd->root;
}

static void
_properties_changed_cb(void *data, Eldbus_Proxy *proxy EINA_UNUSED, void *event)
{
   Eldbus_Proxy_Event_Property_Changed *ev = event;
   Eo *bridge = data;
   Eina_Bool val;
   const char *ifc = eldbus_proxy_interface_get(ev->proxy);
   if (ev->name && !strcmp(ev->name, "ScreenReaderEnabled" ) &&
       ifc && !strcmp(A11Y_DBUS_STATUS_INTERFACE, ifc))
     {
        if (!eina_value_get(ev->value, &val))
          {
             ERR("Unable to get ScreenReaderEnabled property value");
             return;
          }
        if (val)
          _a11y_connection_init(bridge);
        else
          _a11y_connection_shutdown(bridge);
     }
}

EOLIAN Eo_Base*
_elm_atspi_bridge_eo_base_constructor(Eo *obj, Elm_Atspi_Bridge_Data *pd)
{
   Eldbus_Proxy *proxy;
   Eldbus_Pending *req;

   eo_do_super(obj, ELM_ATSPI_BRIDGE_CLASS, eo_constructor());

   elm_need_eldbus();

   if (!(pd->session_bus = eldbus_connection_get(ELDBUS_CONNECTION_TYPE_SESSION)))
     {
        ERR("Unable to connect to Session Bus");
        return NULL;
     }
   if (!(pd->bus_obj = eldbus_object_get(pd->session_bus, A11Y_DBUS_NAME, A11Y_DBUS_PATH)))
     {
        ERR("Could not get /org/a11y/bus object");
        goto obj_err;
     }
   if (!(proxy = eldbus_proxy_get(pd->bus_obj, A11Y_DBUS_STATUS_INTERFACE)))
     {
        ERR("Could not get proxy object for %s interface", A11Y_DBUS_STATUS_INTERFACE);
        goto proxy_err;
     }
   if (!(req = eldbus_proxy_property_get(proxy, "ScreenReaderEnabled", _screen_reader_enabled_get, obj)))
     {
        ERR("Could not send PropertyGet request");
        goto proxy_err;
     }
   pd->pending_requests = eina_list_append(pd->pending_requests, req);

   eldbus_proxy_properties_monitor(proxy, EINA_TRUE);
   eldbus_proxy_event_callback_add(proxy, ELDBUS_PROXY_EVENT_PROPERTY_CHANGED,
                                   _properties_changed_cb, obj);

   return obj;

proxy_err:
   eldbus_object_unref(pd->bus_obj);
   pd->bus_obj = NULL;
obj_err:
   eldbus_connection_unref(pd->session_bus);
   pd->session_bus = NULL;
   return NULL;
}

EOLIAN void
_elm_atspi_bridge_eo_base_destructor(Eo *obj, Elm_Atspi_Bridge_Data *pd)
{
   _a11y_connection_shutdown(obj);

   if (pd->bus_obj) eldbus_object_unref(pd->bus_obj);
   if (pd->session_bus) eldbus_connection_unref(pd->session_bus);
   if (pd->root) eo_del(pd->root);

   eo_do_super(obj, ELM_ATSPI_BRIDGE_CLASS, eo_destructor());
}

#include "elm_atspi_bridge.eo.c"
