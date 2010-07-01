#ifndef _RBUS_DBUS_OBJECT_PARSER_H_
#define _RBUS_DBUS_OBJECT_PARSER_H_

#include <include/remotebus-type.h>

RBUS_EXTERN_START

typedef struct _rbus_arg_type { 
	char	*name; 
	char	*type; 
} rbus_arg_type;
typedef rbus_arg_type *rbus_arg_type_pointer; 

typedef struct _rbus_signal_type { 
	char				*name; 
	rbus_arg_type_pointer		*input_list; 
	int				input_count;
	rbus_arg_type_pointer		*output_list; 
	int				output_count;
	struct _rbus_interface_type	*parent;
} rbus_signal_type;
typedef rbus_signal_type *rbus_signal_type_pointer; 

typedef struct _rbus_method_type { 
	char				*name; 
	rbus_arg_type_pointer		*input_list; 
	int				input_count;
	rbus_arg_type_pointer		*output_list; 
	int				output_count;
	struct _rbus_interface_type	*parent;
} rbus_method_type;
typedef rbus_method_type *rbus_method_type_pointer; 


typedef struct _rbus_interface_type { 
	char				*name; 
	rbus_method_type_pointer	*method_list; 
	int				method_count;
	rbus_signal_type_pointer	*signal_list; 
	int				signal_count;
	struct _rbus_node_type		*parent; 
}rbus_interface_type;
typedef rbus_interface_type *rbus_interface_type_pointer; 

typedef struct _rbus_node_type { 
	char				*name; 
	struct _rbus_node_type		**node_list; 
	int				node_count;
	rbus_interface_type_pointer	*interface_list; 
	int				interface_count;
	struct _rbus_node_type		*parent; 
} rbus_node_type;
typedef rbus_node_type *rbus_node_type_pointer; 

typedef struct _rbus_dbus_object { 
	char			*name;
	rbus_node_type_pointer	root; 
} rbus_dbus_object_type;
typedef rbus_dbus_object_type *rbus_dbus_object_type_pointer; 

rbus_dbus_object_type_pointer parser_object_name( char	*object_name );

void free_dbus_object( rbus_dbus_object_type_pointer object);

RBUS_EXTERN_END

#endif // _RBUS_DBUS_OBJECT_PARSER_H_
