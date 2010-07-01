dbus_includedir=/usr/include/dbus-1.0/

INCLUDES=-I$(top_srcdir) -I$(top_srcdir)/remotebus $(DBUS_CFLAGS) $(LIBXML_CFLAGS)

EXTRA_DIST=


TESTS=rbus-test rbus-create-method-test rbus-call-name-test rbus-all-method-type-test rbus-call-all-method-type-test

## we use noinst_PROGRAMS not check_PROGRAMS so that we build 
## even when not doing "make check"
noinst_PROGRAMS=$(TESTS)

rbus_test_SOURCES=                              \
        rbus-test.c
rbus_test_LDADD=$(top_builddir)/remotebus/librbus.la $(DBUS_LIBS) $(LIBXML_LIBS) 

rbus_create_method_test_SOURCES=		\
	rbus-create-method-test.c
rbus_create_method_test_LDADD=$(top_builddir)/remotebus/librbus.la $(DBUS_LIBS) $(LIBXML_LIBS)

rbus_call_name_test_SOURCES=			\
	rbus-call-name-test.c
rbus_call_name_test_LDADD=$(top_builddir)/remotebus/librbus.la $(DBUS_LIBS) $(LIBXML_LIBS)

rbus_all_method_type_test_SOURCES=		\
	rbus-all-method-type-test.c
rbus_all_method_type_test_LDADD=$(top_builddir)/remotebus/librbus.la $(DBUS_LIBS) $(LIBXML_LIBS)

rbus_call_all_method_type_test_SOURCES=		\
	rbus-call-all-method-type-test.c
rbus_call_all_method_type_test_LDADD=$(top_builddir)/remotebus/librbus.la $(DBUS_LIBS) $(LIBXML_LIBS)
