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
#define ELM_ACCESS_OBJECT_REFERENCE_TEMPLATE ELM_ACCESS_OBJECT_PATH_PREFIX "%llu"

#define SIZE(x) sizeof(x)/sizeof(x[0])
#define ELM_ATSPI_BRIDGE_CLASS_NAME "__Elm_Atspi_Bridge"

#define ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(obj, sd) \
   Elm_Atspi_Bridge_Data *sd = eo_data_scope_get(obj, ELM_ATSPI_BRIDGE_CLASS); \
   if (!sd) return;

#define ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(obj, sd, val) \
   Elm_Atspi_Bridge_Data *sd = eo_data_scope_get(obj, ELM_ATSPI_BRIDGE_CLASS); \
   if (!sd) return val;
#define ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, key, ifc, val) \
   Eldbus_Service_Interface *ifc; \
   eo_do(obj, ifc = eo_key_data_get(key)); \
   if (!ifc) return val;


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

   Eina_Bool connected : 1;
} Elm_Atspi_Bridge_Data;

struct cache_closure {
     Eo *bridge;
     Eldbus_Message_Iter *iter;
};

static Eo *_instance;
static int _init_count = 0;

// Object Event handlers
static Eina_Bool _state_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _property_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info);
static Eina_Bool _children_changed_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info);
static Eina_Bool _window_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _selection_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_text_inserted_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_text_removed_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_caret_moved_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info);
static Eina_Bool _text_selection_changed_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info EINA_UNUSED);

// bridge private methods
static void _bridge_cache_build(Eo *bridge, void *obj);
static void _bridge_object_register(Eo *bridge, Eo *obj);
static void _bridge_object_unregister(Eo *bridge, Eo *obj);
static char * _bridge_path_from_object(Eo *bridge, const Eo *eo);
static void _bridge_signal_send(Eo *bridge, Eldbus_Service_Interface *infc, int signal, const char *minor, unsigned int det1, unsigned int det2, const char *variant_sig, ...);
static Eo * _bridge_object_from_path(Eo *bridge, const char *path);
static void _bridge_iter_object_reference_append(Eo *bridge, Eldbus_Message_Iter *iter, const Eo *obj);

// utility functions
static void _iter_interfaces_append(Eldbus_Message_Iter *iter, const Eo *obj);
static Eina_Bool _elm_atspi_bridge_key_filter(void *data, void *loop, int type, void *event);
static void _object_unregister(Eo *obj, void *data);
static void _object_desktop_reference_append(Eldbus_Message_Iter *iter);
static Eina_Bool _on_object_del(void *data, Eo *obj, const Eo_Event_Description *event EINA_UNUSED, void *event_info EINA_UNUSED);

EO_CALLBACKS_ARRAY_DEFINE(_events_cb,
   { EO_EV_DEL, _on_object_del},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_PROPERTY_CHANGED, _property_changed_signal_send},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_CHILDREN_CHANGED, _children_changed_signal_send},
   { ELM_INTERFACE_ATSPI_ACCESSIBLE_EVENT_STATE_CHANGED, _state_changed_signal_send}
);

EO_CALLBACKS_ARRAY_DEFINE(_window_cb,
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_CREATED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_DESTROYED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_ACTIVATED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_DEACTIVATED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_MAXIMIZED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_MINIMIZED, _window_signal_send},
   { ELM_INTERFACE_ATSPI_WINDOW_EVENT_WINDOW_RESTORED, _window_signal_send}
);

EO_CALLBACKS_ARRAY_DEFINE(_selection_cb,
   { ELM_INTERFACE_ATSPI_SELECTION_EVENT_SELECTION_CHANGED, _selection_signal_send}
);

EO_CALLBACKS_ARRAY_DEFINE(_text_cb,
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_CARET_MOVED, _text_caret_moved_send },
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_INSERTED, _text_text_inserted_send },
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_REMOVED, _text_text_removed_send },
   { ELM_INTERFACE_ATSPI_TEXT_EVENT_ACCESS_TEXT_SELECTION_CHANGED, _text_selection_changed_send }
);

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
   [ATSPI_OBJECT_EVENT_ACTIVE_DESCENDANT_CHANGED] = {"ActiveDescendantsChanged", ELDBUS_ARGS({"siiv(so)", NULL}), 0},
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   AtspiRole atspi_role = ATSPI_ROLE_INVALID;
   Elm_Atspi_Role role;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *role_name = NULL, *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   eo_do(obj, role_name = elm_interface_atspi_accessible_role_name_get());

   Eldbus_Message *ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);
   eldbus_message_arguments_append(ret, "s", role_name);

   return ret;
}

