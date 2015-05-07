#include "gtest/gtest.h"

//http://www.geeknews8.com/989.html

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

class PythonTest : public ::testing::Test {
protected:
	virtual void SetUp() {
	}

	virtual void TearDown() {}
};



TEST_F(PythonTest, RasString) {
	//not support raw string
	/*
	const char* path = r"C:\Python27\Doc";
	const char* path2 = "C:\\Python27\\Doc";

	EXPECT_EQ(path, path2);
	*/
}

TEST_F(PythonTest, Loop) {
	int list[]{ 1, 2, 3, 4, 5, 6 };
	int j = 0;
	for (int i : list){
		j++;
	}
	EXPECT_EQ(j, sizeof(list)/sizeof(int));

}

TEST_F(PythonTest, TUNPLE){
	auto triple = std::make_tuple(5, "hello", true);
	std::cout << std::get<0>(triple);


	int x;
	const char * y = NULL;
	bool z;
	std::tie(x, y, z) = triple;

	EXPECT_EQ(5, x);
	EXPECT_STREQ("hello", y);
	EXPECT_TRUE(z);
}

#include <unordered_map>

TEST_F(PythonTest, UnorderMap){
	auto myDict = std::unordered_map<int, const char*>{ { 5, "foo" }, { 6, "bar" } };
	std::cout << myDict[5];

	EXPECT_STREQ("bar", myDict[6]);

}

#include <algorithm>
TEST_F(PythonTest, lambda){
	auto mylist = std::vector < int > {2, -4, -7, 1, -3, 6, 0};
	std::sort(mylist.begin(), mylist.end(), [](int x, int y){ return std::abs(x) < std::abs(y); });

	bool _eq = mylist == std::vector < int > {0, 1, 2, -3, -4, 6, -7};
	EXPECT_TRUE(_eq);

	/* not support yee
	auto adder(int amount) {
		return [=](int x){ return x + amount; };
	}
	*/


}

int foo(std::initializer_list<int> args){
	int sum = 0;
	for (int i : args)
		sum += i;
	return sum;

}

TEST_F(PythonTest, initializer_list){

	EXPECT_EQ(foo({ 4, 5, 6 }), 15);
}




