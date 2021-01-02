#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
//added includes
#include "threads/synch.h"

typedef int pid_t;

void halt(void);
void exit(int);
pid_t exec(const char *cmd_line);
//int write(int fd, const void *buffer, unsigned size)


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void
syscall_handler (struct intr_frame *f UNUSED)
{
  printf ("system call!\n");
  
  int system_call_number = *((int*)f->esp); // gets the system call code
  int parameter = *((int*)f->esp+4); // gets the system parameters

  printf("system_call_handler() - %d! \n", system_call_number);
  printf("perameter - %d! \n", parameter);
	switch (system_call_number){

	// Halts Pintos by calling the halt function, seen below
	case SYS_HALT:
	printf("SYSTEM CALL: Halt is being executed \n");
	halt();
	break;

	/* Terminates the current user program, whilst returning the kernal status */
	case SYS_EXIT:
	printf("SYSTEM CALL: Exit is being executed \n");
	
	int status = *((int*)f->esp+1);
	
	struct thread* cur = thread_current();

 	cur->exit_code = status; // or exit_code.
 	
	exit(status);

	break;
   
	case SYS_EXEC:
	printf("SYSTEM CALL: Exec is being executed \n");
        
	const char *file_name;
	char cmd_line = file_name;
        printf("%c \n",cmd_line);
	
	exec(cmd_line);
	break;
	
	case SYS_WAIT:
	printf("SYSTEM CALL: Wait is being executed \n");
	break;
	
	case SYS_CREATE:
	printf("SYSTEM CALL: Create is being executed \n");
	break;

	case SYS_REMOVE:
	printf("SYSTEM CALL: Remove is being executed \n");
	break;

	case SYS_OPEN:
	printf("SYSTEM CALL: Open is being executed \n");
	break;

	case SYS_FILESIZE:
	printf("SYSTEM CALL: File size is being executed \n");
	break;

	case SYS_READ:
	printf("SYSTEM CALL: Read is being executed \n");
	/*
	int fd =*(int *)(f->esp + 4);
	void *buffer = *(char**) (f->esp + 8);
	unsigned size = *(unsigned *)(f->esp + 12);
	
	f->eax = read(fd, buffer, size);
	*/
	break;

	case SYS_WRITE:
	printf("SYSTEM CALL: Write is being executed \n");
	/*
	int fd =*(int *)(f->esp + 4);
	void *buffer = *(char**) (f->esp + 8);
	unsigned size = *(unsigned *)(f->esp + 12);
	
	f->eax = write(fd, buffer, size);
	*/
	break;

	case SYS_SEEK:
	printf("SYSTEM CALL: Seek is being executed \n");
	break;

	case SYS_TELL:
	printf("SYSTEM CALL: Tell is being executed \n");
	break;

	case SYS_CLOSE:
	printf("SYSTEM CALL: Close is being executed \n");
	break;

	default:
	printf("Error! no system call was implemented");
	thread_exit();
	}

}

// halt function terminates Pintos
void halt(void){
shutdown_power_off();
}

//Terminates the current user program
void exit(int status){

//returns the kernal status
printf("%s: exit(%d)\n", thread_current()->name, status);
	
//then exits the thread
thread_exit();	
}

//need to fix
pid_t exec(const char *cmd_line){

int pid;

if (cmd_line == NULL){ // if the program cannot load or run 
    return -1;
}

pid = process_execute(cmd_line);

return pid;
}
/*
int wait (pid_t pid){

}


bool create(const char *file, unsigned initial_size){
if (){
return true;
}

return false;
}

bool remove(const char *file){

}

int open(const char *file){
}

int filesize(int fd){
}

int read(int fd, const void *buffer, unsigned size){
}

int write(int fd, const void *buffer, unsigned size){

if(fd==STDOUT_FILENO) {
putbuf((const char*)buffer, (unsigned ) size);
}
else {
printf("sys_write does not support fd output\n");
}

}
*/


