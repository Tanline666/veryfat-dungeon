#include <Pack/RPGraphics.h>
#include <Pack/RPKernel.h>

#include <egg/core.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param pCallback Controller data callback
 * \~english @param pParentHeap Heap to use as the parent of the controller data
 * heap
 * \~english @param heapSize Size of the controller data heap
 */
RPSysKokeshiCtrlMgr::RPSysKokeshiCtrlMgr(
    IRPSysKokeshiCtrlDataCallback* pCallback, EGG::Heap* pParentHeap,
    u32 heapSize)
    : unk10(NULL) {

    for (u8 i = 0; i < ARRAY_SIZE(mpDataMgrs); i++) {
        mpDataMgrs[i] =
            new RPSysKokeshiCtrlDataMgr(pCallback, pParentHeap, heapSize, i);
    }

    unk10 = this;
    RP_GET_INSTANCE(RPSysKokeshiManager)->SetCtrlMgr(this);
}

/**
 * \~english @brief Destructor
 */
RPSysKokeshiCtrlMgr::~RPSysKokeshiCtrlMgr() {}

/**
 * \~english @brief Notifies the user that all resources will be destroyed
 *
 * \~english @param chan WPAD channel of the data manager
 */
void RPSysKokeshiCtrlMgr::notifyReleaseResource(s32 chan) {
    mpDataMgrs[chan]->notifyReleaseResource();
}

/**
 * \~english @brief Resets the internal state for creating resources
 *
 * \~english @param chan WPAD channel of the data manager
 */
void RPSysKokeshiCtrlMgr::resetResource(s32 chan) {
    mpDataMgrs[chan]->resetResource();
}

/**
 * \~english @brief Tests whether the specified data manager is in an idle state
 *
 * \~english @param chan WPAD channel of the data manager
 */
BOOL RPSysKokeshiCtrlMgr::isIdle(s32 chan) const {
    return mpDataMgrs[chan]->isIdle();
}

/**
 * \~english @brief Tests whether an error occured while loading data
 *
 * \~english @param chan WPAD channel of the data manager
 */
BOOL RPSysKokeshiCtrlMgr::isLoadError(s32 chan) const {
    return mpDataMgrs[chan]->isLoadError();
}

/**
 * \~english @brief Creates resources for the next Mii in each database
 */
void RPSysKokeshiCtrlMgr::createResource() {
    for (u8 i = 0; i < ARRAY_SIZE(mpDataMgrs); i++) {
        mpDataMgrs[i]->createResource();
    }
}

/**
 * \~english @brief Gets the heap used for resource allocation by the specified
 * data manager
 *
 * \~english @param chan WPAD channel of the data manager
 */
EGG::Heap* RPSysKokeshiCtrlMgr::getResourceHeap(s32 chan) {
    return mpDataMgrs[chan]->getHeap();
}

/**
 * \~english @brief Appends a new file to the file resource list
 *
 * \~english @param chan WPAD channel of the data manager
 * \~english @param handle Resource handle
 */
void RPSysKokeshiCtrlMgr::appendFile(s32 chan, RPGrpHandle handle) {
    mpDataMgrs[chan]->appendFile(handle);
}

/**
 * \~english @brief Appends a new model to the model resource list
 *
 * \~english @param chan WPAD channel of the data manager
 * \~english @param pModel New model
 */
void RPSysKokeshiCtrlMgr::appendModel(s32 chan, RPGrpModel* pModel) {
    mpDataMgrs[chan]->appendModel(pModel);
}

/**
 * \~english @brief Updates each data manager's state
 */
void RPSysKokeshiCtrlMgr::calc() {
    for (u8 i = 0; i < ARRAY_SIZE(mpDataMgrs); i++) {
        mpDataMgrs[i]->calc();
    }
}
