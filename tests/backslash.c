#include <stdio.h>


int main()
{


	int testif = 2;
	testif +\
= 2;
	testif = 10;
	testif; /\
/ this is a single line comment continued from the previous line ... weird
	testif /\
* hello this is a block comment with the tokens split across lines *\
/ /= 2;

	/\
/ a single line comment\
continued on 2 lines?

	pri\
\
\
ntf("%d\n",          tes\
tif)\
;

	char mychar = '\
a\
';
	printf("%d\n", mychar);

	if (0) {	
	char mystr[] = "blah" "blah" // testing
		"blah"/* hello */"blah";
	printf("%s\n", mystr);

	char mystr2[] = "blah \
blah";
	printf("%s\n", mystr2);

	}

	double testd = 12\
3\
.\
321;
	printf("%f\n", testd);


	return 0;
}
