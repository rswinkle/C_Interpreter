
#include <stdio.h>

#define MULT(a, b) a * b
#define SUB(a, b) a - b

#define HALF SU
#define HALF2 B(a,b)

#define EXPR(a, b) MULT(a, b) + SUB(a, b)

#define TEST() whatever

#define TESTPARAM(a) +a

#define TESTPARAM2(A) A

#define BLAH1 BLAH2
#define BLAH2 EXPR

#define BLAH3 TEST() + EXPR

#define RECURSION(a) a() + b

#define RECURSION2(c, a, b) c(c, b, a)

#define RECURSION3() RECURSION3

#define MACRO 123
#define ONEMACRO 321
#define E2MACRO 234

#define TESTOVERLAP ONEMACRO
#define TESTOVERLAP2 ONE2MACRO

#define AAAA BBBB
#define BBBB AAAA

AAAA
BBBB

RECURSION2(RECURSION2, a, b)

RECURSION3()()()()() ()

int main()
{
	print("%d\n", EXPR (10, 5));

	int whatever = 5
	printf("%d\n", TEST());

	int b = 0;
	printf("%d\n", TESTPARAM(+)b);

	b = BLAH1(10, 4);
	printf("%d\n", b);
	b = BLAH3(10, 4);
	printf("%d\n", b);
	b = TESTOVERLAP;
	print b;
	b = MACRON;
	print b;

	printf("%d\n", EXPR(MULT(1, 2), SUB(4,1)));
	//print EXPR(+- +,-);
	int a = 2;
	b = 3;

	printf("%d\n", SUB(b, a));
	printf("%d\n", MULT(MULT(MULT(2,1), MULT(3,5)), MULT(MULT(10, 4), MULT(10, 4))));

	RECURSION(RECURSION);

	return 0;
}
