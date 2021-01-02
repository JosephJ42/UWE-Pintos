#include <stdio.h>
#include <syscall.h>

int
main (const char *file)
{
    //printf("Hello, World\n");
	
	exec(file);
    return EXIT_SUCCESS;
}
