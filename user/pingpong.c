#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int p0[2];
    int p1[2];
    if (pipe(p0) == -1) {
        fprintf(2, "pipe error\n");
        exit(1);
    }
    if (pipe(p1) == -1) {
        fprintf(2, "pipe error\n");
        exit(1);
    }
    if (fork() == 0) {
        char c;
        int fd0 = dup(p0[0]);
        close(p0[0]);
        close(p0[1]);
        read(fd0, &c, 1);
        close(fd0);
        fprintf(1, "%d: received ping\n", getpid());
        write(p1[1], &c, 1);
        close(p1[1]);
        close(p1[0]);
        exit(0);
    }
    else {
        char c;
        int fd;
        write(p0[1], "c", 1);
        close(p0[1]);
        close(p0[0]);

        fd = dup(p1[0]);
        close(p1[0]);
        close(p1[1]);
        read(fd, &c, 1);
        if (c == 'c') {
            fprintf(1, "%d: received pong\n", getpid());
        }
        exit(0);
    }
}