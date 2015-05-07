#include <iostream>
#include <sstream>
#include <iterator>
#include <numeric>




#include "gtest/gtest.h"


//using namespace tangram;



class StreamTest : public ::testing::Test {
protected:
	virtual void SetUp() {

	}

	virtual void TearDown() {}

public:
};
//
TEST_F(StreamTest, istream_iterator) {
	/*
	http://en.cppreference.com/w/cpp/iterator/istream_iterator
	http://stackoverflow.com/questions/16727125/how-does-stdcopy-work-with-stream-iterators
	*/
	std::istringstream str("0.1 0.2 0.3 0.4");
	std::vector<double> ds(4, 0.0);
	//from str to cout
	//std::partial_sum(std::istream_iterator<double>(str),
	//	std::istream_iterator<double>(),
	//	std::ostream_iterator<double>(std::cout, " "));

	//from str to ds
	std::copy(std::istream_iterator<double>(str),
		std::istream_iterator<double>(),
		ds.begin());

	EXPECT_EQ(ds.size(), 4);
	EXPECT_EQ(ds[0], 0.1);

}


#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
#include <set>
TEST_F(StreamTest, insert) {
	/*
	http://en.cppreference.com/w/cpp/iterator/inserter
	*/
	std::list<int> l{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::multiset<int> s;

	std::fill_n(std::inserter(l, std::next(l.begin())), 3, -1);
	std::fill_n(std::inserter(s, s.begin()), 3, -1);

	for (int n : l) {
		std::cout << n << ' ';
	}
	std::cout << '\n';

	for (int n : s) {
		std::cout << n << ' ';
	}
	std::cout << '\n';



	EXPECT_EQ(l.size(), 13);
	auto it = l.begin();
	EXPECT_EQ(*it++, 1);
	EXPECT_EQ(*it++, -1);
	EXPECT_EQ(*it++, -1);
	EXPECT_EQ(*it++, -1);
	EXPECT_EQ(*it++, 2);


	EXPECT_EQ(s.size(), 3);
	EXPECT_EQ(*s.begin(), -1);

}


TEST_F(StreamTest, back_insert) {
	/*
	http://en.cppreference.com/w/cpp/iterator/back_inserter
	back_inserter is a convenience function template that constructs 
	a std::back_insert_iterator for the container c with the type 
	deduced from the type of the argument.

	template< class Container >
	std::back_insert_iterator<Container> back_inserter( Container& c )
	{
	return std::back_insert_iterator<Container>(c);
	}
	*/
	std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::fill_n(std::back_inserter(v), 3, -1);
	for (int n : v)
		std::cout << n << ' ';

	EXPECT_EQ(v.size(), 13);
	auto it = v.rbegin();
	EXPECT_EQ(*it++, -1);
	EXPECT_EQ(*it++, -1);
	EXPECT_EQ(*it++, -1);
	EXPECT_EQ(*it++, 10);
}


// back_insert_iterator example
/*
如何配合使用 istream_iterator 和 back_insert_iterator
*/
#include <iostream>     // std::cout
#include <iterator>     // std::back_insert_iterator
#include <vector>       // std::vector
#include <algorithm>    // std::copy

TEST_F(StreamTest, back_insert_iterator) {
	/*
	http://www.cplusplus.com/reference/iterator/back_insert_iterator/
	*/
	std::istringstream str("0.1 0.2 0.3 0.4");
	std::istringstream str1("0.1 0.2 0.3 0.4");

	std::vector<double> foo;
	
	std::back_insert_iterator< std::vector<double> > back_it(foo);

	std::copy(std::istream_iterator < double > {str},
		std::istream_iterator < double > {},
		back_it);
	
	EXPECT_EQ(foo.size(), 4);
	EXPECT_EQ(foo[0], 0.1);

	std::copy_if(std::istream_iterator < double > {str1},
		std::istream_iterator < double > {},
		std::back_inserter(foo) , 
		[](double x){ return x >= 0.4; });

	EXPECT_EQ(foo.size(), 5);
	EXPECT_EQ(foo[0], 0.1);
	
}


