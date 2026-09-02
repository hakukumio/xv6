---
title: XV6 lab0
---

## 0-开始前的准备

xv6是MIT的经典项目
它的灵魂直接继承自操作系统史的传奇——Unix 第六版 (Version 6, 简称 V6).但由于V6所运行在的机器太老了，可以把看作是用现代C语言为现代CPU重新书写的“V6 精神续作”。
这里是我一些环境配置过程中踩到坑，方便各位

### 0.1-xv6实现的与未实现功能

内核级别实现的

- 进程
- 虚拟地址空间
- UNIX LIKE文件与目录层次结构
- 管道
- 分时复用
- 21个系统调用

内核级别未实现的

- 用户ID与登录
- 文件的权限保护
- 网络栈，套接字(socket)
- 可分页到disk(磁盘)的虚拟内存
- 支持更多的设备驱动
- 还有很多

### 0.2-xv6怎么启动

执行下面这条命令

```shell
make qemu
```

### 0.3-xv6怎么退出

在成功运行后，ctrl+a，然后按x即可退出xv6

### 0.4-如何编写用户级别应用

首先在user目录下写好文件
接下来就是进入xv6项目的makefile进行修改
详细请看 [xv6-makefile导读](xv6-makefile导读.md)

### 0.5-不知道如何使用操作系统接口

如果你正在使用Linux这样的操作系统，那么你可以通过其内置的man命令来查询各个系统接口的使用方式作为参考。
如chdir,就可以通过输入

```shell
man 2 chdir
```

### 0.4-评分测试

在写好代码后，就要运行测试了，其中在我自己的LINUX环境下，Python的版本实际上是python3，所以需要更改xv6项目的评分脚本`grade-lab-util`中的

```py
#!/usr/bin/env python
```

变为

```py
#!/usr/bin/env python3
```

如果一次性执行所有评分测试

```shell
make grade
```

如果要对单独一个进行测试，如sleep。输入

```shell
make GRADEFLAGS=sleep grade
```

## 1-Sleep

sleep的实现非常简单，我们按照xv6的手册调用用户态的sleep即可

```c
#include "kernel/types.h"
#include "user.h"

void
sh_sleep(int n){
    sleep(n);
}

int
main(int argc,char *argv[]){
    if(argc <= 1){
        fprintf(2,"sleep: no arguments\n");
        exit(1);
    } 
    int n;
    for(int i=1;i < argc;i++){
        n = atoi(argv[i]);
        sh_sleep(n);
    }
    exit(0);
}
```

## 2-pingpong

pingpong的核心就是使用管道，在夫进程与子进程之间传递数据
代码如下

```c
#include "kernel/types.h"
#include "user/user.h"

void main(int argc, char* argv[])
{
    int p0[2];
    int p1[2];
    if (pipe(p0) == -1) {
        fprintf(2, "pipe error");
        exit(1);
    }
    if (pipe(p1) == -1) {
        fprintf(2, "pipe error");
        exit(1);
    }
    if (fork() == 0) {
        char c;
        int fd0 = dup(p0[0]);
        close(p0[0]);
        close(p0[1]);
        read(fd0, &c, 1);//等待父进程对于p0的写入，然后读取

        close(fd0);
        fprintf(1, "%d: received ping\n", getpid());//通过p1向父进程传输数据
        write(p1[1], &c, 1);
        close(p1[1]);
        close(p1[0]);
        exit(0);
    }
    else {
        char c;
        int fd;
        write(p0[1], "c", 1);//通过p0向子进程传输数据
        close(p0[1]);
        close(p0[0]);
        fd = dup(p1[0]);
        close(p1[0]);
        close(p1[1]);
        read(fd, &c, 1);
        if (c == 'c') { //通过p1接收来自子进程的数据，并校验
            fprintf(1, "%d:received pong\n", getpid());
        }
        exit(0);
    }
}
```

## 3-primes

答案如下。这里考查的关键就是关于管道，fd，进程的理解。这里极其关键的一个点在于xv6对于一个进程支持其打开的最大文件数量为16，所以在创建子进程的过程中及时关闭fd是很重要的。

