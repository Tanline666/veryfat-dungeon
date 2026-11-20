#ifndef RVL_SDK_GX_HARDWARE_GEN_H
#define RVL_SDK_GX_HARDWARE_GEN_H
#include <types.h>

#include <revolution/GX/GXTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GX_GEN_SET_OPCODE(cmd, opcode)                                         \
    GX_BITSET_FAST((cmd), (opcode), GX_GEN_OPCODE)

#define GX_GEN_OPCODE_SZ 8
#define GX_GEN_OPCODE_SHIFT 24

#ifdef __cplusplus
}
#endif
#endif
