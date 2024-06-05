// GCC-VERSION - Not tested for -O3 flag for GCC version above 5.

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <linux/futex.h>
#include <pthread.h>
#include "rdtsc.h"
#include "common.h"

typedef unsigned long long ull;

typedef struct stats {
    ull reenter;
    ull nslice;
    ull banned_time;
    ull start;
    ull next_runnable_wait;
    ull prev_slice_wait;
    ull own_slice_wait;
    ull runnable_wait;
    ull succ_wait;
    ull release_succ_wait;
} stats_t;

typedef struct flthread_info {
    ull banned_until;
    ull weight;
    ull slice;
    ull start_ticks;
    int banned;
    stats_t stat;
} flthread_info_t;

enum qnode_state {
    INIT = 0, // not waiting or after next runnable node
    NEXT,
    RUNNABLE,
    RUNNING
};

typedef struct qnode {
    int state __attribute__ ((aligned (CACHELINE)));
    struct qnode *next __attribute__ ((aligned (CACHELINE)));
} qnode_t __attribute__ ((aligned (CACHELINE)));

typedef struct fairlock_t {
    qnode_t *qtail __attribute__ ((aligned (CACHELINE)));
    qnode_t *qnext __attribute__ ((aligned (CACHELINE)));
    ull slice __attribute__ ((aligned (CACHELINE)));
    int slice_valid __attribute__ ((aligned (CACHELINE)));
    pthread_key_t flthread_info_key;
    ull total_weight;
} fairlock_t __attribute__ ((aligned (CACHELINE)));

static inline qnode_t *flqnode(fairlock_t *lock) {
    return (qnode_t *) ((char *) &lock->qnext - offsetof(qnode_t, next));
}

static inline int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, NULL, 0);
}

int fairlock_init(fairlock_t *lock);
flthread_info_t *flthread_info_create(fairlock_t *lock, int weight);
void fairlock_thread_init(fairlock_t *lock, int weight);
int fairlock_destroy(fairlock_t *lock);
void fairlock_acquire(fairlock_t *lock);
void fairlock_release(fairlock_t *lock);

