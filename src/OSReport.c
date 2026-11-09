#include <revolution/OS.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
DECL_WEAK void OSReport(const char* msg, ...) {
#if !defined(PRODUCT)
    va_list vlist;
    va_start(vlist, msg);
    vprintf(msg, vlist);
    va_end(vlist);
#else
#pragma unused(msg)
#endif
}

#ifdef __cplusplus
}
#endif
