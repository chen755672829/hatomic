#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#include "nginx_spinlock.h"

static volatile int lock_s;
int test_num;
#define FORNUM  1000000

void * thread_work1(void * arg)
{
    int i;
    for(i = 0 ; i < FORNUM; i++) {
        spinlock_lock(&lock_s, 1, 2048);
        test_num ++;
        spinlock_unlock(&lock_s);
    }

}

void * thread_work2(void * arg)
{
    int i;
    for(i = 0 ; i < FORNUM; i++) {
        spinlock_lock(&lock_s, 1, 2048);
        test_num ++;
        spinlock_unlock(&lock_s);
    }

}

void * thread_work3(void * arg)
{
    int i;
    for(i = 0 ; i < FORNUM; i++) {
        spinlock_lock(&lock_s, 1, 2048);
        test_num ++;
        spinlock_unlock(&lock_s);
    }

}

int main()
{
    int err;
    test_num = 0;
    spinlock_init(&lock_s);
    
    pthread_t pid[3];
    err = pthread_create(&pid[0], NULL, thread_work1, NULL);
    if(err){
        printf("pid[0] create fail\n");
    }
    err = pthread_create(&pid[1], NULL, thread_work2, NULL);
    if(err){
        printf("pid[1] create fail\n");
    }
    err = pthread_create(&pid[2], NULL, thread_work3, NULL);
    if(err){
        printf("pid[2] create fail\n");
    }

    pthread_join(pid[0], NULL);
    pthread_join(pid[1], NULL);
    pthread_join(pid[2], NULL);
    printf("test_num==%d\n", test_num);
    return 0;
}




