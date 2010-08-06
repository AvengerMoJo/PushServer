#include <include/rbus-get-service-names.h>

DBusConnection* get_connected( DBusBusType bus_type )
{
	DBusError error;
	DBusConnection* conn=NULL;
	int ret;

	dbus_error_init(&error);

	conn = dbus_bus_get( bus_type , &error);
	if (dbus_error_is_set(&error)) {
		_rbus_verbose("Connection Error (%s)\n", error.message);
		dbus_error_free(&error);
	}
	return conn;
}

/* 
	The return list memory space need to be release after use 
*/
service_list_type_pointer get_service_list(DBusConnection* conn)
{
	DBusError error;
	DBusMessage *message;
	DBusMessage *reply;

	int reply_timeout = -1;   /*don't timeout*/
	//char* method = "ListNames"; 
	char* method = "ListActivatableNames";

	service_list_type_pointer ServiceList =  NULL;

	dbus_error_init (&error);
	/* Construct the message */
	message = dbus_message_new_method_call ( "org.freedesktop.DBus", 
						"/org/freedesktop/DBus", 
						"org.freedesktop.DBus", method );

	/* Call ListServices method */
	reply = dbus_connection_send_with_reply_and_block (conn, message, reply_timeout, &error);

	if (dbus_error_is_set (&error)) {
		_rbus_verbose("Calling ListService Error (%s)\n", error.message);
		dbus_error_free(&error);
		return NULL;
	}

	/* creating the ServiceList */ 
	ServiceList = calloc( 1, sizeof( service_list_type ) );

	/* Extract the data from the reply */
	if (!dbus_message_get_args (reply, &error,
                               DBUS_TYPE_ARRAY,  DBUS_TYPE_STRING, &ServiceList->service_name, &ServiceList->service_count,
                               DBUS_TYPE_INVALID )) {
		_rbus_verbose("Failed to complete ListServices call: %s\n", error.message);
		dbus_error_free(&error);
		return NULL;
	}
	dbus_message_unref (reply);
	dbus_message_unref (message);
	return ServiceList; 
}

service_list_type_pointer get_service_names( DBusConnection *session )
{
	return get_service_list( session );
}

service_list_type_pointer get_system_service_names() 
{
	DBusConnection *session = get_connected( DBUS_BUS_SESSION );
	return get_service_names( session ); 
}

service_list_type_pointer get_session_service_names()
{
	DBusConnection *session = get_connected( DBUS_BUS_SYSTEM );
	return get_service_names( session ); 
}
