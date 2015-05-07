#include "MyService.h"
//#include <utils/IServiceManager.h>
//#include <utils/IPCThreadState.h>
        #include <sys/types.h>
       #include <unistd.h>

namespace android {
static struct sigaction oldact;
static pthread_key_t sigbuskey;
 

int AddService::instantiate() {
	LOGE("AddService instantiate");
	int r = defaultServiceManager()->addService(
	           String16("guilh.add"), new AddService());
	       LOGE("AddService r = %d\n", r);
	return r;
}
 

AddService::AddService()
{  
	LOGE("AddService created");
//	mNextConnId = 1;
	pthread_key_create(&sigbuskey, NULL);
}


AddService::~AddService()
{  
	pthread_key_delete(sigbuskey);
	LOGE("AddService destroyed");
}

const String16& AddService::getInterfaceDescriptor() const {
    static String16 sEmptyDescriptor("MyService");
    return sEmptyDescriptor;
}

status_t AddService::onTransact(
       uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags){
	switch(code) {
	case 0: {
		CHECK_INTERFACE(AddService, data, reply);
		pid_t pid = data.readInt32();
		int num = data.readInt32();
		num =  num + 0x1000;
		reply->writeInt32(num);
		LOGE("AddService code 0 pid:%d, Result 0x%X",pid,num);
		 return NO_ERROR;
		} 
		break;
	case 1: 
		{
			CHECK_INTERFACE(AddService, data, reply);
			pid_t pid = data.readInt32();
		   	int fd = dup(data.readFileDescriptor());
			char buf[128];
			sprintf(buf,"pid %d require write fd %d\n",pid,fd);
			lseek(fd,0,SEEK_END);
			write(fd,buf,strlen(buf));
			close(fd);
			LOGE("AddService code 1 :%s",buf);
			 return NO_ERROR;
		} 
		break;
		
	default:
		return BBinder::onTransact(code, data, reply, flags);
	}
}

}; //namespace


int service_load(void){
	return android::AddService::instantiate();
}



