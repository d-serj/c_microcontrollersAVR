#include <stdlib.h>
#include <stdio.h>

int main ()
{
    char a1 = 0;
    int i;

    for(i=sizeof(a1)*8-1; i>=0; --i)
		printf("%d",(a1 >> i)&1);

	printf("\n");
    a1 ^= (1 << 0);


    for(i=sizeof(a1)*8-1; i>=0; --i)
		printf("%d",(a1 >> i)&1);

	printf("\n");
    a1 &= ~(1 << 0);

	for(i=sizeof(a1)*8-1; i>=0; --i)
	printf("%d",(a1 >> i)&1);


    return 0;
}
