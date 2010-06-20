#include <stdio.h>
#include <stdlib.h>
#include <include/remotebus-internal.h>
#include <dbus/dbus.h>

typedef struct remote_call_api {
    char* object; 
    char* interface; 
    char* method; 
    int  argc; 
    char** argv; 
    
    struct remote_call_api *next;
}* remote_call_api_pointer;

DBusConnection* get_session_connected(){
    DBusError err;
    DBusConnection* conn;
    // initialise the errors
    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        exit(1);
    } else { 
        fprintf(stdout, "session bus connected\n" );
	return conn;
    }
    return NULL;
}

void request_name_for_connection( DBusConnection* conn){ 
   DBusError err;
   int ret;
   char* connection_name = "com.novell.PushServer.Consumer"; // target name for the consumer

   // request our name on the bus and check for errors
   ret = dbus_bus_request_name(conn, connection_name, DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
   if (dbus_error_is_set(&err)) { 
      fprintf(stderr, "Name Error (%s)\n", err.message); 
      dbus_error_free(&err);
   }
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) { 
      fprintf(stderr, "Not Primary Owner (%d)\n", ret);
      exit(1); 
   }
}

void method_callback(DBusPendingCall* pending){
   dbus_bool_t stat;
   dbus_uint32_t level;
   DBusMessage* msg;
   DBusMessageIter args;
   DBusMessageIter list;
   DBusError error;
   char** service_list;
   char* stuff;
   int service_list_len, i;
        
   dbus_error_init (&error);

   // block until we receive a reply
   dbus_pending_call_block(pending);
   
   // get the reply message
   msg = dbus_pending_call_steal_reply(pending);
   if (NULL == msg) {
      fprintf(stderr, "Reply Null\n"); 
      exit(1); 
   }
   // free the pending message handle
   dbus_pending_call_unref(pending);

    /* Extract the data from the reply */
    if (!dbus_message_get_args (msg, &error,
                               DBUS_TYPE_STRING, &service_list,
                               DBUS_TYPE_INVALID )) {
        fprintf (stderr, "Failed to complete 71 get_args call: %s\n", error.message);
        exit (1);
    }

    printf("Got Reply: \n%s\n", service_list);
    // free reply and close connection
    dbus_message_unref(msg);   
}

void call_name(DBusConnection* conn, char* service_name ){
   DBusError error;
   DBusMessage* msg;
   DBusMessageIter args;
   DBusPendingCall* pending;
   dbus_uint32_t result;

   dbus_error_init(&error);
   char* param = "calling you with something in the param";

   char* target = service_name ; // target for the method call;
   char* object =  "/org/gnome/Tomboy/RemoteControl"; // started with root .... 
   // we need to some how create a dynamic linked list of object -> method mapping table to displace for call 
   char* interface = DBUS_INTERFACE_INTROSPECTABLE ; 
   //char* interface = "org.freedesktop.DBus.Introspectable";
   //char* name = "RemoteName"; // name of the method
   char* method = "Introspect";

   if( !dbus_bus_start_service_by_name( conn, service_name, 0, &result, &error)){
      fprintf(stderr, "Can't start service %s\n", error.message);
      exit(1);
   } else { 
	// we can start looping all the methods and interface in the object tree
        msg = dbus_message_new_method_call( target, object, interface, method );
        if( NULL == msg ) {
           fprintf(stderr, "Reply Null\n");
           exit(1);
        } else { 
            // send message and get a handle for a reply
            if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)) { // -1 is default timeout
                fprintf(stderr, "Out Of Memory!\n");
                exit(1);
            }
            if (NULL == pending) {
                fprintf(stderr, "Pending Call Null\n");
                exit(1);
            } else {
                dbus_connection_flush(conn);
                // free message
                dbus_message_unref(msg);
                method_callback(pending);
            }
        }
   }
}

int main() { 
    fprintf(stdout, "DBus testing 123\n" );
    DBusConnection* session_bus;
    session_bus = get_session_connected(); 
    request_name_for_connection( session_bus );
    //call_name( session_bus, "com.novell.PushServer.Proxy" );
    call_name( session_bus, "org.gnome.Tomboy" );

    return 0;
}
