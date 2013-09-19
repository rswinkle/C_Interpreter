
#include <stdio.h>


int main()
{
	int i = 10;
	int j = 0;
	printf("%d\n", i);
	for (int i=0, j = 5; i<5; ++i) {
		int i = 2;
		printf("%d\n", i);
	}
	printf("%d\n", i);
	printf("%d\n", j);


	for (int i = 0; i<5; ++i)
		for (int i=0; i<5; ++i)
			printf("%d\n", i);
			
			
	for (int i = 0; i<5; ++i) {
		printf("%d\n", j);
		int j = -i;
		printf("%d\n", j);
	}
	
	printf("%d\n", i);
	printf("%d\n", j);


	for (; i > j; i-=1) {
		printf("%d\n", i);
		int i = 0;
		printf("%d\n", i);
	}

	printf("%d\n", i);
	printf("%d\n", j);

	return 0;
}
