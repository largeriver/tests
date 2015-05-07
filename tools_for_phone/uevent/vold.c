
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include <linux/netlink.h>

#include "uevent.h"
#include "minilog.h"



int main(int argc, char **argv)
{
    int uevent_sock = -1;
    struct sockaddr_nl nladdr;
    int uevent_sz = 64 * 1024;
    int fd;


	LOGE("This program was compiled at %s %s",__DATE__,__TIME__);
	LOGE("Author: bergtian@gmail.com");

    // Socket to listen on for uevent changes
    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = 0xffffffff;

    if ((uevent_sock = socket(PF_NETLINK,
                             SOCK_DGRAM,NETLINK_KOBJECT_UEVENT)) < 0) {
        LOGE("Unable to create uevent socket: %s", strerror(errno));
        exit(1);
    }

    if (setsockopt(uevent_sock, SOL_SOCKET, SO_RCVBUFFORCE, &uevent_sz,
                   sizeof(uevent_sz)) < 0) {
        LOGE("Unable to set uevent socket options: %s", strerror(errno));
        exit(1);
    }

    if (bind(uevent_sock, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
        LOGE("Unable to bind uevent socket: %s", strerror(errno));
        exit(1);
    }

     /*
     * Main loop
     */
    //LOG_VOL("Bootstrapping complete");
    while(1) {
        fd_set read_fds;
        struct timeval to;
        int max = 0;
        int rc = 0;

        to.tv_sec = (60 * 60);
        to.tv_usec = 0;

        FD_ZERO(&read_fds);
 
        FD_SET(uevent_sock, &read_fds);
        if (uevent_sock > max)
            max = uevent_sock;

        if ((rc = select(max + 1, &read_fds, NULL, NULL, &to)) < 0) {
            LOGE("select() failed (%s)", strerror(errno));
            sleep(1);
            continue;
        }

        if (!rc) {
            continue;
        }

 
        if (FD_ISSET(uevent_sock, &read_fds)) {
            if ((rc = process_uevent_message(uevent_sock)) < 0) {
                LOGE("Error processing uevent msg (%s)", strerror(errno));
            }
        }
    } // while

}


