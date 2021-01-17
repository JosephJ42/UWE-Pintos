#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
//Added includes which provide functions
//for syscalls
#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "process.h"
#include "threads/malloc.h"

//Definied the type pid_t for the exec & wait syscall 
typedef int pid_t;

//Provides a structure for the file locks
static struct lock lock_file;

//Syscalls prototypes/casts
void halt(void);
void exit(int);
pid_t exec(const char *cmd_line);
int wait(pid_t pid);
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
int read(int fd, void *buffer, unsigned size); 
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);

//Support function prototypes/casts
void exit_invalid(void);
void verify_validity(int pointer);

//Structures
static void syscall_handler (struct intr_frame *);

struct file_in_use * get_file(int);
struct list file_list;
struct list open_files;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  //initlises the lock, 
  lock_init(&lock_file);

  // initilise file list for getting open files
  list_init(&file_list);
}

//used to exit the current thread if the user address is invalid
void exit_invalid(){
  thread_current()->exit_code = -1;
  thread_exit();
}

//This acts a validation for the pointers provided,
//it checks to see if the pointer actually exists.
//checks to see if the user address of the pointer is valid
//and checks to see if the physical address matches
//the corresponding user virtual address
void verify_validity(int pointer){
  if (pointer==NULL){
	printf ("pointer is null\n");
	exit_invalid();
  }
  if(!is_user_vaddr(pointer)) {
	printf ("pointer not in valid user address\n");
	exit_invalid();
  }
  if(!pagedir_get_page(thread_current()->pagedir,pointer)) {
	printf ("pointer not in valid physical address\n");
	exit_invalid();
  }
}

// Used to navigate through the file list of the current thread
// returning the relevent file based on the fd provided
struct file_in_use * get_file(int fd){

  struct list_elem *e;
  
  for (e = list_begin(&file_list); e != list_end(&file_list); e = list_next(e) ) 
    {
     struct file_in_use * get_file_in_use = list_entry(e, struct file_in_use, file_element);
      
      if (get_file_in_use->fd == fd){
      return get_file_in_use;
      }
    }
printf("No file present with that fd");
return NULL;
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  printf ("system call!\n");
  
  //checks if f ->esp is a valid pointer
  int pointer; 
  pointer = f->esp;
  verify_validity((void*)pointer);
  
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
	//passing any given arguments 
	//Currently not working
	case SYS_EXEC:
		printf("SYSTEM CALL: Exec is being executed \n");
        
		const char *cmd_line;
	
		f->eax = exec(cmd_line);
	break;
	
	//Waits for a child process pid and retrieves the processes exit code
	//Currently not working
	case SYS_WAIT:
	printf("SYSTEM CALL: Wait is being executed \n");
	/*
	verify_validity(pointer+1);	
	pid_t pid_wait = *((int*)f->esp+1);
	f->eax = wait(pid_wait);
	*/
	break;
	
        //Creates a new file
	case SYS_CREATE:
		printf("SYSTEM CALL: Create is being executed \n");
		//Validates the argument being passed from the stack 
		verify_validity(pointer+1);
		verify_validity(pointer+2);
      		
		//pulls these arguments off the stack
		const char* file= ((char*) *((int*)pointer + 1));
		unsigned initial_size = *((unsigned*)pointer + 2);

		//
		f->eax = create(file, initial_size);
	break;
	
	//Removes an existing file
	case SYS_REMOVE:
		printf("SYSTEM CALL: Remove is being executed \n");
		verify_validity(pointer+1);
		const char *file_remove= ((char*) *((int*)pointer + 1));;
	
		f->eax = remove(file_remove);
	break;

	//Opens an existing file
	case SYS_OPEN:
		printf("SYSTEM CALL: Open is being executed \n");
		verify_validity(pointer+1);
		const char *file_open= ((char*) *((int*)pointer + 1));
	
		f->eax = open(file_open);
	break;
	
	//Gets the size (in bytes) of a open file
	case SYS_FILESIZE:
		printf("SYSTEM CALL: File size is being executed \n");
		verify_validity(pointer+1);

		int fd_filesize = *((int*)pointer + 1);

		f->eax = filesize(fd_filesize);
	break;

