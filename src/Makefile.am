#AM_CFLAGS = -Wall -g

bin_PROGRAMS = dmtxd

dmtxd_SOURCES = dmtxd.c dmtxd.h symbol.c symbol.h dmtxutils.c \
		dmtxutils.h  dmtxplugin-gdbus.c dmtxplugin-gdbus.h \
		service-gdbus.c service-gdbus.h utils.c utils.h \
		inotify.c inotify.h inotify-utils.c inotify-utils.h \
		inotify-eventqueue.c inotify-eventqueue.h

AM_CFLAGS = -Wall -g -I/usr/local/include -I/usr/include -D_MAGICK_CONFIG_H \
                @MAGICK_CFLAGS@ @BLUEZ_CFLAGS@ @DBUS_CFLAGS@ @GLIB_CFLAGS@ \
                @GDBUS_CFLAGS@

dmtxd_LDFLAGS = -ldmtx @LIBDMTX_CFLAGS@ @MAGICK_LIBS@ \
        @GDBUS_LIBS@ @GLIB_LIBS@ @DBUS_LIBS@ @BLUEZ_LIBS@ @MAGICK_LIBS@

LD_ADD = @LIBDMTX_CFLAGS@ @MAGICK_LIBS@ \
        @GDBUS_LIBS@ @GLIB_LIBS@ @DBUS_LIBS@ @BLUEZ_LIBS@ @MAGICK_LIBS@
