/* FIXME: Missing license */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>

#include "symbol.h"
#include "utils.h"
#include "inotify.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int inotify_watcher(void)
{
	int length, i = 0;
	int fd;
	int wd;
	char buffer[BUF_LEN];

	fd = inotify_init();

	if (fd < 0) {
		/* FIXME: Missing return? */
		perror("inotify_init");
	}

	wd = inotify_add_watch(fd, DMTX_DATADIR, IN_MODIFY | IN_CREATE | IN_DELETE);
	length = read(fd, buffer, BUF_LEN);

	if (length < 0) {
		/* FIXME: Missing return? */
		perror("read");
	}

	/* FIXME: Please avoid nested if */
	while (i < length) {
		struct inotify_event *event = (struct inotify_event *) &buffer[i];
		if (event->len) {
			if (event->mask & IN_CREATE) {
				if (event->mask & IN_ISDIR)
					log_message("The directory %s was created.\n", event->name);
				else
					log_message("The file %s was created.\n", event->name);
					/* Jump to symbol decode code*/
			} else if (event->mask & IN_DELETE) {
				if (event->mask & IN_ISDIR)
					log_message("The directory %s was deleted.\n", event->name);
				else
					log_message("The file %s was deleted.\n", event->name);
			} else if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR)
					log_message("The directory %s was modified.\n", event->name);
				else
					log_message("The file %s was modified.\n", event->name);
			}
		}
		i += EVENT_SIZE + event->len;
	}

	(void) inotify_rm_watch(fd, wd);
	(void) close(fd);

	/* FIXME: exit is not correct */
	exit(EXIT_SUCCESS);
}
