#include <egg/core.h>

namespace EGG {

enum {
    UNIT_HEAP_BASE_SIZE = (sizeof(UnitHeap) - sizeof(MEMiUntHeapHead)),
    UNIT_HEAP_ALIGN = 4
};

UnitHeap::~UnitHeap() {
    dispose();
    MEMDestroyUnitHeap(mHeapHandle);
}

UnitHeap* UnitHeap::create(void* pHeapStart, u32 size, u32 unit_size, s32 align,
                           u16 flag) {

    UnitHeap* pHeap = NULL;

    void* pHeapEnd = ROUND_DOWN_PTR(addOffset(pHeapStart, size), 4);
    pHeapStart = ROUND_UP_PTR(pHeapStart, 4);

    if (pHeapStart > pHeapEnd ||
        nw4r::ut::GetOffsetFromPtr(pHeapStart, pHeapEnd) <
            sizeof(UnitHeap) + 4) {
        pHeap = NULL;
    } else {
        MEMiHeapHead* pHeapHandle = MEMCreateUnitHeapEx(
            addOffset(pHeapStart, sizeof(UnitHeap)),
            nw4r::ut::GetOffsetFromPtr(pHeapStart, pHeapEnd) - sizeof(UnitHeap),
            unit_size, align, flag);

        if (pHeapHandle) {
            Heap* pContainHeap = Heap::findContainHeap(pHeapStart);

            pHeap = new (pHeapStart) UnitHeap(pHeapHandle);
            pHeap->mParentHeap = pContainHeap;
        }
    }

    return pHeap;
}

UnitHeap* UnitHeap::create(u32 size, u32 unit_size, EGG::Heap* pParentHeap,
                           s32 align, u16 flag) {
    UnitHeap* ret = NULL;

    if (!pParentHeap) {
        pParentHeap = getCurrentHeap();
    }
    if (size == -1) {
        size = pParentHeap->getAllocatableSize();
    }

    void* block = pParentHeap->alloc(size, UNIT_HEAP_ALIGN);
    if (block) {
        if ((ret = create(block, size, unit_size, align, flag)) != NULL)
            ret->mParentHeap = pParentHeap;
        else
            pParentHeap->free(block);
    }

    return ret;
}

void UnitHeap::destroy() {
    Heap* pParentHeap = findParentHeap();

    this->~UnitHeap();
    if (pParentHeap != NULL) {
        pParentHeap->free(this);
    }
}

void* UnitHeap::alloc(u32 size, s32 align) {
#pragma unused(align)
    //! Kudos to wii-ipl!
    if (size > MEMGetMemBlockSizeForUnitHeap(mHeapHandle)) {
        return NULL;
    } else {
        return MEMAllocFromUnitHeap(mHeapHandle);
    }
}

void UnitHeap::free(void* pBlock) {
    MEMFreeToUnitHeap(mHeapHandle, pBlock);
}

u32 UnitHeap::resizeForMBlock(void* pBlock, u32 size) {
#pragma unused(pBlock, size)
    return 0;
}

u32 UnitHeap::getAllocatableSize(s32 align) {
#pragma unused(align)
    return 0;
}

u32 UnitHeap::adjust() {
    return 0;
}

void UnitHeap::initAllocator(Allocator* pAllocator, s32 align) {
#pragma unused(align)
    MEMInitAllocatorForUnitHeap(pAllocator, mHeapHandle);
}

} // namespace EGG
