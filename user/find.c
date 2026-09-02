#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char* basename(char* path)
{
    char* p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    // Return blank-padded name.
    return p;
}

void find(char* path, char* name)
{
  char buf[512], *p;
  int fd;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(!strcmp(basename(path),name)){
      printf("%s\n",path);
    }
    break;
  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      fprintf(2, "find: path is too long\n");
      close(fd);
      return;
    }
    strcpy(buf, path);  // 将路径名移动至buf内,buf将记录目录下文件的路径
    p = buf + strlen(buf);
    *p++ = '/'; //p指向路径最后的文件名
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0) {
        continue;  // 为什么inum为0要continue
      }
      if(!strcmp(de.name,".") || !strcmp(de.name,"..")){
        continue;
      }
      memmove(p, de.name, DIRSIZ);//拼接文件路径
      p[DIRSIZ] = 0;
      find(buf,name);
    }
    break;
  }
  close(fd);
}

int main(int argc, char* argv[])
{
  if(argc != 3){
    fprintf(2,"usage: find <path> <name>\n");
    exit(1);
  }
  find(argv[1],argv[2]);
  exit(0);
}