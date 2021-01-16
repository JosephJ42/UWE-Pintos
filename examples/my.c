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
	create(file,10);
	//remove(file);
	open(file);
	//filesize(2);
        
        void *buffer = "Hello";
	write(2,buffer,5);
	void *buffer_r = "";	
	read(2,buffer_r,5);
	
	//seek(2,3);
	//tell(2);
	//close(2);
    return EXIT_SUCCESS;
}