static Eldbus_Message *
_accessible_get_localized_role_name(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *l_role_name = NULL, *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eina_List *children_list = NULL, *l;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eo *children;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   eo_do(obj, children_list = elm_interface_atspi_accessible_children_get());

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "(so)");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   EINA_LIST_FOREACH(children_list, l, children)
      _bridge_iter_object_reference_append(bridge, iter_array, children);

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
   Eldbus_Message *ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);

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

   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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

static Eina_Hash*
_elm_atspi_state_hash_build(void)
{
   Eina_Hash *ret = eina_hash_string_superfast_new(NULL);
   unsigned int i = 0;

   for (i = 0; i < SIZE(elm_states_to_atspi_state); i++)
     eina_hash_add(ret, elm_states_to_atspi_state[i].name, &elm_states_to_atspi_state[i]);

   return ret;
}

static Eldbus_Message *
_accessible_get_state(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Elm_Atspi_State_Set states;
   uint64_t atspi_states = 0;

   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   if (!obj)
     {
        ERR("Atspi Object %s not found in cache!", obj_path);
        return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid object path.");
     }

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int idx = -1;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, idx = elm_interface_atspi_accessible_index_in_parent_get());

   eldbus_message_arguments_append(ret, "i", idx);

   return ret;
}

static Eldbus_Message *
_accessible_child_at_index(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eo *child = NULL;
   Eina_List *children = NULL;
   int idx;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   eo_do(obj, children = elm_interface_atspi_accessible_children_get());

   child = eina_list_nth(children, idx);
   _bridge_iter_object_reference_append(bridge, iter, child);
   eina_list_free(children);

   return ret;
}

static Eldbus_Message *
_accessible_get_relation_set(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret = NULL;
   Eldbus_Message_Iter *iter = NULL, *iter_array = NULL, *iter_array2 = NULL, *iter_struct;
   Elm_Atspi_Relation *rel;
   Eina_List *rels;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   iter_array = eldbus_message_iter_container_new(iter, 'a', "(ua(so))");
   EINA_SAFETY_ON_NULL_GOTO(iter_array, fail);

   eo_do(obj, rels = elm_interface_atspi_accessible_relation_set_get());

   EINA_LIST_FREE(rels, rel)
     {
        iter_struct = eldbus_message_iter_container_new(iter_array, 'r', NULL);
        eldbus_message_iter_basic_append(iter_struct, 'u', _elm_relation_to_atspi_relation(rel->type));
        iter_array2 = eldbus_message_iter_container_new(iter_struct, 'a', "(so)");
        EINA_SAFETY_ON_NULL_GOTO(iter_array2, fail);
        _bridge_iter_object_reference_append(bridge, iter_array2, rel->obj);
        eldbus_message_iter_container_close(iter_struct, iter_array2);
        eldbus_message_iter_container_close(iter_array, iter_struct);
        free(rel);
     }
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eo *child = NULL;

   int idx;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   if (!eldbus_message_arguments_get(msg, "i", &idx))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   eo_do(obj, child = elm_interface_atspi_selection_selected_child_get(idx));

   _bridge_iter_object_reference_append(bridge, iter, child);

   return ret;
}

static Eldbus_Message *
_selection_child_select(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eina_Bool result;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_all_children_select());
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_selection_clear(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eina_Bool result;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, result = elm_interface_atspi_selection_clear());
   eldbus_message_arguments_append(ret, "b", result);

   return ret;
}

