// vtable.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>


#include "ITest.h"



//class testC:public ITEST{
//public:
//    int m1(int i){i++; return i;}
//    int m2(char i){i+=2;return i;};
//
//};

int main(int argc, char* argv[])
{
   // ITEST * test = CreateInstance_C();
   // test->m1(3);
    cpp2ctest();

    c2cpptest();
    
    system("pause");
	return 0;
}

