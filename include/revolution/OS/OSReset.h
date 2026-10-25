/**
 * @addtogroup rvl_os
 * @{
 */
#ifndef RVL_SDK_OS_RESET_H
#define RVL_SDK_OS_RESET_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL (*OSShutdownFunction)(BOOL final, u32 event);

typedef enum {
    OS_SD_EVENT_FATAL,
    OS_SD_EVENT_1,
    OS_SD_EVENT_SHUTDOWN,
    OS_SD_EVENT_3,
    OS_SD_EVENT_RESTART,
    OS_SD_EVENT_RETURN_TO_MENU,
    OS_SD_EVENT_LAUNCH_APP,
} OSShutdownEvent;

typedef struct OSShutdownFunctionInfo {
    OSShutdownFunction func;             // at 0x0
    u32 prio;                            // at 0x4
    struct OSShutdownFunctionInfo* next; // at 0x8
    struct OSShutdownFunctionInfo* prev; // at 0xC
} OSShutdownFunctionInfo;

typedef struct OSShutdownFunctionQueue {
    OSShutdownFunctionInfo* head; // at 0x0
    OSShutdownFunctionInfo* tail; // at 0x4
} OSShutdownFunctionQueue;

void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info);
BOOL __OSCallShutdownFunctions(u32 pass, u32 event);
void __OSShutdownDevices(u32 event);
void __OSGetDiscState(u8* out);
void OSShutdownSystem(void);
void OSReturnToMenu(void);
u32 OSGetResetCode(void);
/**
 * \~english @deprecated Resets the console. Superseded by OSRestart,
 * OSRebootSystem, and OSReturnToMenu. Because of this, it halts the system if
 * called.
 * \~japanese @deprecated 本体をリセットする。OSRestart、OSRebootSystem、
 * OSReturnToMenuに取って代われたから、この関数を呼び出したら本体が止まる。
 *
 * \~ @param arg0 Dummy
 * \~ @param arg1 Dummy
 * \~ @param arg2 Dummy
 */
void OSResetSystem(u32 arg0, u32 arg1, u32 arg2);

#ifdef __cplusplus
}
#endif
#endif
/**@} */
