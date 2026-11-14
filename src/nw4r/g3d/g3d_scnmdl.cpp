#include <nw4r/g3d.h>

namespace nw4r {
namespace g3d {

NW4R_G3D_RTTI_DEF(ScnMdl);

/******************************************************************************
 *
 * CopiedMatAccess
 *
 ******************************************************************************/
ResTexObj ScnMdl::CopiedMatAccess::GetResTexObj(bool markDirty) {
    if (mpScnMdl != NULL && mTexObj.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFFER_RESTEXOBJ);
        }

        return mTexObj;
    }

    return ResTexObj(NULL);
}

ResTexSrt ScnMdl::CopiedMatAccess::GetResTexSrt(bool markDirty) {
    if (mpScnMdl != NULL && mTexSrt.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFFER_RESTEXSRT);
        }

        return mTexSrt;
    }

    return ResTexSrt(NULL);
}

ResMatChan ScnMdl::CopiedMatAccess::GetResMatChan(bool markDirty) {
    if (mpScnMdl != NULL && mChan.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFFER_RESCHAN);
        }

        return mChan;
    }

    return ResMatChan(NULL);
}

ResGenMode ScnMdl::CopiedMatAccess::GetResGenMode(bool markDirty) {
    if (mpScnMdl != NULL && mGenMode.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFFER_RESGENMODE);
        }

        return mGenMode;
    }

    return ResGenMode(NULL);
}

ResMatPix ScnMdl::CopiedMatAccess::GetResMatPix(bool markDirty) {
    if (mpScnMdl != NULL && mPix.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFFER_RESMATPIX);
        }

        return mPix;
    }

    return ResMatPix(NULL);
}

ResMatTevColor ScnMdl::CopiedMatAccess::GetResMatTevColor(bool markDirty) {
    if (mpScnMdl != NULL && mTevColor.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFFER_RESTEVCOLOR);
        }

        return mTevColor;
    }

    return ResMatTevColor(NULL);
}

ResTev ScnMdl::CopiedMatAccess::GetResTev(bool markDirty) {
    if (mpScnMdl != NULL && mTev.IsValid()) {
        if (markDirty) {
            mpScnMdl->MatBufferDirty(mMatID, ScnMdl::BUFFER_RESTEV);
        }

        return mTev;
    }

    return ResTev(NULL);
}

ResTexObj ScnMdl::CopiedMatAccess::GetResTexObjEx() {
    if (mpScnMdl) {
        if (mTexObj.IsValid()) {
            return mTexObj;
        }
        return mpScnMdl->GetResMdl().GetResMat(mMatID).GetResTexObj();
    }
    return ResTexObj(NULL);
}

ResTexSrt ScnMdl::CopiedMatAccess::GetResTexSrtEx() {
    if (mpScnMdl) {
        if (mTexSrt.IsValid()) {
            return mTexSrt;
        }
        return mpScnMdl->GetResMdl().GetResMat(mMatID).GetResTexSrt();
    }
    return ResTexSrt(NULL);
}

ResMatChan ScnMdl::CopiedMatAccess::GetResMatChanEx() {
    if (mpScnMdl) {
        if (mChan.IsValid()) {
            return mChan;
        }
        return mpScnMdl->GetResMdl().GetResMat(mMatID).GetResMatChan();
    }
    return ResMatChan(NULL);
}

ResGenMode ScnMdl::CopiedMatAccess::GetResGenModeEx() {
    if (mpScnMdl) {
        if (mGenMode.IsValid())
            return mGenMode;

        return mpScnMdl->GetResMdl().GetResMat(mMatID).GetResGenMode();
    }
    return ResGenMode(NULL);
}

ResMatPix ScnMdl::CopiedMatAccess::GetResMatPixEx() {
    if (mpScnMdl) {
        if (mPix.IsValid())
            return mPix;

        return mpScnMdl->GetResMdl().GetResMat(mMatID).GetResMatPix();
    }
    return ResMatPix(NULL);
}

ResMatTevColor ScnMdl::CopiedMatAccess::GetResMatTevColorEx() {
    if (mpScnMdl) {
        if (mTevColor.IsValid())
            return mTevColor;

        return mpScnMdl->GetResMdl().GetResMat(mMatID).GetResMatTevColor();
    }
    return ResMatTevColor(NULL);
}

