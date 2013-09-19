#include <stdio.h>


int main()
{
	int i = 0;
	while (i < 20) {
		i += 1;
		if (i % 2)
			continue;
		if (i == 16)
			break;
		printf("%d\n", i);
	}

	i = 0;
	while (i < 20) {
		i += 1;
		if (i % 2)
			continue;
		if (i == 16)
			break;
		printf("%d\n", i);
		
		int j = 20;
		while (j) {
			j -= 1;
			if (j % 2)
				continue;
			if (j == 4)
				break;
			printf("%d\n", j);
		}
	}

	for (int i=0, j=10; ; i+=1) {
		if (i % 2)
			continue;
		printf("%d\n", i);
		if (i == j)
			break;
	}
	
	printf("%d\n", i);


	return 0;
}
