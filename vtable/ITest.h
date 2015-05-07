#ifdef __cplusplus
#define __STRUCT__ struct
#define interface __STRUCT__
#else
#define interface struct
#endif






#ifdef WIN32

    #define CPP_STDMETHODCALLTYPE      // __stdcall // __stdcall//__cdecl//
    #define STDMETHODCALLTYPE          __stdcall//__cdecl// __stdcall//__stdcall

#else
    #ifndef __stdcall
    #define __stdcall
    #endif

    #ifndef __cdecl
    #define __cdecl __attribute__((__cdecl__))
    #endif


    #define CPP_STDMETHODCALLTYPE       __cdecl //
    #define STDMETHODCALLTYPE           __cdecl ////__stdcall
#endif



#ifdef __cplusplus
extern "C" {
#endif

    typedef interface ITEST ITEST;


#if defined(__cplusplus) && !defined(CINTERFACE)

interface ITEST{
    virtual int CPP_STDMETHODCALLTYPE m1(int i) = 0;
    virtual int CPP_STDMETHODCALLTYPE m2(char i) = 0;
};


#else // #if defined(__cplusplus) && !defined(CINTERFACE)

    typedef struct ITESTlVtbl {
        int ( STDMETHODCALLTYPE *m1)(
            ITEST* This,
           int i);



        int ( STDMETHODCALLTYPE *m2)(
            ITEST* This,
            char i);

    }ITESTlVtbl;

struct ITEST {
    const  ITESTlVtbl* lpVtbl;
};
#endif //#if defined(__cplusplus) && !defined(CINTERFACE)




ITEST * CreateInstance_C(void);
ITEST * CreateInstance_CPP(void);

void cpp2ctest(void);
void c2cpptest(void);

#ifdef __cplusplus
}
#endif

