#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main(){
	setuid(geteuid());
	system("/bin/ls -l");
}
