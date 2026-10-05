#ifndef RVL_SDK_MEM_UNIT_HEAP_H
#define RVL_SDK_MEM_UNIT_HEAP_H
#include <types.h>

#include <revolution/MEM/mem_heapCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

//! So the below struct will look cleaner
typedef struct MEMiUntHeapMBlockHead MEMiUntHeapMBlockHead;

struct MEMiUntHeapMBlockHead {
    MEMiUntHeapMBlockHead* pNextHead; // at 0x0
};

typedef struct MEMiUntMBlockList {
    MEMiUntHeapMBlockHead* pHead; // at 0x0
} MEMiUntMBlockList;

typedef struct MEMiUntHeapHead {
    MEMiUntMBlockList mbList; // at 0x0
    u32 mbSize;               // at 0x4
} MEMiUntHeapHead;

MEMiHeapHead* MEMCreateUnitHeapEx(void* pStart, u32 heapSize, u32 mbSize,
                                  s32 align, u16 opt);
void* MEMAllocFromUnitHeap(MEMiHeapHead* pHeap);
void MEMFreeToUnitHeap(MEMiHeapHead* pHeap, void* pMemBlock);

static inline u32 MEMGetMemBlockSizeForUnitHeap(MEMiHeapHead* pHeap) {
    return (((const MEMiUntHeapHead*)((const u8*)pHeap + sizeof(MEMiHeapHead)))
                ->mbSize);
}

#ifdef __cplusplus
}
#endif
#endif