ScnMdl::CopiedMatAccess::CopiedMatAccess(ScnMdl* pObj, u32 id)
    : mTexObj(NULL),
      mTlutObj(NULL),
      mTexSrt(NULL),
      mChan(NULL),
      mGenMode(NULL),
      mMatMisc(NULL),
      mPix(NULL),
      mTevColor(NULL),
      mIndMtxAndScale(NULL),
      mTexCoordGen(NULL),
      mTev(NULL) {

    if (pObj != NULL && pObj->GetResMdl().GetResMat(id).IsValid()) {
        mpScnMdl = pObj;
        mMatID = id;

        DrawResMdlReplacement& rReplacement = pObj->mReplacement;

        if (rReplacement.texObjDataArray != NULL) {
            mTexObj = ResTexObj(&rReplacement.texObjDataArray[id]);
        } else {
            mTexObj = ResTexObj(NULL);
        }

        if (rReplacement.tlutObjDataArray != NULL) {
            mTlutObj = ResTlutObj(&rReplacement.tlutObjDataArray[id]);
        } else {
            mTlutObj = ResTlutObj(NULL);
        }

        if (rReplacement.texSrtDataArray != NULL) {
            mTexSrt = ResTexSrt(&rReplacement.texSrtDataArray[id]);
        } else {
            mTexSrt = ResTexSrt(NULL);
        }

        if (rReplacement.chanDataArray != NULL) {
            mChan = ResMatChan(&rReplacement.chanDataArray[id]);
        } else {
            mChan = ResMatChan(NULL);
        }

        if (rReplacement.genModeDataArray != NULL) {
            mGenMode = ResGenMode(&rReplacement.genModeDataArray[id]);
        } else {
            mGenMode = ResGenMode(NULL);
        }

        if (rReplacement.matMiscDataArray != NULL) {
            mMatMisc = ResMatMisc(&rReplacement.matMiscDataArray[id]);
        } else {
            mMatMisc = ResMatMisc(NULL);
        }

        if (rReplacement.pixDLArray != NULL) {
            mPix = ResMatPix(&rReplacement.pixDLArray[id]);
        } else {
            mPix = ResMatPix(NULL);
        }

        if (rReplacement.tevColorDLArray != NULL) {
            mTevColor = ResMatTevColor(&rReplacement.tevColorDLArray[id]);
        } else {
            mTevColor = ResMatTevColor(NULL);
        }

        if (rReplacement.indMtxAndScaleDLArray != NULL) {
            mIndMtxAndScale =
                ResMatIndMtxAndScale(&rReplacement.indMtxAndScaleDLArray[id]);
        } else {
            mIndMtxAndScale = ResMatIndMtxAndScale(NULL);
        }

        if (rReplacement.texCoordGenDLArray != NULL) {
            mTexCoordGen =
                ResMatTexCoordGen(&rReplacement.texCoordGenDLArray[id]);
        } else {
            mTexCoordGen = ResMatTexCoordGen(NULL);
        }

        if (rReplacement.tevDataArray != NULL) {
            mTev = ResTev(&rReplacement.tevDataArray[id]);
        } else {
            mTev = ResTev(NULL);
        }

    } else {
        mpScnMdl = NULL;
        mMatID = id;

        mTexObj = ResTexObj(NULL);
        mTlutObj = ResTlutObj(NULL);
        mTexSrt = ResTexSrt(NULL);
        mChan = ResMatChan(NULL);
        mGenMode = ResGenMode(NULL);
        mMatMisc = ResMatMisc(NULL);
        mPix = ResMatPix(NULL);
        mTevColor = ResMatTevColor(NULL);
        mIndMtxAndScale = ResMatIndMtxAndScale(NULL);
        mTexCoordGen = ResMatTexCoordGen(NULL);
        mTev = ResTev(NULL);
    }
}

/******************************************************************************
 *
 * CopiedVisAccess
 *
 ******************************************************************************/
bool ScnMdl::CopiedVisAccess::IsVisible() const {
    if (mpScnMdl != NULL) {
        if (mpVis != NULL) {
            return *mpVis != 0;
        }

        return mpScnMdl->GetResMdl().GetResNode(mNodeID).IsVisible();
    }

    return false;
}

bool ScnMdl::CopiedVisAccess::SetVisibilityEx(bool visible) {
    if (mpScnMdl != NULL) {
        if (mpVis != NULL) {
            if (visible && *mpVis == 0 || !visible && *mpVis != 0) {
                mpScnMdl->VisBufferDirty();
            }

            *mpVis = visible;
        } else {
            mpScnMdl->GetResMdl().GetResNode(mNodeID).SetVisibility(visible);
        }

        return true;
    }

    return false;
}

ScnMdl::CopiedVisAccess::CopiedVisAccess(ScnMdl* pObj, u32 id) {
    if (pObj != NULL && pObj->GetResMdl().GetResNode(id).IsValid()) {
        mpScnMdl = pObj;
        mNodeID = id;

        if (pObj->mReplacement.visArray != NULL) {
            mpVis = &pObj->mReplacement.visArray[id];
        } else {
            mpVis = NULL;
        }
    } else {
        mpScnMdl = NULL;
        mNodeID = id;
        mpVis = NULL;
    }
}

/******************************************************************************
 *
 * ScnMdl
 *
 ******************************************************************************/
