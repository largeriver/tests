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
    //VS2013�в�֧��
    /*
    char u8_array[] = u8"����˭";
    std::string u8_str = u8"����˭";

    char16_t u16_array[] = u"˭������";
    std::u16string = u"˭������";

    char16_t u32_array[] = U"˭������";
    std::u32string = U"˭������";
    */

    EXPECT_TRUE(true);
    EXPECT_EQ(sizeof(int), 4);
}

/*
http://blog.poxiao.me/p/unicode-character-encoding-conversion-in-cpp11/
*/

