#include <Pack/RPKernel.h>
#include <Pack/RPSystem.h>

#include <egg/core.h>

#include <nw4r/lyt.h>

#include <revolution/MEM.h>

/**
 * \~english @brief Disables freeing layout memory in favor of garbage
 * collection
 *
 * \~english @param pAllocator Memory allocator
 * \~english @param pBlock Memory block
 */
static void FreeMemory(MEMAllocator* /* pAllocator */, void* /* pBlock */) {}

/**
 * \~english @brief Constructor
 *
 * \~english @param pHeap Heap to use for allocations
 * \~english @param align Required byte alignment for allocations
 */
RPSysLytAllocator::RPSysLytAllocator(EGG::Heap* pHeap, s32 align)
    : EGG::Allocator(pHeap, align) {

    mExFuncs.allocFunc = funcs->allocFunc;
    mExFuncs.freeFunc = FreeMemory;

    // Redirect base allocator
    funcs = &mExFuncs;
}

/**
 * \~english @brief Destructor
 */
RPSysLytAllocator::~RPSysLytAllocator() {}
