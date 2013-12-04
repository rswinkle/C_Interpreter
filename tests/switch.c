



#include <stdio.h>


int main()
{
	int i = 1;
	for (int i=0; i<20; ++i) {
		switch (i) {
		case 1:
			printf("%d\n", 1);
			break;
		case 2:
			printf("%d\n", 2);
			break;
		case 1+2:
			printf("%d\n", 3);
			break;
		case 3 + (3 > 2):
			printf("%d\n", 4);
			break;
		case 5:
		case 6:
			printf("%d\n", i);
			break;
		case (2 > 1) ? 7 : 5:
			printf("%d\n", 7);
			break;
		case 8:
			printf("%d\n", 8);
			break;

		case 9:
		case 10:
		case (1+2+3)*2-1:
			printf("%d\n", i);
			break;

		//gcc errors on this "case label does not reduce to integer constant"
		//it errors with both std=c99 and gnu99
		//
		//clang compiles but gives a warning:
		//"warning: expression is not an integer constant expression; folding it to a constant is a GNU extension [-Wgnu]" 
		//I guess that warning is based on an old version of GCC that did allow it?
		//
		//tcc accepts it with no complaints
		//
		//it makes sense according to the grammar
		//case constant_expr ':'
		//constant_expr -> cond_expr -> ... -> primary_expr -> ( expr )
		//expr -> comma_expr
		//
		//as long as there are only integer literals in the chain I think
		//it should be accempted even though technically comma_expr allows assign_expr
		//
		//Update, according to pg 251 of C: A Reference Manual
		//"No constant expression may contain assignment, increment, decrement, function call,
		//or comma expressions unless they are contained within the operand of a sizeof operator."
		//
		//oh well I'm leaving it in
		case (2, 3, 12):
			printf("%d\n", 12);
			break;

		default:
		case 13:
			printf("%d\n", i);
		}
	}

	printf("%d\n", i);

	return 0;
}
