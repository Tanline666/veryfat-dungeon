#include <Pack/RPGraphics.h>
#include <Pack/RPKernel.h>

#include <RVLFaceLib.h>

#include <egg/core.h>

#include <nw4r/ut.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param pCallback Controller data callback
 * \~english @param pParentHeap Heap to use as the parent of the controller data
 * heap
 * \~english @param heapSize Size of the controller data heap
 * \~english @param chan Remote channel
 */
RPSysKokeshiCtrlDataMgr::RPSysKokeshiCtrlDataMgr(
    IRPSysKokeshiCtrlDataCallback* pCallback, EGG::Heap* pParentHeap,
    u32 heapSize, s32 chan) {

    mHeapSize = heapSize;

    mpLoader = RP_GET_INSTANCE(RPSysKokeshiManager)->GetCtrlDataLoader(chan);
    mpIterator = new RPSysKokeshiIterater(
        static_cast<RFLDataSource>(RFLDataSource_Controller1 + chan));

    mpCallback = pCallback;
    mpParentHeap = pParentHeap;

    mpHeap = EGG::ExpHeap::create(mHeapSize, mpParentHeap);
    mpAllocator = new (mpHeap) EGG::Allocator(mpHeap);

    NW4R_UT_LIST_INIT(mResourceList, FileResource);
    NW4R_UT_LIST_INIT(mModelList, ModelResource);

    mState = EState_Idle;
}

/**
 * \~english @brief Destructor
 */
RPSysKokeshiCtrlDataMgr::~RPSysKokeshiCtrlDataMgr() {}

/**
 * \~english @brief Notifies the user that all resources will be destroyed
 */
void RPSysKokeshiCtrlDataMgr::notifyReleaseResource() {
    if (mState != EState_Idle) {
        return;
    }

    mState = EState_Release;

    for (u8 i = 0; i < RFL_CTRL_CHAR_MAX; i++) {
        mpCallback->onReleaseResource(mpLoader->getChannel(), i);
    }
}

/**
 * \~english @brief Resets the internal state for creating resources
 */
void RPSysKokeshiCtrlDataMgr::resetResource() {
    if (mState != EState_Idle) {
        return;
    }

    if (mpLoader->isError() == TRUE) {
        mState = EState_Idle;
    }

    mState = EState_Create;
    mpIterator->Reset();
}

/**
 * \~english @brief Tests whether the manager is in an idle state
 */
BOOL RPSysKokeshiCtrlDataMgr::isIdle() const {
    return mState == EState_Idle ? TRUE : FALSE;
}

/**
 * \~english @brief Tests whether an error occured while loading data
 */
BOOL RPSysKokeshiCtrlDataMgr::isLoadError() const {
    return mpLoader->isError();
}

/**
 * \~english @brief Creates resources for the next Mii in the database
 */
void RPSysKokeshiCtrlDataMgr::createResource() {
    if (mState != EState_Create) {
        return;
    }

    EGG::Heap* pOldHeap = EGG::Heap::getCurrentHeap();
    EGG::Allocator* pOldAllocator = RPGrpModel::GetAllocator();

    // Take ownership of any graphics allocations in the callback
    mpHeap->becomeCurrentHeap();
    RPGrpModel::SetAllocator(mpAllocator);

    if (mpIterator->Update()) {
        mpCallback->onCreateResource(mpLoader->getChannel(),
                                     mpIterator->GetIndex());
    } else {
        mState = EState_Idle;
    }

    // Restore original management
    RPGrpModel::SetAllocator(pOldAllocator);
    pOldHeap->becomeCurrentHeap();
}

/**
 * \~english @brief Appends a new file to the file resource list
 *
 * \~english @param handle Resource handle
 */
void RPSysKokeshiCtrlDataMgr::appendFile(RPGrpHandle handle) {
    FileResource* pResFile = new (mpHeap) FileResource();
    pResFile->handle = handle;
    nw4r::ut::List_Append(&mResourceList, pResFile);
}

/**
 * \~english @brief Appends a new model to the model resource list
 *
 * \~english @param pModel New model
 */
void RPSysKokeshiCtrlDataMgr::appendModel(RPGrpModel* pModel) {
    ModelResource* pResModel = new (mpHeap) ModelResource();
    pResModel->pModel = pModel;
    nw4r::ut::List_Append(&mModelList, pResModel);
}

/**
 * \~english @brief Updates the manager state
 */
void RPSysKokeshiCtrlDataMgr::calc() {
    mpLoader->calc();

    switch (mState) {
    case EState_Idle: {
        break;
    }

    case EState_Release: {
        // Destroy logic happens outside of this function
        mState = EState_Reset;
        break;
    }

    case EState_Reset: {
        resetData();
        break;
    }

    case EState_Load: {
        if (!mpLoader->isIdle()) {
            break;
        }

        if (mpLoader->isError() == TRUE) {
            mState = EState_Idle;
        }

        mState = EState_Create;
        mpIterator->Reset();
        break;
    }

    case EState_Create: {
        break;
    }
    }
}

/**
 * \~english @brief Releases resources and attempts to reload controller Mii
 * data
 */
void RPSysKokeshiCtrlDataMgr::resetData() {
    RPGrpModelResManager* pResManager = RPGrpModelResManager::GetCurrent();

    NW4R_UT_LIST_FOREACH_SAFE (FileResource, it, mResourceList, {
        pResManager->Terminate(it->handle);
        nw4r::ut::List_Remove(&mResourceList, it);
    })

    NW4R_UT_LIST_FOREACH_SAFE (ModelResource, it, mModelList, {
        it->pModel->RemoveGenList();
        nw4r::ut::List_Remove(&mModelList, it);
    })

    if (mpHeap != NULL) {
        mpHeap->destroy();
    }

    mpHeap = NULL;
    mpAllocator = NULL;

    mpHeap = EGG::ExpHeap::create(mHeapSize, mpParentHeap);
    mpAllocator = new (mpHeap) EGG::Allocator(mpHeap);

    mState = EState_Load;
    mpLoader->load();
}
