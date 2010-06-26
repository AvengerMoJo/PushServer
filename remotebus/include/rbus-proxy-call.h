#ifndef RBUS_PROXY_CALL_H
#define RBUS_PROXY_CALL_H

#include <stdarg.h>
#include <string.h>
#include <dbus/dbus.h>
#include <include/rbus-dbus-object-parser.h>

#include <include/remotebus-type.h> 

RBUS_EXTERN_START

DBusConnection *proxy_session( char* object_name );
// void method_object_call_blocked( rbus_dbus_object_type_pointer dbus_object, rbus_method_type_pointer method, void ***return_list, int first_arg, ... );
void **method_object_call_blocked( rbus_dbus_object_type_pointer dbus_object, rbus_method_type_pointer method, int first_arg, ... );


RBUS_EXTERN_END

#endif
