#ifndef TCOM_UTIL_HSSSS34
#define TCOM_UTIL_HSSSS34

#include "TCOM_TIUnknown.h"
#if 1
static inline UInt8 _bin2ascii_up(UInt8 data)
{static const SInt8 tab[] = "0123456789ABCDEF";	return (UInt8)tab[data];}

static inline UInt8 _bin2ascii_down(UInt8 data)
{static const UInt8 tab[] = "0123456789abcdef";return (UInt8)tab[data];}

template<class T>
static inline int Bin2HexStr_up(UInt8 c,T * s)
{*s++ = _bin2ascii_up(c>>4);*s++ = _bin2ascii_up(c&0xf);/**s = 0;*/return 2;}

template<class T>
static inline int Bin2HexStr_down(UInt8 c,T * s)
{*s++ = _bin2ascii_down(c>>4);*s++ = _bin2ascii_down(c& 0xf);/**s = 0;*/return 2;}

template<class T>
static inline int Bin2HexStr_up(UInt16 c,T * s)
{s+= Bin2HexStr_up((UInt8)(c>>8),s);s+= Bin2HexStr_up((UInt8)(c& 0xff),s);return 4;}

template<class T>
static int Bin2HexStr_down(UInt16 c,T * s)
{s+= Bin2HexStr_down((UInt8)(c>>8),s);s+= Bin2HexStr_down((UInt8)(c& 0xff),s);return 4;}

template<class T>
static inline int Bin2HexStr_up(UInt32 c,T * s)
{s+= Bin2HexStr_up((UInt16)(c>>16),s);s+= Bin2HexStr_up((UInt16)(c& 0xffff),s);return 8;}

template<class T>
static inline int Bin2HexStr_down(UInt32 c,T * s)
{s+= Bin2HexStr_down((UInt16)(c>>16),s);s+= Bin2HexStr_down((UInt16)(c& 0xffff),s);return 8;}

inline HRESULT CL_StringFromIID(TREFIID riid,char *outStr,ULONG inSize)
{
	if (inSize < 39)
		return E_FAIL;
	*outStr++ = '{';
	outStr	+=Bin2HexStr_up((UInt32)riid.Data1,outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_up(riid.Data2,outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_down(riid.Data3,outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_up(riid.Data4[0],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[1],outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_up(riid.Data4[2],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[3],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[4],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[5],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[6],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[7],outStr);
	*outStr++ = '}';
	*outStr	  = 0;
	return S_OK;
}

inline HRESULT CL_StringFromIID(TREFIID riid,UInt16 * outStr,ULONG inSize)
{
	if (inSize < 39)
		return E_FAIL;
	*outStr++ = '{';
	outStr	+=Bin2HexStr_up((UInt32)riid.Data1,outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_up(riid.Data2,outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_down(riid.Data3,outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_up(riid.Data4[0],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[1],outStr);
	*outStr++ = '-';
	outStr	+=Bin2HexStr_up(riid.Data4[2],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[3],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[4],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[5],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[6],outStr);
	outStr	+=Bin2HexStr_up(riid.Data4[7],outStr);
	*outStr++ = '}';
	*outStr	  = 0;
	return S_OK;
}



#endif

//inline void TCOM_AddClsidInstanceRefCount(){
//}
//inline void TCOM_DecClsidInstanceRefCount(){
//}		

#define os_atomic_add(REFADDR,Y) (++(*REFADDR))
#define os_atomic_sub(REFADDR,Y) (--(*REFADDR))

template <class __If>
class TemTComImpl:public __If
{
public:
	TemTComImpl(TREFIID inRIID):m_Ref(0),m_IID(inRIID)
	{
		//TCOM_AddClsidInstanceRefCount();

	}

	virtual ~TemTComImpl()
	{
		//TCOM_DecClsidInstanceRefCount();
	}
	
	virtual HRESULT QueryInterface ( /* [in] */TREFIID riid,/* [iid_is][out] */ void **ppvObject)
	{
		if(riid == TIID_IUnknown){
			*ppvObject = static_cast<TIUnknown*>(this);
		}
		else if(riid == m_IID) {
			*ppvObject = static_cast<__If*>(this);
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
		ULONG ref =  os_atomic_add(&m_Ref,1);
		if(ref == 1)
		{
			//TCOM_AddClsidInstanceRefCount();
			
		}
		LOGE("AddRef(%p,%ld)",this,ref);
		return ref;
	}

	virtual ULONG  Release()
	{
		ULONG ref = os_atomic_sub(&m_Ref,1);
		LOGE("Release(%p,%ld) >>",this,ref);
		if(0 == ref)	
		{
			delete this;
			//TCOM_DecClsidInstanceRefCount();
		}
		LOGE("Release(%p,%ld) <<",this,ref);
		return ref;
	}


private:
	ULONG m_Ref;
	TIID  m_IID;
};



#endif


