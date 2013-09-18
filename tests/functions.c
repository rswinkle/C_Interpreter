
#include <stdio.h>

int a = 1;

void func1(void)
{
	printf("%d\n", a);
	int a = 2;
	printf("%d\n", a);
	int local = 10 * a;
	printf("%d\n", local);
}

void func2(int a, int b, int c)
{
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);

	int d = 12;
	{
		printf("%d\n", d);
		printf("%d\n", a);
		int a = 15;
		printf("%d\n", a);
		{
			printf("%d\n", d);
			printf("%d\n", a);
			printf("%d\n", b);

			int b = 100;
			printf("%d\n", b);
			printf("%d\n", c);
			{
				printf("%d\n", c);
			}

		}
		int c = 5;
		printf("%d\n", c);

	}
}

int fibbonaci(int n)
{
	printf("%d\n", n);
	if (n < 3) {
		return 1;
	}
	int a = n + 2;
	printf("%d\n", a);
	return fibbonaci(n-2) + fibbonaci(n-1);
}


int main()
{
	int local = 54;
	printf("%d\n", local);
	func1();
	printf("%d\n", a);
	func2(10, local /= 3, local - a*20);
	printf("%d\n", local);

	local = fibbonaci(4);
	printf("%d\n", local);

	return 0;
}


