


#include <stdio.h>


int main()
{
	int i = 0;

	do printf("%d\n", i); while (i);

	i = 10;
	do {
		printf("%d\n", i);
	} while (i--);

	printf("%d\n", i);

	i = 10;
	do {
		printf("%d\n", i);
	} while (--i);

	printf("%d\n", i);

	do {
		int j = 0;
		do printf("%d\n", j); while (++j < 5);
		printf("%d\n", i);
	} while (i++ < 3);
	
	printf("%d\n", i);

	return 0;
}