static Eldbus_Message *
_selection_child_deselect(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;
   
   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *description, *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *name, *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

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
   const char *name, *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   char *key;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;

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
   const char *action, *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eina_List *actions;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int idx;
   Eldbus_Message *ret;
   Eina_Bool result;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   int x, y, w, h;
   Eina_Bool screen_coords;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   x = y = w = h = -1;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x = -1, y = -1;
   Eina_Bool screen_coords;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int w = -1, h = -1;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   char *str;
   Elm_Atspi_Text_Granularity gran;
   int start, end;
   Eldbus_Message *ret;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   char *str;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset;
   Eldbus_Message *ret;
   Eina_Bool res;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset;
   Eldbus_Message *ret;
   Eina_Unicode res;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *name, *obj_path = eldbus_service_object_path_get(iface);
   char *value = NULL;
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eldbus_Message *ret;
   Eina_Bool res;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eina_List *attrs;
   Elm_Atspi_Text_Attribute *attr;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start = -1, end;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eina_List *attrs;
   Elm_Atspi_Text_Attribute *attr;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset;
   Eina_Rectangle rect;
   AtspiCoordType type;
   Eina_Bool screen_coords, res;
   Eldbus_Message *ret;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int offset, x, y;
   AtspiCoordType type;
   Eina_Bool screen_coords;
   Eldbus_Message *ret;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int n;
   Eldbus_Message *ret;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eo_do(obj, n = elm_interface_atspi_text_selections_count_get());

   eldbus_message_arguments_append(ret, "i", n);

   return ret;
}

static Eldbus_Message *
_text_selection_get(const Eldbus_Service_Interface *iface, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int sel_num, start, end;
   Eldbus_Message *ret;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eina_Bool res;
   Eldbus_Message *ret;

   if (!obj)
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.UnknownObject", "Path is not valid accessible object reference.");

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int sel_num;
   Eina_Bool res;
   Eldbus_Message *ret;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int sel_num, start, end;
   Eina_Bool res;
   Eldbus_Message *ret;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eina_Rectangle rect;
   Eina_Bool screen_coords, res;
   AtspiCoordType type;
   Eldbus_Message *ret;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);
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
   const char *obj_path = eldbus_service_object_path_get(iface);
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

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int start, end;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_array;
   Eina_List *attrs, *defaults;
   Elm_Atspi_Text_Attribute *attr;
   Eina_Bool incl_def;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   const char *content;
   Eldbus_Message *ret;
   Eina_Bool res;

   if (!obj)
     return _dbus_invalid_ref_error_new(msg);
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   const char *text;
   Eldbus_Message *ret;
   int pos, len;
   Eina_Bool res;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int start, end;
   Eina_Bool res;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int start, end;
   Eina_Bool res;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int start, end;
   Eina_Bool res;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   int pos;
   Eina_Bool res;

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

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, NULL);

   if (!eina_hash_find(pd->cache, path))
     {
        WRN("Request for nonexisting object: %s", path);
       return NULL;
     }

   int len = strlen(ELM_ACCESS_OBJECT_PATH_PREFIX);

   if (strncmp(path, ELM_ACCESS_OBJECT_PATH_PREFIX, len))
     return NULL;

   tmp = path + len; /* Skip over the prefix */
   if (!strcmp(ELM_ACCESS_OBJECT_PATH_ROOT, tmp))
     return elm_atspi_bridge_root_get(bridge);

   sscanf(tmp, "%llu", &eo_ptr);
   eo = (Eo *) (uintptr_t) eo_ptr;
   ret = eo_isa(eo, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN) ? eo : NULL;
   return ret;
}

static char *
_bridge_path_from_object(Eo *bridge, const Eo *eo)
{
   char path[256];

   EINA_SAFETY_ON_NULL_RETURN_VAL(eo, strdup(ATSPI_DBUS_PATH_NULL));

   if (eo == elm_atspi_bridge_root_get(bridge))
     snprintf(path, sizeof(path), "%s%s", ELM_ACCESS_OBJECT_PATH_PREFIX, ELM_ACCESS_OBJECT_PATH_ROOT);
   else
     snprintf(path, sizeof(path), ELM_ACCESS_OBJECT_REFERENCE_TEMPLATE, (unsigned long long)(uintptr_t)eo);
   return strdup(path);
}

static Eina_Bool
_accessible_property_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg EINA_UNUSED,
                         Eldbus_Message **error EINA_UNUSED)
{
   const char *ret = NULL, *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *ret_obj = NULL, *obj = _bridge_object_from_path(bridge, obj_path);

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

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
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg EINA_UNUSED,
                         Eldbus_Message **error EINA_UNUSED)
{
   int n;
   const char *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

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
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg EINA_UNUSED,
                         Eldbus_Message **error EINA_UNUSED)
{
   Eina_List *actions;
   const char *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

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
   const char *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);

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
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg EINA_UNUSED,
                         Eldbus_Message **error EINA_UNUSED)
{
   double value;
   const char *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

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
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg EINA_UNUSED,
                         Eldbus_Message **error EINA_UNUSED)
{
   const char *value;
   const char *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

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
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg EINA_UNUSED,
                         Eldbus_Message **error EINA_UNUSED)
{
   const char *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int val;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

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
   int value;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, NULL);

   if (!eldbus_message_iter_arguments_get(iter, "i", &value))
     {
       return eldbus_message_error_new(input_msg, "org.freedesktop.DBus.Error.InvalidArgs", "Expected value of type: int.");
     }

   if (!strcmp(property, "Id"))
     {
        pd->id = value;
        Eldbus_Message *answer = eldbus_message_method_return_new(input_msg);
        eldbus_message_arguments_append(answer, "b", EINA_TRUE);
        eldbus_service_property_changed(iface, "Id");
        return answer;
     }

   return NULL;
}

