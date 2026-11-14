#ifndef RVL_SDK_OS_CACHE_H
#define RVL_SDK_OS_CACHE_H
#include <types.h>

#include <revolution/OS/OSContext.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_CACHE_BASE 0xE0000000

// Forward declarations

void DCEnable(void);
void DCInvalidateRange(const void* buf, u32 len);
void DCFlushRange(const void* buf, u32 len);
void DCStoreRange(const void* buf, u32 len);
void DCFlushRangeNoSync(const void* buf, u32 len);
void DCStoreRangeNoSync(const void* buf, u32 len);
void DCZeroRange(const void* buf, u32 len);

void ICInvalidateRange(const void* buf, u32 len);
void ICFlashInvalidate(void);
void ICEnable(void);

void LCEnable(void);
void LCDisable(void);
void LCLoadBlocks(void* dst, const void* src, u32 blocks);
/**
 * @~english @brief Places a single DMA transaction to transfer data from the
 * locked cache to main memory into the queue.
 * @~japanese @brief
 * ロックドキャッシュのデータをメインメモリに転送する単一のDMAトランザクションをキューに入る。
 *
 * @~english @param dst Pointer to 32-byte aligned start address for destination
 * of transferred memory
 * @~japanese @param dst
 * 転送先のメモリの32byteでアライメントされた先頭アドレスへのポインタ
 * @~english @param src Pointer to 32-byte aligned start address for source of
 * transferred memory
 * @~japanese @param src
 * 転送元のメモリの32byteでアライメントされた先頭アドレスへのポインタ
 * @~english @param blocks Transfer size in cache blocks
 * @~japanese @param blocks キャッシュブロック単位での転送サイズ
 */
void LCStoreBlocks(void* dst, const void* src, u32 blocks);
u32 LCStoreData(void* dst, const void* src, u32 len);
u32 LCQueueLength(void);
void LCQueueWait(u32 n);

void L2Enable(void);
void L2Disable(void);
void L2GlobalInvalidate(void);

void DMAErrorHandler(u8 error, OSContext* ctx, u32 dsisr, u32 dar, ...);

void __OSCacheInit(void);

#ifdef __cplusplus
}
#endif
#endif
