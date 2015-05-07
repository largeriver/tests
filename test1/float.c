#include <stdio.h>

float add(float a, float b);
int main()
{
	float a = 3.245;
	float b = 8.245;
	add(a,b);
	return 0;
}

float add(float a, float b)
{
	return a + b;
}