static Eina_Bool
_application_properties_get(const Eldbus_Service_Interface *interface, const char *property,
                         Eldbus_Message_Iter *iter, const Eldbus_Message *request_msg EINA_UNUSED,
                         Eldbus_Message **error EINA_UNUSED)
{
   const char *obj_path = eldbus_service_object_path_get(interface);
   Eo *bridge = eldbus_service_object_data_get(interface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, EINA_FALSE);

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
   ATSPI_DBUS_INTERFACE_ACCESSIBLE, accessible_methods, _event_obj_signals, accessible_properties, _accessible_property_get, NULL
};

static const Eldbus_Service_Interface_Desc event_iface_desc = {
   ATSPI_DBUS_INTERFACE_EVENT_OBJECT, NULL, _event_obj_signals, NULL, NULL, NULL
};

static const Eldbus_Service_Interface_Desc action_iface_desc = {
   ATSPI_DBUS_INTERFACE_ACTION, action_methods, NULL, action_properties, NULL, NULL
};

static const Eldbus_Service_Interface_Desc window_iface_desc = {
   ATSPI_DBUS_INTERFACE_EVENT_WINDOW, NULL, _window_obj_signals, NULL, NULL, NULL
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

static void
_bridge_iter_object_reference_append(Eo *bridge, Eldbus_Message_Iter *iter, const Eo *obj)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);
   Eldbus_Message_Iter *iter_struct = eldbus_message_iter_container_new(iter, 'r', NULL);
   EINA_SAFETY_ON_NULL_RETURN(iter);
   char *path = _bridge_path_from_object(bridge, obj);
   eldbus_message_iter_basic_append(iter_struct, 's', eldbus_connection_unique_name_get(pd->a11y_bus));
   eldbus_message_iter_basic_append(iter_struct, 'o', path);
   eldbus_message_iter_container_close(iter, iter_struct);
   free(path);
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
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_ACCESSIBLE);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_COMPONENT);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_ACTION);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_VALUE_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_VALUE);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_IMAGE_MIXIN))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_IMAGE);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_TEXT);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_EDITABLE_TEXT);
  if (eo_isa(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE))
    eldbus_message_iter_basic_append(iter_array, 's', ATSPI_DBUS_INTERFACE_SELECTION);

  eldbus_message_iter_container_close(iter, iter_array);
}

