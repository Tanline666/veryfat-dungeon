#include <nw4r/ef.h>

#include <revolution/GX.h>

#include <cmath>

namespace nw4r {
namespace ef {

const math::VEC3 DrawStrategyImpl::mXUnitVec(1.0f, 0.0f, 0.0f);
const math::VEC3 DrawStrategyImpl::mYUnitVec(0.0f, 1.0f, 0.0f);
const math::VEC3 DrawStrategyImpl::mZUnitVec(0.0f, 0.0f, 1.0f);
const math::VEC3 DrawStrategyImpl::mZeroVec(0.0f, 0.0f, 0.0f);

// clang-format off
const math::MTX34 DrawStrategyImpl::mIdentityMtx(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f);
// clang-format on

/******************************************************************************
 *
 * DrawStrategyImpl
 *
 ******************************************************************************/
DrawStrategyImpl::DrawStrategyImpl() {}

void DrawStrategyImpl::Initialize(ParticleManager* pm,
                                  const EmitterDrawSetting& ed,
                                  const DrawInfo& drawInfo) {
    _InitTexture(ed);
    _InitTev(ed, drawInfo);
    _InitColor(pm, ed, drawInfo);
}

void DrawStrategyImpl::_InitTexture(const EmitterDrawSetting& ed) {
    mNumTexmap = 0;

    mTexmapMap[TEX_LAYER_1] = -1;
    mTexmapMap[TEX_LAYER_2] = -1;
    mTexmapMap[TEX_LAYER_IND] = -1;

    if (ed.mFlags & EmitterDrawSetting::FLAG_TEX1_ENABLE) {
        mTexmapMap[TEX_LAYER_1] = 0;
        mNumTexmap++;
    }

    if (ed.mFlags & EmitterDrawSetting::FLAG_TEX2_ENABLE) {
        mTexmapMap[TEX_LAYER_2] = mNumTexmap;
        mNumTexmap++;
    }

    if (ed.mFlags & EmitterDrawSetting::FLAG_TEXIND_ENABLE) {
        mTexmapMap[TEX_LAYER_IND] = mNumTexmap;
        mNumTexmap++;
    }

    GXSetNumTexGens(mNumTexmap);
}

void DrawStrategyImpl::_InitTev(const EmitterDrawSetting& ed,
                                const DrawInfo& drawInfo) {

    int i;

    GXSetClipMode(ed.mFlags & EmitterDrawSetting::FLAG_CLIP_DISABLE
                      ? GX_CLIP_DISABLE
                      : GX_CLIP_ENABLE);

    GXSetNumTevStages(ed.mNumTevs);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE,
                          GX_CH_ALPHA);

    for (i = 0; i < ed.mNumTevs; i++) {
        GXSetTevDirect(static_cast<GXTevStageID>(i));

        GXSetTevColorIn(static_cast<GXTevStageID>(i),
                        static_cast<GXTevColorArg>(ed.mTevColor[i].mA),
                        static_cast<GXTevColorArg>(ed.mTevColor[i].mB),
                        static_cast<GXTevColorArg>(ed.mTevColor[i].mC),
                        static_cast<GXTevColorArg>(ed.mTevColor[i].mD));

        GXSetTevAlphaIn(static_cast<GXTevStageID>(i),
                        static_cast<GXTevAlphaArg>(ed.mTevAlpha[i].mA),
                        static_cast<GXTevAlphaArg>(ed.mTevAlpha[i].mB),
                        static_cast<GXTevAlphaArg>(ed.mTevAlpha[i].mC),
                        static_cast<GXTevAlphaArg>(ed.mTevAlpha[i].mD));

        GXSetTevColorOp(static_cast<GXTevStageID>(i),
                        static_cast<GXTevOp>(ed.mTevColorOp[i].mOp),
                        static_cast<GXTevBias>(ed.mTevColorOp[i].mBias),
                        static_cast<GXTevScale>(ed.mTevColorOp[i].mScale),
                        static_cast<GXBool>(ed.mTevColorOp[i].mClamp),
                        static_cast<GXTevRegID>(ed.mTevColorOp[i].mOutReg));

        GXSetTevAlphaOp(static_cast<GXTevStageID>(i),
                        static_cast<GXTevOp>(ed.mTevAlphaOp[i].mOp),
                        static_cast<GXTevBias>(ed.mTevAlphaOp[i].mBias),
                        static_cast<GXTevScale>(ed.mTevAlphaOp[i].mScale),
                        static_cast<GXBool>(ed.mTevAlphaOp[i].mClamp),
                        static_cast<GXTevRegID>(ed.mTevAlphaOp[i].mOutReg));

        GXSetTevKColorSel(static_cast<GXTevStageID>(i),
                          static_cast<GXTevKColorSel>(ed.mTevKColorSel[i]));

        GXSetTevKAlphaSel(static_cast<GXTevStageID>(i),
                          static_cast<GXTevKAlphaSel>(ed.mTevKAlphaSel[i]));

        GXSetTevSwapMode(static_cast<GXTevStageID>(i), GX_TEV_SWAP0,
                         GX_TEV_SWAP0);

        if (ed.mTevTexture[i] == 0) {
            NW4R_ASSERT(mTexmapMap[0] == 0, 134);
            GXSetTevOrder(static_cast<GXTevStageID>(i), GX_TEXCOORD0,
                          GX_TEXMAP0, GX_COLOR0A0);
        } else if (ed.mTevTexture[i] == 1) {
            GXSetTevOrder(static_cast<GXTevStageID>(i),
                          static_cast<GXTexCoordID>(mTexmapMap[TEX_LAYER_2]),
                          static_cast<GXTexMapID>(mTexmapMap[TEX_LAYER_2]),
                          GX_COLOR0A0);
        } else {
            GXSetTevOrder(static_cast<GXTevStageID>(i), GX_TEXCOORD_NULL,
                          GX_TEXMAP_NULL, GX_COLOR0A0);
        }
    }

