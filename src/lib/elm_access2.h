/**
 * @defgroup Access2 Access2
 * @ingroup Elementary
 *
 */

#include "elm_access2.eo.h"

typedef enum
{
    ELM_ACCESS2_ROLE_INVALID,
    ELM_ACCESS2_ROLE_ACCELERATOR_LABEL,
    ELM_ACCESS2_ROLE_ALERT,
    ELM_ACCESS2_ROLE_ANIMATION,
    ELM_ACCESS2_ROLE_ARROW,
    ELM_ACCESS2_ROLE_CALENDAR,
    ELM_ACCESS2_ROLE_CANVAS,
    ELM_ACCESS2_ROLE_CHECK_BOX,
    ELM_ACCESS2_ROLE_CHECK_MENU_ITEM,
    ELM_ACCESS2_ROLE_COLOR_CHOOSER,
    ELM_ACCESS2_ROLE_COLUMN_HEADER,
    ELM_ACCESS2_ROLE_COMBO_BOX,
    ELM_ACCESS2_ROLE_DATE_EDITOR,
    ELM_ACCESS2_ROLE_DESKTOP_ICON,
    ELM_ACCESS2_ROLE_DESKTOP_FRAME,
    ELM_ACCESS2_ROLE_DIAL,
    ELM_ACCESS2_ROLE_DIALOG,
    ELM_ACCESS2_ROLE_DIRECTORY_PANE,
    ELM_ACCESS2_ROLE_DRAWING_AREA,
    ELM_ACCESS2_ROLE_FILE_CHOOSER,
    ELM_ACCESS2_ROLE_FILLER,
    ELM_ACCESS2_ROLE_FOCUS_TRAVERSABLE,
    ELM_ACCESS2_ROLE_FONT_CHOOSER,
    ELM_ACCESS2_ROLE_FRAME,
    ELM_ACCESS2_ROLE_GLASS_PANE,
    ELM_ACCESS2_ROLE_HTML_CONTAINER,
    ELM_ACCESS2_ROLE_ICON,
    ELM_ACCESS2_ROLE_IMAGE,
    ELM_ACCESS2_ROLE_INTERNAL_FRAME,
    ELM_ACCESS2_ROLE_LABEL,
    ELM_ACCESS2_ROLE_LAYERED_PANE,
    ELM_ACCESS2_ROLE_LIST,
    ELM_ACCESS2_ROLE_LIST_ITEM,
    ELM_ACCESS2_ROLE_MENU,
    ELM_ACCESS2_ROLE_MENU_BAR,
    ELM_ACCESS2_ROLE_MENU_ITEM,
    ELM_ACCESS2_ROLE_OPTION_PANE,
    ELM_ACCESS2_ROLE_PAGE_TAB,
    ELM_ACCESS2_ROLE_PAGE_TAB_LIST,
    ELM_ACCESS2_ROLE_PANEL,
    ELM_ACCESS2_ROLE_PASSWORD_TEXT,
    ELM_ACCESS2_ROLE_POPUP_MENU,
    ELM_ACCESS2_ROLE_PROGRESS_BAR,
    ELM_ACCESS2_ROLE_PUSH_BUTTON,
    ELM_ACCESS2_ROLE_RADIO_BUTTON,
    ELM_ACCESS2_ROLE_RADIO_MENU_ITEM,
    ELM_ACCESS2_ROLE_ROOT_PANE,
    ELM_ACCESS2_ROLE_ROW_HEADER,
    ELM_ACCESS2_ROLE_SCROLL_BAR,
    ELM_ACCESS2_ROLE_SCROLL_PANE,
    ELM_ACCESS2_ROLE_SEPARATOR,
    ELM_ACCESS2_ROLE_SLIDER,
    ELM_ACCESS2_ROLE_SPIN_BUTTON,
    ELM_ACCESS2_ROLE_SPLIT_PANE,
    ELM_ACCESS2_ROLE_STATUS_BAR,
    ELM_ACCESS2_ROLE_TABLE,
    ELM_ACCESS2_ROLE_TABLE_CELL,
    ELM_ACCESS2_ROLE_TABLE_COLUMN_HEADER,
    ELM_ACCESS2_ROLE_TABLE_ROW_HEADER,
    ELM_ACCESS2_ROLE_TEAROFF_MENU_ITEM,
    ELM_ACCESS2_ROLE_TERMINAL,
    ELM_ACCESS2_ROLE_TEXT,
    ELM_ACCESS2_ROLE_TOGGLE_BUTTON,
    ELM_ACCESS2_ROLE_TOOL_BAR,
    ELM_ACCESS2_ROLE_TOOL_TIP,
    ELM_ACCESS2_ROLE_TREE,
    ELM_ACCESS2_ROLE_TREE_TABLE,
    ELM_ACCESS2_ROLE_UNKNOWN,
    ELM_ACCESS2_ROLE_VIEWPORT,
    ELM_ACCESS2_ROLE_WINDOW,
    ELM_ACCESS2_ROLE_EXTENDED,
    ELM_ACCESS2_ROLE_HEADER,
    ELM_ACCESS2_ROLE_FOOTER,
    ELM_ACCESS2_ROLE_PARAGRAPH,
    ELM_ACCESS2_ROLE_RULER,
    ELM_ACCESS2_ROLE_APPLICATION,
    ELM_ACCESS2_ROLE_AUTOCOMPLETE,
    ELM_ACCESS2_ROLE_EDITBAR,
    ELM_ACCESS2_ROLE_EMBEDDED,
    ELM_ACCESS2_ROLE_ENTRY,
    ELM_ACCESS2_ROLE_CHART,
    ELM_ACCESS2_ROLE_CAPTION,
    ELM_ACCESS2_ROLE_DOCUMENT_FRAME,
    ELM_ACCESS2_ROLE_HEADING,
    ELM_ACCESS2_ROLE_PAGE,
    ELM_ACCESS2_ROLE_SECTION,
    ELM_ACCESS2_ROLE_REDUNDANT_OBJECT,
    ELM_ACCESS2_ROLE_FORM,
    ELM_ACCESS2_ROLE_LINK,
    ELM_ACCESS2_ROLE_INPUT_METHOD_WINDOW,
    ELM_ACCESS2_ROLE_TABLE_ROW,
    ELM_ACCESS2_ROLE_TREE_ITEM,
    ELM_ACCESS2_ROLE_DOCUMENT_SPREADSHEET,
    ELM_ACCESS2_ROLE_DOCUMENT_PRESENTATION,
    ELM_ACCESS2_ROLE_DOCUMENT_TEXT,
    ELM_ACCESS2_ROLE_DOCUMENT_WEB,
    ELM_ACCESS2_ROLE_DOCUMENT_EMAIL,
    ELM_ACCESS2_ROLE_COMMENT,
    ELM_ACCESS2_ROLE_LIST_BOX,
    ELM_ACCESS2_ROLE_GROUPING,
    ELM_ACCESS2_ROLE_IMAGE_MAP,
    ELM_ACCESS2_ROLE_NOTIFICATION,
    ELM_ACCESS2_ROLE_INFO_BAR,
    ELM_ACCESS2_ROLE_LAST_DEFINED,
} Elm_Access2_Role;

