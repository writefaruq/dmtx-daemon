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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"
#include "inotify.h"

#define LOCK_FILE	"/tmp/dmtxdatadir/dmtxd.lock"

void signal_handler(int sig)
{
	switch (sig) {
		case SIGHUP:
		log_message(LOG_FILE,"hangup signal catched");
		break;
		case SIGTERM:
		log_message(LOG_FILE,"terminate signal catched");
		exit(0);
		break;
	}
}

void daemonize()
{
	int i, lfp, ret;
	pid_t pid, sid;
	char str[10];

	if (getppid() == 1)
		return; /* already a daemon */

	pid = fork();
	if (pid < 0) {
	        printf("testing dmtxd pid: %d \n", pid);
		exit(EXIT_FAILURE); /* fork error */
	}

	if (pid > 0) {
	        /* printf("testing dmtxd pid: %d \n", pid); */
		exit(EXIT_SUCCESS); /* parent exits */
	}

        umask(0);
	/* child (daemon) continues */
	/* obtain a new process group */
	sid = setsid();
	if (sid < 0) {
	        printf("testing dmtxd sid: %d \n", sid);
		exit(EXIT_FAILURE);
	}

	if (sid > 0) {
	         printf("testing dmtxd sid: %d \n", sid);
	}

        lfp = open(LOCK_FILE,O_RDWR | O_CREAT,0640);

	/* can not open */
	if (lfp < 0)
		exit(EXIT_FAILURE);

	/* can not lock */
	if (lockf(lfp, F_TLOCK,0) < 0)
		exit(EXIT_FAILURE);

	/* first instance continues */
	sprintf(str,"%d\n", getpid());
        log_message(LOG_FILE, "testing dmtxd pid: OK \n");

	/* record pid to lockfile */
	ret = write(lfp, str, strlen(str));


	/* close all descriptors */
	for (i = getdtablesize(); i>=0; --i)
		close(i);

	/* handle standart I/O */
	ret = open("/dev/null",O_RDWR);
	ret = dup(i);
	ret = dup(i);

        printf("testing dmtxd handle IO: %d \n", ret);

	/* set newly created file permissions */
	umask(027);

	/* change running directory */
	ret = chdir(DMTX_DATADIR);
	if (ret < 0)
		exit(EXIT_FAILURE);

        log_message(LOG_FILE, "testing dmtxd rundir: OK  \n");

	/* ignore child */
	signal(SIGCHLD, SIG_IGN);

	/* ignore tty signals */
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);

	/* catch hangup signal */
	signal(SIGHUP, signal_handler);

	/* catch kill signal */
	signal(SIGTERM, signal_handler);
}

int main(int argc, char *argv[])
{
	int err;
	daemonize();
//        err = inotify_watcher();
//        printf("err: %d\n", err);
//        if (err < 0) {
//                /* FIXME: Don't use errno, it is a global system variable. Assign errno to a local variable first */
//                log_message(LOG_FILE, "inotify failed ");
//        }
//
//        log_message(LOG_FILE, "inotify: after");


	/* FIXME: Busy loop? Suggestion: is it possible use glib mainloop? */
	while (1) {
		/* run inotify */
		err = inotify_watcher();
		if (err < 0) {
		        /* FIXME: Don't use errno, it is a global system variable. Assign errno to a local variable first */
			log_message(LOG_FILE, "inotify: error");
		}
		sleep(1);
	}

	log_message(LOG_FILE, "\nExiting daemon\n");

	exit(EXIT_SUCCESS);

	return 0;
}

