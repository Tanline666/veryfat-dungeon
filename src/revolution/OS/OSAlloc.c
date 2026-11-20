#include <revolution/OS.h>

#define OFFSET(n, a) (((u32)(n)) & ((a) - 1))

typedef struct OSHeapDescriptor {
    s32 size;                    // at 0x0
    struct OSHeapCell* freeList; // at 0x4
    struct OSHeapCell* usedList; // at 0x8
#if defined(DEBUG)
    u32 paddingSize;
    u32 headerSize;
    u32 payloadSize;
#endif
} OSHeapDescriptor;

typedef struct OSHeapCell {
    struct OSHeapCell* prev;     // at 0x0
    struct OSHeapCell* next;     // at 0x4
    s32 size;                    // at 0x8
    struct OSHeapDescriptor* hd; // at 0xC
    s32 usedSize;                // at 0x10
    char UNK_0x14[0x20 - 0x14];
} OSHeapCell;

volatile s32 __OSCurrHeap = -1;

static void* ArenaEnd = NULL;
static void* ArenaStart = NULL;
static s32 NumHeaps = 0;
static OSHeapDescriptor* HeapArray = NULL;

static OSHeapCell* DLAddFront(OSHeapCell* list, OSHeapCell* child) {
    child->next = list;
    child->prev = NULL;

    if (list != NULL) {
        list->prev = child;
    }

    return child;
}

static OSHeapCell* DLExtract(OSHeapCell* list, OSHeapCell* child) {
    if (child->next != NULL) {
        child->next->prev = child->prev;
    }

    if (child->prev == NULL) {
        return child->next;
    }

    child->prev->next = child->next;
    return list;
}

static OSHeapCell* DLInsert(OSHeapCell* list, OSHeapCell* child) {
    OSHeapCell* prev = NULL;
    OSHeapCell* next = list;

    for (; next != NULL; prev = next, next = next->next) {
        if (child <= next) {
            break;
        }
    }

    child->next = next;
    child->prev = prev;

    if (next != NULL) {
        next->prev = child;

        if ((u8*)child + child->size == (u8*)next) {
            child->size += next->size;
            next = next->next;
            child->next = next;
            if (next != NULL) {
                next->prev = child;
            }
        }
    }

    if (prev != NULL) {
        prev->next = child;

        if ((u8*)prev + prev->size == (u8*)child) {
            prev->size += child->size;
            prev->next = next;
            if (next != NULL) {
                next->prev = prev;
            }
        }

        return list;
    } else {
        return child;
    }
}

static BOOL DLOverlap(OSHeapCell* pList, void* pStart, void* pEnd) {
    OSHeapCell* pCell;

    for (pCell = pList; pCell; pCell = pCell->next) {
        if ((u32)pStart <= (u32)pCell &&
            (u32)(s8*)pCell + pCell->size <= (u32)pEnd) {
            return TRUE;
        }
    }
    return FALSE;
}

static OSHeapCell* DLLookup(OSHeapCell* pList, OSHeapCell* pCell) {
    for (; pList; pList = pList->next) {
        if (pList == pCell) {
            return pList;
        }
    }

    return NULL;
}

void* OSAllocFromHeap(s32 handle, s32 size) {
    OSHeapDescriptor* hd = &HeapArray[handle];
    OSHeapCell* cell;
    u32 avail;
#if defined(DEBUG)
    s32 usedSize = size;

    OS_ASSERT(HeapArray, "OSAllocFromHeap(): heap is not initialized.");
    OS_ASSERT(0 < size, "OSAllocFromHeap(): invalid size.");
    OS_ASSERT(0 <= handle && handle < NumHeaps, "OSAllocFromHeap(): invalid heap handle.");
    OS_ASSERT(0 <= HeapArray[handle].size, "OSAllocFromHeap(): invalid heap handle.");
#endif

    hd = &HeapArray[handle];
    size = ROUND_UP(size + sizeof(OSHeapCell), 32);

    for (cell = hd->freeList; cell != NULL; cell = cell->next) {
        if (size <= cell->size) {
            break;
        }
    }

    if (cell == NULL) {
#if defined(DEBUG)
        OSReport("OSAllocFromHeap: Warning- failed to allocate %d bytes\n",
                 size);
#endif
        return NULL;
    }

#if defined(DEBUG)
    OS_ASSERT(!OFFSET(cell, 32), "OSAllocFromHeap(): heap is broken.");
    OS_ASSERT(!cell->hd, "OSAllocFromHeap(): heap is broken.");
#endif

    avail = cell->size - size;
    if (avail < 64) {
        hd->freeList = DLExtract(hd->freeList, cell);
    } else {
        OSHeapCell* adj;
        cell->size = size;

        adj = (OSHeapCell*)((u8*)cell + size);
        adj->size = avail;
#if defined(DEBUG)
        adj->hd = NULL;
#endif
        adj->prev = cell->prev;
        adj->next = cell->next;

        if (adj->next != NULL) {
            adj->next->prev = adj;
        }

        if (adj->prev != NULL) {
            adj->prev->next = adj;
        } else {
#if defined(DEBUG)
            OS_ASSERT(hd->free == cell, "OSAllocFromHeap(): heap is broken.");
#endif
            hd->freeList = adj;
        }
    }

    hd->usedList = DLAddFront(hd->usedList, cell);
#if defined(DEBUG)
    cell->hd = hd;
    cell->usedSize = usedSize;
    hd->headerSize += sizeof(OSHeapCell);
    hd->paddingSize += cell->size - (sizeof(OSHeapCell) + usedSize);
    hd->payloadSize += usedSize;
#endif
    return (u8*)cell + sizeof(OSHeapCell);
}

