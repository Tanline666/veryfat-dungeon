#ifndef EGG_CORE_UNIT_HEAP_H
#define EGG_CORE_UNIT_HEAP_H
#include <egg/types_egg.h>

#include <egg/core/eggDisposer.h>
#include <egg/core/eggHeap.h>

namespace EGG {

class Allocator;

class UnitHeap : Heap {
public:
    UnitHeap(MEMiHeapHead* heapHead) : Heap(heapHead) {}
    static UnitHeap* create(void* block, u32 size, u32 unit_size, s32 align,
                            u16 flag);
    static UnitHeap* create(u32 size, u32 unit_size, EGG::Heap* heap, s32 align,
                            u16 flag);
    virtual ~UnitHeap(); //!< // at 0x8

    virtual EHeapKind getHeapKind() const {
        return HEAP_KIND_UNIT;
    } //!< // at 0xC

    virtual void initAllocator(Allocator* allocator, s32); //!< // at 0x10

    virtual void* alloc(u32 size, s32 align); //!< // at 0x14
    virtual void free(void* block);           //!< // at 0x18
    virtual void destroy();                   //!< // at 0x1C

    virtual u32 resizeForMBlock(void* pBlock, u32 size); //!< // at 0x20
    virtual u32 getAllocatableSize(s32 align);
    virtual u32 adjust();
    static u32 calcHeapSize(u32 unit_size, u32 unit_count, s32 align);
};

} // namespace EGG

#endif
