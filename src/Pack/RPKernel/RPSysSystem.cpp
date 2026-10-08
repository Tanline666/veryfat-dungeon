#include <Pack/RPKernel.h>
#include <Pack/RPSystem.h>

#include <egg/core.h>
#include <egg/gfxe.h>

#include <revolution/OS.h>
#include <revolution/SC.h>
#include <revolution/VI.h>

void RPSysSystem::initialize() {
    EGG::BaseSystem::configure((EGG::ConfigurationData*)&sConfigData);
    DVDInit();

    //! The below is pretty much ripped from EGG::ConfigurationData::initMemory.
    //! The order of some items has changed, which is why it is not just
    //! inlined.

    void* pMem1Lo = OSGetMEM1ArenaLo();
    void* pMem1Hi = OSGetMEM1ArenaHi();
    void* pMem2Lo = OSGetMEM2ArenaLo();
    void* pMem2Hi = OSGetMEM2ArenaHi();

    void* pMem1Arena = OSInitAlloc(pMem1Lo, pMem1Hi, 2);
    void* pMem2Arena = OSInitAlloc(pMem2Lo, pMem2Hi, 2);

    sConfigData.mCodeEnd = ROUND_UP_PTR(pMem1Arena, 32);
    sConfigData.mCodeStart =
        static_cast<OSBootInfo*>(OSPhysicalToCached(OS_PHYS_BOOT_INFO));
    sConfigData.mMem1Start = ROUND_UP_PTR(pMem1Arena, 32);
    sConfigData.mMem1End = ROUND_DOWN_PTR(pMem1Hi, 32);
    sConfigData.mMem2Start = ROUND_UP_PTR(pMem2Arena, 32);
    sConfigData.mMem2End = ROUND_DOWN_PTR(pMem2Hi, 32);
    sConfigData.mMemSize = sConfigData.mCodeStart->physMemSize;

    OSSetMEM1ArenaLo(pMem1Arena);
    OSSetMEM1ArenaHi(pMem1Arena);
    OSSetMEM2ArenaLo(pMem2Arena);
    OSSetMEM2ArenaHi(pMem2Arena);

    EGG::Heap::initialize();
    u32 heapSizeMem1 = nw4r::ut::GetOffsetFromPtr(sConfigData.mMem1Start,
                                                  sConfigData.mMem1End);
    sConfigData.mRootHeapMem1 = EGG::ExpHeap::create(pMem1Lo, heapSizeMem1);
    u32 heapSizeMem2 = nw4r::ut::GetOffsetFromPtr(sConfigData.mMem2Start,
                                                  sConfigData.mMem2End);
    sConfigData.mRootHeapMem2 = EGG::ExpHeap::create(pMem2Lo, heapSizeMem2);
    sConfigData.mRootHeapDebug = NULL; // unlike initMemory, this is hardcoded
    sConfigData.mSystemHeap = EGG::ExpHeap::create(
        sConfigData.mSystemHeapSize, sConfigData.mRootHeapMem1, 0);
    sConfigData.mSystemHeap->becomeCurrentHeap();
    EGG::GraphicsFifo::create(0x80000u, NULL);
    setupTVMode();
    setupRenderMode();

    sConfigData.mVideo = new EGG::Video(spRenderModeObj);
    sConfigData.mXfbMgr = new EGG::XfbManager;
    for (int i = 0; i < 2; ++i) {
        sConfigData.mXfbMgr->attach(new EGG::Xfb(sConfigData.mRootHeapMem2));
    }
    sConfigData.mDisplay = new EGG::AsyncDisplay(1);
    EGG::Thread::initialize();
    sConfigData.mCreatorThread = new EGG::Thread(OSGetCurrentThread(), 4);
    sConfigData.mPerfView = new EGG::ProcessMeter(TRUE);

    EGG::DvdFile::initialize();
    EGG::Exception::create(64, 32, 4, 0);
    sConfigData.mRootHeapMem2->becomeCurrentHeap();
}

void RPSysSystem::create() {
    spInstance = new (sConfigData.GetSystemHeap()) RPSysSystem;
}

void RPSysSystem::mainLoop() {
    while (TRUE) {
        sConfigData.mDisplay->beginFrame();
        sConfigData.mPerfView->measureBeginFrame();
        RP_GET_INSTANCE(RPSysDvdStatus)->update();
        //! Tentative (for Wii Fit Plus, confirmed in Wii Sports)
        RP_GET_INSTANCE(RPSysSceneMgr)->getCurrentSceneRP();
        //! RPGrpRenderer::CalculateTexCopyBackground();
        sConfigData.mDisplay->beginRender();
        sConfigData.mPerfView->measureBeginRender();
        RP_GET_INSTANCE(RPSysSceneMgr)->draw();
        sConfigData.mPerfView->draw();
        sConfigData.mPerfView->measureEndRender();
        EGG_GET_INSTANCE(EGG::CoreControllerMgr)->beginFrame();
        RP_GET_INSTANCE(RPSysSceneMgr)->calc();
        EGG_GET_INSTANCE(EGG::CoreControllerMgr)->endFrame();
        RP_GET_INSTANCE(RPSndAudioMgr)->calc();
        sConfigData.mPerfView->measureEndFrame();
        RP_GET_INSTANCE(RPSysDvdStatus)->draw();
        RP_GET_INSTANCE(RPSysHomeMenuMgr)->drawBanIcon();
        RP_GET_INSTANCE(RPSysSceneMgr)->getCurrentSceneRP();
        sConfigData.mDisplay->endRender();
        sConfigData.mDisplay->endFrame();
    }
}

