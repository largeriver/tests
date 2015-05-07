/*
Pimpl For Compile - Time Encapsulation(Modern C++)
https://msdn.microsoft.com/en-us/library/hh438477.aspx
*/
#include <memory>
using namespace std;


// my_class.h
class my_class {
	my_class();
	//  ... all public and protected stuff goes here ...

private:
	/*这里的好处是:impl的实现完全隐藏起来，也难于出现命名冲突。*/
	class impl; 
	unique_ptr<impl> pimpl; // opaque type here
};


///////////////////////////////////////////////////////////////////////

// my_class.cpp
class my_class::impl {  // defined privately here
	// ... all private data and functions: all of these
	//     can now change without recompiling callers ...
};


my_class::my_class() : pimpl(new impl)
{
	// ... set impl values ... 
}