static Eina_Bool
_cache_item_reference_append_cb(const Eina_Hash *hash EINA_UNUSED, const void *key EINA_UNUSED, void *data, void *fdata)
{
  if (!eo_ref_get(data) || eo_destructed_is(data))
    return EINA_TRUE;

  struct cache_closure *cl = fdata;
  Eldbus_Message_Iter *iter_struct, *iter_sub_array;
  Eldbus_Message_Iter *iter_array = cl->iter;
  Elm_Atspi_State_Set states;
  Elm_Atspi_Role role;
  Eo *root = elm_atspi_bridge_root_get(cl->bridge);

  eo_do(data, role = elm_interface_atspi_accessible_role_get());

  iter_struct = eldbus_message_iter_container_new(iter_array, 'r', NULL);
  EINA_SAFETY_ON_NULL_RETURN_VAL(iter_struct, EINA_TRUE);

  /* Marshall object path */
  _bridge_iter_object_reference_append(cl->bridge, iter_struct, data);

  /* Marshall application */
  _bridge_iter_object_reference_append(cl->bridge, iter_struct, root);

  Eo *parent = NULL;
  eo_do(data, parent = elm_interface_atspi_accessible_parent_get());
  /* Marshall parent */
  if ((!parent) && (ELM_ATSPI_ROLE_APPLICATION == role))
    _object_desktop_reference_append(iter_struct);
  else
    _bridge_iter_object_reference_append(cl->bridge, iter_struct, parent);

  /* Marshall children  */
  Eina_List *children_list = NULL, *l;
  Eo *child;

  eo_do(data, children_list = elm_interface_atspi_accessible_children_get());
  iter_sub_array = eldbus_message_iter_container_new(iter_struct, 'a', "(so)");
  EINA_SAFETY_ON_NULL_GOTO(iter_sub_array, fail);

  EINA_LIST_FOREACH(children_list, l, child)
     _bridge_iter_object_reference_append(cl->bridge, iter_sub_array, child);

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
   Eldbus_Message_Iter *iter;
   Eldbus_Message *ret;
   struct cache_closure cl;

   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   if (!bridge) return NULL;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(bridge, pd, NULL);

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   cl.iter = eldbus_message_iter_container_new(iter, 'a', CACHE_ITEM_SIGNATURE);
   EINA_SAFETY_ON_NULL_GOTO(cl.iter, fail);

   cl.bridge = bridge;

   eina_hash_foreach(pd->cache, _cache_item_reference_append_cb, &cl);
   eldbus_message_iter_container_close(iter, cl.iter);

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eina_Bool contains = EINA_FALSE;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eo *accessible = NULL;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter;

   if (!eldbus_message_arguments_get(msg, "iiu", &x, &y, &coord_type))
     return eldbus_message_error_new(msg, "org.freedesktop.DBus.Error.InvalidArgs", "Invalid index type.");

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   iter = eldbus_message_iter_get(ret);
   Eina_Bool type = coord_type == ATSPI_COORD_TYPE_SCREEN ? EINA_TRUE : EINA_FALSE;
   eo_do(obj, accessible = elm_interface_atspi_component_accessible_at_point_get(type, x, y));
   _bridge_iter_object_reference_append(bridge, iter, accessible);

   return ret;
}

static Eldbus_Message *
_component_get_extents(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y, w, h;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;
   Eldbus_Message_Iter *iter, *iter_struct;
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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eldbus_Message *ret;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int layer = 0;
   Eldbus_Message *ret;
   AtspiComponentLayer atspi_layer;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   Eina_Bool focus = EINA_FALSE;

   eo_do(obj, focus = elm_interface_atspi_component_focus_grab());

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "b", focus);

   return ret;
}

static Eldbus_Message *
_component_get_alpha(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   Eldbus_Message *ret;
   double alpha = 0;

   eo_do(obj, alpha = elm_interface_atspi_component_alpha_get());

   ret = eldbus_message_method_return_new(msg);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ret, NULL);

   eldbus_message_arguments_append(ret, "d", alpha);

   return ret;
}

static Eldbus_Message *
_component_set_extends(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y, w, h;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;
   Eina_Bool result = EINA_FALSE;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int x, y;
   Eina_Bool result = EINA_FALSE;
   AtspiCoordType coord_type;
   Eldbus_Message *ret;

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
   const char *obj_path = eldbus_service_object_path_get(iface);
   Eo *bridge = eldbus_service_object_data_get(iface, ELM_ATSPI_BRIDGE_CLASS_NAME);
   Eo *obj = _bridge_object_from_path(bridge, obj_path);
   int w, h;
   Eina_Bool result;
   Eldbus_Message *ret;

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
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(obj, ELM_ATSPI_BRIDGE_CLASS);
   pd->cache_interface = eldbus_service_interface_register(pd->a11y_bus, CACHE_INTERFACE_PATH, &cache_iface_desc);
   eldbus_service_object_data_set(pd->cache_interface, ELM_ATSPI_BRIDGE_CLASS_NAME, obj);
}

static void
_set_broadcast_flag(const char *event, Eo *bridge)
{
   char **tokens;
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(bridge, ELM_ATSPI_BRIDGE_CLASS);

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
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(data, ELM_ATSPI_BRIDGE_CLASS);
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
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(bridge, ELM_ATSPI_BRIDGE_CLASS);
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
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_state_broadcast_mask, state_data->type))
     return EINA_FALSE;

   if ((state_data->type > ELM_ATSPI_STATE_LAST_DEFINED) ||
        (int)state_data->type < 0)
     return EINA_FALSE;

   type_desc = elm_states_to_atspi_state[state_data->type].name;

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_STATE_CHANGED, type_desc, state_data->new_value, 0, NULL);
   return EINA_TRUE;
}

