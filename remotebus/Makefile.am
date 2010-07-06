
dbus_includedir=/usr/include/dbus-1.0/

INCLUDES=-g -I$(top_builddir) -I$(top_srcdir) $(DBUS_CFLAGS) $(LIBXML_CFLAGS)

remotebus_internaldir=$(libdir)/remotebus/include/remotebus
remotebus_internal_HEADERS=  \
	include/remotebus-internal.h	\
	include/remotebus-type.h

lib_LTLIBRARIES=librbus.la

noinst_LTLIBRARIES=librbus-internal.la

RBUS_LIBRARY_SOURCES=				\
	rbus-get-service-names.c		\
	rbus-proxy-call.c			\
	include/rbus-dbus-object-parser.h	\
	include/rbus-get-service-names.h	\
	include/rbus-proxy-call.h

RBUS_INTERNAL_SOURCES=				\
	rbus-dbus-object-parser.c		\
	rbus-internal.c				\
	include/rbus-dbus-object-parser.h



librbus_la_SOURCES=				\
        $(RBUS_LIBRARY_SOURCES)			\
        $(RBUS_INTERNAL_SOURCES)

librbus_internal_la_SOURCES=			\
        $(RBUS_INTERNAL_SOURCES)

librbus_internal_la_LIBADD=			\
	$(LIBXML_LIBS)


BUILT_SOURCES=$(remotebus_internal_HEADERS)
EXTRA_DIST=include/remotebus-type.h.in

# librbus_la_LIBADD= 
## don't export symbols that start with "_" (we use this 
## convention for internal symbols)
librbus_la_LDFLAGS= -export-symbols-regex "^[^_].*" -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -no-undefined @R_DYNAMIC_LDFLAG@ @PIC_LDFLAGS@




TESTS=

## we use noinst_PROGRAMS not check_PROGRAMS so that we build 
## even when not doing "make check"
noinst_PROGRAMS=$(TESTS)

