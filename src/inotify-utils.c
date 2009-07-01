#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include "inotify.h"

#include "inotify-eventqueue.h"

#define ALL_MASK 0xffffffff
#define EVENTQ_SIZE 128

struct inotify_event *eventq[EVENTQ_SIZE];
int eventq_head = 0;
int eventq_tail = 0;


/* This simply opens the inotify node in dev (read only) */
int open_inotify_dev ()
{
	int fd;

	fd = open("/dev/inotify", O_RDONLY);

	if (fd < 0)
        {
		perror ("open(\"/dev/inotify\", O_RDONLY) = ");
	}

	return fd;
}


/* This will close the open file descriptor which is the inotify node in /dev */
int close_inotify_dev (int fd)
{
	int r;

	if ( (r = close (fd)) < 0)
        {
		perror ("close (fd) = ");
	}

	return r;
}



/* This method does the dirty work of determining what happened,
   then allows us to act appropriately
*/
void handle_event (struct inotify_event *event)
{
	/* If the event was associated with a filename, we will store it here */
	char * cur_event_filename = NULL;

	/* This is the watch descriptor the event occured on */
	int cur_event_wd = event->wd;


	if (event->len)
        {
		cur_event_filename = event->filename;
	}
	printf("FILENAME=%s\n", cur_event_filename);
        printf("\n");


	/* Perform event dependent handler routines */
	/* The mask is the magic that tells us what file operation occurred */
	switch (event->mask)
	{
		/* File was accessed */
		case IN_ACCESS:
			printf("ACCESS EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File was modified */
		case IN_MODIFY:
			printf("MODIFY EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File changed attributes */
		case IN_ATTRIB:
			printf("ATTRIB EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File was closed */
		case IN_CLOSE:
			printf("CLOSE EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File was opened */
		case IN_OPEN:
			printf("OPEN EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File was moved from X */
		case IN_MOVED_FROM:
			printf("MOVE_FROM EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File was moved to X */
		case IN_MOVED_TO:
			printf("MOVE_TO EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* Subdir was deleted */
		case IN_DELETE_SUBDIR:
			printf("DELETE_SUBDIR EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File was deleted */
		case IN_DELETE_FILE:
			printf("DELETE_FILE EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* Subdir was created */
		case IN_CREATE_SUBDIR:
			printf("CREATE_SUBDIR EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* File was created */
		case IN_CREATE_FILE:
			printf("CREATE_FILE EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* Watched entry was deleted */
		case IN_DELETE_SELF:
			printf("DELETE_SELF EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* Backing FS was unmounted */
		case IN_UNMOUNT:
			printf("UNMOUNT EVENT OCCURRED: File \"%s\" on WD #%i\n",
		        	cur_event_filename, cur_event_wd);
		break;


		/* Too many FS events were received without reading them
		   some event notifications were potentially lost.  */
		case IN_Q_OVERFLOW:
			printf("Warning: AN OVERFLOW EVENT OCCURRED: \n");
		break;


		case IN_IGNORED:
			printf("IGNORED EVENT OCCURRED: \n");
		break;


		/* Some unknown message received */
		default:
			printf ("UNKNOWN EVENT OCCURRED for file \"%s\" on WD #%i\n",
		        cur_event_filename, cur_event_wd);
		break;
	}
}

void handle_events (queue_t q)
{
	struct inotify_event *event;
	while (!queue_empty (q))
	{
		event = queue_front (q);
		queue_dequeue (q);
		handle_event (event);
		free (event);
	}
}

int read_events (queue_t q, int fd)
{
	char buffer[16384];
	size_t buffer_i;
	struct inotify_event *pevent, *event;
	ssize_t r;
	size_t event_size;
	int count = 0;

	r = read (fd, buffer, 16384);

	if (r <= 0)
		return r;

	buffer_i = 0;
	printf("read = %d\n", r);

	printf("sizeof inotify_event = %d\n", sizeof(struct inotify_event));
	while (buffer_i < r) {
		/* Parse events and queue them ! */
		pevent = (struct inotify_event *)&buffer[buffer_i];
		event_size = sizeof(struct inotify_event) + pevent->len;
		printf("pevent->len = %d\n", pevent->len);
		event = malloc(event_size);
		memmove(event, pevent, event_size);
		queue_enqueue(event, q);
		buffer_i += event_size;
		count++;
	}

	return count;
}

int event_check (int fd)
{
	struct timeval timeout;
	int r;
	fd_set rfds;

	timeout.tv_sec = 4;
	timeout.tv_usec = 0;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	r = select (fd+1, &rfds, NULL, NULL, &timeout);

	return r;
}

int process_inotify_events (queue_t q, int fd)
{
	while (1)
	{
		if (!queue_empty(q))
		{
			handle_events (q);
		}

		if (event_check(fd) > 0)
                {
			int r;

			r = read_events (q, fd);

			if (r < 0)
				break;
		}
	}

	return 0;
}

int watch_dir (int fd, const char *dirname, long int mask)
{
	struct inotify_watch_request iwr;
	iwr.dirname = strdup (dirname);
	iwr.mask = mask;
	int wd;

	wd = ioctl(fd, INOTIFY_WATCH, &iwr);

	if (wd < 0)
	{
		printf("ioctl(fd, INOTIFY_WATCH, {%s, %d}) = %d", iwr.dirname, iwr.mask, wd);
		fflush(stdout);
		perror (" ");
	}

	free (iwr.dirname);

	printf("%s WD=%d\n", dirname, wd);
	return wd;
}

int ignore_wd (int fd, int wd)
{
	int r;

	r = ioctl(fd, INOTIFY_IGNORE, &wd);

	if (r < 0)
	{
		perror("ioctl(fd, INOTIFY_IGNORE, &wid) = ");
	}

	return r;
}
