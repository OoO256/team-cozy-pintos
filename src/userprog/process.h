#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
extern int process_wait (tid_t child_tid);
void process_exit (void);
void process_activate (void);


struct thread* get_child_process (struct thread* p, int pid);
void remove_child_process(struct thread* cp);
#endif /* userprog/process.h */