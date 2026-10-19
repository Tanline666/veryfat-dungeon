#ifndef RP_KERNEL_LYT_ALLOCATOR_H
#define RP_KERNEL_LYT_ALLOCATOR_H
#include <Pack/types_pack.h>

#include <egg/core.h>

#include <revolution/MEM.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Layout memory allocator
 */
class RPSysLytAllocator : public EGG::Allocator {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pHeap Heap to use for allocations
     * \~english @param align Required byte alignment for allocations
     */
    RPSysLytAllocator(EGG::Heap* pHeap, s32 align = 4);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysLytAllocator() override; // at 0x8

private:
    //! Custom allocator functions
    MEMAllocatorFuncs mExFuncs; // at 0x14
};

//! @}

#endif
