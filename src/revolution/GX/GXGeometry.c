#include <revolution/GX.h>

inline void __GXSetAmbMat(u32 dirtyFlags) {
    if (dirtyFlags & GX_DIRTY_AMB_COLOR0) {
        GX_XF_LOAD_REG(GX_XF_REG_AMBIENT0, *(u32*)&gxdt->ambColors[0], 0);
    }

    if (dirtyFlags & GX_DIRTY_AMB_COLOR1) {
        GX_XF_LOAD_REG(GX_XF_REG_AMBIENT1, *(u32*)&gxdt->ambColors[1], 0);
    }

    if (dirtyFlags & GX_DIRTY_MAT_COLOR0) {
        GX_XF_LOAD_REG(GX_XF_REG_MATERIAL0, *(u32*)&gxdt->matColors[0], 0);
    }

    if (dirtyFlags & GX_DIRTY_MAT_COLOR1) {
        GX_XF_LOAD_REG(GX_XF_REG_MATERIAL1, *(u32*)&gxdt->matColors[1], 0);
    }
}

inline void __GXSetLightChan(volatile s32 dirtyFlags) {
    int i;
    u32 bits;
    u32 addr = GX_XF_REG_COLOR0CNTRL;

    if (dirtyFlags & GX_DIRTY_NUM_COLORS) {
        u32 colors = GX_BP_GET_GENMODE_NUMCOLORS(gxdt->genMode);
        GX_XF_LOAD_REG(GX_XF_REG_NUMCOLORS, colors, 0);
    }

    bits = dirtyFlags;
    bits = (bits >> 12) & 0xF;
    i = 0;
    for (; bits != 0; i++, addr++, bits >>= 1) {
        if (bits & 1) {
            GX_XF_LOAD_REG(addr, gxdt->colorControl[i], 0);
        }
    }
}

inline void __GXSetTexGen(volatile s32 dirtyFlags) {
    int i;
    u32 bits;
    u32 taddr = GX_XF_REG_TEX0;
    u32 dtaddr;

    if (dirtyFlags & GX_DIRTY_NUM_TEX) {
        u32 gens = GX_BP_GET_GENMODE_NUMTEX(gxdt->genMode);
        GX_XF_LOAD_REG(GX_XF_REG_NUMTEX, gens, 0);
    }

    bits = dirtyFlags;
    bits = (bits >> 16) & 0xFF;
    i = 0;
    for (; bits != 0; taddr++, i++, bits >>= 1) {
        dtaddr = taddr + GX_XF_REG_DUALTEX0 - GX_XF_REG_TEX0;
        if (bits & 1) {
            GX_XF_LOAD_REG(taddr, gxdt->texRegs[i], 0);
            GX_XF_LOAD_REG(dtaddr, gxdt->dualTexRegs[i], 0);
        }
    }
}

void __GXSetDirtyState(void) {
    u32 tempFlags;
    u32 dirtyFlags = gxdt->gxDirtyFlags;

    if (dirtyFlags & GX_DIRTY_SU_TEX) {
        __GXSetSUTexRegs();
    }

    if (dirtyFlags & GX_DIRTY_BP_MASK) {
        __GXUpdateBPMask();
    }

    if (dirtyFlags & GX_DIRTY_GEN_MODE) {
        __GXSetGenMode();
    }

    if (dirtyFlags & GX_DIRTY_VCD) {
        __GXSetVCD();
    }

    if (dirtyFlags & GX_DIRTY_VAT) {
        __GXSetVAT();
    }

    if (dirtyFlags & (GX_DIRTY_VCD | GX_DIRTY_VAT)) {
        __GXCalculateVLim();
    }

    dirtyFlags &= ~0xFF;
    if (dirtyFlags) {
        tempFlags = dirtyFlags & GX_AMB_MAT_MASK;
        if (tempFlags != 0) {
            __GXSetAmbMat(tempFlags);
        }

        tempFlags = dirtyFlags & GX_LIGHT_CHAN_MASK;
        if (tempFlags != 0) {
            __GXSetLightChan(tempFlags);
        }

        tempFlags = dirtyFlags & GX_TEX_GEN_MASK;
        if (tempFlags != 0) {
            __GXSetTexGen(tempFlags);
        }

        tempFlags = dirtyFlags & GX_DIRTY_MTX_IDX;
        if (tempFlags != 0) {
            __GXSetMatrixIndex(GX_VA_PNMTXIDX);
            __GXSetMatrixIndex(GX_VA_TEX4MTXIDX);
        }

        tempFlags = dirtyFlags & GX_DIRTY_VIEWPORT;
        if (tempFlags != 0) {
            __GXSetViewport();
        }

        tempFlags = dirtyFlags & GX_DIRTY_PROJECTION;
        if (tempFlags != 0) {
            __GXSetProjection();
        }

        gxdt->lastWriteWasXF = TRUE;
    }

    gxdt->gxDirtyFlags = 0;
}

