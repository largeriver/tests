#include <stdio.h>

float fl(float a, float b)
{
	float c = 0.5;
	long i = 0;
	while (i < 1000000) {
		float c = a / b;
		a = a / 1.002;
		b = b / 0.999;
		i++;
	//	printf("%f\n", c);
	}

	return c;
}


int main()
{
	float a = 984.795;
	float b = 2.2034;
	fl(a, b);

	//printf("%f\n",ret);
	return 0;
}
