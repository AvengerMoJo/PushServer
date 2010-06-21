#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>
#include <include/remotebus-internal.h>
#include <include/rbus-dbus-object-parser.h>


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

void sending_signal( DBusConnection* conn ){ 
   dbus_uint32_t serial = 0; // unique number to associate replies with requests
   dbus_uint32_t* sigvalue=&serial; 

   char* object = "/com/novell/PushServer/Object"; // object name of the signal
   char* interface = "com.novell.PushSever.Signal.Interface"; // interface name of the signal
   char* name = "signal"; // name of the signal
   DBusMessage* msg;
   DBusMessageIter args;
      
   // create a signal and check for errors 
   msg = dbus_message_new_signal(object, interface , name); 
   if (NULL == msg){ 
      fprintf(stderr, "Message Null\n"); 
      exit(1); 
   } else {
      fprintf(stdout, "signal created\n"); 
   }

   // append arguments onto signal
   dbus_message_iter_init_append(msg, &args);
   fprintf(stdout, "init append\n"); 
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, sigvalue)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   } else {
      fprintf(stdout, "Message appended\n"); 
   }

   // send the message and flush the connection
   if (!dbus_connection_send(conn, msg, &serial)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   } else { 
      fprintf(stdout, "message sended\n"); 
   }
   dbus_connection_flush(conn);
   
   // free the message 
   dbus_message_unref(msg);
   fprintf( stdout, "Object %s, Interface %s, Name %s\n signal has been sended", object, interface, name );
}

void method_callback(DBusPendingCall* pending){
   dbus_bool_t stat;
   dbus_uint32_t level;
   DBusMessage* msg;
   DBusMessageIter args;
   DBusMessageIter list;
   DBusError error;
   char*** service_list;
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

   // read the parameters
/*
   if (!dbus_message_iter_init(msg, &args))
      fprintf(stderr, "Message has no arguments!\n"); 
   else {
      fprintf(stdout, "Message %s!\n", dbus_message_iter_get_element_type(&args) );
   }

   if( !dbus_message_iter_open_container( &args, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, &list )) { 
   }
   if( !dbus_message_iter_close_container( &args, &list )) { 
   }
*/
    /* Extract the data from the reply */
    if (!dbus_message_get_args (msg, &error,
                               DBUS_TYPE_ARRAY,  DBUS_TYPE_STRING, &service_list, &service_list_len,
                               DBUS_TYPE_INVALID )) {
        fprintf (stderr, "Failed to complete 125 get_args call: %s\n", error.message);
        exit (1);
    }
    fprintf (stdout, "Get all the name total %d\n", service_list_len);

    printf("Got Reply: %d\n", service_list_len);
    for( i=0; i< service_list_len; i++){
        fprintf( stdout, "Service Names %s\n", service_list[i] ); 
    }

   // free reply and close connection
   dbus_message_unref(msg);   
}

void* calling_method(DBusConnection* conn){
   DBusMessage* msg;
   DBusMessageIter args;
   DBusPendingCall* pending;

   char* param = "calling you with something in the param";

   char* target = "com.novell.PushServer.Proxy"; // target for the method call;
   char* object = "/com/novell/PushServer"; // object name of the signal
   char* interface = "com.novell.PushServer.Proxy.Interface"; // interface name of the signal
   char* name = "RemoteName"; // name of the method
   msg = dbus_message_new_method_call( target, object, interface, name);
   if (NULL == msg) { 
      fprintf(stderr, "Message Null\n");
      exit(1);
   }

   // append arguments
   dbus_message_iter_init_append(msg, &args);
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   }

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

void loop_signal( rbus_signal_type_pointer cur, int level ){
	int i=0;
	rbus_arg_type_pointer child; 
	if( cur != NULL ) {
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<signal name = %s >\n", cur->name );
		for(  i=0; i< cur->output_count ; i++ ){
			child = cur->output_list[i]; 	
			if( child != NULL ){
				for( i=0; i < level+1; i++ ){
					fprintf(stdout, "\t");
				}
				fprintf(stdout, "<arg name = %s dirction=\"out\" type= %s />\n", child->name, child->type );
			}
		}
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<signal/>\n");
	}
}
void loop_method( rbus_method_type_pointer cur, int level ){
	int i=0;
	rbus_arg_type_pointer child; 
	if( cur != NULL ) {
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<method name = %s >\n", cur->name );
		for(  i=0; i< cur->input_count ; i++ ){
			child = cur->input_list[i]; 	
			if( child != NULL ){
				for( i=0; i < level+1; i++ ){
					fprintf(stdout, "\t");
				}
				fprintf(stdout, "<arg name = %s dirction=\"in\" type= %s />\n", child->name, child->type );
			}
		}
		for(  i=0; i< cur->output_count ; i++ ){
			child = cur->output_list[i]; 	
			if( child != NULL ){
				for( i=0; i < level+1; i++ ){
					fprintf(stdout, "\t");
				}
				fprintf(stdout, "<arg name = %s dirction=\"out\" type= %s />\n", child->name, child->type );
			}
		}
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<method/>\n");
	}
}
void loop_interface( rbus_interface_type_pointer cur, int level ){
	int i=0;
	rbus_method_type_pointer child; 
	rbus_signal_type_pointer s_child; 
	if( cur != NULL ) {
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<interface name = %s >\n", cur->name );
		for(  i=0; i< cur->method_count ; i++ ){
			child = cur->method_list[i]; 	
			if( child != NULL ){
				loop_method( child, level+1 ); 
			}
		}
		for(  i=0; i< cur->signal_count ; i++ ){
			s_child = cur->signal_list[i]; 	
			if( s_child != NULL ){
				loop_signal( s_child, level+1); 
			}
		}
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<interface/>\n" );
	}
}

void loop_node( rbus_node_type_pointer cur, int level ){
	int i=0;
	rbus_node_type_pointer child; 
    	rbus_interface_type_pointer i_child; 
	if( cur != NULL ) {
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<node name = %s >\n", cur->name );
		for(  i=0; i< cur->node_count ; i++ ){
			child = cur->node_list[i]; 	
			if( child != NULL ){
				loop_node( child, level+1 ); 
			}
		}
		for(  i=0; i< cur->interface_count ; i++ ){
			i_child = cur->interface_list[i]; 	
			if( i_child != NULL ){
				loop_interface( i_child, level+1 );
			}
		}
		for( i=0; i < level; i++ ){
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "<node/>\n" ); 
	}
}

int main() { 
    fprintf(stdout, "DBus testing 123\n" );
    //DBusConnection* session_bus;
    //session_bus = get_session_connected(); 
    //sending_signal( session_bus );
    //request_name_for_connection( session_bus );
    //calling_method( session_bus );
    int i=1000;
    while( i-- ){
    //while( 1 ){
    rbus_dbus_object_type_pointer result = parser_object_name( "org.gnome.Tomboy" );

    if( result != NULL ){
	fprintf(stdout, "org.gnome.Tomboy service get_object %s\n", result->name );
	loop_node( result->root, 0 );
    }
    free_dbus_object( result );
    //sleep(1);
    }
    return 0;
}
