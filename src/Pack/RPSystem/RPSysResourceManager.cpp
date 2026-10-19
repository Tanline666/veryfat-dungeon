#include <Pack/RPKernel.h>
#include <Pack/RPParty/RPPartyCommon.h>
#include <Pack/RPSports/RPSportsCommon.h>
#include <Pack/RPSystem.h>

#include <nw4r/ut.h>

#include <cstring>

RP_SINGLETON_IMPL(RPSysResourceManager);

/**
 * \~english @brief List of scenes whose resources should be cached
 */
const RPSysSceneCreator::ESceneID RPSysResourceManager::CACHED_SCENES[] = {
    RPSysSceneCreator::ESceneID_RPHealthPlayerSelectScene,
    RPSysSceneCreator::ESceneID_RPHealthControllerCheckScene,

#if defined(PACK_SPORTS)
    RPSysSceneCreator::ESceneID_RPBsbScene,
    RPSysSceneCreator::ESceneID_RPBowScene,
    RPSysSceneCreator::ESceneID_RPGolScene,
    RPSysSceneCreator::ESceneID_RPTnsScene,
    RPSysSceneCreator::ESceneID_RPBoxScene,
    RPSysSceneCreator::ESceneID_RPSportsPackTitleScene,
    RPSysSceneCreator::ESceneID_RPSportsMainScene,
    RPSysSceneCreator::ESceneID_RPSportsTrainingMenuScene,
    RPSysSceneCreator::ESceneID_RPSportsPhysicalMenuScene,
    RPSysSceneCreator::ESceneID_RPSportsPhysicalPreviewScene,
    RPSysSceneCreator::ESceneID_RPSportsPhysicalResultScene,
    RPSysSceneCreator::ESceneID_RPGolCourseSelectScene,
#elif defined(PACK_PARTY)
    RPSysSceneCreator::ESceneID_RPFshScene,
    RPSysSceneCreator::ESceneID_RPHkyScene,
    RPSysSceneCreator::ESceneID_RPDucScene,
    RPSysSceneCreator::ESceneID_RPPnpScene,
    RPSysSceneCreator::ESceneID_RPBilScene,
    RPSysSceneCreator::ESceneID_RPCowScene,
    RPSysSceneCreator::ESceneID_RPWlyScene,
    RPSysSceneCreator::ESceneID_RPTnkScene,
    RPSysSceneCreator::ESceneID_RPBomScene,
    RPSysSceneCreator::ESceneID_RPPartyPackTitleScene,
    RPSysSceneCreator::ESceneID_RPPartyRootScene,
    RPSysSceneCreator::ESceneID_RPPartyMainScene,
#endif
};
// clang-format on

/**
 * \~english @brief Absolute path to the kokeshi asset archive
 */
const char* RPSysResourceManager::KOKESHI_ARCHIVE_PATH =
    "Common/Kokeshi/common.carc";

/**
 * \~english @brief Constructor
 */
RPSysResourceManager::RPSysResourceManager() {
    NW4R_UT_LIST_INIT(mResourceLists[EList_FileCache], RPSysFile);
    NW4R_UT_LIST_INIT(mResourceLists[EList_FileList], RPSysFile);

    mpPathWork = new char[RPSysFile::PATH_MAX];
}

/**
 * \~english @brief Destructor
 */
RPSysResourceManager::~RPSysResourceManager() {}

/**
 * \~english @brief Gets the path to the common asset folder for the specified
 * scene
 *
 * \~english @param[out] pBuffer Path output buffer
 * \~english @param maxlen Output buffer size
 * \~english @param id Scene ID (defaults to the current scene)
 */
void RPSysResourceManager::GetGameCommonPath(char* pBuffer, u32 /* maxlen */,
                                             s32 id) {
    RPSysSceneCreator* pSceneCreator = RP_GET_INSTANCE(RPSysSceneCreator);

    std::strcpy(pBuffer, "Common/");
    std::strcat(pBuffer, pSceneCreator->getResDirName(id));
}

