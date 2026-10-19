#include <Pack/RPKernel.h>
#include <Pack/RPSystem.h>

#include <egg/core.h>

#include <nw4r/lyt.h>

/******************************************************************************
 *
 * InternalAccessor
 *
 ******************************************************************************/

/**
 * \~english @brief Constructor
 */
RPSysLytResAccessor::InternalAccessor::InternalAccessor() {}

/**
 * \~english @brief Gets the specified resource from the currently open archive
 * @details Font queries are redirected to the font manager
 *
 * \~english @param type Resource type
 * \~english @param pName Resource name
 * \~english @param[out] pSize Resource data size
 */
void* RPSysLytResAccessor::InternalAccessor::GetResource(u32 type,
                                                         const char* pName,
                                                         u32* pSize) {
    switch (type) {
    case RES_TYPE_FONT: {
        return RP_GET_INSTANCE(RPSysFontManager)->GetResFontData(pName);
        break;
    }

    default: {
        return nw4r::lyt::ArcResourceAccessor::GetResource(type, pName, pSize);
    }
    }
}

/******************************************************************************
 *
 * RPSysLytResAccessor
 *
 ******************************************************************************/

/**
 * \~english @brief Constructor
 */
RPSysLytResAccessor::RPSysLytResAccessor() : mpImpl(NULL) {}

/**
 * \~english @brief Creates a resource accessor from the specified heap
 *
 * \~english @param pHeap Heap to use for allocations
 */
RPSysLytResAccessor* RPSysLytResAccessor::create(EGG::Heap* pHeap) {
    if (pHeap == NULL) {
        pHeap = EGG::Heap::getCurrentHeap();
    }

    RPSysLytResAccessor* p = new (pHeap) RPSysLytResAccessor();

    if (p != NULL) {
        p->mpImpl = new (pHeap) InternalAccessor();
    }

    return p;
}

/**
 * \~english @brief Attempts to mount an archive
 *
 * \~english @param pBinary Archive binary data
 * \~english @param pRootDir Root directory to open (optional)
 * \~english @return Success
 */
bool RPSysLytResAccessor::mountArchive(void* pBinary, const char* pRootDir) {
    bool success = false;

    if (!mpImpl->IsAttached()) {
        if (pRootDir == NULL) {
            pRootDir = "";
        }

        success = mpImpl->Attach(pBinary, pRootDir);
    }

    return success;
}
