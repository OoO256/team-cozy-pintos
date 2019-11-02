#include <stdio.h>
#include <syscall.h>

int main(int argc, char**argv){
  
  if(argc == 1){
    printf("fibo! %d\n", fibonacci(atoi(argv[1])));
  }
  else{
    printf("fibo! %d\n", fibonacci(atoi(argv[1])));
    printf("sum! %d\n", sum(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])));
  }
  return 0;
}