/**
 * \~english @brief Loads the common asset archive for the specified scene
 *
 * \~english @param id Scene ID (defaults to the current scene)
 * \~english @param pHeap Heap to use for allocations (defaults to the current
 * heap)
 */
EGG::Archive* RPSysResourceManager::LoadGameCommonArchive(s32 id,
                                                          EGG::Heap* pHeap) {

    instance()->GetGameCommonPath(instance()->mpPathWork, RPSysFile::PATH_MAX,
                                  id);

    std::strcat(instance()->mpPathWork, "common.carc");

    if (pHeap == NULL) {
        pHeap = EGG::Heap::getCurrentHeap();
    }

    void* pArcBinary =
        instance()->LoadCompressed(instance()->mpPathWork, pHeap);

    return EGG::Archive::mount(pArcBinary, pHeap);
}

/**
 * \~english @brief Gets the path to the local asset folder for the specified
 * scene
 *
 * \~english @param[out] pBuffer Path output buffer
 * \~english @param maxlen Output buffer size
 * \~english @param id Scene ID (defaults to the current scene)
 */
void RPSysResourceManager::GetGameLocalPath(char* pBuffer, u32 /* maxlen */,
                                            s32 id) {
    RPSysSceneCreator* pSceneCreator = RP_GET_INSTANCE(RPSysSceneCreator);
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);

    pBuffer[0] = '\0';
    pProjectLocal->appendLocalDirectory(pBuffer);

    std::strcat(pBuffer, pSceneCreator->getResDirName(id));
}

/**
 * \~english @brief Loads the local asset archive for the specified scene
 *
 * \~english @param id Scene ID (defaults to the current scene)
 * \~english @param pHeap Heap to use for allocations (defaults to the current
 * heap)
 */
EGG::Archive* RPSysResourceManager::LoadGameLocalArchive(s32 id,
                                                         EGG::Heap* pHeap) {
    instance()->GetGameLocalPath(instance()->mpPathWork, RPSysFile::PATH_MAX,
                                 id);

    std::strcat(instance()->mpPathWork, "local.carc");

    if (pHeap == NULL) {
        pHeap = EGG::Heap::getCurrentHeap();
    }

    void* pArcBinary =
        instance()->LoadCompressed(instance()->mpPathWork, pHeap);

    return EGG::Archive::mount(pArcBinary, pHeap);
}

/**
 * \~english @brief Gets the path to the stage asset folder for the specified
 * scene
 *
 * \~english @param[out] pBuffer Path output buffer
 * \~english @param maxlen Output buffer size
 * \~english @param id Scene ID (defaults to the current scene)
 */
void RPSysResourceManager::GetGameStagePath(char* pBuffer, u32 /* maxlen */,
                                            s32 id) {
    RPSysSceneCreator* pSceneCreator = RP_GET_INSTANCE(RPSysSceneCreator);

    std::strcpy(pBuffer, "Stage/");
    std::strcat(pBuffer, pSceneCreator->getResDirName(id));
}

/**
 * \~english @brief Loads the specified archive from the current scene's stage
 * assets
 *
 * \~english @param pName Archive name
 */
EGG::Archive* RPSysResourceManager::LoadGameStageArchive(const char* pName) {
    instance()->GetGameStagePath(instance()->mpPathWork, RPSysFile::PATH_MAX);
    std::strcat(instance()->mpPathWork, pName);

    EGG::Heap* pHeap = EGG::Heap::getCurrentHeap();

    void* pArcBinary =
        instance()->LoadCompressed(instance()->mpPathWork, pHeap);

    return EGG::Archive::mount(pArcBinary, pHeap);
}

/**
 * \~english @brief Gets the specified file from the provided archive
 *
 * \~english @param pArchive Archive containing the file
 * \~english @param pName File name
 * \~english @param[out] pSize Size of the specified file
 */
