#include <Pack/RPKernel.h>
#include <Pack/RPSystem.h>

#include <RVLFaceLib.h>

#include <egg/core.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param num Mii buffer capacity
 * \~english @param pHeap Heap to use for allocations
 */
RPSysAppMiiManager::RPSysAppMiiManager(u16 num, EGG::Heap* pHeap)
    : mNumData(num) {
    //! Private variables require out-of-declaration setting
    //! because ProjectLocal is accessed here as well.
    RPSysProjectLocal* local = RP_GET_INSTANCE(RPSysProjectLocal);
    mppStoreData = NULL;
    mppCharData = NULL;

    mppStoreData = new (pHeap) RFLStoreData*[num];
}

/**
 * \~english @brief Destructor
 */
//! Should be defined here: wrong order otherwise
RPSysAppMiiManager::~RPSysAppMiiManager() {}
