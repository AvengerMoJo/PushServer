#include <include/rbus-proxy-call.h>

DBusConnection *proxy_session( char* object_name )
{
        DBusConnection* session=NULL;
        DBusError error;
        dbus_uint32_t result;
        char* connection_name = "com.novell.PushServer.Proxy"; //  Proxy name to register in the session bus 

        dbus_error_init(&error);
        session = dbus_bus_get(DBUS_BUS_SESSION, &error);
        if (dbus_error_is_set(&error)) {
                _rbus_verbose( "Connection Error (%s)\n", error.message);
                dbus_error_free(&error);
        }
        if (NULL == session) {
                return NULL;
                //exit(1);
        } else {
                _rbus_verbose( "Session connected\n");
                // request our name on the bus and check for errors
                result = dbus_bus_request_name(session, connection_name, DBUS_NAME_FLAG_REPLACE_EXISTING , &error);
                if (dbus_error_is_set(&error)) {
                        _rbus_verbose( "Name Error (%s)\n", error.message);
                        dbus_error_free(&error);
                }
                if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != result) {
                        _rbus_verbose( "Not Primary Owner, should we able to get more then one proxy?  (%d)\n", result);
                }
                if( !dbus_bus_start_service_by_name( session, object_name, 0, &result, &error)){
                        _rbus_verbose( "Can't start service %s\n", error.message);
                        dbus_error_free(&error);
                        return NULL;
                } else {
                        _rbus_verbose( "Requested service started\n");
			return session;
		}
	}
}

void create_input_dict( DBusMessageIter *iter, const char *key, char *type_str, void *data)
{
  DBusMessageIter entry, variant;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, "sv", &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
  //func(&entry, data);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, type_str, &variant);
  //func(&variant, data);
  dbus_message_iter_close_container(&entry, &variant);
  dbus_message_iter_close_container(iter, &entry);
}


dbus_bool_t create_input_message( DBusMessage *message, rbus_method_type_pointer method, int type, va_list args )
{
	return dbus_message_append_args_valist( message, type, args ); 
}

dbus_bool_t create_input_message_2( DBusMessage *message, rbus_method_type_pointer method, va_list args )
{
	DBusMessageIter in_args;
	int i, type, next;
	if( message == NULL || method == NULL )
		return;
	_rbus_verbose( "creating input messages with object\n");
	dbus_message_iter_init_append (message, &in_args);
	for( i=0; i < method->input_count; i++ ) { 
		int s; 
		rbus_arg_type_pointer cur_arg = method->input_list[i]; 
		for( s=0; s< strlen( cur_arg->type ); s++ ) { 
			char type;
			char ele_type;
			RBusBasicValue *value;
			type = cur_arg->type[s]; 
			switch( type ) { 
				case 'a':
					s++;
					ele_type = cur_arg->type[s];
					_rbus_verbose( "get array type -->a<-- with -->%s<-- inside\n", ele_type);
					if( dbus_type_is_fixed( ele_type ) ) {
						DBusMessageIter array;
						int ele_count;
						char buf[2]; 
						buf[0]=ele_type;
						buf[1]='\0';
						if (!dbus_message_iter_open_container (&in_args, DBUS_TYPE_ARRAY, buf, &array)) { 
							return FALSE;
						}
						value = (RBusBasicValue*) va_arg (args, const RBusBasicValue**);
						ele_count = va_arg (args, int); 
						if (!dbus_message_iter_append_fixed_array (&array, ele_type, value, ele_count)) {
							return FALSE;
						}
						if (!dbus_message_iter_close_container (&in_args, &array)) {
							return FALSE;
						}
					} else { 
						_rbus_verbose( "get smoething complicated-->a<-- with -->%s<-- inside\n", ele_type);
					}
				case 'v':
				case 'r':
				case 'e':
				case '{':
					break;
				default : // this should be all basic type now 
					_rbus_verbose( "get basic type -->%s<-- \n", type);
					value = (RBusBasicValue*) va_arg ( args, const RBusBasicValue*);
					if (!dbus_message_iter_append_basic (&in_args, type, value)) { 
						return FALSE; 
					}
/*
				case 'y': type = 
				case 'b':
				case 'n':
				case 'q':
				case 'i':
				case 'u':
				case 'x':
				case 't':
				case 's':
				case 'o':
				case 'g':
*/
			}
		} // loop args type length 
	} // loop method - input end 
} 

