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