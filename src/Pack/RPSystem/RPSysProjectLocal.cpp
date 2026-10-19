#include <Pack/RPSystem.h>

#include <cstring>

RP_SINGLETON_IMPL(RPSysProjectLocal);

/**
 * \~english @brief Constructor
 *
 * \~english @param pHeap Parent heap
 */
RPSysProjectLocal::RPSysProjectLocal()
    : mRegion(ERegion_NTSC_U),
      mLocale(EArea_USA),
      mLanguage(EArea_USA),
      mFrameRate(EFrameRate_60Hz),
      mSoundStorage(EStorage_DVDStream) {}

/**
 * \~english @brief Destructor
 */
RPSysProjectLocal::~RPSysProjectLocal() {}

/**
 * \~english @brief Appends the locale directory to the specified path
 *
 * \~english @param pPath Path to append to
 * \~english @param pSuffix Optional suffix to append after the locale
 */
void RPSysProjectLocal::appendLocalDirectory(char* pPath, const char* pSuffix) {
    switch (mLocale) {
    case EArea_England: {
        std::strcat(pPath, "EN/");
        break;
    }

    case EArea_France: {
        std::strcat(pPath, "FR/");
        break;
    }

    case EArea_Germany: {
        std::strcat(pPath, "GE/");
        break;
    }

    case EArea_Italy: {
        std::strcat(pPath, "IT/");
        break;
    }

    case EArea_Spain: {
        std::strcat(pPath, "SP/");
        break;
    }

    case EArea_Netherlands: {
        std::strcat(pPath, "EN/");
        break;
    }

    case EArea_Japan: {
        std::strcat(pPath, "JP/");
        break;
    }

    case EArea_USA: {
        std::strcat(pPath, "US/");
        break;
    }

    case EArea_Quebec: {
        std::strcat(pPath, "FU/");
        break;
    }

    case EArea_Latin: {
        std::strcat(pPath, "SU/");
        break;
    }

    case EArea_Korea: {
        std::strcat(pPath, "KR/");
        break;
    }

    case EArea_China: {
        std::strcat(pPath, "CN/");
        break;
    }

    case EArea_Taiwan: {
        std::strcat(pPath, "TW/");
        break;
    }

    default: {
        break;
    }
    }

    // @bug The default argument is an empty string, not NULL
#if defined(BUG_FIX)
    if (pSuffix != "") {
        std::strcat(pPath, pSuffix);
    }
#else
    if (pSuffix != NULL) {
        std::strcat(pPath, pSuffix);
    }
#endif
}

/**
 * \~english @brief Sets the current locale
 *
 * \~english @param locale Locale area
 */
void RPSysProjectLocal::setLocale(EArea locale) {
    mLocale = locale;
}

/**
 * \~english @brief Sets the current language
 *
 * \~english @param lang Language area
 */
void RPSysProjectLocal::setLanguage(EArea lang) {
    mLanguage = lang;
}
