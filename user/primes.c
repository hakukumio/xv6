#include "kernel/types.h"
#include "user/user.h"

void stage(int fd)
{
    int x;  // 当前流水线所要筛的数
    int y;  // 流水线从管道中读取到的数
    int p[2];
    int n;  // 记录read返回值
    n = read(fd, &x, 4);
    if (n == 0) {  // 读取第一个数时，遇到EOF直接退出
        exit(0);
    }
    else if (n < 4) {  // 未读取到完整的int
        fprintf(1, "read error\n");
        exit(1);
    }
    fprintf(1, "prime %d\n", x);
    pipe(p);
    int fs = fork();
    if (fs == 0) {  // 开启子进程
        close(fd);
        close(p[1]);
        stage(p[0]);
    }
    else if (fs == -1) {  // 子进程开启失败
        fprintf(2, "fork error\n");
        exit(1);
    }
    else {  // 父进程继续从管道中读取
        close(p[0]);
        while ((n = read(fd, &y, 4)) == 4) {
            if ((y % x) != 0) {  // 筛选
                write(p[1], &y, 4);
            }
        }
        close(p[1]);
        close(fd);
        if (n != 0 && n < 4) {  // 读取遇到问题
            fprintf(2, "write error\n");
            n = 1;
        }
        wait(0);
        exit(n);
    }
}

int main(int argc, char* argv[])
{
    int p[2];
    pipe(p);
    for (int i = 2; i < 36; i++) {
        write(p[1], &i, 4);
    }
    if (fork() == 0) {
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