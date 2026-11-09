#ifndef NW4R_DB_ASSERT_H
#define NW4R_DB_ASSERT_H

#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace db {

namespace detail {
typedef struct ConsoleHead ConsoleHead;
} // namespace detail

typedef detail::ConsoleHead* ConsoleHandle;

/**
 * \~english @brief Outputs a message to the assertion console
 * and halts the program when specified.
 * \~japanese @brief アサート用のコンソールへメッセージを出力し、
 * プログラムをそう指定した場合に止まる。
 *
 * \~english @param pFile Source file name
 * \~japanese @param pFile ソースファイル名
 * \~english @param line Line number of assertion
 * \~japanese @param line アサートのライン番号
 * \~english @param pFmt Formatted string
 * \~japanese @param pFmt 書式文字列
 * \~english @param vList Variable argument list
 * \~japanese @param vList 可変引数リスト
 * \~english @param halt Specifies whether to halt system after
 * execution (default is TRUE)
 * \~japanese @param halt 実行後にシステムを止まるかどうかを指定する
 * （デフォルトはTRUE）
 *
 * \~english @note When called through Panic, the system always halts.
 * \~japanese @note Panicで呼び出す時には、システムが常に止まる。
 */
void VPanic(const char* pFile, int line, const char* pFmt, va_list vList,
            bool halt = TRUE);
/**
 * \~english @brief Outputs a message to the assertion console
 * and halts the program.
 * \~japanese @brief アサート用のコンソールへメッセージを出力し、
 * プログラムを止まる。
 *
 * \~english @param pFile Source file name
 * \~japanese @param pFile ソースファイル名
 * \~english @param line Line number of assertion
 * \~japanese @param line アサートのライン番号
 * \~english @param pFmt Formatted string
 * \~japanese @param pFmt 書式文字列
 * \~english @param ... Individual variable arguments
 * \~japanese @param ... 個別の可変引数
 */
void Panic(const char* pFile, int line, const char* pFmt, ...);
/**
 * \~english @brief Outputs a warning message to the assertion console.
 * \~japanese @brief アサート用のコンソールへワーニングメッセージを出力する。
 *
 * \~english @param pFile Source file name
 * \~japanese @param pFile ソースファイル名
 * \~english @param line Line number of warning
 * \~japanese @param line ワーニングのライン番号
 * \~english @param pFmt Formatted string
 * \~japanese @param pFmt 書式文字列
 * \~english @param vList Variable argument list
 * \~japanese @param vList 可変引数リスト
 */
void VWarning(const char* pFile, int line, const char* pFmt, va_list vlist);
/**
 * \~english @brief Outputs a warning message to the assertion console.
 * \~japanese @brief アサート用のコンソールへワーニングメッセージを出力する。
 *
 * \~english @param pFile Source file name
 * \~japanese @param pFile ソースファイル名
 * \~english @param line Line number of warning
 * \~japanese @param line ワーニングのライン番号
 * \~english @param pFmt Formatted string
 * \~japanese @param pFmt 書式文字列
 * \~english @param ... Individual variable arguments
 * \~japanese @param ... 個別の可変引数
 */
void Warning(const char* pFile, int line, const char* pFmt, ...);

ConsoleHandle Assertion_SetConsole(ConsoleHandle console);
ConsoleHandle Assertion_GetConsole();
void Assertion_ShowConsole(u32 tick = 0);
void Assertion_HideConsole();
void Assertion_SetWarningTime(u32 tick);
bool Assertion_SetAutoWarning(bool enable);

#ifndef NW4R_ASSERTMSG
#if defined(PRODUCT)
#define NW4R_ASSERTMSG(exp, line, ...) ((void)0)
#else
#define NW4R_ASSERTMSG(exp, line, ...)                                         \
    (void)((exp) || (db::Panic(__FILE__, line, __VA_ARGS__), 0))
#endif
#endif

#ifndef NW4R_ASSERT
#define NW4R_ASSERT(exp, line)                                                 \
    NW4R_ASSERTMSG((exp), (line), "NW4R:Failed assertion " #exp)
#endif

#ifndef NW4R_NULL_ASSERT
#define NW4R_NULL_ASSERT(exp, line)                                            \
    NW4R_ASSERTMSG((exp) != NULL, (line),                                      \
                   "NW4R:Pointer must not be NULL (" #exp ")")
#endif

#ifndef NW4R_MIN_ASSERT
#define NW4R_MIN_ASSERT(exp, min, line)                                        \
    NW4R_ASSERTMSG((exp) >= (min), (line),                                     \
                   #exp " is out of bounds(%d)\n%d <= " #exp                   \
                        " not satisfied.",                                     \
                   static_cast<int>(exp), static_cast<int>(min))
#endif

#ifndef NW4R_ANY_TO_PTR
#define NW4R_ANY_TO_PTR(ptr)                                                   \
    reinterpret_cast<u32>(reinterpret_cast<const void*>(ptr))
#endif

#ifndef NW4R_POINTER_ASSERT
#define NW4R_POINTER_ASSERT(exp, line)                                         \
    NW4R_ASSERTMSG(((NW4R_ANY_TO_PTR(exp) & 0xFF000000) == 0x80000000) ||      \
                       ((NW4R_ANY_TO_PTR(exp) & 0xFF800000) == 0x81000000) ||  \
                       ((NW4R_ANY_TO_PTR(exp) & 0xF8000000) == 0x90000000) ||  \
                       ((NW4R_ANY_TO_PTR(exp) & 0xFF000000) == 0xC0000000) ||  \
                       ((NW4R_ANY_TO_PTR(exp) & 0xFF800000) == 0xC1000000) ||  \
                       ((NW4R_ANY_TO_PTR(exp) & 0xF8000000) == 0xD0000000) ||  \
                       ((NW4R_ANY_TO_PTR(exp) & 0xFFFFC000) == 0xE0000000),    \
                   line,                                                       \
                   "NW4R:Pointer Error\n" #exp "(=%p) is not valid pointer.",  \
                   (exp))
#endif

#ifndef NW4R_WARNING
#if defined(PRODUCT)
#define NW4R_WARNING(exp, line, ...) ((void)0)
#else
#define NW4R_WARNING(exp, line, ...)                                           \
    (void)((exp) || (db::Warning(__FILE__, line, __VA_ARGS__), 0))
#endif
#endif

} // namespace db
} // namespace nw4r

#endif
