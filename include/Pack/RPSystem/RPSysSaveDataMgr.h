#ifndef RP_SYSTEM_SAVE_DATA_MGR_H
#define RP_SYSTEM_SAVE_DATA_MGR_H
#include <Pack/types_pack.h>

#include <Pack/RPKernel.h>
#include <Pack/RPSingleton.h>

#include <egg/core.h>

#include <revolution/OS.h>

//! @addtogroup rp_system
//! @{

// Forward declarations
class RPSysSaveData;

/**
 * \~english @brief Save data manager
 */
//! IRPSysHostIOSocket may have been removed in Wii Fit Plus
//! TODO(texline): The below structure might be wrong as the game
//! has multiple save files, not just one
class RPSysSaveDataMgr : public IRPSysHostIOSocket {
    RP_SINGLETON_DECL_EX(RPSysSaveDataMgr);

public:
    /**
     * \~english @brief Banner file type
     */
    enum EBannerType {
        EBannerType_Real, //!< Banner file in the home directory
        EBannerType_Temp, //!< Banner file in the temporary directory

        EBannerType_Max
    };

public:
    /**
     * @name Access save data
     */
    /**@{*/
    // UNDER CONSTRUCTION
    /**@}*/

    /**
     * @name Prepare save data
     */
    /**@{*/
    /**
     * \~english @brief Checks which save data files exist on the NAND
     *
     * \~english @return Success
     */
    bool existPackFileSync();
    /**
     * \~english @brief Checks which save data files exist on the NAND
     * (asynchronous)
     *
     * \~english @return Success
     */
    bool existPackFileAsync();
    /**
     * \~english @brief Tests whether the NAND has enough free space to store
     * both the banner and save files (asynchronous)
     *
     * \~english @return Success
     */
    bool existNandMemoryAsync();

    /**
     * \~english @brief Attempts to create both the banner and save files on the
     * NAND
     *
     * \~english @return Success
     */
    bool createPackFileSync();
    /**
     * \~english @brief Attempts to create both the banner and save files on the
     * NAND (asynchonous)
     *
     * \~english @return Success
     */
    bool createPackFileAsync();
    /**
     * \~english @brief Attempts to delete both the banner and save files from
     * the NAND
     *
     * \~english @return Success
     */
    bool deletePackFileSync();
    /**
     * \~english @brief Attempts to save both the banner and save files to the
     * NAND
     *
     * \~english @return Success
     */
    bool savePackFileSync();

    /**
     * \~english @brief Attempts to load the save file from the NAND
     * (asynchronous)
     *
     * \~english @return Success
     */
    bool loadAsync();
    /**
     * \~english @brief Attempts to save both the banner and save files to the
     * NAND (asynchronous)
     *
     * \~english @return Success
     */
    bool saveAsync();

    /**
     * \~english @brief Loads the save file from the NAND
     */
    void loadSync();
    /**
     * \~english @brief Saves both the banner and save files to the NAND
     */
    void saveSync();

    /**
     * @name State checking
     */
    /**@{*/
    /**
     * \~english @brief Tests whether async operations have completed
     */
    bool isAsyncFinish() const;
    /**
     * \~english @brief Tests whether NAND operations have completed
     */
    bool isNandAccessFinish() const;

    /**
     * \~english @brief Tests whether both the banner and save files exist on
     * the NAND
     */
    bool isPackFileExist() const;
    /**
     * \~english @brief Tests whether the NAND has enough free space to store
     * both the banner and save files
     */
    bool isNandMemoryExist() const;

    /**
     * \~english @brief Tests whether the save file is broken (bad magic,
     * checksum, etc.)
     */
    bool isSaveFileBroken() const;
    /**
     * \~english @brief Tests whether any unrecoverable error has occurred
     * @typo
     */
    bool isErrorOccured() const;
    /**@}*/

    /**
     * \~english @brief Toggles the ability to save both the banner and save
     * files to the NAND
     *
     * \~english @param disable Whether to disable saving
     */
    void setSaveDisable(bool disable);

    /**
     * @name NAND
     */
    /**@{*/
    /**
     * \~english @brief Gets the result of the last NAND memory check
     */
    u32 getNandCheckAnswer() const;
    /**
     * \~english @brief Gets the result of the last NAND operation
     */
    s32 getErrorCode() const;

    /**
     * \~english @brief Gets the number of free NAND blocks needed to store both
     * the banner and save files
     */
    u32 getUserBlockSize() const;
    /**
     * \~english @brief Gets the number of free NAND inodes needed to store both
     * the banner and save files
     */
    u32 getSaveFileNum() const;

    /**
     * \~english @brief Marks the current async NAND operations as completed
     */
    void finishNandAccess();
    /**@}*/

