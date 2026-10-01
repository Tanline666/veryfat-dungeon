#ifndef RVL_SDK_OS_CONTEXT_H
#define RVL_SDK_OS_CONTEXT_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OS_CONTEXT_STATE_FP_SAVED = (1 << 0),
} OSContextState;

typedef struct OSContext {
    u32 gprs[32];  // at 0x0
    u32 cr;        // at 0x80
    u32 lr;        // at 0x84
    u32 ctr;       // at 0x88
    u32 xer;       // at 0x8C
    f64 fprs[32];  // at 0x90
    u32 fpscr_pad; // at 0x190
    u32 fpscr;     // at 0x194
    u32 srr0;      // at 0x198
    u32 srr1;      // at 0x19C
    u16 mode;      // at 0x1A0
    u16 state;     // at 0x1A2
    u32 gqrs[8];   // at 0x1A4
    u32 psf_pad;   // at 0x1C4
    f64 psfs[32];  // at 0x1C8
} OSContext;

void OSSaveFPUContext(OSContext* ctx);
void OSSetCurrentContext(OSContext* ctx);
OSContext* OSGetCurrentContext(void);
BOOL OSSaveContext(OSContext* ctx);
void OSLoadContext(OSContext* ctx);
void* OSGetStackPointer(void);
void OSSwitchFiber(void* func, void* stack);
void OSSwitchFiberEx(u32 r3, u32 r4, u32 r5, u32 r6, void* func, void* stack);
void OSClearContext(OSContext* ctx);
void OSInitContext(OSContext* ctx, void* _srr0, void* stack);
void OSDumpContext(const OSContext* ctx);
void __OSContextInit(void);
void OSFillFPUContext(OSContext* ctx);

//! Courtesy to dolsdk2001 team
#define OS_CONTEXT_PSF0 0x1c8
#define OS_CONTEXT_PSF1 0x1d0
#define OS_CONTEXT_PSF2 0x1d8
#define OS_CONTEXT_PSF3 0x1e0
#define OS_CONTEXT_PSF4 0x1e8
#define OS_CONTEXT_PSF5 0x1f0
#define OS_CONTEXT_PSF6 0x1f8
#define OS_CONTEXT_PSF7 0x200
#define OS_CONTEXT_PSF8 0x208
#define OS_CONTEXT_PSF9 0x210
#define OS_CONTEXT_PSF10 0x218
#define OS_CONTEXT_PSF11 0x220
#define OS_CONTEXT_PSF12 0x228
#define OS_CONTEXT_PSF13 0x230
#define OS_CONTEXT_PSF14 0x238
#define OS_CONTEXT_PSF15 0x240
#define OS_CONTEXT_PSF16 0x248
#define OS_CONTEXT_PSF17 0x250
#define OS_CONTEXT_PSF18 0x258
#define OS_CONTEXT_PSF19 0x260
#define OS_CONTEXT_PSF20 0x268
#define OS_CONTEXT_PSF21 0x270
#define OS_CONTEXT_PSF22 0x278
#define OS_CONTEXT_PSF23 0x280
#define OS_CONTEXT_PSF24 0x288
#define OS_CONTEXT_PSF25 0x290
#define OS_CONTEXT_PSF26 0x298
#define OS_CONTEXT_PSF27 0x2A0
#define OS_CONTEXT_PSF28 0x2A8
#define OS_CONTEXT_PSF29 0x2B0
#define OS_CONTEXT_PSF30 0x2B8
#define OS_CONTEXT_PSF31 0x2C0

#ifdef __cplusplus
}
#endif
#endif