void OSFreeToHeap(s32 handle, void* p) {
#if defined(DEBUG)
    OS_ASSERT(HeapArray, "OSFreeToHeap(): heap is not initialized.");
    OS_ASSERT(((s8*) ArenaStart + sizeof(OSHeapCell)) <= p < ((s8*)ArenaEnd), "OSFreeToHeap(): invalid pointer.");
    OS_ASSERT(!OFFSET(p, 32), "OSFreeToHeap(): invalid pointer.");
    OS_ASSERT(0 <= HeapArray[handle].size, "OSFreeToHeap(): invalid heap handle.");
#endif

    OSHeapDescriptor* hd = &HeapArray[handle];
    OSHeapCell* cell = (OSHeapCell*)((u8*)p - sizeof(OSHeapCell));

#if defined(DEBUG)
    OS_ASSERT(cell->hd == hd, "OSFreeToHeap(): invalid pointer.");
    OS_ASSERT(DLLookup(hd->usedList, cell), "OSFreeToHeap(): invalid pointer.");

    cell->hd = NULL;
    hd->headerSize -= sizeof(OSHeapCell);
    hd->paddingSize -= cell->size - (sizeof(OSHeapCell) + cell->usedSize);
    hd->payloadSize -= cell->usedSize;
#endif

    hd->usedList = DLExtract(hd->usedList, cell);
    hd->freeList = DLInsert(hd->freeList, cell);
}

s32 OSSetCurrentHeap(s32 handle) {
#if defined(DEBUG)
    OS_ASSERT(HeapArray, "OSSetCurrentHeap(): heap is not initialized.");
    OS_ASSERT(0 <= handle && handle < NumHeaps, "OSSetCurrentHeap(): invalid heap handle.");
    OS_ASSERT(0 <= HeapArray[handle].size, "OSSetCurrentHeap(): invalid heap handle.");
#endif
    s32 old = __OSCurrHeap;
    __OSCurrHeap = handle;
    return old;
}

void* OSInitAlloc(void* start, void* end, s32 numHeaps) {
    u32 headerSize;
    int i;

    headerSize = numHeaps * sizeof(OSHeapDescriptor);
    HeapArray = (OSHeapDescriptor*)start;
    NumHeaps = numHeaps;

    for (i = 0; i < NumHeaps; i++) {
        OSHeapDescriptor* hd = &HeapArray[i];
        hd->size = -1;
        hd->usedList = NULL;
        hd->freeList = NULL;
#if defined(DEBUG)
        hd->paddingSize = hd->headerSize = hd->payloadSize = NULL;
#endif
    }

    __OSCurrHeap = -1;
    ArenaStart = ROUND_UP_PTR((u8*)HeapArray + headerSize, 32);
    ArenaEnd = ROUND_DOWN_PTR(end, 32);

    return ArenaStart;
}

s32 OSCreateHeap(void* start, void* end) {
    s32 handle;

#if defined(DEBUG)
    OS_ASSERT(HeapArray, "OSCreateHeap(): heap is not initialized.");
    OS_ASSERT(start < end, "OSCreateHeap(): invalid range.");
#endif
    start = ROUND_UP_PTR(start, 32);
    end = ROUND_DOWN_PTR(end, 32);
#if defined(DEBUG)
    OS_ASSERT(start < end, "OSCreateHeap(): invalid range.");
    OS_ASSERT((u32)ArenaStart <= (u32)start && (u32)end <= (u32)ArenaEnd, "OSCreateHeap(): invalid range.");
    OS_ASSERT(sizeof(OSHeapCell) + 32 <= (s8*)end - (s8*)start, "OSCreateHeap(): too small range.");

    for (handle = 0; handle < NumHeaps; handle++) {
        if (HeapArray[handle].size < 0) {
            continue;
        }
        OS_ASSERT(!DLOverlap(HeapArray[handle].freeList, start, end), "OSCreateHeap(): invalid range.");
        OS_ASSERT(!DLOverlap(HeapArray[handle].usedList, start, end), "OSCreateHeap(): invalid range.");
    }
#endif

    for (handle = 0; handle < NumHeaps; handle++) {
        OSHeapCell* cell = (OSHeapCell*)start;
        OSHeapDescriptor* hd = &HeapArray[handle];
        if (hd->size < 0) {
            hd->size = (uintptr_t)end - (uintptr_t)start;

            cell->prev = NULL;
            cell->next = NULL;
            cell->size = hd->size;
#if defined(DEBUG)
            cell->hd = NULL;
#endif
            hd->freeList = cell;
            hd->usedList = NULL;
#if defined(DEBUG)
            hd->paddingSize = hd->headerSize = hd->payloadSize = NULL;
#endif
            return handle;
        }
    }

#if defined(DEBUG)
    OSReport("OSCreateHeap: Warning - Failed to find free heap descriptor.");
#endif

    return -1;
}
