#include "gtest/gtest.h"

#include <exception>
//using namespace tangram;



class SecuritylTest : public ::testing::Test {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {}

public:
};

/*�û��Զ��������� VS2013��֧��*/
/*
struct Watt{ unsigned int v; };
Watt operator "" _w(unsigned long long v){
return{ (unsigned int)v };
}
*/

TEST_F(SecuritylTest, dummy) {
 
    /*��windows�ϣ���δ���ֱ�ӱ��ж�ΪSEH�쳣��������ִ��catch�еĴ���*/
    /*
    try{
        printf("%s%s%s%s%s%s%s%s%s%s%s%s");
    }
    catch (...)
    {
        EXPECT_TRUE(1);
    }
    */

     printf("hahah");
}

