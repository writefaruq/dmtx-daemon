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

static void element_start(GMarkupParseContext *context,
		const gchar *element_name, const gchar **attribute_names,
		const gchar **attribute_values, gpointer user_data, GError **err)
{
	struct context_data *ctx_data = user_data;

	if (!strcmp(element_name, "bdaddr"))
		return;

	if (!strcmp(element_name, "")) {
		int i;
		for (i = 0; attribute_names[i]; i++) {
			if (!strcmp(attribute_names[i], "id")) {
				if (strtol(attribute_values[i], 0, 0) == ATTRID_1284ID)
					ctx_data->found = TRUE;
				break;
			}
		}
		return;
	}

	if (ctx_data->found  && !strcmp(element_name, "text")) {
		int i;
		for (i = 0; attribute_names[i]; i++) {
			if (!strcmp(attribute_names[i], "value")) {
				ctx_data->id = g_strdup(attribute_values[i] + 2);
				ctx_data->found = FALSE;
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
		g_free(ctx_data.id);
		return NULL;
	}

	g_markup_parse_context_free(ctx);

	return ctx_data.bdaddr;
}

void dmtxplugin_gdbus_create_device(char *outfile)
{
        /* parse xml file containing bdaddr */
}
