#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "lib/user/syscall.h"

void syscall_init (void);
int wait(pid_t pid);
void close (int fd);

#endif /* userprog/syscall.h */
