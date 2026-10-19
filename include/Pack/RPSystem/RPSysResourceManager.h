#ifndef RP_SYSTEM_RESOURCE_MANAGER_H
#define RP_SYSTEM_RESOURCE_MANAGER_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>
#include <Pack/RPSystem/RPSysSceneCreator.h>

#include <egg/core.h>

#include <nw4r/ut.h>

//! @addtogroup rp_system
//! @{

// Forward declarations
class RPSysFile;

/**
 * \~english @brief Resource cache manager
 */
class RPSysResourceManager {
    RP_SINGLETON_DECL(RPSysResourceManager);

public:
    /**
     * \~english @brief Resource list type
     */
    enum EList {
        EList_FileCache, //!< Cached, compressed files
        EList_FileList,  //!< Decompressed files ready for access

        EList_Max
    };

public:
    /**
     * \~english @brief Gets the path to the common asset folder for the
     * specified scene
     *
     * \~english @param[out] pBuffer Path output buffer
     * \~english @param maxlen Output buffer size
     * \~english @param id Scene ID (defaults to the current scene)
     */
    void GetGameCommonPath(char* pBuffer, u32 maxlen, s32 id = -1);

    /**
     * \~english @brief Loads the common asset archive for the specified scene
     *
     * \~english @param id Scene ID (defaults to the current scene)
     * \~english @param pHeap Heap to use for allocations (defaults to the
     * current heap)
     */
    static EGG::Archive* LoadGameCommonArchive(s32 id = -1,
                                               EGG::Heap* pHeap = NULL);

    /**
     * \~english @brief Gets the path to the local asset folder for the
     * specified scene
     *
     * \~english @param[out] pBuffer Path output buffer
     * \~english @param maxlen Output buffer size
     * \~english @param id Scene ID (defaults to the current scene)
     */
    void GetGameLocalPath(char* pBuffer, u32 maxlen, s32 id = -1);

    /**
     * \~english @brief Loads the local asset archive for the specified scene
     *
     * \~english @param id Scene ID (defaults to the current scene)
     * \~english @param pHeap Heap to use for allocations (defaults to the
     * current heap)
     */
    static EGG::Archive* LoadGameLocalArchive(s32 id = -1,
                                              EGG::Heap* pHeap = NULL);

    /**
     * \~english @brief Gets the path to the stage asset folder for the
     * specified scene
     *
     * \~english @param[out] pBuffer Path output buffer
     * \~english @param maxlen Output buffer size
     * \~english @param id Scene ID (defaults to the current scene)
     */
    void GetGameStagePath(char* pBuffer, u32 maxlen, s32 id = -1);

    /**
     * \~english @brief Loads the specified archive from the current scene's
     * stage assets
     *
     * \~english @param pName Archive name
     */
    static EGG::Archive* LoadGameStageArchive(const char* pName);

    /**
     * \~english @brief Gets the specified file from the provided archive
     *
     * \~english @param pArchive Archive containing the file
     * \~english @param pName File name
     * \~english @param[out] pSize Size of the specified file
     */
    static void* GetFileFromArchive(EGG::Archive* pArchive, const char* pName,
                                    u32* pSize = NULL);

    /**
     * \~english @brief Gets the specified file from the message archive
     *
     * \~english @param pName Message file name
     */
    static void* GetMessageResource(const char* pName);

    /**
     * \~english @brief Tests whether a file exists at the specified path
     *
     * \~english @param pPath Path to examine
     */
    bool IsExist(const char* pPath);

    /**
     * \~english @brief Removes a file from the list of decompressed files
     * \~ @note RPSysFile automatically removes itself when it is destroyed.
     *
     * \~english @param pFile File to remove
     */
    void RemoveFromFileList(RPSysFile* pFile);

    /**
     * \~english @brief Loads all archives and other assets with static lifetime
     */
    void LoadStaticArchives();

    /**
     * \~english @brief Loads the specified file and adds it to the resource
     * cache
     * @details If the file has already been cached, this function does nothing.
     *
     * \~english @param pPath File path
     * \~english @param list List that the file should be moved to
     * \~english @param pHeap Heap to use for allocations
     */
    void LoadCachedFile(const char* pPath, EList list, EGG::Heap* pHeap);

    /**
     * \~english @brief Loads all archives designated for cached access
     */
    void LoadCacheArchives();

    /**
     * \~english @brief Loads the archive containing all kokeshi assets
     */
    void LoadKokeshiArchive();

    /**
     * \~english @brief Gets the path to the common sound folder
     *
     * \~english @param[out] pBuffer Path output buffer
     * \~english @param maxlen Output buffer size
     */
    void GetStaticSoundCommonPath(char* pBuffer, u32 maxlen);

    /**
     * \~english @brief Gets the path to the local sound folder
     *
     * \~english @param[out] pBuffer Path output buffer
     * \~english @param maxlen Output buffer size
     */
    void GetStaticSoundLocalPath(char* pBuffer, u32 maxlen);

    /**
     * \~english @brief Gets the path to the common sound folder for the
     * specified scene
     *
     * \~english @param[out] pBuffer Path output buffer
     * \~english @param maxlen Output buffer size
     * \~english @param id Scene ID (defaults to the current scene)
     */
    void GetGameSoundCommonPath(char* pBuffer, u32 maxlen, s32 id = -1);

