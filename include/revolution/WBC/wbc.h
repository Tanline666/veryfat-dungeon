#ifndef RVL_SDK_WBC_H
#define RVL_SDK_WBC_H
#include <types.h>

#include <revolution/WPAD.h>

#ifdef __cplusplus
extern "C" {
#endif

enum WBCResult {
    WBC_ERR_OK = WPAD_ERR_OK,
    WBC_ERR_NO_CONTROLLER = WPAD_ERR_NO_CONTROLLER,
    WBC_ERR_BUSY = WPAD_ERR_BUSY,
    WBC_ERR_TRANSFER = WPAD_ERR_TRANSFER,
    WBC_ERR_INVALID = WPAD_ERR_INVALID
};

u16 CALB_DATA[12];
u8 wbcInfo[2];
f64 wbc[28];
u8 rxCalib[16];

static BOOL wbc_lib_initialized;
static BOOL crc_table_computed;
static f32 __abs_weight;
static f64 wbc_grav_coff;

void make_crc_table(void);
u32 calc_read_crc(void);
u32 getOffset(void);
u32 getBattThrsh(void);
//! The below are all WPADCallback functions, so they are structured the same
//! way. Names for arguments are tentative
void get_calibration5(s32 chan, s32 result);
void get_calibration3(s32 chan, s32 result);
void get_calibration2(s32 chan, s32 result);
void get_calibration1(s32 chan, s32 result);

void __WBCInit(void);
BOOL WBCSetupCalibration(void);
BOOL WBCGetCalibrationStatus(void);
enum WBCResult WBCSetZEROPoint(f64 pressVal[], u32 size);
s32 WBCRead(WPADBLStatus* pStatus, f64 weight[], u32 size);
s32 WBCGetBatteryLevel(u8 battery);
s32 WBCGetTGCWeight(f64 weightAve, f64* pTGCWeight, WPADBLStatus* pStatus);

#ifdef __cplusplus
}
#endif
#endif
