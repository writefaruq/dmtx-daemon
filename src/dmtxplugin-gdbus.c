/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.              *
 ***************************************************************************/

#include <dmtxplugin-gdbus.h>

struct context_data {
	gboolean found;
	char *bdaddr;
};

static DBusConnection *conn = NULL;

static void element_start(GMarkupParseContext *context,
		const gchar *element_name, const gchar **attribute_names,
		const gchar **attribute_values, gpointer user_data, GError **err)
{
	struct context_data *ctx_data = user_data;

	if (!strcmp(element_name, "bdaddr"))
		return;

	if (!strcmp(element_name, "text")) {
		int i;
		for (i = 0; attribute_names[i]; i++) {
			if (!strcmp(attribute_names[i], "value")) {
				ctx_data->bdaddr = g_strdup(attribute_values[i] + 2);
				ctx_data->found = TRUE;
			}
		}
	}
}
static GMarkupParser parser = {
	element_start, NULL, NULL, NULL, NULL
};

static char *dmtxplugin_xml_parse_bdaddr(const char *data)
{
	GMarkupParseContext *ctx;
	struct context_data ctx_data;
	int size;

	size = strlen(data);
	ctx_data.found = FALSE;
	ctx_data.bdaddr = NULL;
	ctx = g_markup_parse_context_new(&parser, 0, &ctx_data, NULL);

	if (g_markup_parse_context_parse(ctx, data, size, NULL) == FALSE) {
		g_markup_parse_context_free(ctx);
		g_free(ctx_data.bdaddr);
		return NULL;
	}

	g_markup_parse_context_free(ctx);

	return ctx_data.bdaddr;
}

static char *gdbus_device_create(const char *adapter, char *bdaddr)
{
	DBusMessage *message, *reply, *adapter_reply;
	DBusMessageIter iter, reply_iter;

	char *object_path = NULL;;

	adapter_reply = NULL;

	conn = g_dbus_setup_bus(DBUS_BUS_SYSTEM, NULL, NULL);
	if (conn == NULL)
		return NULL;

	if (adapter == NULL) {
		message = dbus_message_new_method_call("org.bluez", "/",
						       "org.bluez.Manager",
						       "DefaultAdapter");

		adapter_reply = dbus_connection_send_with_reply_and_block(conn,
								  message, -1, NULL);

		dbus_message_unref(message);

		if (dbus_message_get_args(adapter_reply, NULL, DBUS_TYPE_OBJECT_PATH, &adapter, DBUS_TYPE_INVALID) == FALSE)
			return NULL;
	}

	if (adapter_reply != NULL)
		dbus_message_unref(adapter_reply);

	reply = dbus_connection_send_with_reply_and_block(conn,
							message, -1, NULL);

	dbus_message_unref(message);

	if (!reply) {
		message = dbus_message_new_method_call("org.bluez", adapter,
						       "org.bluez.Dmtx",
						       "CreateOOBDevice");
		dbus_message_iter_init_append(message, &iter);
		dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &bdaddr);

		reply = dbus_connection_send_with_reply_and_block(conn,
								  message, -1, NULL);

		dbus_message_unref(message);

		if (!reply)
			return;
	} else {
		if (dbus_message_get_args(reply, NULL, DBUS_TYPE_OBJECT_PATH, &object_path, DBUS_TYPE_INVALID) == FALSE)
			return;
	}

	dbus_message_unref(reply);

	return object_path;
}


void dmtxplugin_gdbus_create_device(char *outfile)
{
        char *data;
        char *bdaddr;
        char *device_path;

        device_path = NULL;
        /* parse xml file containing bdaddr */
        if (g_file_get_contents (outfile, &data, NULL, NULL) == FALSE) {
                log_message("Couldn't load XML file %s\n", outfile);
                return;
        }

        bdaddr = dmtxplugin_xml_parse_bdaddr(data);
        log_message("Decoded bdadd: %s \n ", bdaddr);

        device_path = gdbus_device_create(NULL, bdaddr);
        log_message("Device created on path: %s \n ", device_path);

        dbus_connection_unref(conn);

}

