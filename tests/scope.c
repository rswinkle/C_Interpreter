#include <stdio.h>


int a = 10 % 3, b = 13 * 12 + 2, c = 10;

int main()
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

		{
			printf("%d\n", b);
			int b = 50;
			printf("%d\n", b);
		}

		int c = 5;
		printf("%d\n", c);

	}


	return 0;
}


