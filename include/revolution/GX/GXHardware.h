/**
 * For more details, see:
 * https://www.gc-forever.com/yagcd/chap8.html#sec8
 * https://www.gc-forever.com/yagcd/chap5.html#sec5
 * https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/VideoCommon/BPMemory.h
 * https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/VideoCommon/XFMemory.h
 * https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/VideoCommon/OpcodeDecoding.h
 * https://patents.google.com/patent/US6700586B1/en
 * https://patents.google.com/patent/US6639595B1/en
 * https://patents.google.com/patent/US7002591
 * https://patents.google.com/patent/US6697074
 */

#ifndef RVL_SDK_GX_HARDWARE_H
#define RVL_SDK_GX_HARDWARE_H
#include <types.h>

#include <revolution/GX/GXTypes.h>
#ifdef __cplusplus
extern "C" {
#endif

/************************************************************
 *
 *
 * GX FIFO
 *
 *
 ***********************************************************/

/**
 * FIFO write/gather pipe
 */
extern volatile union {
    // 1-byte
    char c;
    unsigned char uc;
    // 2-byte
    short s;
    unsigned short us;
    // 4-byte
    int i;
    unsigned int ui;
    void* p;
    float f;
} WGPIPE DECL_ADDRESS(0xCC008000);

extern volatile void* __piReg;
extern volatile void* __cpReg;
extern volatile void* __peReg;
extern volatile void* __memReg;

/**
 * FIFO commands
 */
typedef enum {
    GX_FIFO_CMD_NOOP = 0x00,

    GX_FIFO_CMD_LOAD_BP_REG = 0x61,
    GX_FIFO_CMD_LOAD_CP_REG = 0x08,
    GX_FIFO_CMD_LOAD_XF_REG = 0x10,

    GX_FIFO_CMD_LOAD_INDX_A = 0x20,
    GX_FIFO_CMD_LOAD_INDX_B = 0x28,
    GX_FIFO_CMD_LOAD_INDX_C = 0x30,
    GX_FIFO_CMD_LOAD_INDX_D = 0x38,

    GX_FIFO_CMD_CALL_DL = 0x40,
    GX_FIFO_CMD_INVAL_VTX = 0x09,

    GX_FIFO_CMD_DRAW_POINTS = GX_POINTS,
    GX_FIFO_CMD_DRAW_LINES = GX_LINES,
    GX_FIFO_CMD_DRAW_LINESTRIP = GX_LINESTRIP,
    GX_FIFO_CMD_DRAW_TRIANGLES = GX_TRIANGLES,
    GX_FIFO_CMD_DRAW_TRIANGLESTRIP = GX_TRIANGLESTRIP,
    GX_FIFO_CMD_DRAW_TRIANGLEFAN = GX_TRIANGLEFAN,
    GX_FIFO_CMD_DRAW_QUADS = GX_QUADS,
} GXFifoCmd;

#define GX_CP_CMD_TO_OPCODE(index, cmd) (((u32)(index)) | (((u32)(cmd)) << 3))
#define GX_CP_STREAM_REG_ADDR(index, addr)                                     \
    (((u32)(index)) | (((u32)(addr)) << 4))

#define GX_WRITE_S8(ch) WGPIPE.c = (s8)(ch)
#define GX_WRITE_U8(uch) WGPIPE.uc = (u8)(uch)
#define GX_WRITE_U16(ushor) WGPIPE.us = (u16)(ushor)
#define GX_WRITE_U32(ulong) WGPIPE.ui = (u32)(ulong)
#define GX_WRITE_F32(fp) WGPIPE.f = (f32)(fp)

#define GX_CP_REG_WRITE_U16(a, d)                                              \
    *(volatile u16*)((volatile u16*)__cpReg + (a)) = (u16)(d)
#define GX_CP_REG_READ_U16(a) *(volatile u16*)((volatile u16*)__cpReg + (a))

#define GX_MEM_REG_WRITE_U16(a, d) *(vu16*)((vu16*)__memReg + (a)) = (u16)(d)
#define GX_MEM_REG_READ_U16(a) *(volatile u16*)((volatile u16*)__memReg + (a))

#define GX_PE_REG_WRITE_U16(a, d)                                              \
    *(volatile u16*)((volatile u16*)__peReg + (a)) = (u16)(d)
#define GX_PE_REG_READ_U16(a) *(volatile u16*)((volatile u16*)__peReg + (a))

/**
 * FIFO command sizes
 */
#define GX_FIFO_CMD_LOAD_INDX_SIZE 5
#define GX_FIFO_CMD_DRAW_SIZE 3

#define __GX_FIFO_SET_LOAD_INDX_DST(reg, x) ((reg) = GX_BITSET(reg, 0, 12, x))
#define __GX_FIFO_SET_LOAD_INDX_NELEM(reg, x) ((reg) = GX_BITSET(reg, 12, 4, x))
#define __GX_FIFO_SET_LOAD_INDX_INDEX(reg, x)                                  \
    ((reg) = GX_BITSET(reg, 16, 16, x))

#define __GX_FIFO_LOAD_INDX(reg, dst, nelem, index)                            \
    {                                                                          \
        u32 cmd = 0;                                                           \
        __GX_FIFO_SET_LOAD_INDX_DST(cmd, dst);                                 \
        __GX_FIFO_SET_LOAD_INDX_NELEM(cmd, nelem);                             \
        __GX_FIFO_SET_LOAD_INDX_INDEX(cmd, index);                             \
        WGPIPE.c = reg;                                                        \
        WGPIPE.i = cmd;                                                        \
    }

#define GX_FIFO_LOAD_INDX_A(dst, nelem, index)                                 \
    __GX_FIFO_LOAD_INDX(GX_FIFO_CMD_LOAD_INDX_A, dst, nelem, index)

#define GX_FIFO_LOAD_INDX_B(dst, nelem, index)                                 \
    __GX_FIFO_LOAD_INDX(GX_FIFO_CMD_LOAD_INDX_B, dst, nelem, index)

#define GX_FIFO_LOAD_INDX_C(dst, nelem, index)                                 \
    __GX_FIFO_LOAD_INDX(GX_FIFO_CMD_LOAD_INDX_C, dst, nelem, index)

#define GX_FIFO_LOAD_INDX_D(dst, nelem, index)                                 \
    __GX_FIFO_LOAD_INDX(GX_FIFO_CMD_LOAD_INDX_D, dst, nelem, index)

/************************************************************
 *
 *
 * GX Blitting Processor (BP)
 *
 *
 ***********************************************************/

/**
 * Load immediate value into BP register
 */
#define GX_BP_LOAD_REG(data)                                                   \
    WGPIPE.c = GX_FIFO_CMD_LOAD_BP_REG;                                        \
    WGPIPE.i = (data);

/**
 * Set BP command opcode (first 8 bits)
 */
#define GX_BP_SET_OPCODE(cmd, opcode)                                          \
    GX_BITSET_FAST((cmd), (opcode), GX_BP_OPCODE)

#define GX_BP_OPCODE_SZ 12
#define GX_BP_OPCODE_SHIFT 24
#define GX_BP_CMD_SZ (sizeof(u8) + sizeof(u32))

/************************************************************
 *
 *
 * GX Command Processor (CP)
 *
 *
 ***********************************************************/

/**
 * Load immediate value into CP register
 */
#if defined(DEBUG)
#define GX_CP_LOAD_REG(addr, vtxfmt, data)                                     \
    {                                                                          \
        s32 regAddr;                                                           \
        GX_WRITE_U8(GX_CP_CMD_TO_OPCODE(0, GX_CP_CMD_LOADREGS));               \
        GX_WRITE_U8(GX_CP_STREAM_REG_ADDR((vtxfmt), (addr)));                  \
        GX_WRITE_U32((data));                                                  \
        regAddr = (vtxfmt) - GX_POS_MTX_ARRAY + GX_VA_POS;                     \
        if ((addr) == GX_BP_REG_SETMODE0_TEX4) {                               \
            if (regAddr >= 0 && regAddr < 4) {                                 \
                gx->indexBase[regAddr] = (data);                               \
            }                                                                  \
        } else if ((addr) == GX_BP_REG_SETIMAGE2_TEX4) {                       \
            if (regAddr >= 0 && regAddr < 4)                                   \
                gx->indexStride[regAddr] = (data);                             \
        }                                                                      \
    }
#else
#define GX_CP_LOAD_REG(addr, vtxfmt, data)                                     \
    GX_WRITE_U8(GX_CP_CMD_TO_OPCODE(0, GX_CP_CMD_LOADREGS));                   \
    GX_WRITE_U8(GX_CP_STREAM_REG_ADDR((vtxfmt), (addr)));                      \
    GX_WRITE_U32((data));
#endif

#define GX_CP_CMD_SZ (sizeof(u8) + sizeof(u8) + sizeof(u32))

#define GX_TX_PERFMODE(sel3, sel2, sel1, sel0, opcode)                         \
    ((((u32)(sel3)) << 0) | (((u32)(sel2)) << 5) | (((u32)(sel1)) << 10) |     \
     (((u32)(sel0)) << 15) | (((u32)(opcode)) << 24))

/************************************************************
 *
 *
 * GX Transform Unit (XF)
 *
 *
 ***********************************************************/

/**
 * XF memory
 */
typedef enum {
    GX_XF_MEM_POSMTX = 0x0000,
    GX_XF_MEM_NRMMTX = 0x0400,
    GX_XF_MEM_DUALTEXMTX = 0x0500,
    GX_XF_MEM_LIGHTOBJ = 0x0600
} GXXfMem;

/**
 * Header for an XF register load
 */
#define GX_XF_LOAD_REG_HDR(addr, cnt)                                          \
    GX_WRITE_U8(GX_CP_CMD_TO_OPCODE(0, GX_CP_CMD_XF_LOADREGS));                \
    GX_WRITE_U32(((addr) | (cnt) << 16))

/**
 * Load immediate value into XF register
 */
#define GX_XF_LOAD_REG(addr, data, cnt)                                        \
    GX_XF_LOAD_REG_HDR(addr, cnt);                                             \
    GX_WRITE_U32((data));

#define GX_XF_CMD_SZ (sizeof(u8) + sizeof(u32) + sizeof(u32))

#define GX_DEFINE_GX_READ_COUNTER(unit)                                        \
    inline u32 __GXRead##unit##CounterU32(u32 regAddrL, u32 regAddrH) {        \
        u32 ctrH0, ctrH1, ctrL;                                                \
        ctrH0 = GX_##unit##_REG_READ_U16(regAddrH);                            \
        do {                                                                   \
            ctrH1 = ctrH0;                                                     \
            ctrL = GX_##unit##_REG_READ_U16(regAddrL);                         \
            ctrH0 = GX_##unit##_REG_READ_U16(regAddrH);                        \
        } while (ctrH0 != ctrH1);                                              \
        return ((ctrH0 << 16) | ctrL);                                         \
    }