    if (ed.mFlags & EmitterDrawSetting::FLAG_TEXIND_ENABLE) {
        GXSetNumIndStages(1);

        GXSetIndTexOrder(GX_INDTEXSTAGE0,
                         static_cast<GXTexCoordID>(mTexmapMap[TEX_LAYER_IND]),
                         static_cast<GXTexMapID>(mTexmapMap[TEX_LAYER_IND]));

        GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
        GXSetIndTexMtx(GX_ITM_0, ed.mIndTexOffsetMtx, ed.mIndTexScaleExp);

        u8 mask = 1;
        for (i = 0; i < ed.mNumTevs; i++, mask <<= 1) {
            if (ed.mIndirectTargetStage & mask) {
                GXSetTevIndirect(static_cast<GXTevStageID>(i), GX_INDTEXSTAGE0,
                                 GX_ITF_8, GX_ITB_STU, GX_ITM_0, GX_ITW_OFF,
                                 GX_ITW_OFF, FALSE, FALSE, GX_ITBA_OFF);
            }
        }
    } else {
        GXSetNumIndStages(0);
    }

    GXSetZCompLoc(
        ed.mFlags & EmitterDrawSetting::FLAG_ZCOMP_BEFORE_TEX ? TRUE : FALSE);

    GXSetCullMode(GX_CULL_NONE);
    GXSetCoPlanar(FALSE);

    GXSetBlendMode(static_cast<GXBlendMode>(ed.mBlendMode.mType),
                   static_cast<GXBlendFactor>(ed.mBlendMode.mSrcFactor),
                   static_cast<GXBlendFactor>(ed.mBlendMode.mDstFactor),
                   static_cast<GXLogicOp>(ed.mBlendMode.mOp));

    GXSetZMode(
        ed.mFlags & EmitterDrawSetting::FLAG_ZCOMP_ENABLE_TEST ? TRUE : FALSE,
        static_cast<GXCompare>(ed.mZCompareFunc),
        ed.mFlags & EmitterDrawSetting::FLAG_ZCOMP_ENABLE_UPDATE ? TRUE
                                                                 : FALSE);

