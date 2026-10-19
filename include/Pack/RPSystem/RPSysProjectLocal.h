#ifndef RP_SYSTEM_PROJECT_LOCAL_H
#define RP_SYSTEM_PROJECT_LOCAL_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>
#include <Pack/RPSystem/RPSysSceneCreator.h>

#include <egg/core.h>

//! @addtogroup rp_system
//! @{

/**
 * \~english @brief Runtime project localization
 */
class RPSysProjectLocal {
    RP_SINGLETON_DECL(RPSysProjectLocal);

public:
    /**
     * \~english @brief Game region
     */
    enum ERegion {
        ERegion_NTSC_J, //!< Japan
        ERegion_NTSC_U, //!< Americas/Asia
        ERegion_PAL,    //!< Europe/Oceania
        ERegion_KOR     //!< Korea
    };

    /**
     * \~english @brief World area
     */
    enum EArea {
        EArea_England,
        EArea_France,
        EArea_Germany,
        EArea_Italy,
        EArea_Spain,
        EArea_Netherlands,
        EArea_Japan,
        EArea_USA,
        EArea_Quebec,
        EArea_Latin,
        EArea_Korea,
        EArea_China,
        EArea_Taiwan,
    };

    /**
     * \~english @brief Target framerate
     */
    enum EFrameRate {
        EFrameRate_60Hz, //!< NTSC/PAL60 (60Hz)
        EFrameRate_50Hz, //!< PAL50 (50Hz)
    };

    /**
     * \~english @brief Storage device
     */
    enum EStorage {
        //! Sound archives are read from the DVD as needed
        EStorage_DVDStream,
        //! Sound archives are read from the NAND ahead of time
        EStorage_NANDBuffer,
        //! Sound archives are read from the DVD ahead of time
        EStorage_DVDBuffer
    };

public:
    /**
     * \~english @brief Appends the locale directory to the given path
     *
     * \~english @param pPath Path to append to
     * \~english @param pSuffix Optional suffix to append after the locale
     */
    void appendLocalDirectory(char* pPath, const char* pSuffix = "");

    /**
     * \~english @brief Sets the current locale
     *
     * \~english @param locale Locale area
     */
    void setLocale(EArea locale);

    /**
     * \~english @brief Sets the current language
     *
     * \~english @param lang Language area
     */
    void setLanguage(EArea lang);

    /**
     * \~english @brief Gets the current game region
     */
    ERegion getRegion() const {
        return mRegion;
    }

    /**
     * \~english @brief Gets the current game locale
     */
    EArea getLocale() const {
        return mLocale;
    }

    /**
     * \~english @brief Gets the current game language
     */
    EArea getLanguage() const {
        return mLanguage;
    }

    /**
     * \~english @brief Gets the target framerate
     */
    EFrameRate getFrameRate() const {
        return mFrameRate;
    }

    /**
     * \~english @brief Gets the storage device where sound data is located
     */
    EStorage getSoundStorage() const {
        return mSoundStorage;
    }

private:
    //! Game region
    ERegion mRegion; // at 0x8
    //! Game locale
    EArea mLocale; // at 0x10
    //! Game language
    EArea mLanguage; // at 0x14
    //! Target framerate
    EFrameRate mFrameRate; // at 0x18
    //! Sound archive storage device
    EStorage mSoundStorage; // at 0x1C
};

//! @}

#endif
