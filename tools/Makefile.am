dbus_includedir=/usr/include/dbus-1.0/

INCLUDES=-I$(top_srcdir) -I$(top_srcdir)/remotebus $(DBUS_CFLAGS) $(LIBXML_CFLAGS) $(GTK2_CFLAGS)

bin_PROGRAMS=dbus-xray

dbus_xray_SOURCES=		\
	gtk-dbus-xray.c



dbus_xray_LDADD= $(top_builddir)/remotebus/librbus.la $(LIBXML_LIBS) $(GTK2_LIBS) $(DBUS_LIBS) 
dbus_xray_LDFLAGS=@R_DYNAMIC_LDFLAG@


#EXTRA_DIST=


## we use noinst_PROGRAMS not check_PROGRAMS so that we build 
## even when not doing "make check"
#noinst_PROGRAMS=
