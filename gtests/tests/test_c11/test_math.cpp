#include "gtest/gtest.h"




class MathTest : public ::testing::Test {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {}

public:
};







class Solution {
public:
    int hammingWeight(unsigned int n) {
        n = ((n & 0xaaaaaaaa) >> 1) + (n & 0x55555555);
        n = ((n & 0xcccccccc) >> 2) + (n & 0x33333333);
        n = ((n & 0xf0f0f0f0) >> 4) + (n & 0x0f0f0f0f);
        n = ((n & 0xff00ff00) >> 8) + (n & 0x00ff00ff);
        n = ((n & 0xffff0000) >> 16) + (n & 0x0000ffff);
        return n;
    }
};

TEST_F(MathTest, dummy) {
    Solution s;
    EXPECT_EQ(s.hammingWeight(1), 1);

}
