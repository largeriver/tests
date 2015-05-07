/*
 * Copyright (c) 2010, The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_MAX 1024
#define CMD_DISPLAY_MAX 10
#define PATH_MAX_LEN 1024
#define LINE_MAX 1024

struct pid_info_t {
    pid_t pid;

    char cmdline[CMD_DISPLAY_MAX];

    char path[PATH_MAX_LEN];
    ssize_t parent_length;
};

static char * filter_str = NULL;


void print_header()
{
    printf("%-12s %5s %6s %8s %9s %18s %9s %10s %s\n",
            "COMMAND",
            "PID",
            "USER",
            "FD",
            "TYPE",
            "DEVICE",
            "SIZE/OFF",
            "NODE",
            "NAME");
}

void print_type(char *type, struct pid_info_t* info)
{
    static ssize_t link_dest_size;
    static char link_dest[PATH_MAX_LEN];

    strncat(info->path, type, sizeof(info->path));
    if ((link_dest_size = readlink(info->path, link_dest, sizeof(link_dest)-1)) < 0) {
        if (errno == ENOENT)
            goto out;

        snprintf(link_dest, sizeof(link_dest), "%s (readlink: %s)", info->path, strerror(errno));
    } else {
        link_dest[link_dest_size] = '\0';
    }

	//printf("%s\n",link_dest);
    // Things that are just the root filesystem are uninteresting (we already know)
    if (!strcmp(link_dest, "/"))
        goto out;

	if(filter_str && (!strstr(link_dest,filter_str) && !strstr(info->cmdline,filter_str)))
		goto out;

	char tmp[128];
	if(strstr(info->path,"/fd/"))
		sprintf(tmp,"fd/%s",type);
	else
		strcpy(tmp,type);
		
	printf("%-12s %5d %6s %8s %9s %18s %9s %10s %s\n", info->cmdline, info->pid, "???", tmp,
            "???", "???", "???", "???", link_dest);

out:
    info->path[info->parent_length] = '\0';
}



// Prints out all file that have been memory mapped
void print_maps(struct pid_info_t* info)
{
    FILE *maps;
//    char buffer[PATH_MAX_LEN + 100];
	size_t map_start;
	size_t map_end;
    size_t offset;
//    int major, minor;
    char device[10];
    long int inode;
    char file[PATH_MAX_LEN];
    char line[LINE_MAX];
    int ret;

    strncat(info->path, "maps", sizeof(info->path));

    maps = fopen(info->path, "r");
    if (!maps)
        goto out;
		
	while (fgets(line, LINE_MAX, maps) != NULL){
    	//printf("%s\n",line);
    	ret = sscanf(line, "%x-%x %*s %zx %5s %ld %s", &map_start,&map_end,&offset, device, &inode,file);
      
        // We don't care about non-file maps
        if (inode == 0 || !strcmp(device, "00:00"))
            continue;
            
        if (ret != 6)
        	continue;
		
		if(filter_str && (!strstr(file,filter_str) && !strstr(info->cmdline,filter_str)) )
			continue;
			  	
        printf("%-12s %5d %6s 0x%8s %9ld %18s 0x%08X %10ld %s\n", info->cmdline, info->pid, "---", "mem",
                (size_t)(map_end - map_start), device, map_start, inode, file);
	}

    fclose(maps);

out:
    info->path[info->parent_length] = '\0';
}

// Prints out all open file descriptors
void print_fds(struct pid_info_t* info)
{
    static char* fd_path = "fd/";
    strncat(info->path, fd_path, sizeof(info->path));

    int previous_length = info->parent_length;
    info->parent_length += strlen(fd_path);

    DIR *dir = opendir(info->path);
    if (dir == NULL) {
        char msg[BUF_MAX];
        snprintf(msg, sizeof(msg), "%s (opendir: %s)", info->path, strerror(errno));
        printf("%-12s %5d %6s %8s %9s %18s %9s %10s %s\n", info->cmdline, info->pid, "???", "FDS",
                "", "", "", "", msg);
        goto out;
    }

    struct dirent* de;
    while ((de = readdir(dir))) {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        print_type(de->d_name, info);
    }
    closedir(dir);

out:
    info->parent_length = previous_length;
    info->path[info->parent_length] = '\0';
}

void lsof_dumpinfo(pid_t pid)
{
    int fd;
    struct pid_info_t info;
    info.pid = pid;

    snprintf(info.path, sizeof(info.path), "/proc/%d/", pid);

    info.parent_length = strlen(info.path);

    // Read the command line information; each argument is terminated with NULL.
    strncat(info.path, "cmdline", sizeof(info.path));
    fd = open(info.path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Couldn't read %s\n", info.path);
        return;
    }
    
    char cmdline[PATH_MAX_LEN];
    if (read(fd, cmdline, sizeof(cmdline)) < 0) {
        fprintf(stderr, "Error reading cmdline: %s: %s\n", info.path, strerror(errno));
        close(fd);
        return;
    }
    close(fd);
    info.path[info.parent_length] = '\0';

    // We only want the basename of the cmdline
    strncpy(info.cmdline, basename(cmdline), sizeof(info.cmdline));
    info.cmdline[sizeof(info.cmdline)-1] = '\0';

    	
    // Read each of these symlinks
    print_type("cwd", &info);
    print_type("exe", &info);
    print_type("root", &info);

    print_fds(&info);
    print_maps(&info);
    printf("\n");
}

#include <unistd.h>

static void usage(const char * program){
    fprintf(stderr,"%s options [filter_str]\n",program);
    fprintf(stderr,"options:\n");
    fprintf(stderr,"\t-h    print this help\n");
    fprintf(stderr,"\t-p <delay in ms> pid \n");
}


    
int main(int argc, char *argv[])
{
    //const char * filter_str = NULL;
    long filter_pid = -1;	
    int opt;

    while ((opt = getopt(argc, argv, "hp:")) != -1) {
       switch (opt) {
       case 'p':
           filter_pid =  atoi(optarg);
           break;
       default: /* '?' */
       case 'h':
           usage(argv[0]);
           return 0;
       }
   }

   for ( ; optind < argc; optind++){
       	filter_str = argv[optind];
		if(filter_str && filter_str[0] == '\0')
			filter_str = NULL;
		printf("filter_str:%s\n",filter_str);
   }



    DIR *dir = opendir("/proc");

    if (dir == NULL) {
        fprintf(stderr, "Couldn't open /proc\n");
        return -1;
    }

    print_header();

    struct dirent* de;
    while ((de = readdir(dir))) {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        // Only inspect directories that are PID numbers
        char* endptr;
        long pid = strtol(de->d_name, &endptr, 10);
	  	
	  	if (*endptr != '\0')
            continue;

	 	if(filter_pid >=0 && filter_pid != pid)
	 		continue;
	 		 
        lsof_dumpinfo(pid);
    }
    closedir(dir);

    return 0;
}
