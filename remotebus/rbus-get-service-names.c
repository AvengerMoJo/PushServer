#include <include/rbus-get-service-name.h>

DBusConnection* get_connected( DBusBusType bus_type )
{
	DBusError err;
	DBusConnection* conn=NULL;
	int ret;

	dbus_error_init(&err);

	conn = dbus_bus_get( bus_type , &err);
	if (dbus_error_is_set(&err)) {
		_rbus_verbose("Connection Error (%s)\n", err.message);
		dbus_error_free(&err);
	}
	return conn;
}