    if (ed.mFlags & EmitterDrawSetting::FLAG_FOG_ENABLE) {
        GXFogType type;
        f32 start, end, near, far;
        GXColor color;

        drawInfo.GetFog(&type, &start, &end, &near, &far, &color);
        GXSetFog(type, color, start, end, near, far);
    } else {
        GXColor color = {0, 0, 0, 0};
        GXSetFog(GX_FOG_NONE, color, 0.0f, 100.0f, 0.0f, 100.0f);
    }
}

void DrawStrategyImpl::_InitColor(ParticleManager* pManager,
                                  const EmitterDrawSetting& ed,
                                  const DrawInfo& drawInfo) {

#pragma unused(pManager)

    int i;

    mPrevARef0 = -1;
    mPrevARef1 = -1;

    GXSetNumChans(1);

    if (!drawInfo.IsLightEnable()) {

        GXSetChanCtrl(GX_COLOR0A0, FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL,
                      GX_DF_NONE, GX_AF_NONE);
    } else {
        if (ed.mColorInput.mRasColor == ColorInput::RASCOLOR_LIGHTING) {
            GXSetChanCtrl(GX_COLOR0, TRUE, GX_SRC_REG, GX_SRC_REG,
                          drawInfo.GetLightMask(), GX_DF_NONE,
                          drawInfo.IsSpotLight() ? GX_AF_SPOT : GX_AF_NONE);
        } else {
            GXSetChanCtrl(GX_COLOR0, FALSE, GX_SRC_REG, GX_SRC_REG,
                          GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        }

        if (ed.mAlphaInput.mRasColor == ColorInput::RASCOLOR_LIGHTING) {
            GXSetChanCtrl(GX_ALPHA0, TRUE, GX_SRC_REG, GX_SRC_REG,
                          drawInfo.GetLightAlphaMask(), GX_DF_NONE,
                          drawInfo.IsSpotLight() ? GX_AF_SPOT : GX_AF_NONE);
        } else {
            GXSetChanCtrl(GX_ALPHA0, FALSE, GX_SRC_REG, GX_SRC_REG,
                          GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        }
    }

    GXSetChanCtrl(GX_COLOR1A1, FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL,
                  GX_DF_NONE, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, drawInfo.GetChanMatColor());
    GXSetChanAmbColor(GX_COLOR0A0, drawInfo.GetChanAmbColor());

    mUseColor1_1 = FALSE;
    mUseColor1_2 = FALSE;
    mUseColor2_1 = FALSE;
    mUseColor2_2 = FALSE;
    mUseAlpha1_1 = FALSE;
    mUseAlpha1_2 = FALSE;
    mUseAlpha2_1 = FALSE;
    mUseAlpha2_2 = FALSE;

    for (i = 0; i < GX_MAX_TEVREG - 1; i++) {
        switch (ed.mColorInput.mTevColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            mUseColor1_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_2:
            mUseColor1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_MULT:
            mUseColor1_1 = TRUE;
            mUseColor1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_1:
            mUseColor2_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_2:
            mUseColor2_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_MULT:
            mUseColor2_1 = TRUE;
            mUseColor2_2 = TRUE;
            break;
        }

        switch (ed.mAlphaInput.mTevColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            mUseAlpha1_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_2:
            mUseAlpha1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_MULT:
            mUseAlpha1_1 = TRUE;
            mUseAlpha1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_1:
            mUseAlpha2_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_2:
            mUseAlpha2_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_MULT:
            mUseAlpha2_1 = TRUE;
            mUseAlpha2_2 = TRUE;
            break;
        }
    }

    for (i = 0; i < GX_MAX_TEVREG; i++) {
        switch (ed.mColorInput.mTevKColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            mUseColor1_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_2:
            mUseColor1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_MULT:
            mUseColor1_1 = TRUE;
            mUseColor1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_1:
            mUseColor2_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_2:
            mUseColor2_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_MULT:
            mUseColor2_1 = TRUE;
            mUseColor2_2 = TRUE;
            break;
        }

        switch (ed.mAlphaInput.mTevKColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            mUseAlpha1_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_2:
            mUseAlpha1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR1_MULT:
            mUseAlpha1_1 = TRUE;
            mUseAlpha1_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_1:
            mUseAlpha2_1 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_2:
            mUseAlpha2_2 = TRUE;
            break;
        case ColorInput::TEVCOLOR2_MULT:
            mUseAlpha2_1 = TRUE;
            mUseAlpha2_2 = TRUE;
            break;
        }
    }
}

void DrawStrategyImpl::SetupGP(Particle* pp, const EmitterDrawSetting& ed,
                               const DrawInfo& drawInfo, bool first,
                               bool xfdirty) {

    NW4R_POINTER_ASSERT(pp, 368);

    struct VtxDescData {
        GXAttrType type;     // at 0x0
        GXCompCnt compCnt;   // at 0x4
        GXCompType compType; // at 0x8
    };

    bool changed;
    bool XFbug = xfdirty;

    changed = _SetupACmp(pp, ed, first);
    XFbug = XFbug || changed;

    changed = _SetupTevReg(pp, ed, first);
    XFbug = XFbug || changed;

    changed = _SetupTexture(pp, ed, drawInfo, first);
    XFbug = XFbug && !changed;

    if ((ed.mFlags & EmitterDrawSetting::FLAG_CLIP_DISABLE) ||
        !pp->mParticleManager->mManagerEM->mManagerEF->mManagerES
             ->mXFFlushSafe) {
        return;
    }

    if (XFbug) {
        VtxDescData mVtxDescPos;
        VtxDescData mVtxDescNrm;
        VtxDescData mVtxDescClr[2];
        VtxDescData mVtxDescTex0;
        u8 frac;

        GXGetVtxDesc(GX_VA_POS, &mVtxDescPos.type);
        GXGetVtxDesc(GX_VA_NRM, &mVtxDescNrm.type);
        GXGetVtxDesc(GX_VA_CLR0, &mVtxDescClr[0].type);
        GXGetVtxDesc(GX_VA_CLR1, &mVtxDescClr[1].type);
        GXGetVtxDesc(GX_VA_TEX0, &mVtxDescTex0.type);

        GXGetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, &mVtxDescPos.compCnt,
                        &mVtxDescPos.compType, &frac);
        GXGetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, &mVtxDescNrm.compCnt,
                        &mVtxDescNrm.compType, &frac);
        GXGetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, &mVtxDescClr[0].compCnt,
                        &mVtxDescClr[0].compType, &frac);
        GXGetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR1, &mVtxDescClr[1].compCnt,
                        &mVtxDescClr[1].compType, &frac);
        GXGetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, &mVtxDescTex0.compCnt,
                        &mVtxDescTex0.compType, &frac);

        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 8);
        for (int i = 0; i < 8; i++) {
            if (mVtxDescPos.type != GX_NONE) {
                switch (mVtxDescPos.type) {
                case GX_DIRECT: {
                    switch (mVtxDescPos.compCnt) {
                    case GX_POS_XYZ: {
                        switch (mVtxDescPos.compType) {
                        case GX_U8: {
                            GXPosition3u8(0, 0, 0);
                            break;
                        }
                        case GX_S8: {
                            GXPosition3s8(0, 0, 0);
                            break;
                        }
                        case GX_U16: {
                            GXPosition3u16(0, 0, 0);
                            break;
                        }
                        case GX_S16: {
                            GXPosition3s16(0, 0, 0);
                            break;
                        }
                        case GX_F32: {
                            GXPosition3f32(0.0f, 0.0f, 0.0f);
                            break;
                        }
                        default: {
                            break;
                        }
                        }
                        break;
                    }

                    case GX_POS_XY: {
                        switch (mVtxDescPos.compType) {
                        case GX_U8: {
                            GXPosition2u8(0, 0);
                            break;
                        }
                        case GX_S8: {
                            GXPosition2s8(0, 0);
                            break;
                        }
                        case GX_U16: {
                            GXPosition2u16(0, 0);
                            break;
                        }
                        case GX_S16: {
                            GXPosition2s16(0, 0);
                            break;
                        }
                        case GX_F32: {
                            GXPosition2f32(0.0f, 0.0f);
                            break;
                        }
                        default: {
                            break;
                        }
                        }
                        break;
                    }

                    default: {
                        break;
                    }
                    }

                    break;
                }

                case GX_INDEX8: {
                    GXPosition1x8(0);
                    break;
                }

                case GX_INDEX16: {
                    GXPosition1x16(0);
                    break;
                }

                default: {
                    break;
                }
                }
            }

            if (mVtxDescNrm.type != GX_NONE) {
                switch (mVtxDescNrm.type) {
                case GX_DIRECT: {
                    switch (mVtxDescNrm.compCnt) {
                    case GX_NRM_XYZ: {
                        switch (mVtxDescNrm.compType) {
                        case GX_S8: {
                            GXNormal3u8(0, 0, 0);
                            break;
                        }
                        case GX_S16: {
                            GXNormal3u16(0, 0, 0);
                            break;
                        }
                        case GX_F32: {
                            GXNormal3f32(0.0f, 0.0f, 0.0f);
                            break;
                        }
                        //! @bug Missing GX_U8/GX_U16
                        default: {
                            break;
                        }
                        }

                        break;
                    }

                    default: {
                        break;
                    }
                    }

                    break;
                }

                case GX_INDEX8: {
                    GXNormal1x8(0);
                    break;
                }
                case GX_INDEX16: {
                    GXNormal1x16(0);
                    break;
                }
                default: {
                    break;
                }
                }
            }

            for (int j = 0; j < 2; ++j) {
                if (mVtxDescClr[j].type != GX_NONE) {
                    switch (mVtxDescClr[j].type) {
                    case GX_DIRECT:
                        switch (mVtxDescClr[j].compCnt) {
                        case GX_CLR_RGB:
                            GXColor3u8(0, 0, 0);
                            break;
                        case GX_CLR_RGBA:
                            GXColor4u8(0, 0, 0, 0);
                            break;
                        }
                        break;
                    case GX_INDEX8:
                        GXColor1x8(0);
                        break;
                    case GX_INDEX16:
                        GXColor1x16(0);
                        break;
                    default:
                        break;
                    }
                }
            }

            if (mVtxDescTex0.type != GX_NONE) {
                switch (mVtxDescTex0.type) {
                case GX_DIRECT: {

                    switch (mVtxDescTex0.compCnt) {
                    case GX_TEX_ST: {

                        switch (mVtxDescTex0.compType) {
                        case GX_U8: {
                            GXTexCoord2u8(0, 0);
                            break;
                        }

                        case GX_S8: {
                            GXTexCoord2s8(0, 0);
                            break;
                        }

                        case GX_U16: {
                            GXTexCoord2u16(0, 0);
                            break;
                        }

                        case GX_S16: {
                            GXTexCoord2s16(0, 0);
                            break;
                        }

                        case GX_F32: {
                            GXTexCoord2f32(0.0f, 0.0f);
                            break;
                        }

                        default: {
                            break;
                        }
                        }

                        break;
                    }

                    case GX_TEX_S: {

                        switch (mVtxDescTex0.compType) {
                        case GX_U8: {
                            GXTexCoord1u8(0);
                            break;
                        }

                        case GX_S8: {
                            GXTexCoord1s8(0);
                            break;
                        }

                        case GX_U16: {
                            GXTexCoord1u16(0);
                            break;
                        }

                        case GX_S16: {
                            GXTexCoord1s16(0);
                            break;
                        }

                        case GX_F32: {
                            GXTexCoord1f32(0.0f);
                            break;
                        }

                        default: {
                            break;
                        }
                        }

                        break;
                    }

                    default: {
                        break;
                    }
                    }

                    break;
                }

                case GX_INDEX8: {
                    GXTexCoord1x8(0);
                    break;
                }

                case GX_INDEX16: {
                    GXTexCoord1x16(0);
                    break;
                }

                default: {
                    break;
                }
                }
            }
        }
        GXEnd();
    }
}

