#include <nw4r/db.h>

#include <revolution/BASE.h>
#include <revolution/VI.h>

namespace nw4r {
namespace db {

static u32 sWarningTime = 0;
static ConsoleHandle sAssertionConsole = NULL;
static bool sDispWarningAuto = TRUE;

static OSAlarm& GetWarningAlarm_();
static void WarningAlarmFunc_(OSAlarm* pAlarm, OSContext* pContext);

static void Assertion_Printf_(const char* pFmt, ...) {
    va_list vList;
    va_start(vList, pFmt);
    if (sAssertionConsole) {
        Console_VPrintf(sAssertionConsole, pFmt, vList);
    } else {
        OSVReport(pFmt, vList);
    }
    va_end(vList);
}

static bool ShowMapInfoSubroutine_(u32 addr, bool flag) {
    if (!MapFile_Exists()) {
        return false;
    }

    // ---- Indicates whether or not this is to be taken as an address
    if (addr < 0x80000000 || 0x82ffffff < addr) {
        return false;
    }

    {
        u8 strBuf[260];
        bool result;

        result = MapFile_QuerySymbol(addr, strBuf, 260);
        if (result) {
            if (flag) {
                Assertion_Printf_("\n");
            }
            Assertion_Printf_("%s\n", strBuf);
            return true;
        } else {
            return false;
        }
    }
}

static void ShowStack_(u32 sp) {
    Assertion_Printf_("-------------------------------- TRACE\n");

    u32 i;
    u32* pP;
    Assertion_Printf_("Address:   BackChain   LR save\n");
    pP = (u32*)sp;

    for (i = 0; i < 16; i++) {
        if (pP == NULL || (u32)pP == 0xffffffff ||
            ((u32)pP & 0x80000000) == 0) {
            break;
        }
        Assertion_Printf_("%08X:  %08X    %08X ", pP, pP[0], pP[1]);
        if (!ShowMapInfoSubroutine_(pP[1], false)) {
            Assertion_Printf_("\n");
        }
        pP = (u32*)*pP;
    }

    return;
}

DECL_WEAK void VPanic(const char* pFile, int line, const char* pFmt,
                      va_list vList, bool halt) {

    //! sp is a reserved name. Using it causes compiler error
    register u32 stackPtr;
    asm {
        mr stackPtr, r1;
    }
    stackPtr = *((u32*)stackPtr);

    OSDisableInterrupts();
    OSDisableScheduler();
    VISetPreRetraceCallback(NULL);
    VISetPostRetraceCallback(NULL);
    if (sAssertionConsole) {
        detail::DirectPrint_SetupFB(NULL);
    }

    ShowStack_(stackPtr);

    if (sAssertionConsole) {
        Console_Printf(sAssertionConsole, "%s:%d Panic:", pFile, line);
        Console_VPrintf(sAssertionConsole, pFmt, vList);
        Console_Printf(sAssertionConsole, "\n");

        Console_ShowLatestLine(sAssertionConsole);
        Console_SetVisible(sAssertionConsole, TRUE);
        Console_DrawDirect(sAssertionConsole);
    } else {
        OSReport("%s:%d Panic:", pFile, line);
        OSVReport(pFmt, vList);
        OSReport("\n");
    }

    if (halt) {
        PPCHalt();
    }
}

DECL_WEAK void Panic(const char* pFile, int line, const char* pFmt, ...) {
    va_list vList;
    va_start(vList, pFmt);
    VPanic(pFile, line, pFmt, vList);
    va_end(vList);
    //! This needs to be called separately even if VPanic's halt flag is set to
    //! true. Not sure why (texline)
    PPCHalt();
}

DECL_WEAK void VWarning(const char* pFile, int line, const char* pFmt,
                        va_list vList) {
    if (sAssertionConsole) {
        Console_Printf(sAssertionConsole, "%s:%d Warning:", pFile, line);
        Console_VPrintf(sAssertionConsole, pFmt, vList);
        Console_Printf(sAssertionConsole, "\n");
        Console_ShowLatestLine(sAssertionConsole);
        if (sDispWarningAuto) {
            Assertion_ShowConsole(sWarningTime);
        }
    } else {
        OSReport("%s:%d Warning:", pFile, line);
        OSVReport(pFmt, vList);
        OSReport("\n");
    }
}

void Assertion_ShowConsole(u32 tick) {
    if (!sAssertionConsole) {
        return;
    }

    OSAlarm& rAlarm = GetWarningAlarm_();
    OSCancelAlarm(&rAlarm);
    Console_SetVisible(sAssertionConsole, TRUE);
    if (tick > 0) {
        OSSetAlarm(&rAlarm, tick, WarningAlarmFunc_);
    }
}

static OSAlarm& GetWarningAlarm_() {
    static bool sInitializedAlarm = FALSE;
    static OSAlarm sWarningAlarm;

    if (!sInitializedAlarm) {
        OSCreateAlarm(&sWarningAlarm);
        sInitializedAlarm = TRUE;
    }

    return sWarningAlarm;
}

static void WarningAlarmFunc_(OSAlarm* pAlarm, OSContext* pContext) {
#pragma unused(pAlarm, pContext)
    if (sAssertionConsole) {
        Console_SetVisible(sAssertionConsole, FALSE);
    }
}

} // namespace db
} // namespace nw4r