ScnMdl* ScnMdl::Construct(MEMAllocator* pHeap, u32* pSize, ResMdl mdl,
                          u32 bufferOption, int nView) {

    if (!mdl.IsValid()) {
        return NULL;
    }

    if (nView == 0) {
        nView = 1;
    } else if (nView > VIEW_MAX) {
        nView = VIEW_MAX;
    }

    ScnMdl* pObj = NULL;

    u32 numWorldMtx = mdl.GetResMdlInfo().GetNumPosNrmMtx();
    u32 numViewMtx = mdl.GetResMdlInfo().GetNumViewMtx();
    u32 numMat = mdl.GetResMatNumEntries();
    u32 numNode = mdl.GetResNodeNumEntries();

    u32 sizeScnMdl = sizeof(ScnMdl);
    u32 sizeWorldMtxArray = numWorldMtx * sizeof(math::MTX34);
    u32 sizeWorldMtxAttribArray = numWorldMtx * sizeof(u32);

    u32 sizeViewPosMtxArrayUnit = numViewMtx * sizeof(math::MTX34);
    u32 sizeViewPosMtxArray = nView * align32(sizeViewPosMtxArrayUnit);

    u32 sizeViewNrmMtxArrayUnit = numViewMtx * sizeof(math::MTX33);
    u32 sizeViewNrmMtxArray = mdl.GetResMdlInfo().ref().need_nrm_mtx_array
                                  ? nView * align32(sizeViewNrmMtxArrayUnit)
                                  : 0;

    u32 viewTexMtxArrayUnitSize = sizeViewPosMtxArrayUnit;

    // TODO(kiwi) Fakematch
    u32 sizeViewTexMtxArray = mdl.ref().info.need_tex_mtx_array
                                  ? nView * align32(viewTexMtxArrayUnitSize)
                                  : 0;

    u32 matBufferDirtyFlagSize = numMat * sizeof(u32);

    // clang-format off
    u32 resTexObjSize         = (bufferOption & BUFFER_RESTEXOBJ)         ? numMat  * sizeof(ResTexObjData)        : 0;
    u32 resTlutObjSize        = (bufferOption & BUFFER_RESTLUTOBJ)        ? numMat  * sizeof(ResTlutObjData)       : 0;
    u32 resTexSrtSize         = (bufferOption & BUFFER_RESTEXSRT)         ? numMat  * sizeof(ResTexSrtData)        : 0;
    u32 resChanSize           = (bufferOption & BUFFER_RESCHAN)           ? numMat  * sizeof(ResChanData)          : 0;
    u32 resGenModeSize        = (bufferOption & BUFFER_RESGENMODE)        ? numMat  * sizeof(ResGenModeData)       : 0;
    u32 resMatMiscSize        = (bufferOption & BUFFER_RESMATMISC)        ? numMat  * sizeof(ResMatMiscData)       : 0;
    u32 visSize               = (bufferOption & BUFFER_RESANMVIS)         ? numNode * sizeof(u8)                   : 0;
    u32 resPixSize            = (bufferOption & BUFFER_RESMATPIX)         ? numMat  * sizeof(ResPixDL)             : 0;
    u32 resTevColorSize       = (bufferOption & BUFFER_RESTEVCOLOR)       ? numMat  * sizeof(ResTevColorDL)        : 0;
    u32 resIndMtxAndScaleSize = (bufferOption & BUFFER_RESMATINDMTXSCALE) ? numMat  * sizeof(ResIndMtxAndScaleDL)  : 0;
    u32 resTexCoordGenSize    = (bufferOption & BUFFER_RESMATTEXCOORDGEN) ? numMat  * sizeof(ResTexCoordGenDL)     : 0;
    u32 resTevSize            = (bufferOption & BUFFER_RESTEV)            ? numMat  * sizeof(ResTevData)           : 0;
    // clang-format on

    u32 vtxPosTableSize = 0;
    u32 vtxPosBufferSize = 0;

    if (bufferOption & BUFFER_RESVTXPOS) {
        u32 vtxPosNum = mdl.GetResVtxPosNumEntries();
        u32 shpNum = mdl.GetResShpNumEntries();

        vtxPosTableSize = vtxPosNum * sizeof(ResVtxPosData*);

        for (u32 i = 0; i < vtxPosNum; i++) {
            u32 j;
            ResVtxPos pos = mdl.GetResVtxPos(i);

            for (j = 0; j < shpNum; j++) {
                ResShp shp = mdl.GetResShp(j);

                if (pos.ptr() == shp.GetResVtxPos().ptr()) {
                    break;
                }
            }

            if (j != shpNum) {
                vtxPosBufferSize += align32(pos.GetSize());
            }
        }
    }

    u32 vtxNrmTableSize = 0;
    u32 vtxNrmBufferSize = 0;

    if (bufferOption & BUFFER_RESVTXNRM) {
        u32 vtxNrmNum = mdl.GetResVtxNrmNumEntries();
        u32 shpNum = mdl.GetResShpNumEntries();

        vtxNrmTableSize = vtxNrmNum * sizeof(ResVtxNrmData*);

        for (u32 i = 0; i < vtxNrmNum; i++) {
            u32 j;
            ResVtxNrm nrm = mdl.GetResVtxNrm(i);

            for (j = 0; j < shpNum; j++) {
                ResShp shp = mdl.GetResShp(j);

                if (nrm.ptr() == shp.GetResVtxNrm().ptr()) {
                    break;
                }
            }

            if (j != shpNum) {
                vtxNrmBufferSize += align32(nrm.GetSize());
            }
        }
    }

    u32 vtxClrTableSize = 0;
    u32 sizeVtxClrBuffer = 0;

    if (bufferOption & BUFFER_RESVTXCLR) {
        u32 vtxClrNum = mdl.GetResVtxClrNumEntries();
        u32 shpNum = mdl.GetResShpNumEntries();

        vtxClrTableSize = vtxClrNum * sizeof(ResVtxClrData*);

        for (u32 i = 0; i < vtxClrNum; i++) {
            u32 j;
            ResVtxClr clr = mdl.GetResVtxClr(i);

            for (j = 0; j < shpNum; j++) {
                ResShp shp = mdl.GetResShp(j);

                if (clr.ptr() == shp.GetResVtxClr(0).ptr() ||
                    clr.ptr() == shp.GetResVtxClr(1).ptr()) {
                    break;
                }
            }

            if (j != shpNum) {
                sizeVtxClrBuffer += align32(clr.GetSize());
            }
        }
    }

    // clang-format off
    u32 ofsWorldMtxArray       = align32(sizeScnMdl);
    u32 ofsWorldMtxAttribArray = align32(ofsWorldMtxArray + sizeWorldMtxArray);
    u32 ofsViewPosMtxArray     = align32(ofsWorldMtxAttribArray + sizeWorldMtxAttribArray);
    u32 ofsViewNrmMtxArray     = align32(ofsViewPosMtxArray + sizeViewPosMtxArray);
    u32 ofsViewTexMtxArray     = align32(ofsViewNrmMtxArray + sizeViewNrmMtxArray);
    u32 ofsMatBufferDirtyFlag  = align4(ofsViewTexMtxArray + sizeViewTexMtxArray);
    u32 resTexObjOfs           = align32(ofsMatBufferDirtyFlag + matBufferDirtyFlagSize);
    u32 resTlutObjOfs          = align4(resTexObjOfs + resTexObjSize);
    u32 resTexSrtOfs           = align4(resTlutObjOfs + resTlutObjSize);
    u32 resChanOfs             = align4(resTexSrtOfs + resTexSrtSize);
    u32 resGenModeOfs          = align4(resChanOfs + resChanSize);
    u32 resMatMiscOfs          = align4(resGenModeOfs + resGenModeSize);
    u32 visOfs                 = align4(resMatMiscOfs + resMatMiscSize);
    u32 resPixOfs              = align32(visOfs + visSize);
    u32 resTevColorOfs         = align32(resPixOfs + resPixSize);
    u32 resIndMtxAndScaleOfs   = align32(resTevColorOfs + resTevColorSize);
    u32 resTexCoordGenOfs      = align32(resIndMtxAndScaleOfs + resIndMtxAndScaleSize);
    u32 resTevOfs              = align32(resTexCoordGenOfs + resTexCoordGenSize);
    // clang-format on

    u32 vtxPosTableOfs = align32(resTevOfs + resTevSize);
    u32 vtxNrmTableOfs = vtxPosTableOfs + vtxPosTableSize;
    u32 vtxClrTableOfs = vtxNrmTableOfs + vtxNrmTableSize;

    u32 vtxPosBufferOfs = align32(vtxClrTableOfs + vtxClrTableSize);
    u32 vtxNrmBufferOfs = align32(vtxPosBufferOfs + vtxPosBufferSize);
    u32 ofsVtxClrBuffer = align32(vtxNrmBufferOfs + vtxNrmBufferSize);

    u32 size = align32(ofsVtxClrBuffer + sizeVtxClrBuffer);
    if (pSize != NULL) {
        NW4R_POINTER_ASSERT(pSize, 802);
        *pSize = size;
    }

    if (pHeap != NULL) {
        u8* buf = reinterpret_cast<u8*>(Alloc(pHeap, size));
        if (buf == NULL) {
            return NULL;
        }

        // clang-format off
        DrawResMdlReplacement rep;

        rep.flag = NULL;
        if (!(bufferOption & ALWAYS_BUFFER_ANMSHP)) {
            rep.flag |= 1;
        }
        
        rep.visArray              = visSize               != 0 ? reinterpret_cast<u8*>(buf + visOfs)                                : NULL;
        rep.texObjDataArray       = resTexObjSize         != 0 ? reinterpret_cast<ResTexObjData*>(buf + resTexObjOfs)               : NULL;
        rep.tlutObjDataArray      = resTlutObjSize        != 0 ? reinterpret_cast<ResTlutObjData*>(buf + resTlutObjOfs)             : NULL;
        rep.texSrtDataArray       = resTexSrtSize         != 0 ? reinterpret_cast<ResTexSrtData*>(buf + resTexSrtOfs)               : NULL;
        rep.chanDataArray         = resChanSize           != 0 ? reinterpret_cast<ResChanData*>(buf + resChanOfs)                   : NULL;
        rep.genModeDataArray      = resGenModeSize        != 0 ? reinterpret_cast<ResGenModeData*>(buf + resGenModeOfs)             : NULL;
        rep.matMiscDataArray      = resMatMiscSize        != 0 ? reinterpret_cast<ResMatMiscData*>(buf + resMatMiscOfs)             : NULL;
        rep.pixDLArray            = resPixSize            != 0 ? reinterpret_cast<ResPixDL*>(buf + resPixOfs)                       : NULL;
        rep.tevColorDLArray       = resTevColorSize       != 0 ? reinterpret_cast<ResTevColorDL*>(buf + resTevColorOfs)             : NULL;
        rep.indMtxAndScaleDLArray = resIndMtxAndScaleSize != 0 ? reinterpret_cast<ResIndMtxAndScaleDL*>(buf + resIndMtxAndScaleOfs) : NULL;
        rep.texCoordGenDLArray    = resTexCoordGenSize    != 0 ? reinterpret_cast<ResTexCoordGenDL*>(buf + resTexCoordGenOfs)       : NULL;
        rep.tevDataArray          = resTevSize            != 0 ? reinterpret_cast<ResTevData*>(buf + resTevOfs)                     : NULL;

        rep.vtxPosTable = vtxPosTableSize != 0 ? reinterpret_cast<ResVtxPosData**>(buf + vtxPosTableOfs) : NULL;
        rep.vtxNrmTable = vtxNrmTableSize != 0 ? reinterpret_cast<ResVtxNrmData**>(buf + vtxNrmTableOfs) : NULL;
        rep.vtxClrTable = vtxClrTableSize != 0 ? reinterpret_cast<ResVtxClrData**>(buf + vtxClrTableOfs) : NULL;
        // clang-format on

        if (rep.vtxPosTable != NULL) {
            u32 vtxPosNum = mdl.GetResVtxPosNumEntries();
            u32 shpNum = mdl.GetResShpNumEntries();

            u32 ofs = vtxPosBufferOfs;

            for (u32 i = 0; i < vtxPosNum; i++) {
                u32 j;
                ResVtxPos pos = mdl.GetResVtxPos(i);

                for (j = 0; j < shpNum; j++) {
                    ResShp shp = mdl.GetResShp(j);

                    if (pos.ptr() == shp.GetResVtxPos().ptr()) {
                        break;
                    }
                }

                if (j != shpNum) {
                    rep.vtxPosTable[i] =
                        reinterpret_cast<ResVtxPosData*>(buf + ofs);

                    ofs += align32(pos.GetSize());
                    pos.CopyTo(rep.vtxPosTable[i]);
                } else {
                    rep.vtxPosTable[i] = pos.ptr();
                }
            }
        }

        if (rep.vtxNrmTable != NULL) {
            u32 vtxNrmNum = mdl.GetResVtxNrmNumEntries();
            u32 shpNum = mdl.GetResShpNumEntries();

            u32 ofs = vtxNrmBufferOfs;

            for (u32 i = 0; i < vtxNrmNum; i++) {
                u32 j;
                ResVtxNrm nrm = mdl.GetResVtxNrm(i);

                for (j = 0; j < shpNum; j++) {
                    ResShp shp = mdl.GetResShp(j);

                    if (nrm.ptr() == shp.GetResVtxNrm().ptr()) {
                        break;
                    }
                }

                if (j != shpNum) {
                    rep.vtxNrmTable[i] =
                        reinterpret_cast<ResVtxNrmData*>(buf + ofs);

                    ofs += align32(nrm.GetSize());
                    nrm.CopyTo(rep.vtxNrmTable[i]);
                } else {
                    rep.vtxNrmTable[i] = nrm.ptr();
                }
            }
        }

        if (rep.vtxClrTable != NULL) {
            u32 vtxClrNum = mdl.GetResVtxClrNumEntries();
            u32 shpNum = mdl.GetResShpNumEntries();

            u32 ofs = ofsVtxClrBuffer;

            for (u32 i = 0; i < vtxClrNum; i++) {
                u32 j;
                ResVtxClr clr = mdl.GetResVtxClr(i);

                for (j = 0; j < shpNum; j++) {
                    ResShp shp = mdl.GetResShp(j);

                    if (clr.ptr() == shp.GetResVtxClr(0).ptr() ||
                        clr.ptr() == shp.GetResVtxClr(1).ptr()) {
                        break;
                    }
                }

                if (j != shpNum) {
                    rep.vtxClrTable[i] =
                        reinterpret_cast<ResVtxClrData*>(buf + ofs);

                    ofs += align32(clr.GetSize());
                    clr.CopyTo(rep.vtxClrTable[i]);
                } else {
                    rep.vtxClrTable[i] = clr.ptr();
                }
            }
        }

        u32 replacementFlag = NULL;
        if (bufferOption & ALWAYS_BUFFER_ANMSHP) {
            replacementFlag |= 1;
        }

        // clang-format off
        pObj = new (buf) ScnMdl(
            pHeap,
            mdl,
            reinterpret_cast<math::MTX34*>(buf + ofsWorldMtxArray),
            reinterpret_cast<u32*>(buf + ofsWorldMtxAttribArray),
            reinterpret_cast<math::MTX34*>(buf + ofsViewPosMtxArray),
            sizeViewNrmMtxArray != 0 ? reinterpret_cast<math::MTX33*>(buf + ofsViewNrmMtxArray) : NULL,
            sizeViewTexMtxArray != 0 ? reinterpret_cast<math::MTX34*>(buf + ofsViewTexMtxArray) : NULL,
            nView,
            numViewMtx,
            &rep,
            reinterpret_cast<u32*>(buf + ofsMatBufferDirtyFlag), 
            replacementFlag);
        // clang-format off

        pObj->InitBuffer();
    }

    return pObj;
}

