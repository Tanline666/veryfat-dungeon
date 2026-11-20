#ifndef RVL_SDK_GX_PERF_H
#define RVL_SDK_GX_PERF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

void GXSetGPMetric(u32 perf0, u32 perf1);
void GXClearGPMetric(void);

#ifdef __cplusplus
}
#endif
#endif
