#include <stdio.h>
#include <stdlib.h>
#include <include/remotebus-internal.h>
#include <dbus/dbus.h>

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
   char* connection_name = "com.avengergear.PushServer.Consumer"; // target name for the consumer

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
   DBusMessageIter contain_args;
   DBusMessageIter dict_args;
   DBusMessageIter var_args;
   DBusPendingCall* pending;
   dbus_uint32_t result;

   dbus_error_init(&error);
   char* param = "calling you with something in the param";
   char* param_list[2] = { "stuff 1", "stuff 2" };
   dbus_uint16_t number = 16;
   char array_type[2]; 
   char ele_type[5]; 

   char* target = service_name ; // target for the method call;
   char* object =  "/com/avengergear/PushServer/Proxy/TypeTest"; // started with root .... 
   // we need to some how create a dynamic linked list of object -> method mapping table to displace for call 
   //char* interface = DBUS_INTERFACE_INTROSPECTABLE ; 
   //char* interface = "org.freedesktop.DBus.Introspectable";
   char* interface = "com.avengergear.PushServer.Proxy.Interface";

   //char* name = "RemoteName"; // name of the method
   char* method = "TypeTest";

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
	    // append arguments
	    dbus_message_iter_init_append(msg, &args);
	    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param)) {
		fprintf(stderr, "String Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "Send string\n");
	    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT16, &number)) {
		fprintf(stderr, "UINT16 Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "Send uint16\n");
	    array_type[0]='s';
	    array_type[1]='\0';
	    if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, array_type, &contain_args)) {
		fprintf(stderr, "OpenContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "OpenContainer \n");
	    if (!dbus_message_iter_append_basic(&contain_args, DBUS_TYPE_STRING, &param_list[0])) {
		fprintf(stderr, "Arrary String is Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "Sending a string\n");
	    if (!dbus_message_iter_append_basic(&contain_args, DBUS_TYPE_STRING, &param_list[1])) {
		fprintf(stderr, "Arrary String is Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "Sending a string\n");
	    if (!dbus_message_iter_close_container(&args, &contain_args)) {
		fprintf(stderr, "CloseContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "CloseContainer\n");

	    ele_type[0]=DBUS_DICT_ENTRY_BEGIN_CHAR;
	    ele_type[1]='s';
	    ele_type[2]='v';
	    ele_type[3]=DBUS_DICT_ENTRY_END_CHAR;
	    ele_type[4]='\0';

	    if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, ele_type, &contain_args)) {
		fprintf(stderr, "OpenContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "OpenContainer Array\n");
	    if (!dbus_message_iter_open_container(&contain_args, DBUS_TYPE_DICT_ENTRY, NULL, &dict_args)) {
		fprintf(stderr, "OpenContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "OpenContainer Array Dict\n");

	    if (!dbus_message_iter_append_basic(&dict_args, DBUS_TYPE_STRING, &param)) {
		fprintf(stderr, "Arrary Dict String is Out Of Memory!\n");
		exit(1);
	    }

	    array_type[0]='q';
	    array_type[1]='\0';
	    if (!dbus_message_iter_open_container(&dict_args, DBUS_TYPE_VARIANT, array_type , &var_args)) {
		fprintf(stderr, "OpenContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "OpenContainer Array Dict Var\n");

	    if (!dbus_message_iter_append_basic(&var_args, DBUS_TYPE_UINT16, &number)) {
		fprintf(stderr, "Array Dict UNIT16 String is Out Of Memory!\n");
		exit(1);
	    }

	    if (!dbus_message_iter_close_container(&dict_args, &var_args)) {
		fprintf(stderr, "CloseContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "CloseContainer Array Dict\n");
	    if (!dbus_message_iter_close_container(&contain_args, &dict_args)) {
		fprintf(stderr, "CloseContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "CloseContainer Array\n");
	    if (!dbus_message_iter_close_container(&args, &contain_args)) {
		fprintf(stderr, "CloseContainer Out Of Memory!\n");
		exit(1);
	    }
	    fprintf(stderr, "CloseContainer\n");



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
    //call_name( session_bus, "com.avengergear.PushServer.Proxy" );
    call_name( session_bus, "com.avengergear.PushServer.Proxy.TypeTest" );

    return 0;
}
