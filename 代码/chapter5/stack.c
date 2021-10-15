#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int foo(char *str)
{
	char buffer[100];

	/*The following statement has a buffer overflow problem*/
	strcpy(buffer, str);
	
	return 1;
}

int main(int argc, char**argv)
{
	char str[400];
	FILE *badfile;

	/*Change the size of the dummy array to randomize the parameters
	for this lab. Need to use the array at least once*/
	// char dummy[BUF_SIZE*5];  memset(dummy, 0, BUF_SIZE*5);

	badfile = fopen("badfile", "r");
	fread(str, sizeof(char), 300, badfile);
	foo(str);

	printf("Returned Properly\n");
	return 1;
}
