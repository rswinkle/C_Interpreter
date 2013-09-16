#include <stdio.h>

int a = 1;



int main()
{
	int local = 54;
	int test1 = 1, test2 = 2, test3 = 3;

	if (local == 54)
		printf("%d\n", local);
	else {
		local += 1;
	}
	printf("%d\n", local);

	if (local < 60)
		printf("%d\n", test1);
	if (local < 55)
		printf("%d\n", test2);
	else
		printf("%d\n", test3);
		
	if (local > 60)
		printf("%d\n", test1);
	if (local > 55)
		printf("%d\n", test2);
	else
		printf("%d\n", test3);
	
	if (local -= local) {
		int a = 123;
		printf("%d\n", a);
		a + 2;
	}

	if (!local) {
		if (test3 == 3)
			printf("%d\n", test3);
		else
			printf("%d\n", test2);
		if (test1 == test3 - test2) {
			printf("%d\n", test1);
			printf("%d\n", local);
		} else {
			int tmp = test1 - 2;
			printf("%d\n", tmp);
		}
	} else {
		local += test1 * test2 * test3;
		printf("%d\n", local);
	}
	
	local = !local;
	if (!local) {
		if (test3 == 3)
			printf("%d\n", test3);
		else
			printf("%d\n", test2);
		if (test1 == test3 - test2) {
			printf("%d\n", test1);
			printf("%d\n", local);
		} else {
			int tmp = test1 - 2;
			printf("%d\n", tmp);
		}
	} else {
		local += test1 * test2 * test3;
		printf("%d\n", local);
	}


	return 0;
}


