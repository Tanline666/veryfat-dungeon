#include <revolution/BASE.h>
#include <revolution/GX.h>
#include <revolution/VI.h>

#define GX_TMEM_LO 0x00000
#define GX_TMEM_HI 0x80000
#define GX_TMEM_32K 0x08000
#define GX_TMEM_8K 0x02000

const char* __GXVersion =
    "<< RVL_SDK - GX \trelease build: Feb 27 2009 10:04:13 (0x4302_145) >>";

static GXFifoObj FifoObj;
static GXData gxData;
GXData* const __GXData = &gxData;

volatile void* __piReg = NULL;
volatile void* __cpReg = NULL;
volatile void* __peReg = NULL;
volatile void* __memReg = NULL;

static u16 DefaultTexData[] ALIGN(32) = {
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

static GXVtxAttrFmtList GXDefaultVATList[] = {
    {GX_VA_POS, GX_POS_XYZ, GX_F32, 0},
    {GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0},
    {GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0},
    {GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0},
    {GX_VA_TEX0, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX1, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX2, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX3, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX4, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX5, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX6, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX7, GX_TEX_ST, GX_F32, 0},
    {GX_VA_NULL, (GXCompCnt)0, (GXCompType)0, 0}};

static f32 GXDefaultProjData[] = {1.0F, 0.0F, 1.0F, 0.0F, -1.0F, -2.0F, 0.0F};

static void EnableWriteGatherPipe(void);
static void DisableWriteGatherPipe(void);

static GXTlutRegion* __GXDefaultTlutRegionCallback(u32 idx);

static GXTexRegion* __GXDefaultTexRegionCallback(const GXTexObj* pTexObj,
                                                 GXTexMapID id);

static BOOL __GXShutdown(BOOL final, u32 event);

static u32 GXTexRegionAddrTable[] = {
    GX_TMEM_LO + 0x00000, GX_TMEM_LO + 0x10000, GX_TMEM_LO + 0x20000,
    GX_TMEM_LO + 0x30000, GX_TMEM_LO + 0x40000, GX_TMEM_LO + 0x50000,
    GX_TMEM_LO + 0x60000, GX_TMEM_LO + 0x70000,

    GX_TMEM_LO + 0x08000, GX_TMEM_LO + 0x18000, GX_TMEM_LO + 0x28000,
    GX_TMEM_LO + 0x38000, GX_TMEM_LO + 0x48000, GX_TMEM_LO + 0x58000,
    GX_TMEM_LO + 0x68000, GX_TMEM_LO + 0x78000,

    GX_TMEM_LO + 0x00000, GX_TMEM_HI + 0x10000, GX_TMEM_LO + 0x20000,
    GX_TMEM_HI + 0x30000, GX_TMEM_LO + 0x40000, GX_TMEM_HI + 0x18000,
    GX_TMEM_LO + 0x60000, GX_TMEM_HI + 0x38000,

    GX_TMEM_HI + 0x00000, GX_TMEM_LO + 0x10000, GX_TMEM_HI + 0x20000,
    GX_TMEM_LO + 0x30000, GX_TMEM_HI + 0x08000, GX_TMEM_LO + 0x50000,
    GX_TMEM_HI + 0x28000, GX_TMEM_LO + 0x70000,

    GX_TMEM_LO + 0x00000, GX_TMEM_HI + 0x10000, GX_TMEM_LO + 0x20000,
    GX_TMEM_HI + 0x30000, GX_TMEM_LO + 0x40000, GX_TMEM_HI + 0x10000,
    GX_TMEM_LO + 0x60000, GX_TMEM_HI + 0x30000,

    GX_TMEM_HI + 0x00000, GX_TMEM_LO + 0x10000, GX_TMEM_HI + 0x20000,
    GX_TMEM_LO + 0x30000, GX_TMEM_HI + 0x00000, GX_TMEM_LO + 0x50000,
    GX_TMEM_HI + 0x20000, GX_TMEM_LO + 0x70000,
};

static OSShutdownFunctionInfo GXShutdownFuncInfo = {__GXShutdown, 127};

static void EnableWriteGatherPipe(void) {
    u32 hid2 = PPCMfhid2();

    PPCMtwpar((u32)OSUncachedToPhysical((void*)0xCC008000));
    hid2 |= HID2_WPE;
    PPCMthid2(hid2);
}

static GXTexRegion* __GXDefaultTexRegionCallback(const GXTexObj* pTexObj,
                                                 GXTexMapID id) {
    GXTexFmt fmt;
    GXBool isMipMap;

    fmt = GXGetTexObjFmt(pTexObj);
    isMipMap = GXGetTexObjMipMap(pTexObj);
    id = (GXTexMapID)(id % GX_MAX_TEXCOORD);

    switch (fmt) {
    case GX_TF_RGBA8:
        if (isMipMap) {
            return &gxdt->TexRegions2[id];
        } else {
            return &gxdt->TexRegions1[id];
        }
        break;
    case GX_TF_C4:
    case GX_TF_C8:
    case GX_TF_C14X2:
        return &gxdt->TexRegions0[id];
    default:
        if (isMipMap) {
            return &gxdt->TexRegions1[id];
        } else {
            return &gxdt->TexRegions0[id];
        }
        break;
    }
}

static GXTlutRegion* __GXDefaultTlutRegionCallback(u32 idx) {
    if (idx >= 20) {
        return NULL;
    }

    return &gxdt->TlutRegions[idx];
}

static BOOL __GXShutdown(BOOL final, u32 event) {
#pragma unused(event)
    static u32 peCount;
    static s64 time;
    static u32 calledOnce = FALSE;
    u32 reg, newPeCount;
    s64 newTime;

    if (!final) {
        if (!calledOnce) {
            peCount = GX_MEM_COUNTER_READ_U32(MEM_PE_REQCOUNT);
            time = OSGetTime();
            calledOnce = TRUE;
            return FALSE;
        }

        newTime = OSGetTime();
        newPeCount = GX_MEM_COUNTER_READ_U32(MEM_PE_REQCOUNT);

        if (newTime - time < 10) {
            return FALSE;
        }
        if (newPeCount != peCount) {
            peCount = newPeCount;
            time = newTime;
            return FALSE;
        }
    } else {
        GXSetBreakPtCallback(NULL);
        GXSetDrawSyncCallback(NULL);
        GXSetDrawDoneCallback(NULL);
    }

    return TRUE;
}

void __GXInitRevisionBits(void) {
    u32 i;

    for (i = 0; i < GX_MAX_VTXFMT; i++) {
        GX_CP_SET_VAT_GROUP0_BYTEDEQUANT(gxdt->vatA[i], 1);
        GX_CP_SET_VAT_GROUP0_NORMALINDEX3(gxdt->vatB[i], 1);
        GX_CP_LOAD_REG(GX_CP_REG_VAT_GROUP1, i, gxdt->vatB[i]);
    }

    {
        u32 reg1 = NULL;
        u32 reg2 = NULL;

        GX_XF_SET_CLIPDISABLE_DETECT(reg1, 1);
        GX_XF_SET_CLIPDISABLE_REJECT(reg1, 1);
        GX_XF_SET_CLIPDISABLE_ACCEL(reg1, 1);
        GX_XF_SET_CLIPDISABLE_DUALACCEL(reg1, 1);
        GX_XF_SET_CLIPDISABLE_BYPASS(reg1, 1);
        GX_XF_SET_CLIPDISABLE_FASTMATRIX(reg1, 1);
        GX_XF_LOAD_REG(GX_XF_REG_ERROR, reg1, 0);

        //! Fakematch, but too lazy to write new macro
        GX_XF_SET_CLIPDISABLE_DETECT(reg2, 1);
        GX_XF_LOAD_REG(GX_XF_REG_DUALTEXTRAN, reg2, 0);
    }

    {
        u32 reg = 0;
        //! Warning: major fakematches here! These are not the altered settings
        //! at all, but they do the same operations
        GX_XF_SET_CLIPDISABLE_DETECT(reg, 1);
        GX_XF_SET_CLIPDISABLE_REJECT(reg, 1);
        GX_XF_SET_CLIPDISABLE_ACCEL(reg, 1);
        GX_XF_SET_CLIPDISABLE_DUALACCEL(reg, 1);
        GX_XF_SET_OPCODE(reg, 0x58);
        GX_RA_LOAD_REG(reg);
    }
}

GXFifoObj* GXInit(void* base, u32 size) {
    static u32 shutdownFuncRegistered = FALSE;
    u32 i;

    OSRegisterVersion(__GXVersion);

    gxdt->dlistActive = FALSE;
    gxdt->dlistSave = TRUE;
    gxdt->abortPECopyDone = TRUE;

    GXSetMisc(1, 0);

    __piReg = (void*)OSPhysicalToUncached(PI_HW_REGS[0]);
    __cpReg = (void*)OSPhysicalToUncached(0xCC000000);
    __peReg = (void*)OSPhysicalToUncached(0xCC001000);
    __memReg = (void*)OSPhysicalToUncached(MI_HW_REGS[0]);

    __GXFifoInit();
    GXInitFifoBase(&FifoObj, base, size);
    GXSetCPUFifo(&FifoObj);
    GXSetGPFifo(&FifoObj);

    if (!shutdownFuncRegistered) {
        OSRegisterShutdownFunction(&GXShutdownFuncInfo);
        shutdownFuncRegistered = TRUE;
    }

    __GXPEInit();
    EnableWriteGatherPipe();

    gxdt->genMode = NULL;
    //! This doesn't actually do anything, but I am keeping this for visual
    //! consistency
    GX_GEN_SET_OPCODE(gxdt->genMode, GX_BP_REG_GENMODE);
    gxdt->ind_imask = 255;
    GX_GEN_SET_OPCODE(gxdt->ind_imask, GX_BP_REG_INDIMASK);
    gxdt->linePtWidth = 0;
    GX_GEN_SET_OPCODE(gxdt->linePtWidth, GX_BP_REG_LINEPTWIDTH);

    for (i = 0; i < GX_MAX_TEVSTAGE; i++) {
        gxdt->tevc[i] = NULL;
        gxdt->teva[i] = NULL;
        gxdt->ras1_tref[i / 2] = NULL;
        gxdt->texMapID[i] = GX_TEXMAP_NULL;
        GX_TEV_SET_OPCODE(gxdt->tevc[i], (GX_BP_REG_TEVCOLORCOMBINER0 + i * 2));
        GX_TEV_SET_OPCODE(gxdt->teva[i], (GX_BP_REG_TEVALPHACOMBINER0 + i * 2));
        GX_TEV_SET_OPCODE(gxdt->tevKsel[i / 2], (GX_BP_REG_TEVKSEL0 + i / 2));
        GX_TEV_SET_OPCODE(gxdt->ras1_tref[i / 2],
                          (GX_BP_REG_RAS1_TREF0 + i / 2));
    }

    gxdt->ras1_iref = NULL;
    GX_BP_SET_RAS1_IREF_MAP0(gxdt->ras1_iref, GX_BP_REG_RAS1_IREF);

    for (i = 0; i < GX_MAX_TEXCOORD; i++) {
        gxdt->txcRegs[i] = NULL;
        gxdt->dualTxcRegs[i] = NULL;
        GX_TEV_SET_OPCODE(gxdt->txcRegs[i], GX_BP_REG_SU_SSIZE0 + i * 2);
        GX_TEV_SET_OPCODE(gxdt->dualTxcRegs[i], GX_BP_REG_SU_TSIZE0 + i * 2);
    }

    GX_TEV_SET_OPCODE(gxdt->scissorTL, GX_BP_REG_SCISSORTL);
    GX_TEV_SET_OPCODE(gxdt->scissorBR, GX_BP_REG_SCISSORBR);

    GX_TEV_SET_OPCODE(gxdt->blendMode, GX_BP_REG_BLENDMODE);
    GX_TEV_SET_OPCODE(gxdt->dstAlpha, GX_BP_REG_DSTALPHA);
    GX_TEV_SET_OPCODE(gxdt->zMode, GX_BP_REG_ZMODE);
    GX_TEV_SET_OPCODE(gxdt->zControl, GX_BP_REG_ZCONTROL);

    //! Fakematch? The size and shift are correct, but not the name...
    GX_BP_SET_INDTEVSTAGE_ALPHA(gxdt->cpTex, GX_GM_1_0);

    gxdt->scaleZ = 16777216.0f;
    gxdt->offsetZ = 0.0f;
    gxdt->gxDirtyFlags = 0;
    gxdt->vatDirtyFlags = 0;

    //! The block is required or else the compiler throws a fit
    {
        u32 reg;
        u32 freqBase = OS_BUS_CLOCK_SPEED / 500;

        __GXFlushTextureState();
        //! Fakematch
        reg = ((freqBase / 2048) | 1024 | 1761607680);
        GX_RA_LOAD_REG(reg);
        __GXFlushTextureState();
        //! Fakematch
        reg = ((freqBase / 4224) | 512 | 1174405120);
        GX_RA_LOAD_REG(reg);
    }

    __GXInitRevisionBits();

    for (i = 0; i < 8; i++) {
        GXInitTexCacheRegion(&gxdt->TexRegions0[i], GX_FALSE,
                             GXTexRegionAddrTable[i], GX_TEXCACHE_32K,
                             GXTexRegionAddrTable[i + 8], GX_TEXCACHE_32K);
        GXInitTexCacheRegion(&gxdt->TexRegions1[i], GX_FALSE,
                             GXTexRegionAddrTable[i + 16], GX_TEXCACHE_32K,
                             GXTexRegionAddrTable[i + 24], GX_TEXCACHE_32K);
        GXInitTexCacheRegion(&gxdt->TexRegions2[i], GX_TRUE,
                             GXTexRegionAddrTable[i + 32], GX_TEXCACHE_32K,
                             GXTexRegionAddrTable[i + 48], GX_TEXCACHE_32K);
    }

    for (i = 0; i < 16; i++) {
        GXInitTlutRegion(&gxdt->TlutRegions[i],
                         (GX_TMEM_HI + GX_TMEM_32K * 8 + GX_TMEM_8K * i),
                         GX_TLUT_256);
    }
    for (i = 0; i < 4; i++) {
        GXInitTlutRegion(
            &gxdt->TlutRegions[i + 16],
            (GX_TMEM_HI + GX_TMEM_32K * 8 + GX_TMEM_8K * 16 + GX_TMEM_32K * i),
            GX_TLUT_1K);
    }

    {
        u32 reg;

        reg = NULL;
        GX_CP_REG_WRITE_U16(3, reg);

        GX_CP_SET_STATSELREG_STALLPERF(gxdt->perfSel, 0);
        GX_CP_LOAD_REG(GX_CP_STAT_SEL, 0, gxdt->perfSel);

        reg = GX_XF_PERF0_F(NULL, NULL, NULL, NULL);
        GX_XF_LOAD_REG(GX_XF_REG_PERF0, reg, 0);

        reg = GX_BP_REG_SU_PERF(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                NULL, GX_BP_REG_PERF0TRI);
        GX_RA_LOAD_REG(reg);

        reg = GX_BP_REG_RAS_PERF(NULL, NULL, NULL, NULL, NULL,
                                 GX_BP_REG_PERF0QUAD);
        GX_RA_LOAD_REG(reg);

        //! Fakematch
        reg = GX_TX_PERFMODE(NULL, NULL, NULL, NULL, GX_BP_REG_PERF1);
        GX_RA_LOAD_REG(reg);
    }

    __GXSetIndirectMask(0);
    __GXSetTmemConfig(2);
    __GXInitGX();

    return &FifoObj;
}

void __GXInitGX() {
    GXRenderModeObj* pRenderMode;
    GXTexObj texObj;
    Mtx identity;
    GXColor clear = {64, 64, 64, 255};
    GXColor black = {0, 0, 0, 0};
    GXColor white = {255, 255, 255, 255};
    u32 i;

    switch (VIGetTvFormat()) {
    case VI_TVFORMAT_NTSC:
        pRenderMode = &GXNtsc480IntDf;
        break;
    case VI_TVFORMAT_PAL:
        pRenderMode = &GXPal528IntDf;
        break;
    case VI_TVFORMAT_EURGB60:
        pRenderMode = &GXEurgb60Hz480IntDf;
        break;
    case VI_TVFORMAT_MPAL:
        pRenderMode = &GXMpal480IntDf;
        break;
    default:
        GX_ASSERT(FALSE, "GXInit: invalid TV format");
        pRenderMode = &GXNtsc480IntDf;
        break;
    }

    GXSetCopyClear(clear, 0x00FFFFFF);

    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD5, GX_TG_MTX2x4, GX_TG_TEX5, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD6, GX_TG_MTX2x4, GX_TG_TEX6, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD7, GX_TG_MTX2x4, GX_TG_TEX7, GX_IDENTITY);
    GXSetNumTexGens(1);
    GXClearVtxDesc();
    GXInvalidateVtxCache();

    for (i = GX_VA_POS; i <= GX_LIGHT_ARRAY; i++) {
        GXSetArray((GXAttr)i, (void*)__GXData, 0);
    }

    for (i = GX_VTXFMT0; i < GX_MAX_VTXFMT; i++) {
        GXSetVtxAttrFmtv((GXVtxFmt)i, GXDefaultVATList);
    }

    GXSetLineWidth(6, GX_TO_ZERO);
    GXSetPointSize(6, GX_TO_ZERO);
    GXEnableTexOffsets(GX_TEXCOORD0, GX_DISABLE, GX_DISABLE);
    GXEnableTexOffsets(GX_TEXCOORD1, GX_DISABLE, GX_DISABLE);
    GXEnableTexOffsets(GX_TEXCOORD2, GX_DISABLE, GX_DISABLE);
    GXEnableTexOffsets(GX_TEXCOORD3, GX_DISABLE, GX_DISABLE);
    GXEnableTexOffsets(GX_TEXCOORD4, GX_DISABLE, GX_DISABLE);
    GXEnableTexOffsets(GX_TEXCOORD5, GX_DISABLE, GX_DISABLE);
    GXEnableTexOffsets(GX_TEXCOORD6, GX_DISABLE, GX_DISABLE);
    GXEnableTexOffsets(GX_TEXCOORD7, GX_DISABLE, GX_DISABLE);

    identity[0][0] = 1.0f;
    identity[0][1] = 0.0f;
    identity[0][2] = 0.0f;
    identity[0][3] = 0.0f;
    identity[1][0] = 0.0f;
    identity[1][1] = 1.0f;
    identity[1][2] = 0.0f;
    identity[1][3] = 0.0f;
    identity[2][0] = 0.0f;
    identity[2][1] = 0.0f;
    identity[2][2] = 1.0f;
    identity[2][3] = 0.0f;

    GXLoadPosMtxImm(identity, GX_PNMTX0);
    GXLoadNrmMtxImm(identity, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXLoadTexMtxImm(identity, GX_IDENTITY, GX_MTX_3x4);
    GXLoadTexMtxImm(identity, GX_PTIDENTITY, GX_MTX_3x4);
    GXSetViewport(0.0f, 0.0f, pRenderMode->fbWidth, pRenderMode->xfbHeight,
                  0.0f, 1.0f);
    GXSetProjectionv(GXDefaultProjData);

    __GXSetGenMode();
    GXSetCoPlanar(GX_DISABLE);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetScissor(0, 0, pRenderMode->fbWidth, pRenderMode->efbHeight);
    GXSetScissorBoxOffset(0, 0);

    GXSetNumChans(0);

    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
                  GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanAmbColor(GX_COLOR0A0, black);
    GXSetChanMatColor(GX_COLOR0A0, white);

    GXSetChanCtrl(GX_COLOR1A1, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
                  GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanAmbColor(GX_COLOR1A1, black);
    GXSetChanMatColor(GX_COLOR1A1, white);

    GXInvalidateTexAll();

    GXSetTexRegionCallback(__GXDefaultTexRegionCallback);
    GXSetTlutRegionCallback(__GXDefaultTlutRegionCallback);

    GXInitTexObj(&texObj, DefaultTexData, 4, 4, GX_TF_IA8, GX_CLAMP, GX_CLAMP,
                 GX_FALSE);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXLoadTexObj(&texObj, GX_TEXMAP1);
    GXLoadTexObj(&texObj, GX_TEXMAP2);
    GXLoadTexObj(&texObj, GX_TEXMAP3);
    GXLoadTexObj(&texObj, GX_TEXMAP4);
    GXLoadTexObj(&texObj, GX_TEXMAP5);
    GXLoadTexObj(&texObj, GX_TEXMAP6);
    GXLoadTexObj(&texObj, GX_TEXMAP7);

    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD4, GX_TEXMAP4, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD5, GX_TEXMAP5, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD6, GX_TEXMAP6, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD7, GX_TEXMAP7, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE10, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE11, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE12, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE13, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE14, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE15, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                  GX_COLOR_NULL);
    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);

    for (i = GX_TEVSTAGE0; i < GX_MAX_TEVSTAGE; i++) {
        GXSetTevKColorSel((GXTevStageID)i, GX_TEV_KCSEL_1_4);
        GXSetTevKAlphaSel((GXTevStageID)i, GX_TEV_KASEL_1);
        GXSetTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE,
                          GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED,
                          GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN,
                          GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE,
                          GX_CH_ALPHA);

    for (i = GX_TEVSTAGE0; i < GX_MAX_TEVSTAGE; i++) {
        GXSetTevDirect((GXTevStageID)i);
    }
    GXSetNumIndStages(0);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE1, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE2, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE3, GX_ITS_1, GX_ITS_1);

    GXSetFog(GX_FOG_NONE, 0.0f, 1.0f, 0.1f, 1.0f, black);
    GXSetFogRangeAdj(GX_DISABLE, 0, 0);
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetColorUpdate(GX_ENABLE);
    GXSetAlphaUpdate(GX_ENABLE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetZCompLoc(GX_TRUE);
    GXSetDither(GX_ENABLE);
    GXSetDstAlpha(GX_DISABLE, 0);
    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GXSetFieldMask(GX_ENABLE, GX_ENABLE);
    GXSetFieldMode(
        (GXBool)(pRenderMode->field_rendering),
        (GXBool)(((pRenderMode->viHeight == 2 * pRenderMode->xfbHeight)
                      ? GX_ENABLE
                      : GX_DISABLE)));

    GXSetDispCopySrc(0, 0, pRenderMode->fbWidth, pRenderMode->efbHeight);
    GXSetDispCopyDst(pRenderMode->fbWidth, pRenderMode->efbHeight);
    GXSetDispCopyYScale((f32)(pRenderMode->xfbHeight) /
                        (f32)(pRenderMode->efbHeight));
    GXSetCopyClamp((GXCopyClamp)(GX_CLAMP_TOP | GX_CLAMP_BOTTOM));
    GXSetCopyFilter(pRenderMode->aa, pRenderMode->sample_pattern, GX_TRUE,
                    pRenderMode->vfilter);
    GXSetDispCopyGamma(0);
    GXSetDispCopyFrame2Field(0);
    GXClearBoundingBox();
    GXPokeColorUpdate(GX_TRUE);
    GXPokeAlphaUpdate(GX_TRUE);
    GXPokeDither(GX_FALSE);
    GXPokeBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ONE, GX_LO_SET);
    GXPokeAlphaMode(GX_ALWAYS, 0);
    GXPokeAlphaRead(GX_READ_FF);
    GXPokeDstAlpha(GX_DISABLE, 0);
    GXPokeZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GXSetGPMetric(35, 22);
    GXClearGPMetric();
}
