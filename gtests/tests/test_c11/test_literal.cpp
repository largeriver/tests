#include "gtest/gtest.h"


//using namespace tangram;



class LiteralTest : public ::testing::Test {
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

TEST_F(LiteralTest, dummy) {
    EXPECT_TRUE(true);
    EXPECT_EQ(sizeof(int), 4);



}