	//Reads a number of bytes from a open file into the buffer
	//based on the size provided, or if fd = 0 reads from keyborad input 
	case SYS_READ:
		printf("SYSTEM CALL: Read is being executed \n");
		verify_validity(pointer+1);
		verify_validity(pointer+2);
		verify_validity(pointer+3);

		int fd_read = *((int*)pointer + 1);
		void* buffer_read = (void*)(*((int*)pointer + 2));
		unsigned size_read = *((unsigned*)pointer + 3);
	
		f->eax = read(fd_read, buffer_read, size_read);
	break;

	//Writes to the terminal/console if fd = 0,
	//or writes to the to a open file
	case SYS_WRITE:
		printf("SYSTEM CALL: Write is being executed \n");
		verify_validity(pointer+1);
		verify_validity(pointer+2);
		verify_validity(pointer+3);

		int fd = *((int*)pointer + 1);
		void* buffer = (void*)(*((int*)pointer + 2));
		unsigned size = *((unsigned*)pointer + 3);
		
		f->eax = write(fd,buffer,size);
	break;

	//Changes the next byte to be written too or read from
	//in an open file to a position specified
	case SYS_SEEK:
		printf("SYSTEM CALL: Seek is being executed \n");
		verify_validity(pointer+1);
		verify_validity(pointer+2);
	
		int fd_seek = *((int*)pointer + 1);
		unsigned position = *((unsigned*)pointer + 2);

		seek(fd_seek,position);
	break;

	//Returns the position of the next bytes that will be written too
	//or read from in an open file
	case SYS_TELL:
		printf("SYSTEM CALL: Tell is being executed \n");
		verify_validity(pointer+1);

		int fd_tell = *((int*)pointer + 1);

		f->eax = tell(fd_tell);
	break;

	//Closes a currently open file descriptor specified
	case SYS_CLOSE:
		printf("SYSTEM CALL: Close is being executed \n");
		verify_validity(pointer+1);

		int fd_close = *((int*)pointer+ 1);
	
		close(fd_close);
	break;

	//if there are no system calls or an error has occured
	//the current thread is exited
	default:
		printf("Error! no system call was implemented");
		thread_exit();
	}

}

pid_t exec(const char *cmd_line){
int pid;
//checks that the command 
if (cmd_line == NULL){ 
    return -1;
}

//uses the process_execute function in process.c
//to execute the executeable and its arguments
//returning the new processes id (its pid)
pid = process_execute(cmd_line);

return pid;
}
/*
int wait (pid_t pid){

process_wait(pid);

return ;
}
*/

//Used by Syscall create to create a file in the Pintos file system
bool create(const char *file, unsigned initial_size){

//debugging
printf("File %s is present \n", file);
printf("File start size %d\n", initial_size);

//checks to see if the file name provide is valid
if(file==NULL){
printf("create fail \n");
return false;
}

//if so does the required locks to prevent other processes
//accessing the file whilst it's being made, and creates the file
//with an inital size of the variable "initial_size"
if (file){
lock_acquire (&lock_file);
filesys_create (file,initial_size);
lock_release (&lock_file);
printf("create works \n");
return true;
}

// if something else goes wrong, return false
else
return false;
}

//Used by Syscall remove to remove a file from the Pintos file system
bool remove(const char *file_remove){
//debugging
printf("File -%s- is present \n", file_remove);

//checks to see file name provided is valid
if(file_remove==NULL){
printf("remove fail \n");
	return false;
}

//if so, puts locks in place to prevent other process acessing the file
//whilst it's being removed, and removed that file
if(file_remove){
	lock_acquire (&lock_file);
	filesys_remove(file_remove);
	lock_release (&lock_file);
	printf("remove works \n");
	return true;
}

// if something else goes wrong, return false
else
	return false;
}

