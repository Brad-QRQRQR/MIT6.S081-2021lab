#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *tar) {
  int fd;
  struct dirent de;
  struct stat st;
  char buf[512];
  char *p;

  // check if file cane be opened
  if ((fd = open(path, O_RDONLY)) < 0) {
    // fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  // check if file info can be get
  if (fstat(fd, &st) < 0) {
    // fprintf(2, "find: cannot stat %s\n", path);
    return;
  }

  // check if the argument is passed correctly
  if (st.type != T_DIR) {
    // fprintf(2, "find: argv[1] must be a path, not a file name\n");
    return;
  }

  // prevent buffer overflow
  if (strlen(path) + 1 + DIRSIZ > sizeof(buf)) {
    // fprintf(2, "find: path too long\n");
    return;
  }

  // buf is used to store the path to the file being read
  strcpy(buf, path);
  p = buf + strlen(path);
  *p++ = '/';
  
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0) continue;
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
    
    // change the path to the file being read
    memmove(p, de.name, DIRSIZ);

    if (stat(buf, &st) < 0) {
      // fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    switch(st.type) {
    case T_FILE:
      if (strcmp(de.name, tar) == 0) {
        printf("%s\n", buf);
      }
      break;
    // recurse into sub directories
    case T_DIR:
      find(buf, tar);
      break;
    }
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(2, "Usage: find directory relative_path_to_file\n");
    exit(0);
  }

  find(argv[1], argv[2]);
  exit(0);
}