void GXBegin(GXPrimitive prim, GXVtxFmt fmt, u16 verts) {
    GX_ASSERT((fmt < GX_MAX_VTXFMT), "GXBegin: Format Index is out of range");
    GX_ASSERT(!__GXinBegin, "GXBegin: called inside another GXBegin/GXEnd");

    if (gxdt->gxDirtyFlags != 0) {
        __GXSetDirtyState();
    }

#if defined(DEBUG)
    //! The entirety of GXVerify is stubbed out in retail. __GXVerifyState()
    //! cannot be added until later.
    __GXinBegin = TRUE;
#endif

    if (gxdt->WORD_0x0 == 0) {
        __GXSendFlushPrim();
    }

    GX_WRITE_U8(fmt | prim);
    GX_WRITE_U16(verts);
}

void __GXSendFlushPrim(void) {
    u32 i;
    u32 sz = gxdt->vnum * gxdt->vlim;

    GX_WRITE_U8(GX_TRIANGLESTRIP);
    GX_WRITE_U16(gxdt->vnum);

    for (i = 0; i < sz; i += 4) {
        GX_WRITE_U32(NULL);
    }

    gxdt->lastWriteWasXF = TRUE;
}

void GXSetLineWidth(u8 width, u32 offset) {
    GX_ASSERT_BETWEEN_BEGIN_END(GXSetLineWidth);
    GX_BP_SET_LINEPTWIDTH_LINESZ(gxdt->linePtWidth, width);
    GX_BP_SET_LINEPTWIDTH_LINEOFS(gxdt->linePtWidth, offset);
    GX_BP_LOAD_REG(gxdt->linePtWidth);
    gxdt->lastWriteWasXF = FALSE;
}

void GXSetPointSize(u8 size, u32 offset) {
    GX_ASSERT_BETWEEN_BEGIN_END(GXSetPointSize);
    GX_BP_SET_LINEPTWIDTH_POINTSZ(gxdt->linePtWidth, size);
    GX_BP_SET_LINEPTWIDTH_POINTOFS(gxdt->linePtWidth, offset);
    GX_BP_LOAD_REG(gxdt->linePtWidth);
    gxdt->lastWriteWasXF = FALSE;
}

void GXEnableTexOffsets(GXTexCoordID id, GXBool lineOfs, GXBool pointOfs) {
    GX_ASSERT_BETWEEN_BEGIN_END(GXEnableTexOffsets);
    GX_ASSERT((id < GX_MAX_TEXCOORD),
              "GXEnableTexOffsets: Invalid coordinate Id");

    GX_BP_SET_SU_SIZE_USELINEOFS(gxdt->txcRegs[id], lineOfs);
    GX_BP_SET_SU_SIZE_USEPOINTOFS(gxdt->txcRegs[id], pointOfs);
    GX_BP_LOAD_REG(gxdt->txcRegs[id]);
    gxdt->lastWriteWasXF = FALSE;
}

void GXSetCullMode(GXCullMode mode) {
    GXCullMode bits;
    GX_ASSERT_BETWEEN_BEGIN_END(GXSetCullMode);
#if defined(DEBUG)
    switch (mode) {
    case GX_CULL_FRONT:
        bits = GX_CULL_BACK;
        break;
    case GX_CULL_BACK:
        bits = GX_CULL_FRONT;
        break;
    default:
        bits = mode;
        break;
    }
#else
    // Swap bits to get hardware representation
    bits = (GXCullMode)(mode << 1 & 2 | mode >> 1 & 1);
#endif
    GX_BP_SET_GENMODE_CULLMODE(gxdt->genMode, bits);
    gxdt->gxDirtyFlags |= GX_DIRTY_GEN_MODE;
}

void GXGetCullMode(GXCullMode* out) {
    // TODO(kiwi) Fakematch? Should use GX_BP_GET_GENMODE_CULLMODE if possible
    s32 bits = 0;
    bits |= (s32)(gxdt->genMode >> (13 + 1) & 2) >> 1;
    bits |= (s32)(gxdt->genMode >> 13 & 2);
    *out = (GXCullMode)bits;
}

void GXSetCoPlanar(GXBool coplanar) {
    u32 reg;
    GX_ASSERT_BETWEEN_BEGIN_END(GXSetCoPlanar);

    GX_BP_SET_GENMODE_COPLANAR(gxdt->genMode, coplanar);

    //! Fakematch, but this is still satisfying
    reg = (GX_BP_GENMODE_COPLANAR_MASK) | (GX_BP_REG_SSMASK << 24);

    GX_BP_LOAD_REG(reg);
    GX_BP_LOAD_REG(gxdt->genMode);
    gxdt->lastWriteWasXF = FALSE;
}

void __GXSetGenMode(void) {
    GX_RA_LOAD_REG(gxdt->genMode);
    gxdt->lastWriteWasXF = FALSE;
}
