#ifndef NW4R_DB_ASSERT_H
#define NW4R_DB_ASSERT_H

#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace db {

namespace detail {
typedef struct ConsoleHead ConsoleHead;
} // namespace detail

typedef detail::ConsoleHead* ConsoleHandle;

void VPanic(const char* pFile, int line, const char* pFmt, va_list vList,
            bool halt = TRUE);
void Panic(const char* pFile, int line, const char* pFmt, ...);
void VWarning(const char* pFile, int line, const char* pFmt, va_list vlist);
void Warning(const char* pFile, int line, const char* pFmt, ...);

ConsoleHandle Assertion_SetConsole(ConsoleHandle console);
ConsoleHandle Assertion_GetConsole();
void Assertion_ShowConsole(u32 tick = 0);
void Assertion_HideConsole();
void Assertion_SetWarningTime(u32 tick);
bool Assertion_SetAutoWarning(bool enable);

} // namespace db
} // namespace nw4r

#endif
