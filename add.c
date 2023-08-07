#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//assumes it can read the data from file descriptors 0 and 3.
//read 1st int from fd 0
//read 2nd int from fd 3
//output the total to fd 1

int main(int argc, char *argv[])
{
	int x, y, z;
	while (read(0, &x, sizeof(int)) > 0) {
		//if there is any argument
		//we assume that debug output is needed
		
		if (read(3, &y, sizeof(int)) > 0) {
			z = x + y;
			write(1, &z, sizeof(int));
			if (argc > 1) //any parameter enables debug output
				fprintf(stderr, "ADD Read #2: %d\nADD Result: %d\n", y, z);
		} else
			exit(1);
	}
	exit(0);
}	
