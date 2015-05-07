#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#ifndef _TRACE
#define _TRACE LOGE
//#define _TRACE(...)   ((void)0)
#endif

using namespace android;
  
// get the name of the generic interface we hold a reference to
static String16 get_interface_name(sp<IBinder> service)
{
    if (service != NULL) {
        Parcel data, reply;
        status_t err = service->transact(IBinder::INTERFACE_TRANSACTION, data, &reply);
        if (err == NO_ERROR) {
            return reply.readString16();
        }
    }
    return String16();
}

static const String16 strname_IMyService("TIMyService");
enum{
	IMyService_Method_Add = 1,
};

int add(int s1,int s2,int *res){
	sp<IServiceManager> sm = defaultServiceManager();
	if (sm == NULL) {
		aerr << "service: Unable to get default service manager!" << endl;
		return 20;
	}
  

	sp<IBinder> service = sm->checkService(strname_IMyService);
	if (service == 0) {
		LOGW("AddService not published, waiting...");
		return 21;
	}

	Parcel data, reply;
	
	_TRACE("Add>>"); 
	data.writeInterfaceToken(strname_IMyService);
	data.writeInt32(s1);
	data.writeInt32(s2);
	status_t err = service->transact(IMyService_Method_Add, data, &reply);

	if(err == NO_ERROR){
		*res = reply.readInt32();
		err =reply.readInt32();
	}
	_TRACE("Add<< err(%d),res(%ld)",err,*res);	
	_TRACE("	IPCThreadState::self() %p",	IPCThreadState::self());
	printf("press any char to continue:\n");
	fgetc(stdin);
	
	return 0;
}
	

int main(int argc,char* const* argv){

	//sp<IServiceManager> sm = defaultServiceManager();
	int res = 0;
	add(1,18,&res);

	IPCThreadState::self()->flushCommands();
	IPCThreadState::self()->shutdown();
	_TRACE("	IPCThreadState::self() %p",	IPCThreadState::self());
	printf("press any char to exit!\n");
	fgetc(stdin);
	return 0;
}

