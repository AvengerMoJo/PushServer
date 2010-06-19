
dbus_includedir=/usr/include/dbus-1.0/

INCLUDES=-I$(top_builddir) -I$(top_srcdir) $(DBUS_CFLAGS) $(LIBXML_CFLAGS)

remotebus_internaldir=$(libdir)/remotebus/include/remotebus
remotebus_internal_HEADERS=  \
	include/remotebus-internal.h	\
	include/remotebus-type.h

lib_LTLIBRARIES=librbus.la

noinst_LTLIBRARIES=librbus-internal.la

RBUS_INTERNAL_SOURCES=				\
	rbus-internal.c				\
	rbus-dbus-object-parser.c		\
	rbus-dbus-object-parser.h



librbus_la_SOURCES=				\
        $(RBUS_INTERNAL_SOURCES)

librbus_internal_la_SOURCES=			\
        $(RBUS_INTERNAL_SOURCES)

librbus_internal_la_LIBADD=			\
	$(LIBXML_LIBS)


BUILT_SOURCES=$(remotebus_internal_HEADERS)
EXTRA_DIST=include/remotebus-type.h.in


TESTS=rbus-test rbus-create-method-test rbus-call-name-test

## we use noinst_PROGRAMS not check_PROGRAMS so that we build 
## even when not doing "make check"
noinst_PROGRAMS=$(TESTS)

rbus_test_SOURCES=                              \
        rbus-test.c
rbus_test_LDADD=$(DBUS_LIBS) $(LIBXML_LIBS) librbus-internal.la

rbus_create_method_test_SOURCES=		\
	rbus-create-method-test.c
rbus_create_method_test_LDADD=$(DBUS_LIBS)

rbus_call_name_test_SOURCES=				\
	rbus-call-name-test.c
rbus_call_name_test_LDADD=$(DBUS_LIBS)
