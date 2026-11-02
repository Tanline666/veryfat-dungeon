#ifndef RP_SYSTEM_GAME_CONFIG_H
#define RP_SYSTEM_GAME_CONFIG_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>
#include <Pack/RPSystem/RPSysTagParms.h>

//! @addtogroup rp_system
//! @{

/**
 * \~english @brief Pack Project Game Config File (`gameConfig.ini`)
 */
class RPSysGameConfig : public RPSysTagParameters {
    RP_SINGLETON_DECL(RPSysGameConfig);

private:
    //! Scene loaded on boot
    RPSysStringTagParm mRootScene; // at 0xC
    //! TV mode (aspect ratio)
    RPSysStringTagParm mTVMode; // at 0x1C
    //! Game language
    RPSysStringTagParm mLanguage; // at 0x2C
    //! Sound(?)
    RPSysStringTagParm mSound; // at 0x3C
    //! Unknown
    RPSysPrimTagParm<int> mTrainingLevel; // at 0x4C

    //! Common print setting
    RPSysPrimTagParm<int> mRPPrint; // at 0x5C
    //! System print setting
    RPSysPrimTagParm<int> mRPSysPrint; // at 0x6C
    //! Audio print setting
    RPSysPrimTagParm<int> mRPAudPrint; // at 0x7C
    //! Sound print setting
    RPSysPrimTagParm<int> mRPSndPrint; // at 0x8C
    //! User print setting
    RPSysPrimTagParm<int> mRPUserPrint; // at 0x9C
};

//! @}

#endif
