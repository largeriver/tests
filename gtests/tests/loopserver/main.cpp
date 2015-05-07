#define TGMLOG_CHANNEL "loop_tesst"
#include "tgmlog/tgmlog.h"


#include "sockets.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>
#include <stddef.h>

#include <utils/Looper.h>
#include <utils/StrongPointer.h>


using namespace tangram;
sp<Looper> loop;


//using namespace android;
//////////////////////////////////////////////////////////////////////////
static struct CLIENT{
	int socket;
}_client;

static int input_receiver(int fd, int events, void* data){
	struct CLIENT* cli = (struct CLIENT*)data;

	char cszStmt[4096];
	printf("Please input message,say \"byebye\" to exit room\n");
	while (fgets(cszStmt, 4096, stdin))
	{
		if (cszStmt[0] == 0)
			continue;
		if (strcmp(cszStmt, "byebye\n") == 0){
			exit(-2);
			break;
		}

		send(cli->socket, cszStmt, strlen(cszStmt), 0);
		//        write(cli->socket,"\n",1);
	}

	return 1;
}

static int client_receiver(int fd, int events, void* data)
{
	struct CLIENT* cli = (struct CLIENT*)data;
	char cszStmt[4096];

	do{
		ssize_t len = recv(fd, cszStmt, sizeof(cszStmt) - 1, 0);

		if (len > 0)
		{
			cszStmt[len] = 0;
			send(fd, cszStmt, len, 0);
		}
		else if (len == 0){
#ifdef _WIN32
			closesocket(fd);
#else
			close(fd);
#endif
			printf("client(fd=%d) closed\n", fd);
			return 0;
		}
	}while (errno == EAGAIN);

	return 1;
}


	class _Timer :public MessageHandler{
	public:
		struct CLIENT &mClient;
		_Timer(struct CLIENT& cli) :mClient(cli){}
		virtual void handleMessage(const Message& message){
			const char msg[] = "Hello Server!\n";
			int ret = send(mClient.socket, msg, sizeof(msg), 0);
			printf("send to server ret=%d\n", ret);
		};
	};
	//////////////////////////////////////////////////////////////////////////
	static struct SERVER{
		int listen_socket;
	}_server;


	static int server_receiver(int fd, int events, void* data){
		struct SERVER* _ser = (struct SERVER*)data;
		char cszStmt[4096];

		ssize_t len = -1;
		while ((len = recv(fd, cszStmt, sizeof(cszStmt) - 1, 0)) > 0)
		{
			cszStmt[len] = 0;
			printf("client(fd=%d):%s", fd, cszStmt);
			send(fd, cszStmt, len, 0);
		}

		if (len < 0){
			//loop->removeFd(fd);
#ifdef _WIN32
			closesocket(fd);
#else
			close(fd);
#endif
			printf("client(fd=%d) closed\n", fd);
			return 0;
		}

		return 1;
	}

	static int server_accept(int fd, int events, void* data)
	{
		struct SERVER* _ser = (struct SERVER*)data;
		int client_socket = socket_server_accept(_ser->listen_socket);
		if (client_socket > 0){
			socket_init_nonblocking(client_socket);
			int ret = loop->addFd(client_socket, 0, ALOOPER_EVENT_INPUT, server_receiver, data);
			printf("client(fd=%d) connected ret=%d\n", client_socket, ret);
			//  loop->wake();
		}
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////
	static const int _default_port = 8999;
	static void usage(const char* pname){
		printf("usage: %s server|client\n", pname);
		printf("\tthe default port is:%d\n",_default_port);
	}

	int main(int argc, const char* argv[])
	{
		bool isServer = false;

		if (argc != 2){
			usage(argv[0]);
			exit(-1);
		}


		if (0 == strcmp(argv[1], "server")){
			isServer = true;
		}
		else  if (0 == strcmp(argv[1], "client")){
			isServer = false;
		}
		else{
			usage(argv[0]);
			exit(-1);
		}

		loop = new Looper(false);

		if (isServer){
			_server.listen_socket = socket_loopback_server(_default_port, SOCK_STREAM);
			loop->addFd(_server.listen_socket, 0, ALOOPER_EVENT_INPUT, server_accept, &_server);
		}
		else{
			_client.socket = socket_loopback_client(_default_port, SOCK_STREAM);
			socket_init_nonblocking(_client.socket);
			loop->addFd(_client.socket, 0, ALOOPER_EVENT_INPUT, client_receiver, &_client);
#ifndef _WIN32
			//在windows上不支持
			loop->addFd(0,0,ALOOPER_EVENT_INPUT,input_receiver,&_client);
#else
			sp<MessageHandler> handler = new _Timer(_client);
			loop->sendMessageDelayed(seconds_to_nanoseconds(15), handler, Message());
#endif
		}

		do {
			//printf("about to poll...\n");
			int32_t ret = loop->pollOnce(-1);
			switch (ret) {
			case ALOOPER_POLL_WAKE:
				printf("ALOOPER_POLL_WAKE\n");
				break;
			case ALOOPER_POLL_CALLBACK:
				printf("ALOOPER_POLL_CALLBACK\n");
				break;
			case ALOOPER_POLL_TIMEOUT:
				printf("ALOOPER_POLL_TIMEOUT\n");
				break;
			case ALOOPER_POLL_ERROR:
				printf("ALOOPER_POLL_TIMEOUT\n");
				break;
			default:
				printf("ugh? poll returned %d\n", ret);
				break;
			}
		} while (1);


		return 0;
	}