    /**
     * \~english @brief Updates the save manager state
     *
     * \~english @param display Whether the scene is fully displayed
     */
    void update(bool display);

    /**
     * \~english @brief Initializes the banner file binary
     */
    void initBanner();

private:
    /**
     * \~english @brief Bitflag indices
     */
    enum {
        EFlag_NandMemoryExist, //!< Enough NAND memory exists for all files
        EFlag_BannerFileExist, //!< The banner file exists on the NAND
        EFlag_SaveFileExist,   //!< The save file exists on the NAND
        EFlag_3,
        EFlag_DisableSave, //!< Saving is disabled
        EFlag_5,
        EFlag_6,
        EFlag_7,
        EFlag_NandAccess,  //!< NAND operation is happening on another thread
        EFlag_AsyncSave,   //!< Save file is being written on another thread
        EFlag_ErrorWindow, //!< System window error was displayed
        EFlag_11,
        EFlag_NandError, //!< Unrecoverable NAND error
        EFlag_13,
        EFlag_14,
    };

    /**
     * \~english @brief banner.tpl file indices
     */
    enum ETplIndex {
        ETplIndex_BannerTexture, //!< Banner texture
        ETplIndex_IconTexture,   //!< Icon texture
    };

    //! VF work memory size
    static const u32 VF_WORK_SIZE = OS_MEM_KB_TO_B(16);

    //! NAND save file banner size
    // static const u32 SAVE_BANNER_SIZE = NAND_BANNER_SIZE(1);

    //! Number of files written when saving
    static const u32 SAVE_FILE_NUM = 2; // PLACEHOLDER

    //! Number of icon texture frames in the save banner
    static const u32 BANNER_ICON_FRAME_NUM = 1;

private:
    /**
     * \~english @brief Prompts the user to continue without saving
     */
    void checkContinueReject();

    /**
     * \~english @brief Checks if the NAND has enough free space to store both
     * the banner and save files
     */
    void existNandMemorySync() DECOMP_DONT_INLINE;

    /**
     * \~english @brief Checks whether the banner file exists on the NAND
     */
    void existBannerFile();
    /**
     * \~english @brief Creates the banner file on the NAND
     * @details For safety purposes, the banner file is first created in the
     * temporary directory, before being moved out.
     */
    void createBannerFile();
    /**
     * \~english @brief Deletes the banner file from the NAND
     * @details This function only deletes the banner file in the home directory
     */
    void deleteBannerFile();
    /**
     * \~english @brief Saves the banner file to the NAND
     *
     * \~english @param type Which type of banner file to save
     */
    void saveBannerFile(EBannerType type);

    /**
     * \~english @brief Checks whether the save file exists on the NAND
     */
    void existSaveFile();
    /**
     * \~english @brief Deletes the save file from the NAND
     */
    void deleteSaveFile();

    /**
     * @name Thread functions
     */
    /**@{*/
    /**
     * \~english @brief Checks which save data files exist on the NAND
     *
     * \~english @param pArg Thread function argument
     */
    static void existPackFileFunc(void* pArg);
    /**
     * \~english @brief Checks if the NAND has enough free space to store both
     * the banner and save files
     *
     * \~english @param pArg Thread function argument
     */
    static void existNandMemoryFunc(void* pArg);

    /**
     * \~english @brief Creates both the banner and save files on the NAND
     *
     * \~english @param pArg Thread function argument
     */
    static void createPackFileFunc(void* pArg);

    /**
     * \~english @brief Loads the save file from the NAND
     *
     * \~english @param pArg Thread function argument
     */
    static void loadDataFunc(void* pArg);
    /**
     * \~english @brief Saves both the banner and save files to the NAND
     *
     * \~english @param pArg Thread function argument
     */
    static void saveDataFunc(void* pArg);
    /**@}*/

private:
    //! Banner file names
    static const char* BANNER_FILE_NAMES[EBannerType_Max];
    //! Save file name
    static const char* SAVE_FILE_NAME;

    //! Status flags
    EGG::TBitFlag<u32> mFlags; // at 0x8

    //! Last NAND operation result
    s32 mNandErrorCode; // at 0xC
    u32 unk10;
    //! NANDCheck result
    u32 mNandCheckAnswer; // at 0x14

    //! Save file (`RPSports.dat`) size
    u32 mSaveFileSize; // at 0x18
    //! Save banner (`banner.bin`) size
    u32 mBannerFileSize; // at 0x1C
    //! Size of both the save file and banner together
    u32 mClusterFileSize; // at 0x20

    //! Banner file binary
    NANDBanner* mpBannerFile; // at 0x24
    //! Save file binary
    u8* mpSaveFile; // at 0x28

    //! Game save data
    RPSysSaveData* mpSaveData; // at 0x2C
};

//! @}

#endif
