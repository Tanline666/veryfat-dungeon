#ifndef NW4R_DB_EXCEPTION_H
#define NW4R_DB_EXCEPTION_H

#include <nw4r/types_nw4r.h>

#include <nw4r/db/db_console.h>

#include <revolution/GX.h>
#include <revolution/OS.h>

namespace nw4r {
namespace db {

enum ExceptionDisplayInfo {
    EXCEPTION_INFO_MAIN = 1 << 0,
    EXCEPTION_INFO_GPR = 1 << 1,
    EXCEPTION_INFO_GPRMAP = 1 << 2,
    EXCEPTION_INFO_SRR0MAP = 1 << 3,
    EXCEPTION_INFO_FPR = 1 << 4,
    EXCEPTION_INFO_TRACE = 1 << 5,

    EXCEPTION_INFO_ALL = (1 << 6) - 1,
    EXCEPTION_INFO_DEFAULT = EXCEPTION_INFO_MAIN | EXCEPTION_INFO_TRACE
};

struct ExceptionCallbackParam {
    u16 error;          // at 0x0
    u8 PADDING[2];      // at 0x2
    OSContext* context; // at 0x4
    u32 dsisr;          // at 0x8
    u32 dar;            // at 0xC
};

/**
 * \~english @brief Function to be executed upon an exception occurring.
 *
 * \~english @param pConsole Console handle.
 * \~english @param pArg The specified callback argument.
 * \~english @return bool: Always returns true.
 */
typedef bool (*ExceptionUserCallback)(ConsoleHandle pConsole, void* pArg);

struct ExceptionHead {
    OSThread thread;                   // at 0x0
    OSMessageQueue msgQueue;           // at 0x318
    void* frameMemory;                 // at 0x338
    u32 stackPointer;                  // at 0x33C
    ConsoleHandle console;             // at 0x340
    const GXRenderModeObj* renderMode; // at 0x344
    ExceptionUserCallback callback;    // at 0x348
    void* callbackArg;                 // at 0x34C
    u32 msr;                           // at 0x350
    u32 fpscr;                         // at 0x354
    u16 dispInfo;                      // at 0x358
    u8 PADDING[6];                     // at 0x35A
};

/**
 * \~english @brief Creates a thread with the highest priority to handle
 * exceptions.
 * \~japanese @brief 例外を処理するために一番高い優先順位のスレッドを作成する。
 */
void Exception_Init();

/**
 * \~english @brief Sets the console for displaying exception info.
 * \~japanese @brief 例外情報を表示するコンソールを設定する。
 *
 * \~english @param console The specified console handle.
 * \~english @param renderMode Render mode pointer, stored as a GXRenderModeObj.
 * \~english @return ConsoleHandle: The console prior to executing the function.
 */
ConsoleHandle Exception_SetConsole(ConsoleHandle console,
                                   const GXRenderModeObj* pRenderMode);

/**
 * \~english @brief Returns the currently set console for displaying exception
 * info.
 *
 * \~english @return ConsoleHandle: Ditto.
 */
ConsoleHandle Exception_GetConsole();

/**
 * \~english @brief Sets the function to be executed upon an exception
 * occurring.
 *
 * \~english @param callback Callback function itself.
 * \~english @param arg Argument to be passed to callback.
 */
void Exception_SetUserCallback(ExceptionUserCallback callback, void* pArg);

/**
 * \~english @brief Sets which info is to be displayed/reported on screen or by
 * serial. Defaults to Main and Trace.
 *
 * \~english @param info The new display info setting.
 * \~english @return ExceptionDisplayInfo: The prior display info setting.
 */
u16 Exception_SetDisplayInfo(u16 pInfo);
} // namespace db
} // namespace nw4r

#endif
