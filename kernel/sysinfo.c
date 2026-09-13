#include "types.h"
#include "riscv.h"
#include "sysinfo.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

uint64 
sys_sysinfo(void)
{
    struct sysinfo sinfo;
    uint64 addr;
    struct proc* p = myproc();

    if(argaddr(0,&addr)< 0){
        return -1;
    }
    
    //get the free memory
    sinfo.freemem = kcount();
    
    //get processors
    
    sinfo.nproc = proc_count();

    //copy out from kernel to user
    if(copyout(p->pagetable,addr,(char*)&sinfo,sizeof(sinfo)) < 0){
        return -1;
    }
    return 0;
};