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