#ifndef RP_SYSTEM_DVD_STATUS_H
#define RP_SYSTEM_DVD_STATUS_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>
#include <Pack/RPSystem/RPSysProjectLocal.h>

#include <egg/core.h>

//! @addtogroup rp_system
//! @{

/**
 * \~english @brief Disc drive status
 */
class RPSysDvdStatus {
    RP_SINGLETON_DECL(RPSysDvdStatus);

public:
    /**
     * \~english @brief Updates the status of the disc drive
     * \~english @return Whether the disc drive status is OK
     */
    bool update();

    /**
     * \~english @brief Displays the disc drive error message
     */
    void draw();

    /**
     * \~english @brief Tests whether a disc drive error has occurred
     * @note typo
     */
    bool isErrorOccured() const;

private:
    /**
     * \~english @brief Disc drive error type
     */
    enum EErrorStatus {
        EErrorStatus_Success = 0,
        EErrorStatus_Busy,           //!< Drive is busy
        EErrorStatus_NoDisk = -2,    //!< Disk is ejected
        EErrorStatus_WrongDisk = -3, //!< Disk is the wrong game
        EErrorStatus_Retry = -4,     //!< Attempting to recover from error
        EErrorStatus_Fatal = -5,     //!< Fatal error
    };

    enum EErrorHandling {
        EErrorHandling_Default,
        EErrorHandling_Retry,
        EErrorHandling_Fatal,
    };

    //! Number of strings per error message
    static const int ERROR_MESSAGE_STRING_NUM = 2;

    //! Size of the error message buffer
    static const int MESSAGE_BUFFER_SIZE = 256;

private:
    //! Game disc name
    static const char* GAME_DISK_NAME;

    //! DVD error status
    EErrorStatus mErrorStatus; //!< at 0x4
    //! Fader for the background
    EGG::ColorFader* mpErrorFader; //!< at 0x8
};

//! @}

#endif
