

#include <stdio.h>


int main()
{
	int i = 10 > 5 ? 10 : 5;
	int j = i == 11 ? 1 : 0;

	printf("%d\n", i);
	printf("%d\n", j);

	int k = i != 10 ? 20 : -20;

	printf("%d\n", k);

	//tcc 0.9.25 allows redeclaring k like this
	//tcc 0.9.26 errors saying:
	///error: incompatible types for redefinition of 'k'
	//which doesn't make sense to me since the type shouldn't matter
	//redeclaration should just be illegal
//	int k = i != 10 ? 20 : !j ? 123 : 321;

	k = i != 10 ? 20 : !(j+=1) ? 123 : 321;

	printf("%d\n", k);
	
	return 0;
}
