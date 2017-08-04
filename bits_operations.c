#include <stdlib.h>
#include <stdio.h>

void print_byte (char);

int main ()
{
    char a1 = 0;
    int i;

    print_byte(a1);

    a1 ^= (1 << 0);
    print_byte(a1);

    a1 &= ~(1 << 0);
    print_byte(a1);

    return 0;
}

void print_byte (char input)
{
    int i;

    for(i=sizeof(input)*8-1; i>=0; --i)
		printf("%d",(input >> i)&1);

    putchar('\n');
}
