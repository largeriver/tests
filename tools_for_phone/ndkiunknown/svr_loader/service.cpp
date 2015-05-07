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

#ifndef _TRACE
#define _TRACE LOGE
//#define _TRACE(...)   ((void)0)
#endif

class dlloader
{
public:
	dlloader(const char *path){
		const char *error;
		handle = dlopen(path, RTLD_NOW);
		if (handle == NULL) {
			char const *err_str = dlerror();
			_TRACE("load: module=%s %s", path, err_str?err_str:"unknown");
		}
		dlerror();    /* Clear any existing error */
	}

	void* GetFunc(const char * funcsym){
		void* func= 	dlsym(handle,funcsym);
		if (func == NULL) {
			 _TRACE("load: couldn't find symbol %s\n", funcsym);
		}
		return func;
	}

	~dlloader(){
		dlclose(handle);
	}

private:
	void *handle;
		
};


#include "../include/TCOM_IMyService.h"
#include "../include/TCOM_TIUnknown_c.h"


typedef HRESULT (*TCOM_Srv_GetClassObject_func_t)( TREFCLSID rclsid, TREFIID riid, void *ppv );
		

int main(int argc, char** argv)
{
	const char* path_com = "libnuMyServiceCOM.so";
	const char* path_ps = "libnuMyServicePS.so";

	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();
	LOGI("ServiceManager: %p", sm.get());




	/***************************************/
	dlloader loader_com(path_com);
	dlloader loader_ps(path_ps);
	
	TCOM_Srv_GetClassObject_func_t func_com =  (TCOM_Srv_GetClassObject_func_t)loader_com.GetFunc("TCOM_Srv_GetClassObject");
	TCOM_Srv_GetClassObject_func_t func_ps =  (TCOM_Srv_GetClassObject_func_t)loader_ps.GetFunc("TCOM_Srv_GetClassObject");

	_TRACE("func_com %p,func_ps %p",func_com,func_ps);
	
	TIMyService* tms = NULL;
	TIUnknownStub *tus = NULL;
	
	HRESULT hr_com = func_com(TIID_IMyService,TIID_IMyService,&tms);
	if(hr_com <0){
		_TRACE("40 failed to create COM\n");
		return hr_com;
	}
	_TRACE("tms:%p",tms);

	HRESULT hr_ps = func_ps(TIID_IMyService,TIID_IUnknownStub,&tus);
	if(hr_com <0){
		_TRACE("50 failed to create Stub\n");
		return hr_ps;
	}
	_TRACE("tus:%p",tus);

	HRESULT hr = tus->ReisterService(tms);
	if(hr_com <0){
		_TRACE("60 failed to ReisterService\n");
		return hr;
	}else{
		_TRACE("ReisterService successfully\n");
	}

	tms->Release();

	/***************************************/
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();

	tus->Release();



}

