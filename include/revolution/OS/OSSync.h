/**
 * @addtogroup rvl_os
 * @{
 */
#ifndef RVL_SDK_OS_SYNC_H
#define RVL_SDK_OS_SYNC_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \~english @brief Registers the system call handler.
 * \~japanese @brief システムコールハンドラを登録する。
 */
void __OSInitSystemCall(void);

#ifdef __cplusplus
}
#endif
#endif
/**@} */
