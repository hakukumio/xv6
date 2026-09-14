#include "kernel/types.h"
#include "user/user.h"

int 
main(void) {
    printf("My getpid() = %d,ugetpid() = %d\n",getpid(),ugetpid());
    int pid = fork();
    if(pid == 0){//child
        printf("I'm child,getpid()= %d,ugetpid() = %d\n",getpid(),ugetpid());
        exit(0);
    } else if(pid == -1) {
        fprintf(2,"fork error\n");
        exit(1);
    } else {
        wait(0);
        printf("I Love Computer Science\n");
        printf("I'm parent,my child's pid = %d\n",pid);
        exit(0);
    }

}