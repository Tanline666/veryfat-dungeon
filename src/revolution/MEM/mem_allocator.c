#include <revolution/MEM.h>

static void* AllocatorAllocForExpHeap_(MEMAllocator* pAllocator, u32 size) {
    return MEMAllocFromExpHeapEx(pAllocator->heap, size,
                                 pAllocator->heapParam1);
}

static void AllocatorFreeForExpHeap_(MEMAllocator* pAllocator, void* block) {
    MEMFreeToExpHeap(pAllocator->heap, block);
}

static void* AllocatorAllocForFrmHeap_(MEMAllocator* pAllocator, u32 size) {
    return MEMAllocFromFrmHeapEx(pAllocator->heap, size,
                                 pAllocator->heapParam1);
}

static void AllocatorFreeForFrmHeap_(MEMAllocator* pAllocator, void* block) {
#pragma unused(pAllocator)
#pragma unused(block)
}

static void* AllocatorAllocForUnitHeap_(MEMAllocator* pAllocator, u32 size) {
    if (size > MEMGetMemBlockSizeForUnitHeap(pAllocator->heap)) {
        return NULL;
    }
    return MEMAllocFromUnitHeap(pAllocator->heap);
}

static void AllocatorFreeForUnitHeap_(MEMAllocator* pAllocator, void* pBlock) {
    MEMFreeToUnitHeap(pAllocator->heap, pBlock);
}

void* MEMAllocFromAllocator(MEMAllocator* pAllocator, u32 size) {
    return pAllocator->funcs->allocFunc(pAllocator, size);
}

void MEMFreeToAllocator(MEMAllocator* pAllocator, void* block) {
    pAllocator->funcs->freeFunc(pAllocator, block);
}

void MEMInitAllocatorForExpHeap(MEMAllocator* pAllocator, MEMiHeapHead* pHeap,
                                s32 align) {
    static const MEMAllocatorFuncs sAllocatorFunc = {AllocatorAllocForExpHeap_,
                                                     AllocatorFreeForExpHeap_};
    pAllocator->funcs = &sAllocatorFunc;
    pAllocator->heap = pHeap;
    pAllocator->heapParam1 = align;
    pAllocator->heapParam2 = NULL;
}

void MEMInitAllocatorForFrmHeap(MEMAllocator* pAllocator, MEMiHeapHead* pHeap,
                                s32 align) {
    static const MEMAllocatorFuncs sAllocatorFunc = {AllocatorAllocForFrmHeap_,
                                                     AllocatorFreeForFrmHeap_};
    pAllocator->funcs = &sAllocatorFunc;
    pAllocator->heap = pHeap;
    pAllocator->heapParam1 = align;
    pAllocator->heapParam2 = NULL;
}

void MEMInitAllocatorForUnitHeap(MEMAllocator* pAllocator,
                                 MEMiHeapHead* pHeap) {
    static const MEMAllocatorFuncs sAllocatorFunc = {AllocatorAllocForUnitHeap_,
                                                     AllocatorFreeForUnitHeap_};
    pAllocator->funcs = &sAllocatorFunc;
    pAllocator->heap = pHeap;
    pAllocator->heapParam1 = NULL;
    pAllocator->heapParam2 = NULL;
}
