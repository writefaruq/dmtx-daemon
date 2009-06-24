#AM_CFLAGS = -Wall -g

bin_PROGRAMS = dmtxd

dmtxd_SOURCES = dmtxd.c inotify.c inotify.h symbol.c symbol.h dmtxutils.c \
		dmtxutils.h  dmtxplugin-gdbus.c dmtxplugin-gdbus.h \
		service-gdbus.c service-gdbus.h utils.c utils.h

AM_CFLAGS = -Wall -g -I/usr/local/include -D_MAGICK_CONFIG_H \
                @BLUEZ_CFLAGS@ @DBUS_CFLAGS@ @GLIB_CFLAGS@ @GDBUS_CFLAGS@

dmtxd_LDFLAGS = -ldmtx -lWand -lMagick

LD_ADD = @LIBDMTX_CFLAGS@ @MAGICK_LIBS@ \
        @GDBUS_LIBS@ @GLIB_LIBS@ @DBUS_LIBS@ @BLUEZ_LIBS@