bool DrawStrategyImpl::_SetupACmp(Particle* pp, const EmitterDrawSetting& ed,
                                  bool first) {

    NW4R_POINTER_ASSERT(pp, 591);

    //! @bug Copy-paste error, double comparison
#if defined(BUG_FIX)
    if (first || mPrevARef0 != pp->mParameter.mACmpRef0 ||
        mPrevARef1 != pp->mParameter.mACmpRef1) {
#else
    if (first || mPrevARef0 != pp->mParameter.mACmpRef0 ||
        mPrevARef0 != pp->mParameter.mACmpRef0) {
#endif

        //! @bug Copy-paste error, assigns ref0 to prev ref1
        mPrevARef0 = pp->mParameter.mACmpRef0;
#if defined(BUG_FIX)
        mPrevARef1 = pp->mParameter.mACmpRef1;
#else
        mPrevARef1 = pp->mParameter.mACmpRef0;
#endif

        GXSetAlphaCompare(
            static_cast<GXCompare>(ed.mACmpComp0), pp->mParameter.mACmpRef0,
            static_cast<GXAlphaOp>(ed.mACmpOp),
            static_cast<GXCompare>(ed.mACmpComp1), pp->mParameter.mACmpRef1);

        return TRUE;
    }

    return FALSE;
}

bool DrawStrategyImpl::_SetupTevReg(Particle* pp, const EmitterDrawSetting& ed,
                                    bool first) {

    bool changed = FALSE;

    if (mUseColor1_1) {
        pp->GetColorRGB(0, 0, &mDrawColor[0][0]);
    }
    if (mUseColor1_2) {
        pp->GetColorRGB(0, 1, &mDrawColor[0][1]);
    }
    if (mUseColor2_1) {
        pp->GetColorRGB(1, 0, &mDrawColor[1][0]);
    }
    if (mUseColor2_2) {
        pp->GetColorRGB(1, 1, &mDrawColor[1][1]);
    }

    u8 alphaFlick = 255;
    //! \~english @note Code is not optimal. Even if the check below is false,
    //! all of the mUseAlpha comparisons will still be attempted.
    //! \~japanese @note コードは不適。たとえ以下のチェックの結果はFALSEでも、
    //! 全てのmUseAlpha比較はもう試みる。
    if (mUseAlpha1_1 || mUseAlpha1_2 || mUseAlpha2_1 || mUseAlpha2_2) {
        if (ed.mAlphaFlickType) {
            alphaFlick = pp->GetAlphaFlick();
        }
    }
    if (mUseAlpha1_1) {
        mDrawColor[0][0].a = pp->GetAlpha(0, 0);
        if (alphaFlick != 255) {
            mDrawColor[0][0].a =
                static_cast<u8>((mDrawColor[0][0].a * alphaFlick + 128) >> 8);
        }
    }
    if (mUseAlpha1_2) {
        mDrawColor[0][1].a = pp->GetAlpha(0, 1);
        if (alphaFlick != 255) {
            mDrawColor[0][1].a =
                static_cast<u8>((mDrawColor[0][1].a * alphaFlick + 128) >> 8);
        }
    }
    if (mUseAlpha2_1) {
        mDrawColor[1][0].a = pp->GetAlpha(1, 0);
        if (alphaFlick != 255) {
            mDrawColor[1][0].a =
                static_cast<u8>((mDrawColor[1][0].a * alphaFlick + 128) >> 8);
        }
    }
    if (mUseAlpha2_2) {
        mDrawColor[1][1].a = pp->GetAlpha(1, 1);
        if (alphaFlick != 255) {
            mDrawColor[1][1].a =
                static_cast<u8>((mDrawColor[1][1].a * alphaFlick + 128) >> 8);
        }
    }

    if (pp->mParticleManager->mModifier.mLight.mType) {
        if (mUseColor1_1 || mUseColor1_2 || mUseAlpha1_1 || mUseAlpha1_2) {
            pp->mParticleManager->Draw_ModifyColor(pp, &mDrawColor[0][0],
                                                   &mDrawColor[0][1]);
        }
        if (mUseColor2_1 || mUseColor2_2 || mUseAlpha2_1 || mUseAlpha2_2) {
            pp->mParticleManager->Draw_ModifyColor(pp, &mDrawColor[1][0],
                                                   &mDrawColor[1][1]);
        }
    }

    int i;
    for (i = 0; i < GX_MAX_TEVREG - 1; i++) {
        GXColor color = {0, 0, 0, 0};

        if (!ed.mColorInput.mTevColor[i] && !ed.mAlphaInput.mTevColor[i]) {
            continue;
        }

        switch (ed.mColorInput.mTevColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            color.r = mDrawColor[0][0].r;
            color.g = mDrawColor[0][0].g;
            color.b = mDrawColor[0][0].b;
            break;
        case ColorInput::TEVCOLOR1_2:
            color.r = mDrawColor[0][1].r;
            color.g = mDrawColor[0][1].g;
            color.b = mDrawColor[0][1].b;
            break;
        case ColorInput::TEVCOLOR1_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[0][0], mDrawColor[0][1]);
            color.r = mult.r;
            color.g = mult.g;
            color.b = mult.b;
        } break;
        case ColorInput::TEVCOLOR2_1:
            color.r = mDrawColor[1][0].r;
            color.g = mDrawColor[1][0].g;
            color.b = mDrawColor[1][0].b;
            break;
        case ColorInput::TEVCOLOR2_2:
            color.r = mDrawColor[1][1].r;
            color.g = mDrawColor[1][1].g;
            color.b = mDrawColor[1][1].b;
            break;
        case ColorInput::TEVCOLOR2_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[1][0], mDrawColor[1][1]);
            color.r = mult.r;
            color.g = mult.g;
            color.b = mult.b;
        } break;
        }

        switch (ed.mAlphaInput.mTevColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            color.a = mDrawColor[0][0].a;
            break;
        case ColorInput::TEVCOLOR1_2:
            color.a = mDrawColor[0][1].a;
            break;
        case ColorInput::TEVCOLOR1_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[0][0], mDrawColor[0][1]);
            color.a = mult.a;
        } break;
        case ColorInput::TEVCOLOR2_1:
            color.a = mDrawColor[1][0].a;
            break;
        case ColorInput::TEVCOLOR2_2:
            color.a = mDrawColor[1][1].a;
            break;
        case ColorInput::TEVCOLOR2_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[1][0], mDrawColor[1][1]);
            color.a = mult.a;
        } break;
        }

        if (first || mPrevColor[i].r != color.r || mPrevColor[i].g != color.g ||
            mPrevColor[i].b != color.b || mPrevColor[i].a != color.a) {
            GXSetTevColor(static_cast<GXTevRegID>(GX_TEVREG0 + i), color);
            mPrevColor[i] = color;
            changed = TRUE;
        }
    }

    for (i = 0; i < GX_MAX_TEVREG; i++) {
        GXColor color = {0, 0, 0, 0};

        if (ed.mColorInput.mTevKColor[i] == ColorInput::TEVCOLOR_NULL &&
            ed.mAlphaInput.mTevKColor[i] == ColorInput::TEVCOLOR_NULL) {
            continue;
        }

        switch (ed.mColorInput.mTevKColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            color.r = mDrawColor[0][0].r;
            color.g = mDrawColor[0][0].g;
            color.b = mDrawColor[0][0].b;
            break;
        case ColorInput::TEVCOLOR1_2:
            color.r = mDrawColor[0][1].r;
            color.g = mDrawColor[0][1].g;
            color.b = mDrawColor[0][1].b;
            break;
        case ColorInput::TEVCOLOR1_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[0][0], mDrawColor[0][1]);
            color.r = mult.r;
            color.g = mult.g;
            color.b = mult.b;
        } break;
        case ColorInput::TEVCOLOR2_1:
            color.r = mDrawColor[1][0].r;
            color.g = mDrawColor[1][0].g;
            color.b = mDrawColor[1][0].b;
            break;
        case ColorInput::TEVCOLOR2_2:
            color.r = mDrawColor[1][1].r;
            color.g = mDrawColor[1][1].g;
            color.b = mDrawColor[1][1].b;
            break;
        case ColorInput::TEVCOLOR2_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[1][0], mDrawColor[1][1]);
            color.r = mult.r;
            color.g = mult.g;
            color.b = mult.b;
        } break;
        }

        switch (ed.mAlphaInput.mTevKColor[i]) {
        case ColorInput::TEVCOLOR1_1:
            color.a = mDrawColor[0][0].a;
            break;
        case ColorInput::TEVCOLOR1_2:
            color.a = mDrawColor[0][1].a;
            break;
        case ColorInput::TEVCOLOR1_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[0][0], mDrawColor[0][1]);
            color.a = mult.a;
        } break;
        case ColorInput::TEVCOLOR2_1:
            color.a = mDrawColor[1][0].a;
            break;
        case ColorInput::TEVCOLOR2_2:
            color.a = mDrawColor[1][1].a;
            break;
        case ColorInput::TEVCOLOR2_MULT: {
            GXColor mult;
            COLOR_MODULATE(mult, mDrawColor[1][0], mDrawColor[1][1]);
            color.a = mult.a;
        } break;
        }

        if (first || mPrevKColor[i].r != color.r ||
            mPrevKColor[i].g != color.g || mPrevKColor[i].b != color.b ||
            mPrevKColor[i].a != color.a) {
            GXSetTevKColor(static_cast<GXTevKColorID>(i), color);
            mPrevKColor[i] = color;
            changed = TRUE;
        }
    }

    return changed;
}

