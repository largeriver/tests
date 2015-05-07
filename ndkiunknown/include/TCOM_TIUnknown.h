#ifndef TCOM_IUNKNOWN__HHww
#define TCOM_IUNKNOWN__HHww

typedef struct _TGUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} TGUID;
typedef TGUID TIID;

//#if defined(__cplusplus)
//#include <string.h>
inline bool operator==(const TIID& guidOne, const TIID& guidOther)
{
    return !memcmp(&guidOne,&guidOther,sizeof(TIID));
}
inline bool operator!=(const TIID& guidOne, const TIID& guidOther)
{
    return !(guidOne == guidOther);
}
//#endif



//typedef TIID & TREFIID,TREFCLSID;
#define  TREFIID const TIID & 
#define  TREFCLSID const TIID &
typedef long HRESULT;
typedef long LONG;
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef unsigned long ULONG,UInt32;


enum{
	E_INVALIDARG = -4,
	E_OUTOFMEMORY = -3,
	E_ACCESSDENIED	= -2,
	E_FAIL = -1,
	S_OK = 0		
};


#define SUCCESSED(X) ((X)>=0)
#define FAILED(X) ((X)<0)
#define TCOM_EXTERN extern


//IUnknown UUID定义
TCOM_EXTERN const TIID TIID_IUnknown;

/**
@ingroup group_TCOM
@brief TCOM基本接口。
*/ 
class TIUnknown
{
public:
    virtual HRESULT QueryInterface(/* [in] */ TREFIID riid, /* [iid_is][out] 
*/ void **ppvObject) = 0;

    virtual ULONG AddRef(void) = 0;

    virtual ULONG Release(void) = 0;
	virtual ~TIUnknown(){}

#define TCOM_IUNKNOWN__MEMBER \
    virtual HRESULT QueryInterface(/* [in] */ TREFIID riid, /* [iid_is][out] 
*/ void **ppvObject); \
    virtual ULONG AddRef(void); \
    virtual ULONG Release(void)
};

//IUnknown UUID定义
TCOM_EXTERN const TIID TIID_IUnknownStub;
class TIUnknownStub:public TIUnknown{
public:
	virtual HRESULT ReisterService(TIUnknown * pUnk) = 0;
	virtual ~TIUnknownStub(){}
		
};

#endif

