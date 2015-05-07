
#ifndef ANDROID_GUILH_ADD_SERVICE_H
#define ANDROID_GUILH_ADD_SERVICE_H

/*#include <utils.h>
#include <utils/RefBase.h>
#include <utils/IInterface.h>
#include <utils/Parcel.h>*/
#include <binder/Binder.h>
#include <binder/Parcel.h>
//#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <utils/TextOutput.h>
#include <utils/threads.h>

namespace android {
	class AddService : public BBinder{
//		mutable     Mutex     mLock;
//		int32_t   mNextConnId;
	public:
		static  int instantiate();
		AddService();
		virtual ~AddService();
		virtual const String16& getInterfaceDescriptor() const;
		virtual status_t onTransact(uint32_t, const Parcel&, Parcel*, uint32_t);
	};
}; //namespace

extern "C" int service_load(void);


#endif

