#include <nw4r/db.h>

namespace nw4r {
namespace db {

using namespace detail;

static OSMutex sMutex;
static bool sInitialized = FALSE;
static const u32 DIRECT_FONT_WIDTH = 6;
static const u32 DIRECT_FONT_HEIGHT = 10;

static inline u8* GetTextPtr_(ConsoleHandle console, u16 line, u16 xPos) {
    return &console->textBuf[(console->width + 1) * line + xPos];
}

static inline u32 CodeWidth_(const u8* p) {
    return (u32)((*p >= 0x81) ? 2 : 1);
}

static inline u32 GetTabSize_(ConsoleHandle console) {
    s32 tab = (console->UNK_0xA & 12) >> 2;
    return (u32)(0x2 << tab);
}

static inline u8* SearchEndOfLine_(const u8* pStr) {
    while (*pStr != '\n' && *pStr != '\0') {
        ++pStr;
    }
    return const_cast<u8*>(pStr);
}

static void TerminateLine_(ConsoleHandle console) {
    *(GetTextPtr_(console, console->firstPrint, console->xPosPrint)) = '\0';
}

static u8* NextLine_(ConsoleHandle console) {
    // Insert a NULL character at the end of the current line
    *(GetTextPtr_(console, console->firstPrint, console->xPosPrint)) = '\0';
    // Change the output position
    console->xPosPrint = 0;
    console->firstPrint++;
    if (console->firstPrint == console->height) {
        if (!(console->UNK_0xA & 2)) {
            console->firstPrint = 0;
        }
    }
    if (console->firstPrint == console->bufHead) {
        ++console->bufHeadLine;
        if (++console->bufHead == console->height) {
            console->bufHead = 0;
        }
    }
    return GetTextPtr_(console, console->firstPrint, 0);
}

static u8* PutTab_(ConsoleHandle console, u8* pDst) {
    u32 tabWidth = GetTabSize_(console);
    while (console->xPosPrint & (tabWidth - 1)) {
        *pDst++ = ' ';
        console->xPosPrint += 1;
        if (console->xPosPrint >= console->width) {
            break;
        }
    }

    return pDst;
}

static u32 PutChar_(ConsoleHandle console, const u8* str, u8* pDst) {
    u32 codeWidth = CodeWidth_(str);
    u32 cnt;
    if (console->xPosPrint + codeWidth > console->width) {
        return 0;
    }
    console->xPosPrint += codeWidth;
    cnt = codeWidth;
    while (cnt > 0) {
        *pDst++ = *str++;
        cnt--;
    }
    return codeWidth;
}

static inline bool TryLockMutex_(OSMutex* pMutex) {
    if (OSGetCurrentThread()) {
        OSLockMutex(pMutex);
        return TRUE;
    }

    if (pMutex->thread == NULL) {
        return TRUE;
    }

    return FALSE;
}

static inline void UnlockMutex_(OSMutex* pMutex) {
    if (OSGetCurrentThread()) {
        OSUnlockMutex(pMutex);
    }
}

static void PrintToBuffer_(ConsoleHandle console, const u8* pStr) {
    // ---- Buffer area to store characters
    u8* storePtr;

    BOOL enabled = OSDisableInterrupts();

    storePtr = GetTextPtr_(console, console->firstPrint, console->xPosPrint);
    while (*pStr) {
        if (console->UNK_0xA & 2) {
            if (console->firstPrint == console->height) {
                break;
            }
        }
        while (*pStr) {
            bool newLineFlag = false;

            if (*pStr == '\n') {
                pStr++;
                storePtr = NextLine_(console);
                break;
            } else if (*pStr == '\t')
            // Processing tab characters
            {
                pStr++;
                storePtr = PutTab_(console, storePtr);
            } else {
                u32 bytes = PutChar_(console, pStr, storePtr);

                if (bytes > 0) {
                    pStr += bytes;
                    storePtr += bytes;
                } else {
                    newLineFlag = true;
                }
            }
            if (console->xPosPrint >= console->width) {
                newLineFlag = true;
            }

            if (newLineFlag) {
                if (console->UNK_0xA & 1) {
                    pStr = SearchEndOfLine_(pStr);
                } else {
                    if (*pStr == '\n') {
                        ++pStr;
                    }
                    storePtr = NextLine_(console);
                }
                break;
            }

            if (*pStr == '\0') {
                TerminateLine_(console);
            }
        }
    }

    OSRestoreInterrupts(enabled);
}

static void Console_PrintString_(ConsoleOutputType type, ConsoleHandle console,
                                 const u8* pStr) {
    if (type & CONSOLE_OUTPUT_TERMINAL) {
        OSReport("%s", pStr);
    }
    if (type & CONSOLE_OUTPUT_DISPLAY) {
        PrintToBuffer_(console, pStr);
    }
}

void Console_VFPrintf(ConsoleOutputType type, ConsoleHandle console,
                      const char* format, std::va_list vlist) {
    const s32 STRING_BUF_SIZE = 1024;
    static u8 sStrBuf[STRING_BUF_SIZE];

    // Get a lock in order to share sStrBuf and OSReport().
    if (!TryLockMutex_(&sMutex)) {
        return;
    }

    {
        vsnprintf((char*)sStrBuf, STRING_BUF_SIZE, format, vlist);
        Console_PrintString_(type, console, sStrBuf);
    }

    UnlockMutex_(&sMutex);
}

void Console_Printf(ConsoleHandle console, const char* format, ...) {
    va_list vlist;
    va_start(vlist, format);
    Console_VFPrintf(CONSOLE_OUTPUT_ALL, console, format, vlist);
    va_end(vlist);
}

} // namespace db
} // namespace nw4r
