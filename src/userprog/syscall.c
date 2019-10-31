#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "lib/user/syscall.h"
#include "devices/input.h"
#include "lib/kernel/stdio.h"

static void syscall_handler (struct intr_frame *);

void halt(void){
  shutdown_power_off();
}

void exit (int status) {
  thread_exit ();
}

int write (int fd, const void *buffer, unsigned size){
  if (fd == 1) {
    putbuf(buffer, size);
    return size;
  }
  return -1;
}

pid_t exec (const char *cmd_line) {
  return process_execute(cmd_line);
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  
  int num_system_call = *(int *)(f->esp);
  printf("debug num_system_call : %d\n", num_system_call);

  switch (num_system_call)
  {
  case SYS_HALT:
    halt();
    break;
  case SYS_EXIT:
    exit(*(int *)(f->esp+4));
    break;
  case SYS_EXEC:
    exec(*(char **)(f->esp+4));
    break;
  case SYS_WAIT:
    break;
  case SYS_CREATE:
    break;
  case SYS_REMOVE:
    break;
  case SYS_OPEN:
    break;
  case SYS_FILESIZE:
    break;
  case SYS_READ:
    break;
  case SYS_WRITE:
    f->eax = write(*(int *)(f->esp+4), *(void **)(f->esp+8), *(unsigned int *)(f->esp+12));
    break;
  case SYS_SEEK:
    break;
  case SYS_TELL:
    break;
  case SYS_CLOSE:
    break;  
  default:
    break;
  }
  
  thread_exit ();
}
