



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


#define TIID_IMyService_IMPLE
#include "../include/TCOM_IMyService.h"
#include "../include/TCOM_TIUnknown_c.h"
#include "../include/TCOM_Utils.h"

#ifndef _TRACE
#define _TRACE LOGE
//#define _TRACE(...)   ((void)0)
#endif





static const String16 strname_IMyService("TIMyService");
enum{
	IMyService_Method_Add = 1,
};

class BpMyService:public TemTComImpl<TIMyService>
{
public:
	typedef TemTComImpl<TIMyService> IMPLE;
	BpMyService():IMPLE(TIID_IMyService)
	{
		sm = defaultServiceManager();
		if (sm == NULL) {
			_TRACE("service: Unable to get default service manager!");;
			return;
		}
		sm->printRefs();
		m_service = sm->checkService(strname_IMyService);
		if (m_service == NULL) {
			_TRACE("AddService not published, waiting...");
			return;
		}

		_TRACE("BpMyService(%p) sm:%p m_service:%p",this,sm.get(),m_service.get());
	}

	~BpMyService(){
		_TRACE("~BpMyService >>");
		_TRACE("m_service ref:%ld,sm ref:%ld",m_service->getStrongCount(),sm->getStrongCount());
		m_service.clear();
		sm.clear();
		_TRACE("~BpMyService <<");
	}


	virtual HRESULT Add(LONG s1,LONG s2,LONG * res) {
		Parcel data, reply;
		
		if(m_service == NULL)
			return E_INVALIDARG;
		_TRACE("BpMyService::Add>>"); 
		data.writeInterfaceToken(strname_IMyService);
		data.writeInt32(s1);
		data.writeInt32(s2);
		status_t err = m_service->transact(IMyService_Method_Add, data, &reply);

		if(err == NO_ERROR){
			*res = reply.readInt32();
			err =reply.readInt32();
		}
		_TRACE("BpMyService::Add<< err(%d),res(%ld)",err,*res);	
		return err;
	}
	
protected:	
	sp<IServiceManager> sm;
	sp<IBinder> m_service;

};




class BnMyService:public BBinder,public TIUnknownStub
{
public:
	
	BnMyService(TIMyService * inService = NULL):mService(inService)
	{
		if(mService)
			mService->AddRef();
		_TRACE("BnMyService(%p)",this);
	}

	~BnMyService()
	{
		_TRACE("~BnMyService >>");
		if(mService)
			mService->Release();
		mService = NULL;
		_TRACE("~BnMyService <<");
	}

	virtual HRESULT QueryInterface ( /* [in] */TREFIID riid,/* [iid_is][out] */ void **ppvObject)
	{
		if(riid == TIID_IUnknown){
			*ppvObject = static_cast<TIUnknown*>(this);
		}
		else if(riid == TIID_IUnknownStub) {
			*ppvObject = static_cast<TIUnknownStub*>(this);
		}
		else{
			*ppvObject = NULL;
			return E_FAIL;
		}

		((TIUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}

	virtual ULONG  AddRef ()
	{
		incStrong(this);
		_TRACE("AddRef(%p,%lu) ",this,(ULONG)getStrongCount());
		return (ULONG)getStrongCount();
	}

	virtual ULONG  Release()
	{	
		ULONG ref = (ULONG)getStrongCount() -1;
		_TRACE("Release(%p,%lu) >>",this,ref);
		decStrong(this);
		_TRACE("Release(%p,%lu) <<",this,ref);
		return ref;
	}


	HRESULT ReisterService(TIUnknown * pUnk) {
		if(mService)
			mService->Release();
		mService = NULL;
		HRESULT hr =  pUnk->QueryInterface(TIID_IMyService,(void **)&mService);
		if(SUCCESSED(hr)){
		//	mService->AddRef();
			sp<IServiceManager> sm = defaultServiceManager();
			sm->addService(this->getInterfaceDescriptor(),this);
		}
		//pUnk->Release();
		return hr;	
	}

	const String16& getInterfaceDescriptor() const {
 		    return strname_IMyService;
	}



	status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
	{
		switch(code) {
		case IMyService_Method_Add: {
			CHECK_INTERFACE(AddService, data, reply);
			LONG s1 = data.readInt32();
			LONG s2 = data.readInt32();
			LONG res = 0;
			status_t hr = mService->Add(s1,s2,&res);
			reply->writeInt32(res);
			reply->writeInt32(hr);
			_TRACE("BnMyService:Add(s1 %ld,s2 %ld,  res %ld)",s1,s2,res);
			return NO_ERROR;
		} 
		break;
		default:
			return BBinder::onTransact(code, data, reply, flags);
		}
	}

protected:
	TIMyService* mService;
};


extern "C" HRESULT TCOM_Srv_GetClassObject( TREFCLSID rclsid, TREFIID riid, void **ppv ){
	char riid_s1[64],riid_s2[64],riid_s3[64];
	CL_StringFromIID(riid,riid_s1,64);
	CL_StringFromIID(TIID_IMyService,riid_s2,64);
	CL_StringFromIID(TIID_IUnknownStub,riid_s3,64);

	_TRACE("TCOM_Srv_GetClassObject>>PS  riid:%s\nTIID_IMyService:%s\nTIID_IUnknownStub:%s\n",riid_s1,riid_s2,riid_s3);
	if(riid == TIID_IMyService){
		BpMyService* s = new BpMyService();
		_TRACE("TIID_IMyService>>%p",s);
		*ppv = static_cast<TIMyService*>(s);
	}
	else if(riid == TIID_IUnknownStub) {
		BnMyService* s = new BnMyService();
		_TRACE("TIID_IUnknownStub>>%p",s);
		*ppv = static_cast<TIUnknownStub*>(s);
	}
	else{
		*ppv = NULL;
		return E_FAIL;
	}

	((TIUnknown*)*ppv)->AddRef();
	return S_OK;

	
}

