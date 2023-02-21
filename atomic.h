
/***
  * atomic.h 这是在X86平台下的原子操作
  *
  * 原子操作的实现：(1) 使用volatile的特性，不受编译器优化，直接读取内存的特性
  *                 (2) 使用汇编里面的 lock 指令前缀：
  *                     LOCK指令前缀功能如下：
  *                         被修饰的汇编指令成为“原子的”
  *                         与被修饰的汇编指令一起提供内存屏障效果
***/

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include <stdio.h>


typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long  u64;


#define CH_IDEA  1

#define CH__X86_CASE_B      1
#define CH__X86_CASE_W      2
#define CH__X86_CASE_L      4
#define CH__X86_CASE_Q      8

#define ch__cmpxchg_wrong_size(str)     printf(str);


/*强制转化为 volatile类型 让编译器操作内存*/
#define ACCESS_ONCE(x)         (*(volatile typeof(x) *)&(x))

#define WRITE_ONCE(var, val)    ACCESS_ONCE(var) = (val))
#define READ_ONCE(x)            ACCESS_ONCE(x)

#define con_atomic_read(v)      ACCESS_ONCE(v)
#define con_atomic_set(v, i)    WRITE_ONCE(v, i)


#if 0   // 平常数的原子操作，暂时不写
#define __con_atomic_inc(v, size, lock)       \
({                                      \
    switch (size) {
    case CH__X86_CASE_B:                        \
    {                                \
        volatile u8 *__v = (volatile u8 *)(v);        \
        __asm__ __volatile__(lock "incb %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "q" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    case CH__X86_CASE_W:                        \
    {                                \
        volatile u16 *__ptr = (volatile u16 *)(ptr);        \
        __asm__ __volatile__(lock "incw %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "r" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    case CH__X86_CASE_L:                        \
    {                                \
        volatile u32 *__ptr = (volatile u32 *)(ptr);        \
        __asm__ __volatile__(lock "incl %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "r" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    case CH__X86_CASE_Q:                        \
    {                                \
        volatile u64 *__ptr = (volatile u64 *)(ptr);        \
        __asm__ __volatile__(lock "incq %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "r" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    default:                            \
        ch__cmpxchg_wrong_size("cmpxchg error!\n");                    \
    }                    

    }


})


#define con_atomic_inc(v)      __con_atomic_inc(v, sizeof(*v), "lock;")

#endif



/*
 * Atomic compare and exchange.  Compare OLD with MEM, if identical,
 * store NEW in MEM.  Return the initial value in MEM.  Success is
 * indicated by comparing RETURN with OLD.
 */

#define __raw_cmpxchg(ptr, old, new, size, lock)            \
({                                    \
    typeof(*(ptr)) __ret;                    \
    typeof(*(ptr)) __old = (old);                \
    typeof(*(ptr)) __new = (new);                \
    switch (size) {                            \
    case CH__X86_CASE_B:                        \
    {                                \
        volatile u8 *__ptr = (volatile u8 *)(ptr);        \
        __asm__ __volatile__(lock "cmpxchgb %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "q" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    case CH__X86_CASE_W:                        \
    {                                \
        volatile u16 *__ptr = (volatile u16 *)(ptr);        \
        __asm__ __volatile__(lock "cmpxchgw %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "r" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    case CH__X86_CASE_L:                        \
    {                                \
        volatile u32 *__ptr = (volatile u32 *)(ptr);        \
        __asm__ __volatile__(lock "cmpxchgl %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "r" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    case CH__X86_CASE_Q:                        \
    {                                \
        volatile u64 *__ptr = (volatile u64 *)(ptr);        \
        __asm__ __volatile__(lock "cmpxchgq %2,%1"            \
                 : "=a" (__ret), "+m" (*__ptr)        \
                 : "r" (__new), "0" (__old)            \
                 : "memory");                \
        break;                            \
    }                                \
    default:                            \
        ch__cmpxchg_wrong_size("cmpxchg error!\n");                    \
    }                                \
    __ret;                                \
})




#define __cmpxchg(ptr, old, new, size)      \
            __raw_cmpxchg(ptr, old, new ,size, "lock;")

#define cmpxchg(ptr, old, new)                  \
        __cmpxchg(ptr, old, new, sizeof(*(ptr)))



typedef struct hatomic {
     volatile int counter;
} hatomic_t;


/***
  * @brief:原子操作的读
  *  这是这里直接读取内存
  *
***/
static inline int hatomic_read(const hatomic_t *v) {
    return  ACCESS_ONCE(v)->counter;
}



/***
 * @brief: 原子操作的设置
 * 目前有争议，需要查看 内核的 set实现
 **/
/*
b = a;
    movl    -4(%rbp), %eax   先把 rbp寄存器 栈底的位置-4 的值，移动到 eax寄存器中
    movl    %eax, -8(%rbp)   然后把 eax寄存器位置的值

b = 1;
    movl    $1, -8(%rbp)

感觉虽然 b = a是对应汇编是两条指令，但是hatomic_set中i是局部变量，是该cpu寄存器的内存中存储
而且i也不会的不会更改，所以可以使用直接赋值的方式进行使用为原子操作：ACCESS_ONCE(v)->counter = i; 
*/
static inline void hatomic_set(hatomic_t *v, int i) {
#if CH_IDEA
    ACCESS_ONCE(v)->counter = i;
#else
    v->counter = i;
#endif
}

/**
 * @brief  add 添加，把int类型的 i，加上v 返回v; v->counter = i + v->counter;
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v.
 */
static inline void hatomic_add(int i, hatomic_t *v) {
    __asm__ __volatile__("lock;" "addl %1,%0"
    : "+m" (v->counter)
    : "ir" (i));
}

/**
 * @brief  sub 表示 减去，执行 v->counter =  v->counter - i;
 * 也就是 执行v->counter =  v->counter - i 是原子操作
 * atomic_sub - subtract integer from atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v.
 *
 */
static inline void hatomic_sub(int i, hatomic_t *v) {
    __asm__ __volatile__("lock;" "subl %1,%0"
    : "+m" (v->counter)
    : "ir" (i));

}

/***
 * @brief inc 表示自加，v自加 v->counter++;
 * 
 **/
static inline void hatomic_inc(hatomic_t *v) {
    __asm__ __volatile__("lock;" "incl %0"
             : "+m" (v->counter));
}

/***
 *  @brief dec 表示自减， v自减 v->counter--;
 *
 **/
static inline void hatomic_dec(hatomic_t *v) {

     __asm__ __volatile__("lock;" "decl %0"
         : "+m" (v->counter));
}




#endif



