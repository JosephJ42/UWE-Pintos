#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

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
  
  int system_call_number = *((int*)f->esp); //
  printf("system_call_handler() - %d! \n", system_call_number);

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
 	
	thread_exit();
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

