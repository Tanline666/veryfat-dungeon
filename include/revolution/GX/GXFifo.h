#ifndef RVL_SDK_GX_FIFO_H
#define RVL_SDK_GX_FIFO_H
#include <types.h>

#include <revolution/GX/GXInternal.h>
#include <revolution/OS.h>

#ifdef __cplusplus
extern "C" {
#endif

GX_PUBLIC_STRUCT_DECL(GXFifoObj, 128);

typedef void (*GXBreakPtCallback)(void);

void GXGetGPStatus(GXBool* overhi, GXBool* underlow, GXBool* readIdle,
                   GXBool* cmdIdle, GXBool* brkpt);

void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size);

void GXSetCPUFifo(GXFifoObj*);
BOOL GXGetCPUFifo(GXFifoObj*);
void GXSetGPFifo(GXFifoObj*);
BOOL GXGetGPFifo(GXFifoObj*);

void GXGetFifoPtrs(const GXFifoObj*, void** readPtr, void** writePtr);

u32 GXGetFifoCount(GXFifoObj*);
u8 GXGetFifoWrap(GXFifoObj*);

GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback callback);

void GXEnableBreakPt(void* writePtr);
void GXDisableBreakPt(void);

OSThread* GXGetCurrentGXThread(void);

void __GXFifoInit(void);

#ifdef __cplusplus
}
#endif
#endif