enum
{
    ELM_ACCESS2_STATE_INVALID,
    ELM_ACCESS2_STATE_ACTIVE,
    ELM_ACCESS2_STATE_ARMED,
    ELM_ACCESS2_STATE_BUSY,
    ELM_ACCESS2_STATE_CHECKED,
    ELM_ACCESS2_STATE_COLLAPSED,
    ELM_ACCESS2_STATE_DEFUNCT,
    ELM_ACCESS2_STATE_EDITABLE,
    ELM_ACCESS2_STATE_ENABLED,
    ELM_ACCESS2_STATE_EXPANDABLE,
    ELM_ACCESS2_STATE_EXPANDED,
    ELM_ACCESS2_STATE_FOCUSABLE,
    ELM_ACCESS2_STATE_FOCUSED,
    ELM_ACCESS2_STATE_HAS_TOOLTIP,
    ELM_ACCESS2_STATE_HORIZONTAL,
    ELM_ACCESS2_STATE_ICONIFIED,
    ELM_ACCESS2_STATE_MODAL,
    ELM_ACCESS2_STATE_MULTI_LINE,
    ELM_ACCESS2_STATE_MULTISELECTABLE,
    ELM_ACCESS2_STATE_OPAQUE,
    ELM_ACCESS2_STATE_PRESSED,
    ELM_ACCESS2_STATE_RESIZABLE,
    ELM_ACCESS2_STATE_SELECTABLE,
    ELM_ACCESS2_STATE_SELECTED,
    ELM_ACCESS2_STATE_SENSITIVE,
    ELM_ACCESS2_STATE_SHOWING,
    ELM_ACCESS2_STATE_SINGLE_LINE,
    ELM_ACCESS2_STATE_STALE,
    ELM_ACCESS2_STATE_TRANSIENT,
    ELM_ACCESS2_STATE_VERTICAL,
    ELM_ACCESS2_STATE_VISIBLE,
    ELM_ACCESS2_STATE_MANAGES_DESCENDANTS,
    ELM_ACCESS2_STATE_INDETERMINATE,
    ELM_ACCESS2_STATE_REQUIRED,
    ELM_ACCESS2_STATE_TRUNCATED,
    ELM_ACCESS2_STATE_ANIMATED,
    ELM_ACCESS2_STATE_INVALID_ENTRY,
    ELM_ACCESS2_STATE_SUPPORTS_AUTOCOMPLETION,
    ELM_ACCESS2_STATE_SELECTABLE_TEXT,
    ELM_ACCESS2_STATE_IS_DEFAULT,
    ELM_ACCESS2_STATE_VISITED,
    ELM_ACCESS2_STATE_LAST_DEFINED,
} Elm_Access2_State;

