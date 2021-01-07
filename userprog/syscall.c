#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
//added includes
#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "process.h"
#include "threads/malloc.h"

typedef int pid_t;

static struct lock lock_file;


//syscalls prototypes
void halt(void);
void exit(int);
pid_t exec(const char *cmd_line);
int wait(pid_t pid);
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
int read(int fd, const void *buffer, unsigned size); 
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);
//Support function prototypes
void exit_invalid(void);
void verify_validity(int pointer);



//Structures
static void syscall_handler (struct intr_frame *);
struct file_in_use * get_file(int);

//test remove later
struct list open_files;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  //initlises the lock, 
  lock_init(&lock_file);

  list_init (&open_files);
}

//used to exit the current thread if the user address is invalid
void exit_invalid(){
  thread_current()->exit_code = -1;
  thread_exit();
}

//Check to see if the user address is valid
void verify_validity(int pointer){
  if (pointer==NULL){
	exit_invalid();
  }
  if(!is_user_vaddr(pointer)) {
	exit_invalid();
  }
  if(!pagedir_get_page(thread_current()->pagedir,pointer)) {
	exit_invalid();
  }
}

// this is a test implmentation, it will be most likely removed
struct file_in_use * get_file(int fd){
  struct list_elem *e;
  struct file_in_use *fd_struct; 
  e = list_tail (&open_files);
  while ((e = list_prev (e)) != list_head (&open_files)) 
    {
      fd_struct = list_entry (e, struct file_in_use, elem);
      if (fd_struct->fd_num == fd)
    return fd_struct;
    }
  return NULL;
}



static void
syscall_handler (struct intr_frame *f UNUSED)
{
  printf ("system call!\n");
  
  //checks if f ->esp is a valid pointer 
  int pointer = f->esp;
  verify_validity(pointer);
  
  //
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

	//Terminates the current user program, whilst returning the kernal status
	case SYS_EXIT:
	printf("SYSTEM CALL: Exit is being executed \n");
	
	verify_validity(pointer+1);
	
	int status = *((int*)f->esp+1);
	
	struct thread* cur = thread_current();

 	cur->exit_code = status;
	
	thread_exit();

	break;
        
        //Runs an executable given by the command line
	case SYS_EXEC:
	printf("SYSTEM CALL: Exec is being executed \n");
        

	
	//f->eax = exec(cmd_line);
	break;
	
	//
	case SYS_WAIT:
	printf("SYSTEM CALL: Wait is being executed \n");

	

	break;
	
        //Creates a new file
	case SYS_CREATE:
	printf("SYSTEM CALL: Create is being executed \n");
      	const char file_create= (void*)(*((char *)f->esp + 1));
	unsigned initial_size = *((unsigned*)f->esp + 2); 

	f->eax = create(file_create, initial_size);
	break;
	
	//Removes an existing file
	case SYS_REMOVE:
	printf("SYSTEM CALL: Remove is being executed \n");
	const char file_remove= (void*)(*((char *)f->esp + 1));
	
	f->eax = remove(file_remove);
	break;

	//Opens an existing file
	case SYS_OPEN:
	printf("SYSTEM CALL: Open is being executed \n");
	const char file_open= (void*)(*((char *)f->esp + 1));
	
	f->eax = open(file_open);
	break;
	
	//gets the size of a open file
	case SYS_FILESIZE:
	printf("SYSTEM CALL: File size is being executed \n");
	int fd_filesize = *((int*)f->esp + 1);
	f->eax = filesize(fd_filesize);
	break;

	//reads 
	case SYS_READ:
	printf("SYSTEM CALL: Read is being executed \n");
	
	int fd_read = *((int*)f->esp + 1);
	void* buffer_read = (void*)(*((int*)f->esp + 2));
	unsigned size_read = *((unsigned*)f->esp + 3);
	
	f->eax = read(fd_read, buffer_read, size_read);
	
	break;

	//writes
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
	unsigned position = *((unsigned*)f->esp + 2);

	seek(fd_seek,position);
	break;

	case SYS_TELL:
	printf("SYSTEM CALL: Tell is being executed \n");
	int fd_tell = *((int*)f->esp + 1);

	f->eax = tell(fd_tell);
	break;

	case SYS_CLOSE:
	printf("SYSTEM CALL: Close is being executed \n");
	int fd_close = *((int*)f->esp + 1);
	
	close(fd_close);
	
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

// Done, needs clean up
bool create(const char *file, unsigned initial_size){
if(file==NULL){
return false;
}
if (file){
lock_acquire (&lock_file);
filesys_create (file,initial_size);
lock_release (&lock_file);
return true;
}
else
return false;
}

//Done, needs clean up
bool remove(const char *file){
if(file==NULL){
return false;
}
if(file){
lock_acquire (&lock_file);
filesys_remove(file);
lock_release (&lock_file);
return true;
}
else
return false;
}

//Done, needs clean up
int open(const char *file){
int fd;
if (file == NULL){
	return -1;
}
else{
	lock_acquire (&lock_file);
	struct file *new_file = filesys_open(file);
	lock_release (&lock_file);

	//creates a place in the memory for the current file in use
	struct file_in_use *get_file_in_use = malloc(sizeof(struct file_in_use));

	//stores the file in use in the get_file_in_use structure
	get_file_in_use->fp = new_file;

	//gets the file decriptor of the current file in the current thread
	get_file_in_use->fd = thread_current()->fd;

	//increments the file decriptor so that when the next file is opened,
	//(in the case of the same file being opened mutiple time) the file descriptor is different  
	thread_current()->fd++;

     return get_file_in_use->fd;
    }
}

//Done, needs cleanup
int filesize(int fd_filesize){
int size_of_file;

struct file_in_use * file_filesize= get_file(fd_filesize);

lock_acquire (&lock_file);
size_of_file = file_length (file_filesize->fp);
lock_release (&lock_file);
return size_of_file; 
}

//Done, needs cleanup
int read(int fd_read, const void *buffer_read, unsigned size_read){

if(fd_read == 0){
buffer_read=input_getc();
return size_read;
}
else if (fd_read){
struct file_in_use * file_being_read= get_file(fd_read);
lock_acquire (&lock_file);
file_read(file_being_read->fp,buffer_read,size_read);
lock_release (&lock_file);
}
else {
return -1;
}

}

//Done, needs cleanup
int write(int fd, const void *buffer, unsigned size){
int bytes_written;
// writes to the console
if (fd ==1){
putbuf(buffer,size);
printf("\n");
bytes_written = size;
return bytes_written;
}
// writes to the given file (needs more work)
else{

lock_acquire (&lock_file);
// getting the fd of specified file
struct file_in_use * file_being_written= get_file(fd);

//writes to file and gets the number of bytes written
bytes_written = file_write(file_being_written->fp,buffer,size);
lock_release (&lock_file);
return bytes_written;
}

}

//Done, needs cleanup
void seek(int fd, unsigned position){
struct file_in_use * file_being_seeked= get_file(fd);

lock_acquire (&lock_file);
file_seek(file_being_seeked->fp,position);
lock_release (&lock_file);
}
//Done, needs cleanup
unsigned tell(int fd){
int position_of_file;
struct file_in_use * file_being_told= get_file(fd);
if(file_being_told){
lock_acquire (&lock_file);
position_of_file=file_tell(file_being_told->fp);
lock_release (&lock_file);
return position_of_file;
}
else{
return -1;
}
}

//Done, needs cleanup
void close (int fd){
struct file_in_use * file_being_closed= get_file(fd);
lock_acquire (&lock_file);
file_close(file_being_closed->fp);
lock_release (&lock_file);
}

