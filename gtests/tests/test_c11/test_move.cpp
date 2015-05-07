#include "gtest/gtest.h"


//using namespace tangram;



class MoveTest : public ::testing::Test {
protected:
	virtual void SetUp() {

	}

	virtual void TearDown() {}

public:
};

class MyClass{
public:
	MyClass() = default;



	template<typename _Tx>
	int test(_Tx & t){
		std::cout << "test ver1:" << t << std::endl;
		return 1;
	}

	template<typename _Tx>
	int test(const _Tx & t){
		std::cout << "test ver10:" << t << std::endl;
		return 10;
	}

	//我希望vector版本使用此版本
	template<typename _Tx>
	int test(std::vector<_Tx> & ts){
		std::cout << "test ver2:" << ts[0] << std::endl;
		return 2;
	}

		
	template<typename _Tx>
	int wrapper(_Tx && t){
		return test(std::forward<_Tx>(t));
	}

	template<typename _Tx>
	int atest(_Tx && t){
		std::cout << "test ver1:" << t << std::endl;
		return 1;
	}

	//我希望vector版本使用此版本
	template<typename _Tx>
	int atest(std::vector<_Tx> && ts){
		std::cout << "test ver2:" << ts[0] << std::endl;
		return 2;
	}

};
//
TEST_F(MoveTest, perfectforward) {
	MyClass c;
	int a = 1;
	std::vector<int> as = { 1, 2, 3 };

	

	EXPECT_EQ(c.test(a), 1);
	EXPECT_EQ(c.test(1), 10);
	EXPECT_EQ(c.test(as), 2);

	//这里就是完美转发
	EXPECT_EQ(c.wrapper(a), 1);
	EXPECT_EQ(c.wrapper(1), 10);
	EXPECT_EQ(c.wrapper(a = 1), 1);
	EXPECT_EQ(c.wrapper(as), 2);


	EXPECT_EQ(c.atest(a), 1);
	//EXPECT_EQ(c.atest(as), 2);//注意：编译失败，因为此模板实例化时仍然会使用int atest(_Tx && t)
	/*
	我们先来看第一个关键词“转发”，那么在C++中，“转发”表示什么含义呢？
	转发通常用在模板编程中，假设有一个函数F(a1, a2, ..., an)，如果存在另一个函数G(a1, a2, ..., an)，调用G相当于调用了F，
	则我们说函数G将a1, a2, ..., an等参数正确地转发给了函数F。
	再来看第二个关键词“完美”，“完美”转发是相对于其他转发方案而言的。
	*/
}
