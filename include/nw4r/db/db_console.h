#ifndef NW4R_DB_CONSOLE_H
#define NW4R_DB_CONSOLE_H

#include <nw4r/types_nw4r.h>

#include <nw4r/ut.h>

#include <revolution/OS.h>

namespace nw4r {
namespace db {

/**
 * \~english @brief Output displays for debugging consoles.
 * \~japanese @brief デバッグコンソールの出力表示。
 */
enum ConsoleOutputType {
    CONSOLE_OUTPUT_NONE,     //!< \~english No output \~japanese 出力しない
    CONSOLE_OUTPUT_DISPLAY,  //!< \~english TV screen \~japanese テレビ画面
    CONSOLE_OUTPUT_TERMINAL, //!< \~english Terminal \~japanese ターミナル
    CONSOLE_OUTPUT_ALL,      //!< \~english TV and terminal \~japanese
                             //!< テレビとターミナル
};

namespace detail {
//! \~english Forward declaration required for ConsoleHandle
//! \~japanese ConsoleHandleの必要な前方宣言
typedef struct ConsoleHead ConsoleHead;
} // namespace detail

/**
 * \~english @brief Pointer to detail::ConsoleHead.
 * \~japanese @brief detail::ConsoleHeadへのポインタ。
 */
typedef nw4r::db::detail::ConsoleHead* ConsoleHandle;

namespace detail {

/**
 * \~english @brief Struct that defines debugging output consoles.
 * \~japanese @brief デバッグの出力コンソールを定義するストラクト。
 */
struct ConsoleHead {
    u8* textBuf;    //!< // at 0x0
    u16 width;      //!< \~english Character width per line \~japanese
                    //!< 1行あたりの文字幅 \~ // at 0x4
    u16 height;     //!< // at 0x6
    u16 prio;       //!< \~english Console display priority \~japanese
                    //!< コンソールの表示優先度 \~ // at 0x8
    u16 attr;       //!< // at 0xA
    u16 firstPrint; //!< // at 0xC
    u16 xPosPrint;  //!< // at 0xE
    u16 ringTop;
    s32 ringTopLineCnt; //!< // at 0x14
    s32 viewBaseLine;   //!< // at 0x18
    s16 viewPosX;       //!< // at 0x1C
    s16 viewPosY;       //!< // at 0x1E
    u16 viewHeight;     //!< // at 0x20
    bool isVisible;     //!< // at 0x22
    u8 PAD_0x23;
    nw4r::ut::TextWriter* writer; //!< // at 0x24
    ConsoleHandle next;           //!< // at 0x28
};

} // namespace detail

void Console_DrawDirect(ConsoleHandle console);

/**
 * \~english @brief Prints strings to the console via the specified
 * ConsoleOutputType.
 * \~japanese @brief ConsoleOutputTypeを通じてコンソールへ文字列をプリントする。
 *
 * @param type
 * \~english @param console Console handle
 * \~japanese @param console コンソールのハンドル
 * \~english @param pFmt Formatted string
 * \~japanese @param pFmt 書式文字列
 * \~english @param vList Variable argument list
 * \~japanese @param vList 可変引数リスト
 */
void Console_VFPrintf(ConsoleOutputType type, ConsoleHandle console,
                      const char* fmt, std::va_list vList);

/**
 * \~english @brief Prints strings to both TV and terminal.
 * \~japanese @brief テレビもターミナルに文字列をプリントする。
 *
 * \~english @param console Console handle
 * \~japanese @param console コンソールのハンドル
 * \~english @param pFmt Formatted string
 * \~japanese @param pFmt 書式文字列
 * \~english @param vList Variable argument list
 * \~japanese @param vList 可変引数リスト
 */
inline void Console_VPrintf(ConsoleHandle console, const char* pFmt,
                            va_list vList) {
    Console_VFPrintf(CONSOLE_OUTPUT_ALL, console, pFmt, vList);
}

/**
 * \~english @brief Prints strings to both TV and terminal.
 * \~japanese @brief テレビもターミナルに文字列をプリントする。
 *
 * \~english @param console Console handle
 * \~japanese @param console コンソールのハンドル
 * \~english @param pFmt Formatted string
 * \~japanese @param pFmt 書式文字列
 * \~english @param ... Individual variable arguments
 * \~japanese @param ... 個別の可変引数
 */
void Console_Printf(ConsoleHandle console, const char* pFmt, ...);

inline u16 Console_GetViewHeight(ConsoleHandle console) {
    return console->viewHeight;
}

inline bool Console_SetVisible(ConsoleHandle console, bool isVisible) {
    bool old;
    old = console->isVisible;
    console->isVisible = isVisible;
    return old;
}

inline s32 Console_SetViewBaseLine(ConsoleHandle console, s32 line) {
    s32 old;
    old = console->viewBaseLine;
    console->viewBaseLine = line;
    return old;
}

s32 Console_GetTotalLines(ConsoleHandle console);

inline s32 Console_GetBufferHeadLine(ConsoleHandle console) {
    return console->ringTopLineCnt;
}

inline s32 Console_ShowLatestLine(ConsoleHandle console) {
    s32 latestLine =
        Console_GetTotalLines(console) - Console_GetViewHeight(console);
    if (latestLine < 0) {
        latestLine = 0;
    }
    Console_SetViewBaseLine(console, latestLine);
    return latestLine;
}

/**
 * \~english @brief Creates a console.
 * \~japanese @brief コンソールを作成する。
 *
 * \~english @param pBuffer Buffer of the console to be created
 * \~japanese @param pBuffer 作成するコンソールのバッファ
 * \~english @param width Width of the console to be created
 * \~japanese @param width 作成するコンソールの幅
 * @param height
 * @param viewHeight
 * \~english @param prio Console display priority
 * \~japanese @param prio コンソールの表示優先度
 * @param attr
 * \~english @return The created console's handle
 * \~japanese @return 作成したコンソールのハンドル
 */
ConsoleHandle Console_Create(void* pBuffer, u16 width, u16 height,
                             u16 viewHeight, u16 prio, u16 attr);

/**
 * \~english @brief Destroys the specified console.
 * \~japanese @brief 指定のコンソールを破棄する。
 *
 * \~english @param console Specified console handle
 * \~japanese @param console 指定のコンソールハンドル
 * \~english @return Pointer to the destroyed console's buffer
 * \~japanese @return 破棄したコンソールのバッファへのポインタ
 */
void* Console_Destroy(ConsoleHandle console);

void Console_Clear(ConsoleHandle console);

} // namespace db
} // namespace nw4r

#endif
