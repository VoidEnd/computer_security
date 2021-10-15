#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main()
{
	uid_t real_uid = getuid(); // get real user id
	uid_t effective_uid = geteuid(); // get effective user id
    
	char * fn = "/tmp/XYZ";
	char buffer[60];
	FILE *fp;
    
	/* get user input */
	scanf("%50s", buffer );
    
	seteuid (real_uid);
    
	if(!access(fn, W_OK)){
        fp = fopen(fn, "a+");
        fwrite("\n", sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        fclose(fp);
	}else {
        printf("No permission \n");
    }
    
	seteuid (effective_uid);
    
    return 0
}