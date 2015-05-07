#include <stdio.h>
//#include <Windows.h>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
	printf("Running main() from test_main.cpp\n");
	testing::GTEST_FLAG(output) = "xml:D:/test-c11.xml";
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
#ifdef _WIN32
	system("pause");
#endif

	return 0;

}