#include <Pack/RPAudio.h>
#include <Pack/RPSystem.h>

RP_SINGLETON_IMPL_EX(RPSysSceneCreator);

#define RP_COMMON_DIR "RPHealthCommon/"
#define RP_MAIN_SCENE_DIR "RPHealthMainScene/"

/**
 * @brief Information about every Pack Project scene
 */
const RPSysSceneCreator::SceneAttr RPSysSceneCreator::SCENE_ATTR_TABLE[] = {
    // clang-format off
    {ESceneID_RPHealthPlayerSelectScene, ECreateType_Sibling, EExitType_2, EAudioType_Scene, "RPHealthPlayerSelect/", "User Selection"},
    {ESceneID_RPHealthMainMenu, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_COMMON_DIR, "Main Menu"},
    {ESceneID_RPHealthUserSettings, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_COMMON_DIR, "User Settings"},
    {ESceneID_RPHealthLaunchDisk, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_COMMON_DIR, "Launch Disk"},

    {ESceneID_RPHealthBodyTest, ECreateType_Sibling, EExitType_Standard, EAudioType_Scene, RP_COMMON_DIR, "Body Test"},
    {ESceneID_RPHealthBodyTestResult, ECreateType_Sibling, EExitType_Standard, EAudioType_Scene, RP_COMMON_DIR, "Body Test Results"},
    {ESceneID_RPHealthSimpleCheck, ECreateType_Sibling, EExitType_Standard, EAudioType_Scene, RP_COMMON_DIR, "Simple Check"},
    {ESceneID_RPHealthHoldCheck, ECreateType_Sibling, EExitType_Standard, EAudioType_Scene, RP_COMMON_DIR, "Hold Check"},
    {ESceneID_RPHealthConnectCheck, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_COMMON_DIR, "Connect Check"},
    {ESceneID_RPHealthBalanceTest, ECreateType_Sibling, EExitType_Standard, EAudioType_Scene, RP_COMMON_DIR, "Balance Test"},
    {ESceneID_RPHealthCapture, ECreateType_Standard, EExitType_Sibling, EAudioType_Scene, "RPHealthPlayerSelect/", "Capture"},
    {ESceneID_RPHealthPackBootScene, ECreateType_Standard, EExitType_Sibling, EAudioType_Scene, "RPHealthTitle/", /* Boot */ "ブート"},
    {ESceneID_RPHealthTitleScene, ECreateType_Sibling, EExitType_Sibling, EAudioType_Scene, "RPHealthTitle/", /* Title */ "タイトル"},
    {ESceneID_RPHealthGuestMode, ECreateType_Sibling, EExitType_2, EAudioType_Scene, "RPHealthTitle/", "Guest Mode"},
    {ESceneID_RPHealthFirstSettingScene, ECreateType_Sibling, EExitType_Standard, EAudioType_Scene, "RPHealthFirstSetting/", "First Settings"},
    {ESceneID_RPHealthContinue, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_MAIN_SCENE_DIR, "Continue"},
    {ESceneID_RPHealthBasicSettingScene, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_MAIN_SCENE_DIR, "Basic Settings"},
    {ESceneID_RPHealthControllerCheckScene, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_MAIN_SCENE_DIR, "Controller Check"},
    {ESceneID_RPHealthPairingScene, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_MAIN_SCENE_DIR, "Pairing"},
    {ESceneID_RPHealthRest, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_MAIN_SCENE_DIR, "Rest"},
    {ESceneID_RPHealthGetBack, ECreateType_Sibling, EExitType_2, EAudioType_Scene, RP_MAIN_SCENE_DIR, "Get Back"},

    {ESceneID_RPHealthTrainingMenuScene, ECreateType_Sibling, EExitType_2, EAudioType_Scene, "RPHealthMenu/", "Training Menu"},

    {ESceneID_RPHealthShootingScene, ECreateType_Sibling, EExitType_Standard, EAudioType_Common, "RPHealthShooting/", "Shooting"},
    {ESceneID_RPHealthColorScene, ECreateType_Sibling, EExitType_Standard, EAudioType_Common, "RPHealthColor/", "Color"},

    {ESceneID_RPHealthStaffCredit, ECreateType_Sibling, EExitType_2, EAudioType_Common, "RPHealthMenu/", "Staff Credit"},
    {ESceneID_RPHealthStaffCreditPlus, ECreateType_Sibling, EExitType_2, EAudioType_Common, "RPHealthMyMenu/", "Staff Credit Plus"}

    // clang-format on
};

