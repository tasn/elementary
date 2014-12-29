extern "C"
{
#ifdef HAVE_CONFIG_H
# include <elementary_config.h>
#endif

#define ELM_INTERNAL_API_ARGESFSDFEFC
#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_COMPONENT_PROTECTED
#define ELM_INTERFACE_ATSPI_ACTION_PROTECTED
#define ELM_INTERFACE_ATSPI_VALUE_PROTECTED
#define ELM_INTERFACE_ATSPI_EDITABLE_TEXT_PROTECTED
#define ELM_INTERFACE_ATSPI_TEXT_PROTECTED
#define ELM_INTERFACE_ATSPI_SELECTION_PROTECTED
#define ELM_INTERFACE_ATSPI_IMAGE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED

#include <Eo.h>
#include <Evas.h>
#include <Elementary.h>
#include <elm_widget.h>

#include "elm_interface_atspi_accessible.h"
#include "elm_interface_atspi_widget_action.h"
#include "elm_interface_atspi_text.h"
#include "elm_interface_scrollable.h"
}

#include <Elementary.hh>
#include <Eina.hh>

#ifdef ELM_ELOCATION
#include <Elocation.h>
#endif

#ifdef ELM_ELOCATION
static void
_print_position(Elocation_Position *position, ::elm_label label)
{
   char buffer[1024];

   if (!position) return;
   snprintf(buffer, sizeof(buffer),
            "<b>GeoClue position reply with data from timestamp</b> %i<br/>"
            "<b>Latitude:</b> %f<br/>"
            "<b>Longitude:</b> %f<br/>"
            "<b>Altitude:</b> %f<br/>"
            "<b>Accuracy level:</b> %i<br/>"
            "<b>Accuracy horizontal:</b> %f<br/>"
            "<b>Accuracy vertical:</b> %f",
            position->timestamp, position->latitude, position->longitude,
            position->altitude, position->accur->level,
            position->accur->horizontal, position->accur->vertical);
   label.text_set("elm.text", buffer); 
}

static Eina_Bool
_position_changed(void *data, int ev_type, void *event)
{
   Elocation_Position *position;

   position =  static_cast<_Elocation_Postion*>(event);
   _print_position(position, *static_cast<::elm_label*>(data));
   return ECORE_CALLBACK_DONE;
}
#endif

EAPI_MAIN int
elm_main(int argc, char *argv[])
{
#ifdef ELM_ELOCATION
   	Elocation_Address *address;
   	Elocation_Position *position;
#endif

   	if (elm_need_elocation() == false)
     	return -1;

   	elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_HIDDEN);

 	::elm_win win(elm_win_util_standard_add("elocation", "Elocation example"));
  	win.autohide_set(true);

   	::elm_label label(efl::eo::parent = win);  
   	label.line_wrap_set(ELM_WRAP_CHAR);
        label.text_set("elm.text", "Getting location ..."); // XXX
   	label.size_hint_weight_set(EVAS_HINT_EXPAND, 0.0);
   	label.size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
   	label.slide_mode_set(ELM_LABEL_SLIDE_MODE_ALWAYS);
  	label.size_set(600, 480);
   	label.visibility_set(true);

#ifdef ELM_ELOCATION
   	address = elocation_address_new();
   	position = elocation_position_new();

   	ecore_event_handler_add(ELOCATION_EVENT_POSITION, _position_changed, &label);

   	elocation_position_get(position);
   	_print_position(position, label);
#endif

   	win.size_set(600, 480);
   	win.visibility_set(true);

   	elm_run();

// #ifdef ELM_ELOCATION
//    	elocation_position_free(position);
//    	elocation_address_free(address);
// #endif

   	return 0;
}
ELM_MAIN()
