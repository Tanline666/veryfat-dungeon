/**
 * @addtogroup rvl_os
 * @{
 */
#ifndef RVL_SDK_OS_MUTEX_H
#define RVL_SDK_OS_MUTEX_H
#include <types.h>

#include <revolution/OS/OSThread.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSMutex {
    OSThreadQueue queue;  // at 0x0
    OSThread* thread;     // at 0x8
    s32 lock;             // at 0xC
    struct OSMutex* next; // at 0x10
    struct OSMutex* prev; // at 0x14
} OSMutex;

/**
 * @~english @brief Initializes a mutex.
 * @~japanese @brief ミューテックスを初期化する。
 *
 * @~english @param mutex Pointer to mutex to be initialized
 * @~japanese @param mutex 初期化するミューテックスへのポインタ
 */
void OSInitMutex(OSMutex* mutex);
/**
 * @~english @brief Attempts to lock the specified mutex for the calling thread.
 * @~japanese @brief
 * 呼び出すスレッドのために指定されたミューテックスのロックを試みる。
 * @~english @details If another thread has already locked the mutex, the
 * calling thread is suspended until the mutex is unlocked.
 * @~japanese @details
 * 別のスレッドが既にミューテックスをロックしたら、呼び出すスレッドはミューテックスが解放されるまで一時停止される。
 *
 * @~english @param mutex Pointer to mutex
 * @~japanese @param mutex ミューテックスへのポインタ
 */
void OSLockMutex(OSMutex* mutex);
void OSUnlockMutex(OSMutex* mutex);
void __OSUnlockAllMutex(OSThread* thread);
BOOL OSTryLockMutex(OSMutex* mutex);

#ifdef __cplusplus
}
#endif
#endif
/**@} */
