#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void communicate(char *buf, int p[2]) {
  int read_cnt, temp;

  read_cnt = 0;
  while ((temp = read(p[0], buf, 1))) {
    read_cnt += temp;
  }

  if (read_cnt != 1) {
    fprintf(2, "%d: an read error occurs, where read count is %d", getpid(), read_cnt);
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  if (argc > 1) {
    fprintf(2, "Usage: pingpong\n");
    exit(0);
  }
  
  int p2c[2], c2p[2];
  char buf[2];
  int status;

  pipe(p2c);
  pipe(c2p);
  write(p2c[1], "p", 1);
  close(p2c[1]);

  if (fork() == 0) {
    communicate(buf, p2c);
    close(p2c[0]);
    write(c2p[1], "c", 1);
    fprintf(1, "%d: received ping\n", getpid());
  }else {
    wait(&status);
    if (status) {
      fprintf(2, "%d: an error in child process occurs\n", getpid());
      exit(1);
    }
    close(c2p[1]);
    communicate(buf, c2p);
    fprintf(1, "%d: received pong\n", getpid());
  }

  exit(0);
}