    /**
     * \~english @brief Gets the path to the local sound folder for the
     * specified scene
     *
     * \~english @param[out] pBuffer Path output buffer
     * \~english @param maxlen Output buffer size
     * \~english @param id Scene ID (defaults to the current scene)
     */
    void GetGameSoundLocalPath(char* pBuffer, u32 maxlen, s32 id = -1);

    /**
     * \~english @brief Loads the local archive in the specified static asset
     * directory
     *
     * \~english @param pStaticDir Static asset directory
     */
    EGG::Archive* LoadStaticLocalArchive(const char* pStaticDir)
        DECOMP_DONT_INLINE;

    /**
     * \~english @brief Loads the common archive in the specified static asset
     * directory
     *
     * \~english @param pStaticDir Static asset directory
     */
    EGG::Archive* LoadStaticCommonArchive(const char* pPath);

    /**
     * \~english @brief Loads the archive containing all message data
     */
    void LoadMessageArchive();

    /**
     * \~english @brief Loads the archive containing all font data
     */
    void LoadFontArchive();

    /**
     * \~english @brief Gets the archive containing all kokeshi assets
     */
    EGG::Archive* GetKokeshiArchive() const {
        return mpKokeshiArchive;
    }

    /**
     * \~english @brief Gets the archive containing all message data
     */
    EGG::Archive* GetMessageArchive() const {
        return mpMessageArchive;
    }

    /**
     * \~english @brief Gets the archive containing all font data
     */
    EGG::Archive* GetFontArchive() const {
        return mpFontArchive;
    }

    /**
     * \~english @brief Gets the archive containing static common assets for
     * this pack
     */
    EGG::Archive* GetStaticCommonArchive() const {
        return mpStaticCommonArchive;
    }

    /**
     * \~english @brief Gets the archive containing static locale assets for
     * this pack
     */
    EGG::Archive* GetStaticLocalArchive() const {
        return mpStaticLocalArchive;
    }

    /**
     * \~english @brief Gets the archive containing static layout assets for
     * this pack
     */
    EGG::Archive* GetStaticLayoutArchive() const {
        return mpStaticLayoutArchive;
    }

    /**
     * \~english @brief Gets the extra handle for opening files outside the DVD
     */
    void* GetMultiHandle() {
        return &mpMultiHandle;
    }

private:
    /**
     * \~english @brief Loads the specified file from the DVD, decompressing its
     * contents if applicable
     *
     * \~english @param pPath File path
     * \~english @param pHeap Heap to use for allocations
     * \~english @return Raw file contents
     */
    u8* LoadCompressed(const char* pPath, EGG::Heap* pHeap);

    /**
     * \~english @brief Loads the specified file from the DVD
     *
     * \~english @param pPath Path to the file
     * \~english @param pHeap Heap to use for allocations
     * \~english @param allocDir Direction of heap allocations
     * \~english @param[out] pSize Size of the specified file
     */
    u8* LoadFromDVD(const char* pPath, EGG::Heap* pHeap,
                    EGG::DvdRipper::EAllocDirection allocDir,
                    s32* pSize = NULL);

    /**
     * \~english @brief Searches the specified resource list for the provided
     * filepath
     *
     * \~english @param pPath File path
     * \~english @param list Resource list
     */
    RPSysFile* FindFile(const char* pPath, EList list) const;

    /**
     * \~english @brief Creates a new entry in the specified resource list
     *
     * \~english @param pPath File path
     * \~english @param list Resource list
     * \~english @param size File size
     * \~english @param pData File data
     * \~english @param pHeap Heap to use for allocations
     */
    void MakeFile(const char* pPath, EList list, s32 size, const void* pData,
                  EGG::Heap* pHeap);

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
    u8* ExpandCompressed(u8* pData, s32 size, s32* pExpandSize,
                         EGG::Heap* pHeap) const;

private:
    //! List of scenes whose resources should be cached
    static const RPSysSceneCreator::ESceneID CACHED_SCENES[];
    //! Static directory names for each pack
    static const char* STATIC_DIR_NAMES[1];
    //! Absolute path to the kokeshi asset archive
    static const char* KOKESHI_ARCHIVE_PATH;

    //! Resource lists
    nw4r::ut::List mResourceLists[EList_Max]; // at 0x4

    //! Work buffer for building filepaths
    char* mpPathWork; // at 0x1C

    //! Archive containing all kokeshi assets
    EGG::Archive* mpKokeshiArchive; // at 0x20
    //! Archive containing all message data
    EGG::Archive* mpMessageArchive; // at 0x24
    //! Archive containing all font data
    EGG::Archive* mpFontArchive; // at 0x28

    //! Archive containing static common assets for this pack
    EGG::Archive* mpStaticCommonArchive; // at 0x2C
    //! Archive containing static locale assets for this pack
    EGG::Archive* mpStaticLocalArchive; // at 0x30
    //! Archive containing static layout assets for this pack
    EGG::Archive* mpStaticLayoutArchive; // at 0x34

#if defined(PACK_SPORTS)
    //! Sports Pack Mii manager
    RPSportsAppMiiManager* mpAppMiiManager; // at 0x38
#endif

    //! Extra handle for opening files outside the DVD
    void* mpMultiHandle; // at 0x3C
};

//! @}

#endif
