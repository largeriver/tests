#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <utils/TextOutput.h>
#include <utils/Log.h>

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

using namespace android;

extern "C" sp<IBinder>  load_service(void);
int main(int argc, char** argv)
{
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();

	LOGI("ServiceManager: %p", sm.get());
	sp<IBinder> service=	load_service();
	if(service == NULL){
		LOGE("40 load service failed\n");
		return -EINVAL;
	}
	sm->addService(service->getInterfaceDescriptor(),service);

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
}