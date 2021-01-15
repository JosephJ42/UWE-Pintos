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
	char *file = "Test File";
	create(file,28);
	//remove(file);
	open(file);
	filesize(2);
	//read(1);
	//write(1);
	//seek(1);
	//tell(1);
	//close(4);
    return EXIT_SUCCESS;
}
