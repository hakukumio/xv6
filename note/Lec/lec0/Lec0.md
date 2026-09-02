# 操作系统接口

## 操作系统要干什么

操作系统是一道在硬件与我们熟悉的软件之间的中间层。
核心任务就是抽象硬件，利于软件编写
而操作系统向用户提供的服务就是接口，而我们接下来就是要讲接口
如果变成八股文，就是

- 操作系统的主要功能就是资源管理(resource manager)，抽象硬件,多路复用

## 0-接口

接口是操作系统向用户提供的核心服务。他让软件编写可以使用操作系统提供的服务来获得一些完美的抽象。如文件

## 1-进程与内存

### 1.1-进程的概念

即用户态内存(指令，数据和栈)与状态(运行，停止，结束)。

### 1.2-PID

操作系统用于标识进程的ID

### 1.3-一些例子

**fork()** fork会创建一个子进程。其中对父进程返回子进程的pid，对于子进程则返回0

**exit()** exit会让调用它的进程停止运行，并释放其用户态资源(如已打开文件，内存)

**wait()** 调用wait的父进程会进入等待以回收子进程。一次调用，回收一个。

```c
int pid;
pid = fork();
if(pid > 0){
    printf("I'm parent\n");
    wait();
    printf("My child's PID is %d\n",pid);
}
else if(pid == 0){
    printf("I'm child,my see fork() = %d\n",pid);
    exit();
}
else{
    printf("Error\n");
}
```

## 2-I/O

### 2.1-fd(file descriptionn)

文件描述符是一个整数，即进程能够读取的一个被内核管理的对象。
而每个进程都有这样的一张索引表
其中说些特殊的

|fd|功能|
|----|----|
|0|标准输入|
|1|标准输出|
|2|标准错误|

### 2.2-open

open是一个关键到的系统接口。他和核心在于打开文件，并为其分配最小到的空闲fd。

看看 **I/O重定向**的经典实现上
如一个`progname < filename`.

```c
int pid = fork();
if(pid == 0){
    close(0);
    open(filename,O_RDONLY);
    exec(progname,argv);
}
```

### 2.3-dup

dup的功能就是 “复制”。他也和open一样，返回可用的最小fd

看下面这个简短的例子.作为标准输出的0在被关闭之前，使用dup“复制”了一份
接下来使用的fd就代表着0所代表的那个对象

```c
void 
main(int argc,char *argv[]){
    if(write(0,"hello world\n",12) != 12){
        fprintf(2,"write ERROR");
    }
    int fd = dup(0);
    close(0);
    if(write(fd,"hello world\n",12) != 12){
        fprintf(2,"write ERROR");
    }
    exit(0);
}
```

## 3-管道(pipe)

管道就是为了进程之间传话。

### 3.1-pipe

xv6提供了pipe这个系统调用。他的作用就是找两个最小的可用fd，让其指向内核的一小块缓冲区。
看下这个代码例子,使用用户级别的wc(word count)来演示pipe的功能
pipe的核心在于内核会维护一个计数器以保障当没有人能够向缓冲区写入时才开启读，否则读操作会被阻塞。
这也是为什么我们在子进程与夫进程之间需要`close(p[1])`的原因

而`close(p[0])`的原因为“清理”

```c
#include "kernel/types.h"
#include "user/user.h"

void 
main(int argc,char *arg[]){
    int p[2];
    char *argv[2];
    argv[0] = "wc";
    argv[1] = 0;
    pipe(p);
    if(fork() == 0) {
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        exec("wc", argv);
    } else {
        write(p[1], "hello world\n", 12);
        close(p[0]);
        close(p[1]);
        wait(0);
        exit(0);
    }
}
```

其中管道与临时文件的区别为

- 管道会进行自我清扫，如果是 shell 重定向的话，我们必须要在任务完成后删除 /tmp/xyz
- 管道可以传输任意长度的数据
- 管道允许同步：两个进程可以使用一对管道来进行二者之间的信息传递，每一个读操作都阻塞调用进程，直到另一个进程用 write 完成数据的发送。

## 4-file System(文件系统)

文件与目录。经典的概念。其中xv6的目录也是文件(UNIX LIKE).所以UNIX系统的树状文件结构等等xv6也是有的

### 4.1-目录

在xv6中，目录是一种特殊的文件。
使用`mkdir`，将会创建目录
使用`chdir`，能够变更当前地工作目录

### 4.2-文件名

文件数据存放在磁盘地物理地址上，而文件名将指向其在磁盘上所在地位置。

**try it**在Windows上，假设D盘内已经有一个名为Hello.txt的文件，在CMD内输入`mklink /H world.txt hello.txt`,此时`hello.txt`,`world.txt`指向的是同一个文件数据。此时打开`hello.txt`,修改文件数据，保存。在`world.txt`内能够看到同步的修改

那么文件的磁盘空间要被释放，指向它的文件名数需要为0。