bool DrawStrategyImpl::_SetupTexture(Particle* pp, const EmitterDrawSetting& ed,
                                     const DrawInfo& drawInfo, bool first) {

    NW4R_POINTER_ASSERT(pp, 881);

    bool changed = FALSE;

    for (int texIndex = 0; texIndex < TEX_LAYER_MAX; texIndex++) {
        if (mTexmapMap[texIndex] >= 0) {
            NW4R_POINTER_ASSERT(pp->mParameter.mTexture[texIndex], 890);
            if (pp->mParameter.mTexture[texIndex] == NULL) {
                continue;
            }

            const TextureData* texData = pp->mParameter.mTexture[texIndex];

            GXTexWrapMode wrapS = pp->GetTextureWrapS(texIndex);
            GXTexWrapMode wrapT = pp->GetTextureWrapT(texIndex);

            if (first || texData != mPrevTexture[texIndex].data ||
                wrapS != mPrevTexture[texIndex].wrapS ||
                wrapT != mPrevTexture[texIndex].wrapT) {

                changed = TRUE;
                mPrevTexture[texIndex].data = texData;
                mPrevTexture[texIndex].data = texData;
                mPrevTexture[texIndex].wrapS = wrapS;
                mPrevTexture[texIndex].wrapT = wrapT;

                int texfmt;
                GXTexObj texobj;
                GXTlutObj tlutobj;

                texfmt = texData->format;

                switch (texfmt) {
                case GX_TF_C4:
                case GX_TF_C8:
                case GX_TF_C14X2:
                    GXInitTlutObj(&tlutobj, texData->tlut,
                                  static_cast<GXTlutFmt>(texData->tlutFormat),
                                  texData->tlutEntries);
                    GXLoadTlut(&tlutobj,
                               static_cast<u32>(mTexmapMap[texIndex]));
                    GXInitTexObjCI(&texobj, texData->texture, texData->width,
                                   texData->height,
                                   static_cast<GXCITexFmt>(texfmt), wrapS,
                                   wrapT, (texData->mipmap > 1) ? TRUE : FALSE,
                                   static_cast<u32>(mTexmapMap[texIndex]));
                    break;
                case GX_TF_I4:
                case GX_TF_I8:
                case GX_TF_IA4:
                case GX_TF_IA8:
                case GX_TF_RGB565:
                case GX_TF_RGB5A3:
                case GX_TF_RGBA8:
                case GX_TF_CMPR:
                    GXInitTexObj(&texobj, texData->texture, texData->width,
                                 texData->height, static_cast<GXTexFmt>(texfmt),
                                 wrapS, wrapT,
                                 (texData->mipmap > 1) ? TRUE : FALSE);
                    break;
                default:
                    NW4R_ASSERT(FALSE, 954);
                }

                f32 lodmax = -1.0f + texData->mipmap;
                f32 lodbias = 0.0f;

                GXInitTexObjLOD(
                    &texobj, static_cast<GXTexFilter>(texData->min_filt),
                    static_cast<GXTexFilter>(texData->mag_filt), 0.0F, lodmax,
                    texData->lod_bias, FALSE, FALSE, GX_ANISO_1);
                GXLoadTexObj(&texobj,
                             static_cast<GXTexMapID>(mTexmapMap[texIndex]));
            }

            f32 scaleS = pp->Draw_GetTextureScaleS(texIndex);
            f32 scaleT = pp->Draw_GetTextureScaleT(texIndex);
            f32 transS = pp->Draw_GetTextureTransS(texIndex);
            f32 transT = pp->Draw_GetTextureTransT(texIndex);

            if (first || scaleS != mPrevTexture[texIndex].scaleS ||
                scaleT != mPrevTexture[texIndex].scaleT ||
                transS != mPrevTexture[texIndex].transS ||
                transT != mPrevTexture[texIndex].transT ||
                pp->mParameter.mTextureScale[texIndex].x !=
                    mPrevTexture[texIndex].scale.x ||
                pp->mParameter.mTextureScale[texIndex].y !=
                    mPrevTexture[texIndex].scale.y ||
                pp->mParameter.mTextureRotate[texIndex] !=
                    mPrevTexture[texIndex].rotate ||
                pp->mParameter.mTextureTranslate[texIndex].x !=
                    mPrevTexture[texIndex].translate.x ||
                pp->mParameter.mTextureTranslate[texIndex].y !=
                    mPrevTexture[texIndex].translate.y) {

                changed = TRUE;
                mPrevTexture[texIndex].scaleS = scaleS;
                mPrevTexture[texIndex].scaleT = scaleT;
                mPrevTexture[texIndex].transS = transS;
                mPrevTexture[texIndex].transT = transT;
                mPrevTexture[texIndex].scale.x =
                    pp->mParameter.mTextureScale[texIndex].x;
                mPrevTexture[texIndex].scale.y =
                    pp->mParameter.mTextureScale[texIndex].y;
                mPrevTexture[texIndex].rotate =
                    pp->mParameter.mTextureRotate[texIndex];
                mPrevTexture[texIndex].translate.x =
                    pp->mParameter.mTextureTranslate[texIndex].x;
                mPrevTexture[texIndex].translate.y =
                    pp->mParameter.mTextureTranslate[texIndex].y;

                {
                    nw4r::math::MTX34 texmtx;
                    nw4r::math::MTX34Identity(&texmtx);

                    texmtx._03 = pp->mParameter.mTextureTranslate[texIndex].x;
                    texmtx._13 = pp->mParameter.mTextureTranslate[texIndex].y;

                    if (pp->mParameter.mTextureRotate[texIndex] != 0.0f) {
                        f32 x = texmtx._03 - 0.5f;
                        f32 y = texmtx._13 - 0.5f;

                        f32 sr;
                        f32 cr;
                        PSSinCosRad(&sr, &cr,
                                    pp->mParameter.mTextureRotate[texIndex]);

                        texmtx._00 = texmtx._11 = cr;
                        texmtx._01 = -sr;
                        texmtx._10 = sr;
                        texmtx._03 = 0.5f + cr * x - sr * y;
                        texmtx._13 = 0.5f + sr * x + cr * y;
                    }

                    if (scaleS != 1.0f || scaleT != 1.0f ||
                        pp->mParameter.mTextureScale[texIndex].x != 1.0f ||
                        pp->mParameter.mTextureScale[texIndex].y != 1.0f ||
                        transS != 0 || transT != 0) {

                        f32 sssx =
                            scaleS * pp->mParameter.mTextureScale[texIndex].x;
                        f32 stsy =
                            scaleT * pp->mParameter.mTextureScale[texIndex].y;
                        texmtx._00 *= sssx;
                        texmtx._01 *= sssx;
                        texmtx._03 =
                            scaleS *
                                (0.5f +
                                 (texmtx._03 - 0.5f) *
                                     pp->mParameter.mTextureScale[texIndex].x) +
                            transS;
                        texmtx._10 *= stsy;
                        texmtx._11 *= stsy;
                        texmtx._13 =
                            scaleT *
                                (0.5f +
                                 (texmtx._13 - 0.5f) *
                                     pp->mParameter.mTextureScale[texIndex].y) +
                            transT;
                    }

                    if (((ed.mFlags >> (7 + texIndex)) & 1) == NULL) {
                        GXLoadTexMtxImm(
                            texmtx,
                            static_cast<u32>(GX_TEXMTX0 +
                                             mTexmapMap[texIndex] * 3),
                            GX_MTX_2x4);

                        if (first) {
                            GXSetTexCoordGen(
                                static_cast<GXTexCoordID>(mTexmapMap[texIndex]),
                                GX_TG_MTX2x4, GX_TG_TEX0,
                                static_cast<u32>(GX_TEXMTX0 +
                                                 mTexmapMap[texIndex] * 3));
                        }

                    } else {
                        math::MTX34Mult(&texmtx, drawInfo.GetProjMtx(),
                                        &texmtx);

                        GXLoadTexMtxImm(
                            texmtx,
                            static_cast<u32>(GX_PTTEXMTX0 +
                                             mTexmapMap[texIndex] * 3),
                            GX_MTX_3x4);

                        if (first) {
                            GXSetTexCoordGen2(
                                static_cast<GXTexCoordID>(mTexmapMap[texIndex]),
                                GX_TG_MTX3x4, GX_TG_POS, GX_PNMTX0, FALSE,
                                static_cast<u32>(GX_PTTEXMTX0 +
                                                 mTexmapMap[texIndex] * 3));
                        }
                    }
                }
            }
        }
    }
    return changed;
}

