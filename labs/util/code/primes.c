#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXINT 35

__attribute__((noreturn))
void seive() {
  int prime;
  int pi[2];
  int num;
  int flag;
  if (read(0, &prime, 4) != 4) {
    fprintf(2, "Pid %d: failed to read numbers\n", getpid());
    exit(1);
  }

  printf("prime %d\n", prime);

  flag = (read(0, &num, 4) != 4);
  if (flag) {
    exit(0);
  }

  // another pipes for right neighbor
  pipe(pi);

  if (num % prime != 0) {
    write(pi[1], &num, 4);
  } 

  if (fork() == 0) {
    // redirect I/O
    close(0);
    dup(pi[0]);
    close(pi[0]);
    close(pi[1]);
    
    seive();
  }else {
    // close unnecessary fd
    close(pi[0]);
    
    // read from left neighbors
    int num;
    while(read(0, &num, 4) == 4) {
      if (num % prime == 0) continue;
      write(pi[1], &num, 4);
    }
    close(pi[1]);
    
    // wait and exit
    wait(0);
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    fprintf(2, "Usage: primes\n");
    exit(0);
  }

  // pipes for feeding numbers
  int pi[2];
  pipe(pi);

  if (fork() == 0) {
    // redirect I/O
    close(0);
    close(1);
    dup(pi[0]);
    close(pi[0]);
    close(pi[1]);

    seive();
  }else {
    // close unnecessary fd
    close(0);
    close(1);
    close(pi[0]);

    // feed numbers (2~35)
    for (int i = 2; i <= MAXINT; i++) {
      write(pi[1], &i, 4);
    }
    close(pi[1]);
    
    // wait and exit
    wait(0);
    exit(0);
  }
}