void ScnMdl::ScnMdl_G3DPROC_CALC_WORLD(u32 param, const math::MTX34* pParent) {
    ScnMdlSmpl_CalcPosture(param, pParent);

    if (IsVisBufferRefreshNeeded() && IsVisBufferDirty()) {
        CleanVisBuffer();
    }

    if (GetAnmObjVis() != NULL) {
        if (mReplacement.visArray != NULL) {
            ApplyVisAnmResult(mReplacement.visArray, GetResMdl(),
                              GetAnmObjVis());
            VisBufferDirty();
        } else {
            ApplyVisAnmResult(GetResMdl(), GetAnmObjVis());
        }
    }

    CheckCallback_CALC_WORLD(CALLBACK_TIMING_C, param,
                             const_cast<math::MTX34*>(pParent));
}

void ScnMdl::ScnMdl_G3DPROC_CALC_MAT(u32 param, void* pInfo) {
    CheckCallback_CALC_MAT(CALLBACK_TIMING_A, param, pInfo);

    ResMdl mdl = GetResMdl();
    u32 numMat = mdl.GetResMatNumEntries();

    for (u32 i = 0; i < numMat; i++) {
        ResMat mat = mdl.GetResMat(i);

        if (IsMatBufferDirty(i, BUFFER_RESTEXOBJ)) {
            CleanMatBuffer(i, BUFFER_RESTEXOBJ);
        }

        if (IsMatBufferDirty(i, BUFFER_RESTLUTOBJ)) {
            CleanMatBuffer(i, BUFFER_RESTLUTOBJ);
        }

        if (IsMatBufferDirty(i, BUFFER_RESMATINDMTXSCALE)) {
            CleanMatBuffer(i, BUFFER_RESMATINDMTXSCALE);
        }

        if (IsMatBufferDirty(i, BUFFER_RESTEXSRT)) {
            CleanMatBuffer(i, BUFFER_RESTEXSRT);
        }

        if (IsMatBufferDirty(i, BUFFER_RESCHAN)) {
            CleanMatBuffer(i, BUFFER_RESCHAN);
        }

        if (IsMatBufferDirty(i, BUFFER_RESTEVCOLOR)) {
            CleanMatBuffer(i, BUFFER_RESTEVCOLOR);
        }

        if (IsMatBufferDirty(i, BUFFER_RESGENMODE)) {
            CleanMatBuffer(i, BUFFER_RESGENMODE);
        }

        if (IsMatBufferDirty(i, BUFFER_RESMATMISC)) {
            CleanMatBuffer(i, BUFFER_RESMATMISC);
        }

        if (IsMatBufferDirty(i, BUFFER_RESMATPIX)) {
            CleanMatBuffer(i, BUFFER_RESMATPIX);
        }

        if (IsMatBufferDirty(i, BUFFER_RESMATTEXCOORDGEN)) {
            CleanMatBuffer(i, BUFFER_RESMATTEXCOORDGEN);
        }

        if (IsMatBufferDirty(i, BUFFER_RESTEV)) {
            CleanMatBuffer(i, BUFFER_RESTEV);
        }

        if (GetAnmObjTexPat() != NULL && GetAnmObjTexPat()->TestExistence(i)) {
            TexPatAnmResult resultBuf;

            const TexPatAnmResult* pResult =
                GetAnmObjTexPat()->GetResult(&resultBuf, i);

            ResTexObj texObj = mReplacement.texObjDataArray != NULL
                                   ? ResTexObj(&mReplacement.texObjDataArray[i])
                                   : mat.GetResTexObj();

            ResTlutObj tlutObj =
                mReplacement.tlutObjDataArray != NULL
                    ? ResTlutObj(&mReplacement.tlutObjDataArray[i])
                    : mat.GetResTlutObj();

            ApplyTexPatAnmResult(texObj, tlutObj, pResult);

            texObj.EndEdit();
            tlutObj.EndEdit();

            MatBufferDirty(i, BUFFER_RESTEXOBJ | BUFFER_RESTLUTOBJ);
        }

        if (GetAnmObjTexSrt() != NULL && GetAnmObjTexSrt()->TestExistence(i)) {
            TexSrtAnmResult resultBuf;

            const TexSrtAnmResult* pResult =
                GetAnmObjTexSrt()->GetResult(&resultBuf, i);

            ResTexSrt srt = mReplacement.texSrtDataArray != NULL
                                ? ResTexSrt(&mReplacement.texSrtDataArray[i])
                                : mat.GetResTexSrt();

            ResMatIndMtxAndScale ind =
                mReplacement.indMtxAndScaleDLArray != NULL
                    ? ResMatIndMtxAndScale(
                          &mReplacement.indMtxAndScaleDLArray[i])
                    : mat.GetResMatIndMtxAndScale();

            ApplyTexSrtAnmResult(srt, ind, pResult);

            ind.EndEdit();
            srt.EndEdit();

            MatBufferDirty(i, BUFFER_RESTEXSRT | BUFFER_RESMATINDMTXSCALE);
        }

        if (GetAnmObjMatClr() != NULL && GetAnmObjMatClr()->TestExistence(i)) {
            ClrAnmResult resultBuf;

            ResMatTevColor tevColor =
                mReplacement.tevColorDLArray != NULL
                    ? ResMatTevColor(&mReplacement.tevColorDLArray[i])
                    : mat.GetResMatTevColor();

            ResMatChan chan = mReplacement.chanDataArray != NULL
                                  ? ResMatChan(&mReplacement.chanDataArray[i])
                                  : mat.GetResMatChan();

            const ClrAnmResult* pResult =
                GetAnmObjMatClr()->GetResult(&resultBuf, i);

            ApplyClrAnmResult(chan, tevColor, pResult);

            chan.EndEdit();
            tevColor.EndEdit();

            MatBufferDirty(i, BUFFER_RESTEVCOLOR | BUFFER_RESCHAN);
        }
    }

    CheckCallback_CALC_MAT(CALLBACK_TIMING_C, param, pInfo);
}