DrawStrategyImpl::GetFirstDrawParticleFunc
DrawStrategyImpl::GetGetFirstDrawParticleFunc(int drawOrder) {

    switch (drawOrder) {
    case DRAWORDER_YOUNGERS_FIRST: {
        return GetFirstDrawParticle_YoungersFirst;
    }

    default: /* DRAWORDER_ELDERS_FIRST */ {
        return GetFirstDrawParticle_EldersFirst;
    }
    }
}

DrawStrategyImpl::GetNextDrawParticleFunc
DrawStrategyImpl::GetGetNextDrawParticleFunc(int drawOrder) {

    switch (drawOrder) {
    case DRAWORDER_YOUNGERS_FIRST: {
        return GetNextDrawParticle_YoungersFirst;
    }

    default: /* DRAWORDER_ELDERS_FIRST */ {
        return GetNextDrawParticle_EldersFirst;
    }
    }
}

Particle*
DrawStrategyImpl::GetFirstDrawParticle_EldersFirst(ParticleManager* pManager) {

    return GetOldestDrawParticle(pManager);
}

Particle* DrawStrategyImpl::GetFirstDrawParticle_YoungersFirst(
    ParticleManager* pManager) {

    return GetYoungestDrawParticle(pManager);
}

Particle* DrawStrategyImpl::GetNextDrawParticle_EldersFirst(ParticleManager* pm,
                                                            Particle* current) {

    return GetYoungerDrawParticle(pm, current);
}

