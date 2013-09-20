#include <stdio.h>


int main()
{
	int i, j = 3;

start1:
	i = j;

	int a = 5;
start2:
	printf("%d\n", i);
	--i;
	a--;
	printf("%d\n", a);
	if (i)
		goto start2;

	printf("%d\n", j);
	if (--j)
		goto start1;


	printf("%d\n", i);
	printf("%d\n", j);
	printf("%d\n", a);


	int b = 0;
	goto into;

	{
		int b = 4;

into:
		++b;
		printf("%d\n", b);
		int a = b / 2;
		printf("%d\n", a);
	}

	{
		int b = 3;
		printf("%d\n", b);

		goto outof;

		int a = b++;
		printf("%d\n", a);
		printf("%d\n", b);
	}

outof:
	printf("%d\n", b);


//goto endblock;  <--- illegal/non-standard pointless

	{
		int i = 100;
		printf("%d\n", i);
//endblock:
	}


goto beginblock;

	j += 10;
	printf("%d\n", j);

beginblock:
	{
		int j = 200;
		printf("%d\n", j);
	}


	{
		int a = 10;
		printf("%d\n", a);
		{
			goto sib_inner;
			{
				printf("%d\n", a);
			}
			int a = 5;
			printf("%d\n", a);
		}

		printf("%d\n", a);
	
		{
			int a = 15;
			{
sib_inner:
				printf("%d\n", a);

			}
		}
	}

	goto decl;

	{
		int a = 10;
		{
			int a = 5;
			printf("%d\n", a);
decl:
			;    // <-- label can only go with a statement not a declaration
			int b = 6;
			printf("%d\n", b);

		}
	}

	int check = 2;	
	{
		int test = 321;
		{
backinto:
			printf("%d\n", test);

		}
		printf("%d\n", test);
		test /= 2;
		check--;
	}

	int test = 123;
	printf("%d\n", test);
	
	if (check)
		goto backinto;


	return 0;
}
	
