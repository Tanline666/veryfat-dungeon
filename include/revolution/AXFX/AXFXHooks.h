/**
 * @addtogroup rvl_axfx
 * @{
 */
#ifndef RVL_SDK_AXFX_HOOKS_H
#define RVL_SDK_AXFX_HOOKS_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*AXFXAllocHook)(size_t size);
typedef void (*AXFXFreeHook)(void* block);

extern AXFXAllocHook __AXFXAlloc;
extern AXFXFreeHook __AXFXFree;

/**
 * \~english @brief Registers custom AXFX memory allocation routines.
 * \~japanese @brief カスタムAXFXメモリ確保ルーチンを登録する。
 *
 * \~english @param alloc AXFX memory allocation function
 * \~japanese @param alloc AXFXメモリ確保関数
 * \~english @param free AXFX memory freeing function
 * \~japanese @param free AXFXメモリ解放関数
 */
void AXFXSetHooks(AXFXAllocHook alloc, AXFXFreeHook free);
/**
 * \~english @brief Gets the registered AXFX memory allocation routines.
 * \~japanese @brief 登録されているAXFXメモリ確保ルーチンを取得する。
 *
 * \~english @param alloc Storage destination of AXFX memory allocation function
 * \~japanese @param alloc AXFXメモリ確保関数の格納先
 * \~english @param free Storage destination of AXFX memory freeing function
 * \~japanese @param free AXFXメモリ解放関数の格納先
 */
void AXFXGetHooks(AXFXAllocHook* alloc, AXFXFreeHook* free);

#ifdef __cplusplus
}
#endif
#endif
/**@} */
