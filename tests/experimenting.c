
#include <stdio.h>

//float i = 5.5;
int i = 5;
int c = 15;


int blah = 25;

//int blah3 = c + i;





//{   < --error
	void test_func_in_block()
	{
		printf("test_func_in_block\n");
	}
//}   < --error





int* test_function()
{
//	printf("%d\n", j);   <--- error
	int j;

	//printf("%p %d\n", &a, a);   // <-- error

	static int a;
	a++;

	return &a;
}

int test_ret()
{
	int z = 2 + 2;
	printf("test_ret %d\n", z);
	//returns 11 which is return of printf but it's undefined
	//so this is gcc specific
}

/* error return type defaults to int
test_func3()
{
	printf("hello\n");
}
*/

// char test_function;  < -- illegal

int main()
{

	int i=1, a = 2, b, c = 10, d;

	printf("%d %d\n", i, a);
	{
		int i = 2, a = 3;
		printf("%d %d %d\n", i, a, c);
		c += 2;
		int c = 5;
		{
			printf("which i am I %d\n", i);
			int i = 3, a = 4;
			printf("%d %d %d\n", i, a, c);

		}
	}

	printf("%d %d %d\n", i, a, c);


	for (int j=0; j<10; ++j) {
		printf("outer j = %d\n", j);
		for (int j = 0; j<5; ++j) {
			printf("j = %d\n", j);
			{
				int j = j+1;
				printf("inner j = %d\n", j);	
			}
			{
				int j = j-1;
				printf("inner2 j = %d\n", j);
			}

		}
	}
	printf("\n\n\n");

	printf("%p\n", &i);
	int* tmp;
	for (int i=0; i<3; ++i) {
		tmp = test_function();
		printf("i = %p %d\n", tmp, *tmp);
	}

	float test_function = 1.2;     // <-- legal
	printf("%f\n", test_function);
	test_function();               // <-- ilegal

	printf("main function = %p\n", main);
	double main = 10.5;
	printf("%f\n", main);



	printf("test_ret return = %d\n", test_ret());


	printf("%d %d\n", blah, blah);


	int testif = 2
	if (testif > 1) {
		int amihere = 3;
		printf("amihere = %d\n", amihere);
	}

	printf("amihere = %d\n", amihere);










	return 0;
}



int blah2 = 50;
