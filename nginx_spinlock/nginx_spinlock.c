#include "nginx_spinlock.h"

static int ncpu;

// main之前运行的函数
__attribute((constructor)) void getncpu()
{
    ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    //printf("ncpu==%d\n", ncpu);
}

void
spinlock_lock(volatile int *lock, int value, unsigned int spin)
{

    unsigned int  i, n;
   //一直处于循环中，直到获取到锁
    for ( ;; ) {
        //lock为0表示没有其他进程持有锁，这时将lock值设置为value参数表示当前进程持有了锁
        if (*lock == 0 && cmpxchg(lock, 0, value) == 0) {
            return;
        }

        //如果是多处理器系统
        if (ncpu > 1) {

            for (n = 1; n < spin; n <<= 1) {
                //随着等待的次数越来越多，实际去检查锁的间隔时间越来越大
                for (i = 0; i < n; i++) {
                    cpu_pause();    //告诉CPU现在处于自旋锁等待状态
                }
                //检查锁是否被释放
                if (*lock == 0 && cmpxchg(lock, 0, value) == 0) {
                    return;
                }
            }
        }
        // sched_yield() 和 usleep 都先让出cpu的使用权, 但仍然处于可执行状态
        sched_yield();
    }
}

void spinlock_init(volatile int *s)
{
    *s = 0;
}


