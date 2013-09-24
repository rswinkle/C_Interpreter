#include <stdio.h>

double d = 2.48;


double sqrt(double x)
{
	double guess = 1.0;
	double tmp;
	while (1) {
		tmp = guess * guess - x;
		tmp = (tmp < 0) ? 0 - tmp : tmp;
		if (tmp < 0.001)
			return guess;
		guess = (guess + x/guess) / 2;
	}
}

int testfunc(int a, double b)
{
	printf("%d\n", a);
	printf("%f\n", b);
	return a + b;
}

short testshort(short a, unsigned short b)
{
	printf("%d\n", a);
	printf("%u\n", b);

	return a + b;
}


int main()
{
	short unsigned s = 100;
	int short unsigned crap = 123;
	int a = 5;

	printf("%f\n", d);
	printf("%d\n", a);

	a = d;
	d += a;
	printf("%f\n", d);
	printf("%d\n", a);

	d = sqrt(2.0);
	printf("%f\n", d);
	printf("%f\n", sqrt(25));

	printf("%d\n", testfunc(a, d));
	printf("%d\n", testfunc(d, d));

	d = 2.5;
	a = 70000;
	printf("%d\n", testfunc(a, d));
	printf("%d\n", testfunc(d, d));

	//2 4 8 so my conversions are ok here at least
	//printf("%lu %lu %lu\n", sizeof(short), sizeof(int), sizeof(long));
	return 0;
}
