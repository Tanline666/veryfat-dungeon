#include <MSL/cstring>

#include <nw4r/db.h>

#include <revolution/BASE.h>
#include <revolution/VI.h>

namespace nw4r {
namespace db {

//! Forward declarations

static void ErrorHandler_(u16 error, OSContext* context, u32 dsisr, u32 dar);

static void* RunThread_(void* arg);

static void DumpException_(const ExceptionCallbackParam* exPtr);

static bool IsValidStackAddr_(u32 addr);

static void ShowStackTrace_(u32 sp);

static void Exception_Printf_(const char* fmt, ...);

static void PrintContext_(u16 error, const OSContext* context, u32 dsisr,
                          u32 dar);

static void ShowMainInfo_(u16 error, const OSContext* context, u32 dsisr,
                          u32 dar);

static void ShowGPR_(const OSContext* pContext);

static void ShowGPRMap_(const OSContext* pContext);

static void ShowSRR0Map_(const OSContext* pContext);

static void ShowFloat_(const OSContext* pContext);

static void ShowFloatSub_(s32 index, f32 f);

static void setFPException(u32 type);

static bool ShowMapInfoSubroutine_(u32 address, bool flag);

static void WaitTime_(s32 time);

static void DrawConsoleEndless_(ConsoleHandle console);

static ExceptionHead sException;
// TODO(texline) why?
static u8 sThreadBuffer[16384];
static OSMessage sMsgBuffer[1];

static const char* CPU_EXP_NAME[] = {
    "SYSTEM RESET",       "MACHINE CHECK",    "DSI",     "ISI",
    "EXTERNAL INTERRUPT", "ALIGNMENT",        "PROGRAM", "FLOATING POINT",
    "DECREMENTER",        "SYSTEM CALL",      "TRACE",   "PERFORMACE MONITOR",
    "BREAK POINT",        "SYSTEM INTERRUPT",
    "THERMAL INTERRUPT", // unused
    "PROTECTION",         "FLOATING POINT"};

void Exception_Init() {
    memset(&sException, 0, sizeof(ExceptionHead));
    sException.callback = NULL;
    sException.console = NULL;
    sException.dispInfo = EXCEPTION_INFO_DEFAULT;
    sException.frameMemory = NULL;
    sException.renderMode = NULL;

    OSCreateThread(&sException.thread, RunThread_, NULL, sThreadBuffer + 16384,
                   16384, 0, OS_THREAD_DETACHED);

    OSInitMessageQueue(&sException.msgQueue, &sMsgBuffer[0], 1);
    OSResumeThread(&sException.thread);

    OSSetErrorHandler(OS_ERR_DSI, (OSErrorHandler)ErrorHandler_);
    OSSetErrorHandler(OS_ERR_ISI, (OSErrorHandler)ErrorHandler_);
    OSSetErrorHandler(OS_ERR_ALIGNMENT, (OSErrorHandler)ErrorHandler_);
    OSSetErrorHandler(OS_ERR_PROTECTION, (OSErrorHandler)ErrorHandler_);

    setFPException(NULL);
}

void ErrorHandler_(u16 error, OSContext* context, u32 dsisr, u32 dar) {
    ExceptionCallbackParam exCallbackObject;

    sException.msr = PPCMfmsr();
    sException.fpscr = context->fpscr;
    OSFillFPUContext(context);
    OSSetErrorHandler(error, NULL);

    if (error == OS_ERR_PROTECTION) {
        OSProtectRange(0, 0, 0, 3);
        OSProtectRange(1, 0, 0, 3);
        OSProtectRange(2, 0, 0, 3);
        OSProtectRange(3, 0, 0, 3);
    }

    exCallbackObject.error = error;
    exCallbackObject.context = context;
    exCallbackObject.dsisr = dsisr;
    exCallbackObject.dar = dar;

    OSSendMessage(&sException.msgQueue, (OSMessage)(&exCallbackObject),
                  OS_MSG_BLOCKING);
    if (OSGetCurrentThread() == NULL) {
        VISetPreRetraceCallback(NULL);
        VISetPostRetraceCallback(NULL);
        u32 msr = PPCMfmsr();
        msr |= MSR_RI;
        PPCMtmsr(msr);

        DumpException_(&exCallbackObject);
    }

    while (OSEnableScheduler() > 0) {
    }

    OSYieldThread();
    OSDisableScheduler();
}

void* RunThread_(void* arg) {
//! OSCreateThread needs a voidptr parameter.
#pragma unused(arg)
    u32 msr = PPCMfmsr();
    msr &= ~(MSR_FE0 | MSR_FE1);
    PPCMtmsr(msr);
    {
        OSMessage msg;

        OSReceiveMessage(&sException.msgQueue, &msg, OS_MSG_BLOCKING);
        OSDisableInterrupts();
        VISetPreRetraceCallback(NULL);
        VISetPostRetraceCallback(NULL);
        if (sException.console) {
            VISetBlack(FALSE);
            VIFlush();
        }
        DumpException_((ExceptionCallbackParam*)msg);
    }
    return NULL;
}

void DumpException_(const ExceptionCallbackParam* exPtr) {
    u16 error = exPtr->error;
    const OSContext* context = exPtr->context;
    u32 dsisr = exPtr->dsisr;
    u32 dar = exPtr->dar;

    if (error < OS_ERR_MAX) {
        sException.stackPointer = context->gprs[1];
    }
    if (sException.console) {
        sException.frameMemory =
            detail::DirectPrint_SetupFB(sException.renderMode);
    }
    while (TRUE) {
        PrintContext_(error, context, dsisr, dar);
        if (sException.console && DirectPrint_IsActive()) {
            Console_DrawDirect(sException.console);
        }
        if (sException.callback) {
            u16 dispInfo = sException.dispInfo;
            while (dispInfo == sException.dispInfo) {
                if (!(*(sException.callback))(sException.console,
                                              sException.callbackArg)) {
                    return;
                }
            }
        } else if (sException.console) {
            DrawConsoleEndless_(sException.console);
        } else {
            return;
        }
    }
}

bool IsValidStackAddr_(u32 addr) {
    const u32 OFFSET_MASK = 0x0FFFFFFF;
    if (addr == NULL || (u32)addr == 0xffffffff) {
        return FALSE;
    }

    if (OSIsMEM1Region(addr)) {
        if ((addr & OFFSET_MASK) >= OSGetPhysicalMem1Size()) {
            return FALSE;
        }
    } else if (OSIsMEM2Region(addr)) {
        if ((addr & OFFSET_MASK) >= OSGetPhysicalMem2Size()) {
            return FALSE;
        }
    } else {
        return FALSE;
    }
    return TRUE;
}

void ShowStackTrace_(u32 sp) {
    Exception_Printf_("-------------------------------- TRACE\n");

    u32 i;
    u32* p;

    Exception_Printf_("Address:   BackChain   LR save\n");
    p = reinterpret_cast<u32*>(sp);

    for (i = 0; i < 16; i++) {
        if (!IsValidStackAddr_(u32(p))) {
            break;
        }
        Exception_Printf_("%08X:  %08X    %08X ", p, p[0], p[1]);

        if (!ShowMapInfoSubroutine_(p[1], false)) {
            Exception_Printf_("\n");
        }

        p = (u32*)*p;
    }

    return;
}

void Exception_Printf_(const char* fmt, ...) {
    const GXRenderModeObj* rMode = sException.renderMode;
    {
        va_list vlist;
        va_start(vlist, fmt);
        OSVReport(fmt, vlist);
        va_end(vlist);
    }
    if (sException.console) {
        void* fb = VIGetCurrentFrameBuffer();
        DirectPrint_ChangeXfb(fb, rMode->fbWidth, rMode->xfbHeight);

        va_list vlist;
        va_start(vlist, fmt);
        Console_VFPrintf(CONSOLE_OUTPUT_DISPLAY, sException.console, fmt,
                         vlist);
        va_end(vlist);
    }
}

void PrintContext_(u16 error, const OSContext* pContext, u32 dsisr, u32 dar) {
    if (error > OS_ERR_MAX) {
        Exception_Printf_("******** USER HALT ********\nFrameMemory:%XH\n",
                          sException.frameMemory);
    } else {
        Exception_Printf_(
            "******** EXCEPTION OCCURRED! ********\nFrameMemory:%XH\n",
            sException.frameMemory);
    }
    {
        if (sException.dispInfo & EXCEPTION_INFO_MAIN) {
            Exception_Printf_("---MainInfo---\n");
            ShowMainInfo_(error, pContext, dsisr, dar);
        }
        if (sException.dispInfo & EXCEPTION_INFO_GPR) {
            Exception_Printf_("---EXCEPTION_INFO_GPR---\n");
            ShowGPR_(pContext);
        }
        if (sException.dispInfo & EXCEPTION_INFO_SRR0MAP) {
            Exception_Printf_("---EXCEPTION_INFO_SRR0MAP---\n");
            ShowSRR0Map_(pContext);
        }
        if (sException.dispInfo & EXCEPTION_INFO_GPRMAP) {
            Exception_Printf_("---EXCEPTION_INFO_GPRMAP---\n");
            ShowGPRMap_(pContext);
        }
        if (sException.dispInfo & EXCEPTION_INFO_FPR) {
            Exception_Printf_("---EXCEPTION_INFO_FPR---\n");
            ShowFloat_(pContext);
            Exception_Printf_(" MSR:%08XH     FPSCR:%08XH\n", sException.msr,
                              sException.fpscr);
        }
        if (sException.dispInfo & EXCEPTION_INFO_TRACE) {
            Exception_Printf_("---EXCEPTION_INFO_TRACE---\n");
            ShowStackTrace_(sException.stackPointer);
        }

        Exception_Printf_("--------------------------------\n");
    }

    DECOMP_I_BOMB
}

void ShowMainInfo_(u16 error, const OSContext* pContext, u32 dsisr, u32 dar) {
    if (error < OS_ERR_MAX) {
        Exception_Printf_("CONTEXT:%08XH  (%s EXCEPTION)\n", pContext,
                          CPU_EXP_NAME[error]);
    } else {
        Exception_Printf_("CONTEXT:%08XH\n", pContext);
    }
    if (error == OS_ERR_FP_EXCEPTION) {
        u32 fpscr = sException.fpscr;
        // TODO(texline): how is this calculated?
        fpscr &= (fpscr << 22) & 0x3E000000 | 0x1F80700;
        if (fpscr & FPSCR_VX) {
            Exception_Printf_(" FPE: Invalid operation\n");
            if (sException.fpscr & FPSCR_VXSNAN) {
                Exception_Printf_(" SNaN\n");
            }
            if (sException.fpscr & FPSCR_VXISI) {
                Exception_Printf_(" Infinity - Infinity\n");
            }
            if (sException.fpscr & FPSCR_VXIDI) {
                Exception_Printf_(" Infinity / Infinity\n");
            }
            if (sException.fpscr & FPSCR_VXZDZ) {
                Exception_Printf_(" 0 / 0\n");
            }
            if (sException.fpscr & FPSCR_VXIMZ) {
                Exception_Printf_(" Infinity * 0\n");
            }
            if (sException.fpscr & FPSCR_VXVC) {
                Exception_Printf_(" Invalid compare\n");
            }
            if (sException.fpscr & FPSCR_VXSOFT) {
                Exception_Printf_(" Software request\n");
            }
            if (sException.fpscr & FPSCR_VXSQRT) {
                Exception_Printf_(" Invalid square root\n");
            }
            if (sException.fpscr & FPSCR_VXCVI) {
                Exception_Printf_(" Invalid integer convert\n");
            }
        }
        if (fpscr & FPSCR_OX) {
            Exception_Printf_(" FPE: Overflow\n");
        }
        if (fpscr & FPSCR_UX) {
            Exception_Printf_(" FPE: Underflow\n");
        }
        if (fpscr & FPSCR_ZX) {
            Exception_Printf_(" FPE: Zero division\n");
        }
        if (fpscr & FPSCR_XX) {
            Exception_Printf_(" FPE: Inexact result\n");
        }
    }
    Exception_Printf_("SRR0:   %08XH   SRR1:%08XH\n", pContext->srr0,
                      pContext->srr1);
    Exception_Printf_("DSISR:  %08XH   DAR: %08XH\n", dsisr, dar);
}

void ShowGPR_(const OSContext* pContext) {
    Exception_Printf_("-------------------------------- GPR\n");
    for (int reg = 0; reg < 10; reg++) {
        Exception_Printf_("R%02d:%08XH  R%02d:%08XH  R%02d:%08XH\n", reg,
                          pContext->gprs[reg], reg + 11,
                          pContext->gprs[reg + 11], reg + 22,
                          pContext->gprs[reg + 22]);
    }
    Exception_Printf_("R%02d:%08XH  R%02d:%08XH\n", 10, pContext->gprs[10],
                      10 + 11, pContext->gprs[10 + 11]);
}

void ShowGPRMap_(const OSContext* pContext) {
    bool drawnFlag = FALSE;

    Exception_Printf_("-------------------------------- GPRMAP\n");
    for (int reg = 0; reg < 31; reg++) {
        u32 address = pContext->gprs[reg];
        if (address < 0x80000000 || 0x82ffffff < address) {
            continue;
        }
        drawnFlag = TRUE;

        Exception_Printf_("R%02d: %08XH ", reg, address);
        if (!ShowMapInfoSubroutine_(address, FALSE)) {
            Exception_Printf_(" no information\n");
        }
    }

    if (drawnFlag == FALSE) {
        Exception_Printf_("  no register which seem to address.\n");
    }

    return;
}

void ShowSRR0Map_(const OSContext* pContext) {
    Exception_Printf_("-------------------------------- SRR0MAP\n");
    u32 srr0 = pContext->srr0;
    if (!(srr0 < 0x80000000 || 0x82FFFFFF < srr0)) {
        Exception_Printf_("SRR0: %08XH ", srr0);

        if (!ShowMapInfoSubroutine_(srr0, FALSE)) {
            Exception_Printf_(" no information\n");
        }
    }
    return;
}

void ShowFloatSub_(s32 index, f32 f) {
    if (isnan(f)) {
        Exception_Printf_("F%02d: Nan      ", index);
    } else if (isinfinite(f)) {
        if (*((u8*)&f) & 0x80) {
            Exception_Printf_("F%02d:+Inf      ", index);
        } else {
            Exception_Printf_("F%02d:-Inf      ", index);
        }
    } else {
        if (f == 0.0f) {
            Exception_Printf_("F%02d: 0.0      ", index);
        } else {
            Exception_Printf_("F%02d:%+.3E", index, f);
        }
    }
}

void ShowFloat_(const OSContext* pContext) {
    Exception_Printf_("-------------------------------- FPR\n");
    for (s32 reg = 0; reg < 10; reg++) {
        ShowFloatSub_(reg, pContext->fprs[reg]);
        Exception_Printf_(" ");
        ShowFloatSub_(reg + 11, pContext->fprs[reg + 11]);
        Exception_Printf_(" ");
        ShowFloatSub_(reg + 22, pContext->fprs[reg + 22]);
        Exception_Printf_("\n");
    }
    ShowFloatSub_(10, pContext->fprs[10]);
    Exception_Printf_(" ");
    ShowFloatSub_(21, pContext->fprs[21]);
    Exception_Printf_("\n");
}

void setFPException(u32 type) {
    __OSFpscrEnableBits = type;
    if (type) {
        (void)OSSetErrorHandler(OS_ERR_FP_EXCEPTION,
                                (OSErrorHandler)ErrorHandler_);

    } else {
        (void)OSSetErrorHandler(OS_ERR_FP_EXCEPTION, NULL);
    }
}

bool ShowMapInfoSubroutine_(u32 address, bool flag) {
    if (!MapFile_Exists()) {
        return FALSE;
    }

    if (address < 0x80000000 || 0x82FFFFFF < address) {
        return FALSE;
    }

    {
        enum { STR_LEN = 260 };
        u8 strBuf[STR_LEN];

        if (MapFile_QuerySymbol(address, strBuf, 260)) {
            if (flag) {
                Exception_Printf_("\n");
            }
            Exception_Printf_("%s\n", strBuf);
            return TRUE;
        } else {
            return FALSE;
        }
    }
}

ConsoleHandle Exception_SetConsole(ConsoleHandle console,
                                   const GXRenderModeObj* pRenderMode) {
    ConsoleHandle old = sException.console;
    sException.console = console;
    sException.renderMode = pRenderMode;
    return old;
}

void Exception_SetUserCallback(ExceptionUserCallback callback, void* pArg) {
    sException.callback = callback;
    sException.callbackArg = pArg;
}

void WaitTime_(s32 time) {
    if (time == 0) {
        return;
    }

    s64 start = OSGetTime();
    while (OS_TICKS_TO_MSEC(OSGetTime() - start) < time) {
    }
}

void DrawConsoleEndless_(ConsoleHandle console) {
    s32 line = Console_GetBufferHeadLine(console);
    while (TRUE) {
        if (line >= Console_GetTotalLines(console)) {
            line = Console_GetBufferHeadLine(console);
        }
        Console_SetViewBaseLine(console, line);
        Console_DrawDirect(console);
        WaitTime_(200);
        line++;
    }
}

} // namespace db
} // namespace nw4r
