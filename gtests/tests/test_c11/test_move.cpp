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

	//��ϣ��vector�汾ʹ�ô˰汾
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

	//��ϣ��vector�汾ʹ�ô˰汾
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

	//�����������ת��
	EXPECT_EQ(c.wrapper(a), 1);
	EXPECT_EQ(c.wrapper(1), 10);
	EXPECT_EQ(c.wrapper(a = 1), 1);
	EXPECT_EQ(c.wrapper(as), 2);


	EXPECT_EQ(c.atest(a), 1);
	//EXPECT_EQ(c.atest(as), 2);//ע�⣺����ʧ�ܣ���Ϊ��ģ��ʵ����ʱ��Ȼ��ʹ��int atest(_Tx && t)
	/*
	������������һ���ؼ��ʡ�ת��������ô��C++�У���ת������ʾʲô�����أ�
	ת��ͨ������ģ�����У�������һ������F(a1, a2, ..., an)�����������һ������G(a1, a2, ..., an)������G�൱�ڵ�����F��
	������˵����G��a1, a2, ..., an�Ȳ�����ȷ��ת�����˺���F��
	�������ڶ����ؼ��ʡ�����������������ת�������������ת���������Եġ�
	*/
}
