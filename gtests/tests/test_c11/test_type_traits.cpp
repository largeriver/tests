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

class TypeTraitsTest : public ::testing::Test {
protected:
	virtual void SetUp() {
	}

	virtual void TearDown() {}
};

TEST_F(TypeTraitsTest, is_base_of) {
	std::cout << "is_base_of<base, base> == " << std::boolalpha
		<< std::is_base_of<base, base>::value << std::endl;
	std::cout << "is_base_of<base, derived> == " << std::boolalpha
		<< std::is_base_of<base, derived>::value << std::endl;
	std::cout << "is_base_of<derived, base> == " << std::boolalpha
		<< std::is_base_of<derived, base>::value << std::endl;

	
	EXPECT_TRUE((std::is_base_of<base, base>::value));
	EXPECT_TRUE((std::is_base_of<base, derived>::value));
	EXPECT_FALSE((std::is_base_of<derived, base>::value));


};

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
工作中遇到一个问题，我有一个容器，装着各式各样的对象的指针，需要把拥有dump方法的指针内容dump出去，而对于没有dump方法的对象，直接忽略。
首先想到的是给每个对象提供一个查询操作，从而得知是否拥有dump方法。显然这个方法不能让人满意，需要更改大量的class实现。C++如果我能自动判断某个类型是否拥有某方法，这个问题可以完美的解决，因为是否含有某方法编译期已经确定了，所以是有可能通过一些技巧来实现这个功能的。
*/

/*
template<typename U> static int Test(...); 能匹配所有的类型U
template<typename U, void (U::*)()> struct HELPS; 
当Type U有hello，且hello的类型为 void (U::*)()时，模板HELPS<U, &U::hello>能正确匹配，否则模板无法匹配
Test<T>(0)  优先匹配 HELPS<U, &U::hello> 因为template<typename U> static char Test(HELPS<U, &U::hello>*) 是一个特化
sizeof操作符不需要计算表达式的值，是一个编译期的操作，定义指针类型可以只有声明没有定义
所以HELPS和Test都不需要实现，仅有声明就可以通过编译


*/
template<typename T>
struct has_hello{
	template<typename U, void (U::*)()> struct HELPS;
	template<typename U> static char Test(HELPS<U, &U::hello>*);
	template<typename U> static int Test(...);
	const static bool Has = sizeof(Test<T>(0)) == sizeof(char);
};

//测试某个类型是否存在某个方法
TEST_F(TypeTraitsTest, has_a_func){
	struct A
	{
		void hello(){
			std::cout << "A is Hello." << std::endl;
		}
		int x;
	};
	EXPECT_TRUE(has_hello<A>::Has);

}
