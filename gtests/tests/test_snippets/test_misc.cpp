#include "gtest/gtest.h"
#include <type_traits> 
#include <iostream> 



struct base
{
	int val;
};

struct derived
	: public base
{
};

class MiscTest : public ::testing::Test {
protected:
	virtual void SetUp() {
	}

	virtual void TearDown() {}
};

TEST_F(MiscTest, decltype) {

	int a = 3, b = 4;
	decltype(a) c = a;
	decltype((b)) d = a;
	++c;
	++d;

	
	EXPECT_EQ(a,4);
	EXPECT_EQ(b,4);
	EXPECT_EQ(c, 4);
	EXPECT_EQ(d, 4);


	//Assignment is an example of an expression that yields a reference type.
	
};

TEST_F(MiscTest, decltype2) {
	//Assignment is an example of an expression that yields a reference type.
	int a = 3, b = 4;
	decltype(a) c = a;
	decltype(a = b) d = a;//注意这里，a=b仅用于推导，但并不执行

	std::cout << "a=" << a << " b=" << b << " c=" << c << " d=" << d << std::endl;

	EXPECT_EQ(a, 3);
	EXPECT_EQ(b, 4);
	EXPECT_EQ(c, 3);
	EXPECT_EQ(d, 3);// int& d

};




TEST_F(MiscTest, decltype_auto) {
	//Give an example of an expression where auto and  decltypewill deduce the same type and an example where they will deduce differing types.
	//Assignment is an example of an expression that yields a reference type.
	int a = 3, b = 4,*c = &b;
	decltype(*c) d1 = b;
	auto e1 = *c;
	b++;
	EXPECT_EQ(d1, 5);
	EXPECT_EQ(e1, 4);
	//a =3 b =5 


	decltype(a = *c) d2 = a; //int &d2 = a 
	auto e2 = (a=*c);		//int e2 = a = 5
	a++;
	EXPECT_EQ(d2, 6);
	EXPECT_EQ(e2, 5);
	//a = 6,b = 5

	decltype((b)) d3 = a;//int &d3 = a
	auto e3 = ((b));	// int e3 = 5
	b++;
	EXPECT_EQ(d3, 6);
	EXPECT_EQ(e3, 5);

	
};

TEST_F(MiscTest, expression){
	/*
	http://my.oschina.net/u/90679/blog/109042
	*/
	int a = 10; int b = 6;
	int c = 0;
	std::cout << (c = a + b) << " " << a++ << " " << b++ << std::endl;
	EXPECT_EQ(c, 18);
}