void ScnMdl::ScnMdl_G3DPROC_DRAW_OPA(u32 param, void* pInfo) {
    CheckCallback_DRAW_OPA(CALLBACK_TIMING_A, param, pInfo);

    u32 drawMode = pInfo != NULL ? *static_cast<u32*>(pInfo) : GetDrawMode();

    DrawResMdlDirectly(GetResMdl(), GetViewPosMtxArray(), GetViewNrmMtxArray(),
                       GetViewTexMtxArray(), GetByteCodeDrawOpa(), NULL,
                       &mReplacement, drawMode);

    CheckCallback_DRAW_OPA(CALLBACK_TIMING_C, param, pInfo);
}

void ScnMdl::ScnMdl_G3DPROC_DRAW_XLU(u32 param, void* pInfo) {
    CheckCallback_DRAW_XLU(CALLBACK_TIMING_A, param, pInfo);

    u32 drawMode = pInfo != NULL ? *static_cast<u32*>(pInfo) : GetDrawMode();

    DrawResMdlDirectly(GetResMdl(), GetViewPosMtxArray(), GetViewNrmMtxArray(),
                       GetViewTexMtxArray(), NULL, GetByteCodeDrawXlu(),
                       &mReplacement, drawMode);

    CheckCallback_DRAW_XLU(CALLBACK_TIMING_C, param, pInfo);
}

