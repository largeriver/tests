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

/*用户自定义字面量 VS2013不支持*/
/*
struct Watt{ unsigned int v; };
Watt operator "" _w(unsigned long long v){
return{ (unsigned int)v };
}
*/

TEST_F(SecuritylTest, dummy) {
 
    /*在windows上，这段代码直接被判定为SEH异常，根本不执行catch中的代码*/
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

