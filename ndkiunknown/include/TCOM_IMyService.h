
#ifndef ANDROID_GUILH_ADD_SERVICE_H
#define ANDROID_GUILH_ADD_SERVICE_H
#include "TCOM_TIUnknown.h"


TCOM_EXTERN const TIID TIID_IMyService;

class TIMyService:public TIUnknown{
public:
	virtual HRESULT Add(LONG s1,LONG s2,LONG * res) = 0;
};


#endif

