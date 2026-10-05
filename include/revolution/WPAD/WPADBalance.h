#ifndef RVL_SDK_WPAD_BALANCE_H
#define RVL_SDK_WPAD_BALANCE_H
#include <types.h>

#include <revolution/WPAD/WPAD.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Sourced from Charm Girls Club: Pajama Party (released October 2009 with
//! DWARF v2, same month as Wii Fit Plus)
typedef struct WPADBLStatus {
    u16 button;                          // at 0x0
    s16 accX;                            // at 0x2
    s16 accY;                            // at 0x4
    s16 accZ;                            // at 0x6
    DPDObject obj[WPAD_MAX_DPD_OBJECTS]; // at 0x8
    u8 dev;                              // at 0x28
    s8 err;                              // at 0x29
    u16 press[4];                        // at 0x2A
    s8 temp;                             // at 0x32
    u8 battery;                          // at 0x33
} WPADBLStatus;

s32 WPADGetBLCalibration(s32 chan, u8* pData, u16 address, u16 length,
                         WPADCallback callback);

#ifdef __cplusplus
}
#endif
#endif
