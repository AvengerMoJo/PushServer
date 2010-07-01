#include <stdio.h>
#include <string.h>
#include <dbus/dbus.h>
#include <libxml/parser.h>
#include <include/remotebus-internal.h>
#include <include/rbus-dbus-object-parser.h>

void inline add_node( rbus_node_type_pointer parent, rbus_node_type_pointer child )
{
	int i;
        rbus_node_type_pointer *tmp;
	if( child != NULL ) { 
		child->parent = parent; 
		if( parent->node_count >= 0 ){
			tmp = calloc( parent->node_count+1, sizeof( rbus_node_type_pointer ));
			for( i=0; i<parent->node_count; i++){
				tmp[i] = parent->node_list[i]; 
			}
			free( parent->node_list );
			parent->node_list = tmp; 
			parent->node_list[parent->node_count] = child; 
			parent->node_count++;
		}
	}
}

void inline add_interface( rbus_node_type_pointer parent, rbus_interface_type_pointer child )
{
	int i;
        rbus_interface_type_pointer *tmp;
	if( child != NULL ) { 
		child->parent = parent; 
		if( parent->interface_count >= 0 ){
			tmp = calloc( parent->interface_count+1, sizeof( rbus_interface_type_pointer ));
			for( i=0; i<parent->interface_count; i++){
				tmp[i] = parent->interface_list[i]; 
			}
			free( parent->interface_list );
			parent->interface_list = tmp; 
			parent->interface_list[parent->interface_count] = child; 
			parent->interface_count++;
		}
	}
}

void inline add_method( rbus_interface_type_pointer interface, rbus_method_type_pointer method )
{
	int i;
        rbus_method_type_pointer *tmp;
	if( method != NULL ) { 
		method->parent = interface; 
		if( interface->method_count >= 0 ){
			tmp = calloc( interface->method_count+1, sizeof( rbus_method_type_pointer ));
			for( i=0; i<interface->method_count; i++){
				tmp[i] = interface->method_list[i]; 
			}
			free( interface->method_list );
			interface->method_list = tmp; 
			interface->method_list[interface->method_count] = method; 
			interface->method_count++;
		}
	}
}

void inline add_signal( rbus_interface_type_pointer interface, rbus_signal_type_pointer signal )
{
        int i;
        rbus_signal_type_pointer *tmp;
	if( signal != NULL ) { 
		signal->parent = interface; 
		if( interface->signal_count >= 0 ){
			tmp = calloc( interface->signal_count+1, sizeof( rbus_signal_type_pointer ));
			for( i=0; i<interface->signal_count; i++){
				tmp[i] = interface->signal_list[i];
			}
			free( interface->signal_list );
			interface->signal_list = tmp;
			interface->signal_list[interface->signal_count] = signal;
			interface->signal_count++;
		}
	}
}

#define method_add_in_arg(m,a) add_in_arg(m,a)
#define method_add_out_arg(m,a) add_out_arg(m,a)
#define signal_add_in_arg(s,a) add_in_arg(s,a)
#define signal_add_out_arg(s,a) add_out_arg(s,a)

#define add_in_arg(x,a) {								\
	int i;										\
        rbus_arg_type_pointer *tmp;							\
        if( x->input_count>= 0 ){							\
                tmp = calloc( x->input_count+1, sizeof( rbus_arg_type_pointer ));	\
		for( i=0; i< x->input_count; i++){					\
			tmp[i] = x->input_list[i]; 					\
		}									\
		free( x->input_list ); 							\
		x->input_list = tmp; 							\
		x->input_list[x->input_count] = a; 					\
        	x->input_count++;							\
	}										\
}

#define add_out_arg(x,a) {								\
	int i;										\
        rbus_arg_type_pointer *tmp;							\
        if( x->output_count>= 0 ){							\
                tmp = calloc( x->output_count+1, sizeof( rbus_arg_type_pointer ));	\
		for( i=0; i< x->output_count; i++){					\
			tmp[i] = x->output_list[i]; 					\
		}									\
		free( x->output_list );							\
		x->output_list = tmp; 							\
		x->output_list[x->output_count] = a; 					\
        	x->output_count++;							\
	}										\
}

