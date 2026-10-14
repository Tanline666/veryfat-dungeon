#include <Pack/RPHealth.h>

#include <revolution/WBC.h>

s32 RPHealthWiiBoard::getBatteryLevel() {
    return WBCGetBatteryLevel(mBatteryLevel);
}
