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
����������һ�����⣬����һ��������װ�Ÿ�ʽ�����Ķ����ָ�룬��Ҫ��ӵ��dump������ָ������dump��ȥ��������û��dump�����Ķ���ֱ�Ӻ��ԡ�
�����뵽���Ǹ�ÿ�������ṩһ����ѯ�������Ӷ���֪�Ƿ�ӵ��dump��������Ȼ������������������⣬��Ҫ���Ĵ�����classʵ�֡�C++��������Զ��ж�ĳ�������Ƿ�ӵ��ĳ���������������������Ľ������Ϊ�Ƿ���ĳ�����������Ѿ�ȷ���ˣ��������п���ͨ��һЩ������ʵ��������ܵġ�
*/

/*
template<typename U> static int Test(...); ��ƥ�����е�����U
template<typename U, void (U::*)()> struct HELPS; 
��Type U��hello����hello������Ϊ void (U::*)()ʱ��ģ��HELPS<U, &U::hello>����ȷƥ�䣬����ģ���޷�ƥ��
Test<T>(0)  ����ƥ�� HELPS<U, &U::hello> ��Ϊtemplate<typename U> static char Test(HELPS<U, &U::hello>*) ��һ���ػ�
sizeof����������Ҫ������ʽ��ֵ����һ�������ڵĲ���������ָ�����Ϳ���ֻ������û�ж���
����HELPS��Test������Ҫʵ�֣����������Ϳ���ͨ������


*/
template<typename T>
struct has_hello{
	template<typename U, void (U::*)()> struct HELPS;
	template<typename U> static char Test(HELPS<U, &U::hello>*);
	template<typename U> static int Test(...);
	const static bool Has = sizeof(Test<T>(0)) == sizeof(char);
};

//����ĳ�������Ƿ����ĳ������
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
