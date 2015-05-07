#include "gtest/gtest.h"

#include <iostream>
using namespace std;
//using namespace tangram;



class UnicodeTest : public ::testing::Test {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {}

public:
};
//
TEST_F(UnicodeTest, dummy) {
    //VS2013尚不支持
    /*
    char u8_array[] = u8"你是谁";
    std::string u8_str = u8"你是谁";

    char16_t u16_array[] = u"谁在这里";
    std::u16string = u"谁在这里";

    char16_t u32_array[] = U"谁在这里";
    std::u32string = U"谁在这里";
    */

    EXPECT_TRUE(true);
    EXPECT_EQ(sizeof(int), 4);
}

/*
http://blog.poxiao.me/p/unicode-character-encoding-conversion-in-cpp11/
*/

