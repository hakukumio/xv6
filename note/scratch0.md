# 从0开始读源码-第0节-pipe

## 0-依赖关系的查找

在用户层面，我们使用`pipe()`来完成关于管道的一些操作。那么我们就要了解其是如何运作的，其依赖的底层源码是什么。
在user目录下我们通过包含`"include\user.h"`,然后填写makefile内的UPGROS就可以完成对于系统接口的调用。
那么，我们如何像是一个侦探一样，一步步地从0开始读懂xv6地源码并训练出一种对于陌生项目也能去懂得自己要找什么文件看什么代码的能力

### 0.1-看项目结构

查看一个项目的项目结构有助于我们知道项目的功能组成，以及如何去找到自己可能需要的文件。
在xv6当中，我们的项目源码基本就是

- kernel(内核)
- user(用户程序)
- Makefile

### 0.2-善用查找

当我们获知到一个符号的时候，如果不好找线索，可以在项目内搜索这个符号，然后去比对查看。这样往往能够高效地获知接口地实现是什么

### 0.3-保持依赖链

现代软件的强大核心在于复杂度控制。通过层层加码的封装来实现。所以读源码了解原理需要保持依赖关系。

### 0.4-查找pipe相关的文件

对于我们的pipe，我们的步骤就是

- 找到user/user.h,看到了用户态使用的接口pipe的声明
- 在user/内找到user/usys.pl这个脚本文件，根据注释说明找到user内与系统嗲用有关的user/usys.S这个汇编文件
- 在user/usys.S这个汇编文件内找到pipe相关的汇编指令，看到其实际上是调用一个名为`SYS_pipe`的接口
- 在项目内查找`SYS_pipe`，看到其在kernel/syscall.c,kernel/syscall.h中有定义。看到其指向`sys_pipe`
- 查找sys_pipe，看到其在kernel/sysfile.c内定义累实现。而此时此刻我们才算真正地开始深入了解pipe的底层原理
- 此时sys_pipe也依赖于其他的一些数据结构，函数。由于我们是了解pipe的底层原理，所以很多东西只看接口，功能就足够了。而重点关注在其内调用的`pipealloc`函数上
- 通过`pipealloc`函数，我们找到`kernel/pipe.c`，看到了pipe的数据结构,其被创建，关闭，读取，写入的实现

## 1-pipe依赖的数据结构

pipe的数据结构位于`kernel/pipe.c`中

```c
struct pipe {
  struct spinlock lock;
  char data[PIPESIZE];
  uint nread;     // number of bytes read
  uint nwrite;    // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
};
```

其中，管道的初始化，读写端口的新增减少，对于管道内内容的读取写入会同步修改这个数据结构内的内容。

## 2-初始化

我们调用`int pipe(int* )`，在`kernel/sysfile.c`记录了其具体实现

- 1.调用pipealloc创建管道，并得到作为读/写端口的文件
- 2.调用fdallo，给这两个文件获取到对应的fd。此时fd就可以作为pipe的读写端口使用
- 3.将得到的fd放到用户提供的数组中

所以我们看pipealloc的实现
在pipealloc内，则是做这样的事情

- 1.调用filealloc申请得到两个文件
- 2.调用kalloc，在内存中创建pipe这一数据结构
- 3.初始化pipe这一数据结构
- 4.初始化两个文件，让一个只读，一个只写
- 返回这两个文件

## 3-关闭

管道的关闭在`kernel/pipe.c`内的`pipeclose`内
在我们调用`close(fd)`的时候，首先是跟踪fd与文件之间的关系，如果没有fd指向该文件，那么调用`fileclose`.而`fileclose`会其看文件类型，如果是pipe，然后调用`pipeclose`
这里的核心是pipeclose的逻辑

- 1.关闭的如果是读端口,唤醒相应进程。如果是写端口，唤醒相应进程
- 2.如果管道此时即无读端口也无写端口，那么释放管道数据结构锁占用的内存

## 4-读取

我们调用`read`的时候，其会执行我们的`piperead`
piperead的源码如下

```c
int
piperead(struct pipe *pi, uint64 addr, int n)
{
  int i;
  struct proc *pr = myproc();
  char ch;

  acquire(&pi->lock);
  while(pi->nread == pi->nwrite && pi->writeopen){  //DOC: pipe-empty
    if(pr->killed){
      release(&pi->lock);
      return -1;
    }
    sleep(&pi->nread, &pi->lock); //DOC: piperead-sleep
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(pi->nread == pi->nwrite)
      break;
    ch = pi->data[pi->nread++ % PIPESIZE];
    if(copyout(pr->pagetable, addr + i, &ch, 1) == -1)
      break;
  }
  wakeup(&pi->nwrite);  //DOC: piperead-wakeup
  release(&pi->lock);
  return i;
}
```

这里解析下

- 1.如果缓冲区为空，并且管道还有写端口，则会阻塞等待。让写者能够写内容进来
- 2.上述条件不满足时，进入循环，读取缓冲区。程序会一个一个字节进行读取，写入到用户提供的内存地址上。其中如果写入失败或者中途遇到缓冲区为空(nread==nwrite)的情况时,中断
- 3.读取结束，唤醒那些因为缓冲区满而阻塞的write
- 4.返回读取到的字节数

## 5-写入

写入操作也是一个一个字节德写入缓冲区。
每写一个字节检查缓冲区是否已满，满了就阻塞等待。
如果管道不可读了或者进程被杀，返回-1
如果写入字节德过程中遇到问题，则中断写入