void RPSysSystem::loadFrameWork() {
    EGG_GET_INSTANCE(EGG::CoreControllerMgr)->endFrame();
    RP_GET_INSTANCE(RPSndAudioMgr)->calc();
    RP_GET_INSTANCE(RPSysDvdStatus)->draw();
    RP_GET_INSTANCE(RPSysHomeMenuMgr)->drawBanIcon();
    sConfigData.mDisplay->endRender();
    sConfigData.mDisplay->endFrame();
    sConfigData.mDisplay->beginFrame();
    RP_GET_INSTANCE(RPSysDvdStatus)->update();
    sConfigData.mDisplay->beginRender();
    RP_GET_INSTANCE(RPSysSceneMgr)->drawMgrFader();
    EGG_GET_INSTANCE(EGG::CoreControllerMgr)->beginFrame();
    //! RPSysHomeMenuMgr function at 801F2864
    mLoadCount += mFrameRate;
    if (RP_GET_INSTANCE(RPSysSceneMgr)->isShutDownReserved()) {
        returnToMenu();
    }
}

void RPSysSystem::setupTVMode() {
    int i;

    VIInit();
    for (i = 0; i < 60; i++) {
        if (SCCheckStatus() == SC_STATUS_OK) {
            break;
        }
        VIWaitForRetrace();
    }
    if (SCGetAspectRatio() == SC_ASPECT_WIDE) {
        EGG::Screen::SetTVMode(EGG::Screen::TV_MODE_WIDE);
    } else {
        EGG::Screen::SetTVMode(EGG::Screen::TV_MODE_STD);
    }

    DECOMP_I_BOMB;
}

RPSysSceneCreator::ESceneID RPSysSystem::getBootScene() {
    return RPSysSceneCreator::ESceneID_RPSysBootScene;
}

void RPSysSystem::startLoadCount() {
    mLoadCount = 0;
}

void RPSysSystem::setDimming(BOOL dim) {
    VIEnableDimming(dim);
}

void RPSysSystem::setAutoSleepTime(u8 time) {
    WPADSetAutoSleepTime(time);
}

const char* RPSysSystem::getTimeStampString() {
    return mpTimeStampString;
}

RPSysSystem::~RPSysSystem() {}

/**
 * @brief Constructor
 */

RPSysSystem::RPSysSystem() {
    mpResourceHeap = NULL;
    mpReserveHeap = NULL;
    mpAssertHeap = NULL;
    mpDebugHeap = NULL;
    mpCurrentHeap = NULL;
    mUNK_0x18 = NULL;
    mUNK_0x1C = NULL;
    mUNK_0x20 = NULL;
    mUNK_0x24 = NULL;
    OSInitMutex(&mCurrentHeapMutex);
    mpNandThread = NULL;
    mpDvdThread = NULL;
    mNandEndMessage = FOURCC('n', 'a', 'n', 'd');
    mDvdEndMessage = FOURCC('d', 'i', 's', 'k');
    mPowerFlag = 1;
    mFrameRate = 1;
    sConfigData.GetDisplay()->setFrameRate(mFrameRate);
    mLoadCount = 0;
    mpTimeStampString = NULL;
    mFrameTime = (f32)mFrameRate / 60.0f;
}

void RPSysSystem::setupLocalSettings() {
    u8 sysLanguage = SCGetLanguage();

    if (sysLanguage == SC_LANG_EN) {
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLocale(RPSysProjectLocal::EArea_USA);
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLanguage(RPSysProjectLocal::EArea_USA);
    } else if (sysLanguage == SC_LANG_FR) {
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLocale(RPSysProjectLocal::EArea_USA);
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLanguage(RPSysProjectLocal::EArea_France);
    } else if (sysLanguage == SC_LANG_SP) {
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLocale(RPSysProjectLocal::EArea_USA);
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLanguage(RPSysProjectLocal::EArea_Spain);
    }
    //! Requires ifdef for non Americas versions
    else {
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLocale(RPSysProjectLocal::EArea_USA);
        RP_GET_INSTANCE(RPSysProjectLocal)
            ->setLanguage(RPSysProjectLocal::EArea_USA);
    }
}

void RPSysSystem::setCallBack() {
    OSSetResetCallback(softResetCallBack);
    OSSetPowerCallback(shutdownSystemCallBack);
}

/**
 * @brief Controls whether the game restarts or returns to the Wii Menu upon a
 * soft reset.
 */
void RPSysSystem::softResetCallBack() {
    if (RP_GET_INSTANCE(RPSysSystem)->mPowerFlag) {
        //! TODO(texline) isErrorOccured is an inline in Wii Fit Plus.
        //! Maybe check the value directly?
        if (RP_GET_INSTANCE(RPSysDvdStatus)->isErrorOccured()) {
            RP_GET_INSTANCE(RPSysSceneMgr)->returnToMenu(FALSE);
        } else {
            VIEnableDimming(FALSE);

            if (RP_GET_INSTANCE(RPSysSceneMgr)->isNormalState() &&
                !RP_GET_INSTANCE(RPSysHomeMenuMgr)->softReset()) {

                RP_GET_INSTANCE(RPSysSceneMgr)->softReset(FALSE);
            }
        }
    }
}

void RPSysSystem::shutdownSystemCallBack() {
    if (RP_GET_INSTANCE(RPSysSystem)->mPowerFlag) {
        RP_GET_INSTANCE(RPSysSceneMgr)->shutdownSystem(FALSE);
    }
}
