#ifndef MSL_PRINTF_H
#define MSL_PRINTF_H
#include <MSL/stdarg.h>
#include <MSL/stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

int vprintf(const char*, va_list);
int sprintf(char*, const char*, ...);
int snprintf(char*, size_t, const char*, ...);
int vsprintf(char*, const char*, va_list);
int vsnprintf(char*, size_t, const char*, va_list);

#ifdef __cplusplus
}
#endif
#endif