Particle*
DrawStrategyImpl::GetNextDrawParticle_YoungersFirst(ParticleManager* pManager,
                                                    Particle* pp) {

    return GetElderDrawParticle(pManager, pp);
}

/******************************************************************************
 *
 * AheadContext
 *
 ******************************************************************************/
DrawStrategyImpl::AheadContext::AheadContext(const math::MTX34& viewMtx,
                                             ParticleManager* pm) {

    NW4R_POINTER_ASSERT(pm, 1177);

    mCommon.mViewMtx = &viewMtx;
    mCommon.mParticleManager = pm;

    NW4R_POINTER_ASSERT(pm->mManagerEM, 1182);
    pm->mManagerEM->CalcGlobalMtx(&mCommon.mEmitterMtx);
    pm->CalcGlobalMtx(&mCommon.mParticleManagerMtx);

    if (!math::MTX34Inv(&mCommon.mParticleManagerMtxInv,
                        &mCommon.mParticleManagerMtx)) {
        math::MTX34Identity(&mCommon.mParticleManagerMtxInv);
    }

    math::VEC3 yAxis(mCommon.mEmitterMtx._01, mCommon.mEmitterMtx._11,
                     mCommon.mEmitterMtx._21);
    math::VEC3TransformNormal(&yAxis, &mCommon.mParticleManagerMtxInv, &yAxis);

    if (!Normalize(&yAxis)) {
        yAxis = math::VEC3(0.0f, 1.0f, 0.0f);
    }

    mCommon.mEmitterAxisY = yAxis;

    math::VEC3 center(mCommon.mEmitterMtx._03, mCommon.mEmitterMtx._13,
                      mCommon.mEmitterMtx._23);
    math::VEC3TransformCoord(&center, &mCommon.mParticleManagerMtxInv, &center);

    mCommon.mEmitterCenter = center;

    EmitterDrawSetting& ed = *pm->mResource->GetEmitterDrawSetting();

    switch (ed.typeDir) {
    case EmitterDrawSetting::DIR_NO_DESIGN:
    case EmitterDrawSetting::DIR_NO_DESIGN2: {
        math::MTX34& rMtx = mCommon.mParticleManagerMtxInv;
        math::VEC3 worldAxisY(rMtx._01, rMtx._11, rMtx._21);

        if (!Normalize(&worldAxisY)) {
            worldAxisY = math::VEC3(0.0f, 1.0f, 0.0f);
        }

        mNoDesign.mWorldYAxis = worldAxisY;
        break;
    }

    default: {
        break;
    }
    }
}

} // namespace ef
} // namespace nw4r
