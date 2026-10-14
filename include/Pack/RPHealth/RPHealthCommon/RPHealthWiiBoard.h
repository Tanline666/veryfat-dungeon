#ifndef RP_HEALTH_WII_BOARD_H
#define RP_HEALTH_WII_BOARD_H
#include <Pack/types_pack.h>

//! Major WIP! Probably has KPAD or something.
//! Luckily, this appears to be pretty much unchanged in Wii Fit U.
//! mBatteryLevel is at the exact same offset!
class RPHealthWiiBoard {
public:
    void STUB();
    s32 getBatteryLevel();

    //! Tentative privilege
private:
    u8 mUNK_0x0[19392];
    u8 mBatteryLevel; // at 0x4BC0
};

#endif
