#ifndef RP_SYSTEM_GAME_CONFIG_H
#define RP_SYSTEM_GAME_CONFIG_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>
#include <Pack/RPSystem/RPSysTagParms.h>

//! @addtogroup rp_system
//! @{

/**
 * @brief Pack Project Game Config File (`gameConfig.ini`)
 */
class RPSysGameConfig : public RPSysTagParameters {
    RP_SINGLETON_DECL_EX(RPSysGameConfig);

private:
    //! Scene loaded on boot
    RPSysStringTagParm mRootScene; // at 0x10
    //! TV mode (aspect ratio)
    RPSysStringTagParm mTVMode; // at 0x20
    //! Game language
    RPSysStringTagParm mLanguage; // at 0x30
    //! Sound(?)
    RPSysStringTagParm mSound; // at 0x40
    //! Unknown
    RPSysStringTagParm mTrainingLevel; // at 0x50

    //! Common print setting
    RPSysPrimTagParm<int> mRPPrint; // at 0x60
    //! System print setting
    RPSysPrimTagParm<int> mRPSysPrint; // at 0x70
    //! Audio print setting
    RPSysPrimTagParm<int> mRPAudPrint; // at 0x80
    //! Sound print setting
    RPSysPrimTagParm<int> mRPSndPrint; // at 0x90
    //! User print setting
    RPSysPrimTagParm<int> mRPUserPrint; // at 0xA0
};

//! @}

#endif
