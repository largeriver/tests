#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "GlueProtocol.pb.h"


using namespace ::com::tranzda::glue;
using namespace ::google::protobuf::io;

#include <iostream>
using namespace std;
typedef int int32;

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
class FdWriter 
{
    FileOutputStream *_OstreamOutputStream;
    CodedOutputStream *_CodedOutputStream;
public:
    FdWriter(int fd)
    {
        _OstreamOutputStream = new FileOutputStream(fd);
        _CodedOutputStream = new CodedOutputStream(_OstreamOutputStream);

    }

    inline void operator()(const ::google::protobuf::Message &msg)
    {
	    std::cout << "msg size = " << msg.ByteSize() << " content = " <<  msg.DebugString() << std::endl;

        _CodedOutputStream->WriteVarint32(msg.ByteSize());

        if ( !msg.SerializeToCodedStream(_CodedOutputStream) )
            std::cout << "SerializeToCodedStream error " << std::endl;

		/**
			目前，除了销毁FdWriter 能够flush外，
			直接调用_OstreamOutputStream.Flush会引起_CodedOutputStream 在析构时出错。
		*/
    }

    ~FdWriter()
    {
        delete _CodedOutputStream;
        delete _OstreamOutputStream;
    }
};

class FDReader
{
    FileInputStream *_IstreamInputStream;
    CodedInputStream *_CodedInputStream;
public:
    FDReader(int fd)
    {
        _IstreamInputStream = new FileInputStream(fd);
        _CodedInputStream = new CodedInputStream(_IstreamInputStream);      
    }

    template<class T>
    bool ReadNext(T& msg)
    {
        google::protobuf::uint32 size;
		bool ret = _CodedInputStream->ReadVarint32(&size);
		
        if (ret)
        {   
            CodedInputStream::Limit msgLimit = _CodedInputStream->PushLimit(size);
			ret = msg.ParseFromCodedStream(_CodedInputStream);
            if ( ret )
            {
                _CodedInputStream->PopLimit(msgLimit);      
                std::cout << " FASReader ReadNext: \n" << msg.DebugString() << std::endl;
            }else{
            	//handle parse error
            }
        }else{
        	//maybe end of file
        }

        return ret;
    }

    ~FDReader()
    {
        delete _CodedInputStream;
        delete _IstreamInputStream;
    }
};

#if 0
class FASWriter 
{
    std::ofstream mFs;
    OstreamOutputStream *_OstreamOutputStream;
    CodedOutputStream *_CodedOutputStream;
public:
    FASWriter(const std::string &file) : mFs(file,std::ios::out | std::ios::binary)
    {
        assert(mFs.good());

        _OstreamOutputStream = new OstreamOutputStream(&mFs);
        _CodedOutputStream = new CodedOutputStream(_OstreamOutputStream);
    }

    inline void operator()(const ::google::protobuf::Message &msg)
    {
        _CodedOutputStream->WriteVarint32(msg.ByteSize());

        if ( !msg.SerializeToCodedStream(_CodedOutputStream) )
            std::cout << "SerializeToCodedStream error " << std::endl;
    }

    ~FASWriter()
    {
        delete _CodedOutputStream;
        delete _OstreamOutputStream;
        mFs.close();
    }
};

class FASReader
{
    std::ifstream mFs;

    IstreamInputStream *_IstreamInputStream;
    CodedInputStream *_CodedInputStream;
public:
    FASReader(const std::string &file), mFs(file,std::ios::in | std::ios::binary)
    {
        assert(mFs.good());

        _IstreamInputStream = new IstreamInputStream(&mFs);
        _CodedInputStream = new CodedInputStream(_IstreamInputStream);      
    }

    template<class T>
    bool ReadNext()
    {
        T msg;
        int32 size;

        bool ret;
        if ( ret = _CodedInputStream->ReadVarint32(&size) )
        {   
            CodedInputStream::Limit msgLimit = _CodedInputStream->PushLimit(size);
            if ( ret = msg.ParseFromCodedStream(_CodedInputStream) )
            {
                _CodedInputStream->PopLimit(msgLimit);      
                std::cout << mFeed << " FASReader ReadNext: " << msg.DebugString() << std::endl;
            }
        }

        return ret;
    }

    ~FASReader()
    {
        delete _CodedInputStream;
        delete _IstreamInputStream;
        mFs.close();
    }
};
#endif

#define SERV_PORT 8288
#define MAXLINE 1024
void err_quit(const char* log){

	fprintf(stderr,"%s",log);
	exit(-1);
	
}

 void  str_cli(FILE *fp, int sockfd)
 {
     char    sendline[MAXLINE], recvline[MAXLINE];

     while (fgets(sendline, MAXLINE, fp) != NULL) {

         write(sockfd, sendline,strlen(sendline));

         if (read(sockfd, recvline, MAXLINE) == 0)
             err_quit("str_cli: server terminated prematurely");

         fputs(recvline, stdout);
     }
 }

 void invoke(int socketfd){
 	static int _seqno = 0;
 	//int32 err = -1;
 	//send messgae

 	{
		MsgGlue::MsgClipBoard* cbMsg =  new MsgGlue::MsgClipBoard(); 
		cbMsg->set_text("hello world");
		
		MsgGlue msg_send;
		msg_send.set_type("clipboard.set");
		msg_send.set_seqno(_seqno+=2);
		msg_send.set_allocated_clipboard(cbMsg);

		FdWriter writer(socketfd);
		writer(msg_send);
 	}


	//read message
	//{
		std::cerr << "wait reply"<< std::endl;
		MsgGlue msg_reply;
		FDReader reader(socketfd);
		bool ret = reader.ReadNext<>(msg_reply);
		if(ret){
			std::cerr << "recv:" << msg_reply.DebugString() << std::endl;
		}else{
			err_quit("recv msg failed!");
		}
	//}
	
 }


 int main(int argc, char **argv)
 {
     int     sockfd;
     struct sockaddr_in servaddr;

     if (argc != 2)
         err_quit("usage: tcpcli <IPaddress>");

     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     memset(&servaddr, 0,sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(SERV_PORT);
     inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

     int err = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	 if(err ==0){
	     //str_cli(stdin, sockfd);     /* do it all */
	     invoke(sockfd);
	 	std::cerr << "invoke finished" << std::endl;
	 	}
 	else{
		fprintf(stderr,"connect failed, err = %d , %s",err,strerror(errno));
	}

	close(sockfd);
	std::cerr << "exit" << std::endl;
     exit(0);
 }