static Eina_Bool
_property_changed_signal_send(void *data, Eo *obj EINA_UNUSED, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   const char *property = event_info;
   char *atspi_desc;
   enum _Atspi_Object_Property prop = ATSPI_OBJECT_PROPERTY_LAST;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

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

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_PROPERTY_CHANGED, atspi_desc, 0, 0, NULL, NULL);
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
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

   type = ev_data->is_added ? ATSPI_OBJECT_CHILD_ADDED : ATSPI_OBJECT_CHILD_REMOVED;

   // update cached objects
   if (ev_data->is_added)
     _bridge_cache_build(data, ev_data->child);

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

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_CHILDREN_CHANGED, atspi_desc, idx, 0, "(so)", eldbus_connection_unique_name_get(pd->a11y_bus), ev_data->child);

   return EINA_TRUE;
}

static Eina_Bool
_window_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info EINA_UNUSED)
{
   enum _Atspi_Window_Signals type;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, eo_class_name_get(ELM_INTERFACE_ATSPI_WINDOW_INTERFACE), ifc, EINA_FALSE);

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

   _bridge_signal_send(data, ifc, type, "", 0, 0, "i", 0);
   return EINA_TRUE;
}

static Eina_Bool
_selection_signal_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_SELECTION_CHANGED))
     return EINA_FALSE;

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_SELECTION_CHANGED, "", 0, 0, "i", 0);
   return EINA_TRUE;
}

static void _bridge_signal_send(Eo *bridge, Eldbus_Service_Interface *infc, int signal, const char *minor, unsigned int det1, unsigned int det2, const char *variant_sig, ...)
{
   Eldbus_Message *msg;
   Eldbus_Message_Iter *iter , *iter_stack[64], *iter_struct;
   va_list va;
   Eo *atspi_obj;
   char *path;
   int top = 0;

   EINA_SAFETY_ON_NULL_RETURN(infc);
   EINA_SAFETY_ON_NULL_RETURN(minor);
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   msg = eldbus_service_signal_new(infc, signal);
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
                   free(path);
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
   free(path);

   if (eldbus_service_signal_send(infc, msg))
      DBG("Signal send::[%s,%d,%d]", minor, det1, det2);
}

static Eina_Bool
_text_caret_moved_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   int cursor_pos = 0;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_FALSE);
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CARET_MOVED))
     return EINA_TRUE;

   eo_do(obj, cursor_pos = elm_interface_atspi_text_caret_offset_get());

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_TEXT_CARET_MOVED, "", cursor_pos, 0, NULL, NULL);

   return EINA_TRUE;
}

static Eina_Bool
_text_text_inserted_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   Elm_Atspi_Text_Change_Info *info = event_info;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CHANGED))
     return EINA_TRUE;

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_TEXT_CHANGED, "insert", info->pos, info->len, "s", info->content);

   return EINA_TRUE;
}

static Eina_Bool
_text_text_removed_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info)
{
   Elm_Atspi_Text_Change_Info *info = event_info;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_CHANGED))
     return EINA_TRUE;

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_TEXT_CHANGED, "delete", info->pos, info->len, "s", info->content);

   return EINA_TRUE;
}

static Eina_Bool
_text_selection_changed_send(void *data, Eo *obj, const Eo_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN_VAL(data, pd, EINA_TRUE);
   ELM_ATSPI_OBJECT_INTERFACE_GET_OR_RETURN_VAL(obj, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc, EINA_FALSE);

   if (!STATE_TYPE_GET(pd->object_broadcast_mask, ATSPI_OBJECT_EVENT_TEXT_SELECTION_CHANGED))
     return EINA_TRUE;

   _bridge_signal_send(data, ifc, ATSPI_OBJECT_EVENT_TEXT_SELECTION_CHANGED, "", 0, 0, NULL, NULL);

   return EINA_TRUE;
}

static void
_event_handlers_register(Eo *bridge)
{
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(bridge, ELM_ATSPI_BRIDGE_CLASS);

   _registered_events_list_update(bridge);

   // register signal handlers in order to update list of registered listeners of ATSPI-Clients
   pd->register_hdl = eldbus_signal_handler_add(pd->a11y_bus, ATSPI_DBUS_NAME_REGISTRY, ATSPI_DBUS_PATH_REGISTRY, ATSPI_DBUS_INTERFACE_REGISTRY, "EventListenerRegistered", _handle_listener_change, bridge);
   pd->unregister_hdl = eldbus_signal_handler_add(pd->a11y_bus, ATSPI_DBUS_NAME_REGISTRY, ATSPI_DBUS_PATH_REGISTRY, ATSPI_DBUS_INTERFACE_REGISTRY, "EventListenerDeregistered", _handle_listener_change, bridge);

   pd->key_flr = ecore_event_filter_add(NULL, _elm_atspi_bridge_key_filter, NULL, bridge);
}

