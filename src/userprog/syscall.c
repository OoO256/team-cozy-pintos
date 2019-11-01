#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "lib/user/syscall.h"
#include "devices/input.h"
#include "lib/kernel/stdio.h"
#include <string.h>
static void syscall_handler (struct intr_frame *);

void halt(void){
  shutdown_power_off();
}

void exit (int status) {
#ifdef USERPROG
  struct thread *cur = thread_current ();
  printf ("%s: exit(%d)\n", cur->name, status);
#endif
  thread_exit ();
}

int write (int fd, const void *buffer, unsigned size){
  printf("[debug] write : fd %d buf %p size %d\n", fd, buffer, (int)size);
  char *bufPtr = buffer;
  printf("[debug] write : contents of buf -> ");
  int i;  
  for(i = 0; i < 100; i++){
    printf("%c ", bufPtr[i]);
  }
  printf("\n");
  if (fd == 1) {
    putbuf(buffer, size);
    return size;
  }
  return -1;
}

pid_t exec (const char *cmd_line) {
  return process_execute(cmd_line);
}

int read(int fd, void*buffer, unsigned size){
	int i = -1;
	if(fd == 0){
		char*bufPtr = buffer;
		for(i = 0; i < size && bufPtr[i] != 0; i++);
	}
	return i;
}

int wait(pid_t pid){
	return -1;
}


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

int cache[1000];
int fibonacci(int n){
  printf("[debug] fibonacci n : %d\n", n);

  
  int  i;
  cache[0] = 0;
  cache[1] = 1;
  for(i = 2; i <= n; i++){
    cache[n] = cache[n-1] + cache[n-2];
  }
  return cache[n];
}

int sum(int arg0, int arg1, int arg2, int arg3){
  printf("[debug] sum argus : %d %d %d %d\n", arg0, arg1, arg2, arg3);
  int SUM = arg0 + arg1 + arg2 + arg3;
  return SUM;
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf ("system call!\n");
  
  int num_system_call = *(int *)(f->esp);
  printf("[debug] in syscall handler\n");
  printf("debug num_system_call : %d\n", num_system_call);
  printf("esp : %p\n", f->esp);
  uint8_t* espPtr = f->esp;
  int i;
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
  
    f->eax = read(*(int *)(f->esp+4), *(void **)(f->esp+8), *(unsigned int *)(f->esp+12));
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
  case SYS_FIBONACCI:
    f->eax = fibonacci(*(int *)(f->esp+4));
    break;
  case SYS_SUM:
    f->eax = sum(*(int*)(f->esp+4), *(int*)(f->esp+8), *(int*)(f->esp+12), *(int*)(f->esp+16));
    break;
  default:
    break;
  }
  
  thread_exit ();
}