void* RPSysResourceManager::GetFileFromArchive(EGG::Archive* pArchive,
                                               const char* pName, u32* pSize) {

    EGG::Archive::FileInfo fileInfo;
    void* pFile = pArchive->getFile(pName, &fileInfo);

    if (pSize != NULL) {
        *pSize = fileInfo.getLength();
    }

    return pFile;
}

/**
 * \~english @brief Gets the specified file from the message archive
 *
 * \~english @param pName Message file name
 */
void* RPSysResourceManager::GetMessageResource(const char* pName) {
    return GetFileFromArchive(instance()->GetMessageArchive(), pName);
}

/**
 * \~english @brief Tests whether a file exists at the specified path
 *
 * \~english @param pPath Path to examine
 */

bool RPSysResourceManager::IsExist(const char* pPath) {
    EGG::DvdFile f;
    return f.open(pPath, GetMultiHandle());
}

/**
 * \~english @brief Removes a file from the list of decompressed files
 * \~ @note RPSysFile automatically removes itself when it is destroyed.
 *
 * \~english @param pFile File to remove
 */
void RPSysResourceManager::RemoveFromFileList(RPSysFile* pFile) {
    nw4r::ut::List_Remove(&mResourceLists[EList_FileList], pFile);
}

/*
void RPSysResourceManager::LoadStaticArchives() {
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);
    EGG::Heap* pResHeap = RP_GET_INSTANCE(RPSysSystem)->getResourceHeap();

    mpStaticCommonArchive =
        LoadStaticCommonArchive(STATIC_DIR_NAMES[pProjectLocal->getPack()]);

    mpStaticLocalArchive =
        LoadStaticLocalArchive(STATIC_DIR_NAMES[pProjectLocal->getPack()]);

#if defined(PACK_SPORTS)
    mpAppMiiManager = new (pResHeap) RPSportsAppMiiManager(pResHeap);
    mpAppMiiManager->LoadResource();
#endif

    LoadMessageArchive();
    LoadFontArchive();

    RPSysKokeshiManager* pKokeshiManager = RP_GET_INSTANCE(RPSysKokeshiManager);
#if defined(PACK_SPORTS)
    pKokeshiManager->SetBodyManager(new (pResHeap) RPSportsBodyManager());
#elif defined(PACK_PARTY)
    pKokeshiManager->SetBodyManager(new (pResHeap) RPPartyBodyManager());
#endif

    RP_GET_INSTANCE(RPSysFontManager)->LoadResFonts();
}
    */

/**
 * \~english @brief Loads the specified file and adds it to the resource cache
 * @details If the file has already been cached, this function does nothing.
 *
 * \~english @param pPath File path
 * \~english @param list List that the file should be moved to
 * \~english @param pHeap Heap to use for allocations
 */
void RPSysResourceManager::LoadCachedFile(const char* pPath, EList list,
                                          EGG::Heap* pHeap) {
    RPSysFile* pFile = FindFile(pPath, EList_FileCache);
    if (pFile != NULL) {
        return;
    }

    s32 fileSize = 0;
    u8* pFileData =
        LoadFromDVD(pPath, pHeap, EGG::DvdRipper::ALLOC_DIR_TAIL, &fileSize);

    DCFlushRange(pFileData, fileSize);
    MakeFile(pPath, list, fileSize, pFileData, pHeap);
}

/**
 * \~english @brief Loads all archives designated for cached access
 */
void RPSysResourceManager::LoadCacheArchives() {
    EGG::Heap* pResHeap = RP_GET_INSTANCE(RPSysSystem)->getResourceHeap();

    for (int i = 0; i < static_cast<int>(ARRAY_SIZE(CACHED_SCENES)); i++) {
        GetGameCommonPath(instance()->mpPathWork, RPSysFile::PATH_MAX,
                          CACHED_SCENES[i]);

        std::strcat(instance()->mpPathWork, "common.carc");
        LoadCachedFile(instance()->mpPathWork, EList_FileCache, pResHeap);

        // Local archives in Wii Play are small enough to also cache
#if defined(PACK_PARTY)
        GetGameLocalPath(instance()->mpPathWork, RPSysFile::PATH_MAX,
                         CACHED_SCENES[i]);

        std::strcat(instance()->mpPathWork, "local.carc");
        LoadCachedFile(instance()->mpPathWork, EList_FileCache, pResHeap);
#endif
    }
}