dbus_bool_t create_output_message( DBusMessage *message, rbus_method_type_pointer method)
{
	va_list my_list;
	int list_count=0; 
	int i;
	if( message == NULL || method == NULL ) {
		return FALSE;
	}
	void **returnlist_pointer; 
	returnlist_pointer = calloc( method->output_count, sizeof( void * )); 
	for( i=0; i< method->output_count; i++){
		int type_len=0,x;
		_rbus_verbose( "Creating reply args list for %s, type -->%s<--\n", method->output_list[i]->name, method->output_list[i]->type );
		type_len=strlen( method->output_list[i]->type ); 	
		list_count += type_len; 
		for( x=0; x< type_len; x++ ){
			if( method->output_list[i]->type[x] == 'a' ) { 
				x++;
				if( method->output_list[i]->type[x] == '{' ){ 
				}
			}
		}
	}
	my_list = calloc( list_count+1, sizeof( va_list ) ); 
	for( i=0; i< method->output_count; i++){
		int args_len = strlen( method->output_list[i]->type ); 
		int x;
		for( x=0; x< args_len; x++){
			if( method->output_list[i]->type[x] == 'a' ){
				x++; 
			}
		}
	}
	 
/*
				if (!dbus_message_get_args (reply, &error, 
								DBUS_TYPE_ARRAY,  DBUS_TYPE_STRING, &g_service_list, &g_service_list_len,
								DBUS_TYPE_INVALID )) { 
	
dbus_message_get_args_valist (DBusMessage     *message,
                              DBusError       *error,
                              int              first_arg_type, va_list );
*/
}
void *create_output_result( DBusMessageIter *message, void *result, char* type )
{
	DBusMessageIter subiter;
	DBusMessageIter subsub;
	int current_type = dbus_message_iter_get_arg_type (message);
	_rbus_verbose("Current type ->%c<-, recorded type ->%c<-\n", current_type, type[0] );
	if( current_type == DBUS_TYPE_INVALID)
		return NULL;

	switch( type[0] ) {
		case 'v':
			{
			dbus_message_iter_recurse (message, &subiter);
			int type = dbus_message_iter_get_arg_type (message);
			//char* type = dbus_message_iter_get_signature( &subiter ); 
			_rbus_verbose("variant type ->%d<-\n", type );
			//if( dbus_signature_validate( type, NULL ) ){
				//_rbus_verbose("validate variant type ->%d<-\n", type );
				return create_output_result( &subiter, result, type );
			//}
			//}
			}
		case 'r':
		case 'e':
		case '{':
			{ 
				void *key, *value; 
				void **sub_result = calloc( sizeof(void*), 2 );
				dbus_message_iter_recurse (message, &subiter);
				key = create_output_result(&subiter, result, &type[1] );
				dbus_message_iter_next (&subiter);
				value = create_output_result(&subiter, result, &type[2] );
				sub_result[0] = key; 
				sub_result[1] = value; 
				return sub_result; 
			}
		case 'a':
			dbus_message_iter_recurse (message, &subiter);
			_rbus_verbose("subiter type ->%c<-\n", dbus_message_iter_get_arg_type(&subiter) );
			if(dbus_message_iter_get_element_type (message) == 'e' ) { 
				_rbus_verbose("Element type->%c<-\n", dbus_message_iter_get_element_type (message)  );
				return create_output_result(&subiter, result , &type[1] );
			}
			while ((current_type = dbus_message_iter_get_arg_type (&subiter)) != DBUS_TYPE_INVALID) {
				void* sub_result;
				_rbus_verbose("Array current type ->%c<-, recorded type ->%c<-\n", current_type, type[1] );
				create_output_result(&subiter, &sub_result, &type[1] );
				dbus_message_iter_next (&subiter);
			}
			break; 
		default :
			dbus_message_iter_get_basic (message, &result);
			_rbus_verbose("get type ->%c<-, recorded result->%s<-\n", current_type, result );
			return result;
			break;

	}
}

void** method_object_call_blocked( rbus_dbus_object_type_pointer dbus_object, rbus_method_type_pointer method, int first_arg, ... )
{
        DBusConnection *session=NULL;
        DBusError error;
	va_list args;
	_rbus_verbose( "Calling remote method by object\n");
	if( dbus_object != NULL && method != NULL ) {
        	dbus_error_init(&error);
		session = proxy_session( dbus_object->name );
		if( session != NULL ) { 
			DBusMessage *message;
			DBusMessage *reply;
			message = dbus_message_new_method_call ( dbus_object->name , method->parent->parent->name, method->parent->name, method->name );
			_rbus_verbose( "Remote method message intraface created, does it need arg? %d\n", method->input_count );
			if( method->input_count > 0 ) {
				DBusMessageIter in_args;
				void *value;
				va_start (args, first_arg );
				dbus_message_append_args_valist( message, first_arg, args);
				_rbus_verbose( "dbus args_list appended \n" );
/*
				dbus_message_iter_init_append(message, &in_args);
				if (!create_input_message( message, method, first_arg, args )) {
					va_end (args);
					return;
				}
*/
				va_end (args);
			}
			reply = dbus_connection_send_with_reply_and_block (session, message, 1000, &error);
			if (dbus_error_is_set (&error)) {
				_rbus_verbose("Error: %s\n", error.message);
                		dbus_error_free(&error);
				return ;
			}
			_rbus_verbose( "does it need reply? %d\n", method->output_count );
			if( method->output_count > 0 ) {
				DBusMessageIter loop;
				int current_type, i=0;
				void **return_list = calloc( 1, sizeof(void**) );
				return_list = calloc( method->output_count+2, sizeof(void*) );
				if( !dbus_message_iter_init ( reply , &loop) ) { 
					_rbus_verbose("Cannot look into the reply message\n");
					return;
				}
				while ((current_type = dbus_message_iter_get_arg_type (&loop)) != DBUS_TYPE_INVALID) {
					_rbus_verbose("Current type ->%c<-, recorded type ->%s<-\n", current_type, method->output_list[i]->type );
					void *tmp = create_output_result( &loop, &return_list[i], method->output_list[i]->type );
					return_list[i] = tmp;
					//printf("->%s<-\n", return_list[i]);
					i++;
					dbus_message_iter_next (&loop);
				}
				return_list[i]=NULL;
				return return_list;

				/*
				if (!create_output_message( reply, method )) {
					return ;
				}
*/
			}
		}
	}
}