void ScnMdl::ScnMdl_G3DPROC_CALC_VTX(u32 param, void* pInfo) {
#pragma unused(param)
#pragma unused(pInfo)

    if (GetAnmObjShp() == NULL) {
        return;
    }

    CalcVtx(GetResMdl(), GetAnmObjShp(), mReplacement.vtxPosTable,
            mReplacement.vtxNrmTable, mReplacement.vtxClrTable);
}

void ScnMdl::G3dProc(u32 task, u32 param, void* pInfo) {
    if (IsG3dProcDisabled(task)) {
        return;
    }

    switch (task) {
    case G3DPROC_GATHER_SCNOBJ: {
        ScnMdlSmpl_G3DPROC_GATHER_SCNOBJ(param,
                                         static_cast<IScnObjGather*>(pInfo));
        break;
    }

    case G3DPROC_CALC_WORLD: {
        ScnMdl_G3DPROC_CALC_WORLD(param, static_cast<math::MTX34*>(pInfo));
        break;
    }

    case G3DPROC_CALC_MAT: {
        ScnMdl_G3DPROC_CALC_MAT(param, pInfo);
        break;
    }

    case G3DPROC_CALC_VIEW: {
        ScnMdlSmpl_G3DPROC_CALC_VIEW(param, static_cast<math::MTX34*>(pInfo));
        break;
    }

    case G3DPROC_DRAW_OPA: {
        ScnMdl_G3DPROC_DRAW_OPA(param, pInfo);
        break;
    }

    case G3DPROC_DRAW_XLU: {
        ScnMdl_G3DPROC_DRAW_XLU(param, pInfo);
        break;
    }

    case G3DPROC_UPDATEFRAME: {
        ScnMdlSmpl_G3DPROC_UPDATEFRAME(param, pInfo);

        if (mpAnmObjShp != NULL) {
            mpAnmObjShp->UpdateFrame();
        }
        break;
    }

    case G3DPROC_CHILD_DETACHED: {
        RemoveAnmObj(static_cast<AnmObj*>(pInfo));
        break;
    }

    case G3DPROC_CALC_VTX: {
        ScnMdl_G3DPROC_CALC_VTX(param, pInfo);
        break;
    }

    default: {
        DefG3dProcScnLeaf(task, param, pInfo);
        break;
    }
    }
}

