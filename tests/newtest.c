#include <stdio.h>



int main()
{

	int i, a, b, c, d;
	i = 42 + 20 + 25; 
	printf("%d\n", i);
	i = i - 7 - 10;
	printf("%d\n", i);
	i = 3 + 4 * 5;
	printf("%d\n", i);
	a = 2;
	b = 8;
	c = 4;
	i = a * (b - 8 / (c - a));
	printf("%d\n", i);
	c = 10 - 3 - 4 - 1;
	printf("%d\n", c);
	a = b = c = 10 - 2 * 2;
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);
	i = a + b + 3, c *= 2, c -= 5;
	printf("%d\n", i);
	printf("%d\n", c);

	a = 10, i+=5, i-3;
	printf("%d\n", a);
	printf("%d\n", i);
	a = (10, i+=5, i-3);
	printf("%d\n", a);
	printf("%d\n", i);

	d = 0;
	a = b = d || (a += 15) && a == (1 + 6) % 5 * 10;
	printf("%d\n", a);
	printf("%d\n", b);

	a = 1, b =2, c = 3, d = 4;
	a += b += c += d;
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);
	printf("%d\n", d);

	a = a > (c += 2);
	printf("%d\n", a);
	printf("%d\n", c);
	



	return 0;
}