/**
 * \~english @brief Loads the archive containing all kokeshi assets
 */
void RPSysResourceManager::LoadKokeshiArchive() {
    EGG::Heap* pResHeap = RP_GET_INSTANCE(RPSysSystem)->getResourceHeap();

    std::strcpy(instance()->mpPathWork, KOKESHI_ARCHIVE_PATH);

    void* pArcBinary =
        instance()->LoadCompressed(instance()->mpPathWork, pResHeap);

    mpKokeshiArchive = EGG::Archive::mount(pArcBinary, pResHeap);
}

/**
 * \~english @brief Gets the path to the common sound folder
 *
 * \~english @param[out] pBuffer Path output buffer
 * \~english @param maxlen Output buffer size
 */
void RPSysResourceManager::GetStaticSoundCommonPath(char* pBuffer,
                                                    u32 /* maxlen */) {
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);

    std::strcpy(pBuffer, "Sound/");
    std::strcat(pBuffer, STATIC_DIR_NAMES[pProjectLocal->getPack()]);
}

/**
 * \~english @brief Gets the path to the local sound folder
 *
 * \~english @param[out] pBuffer Path output buffer
 * \~english @param maxlen Output buffer size
 */
void RPSysResourceManager::GetStaticSoundLocalPath(char* pBuffer,
                                                   u32 /* maxlen */) {
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);

    std::strcpy(pBuffer, "Sound/");
    std::strcat(pBuffer, STATIC_DIR_NAMES[pProjectLocal->getPack()]);
    pProjectLocal->appendLocalDirectory(pBuffer);
}

/**
 * \~english @brief Gets the path to the common sound folder for the specified
 * scene
 *
 * \~english @param[out] pBuffer Path output buffer
 * \~english @param maxlen Output buffer size
 * \~english @param id Scene ID (defaults to the current scene)
 */
void RPSysResourceManager::GetGameSoundCommonPath(char* pBuffer,
                                                  u32 /* maxlen */, s32 id) {
    RPSysSceneCreator* pSceneCreator = RP_GET_INSTANCE(RPSysSceneCreator);

    std::strcpy(pBuffer, "Sound/");
    std::strcat(pBuffer, pSceneCreator->getResDirName(id));
}

/**
 * \~english @brief Gets the path to the local sound folder for the specified
 * scene
 *
 * \~english @param[out] pBuffer Path output buffer
 * \~english @param maxlen Output buffer size
 * \~english @param id Scene ID (defaults to the current scene)
 */
void RPSysResourceManager::GetGameSoundLocalPath(char* pBuffer,
                                                 u32 /* maxlen */, s32 id) {
    RPSysSceneCreator* pSceneCreator = RP_GET_INSTANCE(RPSysSceneCreator);
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);

    std::strcpy(pBuffer, "Sound/");
    std::strcat(pBuffer, pSceneCreator->getResDirName(id));
    pProjectLocal->appendLocalDirectory(pBuffer);
}

/**
 * \~english @brief Loads the local archive in the specified static asset
 * directory
 *
 * \~english @param pStaticDir Static asset directory
 */
