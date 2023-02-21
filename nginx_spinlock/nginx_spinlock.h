#include <stdio.h>
#include <unistd.h>
#include "../atomic.h"

#define cpu_pause()         __asm__ (".byte 0xf3, 0x90")
#define spinlock_unlock(lock)    *(lock) = 0

void spinlock_lock(volatile int *lock, int value, unsigned int spin);

void spinlock_init(volatile int *s);