void inline parse_method( rbus_method_type_pointer method, xmlNodePtr method_node )
{
	char* io; 
	rbus_arg_type_pointer  new_arg; 
	xmlNodePtr child = method_node->xmlChildrenNode;
	while( child != NULL ){
		if ((!xmlStrcmp(child->name, (const xmlChar *)"arg"))){
			new_arg = calloc( 1, sizeof( rbus_arg_type ) );
			new_arg->name = xmlGetProp(child, "name");
			new_arg->type = xmlGetProp(child, "type");
			io = xmlGetProp(child, "direction");
			if( strcmp( io, "in" )==0 ) {
				_rbus_verbose( "Find a input arg name %s, type %s\n", new_arg->name, new_arg->type );
				method_add_in_arg( method, new_arg ); 	
			} else if( strcmp( io, "out" ) == 0 ){ 
				_rbus_verbose( "Find a output arg name %s, type %s\n", new_arg->name, new_arg->type );
				method_add_out_arg( method, new_arg ); 	
			} 
			free( io );
		}
		child = child->next;
	}
}

void inline parse_signal( rbus_signal_type_pointer signal, xmlNodePtr signal_node )
{
	char* io; 
	rbus_arg_type_pointer  new_arg; 
	xmlNodePtr child = signal_node->xmlChildrenNode;
	while( child != NULL ){
		if ((!xmlStrcmp(child->name, (const xmlChar *)"arg"))){
			new_arg = calloc( 1, sizeof( rbus_arg_type ));
			new_arg->name = xmlGetProp(child, "name");
			new_arg->type = xmlGetProp(child, "type");
			io = xmlGetProp(child, "direction");
			if( strcmp( io, "in" )==0 ) {
				_rbus_verbose( "Find a input arg name %s, type %s\n", new_arg->name, new_arg->type );
				signal_add_in_arg( signal, new_arg ); 	
			} else if( strcmp( io, "out" ) == 0 ){ 
				_rbus_verbose( "Find a output arg name %s, type %s\n", new_arg->name, new_arg->type );
				signal_add_out_arg( signal, new_arg ); 	
			}
			free( io );
		}
		child = child->next;
	}
}

void inline parse_interface( rbus_interface_type_pointer new_interface, xmlNodePtr interface_node )
{
	rbus_method_type_pointer  new_method; 
	rbus_signal_type_pointer  new_signal; 
	xmlNodePtr child = interface_node->xmlChildrenNode;
	while( child != NULL ){
		if ((!xmlStrcmp(child->name, (const xmlChar *)"method"))){
			new_method = calloc( 1, sizeof( rbus_method_type ));
			new_method->name = xmlGetProp(child, "name");
			_rbus_verbose( "Find a method name %s\n", new_method->name );
			add_method( new_interface, new_method ); 
			parse_method( new_method, child );
		} else if((!xmlStrcmp(child->name, (const xmlChar *)"signal"))){
			new_signal = calloc( 1, sizeof( rbus_signal_type ));
			new_signal->name = xmlGetProp(child, "name");
			_rbus_verbose( "Find a signal name %s\n", new_signal->name );
			add_signal( new_interface, new_signal );
			parse_signal( new_signal, child );
		} else if((!xmlStrcmp(child->name, (const xmlChar *)"annotation"))){
			_rbus_verbose( "Find a %s\n", child->name );
		} 
		child = child->next;
	}
}

