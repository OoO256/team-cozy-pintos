#include <stdio.h>
#include <syscall.h>

int main(int argc, char**argv){

    printf("%d ", fibonacci(atoi(argv[1])));
    printf("%d\n", sum_of_four_int(atoi(argv[1]),
	atoi(argv[2]), atoi(argv[3]), atoi(argv[4])));
  return 0;
}
