#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int readline(char *buf, int size) {
  int cnt = 0;
  while (cnt + 1 <= size && read(0, buf, 1) == 1) {
    if (*buf == '\n') {
      *buf = 0;
      break;
    }
    buf++; 
    cnt++;
  }
  if (cnt && *buf != 0) {
    fprintf(2, "xargs: argument length exceeding buffer size %d\n", size);
    exit(1);
  }
  return cnt;
}

void split(char *buf, int argc, char *argv[]) {
  char *p = buf;
  int cnt = 0;
  int offset = 0;

  while (*p) {
    while (*p && *p != ' ') {
      p++;
      cnt++;
    }
    
    if (argv[argc]) {
      free(argv[argc]);
      argv[argc] = 0;
    }

    argv[argc] = malloc(sizeof(char) * (cnt + 1));
    memmove(argv[argc], buf + offset, cnt);
    argv[argc][cnt] = 0;
    
    argc++;
    offset += cnt;
    if (*p) {
      offset++;
      p++;
    }
  
    if (argc >= MAXARG) {
      fprintf(2, "xargs: too many arguments\n");
      exit(1);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "Usage: xargs command...");
    exit(0);
  }

  char buf[1024];
  char *nargv[MAXARG];
  int nargc;

  nargc = argc - 1;
  for (int i = 1; i < argc; i++) {
    nargv[i - 1] = argv[i];
  }
  for (int i = nargc; i < MAXARG; i++) {
    nargv[i] = 0;
  }
  
  while (readline(buf, 1024)) {
    split(buf, nargc, nargv);
    if (fork() == 0) {
      exec(nargv[0], nargv);
    }else {
      wait(0);
    }
  }
  
  exit(0);
}