/**
 * Elm_Atspi_Relation_Type describes relation between two objects.
 */
enum {
     ELM_ACCESS2_RELATION_NULL,
     ELM_ACCESS2_RELATION_LABEL_FOR,
     ELM_ACCESS2_RELATION_LABELLED_BY,
     ELM_ACCESS2_RELATION_CONTROLLER_FOR,
     ELM_ACCESS2_RELATION_CONTROLLED_BY,
     ELM_ACCESS2_RELATION_MEMBER_OF,
     ELM_ACCESS2_RELATION_TOOLTIP_FOR,
     ELM_ACCESS2_RELATION_NODE_CHILD_OF,
     ELM_ACCESS2_RELATION_NODE_PARENT_OF,
     ELM_ACCESS2_RELATION_EXTENDED,
     ELM_ACCESS2_RELATION_FLOWS_TO,
     ELM_ACCESS2_RELATION_FLOWS_FROM,
     ELM_ACCESS2_RELATION_SUBWINDOW_OF,
     ELM_ACCESS2_RELATION_EMBEDS,
     ELM_ACCESS2_RELATION_EMBEDDED_BY,
     ELM_ACCESS2_RELATION_POPUP_FOR,
     ELM_ACCESS2_RELATION_PARENT_WINDOW_OF,
     ELM_ACCESS2_RELATION_DESCRIPTION_FOR,
     ELM_ACCESS2_RELATION_DESCRIBED_BY,
     ELM_ACCESS2_RELATION_LAST_DEFINED,
} Elm_Access2_Relation;

typedef Eo (*Factory_Construct_Cb)(const Eo *obj);

/**
 * @brief Creates accessible object from Eo object.
 * @since 1.13
 */
EAPI Eo*
elm_access2_object_factory_create(Eo *obj);

/**
 * @brief Utility function helping to 
 * @since 1.13
 */
EAPI Eina_Bool
elm_access2_factory_type_register(Eo_Class *object_class, Factory_Construct_Cb cb);

/**
 * @brief
 * @since 1.13
 */
EAPI Eina_Bool
elm_access2_object_register(Eo *obj, Eo *accessible_object);

/**
 * @brief
 * @since 1.13
 */
EAPI Eo*
elm_access2_object_get(Eo *obj);

/**
 * @brief
 * @since 1.13
 */
EAPI Eo*
elm_access2_application_object_get(void);