/**
 * @brief Constructor
 *
 * @param pHeap Parent heap
 */
RPSysSceneCreator::RPSysSceneCreator(EGG::Heap* pHeap)
    : mpParentHeap(pHeap), mPrevSceneID(-1) {}

/**
 * @brief Gets the index of the specified scene's entry in the table
 *
 * @param id Scene ID
 * @return Entry index, or -1 if not found
 */
s32 RPSysSceneCreator::getEntryIndex(ESceneID id) {
    for (s32 i = 0; i < ARRAY_SIZE(SCENE_ATTR_TABLE); i++) {
        if (SCENE_ATTR_TABLE[i].scene == id) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Requests a scene change after fading out
 * @details A scene id of -1 will select the current scene.
 *
 * @param id New scene ID
 * @param reconfigure Whether to fully reconfigure the scene
 * @return Success
 */
bool RPSysSceneCreator::changeSceneAfterFade(s32 id, bool reconfigure) {
    bool success = false;

    if (RP_GET_INSTANCE(RPSysSceneMgr)->isTaskFinished()) {
        int current = RP_GET_INSTANCE(RPSysSceneMgr)->getCurrentSceneID();

        if (reconfigure) {
            id = current;
        }

        success = RP_GET_INSTANCE(RPSysSceneMgr)
                      ->changeNextSceneAfterFade(id, reconfigure);

        if (success && current >= 0) {
            // Fade out audio before switching sound archives
            if (getAudioType(current) != getAudioType(id)) {
                u16 frames = RP_GET_INSTANCE(RPSysSceneMgr)->getFadeFrame();
                RP_GET_INSTANCE(RPSndAudioMgr)->fadeOutAllSound(frames);
            }

            if (getExitType(id) != EExitType_Child) {
                mPrevSceneID = current;
            }
        }
    }

    return success;
}

/**
 * @brief Changes to the title scene
 *
 * @return Success
 */
bool RPSysSceneCreator::changeSoftReset() {
    return changeSceneAfterFade(ESceneID_RPHealthPlayerSelectScene);
}

/**
 * @brief Destroys the specified scene
 *
 * @param id Scene ID
 */
void RPSysSceneCreator::destroy(s32 /* id */) {
    if (RP_GET_INSTANCE(RPSysSceneMgr)->isApplicationEndReserved()) {
        RP_GET_INSTANCE(RPSysSystem)->systemShutDown();
    }
}

/**
 * @brief Gets the specified scene's exit type
 *
 * @param id Scene ID
 */
RPSysSceneCreator::EExitType RPSysSceneCreator::getExitType(s32 id) {
    s32 index = getEntryIndex(static_cast<ESceneID>(id));

// @bug Table index is not validated
#if defined(BUG_FIX)
    if (index < 0) {
        return EExitType_Standard;
    }
#endif

    return SCENE_ATTR_TABLE[index].exitType;
}

/**
 * @brief Gets the specified scene's audio type
 *
 * @param id Scene ID
 */
RPSysSceneCreator::EAudioType RPSysSceneCreator::getAudioType(s32 id) {
    s32 index = getEntryIndex(static_cast<ESceneID>(id));

// @bug Table index is not validated
#if defined(BUG_FIX)
    if (index < 0) {
        return EAudioType_Scene;
    }
#endif

    return SCENE_ATTR_TABLE[index].audioType;
}

/**
 * @brief Gets the specified scene's resource directory
 * @details A scene id of -1 will select the current scene.
 *
 * @param id Scene ID
 */
const char* RPSysSceneCreator::getResDirName(s32 id) {
    if (id < 0) {
        id = RP_GET_INSTANCE(RPSysSceneMgr)->getCurrentSceneID();
    }

    s32 index = getEntryIndex(static_cast<ESceneID>(id));

// @bug Table index is not validated
#if defined(BUG_FIX)
    if (index < 0) {
        return RP_COMMON_DIR;
    }
#endif

    return SCENE_ATTR_TABLE[index].pResDirName;
}

/**
 * @brief Gets the specified scene's creation type
 *
 * @param id Scene ID
 */
RPSysSceneCreator::ECreateType RPSysSceneCreator::getCreateType(s32 id) {
    s32 index = getEntryIndex(static_cast<ESceneID>(id));

// @bug Table index is not validated
#if defined(BUG_FIX)
    if (index < 0) {
        return ECreateType_Standard;
    }
#endif

    return SCENE_ATTR_TABLE[index].createType;
}

#if defined(PACK_SPORTS)
/**
 * @brief Creates the specified Sports Pack scene
 *
 * @param id Scene ID
 */
RPSysScene* RPSysSceneCreator::createSportsScene(s32 id) {
    switch (id) {
    case ESceneID_RPBsbScene: {
        return new RPBsbScene();
    }

    case ESceneID_RPBowScene: {
        return new RPBowScene();
    }

    case ESceneID_RPGolScene: {
        return new RPGolScene();
    }

    case ESceneID_RPTnsScene: {
        return new RPTnsScene();
    }

    case ESceneID_RPBoxScene: {
        return new RPBoxScene();
    }

    case ESceneID_RPSportsPackTitleScene: {
        return new RPSportsPackTitleScene();
    }

    case ESceneID_RPSportsMainScene: {
        return new RPSportsMainScene();
    }

    case ESceneID_RPSportsTrainingMenuScene: {
        return new RPSportsTrainingMenuScene();
    }

    case ESceneID_RPSportsPhysicalMenuScene: {
        return new RPSportsPhysicalMenuScene();
    }

    case ESceneID_RPSportsPhysicalPreviewScene: {
        return new RPSportsPhysicalPreviewScene();
    }

    case ESceneID_RPSportsPhysicalResultScene: {
        return new RPSportsPhysicalResultScene();
    }

    case ESceneID_RPGolCourseSelectScene: {
        return new RPGolCourseSelectScene();
    }

    default: {
        return NULL;
    }
    }
}
#endif

#if defined(PACK_PARTY)
/**
 * @brief Creates the specified Party Pack scene
 *
 * @param id Scene ID
 */
RPSysScene* RPSysSceneCreator::createPartyScene(s32 id) {
    switch (id) {
    case ESceneID_RPFshScene: {
        return new RPFshScene();
    }

    case ESceneID_RPHkyScene: {
        return new RPHkyScene();
    }

    case ESceneID_RPDucScene: {
        return new RPDucScene();
    }

    case ESceneID_RPPnpScene: {
        return new RPPnpScene();
    }

    case ESceneID_RPBilScene: {
        return new RPBilScene();
    }

    case ESceneID_RPCowScene: {
        return new RPCowScene();
    }

    case ESceneID_RPWlyScene: {
        return new RPWlyScene();
    }

    case ESceneID_RPTnkScene: {
        return new RPTnkScene();
    }

    case ESceneID_RPBomScene: {
        return new RPBomScene();
    }

    case ESceneID_RPPartyPackTitleScene: {
        return new RPPartyPackTitleScene();
    }

    case ESceneID_RPPartyRootScene: {
        return new RPPartyRootScene();
    }

    case ESceneID_RPPartyMainScene: {
        return new RPPartyMainScene();
    }

    default: {
        return NULL;
    }
    }
}
#endif

/**
 * @brief Destructor
 */
RPSysSceneCreator::~RPSysSceneCreator() {}
