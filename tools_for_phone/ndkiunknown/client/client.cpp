#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include "client.h"

#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
 

using namespace android;

class dlloader
{
public:
	dlloader(const char *path){
		const char *error;
		handle = dlopen(path, RTLD_NOW);
		if (handle == NULL) {
			char const *err_str = dlerror();
			LOGE("load: module=%s\n%s\n", path, err_str?err_str:"unknown");
		}
		dlerror();    /* Clear any existing error */
	}

	void* GetFunc(const char * funcsym){
		void* func= 	dlsym(handle,funcsym);
		if (func == NULL) {
			 LOGE("load: couldn't find symbol %s\n", funcsym);
		}
		return func;
	}

	~dlloader(){
		LOGE("~dlloader >>");
		dlclose(handle);
		LOGE("~dlloader <<");
	}

private:
	void *handle;
		
};

#include "../include/TCOM_IMyService.h"
#include "../include/TCOM_TIUnknown_c.h"

typedef HRESULT (*TCOM_Srv_GetClassObject_func_t)( TREFCLSID rclsid, TREFIID riid, void **ppv );

int main(int argc,char* const* argv)
{
//	sp<IServiceManager> sm = defaultServiceManager();
//	LOGE("sm:%p",sm.get());
	
	if (argc <3) {
		aout << "Usage: " << argv[0]  << "  num1 num2 local|remote\n" << endl;
		return -1;
	}



	LONG s1 = atoi(argv[optind++]);
	LONG s2 = atoi(argv[optind++]);
	LONG res  =0;
	bool local = false;
	const char* path = "libnuMyServiceCOM.so";

	if(argc > optind){
		local = (0 == strcmp(argv[optind++],"local"));
	}

	LOGE("local:%s",local?"yes":"false");
	
	if(!local)
		path = "libnuMyServicePS.so";

	TIMyService* tms = NULL;

	{

		dlloader loader_com(path);
		TCOM_Srv_GetClassObject_func_t func_com =  (TCOM_Srv_GetClassObject_func_t)loader_com.GetFunc("TCOM_Srv_GetClassObject");
		HRESULT hr_com = func_com(TIID_IMyService,TIID_IMyService,(void**)&tms);
		if(hr_com <0){
			LOGE("40 failed to create COM\n");
			return hr_com;
		}else{
			LOGE("50 load service successfully\n");
		}


		HRESULT hr =tms->Add(s1,s2,&res);
		fprintf(stderr,"%ld + %ld = %ld\n",s1,s2,res);
		tms->Release();
		tms = NULL;
	}
	
	LOGE("well down");
//	sm.clear();
	LOGE("clean up");

	printf("press any char to exit!");
	fgetc(stdin);
	return 0;
}