char* get_object_xml( DBusConnection *session, char *object_name, char *path )
{
	DBusMessage	*result_msg;
	DBusError	error;
	DBusMessage	*introspect_call;
	char		*xml_data = NULL;
	char		*result = NULL;

	dbus_error_init(&error);

	if( strlen(path)==0 )
		introspect_call = dbus_message_new_method_call( object_name, "/" , DBUS_INTERFACE_INTROSPECTABLE, "Introspect" );
	else
		introspect_call = dbus_message_new_method_call( object_name, path , DBUS_INTERFACE_INTROSPECTABLE, "Introspect" );

	if( NULL == introspect_call ) {
		_rbus_verbose( "Can't get introspection method\n" );
		dbus_error_free(&error);
		return NULL;
	} else {
		_rbus_verbose( "Instrospector method obtained\n" );
		result_msg = dbus_connection_send_with_reply_and_block ( session, introspect_call, -1, &error);
		if( NULL == result_msg ) {
			_rbus_verbose( "Introspect Reply Null\n" );
			dbus_message_unref(introspect_call);
			dbus_error_free(&error);
			return NULL;
		} else {
			_rbus_verbose( "Reply message obtained\n" );
			if (!dbus_message_get_args (result_msg, &error,
					DBUS_TYPE_STRING, &xml_data,
					DBUS_TYPE_INVALID )) {
				_rbus_verbose( "Failed to get_args call: %s\n", error.message );
				dbus_message_unref(result_msg);
				dbus_message_unref(introspect_call);
				dbus_error_free(&error);
				return NULL;
			}
			_rbus_verbose( "Got Reply: \n%s\n", xml_data );
			dbus_message_unref(result_msg);
			dbus_message_unref(introspect_call);
			dbus_error_free(&error);
			return strdup( xml_data );
		}
	}
}

rbus_node_type_pointer create_object( DBusConnection *session, char *object_name, char* path)
{
	char* xml;
	char* node_name;
	char* new_node_name;
	xmlDocPtr doc; /* the resulting document tree */
	xmlNodePtr root;
	xmlNodePtr child;
	rbus_node_type_pointer new_node;
	rbus_interface_type_pointer new_interface;

	rbus_node_type_pointer node = calloc( 1, sizeof( rbus_node_type )); 
	node->name = strdup( path ); 

	xml = get_object_xml( session, object_name, path );
	if( xml == NULL )
		return node;

    	doc = xmlReadMemory(xml, strlen(xml), "nothing.xml", NULL, 0);
	if (doc == NULL) {
		_rbus_verbose( "Failed to parse document\n");
		free(xml);
	} else {
		root = xmlDocGetRootElement(doc);
		if (root == NULL) {
			_rbus_verbose( "empty document\n");
		} else {
			if (xmlStrcmp(root->name, (const xmlChar *) "node")) {
				_rbus_verbose( "document of the wrong type, root node != node");
			} else { 
				child = root->xmlChildrenNode;
				while (child != NULL) {
					if ((!xmlStrcmp(child->name, (const xmlChar *)"node"))){
						// loop_node(doc, cur);
						node_name = xmlGetProp(child, "name");
						new_node_name = calloc( strlen(path)+strlen(node_name)+2, sizeof(char) );
						strcat( new_node_name, path );
						strcat( new_node_name, "/" );
						strcat( new_node_name, node_name);
						_rbus_verbose( "Find a Child name %s\n", new_node_name);
						new_node = create_object( session, object_name, new_node_name );
						free( node_name );
						free( new_node_name );
						add_node( node, new_node ); 
					} else if (!xmlStrcmp(child->name, (const xmlChar *)"interface")){
        					new_interface = calloc( 1, sizeof( rbus_interface_type ));
						new_interface->name = xmlGetProp(child, "name");
						_rbus_verbose( "Find a Interface %s\n", new_interface->name );
						add_interface( node, new_interface ); 
						parse_interface( new_interface, child ); 
					} 
//else fprintf(stdout, "Find a %s\n", child->name );
					child = child->next;
				}
			}
		}
		free( xml );
    		xmlFreeDoc(doc);
	}
	return node;
}

