
#include <stdio.h>
#include <malloc.h>

#include <pthread.h>

#include "atomic.h"

#define hmalloc(v) malloc(sizeof(*(v)))

#define TEST_MAX 1000000
#define HATOMIC_T_TEST 0
#define CAS_TEST 1


static volatile int ret = 0;
static volatile int dec = 0;
static volatile int add = 0;
static volatile int sub = 0;

static int cas = -5000000;

static hatomic_t *v = NULL;
static hatomic_t *v_dec = NULL;
static hatomic_t *v_add = NULL;
static hatomic_t *v_sub = NULL;



/***
其中一个 ret++的执行过程
汇编角度看：
movl    -4(%rbp), %eax    把 内存的位置的ret值，移动到eax寄存器中
addl    $1, %eax          然后把 再eax寄存器加1
movl    %eax, -4(%rbp)    然后把寄存器中的值，写回到 内存中

问king老师，king老师是这么解释的：
-4（%rbp）表示 内存（非寄存器中）里面的变量ret的位置

但是我查的 rbp 也是一个寄存器， 算了，先这么理解吧。

当多个线程同时操作ret++时，


若执行过程如下：

线程一：                            线程二：
若这时候 内存中ret值为1
movl    -4(%rbp), %eax
                                这里 内存中ret值也为1
                            movl    -4(%rbp), %eax
                            addl    $1, %eax      
                            movl    %eax, -4(%rbp)
addl    $1, %eax      
movl    %eax, -4(%rbp)

这时候两个线程操作完后，ret的值为2，而不是3

*/

void *func1() {

    int i, tmp_cas;
    for (i = 0; i < TEST_MAX; i++ ) {

#if HATOMIC_T_TEST
        ret++;
        hatomic_inc(v);

        dec--;
        hatomic_dec(v_dec);

        add = add+1;
        hatomic_add(1, v_add);

        sub = sub - 1;
        hatomic_sub(1, v_sub);
#endif

#if CAS_TEST
        do {
            tmp_cas = cas;
        } while(cmpxchg(&cas, tmp_cas, tmp_cas - 1) != tmp_cas);
#endif

    }
}

void *func2() {

    int i, tmp_cas;
    for (i = 0; i < TEST_MAX; i++ ) {
#if HATOMIC_T_TEST
        ret++;
        hatomic_inc(v);

        dec--;
        hatomic_dec(v_dec);

        add = add+1;
        hatomic_add(1, v_add);

        sub = sub - 1;
        hatomic_sub(1, v_sub);
#endif

#if CAS_TEST
        do {
            tmp_cas = cas;
        } while(cmpxchg(&cas, tmp_cas, tmp_cas - 1) != tmp_cas);
#endif
    }
}

void *func3() {

    int i, tmp_cas;
    for (i = 0; i < TEST_MAX; i++ ) {
#if HATOMIC_T_TEST
        ret++;
        hatomic_inc(v);
        dec--;
        hatomic_dec(v_dec);
        
        add = add+1;
        hatomic_add(1, v_add);
        
        sub = sub - 1;
        hatomic_sub(1, v_sub);
#endif 
#if CAS_TEST
        do {
            tmp_cas = cas;
        } while(cmpxchg(&cas, tmp_cas, tmp_cas - 1) != tmp_cas);
#endif
    }
}

void *func4() {

    int i, tmp_cas ;
    for (i = 0; i < TEST_MAX; i++ ) {
#if HATOMIC_T_TEST

        ret++;
        hatomic_inc(v);

        dec--;
        hatomic_dec(v_dec);

        add = add+1;
        hatomic_add(1, v_add);

        sub = sub - 1;
        hatomic_sub(1, v_sub);
#endif
#if CAS_TEST

        do {
            tmp_cas = cas;
        } while(cmpxchg(&cas, tmp_cas, tmp_cas - 1) != tmp_cas);
#endif
    }
}


void init() {
    v = (hatomic_t *)hmalloc(v);
    v->counter = 0;

    v_add = (hatomic_t *)hmalloc(v_add);
    v_add->counter = 0;

    v_dec = (hatomic_t *)hmalloc(v_dec);
    v_dec->counter = 0;

    v_sub = (hatomic_t *)hmalloc(v_sub);
    v_sub->counter = 0;

    int cas_tt = 2;

    int return_cas = cmpxchg(&cas_tt, 3, 5);

    printf("return_cas==%d cas_tt==%d\n", return_cas, cas_tt);

}


int main() {

    init();
    pthread_t p[4];

    pthread_create(&p[0], NULL, func1, NULL);
    pthread_create(&p[1], NULL, func2, NULL);
    pthread_create(&p[2], NULL, func3, NULL);
    pthread_create(&p[3], NULL, func4, NULL);

    pthread_join(p[0], NULL);
    pthread_join(p[1], NULL);
    pthread_join(p[2], NULL);
    pthread_join(p[3], NULL);

    /**
     * 执行结果：
     * [root@lvs base_module]# ./atomic
     * ret==3551939 v->conuter==4000000
     * add==3574536 v_add->conuter==4000000
     * dec==-3474442 v_dec->conuter==-4000000
     * sub==-3597176 v_sub->conuter==-4000000
     **/
    printf("ret==%d v->conuter==%d\n", ret, v->counter);
    printf("add==%d v_add->conuter==%d\n", add, v_add->counter);
    printf("dec==%d v_dec->conuter==%d\n", dec, v_dec->counter);
    printf("sub==%d v_sub->conuter==%d\n", sub, v_sub->counter);

    printf("cas==%d\n", cas);
    return 0;
}





