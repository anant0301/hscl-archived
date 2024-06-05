#ifndef CUSTOMLOCK
#define CUSTOMLOCK

// namespace upscaledb {

// void thread_pin(int id) {
// 	int ncpu = 0;
//     cpu_set_t mask;
//     CPU_ZERO(&mask);
//     CPU_SET(2 * id , &mask);
//     CPU_SET(2 * id + 1, &mask);
//     if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
//         perror("sched_setaffinity");
//         exit(EXIT_FAILURE);
//     }
// }

// #ifdef MUTEX
// #include <pthread.h>
// #include "rdtsc.h"
// #include "common.h"
// typedef pthread_mutex_t lock_t;
// #define lock_init(plock) pthread_mutex_init(plock, NULL)
// #define lock_acquire(plock) pthread_mutex_lock(plock)
// #define lock_release(plock) pthread_mutex_unlock(plock)
// #define thread_initthread_init(plock, id, parent, prior) {if (1);}
// #define lock_destroy(plock) {if (1);}

// #elif SPIN
// #include <pthread.h>
// typedef pthread_spinlock_t lock_t;
// #define lock_init(plock) pthread_spin_init(plock, PTHREAD_PROCESS_PRIVATE)
// #define lock_acquire(plock) pthread_spin_lock(plock)
// #define lock_release(plock) pthread_spin_unlock(plock)

// #elif CPTKT
// #include "cptltkt.h"
// typedef cpt_mutex_t lock_t;
// #define lock_init() cpt_mutex_create(PTHREAD_PROCESS_PRIVATE)
// #define lock_acquire(plock) cpt_mutex_lock(plock, NULL)
// #define lock_release(plock) cpt_mutex_unlock(plock, NULL)
// #define thread_init(plock) cpt_thread_start()
// #define lock_init_context(plock, pnode, nthread) cpt_init_context(plock, pnode, nthread)

// #elif CTTKT
// #include "ctkttkt.h"
// typedef ctkt_mutex_t lock_t;
// #define lock_init() ctkt_mutex_create(PTHREAD_PROCESS_PRIVATE)
// #define lock_acquire(plock) ctkt_mutex_lock(plock, NULL)
// #define lock_release(plock) ctkt_mutex_unlock(plock, NULL)
// #define thread_init(plock) ctkt_thread_start()
// #define lock_init_context ctkt_init_context

// #elif HMCS
// #include "hmcs.h"
// typedef hmcs_mutex_t lock_t;
// #define lock_init() hmcs_mutex_create(PTHREAD_PROCESS_PRIVATE)
// #define lock_acquire(plock, pcontext) hmcs_mutex_lock(plock, pcontext)
// #define lock_release(plock, pcontext) hmcs_mutex_unlock(plock, pcontext)
// #define thread_init(plock) hmcs_thread_start()
// #define lock_init_context hmcs_init_context

// #elif HTEPFL
// #include "htlockepfl.h"
// typedef htlockepfl_mutex_t lock_t;
// #define lock_init() htlockepfl_mutex_create(PTHREAD_PROCESS_PRIVATE)
// #define lock_acquire(plock, pcontext) htlockepfl_mutex_lock(plock, pcontext)
// #define lock_release(plock, pcontext) htlockepfl_mutex_unlock(plock, pcontext)
// #define thread_init(plock) htlockepfl_thread_start()
// #define lock_init_context htlockepfl_init_context

// #elif HYMCS
// #include "hyshmcs.h"
// typedef hyshmcs_mutex_t lock_t;
// #define lock_init() hyshmcs_mutex_create(PTHREAD_PROCESS_PRIVATE)
// #define lock_acquire(plock, pcontext) hyshmcs_mutex_lock(plock, pcontext)
// #define lock_release(plock, pcontext) hyshmcs_mutex_unlock(plock, pcontext)
// #define thread_init(plock) hyshmcs_thread_start()
// #define lock_init_context hyshmcs_init_context

// #elif HSCL
#define HSCLDEBUG
#include "hfairlock.h"
typedef struct hfairlock_t lock_t;
#define lock_init(plock) hfairlock_init(plock)
#define thread_init(plock, id, parent, prior) hfairlock_thread_init(plock, id, parent, prior)
#define lock_acquire(plock) hfairlock_acquire(plock)
#define lock_release(plock) hfairlock_release(plock)
#define lock_destroy(plock) hfairlock_destroy(plock)

// #elif FAIRLOCK
// #include "fairlock.h"
// typedef struct fairlock_t lock_t;
// #define lock_init(plock) fairlock_init(plock)
// #define thread_init(plock, id) fairlock_thread_init(plock, id)
// #define lock_acquire(plock) fairlock_acquire(plock)
// #define lock_release(plock) fairlock_release(plock)
// #define lock_destroy(plock) fairlock_destroy(plock)

// #endif // type of Mutex imported
// };

#endif /* CUSTOMLOCK */