// (C) Copyright 2007-8 Anthony Williams
// (C) Copyright 2011,2012,2015 Vicente J. Botet Escriba
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef TESTLOCK
#define TESTLOCK
#include <iostream>
#include <pthread.h>
#include <boost/thread/mutex.hpp>
#include "mulock.h"
// #include "rdtsc.h"

typedef unsigned long long ull;

typedef struct lockstats_t {
    int id;
    int tid;
    int find;
    int insert;
    ull hold_time;
    ull start_ticks;
} lockstats_t;

#define NUM_THREADS 8

class TestLock {
private:
    pthread_key_t key;
    // thread counter for no of threads is cs
    int csthread;
    // number of lock acquires
    ull nacquires;
    // idle time start
    ull idle_start;
    // total idle time
    ull lock_idle;
    // bool for locking
    int locked_;
    // h-scl
    lock_t mulock;
    int *prior;
    int *parent;
    void read_thread_data() {
        const char *fpath = "/users/ant0301/hierarchical-scl/upscaledb/thread_data.txt";
        FILE *fp = fopen(fpath, "r");
        int nthread;
        fscanf(fp, "%d", &nthread);
        prior = (int *)malloc(sizeof(int)* nthread);
        parent = (int *)malloc(sizeof(int)* nthread);
        for (int i = 0; i < nthread; ++i)
        {
            fscanf(fp, "%d %d", &prior[i], &parent[i]);
        }
    }

public:
    TestLock()
    {
        pthread_key_create(&key, NULL);
        lock_init(&mulock);
        read_thread_data();
        locked_ = 0;
        idle_start = 0;//rdtsc();
    }
    ~TestLock()
    {
      print_stats();
      free(prior);
      free(parent);
      pthread_key_delete(key);
      lock_destroy(&mulock);
    }

    void setcpu(int id) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        if (id % 2 == 0) {
            // insert threads
            for (int i = 0; i < 8; i++) {
                CPU_SET(i, &cpuset);
            }
        } else {
            // find threads
            for (int i = 8; i < 16; i++) {
                CPU_SET(i, &cpuset);
            }
        }
    }

    int write_stats_file(lockstats_t *info) {
        char str[90];
        sprintf(str, "/users/ant0301/hierarchical-scl/upscaledb/output/%d", info->id);
        FILE* fd = fopen(str, "w+");
        if (fd == NULL) {
            // perror("Unable to write to file\n");
            return 0;
        }

        flthread_info_t *flth_info = (flthread_info_t *)pthread_getspecific(mulock.flthread_info_key);
        lockstats_t *dump = (lockstats_t *)pthread_getspecific(key);
        fprintf(fd, 
                "hold_time(ms) %f\n"
                "nslice %llu\n"
                "parent %d\n"
                "weight %llu\n"
                "own_slice_wait %llu\n"
                "prev_slice_wait %llu\n"
                "runnable_wait %llu\n"
                "next_runnable_wait %llu\n"
                "succ_wait %llu\n"
                "reenter %llu\n"
                "banned(actual) %llu\n"
                "banned %llu\n"
                "elapse %llu\n"
                "find %d\n"
                "insert %d\n"
                ,
                dump->hold_time / (float) (CYCLE_PER_US * 1000),
                flth_info->nslice,
                flth_info->parent,
                flth_info->weight,
                flth_info->stat.own_slice_wait,
                flth_info->stat.prev_slice_wait,
                flth_info->stat.runnable_wait,
                flth_info->stat.next_runnable_wait,
                flth_info->stat.succ_wait,
                flth_info->stat.reenter,
                flth_info->stat.banned_time,
                flth_info->banned_until-flth_info->stat.start,
                flth_info->start_ticks-flth_info->stat.start,
                dump->find,
                dump->insert
                );
        fclose(fd);
        return 1;
    }

    void lock()
    {
        lockstats_t *info = (lockstats_t *)pthread_getspecific(key);
        if (info == NULL) {
            info = (lockstats_t *)malloc(sizeof(lockstats_t));
            pthread_setspecific(key, info);
            info->id = gettid();
            info->find = info->insert = 0;
            setcpu(info->id);
            thread_init(&mulock, info->id, parent[info->id % NUM_THREADS], prior[info->id % NUM_THREADS]);
        }
        int i = __atomic_add_fetch(&csthread, 1, __ATOMIC_RELAXED);
        if (i == 1) {
            if (idle_start != 0) {
                lock_idle += rdtsc() - idle_start;
            }
        }
        lock_acquire(&mulock);
        nacquires++;
        locked_ = info->id;
        info->start_ticks = rdtscp();
        if (write_stats_file(info) == 0) {
            std::cout << "Unable to write to file" << "/users/ant0301/hierarchical-scl/upscaledb/output/" << info->id << std::endl;
        }
    }

    void unlock()
    {
        lockstats_t *info = (lockstats_t *)pthread_getspecific(key);
        if (locked_ == info->id) {
            locked_ = -1;
            int i = __atomic_sub_fetch(&csthread, 1, __ATOMIC_RELAXED);
            if (i == 0) {
                idle_start = rdtsc();
            }
            lock_release(&mulock);
            info->hold_time += rdtscp() - info->start_ticks;
        }
    }

    typedef lock_t* native_handle_type;
    native_handle_type native_handle()
    {
        return &mulock;
    }

    void set_op(int op) {
        lockstats_t *dump = (lockstats_t *)pthread_getspecific(key);
        if (op == 0) {
            // insert
            dump->insert = 1;
        } else {
            // find
            dump->find = 1;
        }
    }

    void print_stats() {
        std::cout << "\nidle_time(ms)" << lock_idle / (float) (CYCLE_PER_US * 1000);
        std::cout << std::endl;
    }
};
#endif /*TESTLOCK*/

