// GCC-VERSION - Not tested for -O3 flag for GCC version above 5.

#ifndef __HFAIRLOCK_H__
#define __HFAIRLOCK_H__

#include <cstdio>
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

// namespace upscaledb {
typedef unsigned long long ull;

#define MAX_DEPTH 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct stats {
    ull reenter;
    ull banned_time;
    ull start;
    ull next_runnable_wait;
    ull prev_slice_wait;
    ull own_slice_wait;
    ull runnable_wait;
    ull succ_wait;
    ull release_succ_wait;
} stats_t;

typedef struct node {
    ull cs;
    int parent;
    int weight;
    ull slice;
    ull banned_until;
} node_t;

typedef struct flthread_info {
    ull banned_until;
    ull weight;
    ull slice;
    ull start_ticks;
    int banned;
    int parent;
    stats_t stat;
    ull nslice;
    int path[MAX_DEPTH];
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

typedef struct hfairlock_t {
    qnode_t *qtail __attribute__ ((aligned (CACHELINE)));
    qnode_t *qnext __attribute__ ((aligned (CACHELINE)));
    ull slice __attribute__ ((aligned (CACHELINE)));
    int slice_valid __attribute__ ((aligned (CACHELINE)));
    node_t *hierarchy __attribute__ ((aligned (CACHELINE)));
    pthread_key_t flthread_info_key;
    ull total_weight;
} hfairlock_t __attribute__ ((aligned (CACHELINE)));

static inline qnode_t *flqnode(hfairlock_t *lock) {
    return (qnode_t *) ((char *) &lock->qnext - offsetof(qnode_t, next));
}

static inline int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, NULL, 0);
}


static flthread_info_t *flthread_info_create(hfairlock_t *lock, int weight) {
    flthread_info_t *info;
    info = (flthread_info_t *)malloc(sizeof(flthread_info_t));
    info->banned_until = rdtsc();
    if (weight == 0) {
        int prio = getpriority(PRIO_PROCESS, 0);
        weight = prio_to_weight[prio+20];
    }
    info->weight = weight;
    __sync_add_and_fetch(&lock->total_weight, weight);
    info->banned = 0;
    info->slice = 0;
    info->start_ticks = 0;
    memset(&info->stat, 0, sizeof(stats_t));
    info->stat.start = info->banned_until;
    return info;
}

int hfairlock_init(hfairlock_t *lock);
void read_hierarchy(hfairlock_t *lock, const char *str);
void set_path(hfairlock_t *lock, int path[], int parent, int weight, ull banned_until);

void hfairlock_thread_init(hfairlock_t *lock, int id, int parent, int prior);
// void hfairlock_thread_init(hfairlock_t *lock, int id);

int hfairlock_destroy(hfairlock_t *lock);
ull get_updated_ban(hfairlock_t *lock, int parent, ull banned_until);
int is_reacquired(hfairlock_t *lock, int parent);
ull set_slice(hfairlock_t *lock, flthread_info_t *info);
void hfairlock_acquire(hfairlock_t *lock);
void hfairlock_release(hfairlock_t *lock);
// }

#endif // __HFAIRLOCK_H__
