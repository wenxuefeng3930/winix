#include <lib.h>



unsigned int alarm(unsigned int seconds){
    struct message m;
    if(seconds == 0)
        return 0;
    m.m1_i1 = seconds;
    return _syscall(SYSCALL_ALARM,&m);
}