//Used by Syscall open to open a file
int open(const char *file_open){

int fd_open;
//debugging

printf("File -%s- is present \n", file_open);

//checks the filename provided is valid
if (file_open == NULL){
	return -1;
}
//if so, opens that file in pintos file system and stored its file descriptor (fd) into
//the file list, allowing for other syscalls to later access this open file
//based on the fd
else{

	lock_acquire (&lock_file);
	struct file * new_file = filesys_open(file_open);
	lock_release (&lock_file);

	//creates a place in the memory for the current file in use
	struct file_in_use * get_file_in_use = malloc(sizeof(struct file_in_use));
	
	//Used the file_in_use structure implemented in threads.h.
        //Provides that file with a fd (cannot be fd 0 or 1) for the open file
  	get_file_in_use->fd = ++thread_current()->current_fd +1;
 	get_file_in_use->fp = new_file;
 	
	//Then places it into the file_list for the current thread	
	list_push_back(&file_list, &(get_file_in_use->file_element));
	 
	//then returns the opened files fd 
	fd_open = get_file_in_use->fd;
     
	printf("fd = %d \n",fd_open);

     return fd_open;
    }
}

//Used by Syscall filesize to determine the size (in bytes) of the provided file
int filesize(int fd_filesize){
int size_of_file;

	//gets the file that is needed based on the fd provided in fd_filesize
	struct file_in_use * file_filesize= get_file(fd_filesize);
	
	//puts the appropriate lock in place,
	//and gets the size, in bytes, of the requested file
	lock_acquire (&lock_file);
	size_of_file = file_length (file_filesize->fp);
	lock_release (&lock_file);

	printf("size of file = %d \n",size_of_file);
 return size_of_file; 
}

//Used by Syscall read to read from an open file or from keyborad input
//based on the fd
//Not working 
int read(int fd_read, void *buffer_read, unsigned size_read){

int bytes_read;

 //reads from keyborad input
if(fd_read == 0){
	buffer_read=input_getc();
	bytes_read = size_read;
 return bytes_read;
 }
else{
	//Puts the appropriate locks in place, the reads a number of bytes
	//based on "size_read", from the provided file ( based on the provided fd
	//the placing that in the buffer, returning the number of bytes read
	lock_acquire (&lock_file);
	struct file_in_use * file_being_read= get_file(fd_read);
	bytes_read = file_read(file_being_read->fp,buffer_read,size_read);
	lock_release (&lock_file);
	printf("number of bytes read = %d \n", bytes_read);
	return bytes_read;
 }
}

//Used by Syscall write to write to an open file, using data stored in the buffer
//writting a number of bytes based on "size" or writes to the console if fd = 1 
int write(int fd, const void *buffer, unsigned size){
int bytes_written;
// writes to the console returning the number of bytes written
if (fd ==1){
putbuf(buffer,size);
printf("\n");
bytes_written = size;
return bytes_written;
}
else{
// writes to the given file
lock_acquire (&lock_file);
// getting the fd of specified file
struct file_in_use * file_being_written= get_file(fd);

//writes to file and gets the number of bytes written, and returns the number of bytes written
bytes_written = file_write(file_being_written->fp,buffer,size);
lock_release (&lock_file);
printf("number of bytes written = %d \n", bytes_written);
return bytes_written;
}
}

//Used by Syscall seek to change the next byte to be written to
//read from in a specified open file, to the position provided 
void seek(int fd, unsigned position){
struct file_in_use * file_being_seeked= get_file(fd);
printf("position = %d \n", position);
lock_acquire (&lock_file);
file_seek(file_being_seeked->fp,position);
lock_release (&lock_file);
}

//Used by Syscall tell which returns that next bytes
//that will be written to or read from in the specified open file
unsigned tell(int fd){
int position_of_file;
struct file_in_use * file_being_told= get_file(fd);
if(file_being_told){
lock_acquire (&lock_file);
position_of_file=file_tell(file_being_told->fp);
lock_release (&lock_file);
printf("position = %d \n", position_of_file);
return position_of_file;
}
else{
return -1;
}
}

//Used by Syscall close, to close the open file in the current thread
//and deallocating the assigned memory used to store it.
void close (int fd){
struct file_in_use * file_being_closed= get_file(fd);
lock_acquire (&lock_file);
file_close(file_being_closed->fp);
lock_release (&lock_file);
//removed the current file from the file list and frees up
//the memory allocated to it in syscall open
list_remove(&file_being_closed->file_element);
free(file_being_closed);

}