static void
_bridge_object_unregister(Eo *bridge, Eo *obj)
{
   char *path;
   Eldbus_Message *sig;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   _object_unregister(obj, bridge);

   sig = eldbus_service_signal_new(pd->cache_interface, ATSPI_OBJECT_CHILD_REMOVED);
   Eldbus_Message_Iter *iter = eldbus_message_iter_get(sig);
   _bridge_iter_object_reference_append(bridge, iter, obj);
   eldbus_service_signal_send(pd->cache_interface, sig);

   path = _bridge_path_from_object(bridge, obj);
   eina_hash_del(pd->cache, path, obj);
   free(path);
}

static Eina_Bool
_on_object_del(void *data, Eo *obj, const Eo_Event_Description *event EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Eo *bridge = data;

   _bridge_object_unregister(bridge, obj);

   return EINA_TRUE;
}

static void
_bridge_cache_build(Eo *bridge, void *obj)
{
   Eina_List *children;
   Eo *child;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   if (!eo_isa(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN))
     return;

   _bridge_object_register(bridge, obj);

   eo_do(obj, children = elm_interface_atspi_accessible_children_get());
   EINA_LIST_FREE(children, child)
      _bridge_cache_build(bridge, child);
}

static Eina_Bool _unregister_cb(const Eina_Hash *hash EINA_UNUSED, const void *key EINA_UNUSED, void *data, void *fdata)
{
   _object_unregister(data, fdata);
   return EINA_TRUE;
}

static void
_a11y_connection_shutdown(Eo *bridge)
{
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(bridge, ELM_ATSPI_BRIDGE_CLASS);
   Eldbus_Pending *pending;

   if (pd->connected)
      _elm_atspi_bridge_app_unregister(bridge);

   if (pd->cache)
     {
        eina_hash_foreach(pd->cache, _unregister_cb, bridge);
        eina_hash_free(pd->cache);
     }
   pd->cache = NULL;

   if (pd->cache_interface)
     eldbus_service_object_unregister(pd->cache_interface);

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

   eo_do(bridge, eo_event_callback_call(ELM_ATSPI_BRIDGE_EVENT_DISCONNECTED, NULL));
   pd->connected = EINA_FALSE;
}

static void _disconnect_cb(void *data, Eldbus_Connection *conn EINA_UNUSED, void *event_info EINA_UNUSED)
{
   _a11y_connection_shutdown(data);
}

static void
_a11y_bus_initialize(Eo *obj, const char *socket_addr)
{
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(obj, ELM_ATSPI_BRIDGE_CLASS);
   Eo *root;

   pd->a11y_bus = eldbus_private_address_connection_get(socket_addr);
   if (!pd->a11y_bus)
     return;

   eldbus_connection_event_callback_add(pd->a11y_bus, ELDBUS_CONNECTION_EVENT_DISCONNECTED, _disconnect_cb, obj);

   // init data structures
   pd->cache = eina_hash_string_superfast_new(NULL);
   pd->state_hash = _elm_atspi_state_hash_build();

   // register interfaces
   _cache_register(obj);
   _event_handlers_register(obj);
   _elm_atspi_bridge_app_register(obj);

   // buid cache
   eo_do(obj, root = elm_obj_atspi_bridge_root_get());
   _bridge_cache_build(obj, root);
}

