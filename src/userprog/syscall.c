#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/user/syscall.h"
#include "devices/input.h"
#include "lib/kernel/stdio.h"
#include <string.h>
#include "userprog/process.h"

#include "filesys/filesys.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *);

void check_user_vaddr(const void *vaddr) {
  if (!is_user_vaddr(vaddr)) {
    exit(-1);
  }
}

void halt(void){
  shutdown_power_off();
}

void exit (int status) {
  struct thread *cur = thread_current();
  cur->exit_status = status;
  printf("%s: exit(%d)\n", cur -> name, cur->exit_status);
  thread_exit();
}

int write (int fd, void *buffer, unsigned size){
  check_user_vaddr(buffer);
  if (buffer == NULL)
    exit(-1);
  
  if(fd == 0)
  {
    exit(-1);
  }
  else if (fd == 1)
  {
    putbuf(buffer, size);
    return size;
  }
  else
  {
    struct file* f = thread_find_file(fd);

    if (f == NULL)
      exit(-1);

    // LOCK
    int write_cnt = file_write(f, buffer, size);

    return write_cnt;   
  }
  return -1;
}

pid_t exec (const char *cmd_line) {
  pid_t pid_child = process_execute(cmd_line);

  struct thread *child = get_child_process(thread_current(), pid_child);
  sema_down(&(child->sema_load));
  
  if (child->is_loaded == false)
    return -1;
  else
    return pid_child;
}

int read(int fd, void*buffer, unsigned size){
  check_user_vaddr(buffer);
  if (buffer == NULL)
    exit(-1);
	
	if(fd == 0){
    char *char_buf = buffer;
    int i;
		for(i = 0; i < size && char_buf[i] != 0; i++)
      char_buf[i] = input_getc();
    return i;
	}
  else if (fd == 1){
    exit(-1);
  }
  else
  {
    struct file *f = thread_find_file(fd);
    int read_cnt = file_read(f, buffer, size);

    return read_cnt;
  }
  
	return -1;
}

int wait(pid_t pid){
  int ret = process_wait(pid);
	return ret;
}

bool create(const char *file, unsigned int initial_size){
  check_user_vaddr(file);
  if (file == NULL)
    exit(-1);
  
  bool success;
  // TODO : lock_acquire (&filesys_lock);
  success = filesys_create (file, initial_size);
  // lock_release (&filesys_lock);

  return success;
}

bool remove(const char *file){
  check_user_vaddr(file);
  if (file == NULL)
    exit(-1);

  bool success;
  success = filesys_remove (file);
  return success;
}

int open(const char *file){
  check_user_vaddr(file);
  if (file == NULL)
    exit(-1);

  struct file *f;
  f = filesys_open (file);

  if (f == NULL)
    return -1;

  return thread_open_file(f);
}

int filesize (int fd){
  struct file *f = thread_find_file(fd);

  if (f == NULL){
    return -1;
  }
  else
  {
    return file_length(f);
  }  
}

void seek (int fd, unsigned int pos){
  struct file *f = thread_find_file(fd);
  if(f != NULL){
    file_seek(f, pos);
  } 
}

unsigned int tell (int fd){
  struct file *f = thread_find_file(fd);
  return file_tell(f);
}

void close (int fd) {
  struct file *f = thread_find_file(fd);
  if (f == NULL)
    return -1;
  
  thread_close_file(fd);

  // TODO : LOCK
  file_close(f);
}



void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

int fibonacci(int n){
  //printf("[debug] fibonacci n : %d\n", n);

  int a = 1, b = 1, c = 0; // an, an-1, an-2;
  if (n == 0)
    return c;

  if (n == 1)
    return b;

  int temp;
  n -= 2;
  while (n--)
  {
    temp = a + b;
    c = b;
    b = a;
    a = temp;
  }
  return a;
}

int sum_of_four_int(int arg0, int arg1, int arg2, int arg3){
  //printf("[debug] sum argus : %d %d %d %d\n", arg0, arg1, arg2, arg3);
  int SUM = arg0 + arg1 + arg2 + arg3;
  return SUM;
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf ("system call!\n");
  
  int num_system_call = *(int *)(f->esp);
  //printf("[debug] in syscall handler\n");
  //printf("debug num_system_call : %d\n", num_system_call);
  uint8_t* espPtr = f->esp;
  int i;
  switch (num_system_call)
  {
  case SYS_HALT:
    halt();
    break;
  case SYS_EXIT:
    check_user_vaddr(f->esp+4);
    exit(*(int *)(f->esp+4));
    break;
  case SYS_EXEC:
    check_user_vaddr(f->esp+4);
    f->eax = exec(*(char **)(f->esp+4));
    break;
  case SYS_WAIT:
    check_user_vaddr(f->esp+4);
    f->eax = wait(*(int *)(f->esp+4));
    break;
  case SYS_CREATE:
    check_user_vaddr(f->esp+4);    
    check_user_vaddr(f->esp+8);
    f->eax = create(*(const char **)(f->esp+4), *(unsigned int *)(f->esp+8));
    break;
  case SYS_REMOVE:
    check_user_vaddr(f->esp+4);
    f->eax = remove(*(const char **)(f->esp+4));
    break;
  case SYS_OPEN:
    check_user_vaddr(f->esp+4);
    f->eax = open(*(const char **)(f->esp+4));
    break;
  case SYS_FILESIZE:
    check_user_vaddr(f->esp+4);
    f->eax = filesize(*(int *)(f->esp+4));
    break;
  case SYS_READ:  
    check_user_vaddr(f->esp+4);
    check_user_vaddr(f->esp+8);
    check_user_vaddr(f->esp+12);
    f->eax = read(*(int *)(f->esp+4), *(void **)(f->esp+8), *(unsigned int *)(f->esp+12));
    break;
  case SYS_WRITE:
    check_user_vaddr(f->esp+4);
    check_user_vaddr(f->esp+8);
    check_user_vaddr(f->esp+12);
    f->eax = write(*(int *)(f->esp+4), *(void **)(f->esp+8), *(unsigned int *)(f->esp+12));
    break;
  case SYS_SEEK:
    check_user_vaddr(f->esp+4);
    check_user_vaddr(f->esp+8);
    seek(*(int *)(f->esp+4), *(unsigned int*)(f->esp+8));
    break;
  case SYS_TELL:
    check_user_vaddr(f->esp+4);
    f->eax = tell(*(unsigned int*)(f->esp+4));
    break;
  case SYS_CLOSE:
    check_user_vaddr(f->esp+4);
    close(*(unsigned int*)(f->esp+4));
    break;
  case SYS_FIBONACCI:
    f->eax = fibonacci(*(int *)(f->esp+4));
    break;
  case SYS_SUM:
    f->eax = sum_of_four_int(*(int*)(f->esp+4), *(int*)(f->esp+8), *(int*)(f->esp+12), *(int*)(f->esp+16));
    break;
  default:
    break;
  }
}
