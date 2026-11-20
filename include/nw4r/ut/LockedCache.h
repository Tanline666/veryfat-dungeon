#ifndef NW4R_UT_LOCKED_CACHE_H
#define NW4R_UT_LOCKED_CACHE_H
#include <nw4r/types_nw4r.h>

#include <revolution/OS.h>

namespace nw4r {
namespace ut {
class LC {
public:
    static void Enable();
    static void Disable();

    static bool Lock();
    static void Unlock();

    static void LoadBlocks(void* pDst, void* pSrc, u32 blocks);
    /**
     * @~english @brief Places a single DMA transaction to transfer data from
     * the locked cache to main memory into the queue.
     * @~japanese @brief
     * ロックドキャッシュのデータをメインメモリに転送する単一のDMAトランザクションをキューに入る。
     *
     * @~english @param destAddr Pointer to 32-byte aligned start address for
     * destination of transferred memory
     * @~japanese @param destAddr
     * 転送先のメモリの32byteでアライメントされた先頭アドレスへのポインタ
     * @param srcTag
     * @param numBlocks
     */
    static void StoreBlocks(void* destAddr, void* srcTag, u32 numBlocks);
    static u32 StoreData(void* pDst, void* pSrc, u32 size);

    static void* GetBase() {
        return reinterpret_cast<void*>(OS_CACHE_BASE);
    }

    static void QueueWait(u32 len) {
        LCQueueWait(len);
    }

    static void QueueWaitEx(u32 len) {
        while (LCQueueLength() != len) {
            OSYieldThread();
        }
    }
};
} // namespace ut
} // namespace nw4r

#endif