```c
#include "kernel/types.h"
#include "user/user.h"

void stage(int fd)
{
    int x;//当前流水线所要筛的数
    int y;//流水线从管道中读取到的数
    int p[2];
    int n;//记录read返回值
    n =read(fd, &x, 4);
    if(n == 0){//读取第一个数时，遇到EOF直接退出
        exit(0);
    }
    else if(n < 4){ //未读取到完整的int
        fprintf(1,"read error");
        exit(1);
    }
    fprintf(1,"prime %d\n", x);
    pipe(p);
    if (fork() == 0) {//开启子进程
        close(fd);
        close(p[1]);
        stage(p[0]);
        close(p[0]);
        exit(0);
    }
    else {//父进程继续从管道中读取
        close(p[0]);
        while((n = read(fd,&y,4)) == 4){
            if((y%x) != 0){//筛选
                write(p[1],&y,4);
            }
        }
        close(p[1]);
        close(fd);
        if(n != 0 && n < 4){//读取遇到问题
            fprintf(2,"write error\n");
            n = 1;
        }
        wait(0);
        exit(n);
    }
}

void main(int argc, char* argv[])
{
    int p[2];
    pipe(p);
    for(int i=2;i<36;i++){
        write(p[1],&i,4);
    }
    if(fork() == 0){
        close(p[1]);
        stage(p[0]);
        close(p[0]);
        exit(0);
    }
    else {
        close(p[0]);
        close(p[1]);
        wait(0);
        exit(0);
    }
}
```

如果完成了这个实验，建议读读pipe的源码
详细可以读这个文章 [从0开始读源码-第0节-pipe](scratch0.md)

## 4-find

开始find之前，需要了解文件系统的核心概念与源代码
看文件系统的源码

### 4.1-文件数据结构

|层面|结构体|定义位置|是什么|
|----|----|----|----|
|磁盘上|struct dinode|kernel/fs.h|每个文件的身份证,记录其类型，大小，数据块位置等等|
|内核内存|struct inode|kernel/file.h|dinode的缓存版本,并且带引用计数/锁|
|用户程序|struct stat|kernel/stat.h|内核通过stat()/fstat()给用户看的文件状态信息|
|目录内容|struct dirent|kernel/fs.h|目录文件的一行，目录文件的内容就是这样的一个数组|

在这样的数据结构下带来一些点

- 文件名长度限制在14字节以内
-  

### 4.2-读取目录下的文件

读取目录下文件的步骤如下

- 1.获取目录文件的路径path
- 2.读取目录文件内的dirent条目。每读取一个，将dirent.name与path进行拼接，获得目录下文件的路径
- 3.获取到文件的路径，然后就可以获取其状态，打开等等
- 4.若目录文件的dirent没有读完,回到继续执行

### 4.3-程序的构造

所以很显然地我们能够开始想怎么去写代码了
我们定义一个find，接口为

```c
//查找path下所有与name匹配的文件并输出
void find(char *path,char *name);
```

而find内部，参考ls的实现，则可以这样去递归实现

```c
void find(char *path,char *name){
    char buf[512];
    int fd = open(path,0);
    struct stat st = fstat(fd,&st);
    switch(st.type){
    case T_FILE:
        if(strcmp(basename(path),name)){
            printf(%s\n);
        }
        break;
    case T_DIR:
        //拼接路径名到buf的代码(省略)
        //然后读取DIR文件，遍历里面的每个文件，执行find

    }
    break;
}
```

### 4.4-源码

```c
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
```

## 5-xargs

这个比较简单，就是从标准输入内实现读取一行的功能，然后每读取一行，进行参数拼接，fork，exec相应的程序即可。
源码如下

```c
#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

char** addArgv(int argc, char* argv[], char* arg)
{
    int i =0;
    static char* na[MAXARG] = {0};
    for (i = 0; i < MAXARG; i++) {
        na[i] = 0;
    }
    for (i = 0; i < argc; i++) {
        na[i] = argv[i];
    }
    na[argc] = arg;
    return na;
}

void execArgs(char* path, char* argv[])
{
    #ifdef DEBUG
    fprintf(2,"DEBUG: path=%s argv[0]=%s argv[1] = %s argv[2]=%s\n",
             path,argv[0],argv[1],argv[2]);
    #endif

    if (fork() == 0) {
        if (exec(path, argv) == -1) {
            fprintf(1, "xargs: exec %s failed\n", path);
            exit(1);
        }
    }
    else {
        wait(0);
    }
}

int readline(char* buf, int max)
{
    int i = 0, n = -1;

    for (i = 0; (i + 1) < max; i++) {
        n = read(0, buf + i, 1);
        if (n <= 0 || buf[i] == '\n') {
            break;
        }
    }
    buf[i] = '\0';
    if (i > 0) {
        return i;
    }
    // fprintf(2,"xargs: raed error with %d\n",n);
    return -1;
}

int main(int argc, char* argv[])
{
    if(argc < 2){
        fprintf(2,"xargs: too few arguments");
        exit(0);
    }
    char buf[512];
    while (readline(buf, sizeof(buf)) > 0) {
        execArgs(argv[1], addArgv(argc - 1, &argv[1], buf));
    }
    exit(0);
}
```