GX_DEFINE_GX_READ_COUNTER(CP);
GX_DEFINE_GX_READ_COUNTER(PE);
GX_DEFINE_GX_READ_COUNTER(MEM);

#define GX_CP_COUNTER_READ_U32(name) __GXReadCPCounterU32(name##L, name##H)

#define GX_PE_COUNTER_READ_U32(name)                                           \
    __GXReadPECounterU32(name##L_ID, name##H_ID)

#define GX_MEM_COUNTER_READ_U32(name)                                          \
    __GXReadMEMCounterU32(name##L_IDX, name##H_IDX)

/**
 * Enums for Tex0-Tex7 register fields
 */
typedef enum {
    GX_XF_TEX_PROJ_ST, // (s,t): texmul is 2x4
    GX_XF_TEX_PROJ_STQ // (s,t,q): texmul is 3x4
} GXXfTexProj;

typedef enum {
    GX_XF_TEX_FORM_AB11, // (A, B, 1.0, 1.0) (used for regular texture source)
    GX_XF_TEX_FORM_ABC1  // (A, B, C, 1.0) (used for geometry or normal source)
} GXXfTexForm;

typedef enum {
    GX_XF_TG_REGULAR, // Regular transformation (transform incoming data)
    GX_XF_TG_BUMP,    // Texgen bump mapping

    GX_XF_TG_CLR0, // Color texgen: (s,t)=(r,g:b) (g and b are concatenated),
                   // color0

    GX_XF_TG_CLR1 // Color texgen: (s,t)=(r,g:b) (g and b are concatenated),
                  // color1
} GXXfTexGen;

/**
 * Misc. hardware enums
 */
typedef enum {
    GX_RAS_COLOR0A0,
    GX_RAS_COLOR1A1,
    GX_RAS_ALPHA_BUMP = 5,
    GX_RAS_ALPHA_BUMPN,
    GX_RAS_COLOR_ZERO,

    GX_RAS_MAX_CHANNEL
} GXRasChannelID;

typedef enum {
    GX_TEVREG_COLOR,
    GX_TEVREG_KONST,
} GXTevRegType;

typedef enum {
    MEM_MARR0_START_IDX,
    MEM_MARR0_END_IDX,
    MEM_MARR1_START_IDX,

    MEM_PE_REQCOUNTH_IDX = 39,
    MEM_PE_REQCOUNTL_IDX = 40,
} GXMemRegs;

#define GX_RA_LOAD_REG(reg)                                                    \
    {                                                                          \
        GX_WRITE_U8(GX_CP_CMD_TO_OPCODE(1, 12));                               \
        GX_WRITE_U32((reg));                                                   \
    }

#ifdef __cplusplus
}
#endif
#endif
