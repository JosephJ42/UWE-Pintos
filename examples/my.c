#include <stdio.h>
#include <syscall.h>

int
main (void)
{
    //printf("Hello, World\n");
	//halt();
	//exit(0);
	//exec();
	//wait();
	const char *file;
	file = "file";
	create(file,4);
	//remove(file);
	open(file);
	//filesize(1);
	//read(1);
	//write(1);
	//seek(1);
	//tell(1);
	//close(4);
    return EXIT_SUCCESS;
}