static void
_a11y_bus_address_get(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending)
{
   const char *errname, *errmsg, *sock_addr = NULL;
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(data, ELM_ATSPI_BRIDGE_CLASS);

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
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(bridge, ELM_ATSPI_BRIDGE_CLASS);
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
   Elm_Atspi_Bridge_Data *pd = eo_data_scope_get(data, ELM_ATSPI_BRIDGE_CLASS);
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
   char *path;
   struct cache_closure cc;
   Eldbus_Message *sig;
   Eldbus_Service_Interface *ifc;

   ELM_ATSPI_BRIDGE_DATA_GET_OR_RETURN(bridge, pd);

   if (!eo_isa(obj, ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN))
     {
        WRN("Unable to register class w/o Elm_Interface_Atspi_Accessible!");
        return;
     }

   path = _bridge_path_from_object(bridge, obj);

   if (eina_hash_find(pd->cache, path))
     {
        WRN("Object at path: %s already registered", path);
        free(path);
        return;
     }

   eina_hash_add(pd->cache, path, obj);

   ifc = eldbus_service_interface_register(pd->a11y_bus, path, &accessible_iface_desc);
   eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
   eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN), ifc));

   if (eo_isa(obj, ELM_ATSPI_APP_OBJECT_CLASS))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &application_iface_desc);
        eo_key_data_set(ATSPI_DBUS_INTERFACE_APPLICATION, ifc);
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
     }

   ifc = eldbus_service_interface_register(pd->a11y_bus, path, &event_iface_desc);
   eo_do(obj,
         eo_event_callback_array_add(_events_cb(), bridge),
         eo_key_data_set(ATSPI_DBUS_INTERFACE_EVENT_OBJECT, ifc)
         );
   eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);

   if (eo_isa(obj, ELM_INTERFACE_ATSPI_ACTION_MIXIN))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &action_iface_desc);
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_ACTION_MIXIN), ifc));
     }
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_COMPONENT_MIXIN))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &component_iface_desc);
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_COMPONENT_MIXIN), ifc));
     }
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &editable_text_iface_desc);
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_TEXT_INTERFACE), ifc));
     }
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_IMAGE_MIXIN))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &image_iface_desc);
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_IMAGE_MIXIN), ifc));
     }
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &selection_iface_desc);
        eo_do(obj, eo_event_callback_array_add(_selection_cb(), bridge));
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_SELECTION_INTERFACE), ifc));
     }
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &text_iface_desc);
        eo_do(obj, eo_event_callback_array_add(_text_cb(), bridge));
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_TEXT_INTERFACE), ifc));
     }
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_WINDOW_INTERFACE))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &window_iface_desc);
        eo_do(obj, eo_event_callback_array_add(_window_cb(), bridge));
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_WINDOW_INTERFACE), ifc));
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
     }
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_VALUE_INTERFACE))
     {
        ifc = eldbus_service_interface_register(pd->a11y_bus, path, &value_iface_desc);
        eldbus_service_object_data_set(ifc, ELM_ATSPI_BRIDGE_CLASS_NAME, bridge);
        eo_do(obj, eo_key_data_set(eo_class_name_get(ELM_INTERFACE_ATSPI_VALUE_INTERFACE), ifc));
     }

   sig = eldbus_service_signal_new(pd->cache_interface, ATSPI_OBJECT_CHILD_ADDED);
   cc.iter = eldbus_message_iter_get(sig);
   cc.bridge = bridge;
   _cache_item_reference_append_cb(NULL, NULL, obj, &cc);

   eldbus_service_signal_send(pd->cache_interface, sig);

   free(path);
}

static void _object_unregister(Eo *obj, void *data)
{
   Eldbus_Service_Interface *ifc;

   eo_do(obj, eo_event_callback_array_del(_events_cb(), data));
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_WINDOW_INTERFACE))
     eo_do(obj, eo_event_callback_array_del(_window_cb(), data));
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_SELECTION_INTERFACE))
      eo_do(obj, eo_event_callback_array_del(_selection_cb(), data));
   if (eo_isa(obj, ELM_INTERFACE_ATSPI_TEXT_INTERFACE))
      eo_do(obj, eo_event_callback_array_del(_text_cb(), data));

   eo_do(obj, ifc = eo_key_data_get(ATSPI_DBUS_INTERFACE_EVENT_OBJECT));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_ACCESSIBLE_MIXIN)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_ACTION_MIXIN)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_COMPONENT_MIXIN)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_EDITABLE_TEXT_INTERFACE)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_IMAGE_MIXIN)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_SELECTION_INTERFACE)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_TEXT_INTERFACE)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_WINDOW_INTERFACE)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(eo_class_name_get(ELM_INTERFACE_ATSPI_VALUE_INTERFACE)));
   if (ifc) eldbus_service_interface_unregister(ifc);
   eo_do(obj, ifc = eo_key_data_get(ATSPI_DBUS_INTERFACE_APPLICATION));
   if (ifc) eldbus_service_interface_unregister(ifc);
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
      pd->root = eo_add(ELM_ATSPI_APP_OBJECT_CLASS, NULL);

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