EGG::Archive*
RPSysResourceManager::LoadStaticLocalArchive(const char* pStaticDir) {
    RPSysSceneCreator* pSceneCreator = RP_GET_INSTANCE(RPSysSceneCreator);
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);

    mpPathWork[0] = '\0';
    pProjectLocal->appendLocalDirectory(mpPathWork);

    std::strcat(mpPathWork, pStaticDir);
    std::strcat(mpPathWork, "local.carc");

    EGG::Heap* pResHeap = RP_GET_INSTANCE(RPSysSystem)->getResourceHeap();

    void* pArcBinary =
        instance()->LoadCompressed(instance()->mpPathWork, pResHeap);

    return EGG::Archive::mount(pArcBinary, pResHeap);
}

/**
 * \~english @brief Loads the common archive in the specified static asset
 * directory
 *
 * \~english @param pStaticDir Static asset directory
 */
EGG::Archive*
RPSysResourceManager::LoadStaticCommonArchive(const char* pStaticDir) {
    std::strcpy(mpPathWork, "Common/");
    std::strcat(mpPathWork, pStaticDir);
    std::strcat(mpPathWork, "common.carc");

    EGG::Heap* pResHeap = RP_GET_INSTANCE(RPSysSystem)->getResourceHeap();

    void* pArcBinary = LoadCompressed(mpPathWork, pResHeap);
    return EGG::Archive::mount(pArcBinary, pResHeap);
}

DECOMP_FORCEACTIVE(RPSysResourceManager_cpp,
                   "effect.carc",
                   "local.carc");

/**
 * \~english @brief Loads the archive containing all message data
 */
void RPSysResourceManager::LoadMessageArchive() {
    EGG::Heap* pResHeap = RP_GET_INSTANCE(RPSysSystem)->getResourceHeap();
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);

    instance()->mpPathWork[0] = '\0';
    pProjectLocal->appendLocalDirectory(instance()->mpPathWork);
    std::strcat(instance()->mpPathWork, "Message/message.carc");

    void* pArcBinary =
        instance()->LoadCompressed(instance()->mpPathWork, pResHeap);

    mpMessageArchive = EGG::Archive::mount(pArcBinary, pResHeap);
}

/**
 * \~english @brief Loads the archive containing all font data
 */
void RPSysResourceManager::LoadFontArchive() {
    EGG::Heap* pResHeap = RP_GET_INSTANCE(RPSysSystem)->getResourceHeap();
    RPSysProjectLocal* pProjectLocal = RP_GET_INSTANCE(RPSysProjectLocal);

    instance()->mpPathWork[0] = '\0';
    pProjectLocal->appendLocalDirectory(instance()->mpPathWork);
    std::strcat(instance()->mpPathWork, "Font/font.carc");

    void* pArcBinary =
        instance()->LoadCompressed(instance()->mpPathWork, pResHeap);

    mpFontArchive = EGG::Archive::mount(pArcBinary, pResHeap);
}

/**
 * \~english @brief Loads the specified file from the DVD, decompressing its
 * contents if applicable
 *
 * \~english @param pPath File path
 * \~english @param pHeap Heap to use for allocations
 * \~english @return Raw file contents
 */
u8* RPSysResourceManager::LoadCompressed(const char* pPath, EGG::Heap* pHeap) {
    RPSysFile* pFile = NULL;

    // See if the file has already been decompressed
    pFile = FindFile(pPath, EList_FileList);
    if (pFile != NULL) {
        return const_cast<u8*>(pFile->GetData());
    }

    s32 fileSize = 0;
    pFile = FindFile(pPath, EList_FileCache);

    // If the file is waiting in the cache, decompress it
    if (pFile != NULL) {
        u8* pFileData = const_cast<u8*>(pFile->GetData());
        fileSize = pFile->GetSize();

        s32 expandSize = 0;
        u8* pExpandData =
            ExpandCompressed(pFileData, fileSize, &expandSize, pHeap);

        DCFlushRange(pExpandData, expandSize);

        MakeFile(pPath, EList_FileList, expandSize, pExpandData, pHeap);
        return pExpandData;
    }
    // Cache miss, need to load AND decompress the file
    else {
        u8* pFileData = LoadFromDVD(pPath, pHeap,
                                    EGG::DvdRipper::ALLOC_DIR_HEAD, &fileSize);

        s32 expandSize = 0;
        u8* pExpandData =
            ExpandCompressed(pFileData, fileSize, &expandSize, pHeap);

        DCFlushRange(pExpandData, expandSize);

        // Original compressed data is temporary memory
        delete pFileData;

        MakeFile(pPath, EList_FileList, expandSize, pExpandData, pHeap);
        return pExpandData;
    }
}

