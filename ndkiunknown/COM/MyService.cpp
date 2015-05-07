
#include <sys/types.h>
#include <unistd.h>

#include <binder/Binder.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>

#include <utils/TextOutput.h>
#include <utils/threads.h>
#include <utils/Debug.h>
#include <utils/Log.h>

using namespace android;


#include "../include/TCOM_IMyService.h"
#include "../include/TCOM_TIUnknown_c.h"
#include "../include/TCOM_Utils.h"

#ifndef _TRACE
#define _TRACE LOGE
//#define _TRACE(...)   ((void)0)
#endif


class MyService:public TemTComImpl<TIMyService>
{
public:
	typedef TemTComImpl<TIMyService> IMPLE;

	MyService():IMPLE(TIID_IMyService){
		_TRACE("MyService(%p)",this);
	}

	~MyService(){
		_TRACE("~MyService");
	}
	
	virtual HRESULT Add(LONG s1,LONG s2,LONG * res) {
		*res = s1+s2;
		return S_OK;
	}

};

extern "C" HRESULT TCOM_Srv_GetClassObject( TREFCLSID rclsid, TREFIID riid, void **ppv ){
	char riid_s1[64],riid_s2[64];
	CL_StringFromIID(riid,riid_s1,64);
	CL_StringFromIID(TIID_IMyService,riid_s2,64);
	_TRACE("TCOM_Srv_GetClassObject>>COM  riid:%s\nTIID_IMyService:%s",riid_s1,riid_s2);

	if(riid == TIID_IMyService){
		MyService* s = new MyService();
		*ppv = static_cast<TIMyService*>(s);
	}else{
		*ppv = NULL;
		return E_FAIL;
	}

	((TIUnknown*)*ppv)->AddRef();
	return S_OK;
	
}

