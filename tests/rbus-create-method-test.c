#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <include/remotebus-internal.h>
#include <dbus/dbus.h>

void reply_to_method_call(DBusMessage* msg, DBusConnection* conn);

char** g_service_list;   // we need to create some kind of data strcuture for it 
int g_service_list_len;   // we need to create some kind of data strcuture for it 

DBusConnection* get_system_connected()
{
    DBusError err;
    DBusConnection* conn;
    int ret;
    // initialise the errors
    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        exit(1);
    } else { 
        fprintf(stdout, "system bus connected\n" );
	return conn;
    }
    return NULL;
}

DBusConnection* get_session_connected(){
    DBusError err;
    DBusConnection* conn;
    int ret;
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

void request_name_for_connection( DBusConnection* conn ){
   DBusError err;
   int ret;
   char* connection_name = "com.novell.PushServer.Proxy"; // target name for the consumer

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




void main_loop(DBusConnection* conn){
   DBusMessage* msg;
   // loop, testing for new messages
   while (TRUE) {
      // non blocking read of the next available message
      dbus_connection_read_write(conn, 0);
      msg = dbus_connection_pop_message(conn);

      // loop again if we haven't got a message
      if (NULL == msg) { 
         sleep(1); 
         continue; 
      }

      // check this is a method call for the right interface and method
      if (dbus_message_is_method_call(msg, "com.novell.PushServer.Proxy.Interface", "RemoteName"))
         reply_to_method_call(msg, conn);
      if (dbus_message_is_method_call(msg, "com.novell.PushServer.Proxy.Interface", "RemoteNameDestination"))
         reply_to_method_call(msg, conn);

      // free the message
      dbus_message_unref(msg);
   }
}

void reply_to_method_call(DBusMessage* msg, DBusConnection* conn) {
   DBusMessage* reply;
   DBusMessageIter args;
   DBusMessageIter list;
   dbus_bool_t stat = TRUE;
   dbus_uint32_t level = 21614;
   dbus_uint32_t serial = 0;
   char* param = "";
   int i=0;

   // read the arguments
   if (!dbus_message_iter_init(msg, &args))
      fprintf(stderr, "Message has no arguments!\n"); 
   else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) 
      fprintf(stderr, "Argument is not string!\n"); 
   else 
      dbus_message_iter_get_basic(&args, &param);
   printf("Somebody calling us with a name->%s<-\n", param);

   // create a reply from the message
   reply = dbus_message_new_method_return(msg);

   // add the arguments to the reply
/*
   if (!dbus_message_append_args(reply, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &g_service_list, g_service_list_len-1,
                               DBUS_TYPE_INVALID )) {
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   }
*/

   dbus_message_iter_init_append(reply, &args);
   if ( !dbus_message_iter_open_container (&args, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING , &list) ){
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   }

   for( i=0; i< g_service_list_len ; i++){
      if (!dbus_message_iter_append_basic(&list, DBUS_TYPE_STRING, &(g_service_list[i]))) { 
         fprintf(stderr, "Out Of Memory!\n"); 
         exit(1);
      } else {
         fprintf(stdout, "Sending ->%s<-\n", (g_service_list)[i] ); 
      }
   }

   if ( !dbus_message_iter_close_container (&args, &list) ){
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   }

   printf("Replay then with string array...\n");

   // send the reply && flush the connection
   if (!dbus_connection_send(conn, reply, &serial)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   }
   dbus_connection_flush(conn);

   // free the reply
   dbus_message_unref(reply);
}

int get_service_list(DBusConnection* conn){ 
    DBusError error;
    DBusMessage *message;
    DBusMessage *reply;
    int reply_timeout;
    //int service_list_len=-1;
    int i;
    char** stuff;

    char* target = "org.freedesktop.DBus";
    char* object = "/org/freedesktop/DBus"; 
    char* interface = "org.freedesktop.DBus"; 
    //char* method = "ListNames"; 
    char* method = "ListActivatableNames"; 
    char* more;

    dbus_error_init (&error);
    /* Construct the message */
    message = dbus_message_new_method_call ( target, object, interface, method );

    /* Call ListServices method */
    reply_timeout = -1;   /*don't timeout*/
    reply = dbus_connection_send_with_reply_and_block (conn, message, reply_timeout, &error);

    if (dbus_error_is_set (&error)) {
        fprintf (stderr, "Error: %s\n", error.message);
        exit (1);
    }
    /* Extract the data from the reply */
    if (!dbus_message_get_args (reply, &error, 
                               //DBUS_TYPE_ARRAY, &service_list,
                               //DBUS_TYPE_ARRAY_AS_STRING DBUS_TYPE_STRING_AS_STRING , service_list,
                               DBUS_TYPE_ARRAY,  DBUS_TYPE_STRING, &g_service_list, &g_service_list_len,
                               DBUS_TYPE_INVALID )) { 
        fprintf (stderr, "Failed to complete ListServices call: %s\n", error.message);
        exit (1);
    }
    fprintf (stdout, "Get all the name total %d\n", g_service_list_len);
    if( g_service_list_len> 0 ) { 
	for( i=0; i< g_service_list_len; i++){
            printf("service name ->%s<-\n", g_service_list[i] );
        }
    }
    dbus_message_unref (reply);
    dbus_message_unref (message);
    return g_service_list_len;
}

char** recreate_full_service_name( int len ){
    int i=0,x=0;
    char** result_list = calloc( len, sizeof( char* )) ;
    for( i=0; g_service_list[i]!=NULL ; i++) { 
        if( g_service_list[i][0]!=':' ){
    	    fprintf(stdout, "dupping %s\n", g_service_list[i] );
            result_list[x++]=strdup( g_service_list[i] ); 
            free(g_service_list[i]);
        }
    }
    result_list[x]= NULL;
    return result_list; 
}



int main() { 
    DBusConnection* system_bus;
    DBusConnection* session_bus;
    char** tmp;
    int service_number=0,i;
    int full_name=0;

/*
    fprintf(stdout, "Getting all DBUS system services started\n" );
    system_bus = get_system_connected();
    service_number = get_service_list( system_bus );
    if( service_number > 0 ) { 
	for( i=0; i< service_number; i++){
            if( g_service_list[i][0] != ':' ) {
                full_name++; 
            }
        }
        tmp = recreate_full_service_name( full_name );
        free( g_service_list ); 
        g_service_list = tmp;
        g_service_list_len = full_name;
    }
    i=0;
    printf("There are %d well known service name\n", g_service_list_len );
    while( g_service_list[i] != NULL )
            printf("service name ->%s<-\n", g_service_list[i++] );
    fprintf(stdout, "Getting all DBUS system services ended\n" );
*/

    fprintf(stdout, "Getting all DBUS session services started\n" );
    session_bus = get_session_connected(); 
    request_name_for_connection( session_bus );
    service_number = get_service_list( session_bus );
    if( service_number > 0 ) { 
	for( i=0; i< service_number; i++){
            if( g_service_list[i][0] != ':' ) {
                full_name++; 
            }
        }
        tmp = recreate_full_service_name( full_name );
        free( g_service_list ); 
        g_service_list = tmp;
        g_service_list_len = full_name;
    }
    i=0;
    printf("There are %d well known service name\n", g_service_list_len );
    while( g_service_list[i] != NULL )
            printf("service name ->%s<-\n", g_service_list[i++] );
    fprintf(stdout, "Getting all DBUS session services ended\n" );

    fprintf(stdout, "DBus getting all services started\n" );
    main_loop( session_bus );

    return 0;
}
