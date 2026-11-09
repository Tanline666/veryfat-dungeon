#include <nw4r/snd.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace snd {
namespace detail {

AxfxImpl* AxfxImpl::mCurrentFx = NULL;
u32 AxfxImpl::mAllocatedSize = 0;

bool AxfxImpl::CreateHeap(void* pBuffer, u32 size) {
    if (!pBuffer || !size) {
        mHandle = NULL;
        return FALSE;
    }

    mHandle = MEMCreateFrmHeap(pBuffer, size);
    return mHandle != NULL;
}

void AxfxImpl::DestroyHeap() {
    if (mHandle != NULL) {
        MEMDestroyFrmHeap(mHandle);
        mHandle = NULL;
    }
}

void AxfxImpl::HookAlloc(AXFXAllocHook* pAllocHook, AXFXFreeHook* pFreeHook) {
    AXFXGetHooks(pAllocHook, pFreeHook);
    AXFXSetHooks(Alloc, Free);
    mCurrentFx = this;
    mAllocatedSize = 0;
}

u32 AxfxImpl::RestoreAlloc(AXFXAllocHook allocHook, AXFXFreeHook freeHook) {
    AXFXSetHooks(allocHook, freeHook);
    mCurrentFx = NULL;
    return mAllocatedSize;
}

void* AxfxImpl::Alloc(u32 size) {
    NW4R_NULL_ASSERT(mCurrentFx, 136);
    NW4R_ASSERT(mCurrentFx->mHandle != MEM_HEAP_INVALID_HANDLE, 137);

    void* addr = MEMAllocFromFrmHeap(mCurrentFx->mHandle, size);

    mCurrentFx->mAllocCounter++;
    mAllocatedSize += ut::RoundUp(size, 4);

    return addr;
}

void AxfxImpl::Free(void* pBlock) {
#pragma unused(pBlock)

    if (mCurrentFx->mAllocCounter != 0) {
        mCurrentFx->mAllocCounter--;
    }

    if (mCurrentFx->mAllocCounter == 0) {
        MEMFreeToFrmHeap(mCurrentFx->mHandle, MEM_FRM_HEAP_FREE_ALL);
    }
}

} // namespace detail
} // namespace snd
} // namespace nw4r
