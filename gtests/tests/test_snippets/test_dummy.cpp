#include "gtest/gtest.h"
//////////////////////////////////////////////
TEST(DummyTest, dummy) {
    EXPECT_TRUE(true);
    EXPECT_EQ(sizeof(int), 4);
}

///////////////////////////////////////////////////
class Dmummy2Test : public ::testing::Test {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {}

public:
};

TEST_F(Dmummy2Test, dummy) {
    EXPECT_TRUE(true);
    EXPECT_EQ(sizeof(int), 4);
    EXPECT_STREQ("avc","avc");
}

////////////////////////////////////////////////
