

int a = 1;
#include "../tests/inc_test2.c"

void func2(int a, int b, int c)
{
	print a;
	print b;
	print c;

	int d = 12;
	{
		print d;
		print a;
		int a = 15;
		print a;
		{
			print d;
			print a;
			print b;

			int b = 100;
			print b;
			print c;
			{
				print c;
			}

		}
		int c = 5;
		print c;

	}
}


#define FIB_NUM 4

int fibbonaci(int n)
{
	print n;
	if (n < 3) {
		return 1;
	}
	int a = n + 2;
	print a;
	return fibbonaci(n-2) + fibbonaci(n-1);
}



