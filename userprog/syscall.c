#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
//added includes
#include "threads/synch.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "process.h"

typedef int pid_t;

static struct lock lock_file;



//prototypes
static void halt(void);
static void exit(int);
static pid_t exec(const char *cmd_line);
static int wait(pid_t pid);
static bool create(const char *file, unsigned initial_size);
static bool remove(const char *file);
static int open(const char *file);
static int filesize(int fd);
static int read(int fd, const void *buffer, unsigned size); 
static int write(int fd, const void *buffer, unsigned size);
static void seek(int fd, unsigned position);
static unsigned tell(int fd);
static void close(int fd);


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  lock_init(&lock_file);
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
	shutdown_power_off();
	break;

	/* Terminates the current user program, whilst returning the kernal status */
	case SYS_EXIT:
	printf("SYSTEM CALL: Exit is being executed \n");
	
	int status = *((int*)f->esp+1);
	
	struct thread* cur = thread_current();

 	cur->exit_code = status; // or exit_code.
	
	thread_exit();

	break;
   
	case SYS_EXEC:
	printf("SYSTEM CALL: Exec is being executed \n");
        
	const char *file_name;
	char cmd_line = file_name;
        printf("%c \n",cmd_line);
	
	f->eax = exec(cmd_line);
	break;
	
	case SYS_WAIT:
	printf("SYSTEM CALL: Wait is being executed \n");
	break;
	
	case SYS_CREATE:
	printf("SYSTEM CALL: Create is being executed \n");
      	const char *file;
	unsigned initial_size = 1;
	f->eax = create(file, initial_size);
	break;

	case SYS_REMOVE:
	printf("SYSTEM CALL: Remove is being executed \n");
	//const char *file;
	//f->eax = remove(file);
	break;

	case SYS_OPEN:
	printf("SYSTEM CALL: Open is being executed \n");
	//const char *file;
	//f->eax = open(file);
	break;

	case SYS_FILESIZE:
	printf("SYSTEM CALL: File size is being executed \n");
	int fd_filesize = *((int*)f->esp + 1);
	f->eax = filesize(fd_filesize);
	break;

	case SYS_READ:
	printf("SYSTEM CALL: Read is being executed \n");
	
	int fd_read = *((int*)f->esp + 1);
	void* buffer_read = (void*)(*((int*)f->esp + 2));
	unsigned size_read = *((unsigned*)f->esp + 3);
	
	f->eax = read(fd_read, buffer_read, size_read);
	
	break;

	case SYS_WRITE:
	printf("SYSTEM CALL: Write is being executed \n");
	
	int fd = *((int*)f->esp + 1);
	void* buffer = (void*)(*((int*)f->esp + 2));
	unsigned size = *((unsigned*)f->esp + 3);
		
	
	f->eax = write(fd,buffer,size);
	
	break;

	case SYS_SEEK:
	printf("SYSTEM CALL: Seek is being executed \n");
	int fd_seek = *((int*)f->esp + 1);
	break;

	case SYS_TELL:
	printf("SYSTEM CALL: Tell is being executed \n");
	int fd_tell = *((int*)f->esp + 1);
	f->eax = tell(fd_tell);
	break;

	case SYS_CLOSE:
	printf("SYSTEM CALL: Close is being executed \n");
	int fd_close = *((int*)f->esp + 1);

	
	break;

	default:
	printf("Error! no system call was implemented");
	thread_exit();
	}

}

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
*/

bool create(const char *file, unsigned initial_size){
if (file){
lock_acquire (&lock_file);
filesys_create (file,initial_size);
lock_release (&lock_file);
return true;
}
else
return false;
}

bool remove(const char *file){

}

int open(const char *file){
}


int filesize(int fd_filesize){

lock_acquire (&lock_file);
file_length (fd_filesize);
lock_release (&lock_file);
 
}

//needs to be fix
int read(int fd_read, const void *buffer_read, unsigned size_read){

if(fd_read == 0){
buffer_read=input_getc();
}
else{
lock_acquire (&lock_file);
file_read(/*->file*/fd_read,buffer_read,size_read);
lock_release (&lock_file);
}


}


int write(int fd, const void *buffer, unsigned size){
// writes to the console
if (fd ==1){
putbuf((const char*)buffer, (unsigned ) size);
printf("\n");
}
// writes to the given file (needs more work)
else{

lock_acquire (&lock_file);
file_write(/*->file*/fd,buffer,size);
lock_release (&lock_file);
}

}

void seek(int fd, unsigned position){
}

unsigned tell(int fd){
/*
if(){
file_tell (struct file *);
}*/
}

void close (int fd){}

