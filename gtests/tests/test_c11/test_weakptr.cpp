#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

/*

本测试主要是用来描述weakptr与shared_ptr的转换，其也并不增加shared_ptr的强引用计数。
*/
using namespace std;

class Controller
{
public:
	int Num;
	string Status;
	vector<weak_ptr<Controller>> others;
	explicit Controller(int i) : Num(i), Status("On")
	{
		//cout << "Creating Controller" << Num << endl;
	}

	~Controller()
	{
		//cout << "Destroying Controller" << Num << endl;
	}

	// Demonstrates how to test whether the   
	// pointed-to memory still exists or not.  
	void CheckStatuses() const
	{
		for_each(others.begin(), others.end(), [](weak_ptr<Controller> wp)
		{
			try
			{
				auto p = wp.lock();
				cout << "Status of " << p->Num << " = " << p->Status << endl;
			}

			catch (bad_weak_ptr b)
			{
				cout << "Null object" << endl;
			}
		});
	}
};



//int main()
//{
//	RunTest();
//	wcout << L"Press any key" << endl;
//	char ch;
//	cin.getline(&ch, 1);
//}

#include "gtest/gtest.h"
class WeakPtrTest : public ::testing::Test {
protected:
	virtual void SetUp() {

	}

	virtual void TearDown() {}

public:
};
//
TEST_F(WeakPtrTest, dummy) {
	

	vector<shared_ptr<Controller>> v;

	v.push_back(shared_ptr<Controller>(new Controller(0)));
	v.push_back(shared_ptr<Controller>(new Controller(1)));
	v.push_back(shared_ptr<Controller>(new Controller(2)));
	v.push_back(shared_ptr<Controller>(new Controller(3)));
	v.push_back(shared_ptr<Controller>(new Controller(4)));

	// Each controller depends on all others not being deleted.  
	// Give each controller a pointer to all the others.  
	for (int i = 0; i < v.size(); ++i)
	{
		for_each(v.begin(), v.end(), [v, i](shared_ptr<Controller> p)
		{
			if (p->Num != i)
			{
				v[i]->others.push_back(weak_ptr<Controller>(p));
				wcout << "push_back to v[" << i << "]: " << p->Num << endl;
			}
		});
	}

	for_each(v.begin(), v.end(), [](shared_ptr<Controller>& p)
	{
		wcout << "use_count = " << p.use_count() << endl;
		p->CheckStatuses();
		EXPECT_EQ(p.use_count(), 1);
	});

	
}

