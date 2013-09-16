#include <stdio.h>


int a=0, b=0, c=10;

int main()
{
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);

	int d = 9, e = 10 + (b += 1), f = d *= 2;
	printf("%d\n", d);
	printf("%d\n", e);
	printf("%d\n", f);

	int a = 50;
	printf("%d\n", a);
	printf("%d\n", b);
	int b = 100;
	printf("%d\n", b);

	return 0;
}


