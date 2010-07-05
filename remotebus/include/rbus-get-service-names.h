#ifndef RBUS_GET_SERVICE_NAME_H
#define RBUS_GET_SERVICE_NAME_H

#include <string.h>
#include <dbus/dbus.h>
#include <include/remotebus-type.h> 

RBUS_EXTERN_START

typedef struct _service_list_type { 
	char **service_name; 
	int    service_count;
} service_list_type;

typedef service_list_type *service_list_type_pointer; 

service_list_type_pointer get_system_service_names();
service_list_type_pointer get_session_service_names();

RBUS_EXTERN_END

#endif

