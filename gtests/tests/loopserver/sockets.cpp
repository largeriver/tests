#include "sockets.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>
#include <stddef.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>	
#include <sys/types.h>
#include <netinet/in.h>

#define close_socket close
#else
#define close_socket closesocket
#endif

#define LISTEN_BACKLOG 4

/* Connect to port on the loopback IP interface. type is
* SOCK_STREAM or SOCK_DGRAM.
* return is a file descriptor or -1 on error
*/
int socket_loopback_client(int port, int type)
{
	struct sockaddr_in addr;
	//socklen_t alen;
	int s;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	s = socket(AF_INET, type, 0);
	if (s < 0) return -1;

	if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close_socket(s);
		return -1;
	}

	return s;

}

/* open listen() port on loopback interface */
int socket_loopback_server(int port, int type)
{
	struct sockaddr_in addr;
//	size_t alen;
	int s, n;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	s = socket(AF_INET, type, 0);
	if (s < 0) return -1;

	n = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n));
	

	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close_socket(s);
		return -1;
	}

	if (type == SOCK_STREAM) {
		int ret;

		ret = listen(s, LISTEN_BACKLOG);

		if (ret < 0) {
			close_socket(s);
			return -1;
		}
	}

	return s;
}


/* open listen() port on any interface */
int socket_inaddr_any_server(int port, int type)
{
	struct sockaddr_in addr;
//	size_t alen;
	int s, n;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	s = socket(AF_INET, type, 0);
	if (s < 0) return -1;

	n = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n));

	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close_socket(s);
		return -1;
	}

	if (type == SOCK_STREAM) {
		int ret;

		ret = listen(s, LISTEN_BACKLOG);

		if (ret < 0) {
			close_socket(s);
			return -1;
		}
	}

	return s;
}


/* Connect to port on the IP interface. type is
* SOCK_STREAM or SOCK_DGRAM.
* return is a file descriptor or -1 on error
*/
int socket_network_client(const char *host, int port, int type)
{
	struct hostent *hp;
	struct sockaddr_in addr;
	socklen_t alen;
	int s;

	hp = gethostbyname(host);
	if (hp == 0) return -1;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = hp->h_addrtype;
	addr.sin_port = htons(port);
	memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);

	s = socket(hp->h_addrtype, type, 0);
	if (s < 0) return -1;

	if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close_socket(s);
		return -1;
	}

	return s;

}


int socket_server_accept(int listen_socket){
	struct sockaddr_in addr;
#ifdef _WIN32
	int size = sizeof(addr);
#else
	socklen_t size = sizeof(addr);
#endif


	return  accept(listen_socket, (struct sockaddr*)&addr, &size);
}




void socket_init_nonblocking(int inFileDesc)
{
#ifdef _WIN32
	u_long one = 1;
	int err = ioctlsocket(inFileDesc, FIONBIO, &one);
#else
	int flag = fcntl(inFileDesc, F_GETFL, 0);
	int err = fcntl(inFileDesc, F_SETFL, flag | O_NONBLOCK);
#endif
	//    AssertV(err == 0, OSThread::GetErrno());
}