rbus_dbus_object_type_pointer parser_object_name( char* object_name )
{
	DBusConnection* session;
	DBusError error;
	dbus_uint32_t result;
	rbus_dbus_object_type_pointer dbus_object;
	char* connection_name = "com.novell.PushServer.Parser"; // target name for the consumer

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
			_rbus_verbose( "Not Primary Owner (%d)\n", result);
		}
		if( !dbus_bus_start_service_by_name( session, object_name, 0, &result, &error)){
			_rbus_verbose( "Can't start service %s\n", error.message);
			dbus_error_free(&error);
			return NULL;
		} else {
			_rbus_verbose( "Service started\n");
			dbus_object = calloc( 1, sizeof(rbus_dbus_object_type));		
			dbus_object->name = strdup( object_name ); 
			dbus_object->root = create_object( session, object_name, "" );

			result = dbus_bus_release_name(session, connection_name, &error);
			if (dbus_error_is_set(&error)) {
				_rbus_verbose( "Name Error (%s)\n", error.message);
				dbus_error_free(&error);
			}
			if( DBUS_RELEASE_NAME_REPLY_RELEASED != result ) {
				_rbus_verbose( "Something wrong with releasing the name (%d)\n", result);
			}
		}
	}
        return dbus_object;
} 


void free_signal( rbus_signal_type_pointer signal )
{
        int i=0;
        rbus_arg_type_pointer child;
        if( signal != NULL ) {
                for(  i=0; i< signal->output_count ; i++ ){
                        child = signal->output_list[i];
                        if( child != NULL ){
				free( child->name );
				free( child->type );
                                free( child ); 
                        }
                }
		free( signal->output_list );
		signal->output_count=0;
		free( signal->name );
        }
}


void free_method( rbus_method_type_pointer method )
{
        int i=0;
        rbus_arg_type_pointer child;
        if( method != NULL ) {
                for(  i=0; i< method->input_count ; i++ ){
                        child = method->input_list[i];
                        if( child != NULL ){
				free( child->name );
				free( child->type );
                                free( child ); 
                        }
                }
		free( method->input_list );
		method->input_count=0;
                for(  i=0; i< method->output_count ; i++ ){
                        child = method->output_list[i];
                        if( child != NULL ){
				free( child->name );
				free( child->type );
                                free( child ); 
                        }
                }
		free( method->output_list );
		method->output_count=0;
		free( method->name );
        }
}


void free_interface( rbus_interface_type_pointer interface )
{
	int i=0;
        rbus_method_type_pointer child;
        rbus_signal_type_pointer s_child;
	if( interface != NULL ) {
		for(  i=0; i< interface->method_count ; i++ ){
                        child = interface->method_list[i];
                        if( child != NULL ){
                                free_method( child );
                        }
			free( child ); 
                }
		free( interface->method_list ); 
                for(  i=0; i< interface->signal_count ; i++ ){
                        s_child = interface->signal_list[i];
                        if( s_child != NULL ){
                                free_signal( s_child );
                        }
			free( s_child ); 
                }
		free( interface->signal_list ); 
		free( interface->name );
	}
}
void free_node( rbus_node_type_pointer node)
{
	int i=0;
        rbus_node_type_pointer child;
        rbus_interface_type_pointer i_child;
	if( node != NULL ) {
                for(  i=0; i< node->node_count ; i++ ){
                        child = node->node_list[i];
                        if( child != NULL ){
                                free_node( child );
                        }
			free( child );
                }
		free( node->node_list );
                for(  i=0; i< node->interface_count ; i++ ){
                        i_child = node->interface_list[i];
                        if( i_child != NULL ){
                                free_interface( i_child );
                        }
			free( i_child );
                }
		free( node->interface_list );
		free( node->name );
	}
}

void free_dbus_object( rbus_dbus_object_type_pointer object)
{
	if( object != NULL ){
		free_node( object->root );
		free( object->name );
		free( object->root );
		free( object );
	}
}
