#include <nw4r/db.h>

namespace nw4r {
namespace db {

using namespace detail;

static ConsoleHandle sConsoleList = NULL;
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
    s32 tab = (console->attr & 12) >> 2;
    return (u32)(0x2 << tab);
}

static inline u8* SearchEndOfLine_(const u8* pStr) {
    while (*pStr != '\n' && *pStr != '\0') {
        ++pStr;
    }
    return const_cast<u8*>(pStr);
}

static inline u16 GetRingUsedLines_(ConsoleHandle console) {
    NW4R_NULL_ASSERT(console, 108);
    {
        s32 lines = console->firstPrint - console->ringTop;
        if (lines < 0) {
            lines += console->height;
        }
        return lines;
    }
}

static inline u16 GetActiveLines_(ConsoleHandle console) {
    u16 lines = GetRingUsedLines_(console);
    if (console->xPosPrint > 0) {
        lines++;
    }
    return lines;
}

static void TerminateLine_(ConsoleHandle console) {
    *(GetTextPtr_(console, console->firstPrint, console->xPosPrint)) = '\0';
}

static u8* NextLine_(ConsoleHandle console) {
    *(GetTextPtr_(console, console->firstPrint, console->xPosPrint)) = '\0';

    console->xPosPrint = 0;
    console->firstPrint++;

    if (console->firstPrint == console->height) {
        if (!(console->attr & 2)) {
            console->firstPrint = 0;
        }
    }

    if (console->firstPrint == console->ringTop) {
        ++console->ringTopLineCnt;
        if (++console->ringTop == console->height) {
            console->ringTop = 0;
        }
    }

    return GetTextPtr_(console, console->firstPrint, 0);
}

static u8* PutTab_(ConsoleHandle console, u8* pDst) {
    u32 tabWidth = GetTabSize_(console);

    //! \~english Needs to be a do instead of a direct while statement...
    //! \~japanese 直接的なwhile文の代わりにdo文が必要だー・・・。
    do {
        *pDst++ = ' ';
        console->xPosPrint += 1;
        if (console->xPosPrint >= console->width) {
            break;
        }
    } while (console->xPosPrint & (tabWidth - 1));

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

static ConsoleHandle SearchConsoleFromListByPriority_(u16 prio) {
    ConsoleHandle console = sConsoleList;
    if (!console || console->prio < prio) {
        return NULL;
    }

    while (console->next) {
        if (console->next->prio < prio) {
            return console;
        }
        console = console->next;
    }

    return console;
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

/**
 * \~english @brief Releases the specified mutex.
 * \~japanese @brief 指定のミューテックスを解放する。
 *
 * \~english @param pMutex Pointer to the specified mutex
 * \~japanese @param pMutex 指定のミューテックスへのポインタ
 */
static inline void UnlockMutex_(OSMutex* pMutex) {
    if (OSGetCurrentThread()) {
        OSUnlockMutex(pMutex);
    }
}

/**
 * \~english @brief Adds a new console handle to the console list.
 * \~japanese @brief コンソールリストへ新しいコンソールハンドルを追加する。
 *
 * \~english @param console Handle to be added
 * \~japanese @param console 追加するハンドル
 */
static void AppendConsoleToList_(ConsoleHandle console) {
    NW4R_NULL_ASSERT(console, 320);
    OSLockMutex(&sMutex);
    {
        ConsoleHandle prioConsole =
            SearchConsoleFromListByPriority_(console->prio);

        if (!prioConsole) {
            console->next = sConsoleList;
            sConsoleList = console;
            goto unlock;
        }
        console->next = prioConsole->next;
        prioConsole->next = console;
    }

unlock:
    OSUnlockMutex(&sMutex);
}

static void RemoveConsoleFromList_(ConsoleHandle console) {
    NW4R_NULL_ASSERT(console, 354);
    OSLockMutex(&sMutex);
    {
        ConsoleHandle pConsole = sConsoleList;
        NW4R_NULL_ASSERT(pConsole, 361);

        if (pConsole == console) {
            sConsoleList = console->next;
            console->next = NULL;
            goto unlock;
        }

        while (pConsole->next) {
            if (pConsole->next == console) {
                pConsole->next = console->next;
                console->next = NULL;
                goto unlock;
            }
            pConsole = pConsole->next;
        }
#line 386
        OS_ERROR("illegal console handle"); //!< \~japanese 不正なコンソールハンドル
    }

unlock:
    OSUnlockMutex(&sMutex);

    DECOMP_I_BOMB
}

ConsoleHandle Console_Create(void* buffer, u16 width, u16 height,
                             u16 viewHeight, u16 prio, u16 attr) {
    ConsoleHandle pConsole;

    NW4R_NULL_ASSERT(buffer, 418);

    if (!sInitialized) {
        OSInitMutex(&sMutex);
        sInitialized = TRUE;
    }

    pConsole = reinterpret_cast<ConsoleHandle>(buffer);

    pConsole->textBuf = reinterpret_cast<u8*>(buffer) + sizeof(ConsoleHead);
    pConsole->width = width;
    pConsole->height = height;
    pConsole->prio = prio;
    pConsole->attr = attr;

    pConsole->isVisible = FALSE;

    pConsole->firstPrint = NULL;
    pConsole->xPosPrint = NULL;
    pConsole->ringTop = NULL;
    pConsole->ringTopLineCnt = NULL;
    pConsole->viewBaseLine = NULL;

    pConsole->viewPosX = 30;
    pConsole->viewPosY = 50;
    pConsole->viewHeight = viewHeight;

    pConsole->writer = NULL;
    Console_Clear(pConsole);
    AppendConsoleToList_(pConsole);
    return pConsole;
}

void* Console_Destroy(ConsoleHandle console) {
    NW4R_NULL_ASSERT(console, 468);
    RemoveConsoleFromList_(console);
    return console;
}

void Console_Clear(ConsoleHandle console) {
    NW4R_NULL_ASSERT(console, 488);
    if (!TryLockMutex_(&sMutex)) {
        return;
    }
    BOOL enabled = OSDisableInterrupts();

    console->firstPrint = NULL;
    console->xPosPrint = NULL;
    console->ringTop = NULL;
    console->ringTopLineCnt = NULL;
    console->viewBaseLine = NULL;

    OSRestoreInterrupts(enabled);
    UnlockMutex_(&sMutex);
}

static void DoDrawString_(ConsoleHandle console, u32 printLine, const u8* str,
                          ut::TextWriter* pWriter) {
    if (pWriter) {
        pWriter->Printf("%s\n", str);
    } else {
        s32 height = console->viewPosY + printLine * DIRECT_FONT_HEIGHT;
        DirectPrint_DrawString(console->viewPosX, height, FALSE, "%s\n", str);
    }
}

static void DoDrawConsole_(ConsoleHandle console,
                           nw4r::ut::TextWriter* pWriter) {
    TryLockMutex_(&sMutex);
    {
        s32 viewOffset = console->viewBaseLine - console->ringTopLineCnt;
        u16 line;
        u16 printLines = 0;
        u16 topLine;

        if (viewOffset < 0) {
            viewOffset = 0;
        } else if (viewOffset > GetActiveLines_(console)) {
            goto unlock;
        }

        line = console->ringTop + viewOffset;
        if (line >= console->height) {
            line -= console->height;
        }

        topLine = console->firstPrint + ((console->xPosPrint > 0) ? 1 : 0);
        if (topLine == console->height) {
            topLine = 0;
        }

        while (line != topLine) {
            DoDrawString_(console, printLines, GetTextPtr_(console, line, 0),
                          pWriter);
            ++printLines;

            if (++line == console->height) {
                if (console->attr & 2) {
                    break;
                }
                line = 0;
            }

            if (printLines >= console->viewHeight) {
                break;
            }
        }
    }

unlock:
    UnlockMutex_(&sMutex);
}

void Console_DrawDirect(ConsoleHandle console) {
    NW4R_NULL_ASSERT(console, 681);
    if (!DirectPrint_IsActive()) {
        return;
    }

    if (!console->isVisible) {
        return;
    }

    {
        {
            int width, height;

            width = (int)(console->width * DIRECT_FONT_WIDTH +
                          DIRECT_FONT_WIDTH * 2);
            height = (int)(console->viewHeight * DIRECT_FONT_HEIGHT + 4);
            nw4r::db::DirectPrint_EraseXfb(
                (int)(console->viewPosX - DIRECT_FONT_WIDTH),
                (int)(console->viewPosY - 3), width, height);
        }
        DoDrawConsole_(console, NULL);
        DirectPrint_StoreCache();
    }
}

static void PrintToBuffer_(ConsoleHandle console, const u8* str) {
    u8* pStore;
    NW4R_NULL_ASSERT(console, 802);
    NW4R_NULL_ASSERT(str, 803);
    BOOL enabled = OSDisableInterrupts();

    pStore = GetTextPtr_(console, console->firstPrint, console->xPosPrint);
    while (*str) {
        if (console->attr & 2) {
            if (console->firstPrint == console->height) {
                break;
            }
        }
        while (*str) {
            bool newLineFlag = FALSE;

            if (*str == '\n') {
                str++;
                pStore = NextLine_(console);
                break;
            } else if (*str == '\t') {
                str++;
                pStore = PutTab_(console, pStore);
            } else {
                u32 bytes = PutChar_(console, str, pStore);

                if (bytes > 0) {
                    str += bytes;
                    pStore += bytes;
                } else {
                    newLineFlag = TRUE;
                }
            }
            if (console->xPosPrint >= console->width) {
                newLineFlag = TRUE;
            }

            if (newLineFlag) {
                if (console->attr & 1) {
                    str = SearchEndOfLine_(str);
                } else {
                    if (*str == '\n') {
                        ++str;
                    }
                    pStore = NextLine_(console);
                }
                break;
            }

            if (*str == '\0') {
                TerminateLine_(console);
            }
        }
    }

    OSRestoreInterrupts(enabled);
}

static void Console_PrintString_(ConsoleOutputType type, ConsoleHandle console,
                                 const u8* pStr) {
    NW4R_NULL_ASSERT(console, 908);
    if (type & CONSOLE_OUTPUT_TERMINAL) {
        OSReport("%s", pStr);
    }
    if (type & CONSOLE_OUTPUT_DISPLAY) {
        PrintToBuffer_(console, pStr);
    }
}

void Console_VFPrintf(ConsoleOutputType type, ConsoleHandle console,
                      const char* format, std::va_list vlist) {
    static u8 sStrBuf[1024];

    NW4R_NULL_ASSERT(console, 940);
    if (!TryLockMutex_(&sMutex)) {
        return;
    }

    {
        vsnprintf((char*)sStrBuf, 1024, format, vlist);
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

s32 Console_GetTotalLines(ConsoleHandle console) {
    s32 count;

    NW4R_NULL_ASSERT(console, 1127);

    BOOL enabled = OSDisableInterrupts();
    count = console->ringTopLineCnt + GetActiveLines_(console);
    OSRestoreInterrupts(enabled);

    return count;
}

} // namespace db
} // namespace nw4r