bool ScnMdl::SetScnObjOption(u32 option, u32 value) {
    switch (option) {
    case OPTID_VISBUFFER_REFRESH_NEEDED: {
        if (value) {
            mFlagVisBuffer &= ~VISBUFFER_NOT_REFRESH_NEEDED;
        } else {
            mFlagVisBuffer |= VISBUFFER_NOT_REFRESH_NEEDED;
        }
        break;
    }

    default: {
        return ScnMdlSimple::SetScnObjOption(option, value);
    }
    }

    return true;
}

bool ScnMdl::GetScnObjOption(u32 option, u32* pValue) const {
    if (pValue == NULL) {
        return false;
    }

    switch (option) {
    case OPTID_VISBUFFER_REFRESH_NEEDED: {
        *pValue = !(mFlagVisBuffer & VISBUFFER_NOT_REFRESH_NEEDED);
        break;
    }

    default: {
        return ScnMdlSimple::GetScnObjOption(option, pValue);
    }
    }

    return true;
}

void ScnMdl::InitBuffer() {
    ResMdl mdl = GetResMdl();

    u32 numMat = mdl.GetResMatNumEntries();
    u32 numNode = mdl.GetResNodeNumEntries();

    u32 i;

    if (mReplacement.visArray != NULL) {
        for (i = 0; i < numNode; i++) {
            if (mdl.GetResNode(i).IsVisible()) {
                mReplacement.visArray[i] = true;
            } else {
                mReplacement.visArray[i] = false;
            }
        }
    }

    for (i = 0; i < numMat; i++) {
        ResMat mat = mdl.GetResMat(i);
        mpMatBufferDirtyFlag[i] = 0;

        if (mReplacement.texObjDataArray != NULL) {
            mat.GetResTexObj()
                .CopyTo(&mReplacement.texObjDataArray[i])
                .EndEdit();
        }

        if (mReplacement.tlutObjDataArray != NULL) {
            mat.GetResTlutObj()
                .CopyTo(&mReplacement.tlutObjDataArray[i])
                .EndEdit();
        }

        if (mReplacement.texSrtDataArray != NULL) {
            mat.GetResTexSrt()
                .CopyTo(&mReplacement.texSrtDataArray[i])
                .EndEdit();
        }

        if (mReplacement.chanDataArray != NULL) {
            mat.GetResMatChan()
                .CopyTo(&mReplacement.chanDataArray[i])
                .EndEdit();
        }

        if (mReplacement.genModeDataArray != NULL) {
            mat.GetResGenMode()
                .CopyTo(&mReplacement.genModeDataArray[i])
                .EndEdit();
        }

        if (mReplacement.matMiscDataArray != NULL) {
            mat.GetResMatMisc()
                .CopyTo(&mReplacement.matMiscDataArray[i])
                .EndEdit();
        }

        if (mReplacement.pixDLArray != NULL) {
            mat.GetResMatPix().CopyTo(&mReplacement.pixDLArray[i]).EndEdit();
        }

        if (mReplacement.tevColorDLArray != NULL) {
            mat.GetResMatTevColor()
                .CopyTo(&mReplacement.tevColorDLArray[i])
                .EndEdit();
        }

        if (mReplacement.indMtxAndScaleDLArray != NULL) {
            mat.GetResMatIndMtxAndScale()
                .CopyTo(&mReplacement.indMtxAndScaleDLArray[i])
                .EndEdit();
        }

        if (mReplacement.texCoordGenDLArray != NULL) {
            mat.GetResMatTexCoordGen()
                .CopyTo(&mReplacement.texCoordGenDLArray[i])
                .EndEdit();
        }

        if (mReplacement.tevDataArray != NULL) {
            mat.GetResTev().CopyTo(&mReplacement.tevDataArray[i]).EndEdit();
        }
    }
}

void ScnMdl::CleanMatBuffer(u32 idx, u32 option) {
    ResMat mat = GetResMdl().GetResMat(idx);

    if ((option & BUFFER_RESTEXOBJ) && mReplacement.texObjDataArray != NULL) {
        mat.GetResTexObj().CopyTo(&mReplacement.texObjDataArray[idx]).EndEdit();
    }

    if ((option & BUFFER_RESTLUTOBJ) && mReplacement.tlutObjDataArray != NULL) {
        mat.GetResTlutObj().CopyTo(&mReplacement.tlutObjDataArray[idx]).EndEdit();
    }

    if ((option & BUFFER_RESTEXSRT) && mReplacement.texSrtDataArray != NULL) {
        mat.GetResTexSrt().CopyTo(&mReplacement.texSrtDataArray[idx]).EndEdit();
    }

    if ((option & BUFFER_RESCHAN) && mReplacement.chanDataArray != NULL) {
        mat.GetResMatChan().CopyTo(&mReplacement.chanDataArray[idx]).EndEdit();
    }

    if ((option & BUFFER_RESGENMODE) && mReplacement.genModeDataArray != NULL) {
        mat.GetResGenMode().CopyTo(&mReplacement.genModeDataArray[idx]).EndEdit();
    }

    if ((option & BUFFER_RESMATMISC) && mReplacement.matMiscDataArray != NULL) {
        mat.GetResMatMisc().CopyTo(&mReplacement.matMiscDataArray[idx]).EndEdit();
    }

    if ((option & BUFFER_RESMATPIX) && mReplacement.pixDLArray != NULL) {
        mat.GetResMatPix().CopyTo(&mReplacement.pixDLArray[idx]).EndEdit();
    }

    if ((option & BUFFER_RESTEVCOLOR) &&
        mReplacement.tevColorDLArray != NULL) {

        mat.GetResMatTevColor()
            .CopyTo(&mReplacement.tevColorDLArray[idx])
            .EndEdit();
    }

    if ((option & BUFFER_RESMATINDMTXSCALE) &&
        mReplacement.indMtxAndScaleDLArray != NULL) {

        mat.GetResMatIndMtxAndScale()
            .CopyTo(&mReplacement.indMtxAndScaleDLArray[idx])
            .EndEdit();
    }

    if ((option & BUFFER_RESMATTEXCOORDGEN) &&
        mReplacement.texCoordGenDLArray != NULL) {

        mat.GetResMatTexCoordGen()
            .CopyTo(&mReplacement.texCoordGenDLArray[idx])
            .EndEdit();
    }

    if ((option & BUFFER_RESTEV) && mReplacement.tevDataArray != NULL) {
        mat.GetResTev().CopyTo(&mReplacement.tevDataArray[idx]).EndEdit();
    }

    mpMatBufferDirtyFlag[idx] = mpMatBufferDirtyFlag[idx] & ~option;
}

