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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#define ALL_MASK 0xffffffff

#include "symbol.h"
#include "utils.h"
#include "inotify.h"
#include "inotify-eventqueue.h"
#include "inotify-utils.h"
#include "dmtxd.h"

void handle_file_creation(char *infile)
{
        int img_count = 0;
        char *outfile = DMTX_SYMBOL_OUTPUT;
        img_count = symbol_decode(infile, outfile);
        //printf(" from %s to %s, %d symbol decoded \n", infile, outfile, img_count);
        if ( img_count == 1) { /* assuming successful decode */
                log_message(LOG_FILE, "Decoded dmtx symbol\n");
                dmtxplugin_gdbus_create_device(outfile);
        } else {
                log_message(LOG_FILE, "failed to decode dmtx symbol\n");
        }
}

int inotify_watcher(void)
{
	/* This is the file descriptor for the inotify device */
	int inotify_fd;

	/* First we open the inotify dev entry */
	inotify_fd = open_inotify_dev();
	if (inotify_fd < 0)
	{
		return 0;
	}



	/* We will need a place to enqueue inotify events,
           this is needed because if you do not read events
	   fast enough, you will miss them.
	*/
	queue_t q;
	q = queue_create (128);



	/* Watch the directory passed in as argument
	   Read on for why you might want to alter this for
	   more efficient inotify use in your app.
	*/
	watch_dir (inotify_fd, DMTX_SYMBOLDIR, ALL_MASK);
	process_inotify_events (q, inotify_fd);



	/* Finish up by destroying the queue, closing the fd
           and returning a proper code
        */
	queue_destroy (q);
	close_inotify_dev (inotify_fd);
	return 0;
}
