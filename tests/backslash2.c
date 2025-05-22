#include <stdio.h>


int main()
{
	char mychar = '\
n\
';
	printf("%c\n", mychar);

	mychar = '\
"';
	printf("%c\n", mychar);

	mychar = '\
\"';
	printf("%c\n", mychar);

	mychar = '\
\n\
';
	printf("'%c'\n", mychar);

	char mystr[] = "hello \
goodbye\nwhat\
n 'is\" going\
\non";
	printf("%s\n", mystr);

	char mystr2[] = "hel\lo";

	printf("%s\n", mystr2);

	return 0;
}