void ScnMdl::CleanVisBuffer() {
    ResMdl mdl = GetResMdl();
    u32 numNode = mdl.GetResNodeNumEntries();

    if (mReplacement.visArray != NULL) {
        for (u32 i = 0; i < numNode; i++) {
            if (mdl.GetResNode(i).IsVisible()) {
                mReplacement.visArray[i] = true;
            } else {
                mReplacement.visArray[i] = false;
            }
        }
    }

    mFlagVisBuffer &= ~VISBUFFER_DIRTY;
}

bool ScnMdl::SetAnmObj(AnmObj* p, AnmObjType tp) {
    if (p != NULL && p->GetParent() == NULL) {
        if (tp == ANMOBJTYPE_SHP || tp == ANMOBJTYPE_NOT_SPECIFIED) {
            AnmObjShp* pShp = DynamicCast<AnmObjShp>(p);

            if (pShp != NULL) {
                if (!pShp->IsBound()) {
                    return FALSE;
                }

                if (mpAnmObjShp != NULL) {
                    RemoveAnmObj(mpAnmObjShp);
                }

                mpAnmObjShp = pShp;
                pShp->G3dProc(G3DPROC_ATTACH_PARENT, 0, this);
                mReplacement.flag &= ~1;

                return TRUE;
            } else {
                if (tp == ANMOBJTYPE_NOT_SPECIFIED) {
                    return ScnMdlSimple::SetAnmObj(p, tp);
                }

                return FALSE;
            }
        } else {
            return ScnMdlSimple::SetAnmObj(p, tp);
        }
    }

    return FALSE;
}

bool ScnMdl::RemoveAnmObj(AnmObj* p) {
    if (p == NULL) {
        return NULL;
    }

    if (p == mpAnmObjShp) {
        mpAnmObjShp->G3dProc(G3DPROC_DETACH_PARENT, 0, this);
        mpAnmObjShp = NULL;

        if (!(mReplacementFlag & 1)) {
            mReplacement.flag |= 1;
            return TRUE;
        }

        if (mReplacement.vtxPosTable != NULL) {
            u32 vtxPosNum = GetResMdl().GetResVtxPosNumEntries();

            for (u32 i = 0; i < vtxPosNum; i++) {
                ResVtxPos pos = GetResMdl().GetResVtxPos(i);

                if (pos.ptr() != mReplacement.vtxPosTable[i]) {
                    pos.CopyTo(mReplacement.vtxPosTable[i]);
                }
            }
        }

        if (mReplacement.vtxNrmTable != NULL) {
            u32 vtxNrmNum = GetResMdl().GetResVtxNrmNumEntries();

            for (u32 i = 0; i < vtxNrmNum; i++) {
                ResVtxNrm nrm = GetResMdl().GetResVtxNrm(i);

                if (nrm.ptr() != mReplacement.vtxNrmTable[i]) {
                    nrm.CopyTo(mReplacement.vtxNrmTable[i]);
                }
            }
        }

        if (mReplacement.vtxClrTable != NULL) {
            u32 vtxClrNum = GetResMdl().GetResVtxClrNumEntries();

            for (u32 i = 0; i < vtxClrNum; i++) {
                ResVtxClr clr = GetResMdl().GetResVtxClr(i);

                if (clr.ptr() != mReplacement.vtxClrTable[i]) {
                    clr.CopyTo(mReplacement.vtxClrTable[i]);
                }
            }
        }

        return true;
    }

    return ScnMdlSimple::RemoveAnmObj(p);
}

AnmObj* ScnMdl::RemoveAnmObj(AnmObjType type) {
    if (type == ANMOBJTYPE_SHP) {
        AnmObj* pOld = mpAnmObjShp;
        RemoveAnmObj(mpAnmObjShp);
        return pOld;
    }

    return ScnMdlSimple::RemoveAnmObj(type);
}

AnmObj* ScnMdl::GetAnmObj(AnmObjType type) {
    if (type == ANMOBJTYPE_SHP) {
        return mpAnmObjShp;
    }

    return ScnMdlSimple::GetAnmObj(type);
}

const AnmObj* ScnMdl::GetAnmObj(AnmObjType type) const {
    if (type == ANMOBJTYPE_SHP) {
        return mpAnmObjShp;
    }

    return ScnMdlSimple::GetAnmObj(type);
}

ScnMdl::ScnMdl(MEMAllocator* pHeap, ResMdl mdl,
               math::MTX34* pWorldMtxArray, u32* pWorldMtxAttribArray,
               math::MTX34* pViewPosMtxArray, math::MTX33* pViewNrmMtxArray,
               math::MTX34* pViewTexMtxArray, int nView, int numViewMtx,
               DrawResMdlReplacement* pReplacement, u32* pMatBufferDirtyFlag, u32 replacementFlag)
    : ScnMdlSimple(pHeap, mdl, pWorldMtxArray, pWorldMtxAttribArray,
                   pViewPosMtxArray, pViewNrmMtxArray, pViewTexMtxArray,
                   nView, numViewMtx),
      mpAnmObjShp(NULL),
      mFlagVisBuffer(NULL),
      mpMatBufferDirtyFlag(pMatBufferDirtyFlag),
      mReplacement(*pReplacement), 
      mReplacementFlag(replacementFlag) {}

ScnMdl::~ScnMdl() {
    if (mpAnmObjShp != NULL) {
        RemoveAnmObj(mpAnmObjShp);
    }
}

} // namespace g3d
} // namespace nw4r