/**
 * \~english @brief Loads the specified file from the DVD
 *
 * \~english @param pPath Path to the file
 * \~english @param pHeap Heap to use for allocations
 * \~english @param allocDir Direction of heap allocations
 * \~english @param[out] pSize Size of the specified file
 */
u8* RPSysResourceManager::LoadFromDVD(const char* pPath, EGG::Heap* pHeap,
                                      EGG::DvdRipper::EAllocDirection allocDir,
                                      s32* pSize) {
    EGG::DvdFile f;

    if (!f.open(pPath, GetMultiHandle())) {
        return NULL;
    }

    s32 fileSize = ROUND_UP(f.getFileSize(), 32);

    if (pSize != NULL) {
        *pSize = fileSize;
    }

    int align = 32;
    if (allocDir != EGG::DvdRipper::ALLOC_DIR_TAIL) {
        align = -32;
    }

    u8* pBuffer = new (pHeap, align) u8[fileSize];

    if (pBuffer == NULL) {
        return NULL;
    }

    return EGG::DvdRipper::loadToMainRAM(&f, pBuffer, pHeap,
                                         EGG::DvdRipper::ALLOC_DIR_HEAD);
}

DECOMP_FORCEACTIVE(RPSysResourceManager_cpp_1,
                   "/",
                   "Effect/",
                   "Stage/",
                   "",
                   "Common/");

/**
 * \~english @brief Searches the specified resource list for the provided
 * filepath
 *
 * \~english @param pPath File path
 * \~english @param list Resource list
 */
RPSysFile* RPSysResourceManager::FindFile(const char* pPath, EList list) const {
    RP_NW4R_LIST_FOREACH (RPSysFile, it, mResourceLists[list]) {
        if (std::strcmp(pPath, it->GetPath()) == 0) {
            return it;
        }
    }

    return NULL;
}

/**
 * \~english @brief Creates a new entry in the specified resource list
 *
 * \~english @param pPath File path
 * \~english @param list Resource list
 * \~english @param size File size
 * \~english @param pData File data
 * \~english @param pHeap Heap to use for allocations
 */
void RPSysResourceManager::MakeFile(const char* pPath, EList list, s32 size,
                                    const void* pData, EGG::Heap* pHeap) {

    RPSysFile* pFile = new (pHeap) RPSysFile(pPath, size, pData);
    nw4r::ut::List_Append(&mResourceLists[list], pFile);
}

/**
 * \~english @brief Decompresses the contents of the specified data
 * @details If the data is not compressed, this function returns the
 * original file contents
 *
 * \~english @param pData File data
 * \~english @param size Data size
 * \~english @param[out] pExpandSize The data size after decompression
 * \~english @param pHeap Heap to use for allocations
 * \~english @return Decompressed data
 */
u8* RPSysResourceManager::ExpandCompressed(u8* pData, s32 size,
                                           s32* pExpandSize,
                                           EGG::Heap* pHeap) const {
    u8* pExpandData = NULL;

    if (EGG::Decomp::checkCompressed(pData) != EGG::Decomp::cCompress_None) {
        *pExpandSize = EGG::Decomp::getExpandSize(pData);
        pExpandData = new (pHeap, 32) u8[*pExpandSize];
        EGG::Decomp::decode(pData, pExpandData);
    } else {
        *pExpandSize = size;
        pExpandData = new (pHeap, 32) u8[*pExpandSize];
        std::memcpy(pExpandData, pData, *pExpandSize);
    }

    return pExpandData;
}
