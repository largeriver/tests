#include "ITest.h"


class testC:public ITEST{
public:
    int CPP_STDMETHODCALLTYPE m1(int i){i++; return i;}
    int CPP_STDMETHODCALLTYPE m2(char i){i+=2;return i;};

};

static class testC test;

ITEST * CreateInstance_CPP(){
    return &test;
}


void c2cpptest()
{
    ITEST * ins =  CreateInstance_C();
    ins->m1(4);
}