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
#include <dlfcn.h>


using namespace android;


typedef 	int (*service_load_func_t)(void);
int main(int argc, char** argv)
{
	char* path = argv[1];
	if(argc ==1){
		fprintf(stderr,"10 usage: %s lib1[ lib2 [...]]\n",argv[0]);
		return -1;
	}

	/***************************************/
	void *handle;

	const char *error;

	handle = dlopen(path, RTLD_NOW);
	if (handle == NULL) {
		char const *err_str = dlerror();
		LOGE("load: module=%s\n%s\n", path, err_str?err_str:"unknown");
		return -EINVAL;
	}
	dlerror();    /* Clear any existing error */


	/***************************************/   
	    /* Get the address of the struct hal_module_info. */
	    const char *sym = "service_load";
	    service_load_func_t service_load_func= (service_load_func_t)dlsym(handle, sym);
	    if (service_load_func == NULL) {
       	 LOGE("load: couldn't find symbol %s\n", sym);
	        return -EINVAL;
	    }

	/***************************************/
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();
	LOGI("ServiceManager: %p", sm.get());


	/***************************************/
	int ret = (*service_load_func)();
	if(ret <0){
		fprintf(stderr,"40 load service %s failed\n",path);
		return ret;
	}


	/***************************************/
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();


	/***************************************/
	dlclose(handle);
}

