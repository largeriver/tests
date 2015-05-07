#include "ITest.h"

 int STDMETHODCALLTYPE mym1(ITEST* This,int i){
    return i;
}

 int STDMETHODCALLTYPE mym2(ITEST* This,char i){
    return i+3;
}


static const struct ITESTlVtbl vtbl={
    mym1,
    mym2,
};

static struct ITEST test={&vtbl};

ITEST * CreateInstance_C(){
    return &test;

}


void cpp2ctest()
{
    ITEST * ins =  CreateInstance_CPP();
    ins->lpVtbl->m1(ins,4);
}