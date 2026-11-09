#include <nw4r/ef.h>
#include <nw4r/math.h>
#include <nw4r/ut.h>

#include <cstring>

namespace nw4r {
namespace ef {

math::MTX34 ParticleManager::smDrawMtxPMtoEM;

s32 ParticleManager::smMtxInvId = 0;

ParticleManager::ParticleManager()
    : mActivityList(offsetof(Particle, mActivityLink)) {}

void ParticleManager::SendClosing() {
    mManagerEM->Closing(this);
}

void ParticleManager::DestroyFunc() {}

bool ParticleManager::Closing(Particle* pParticle) {
    pParticle->mParticleManager->UnRef();

    mActivityList.ToClosing(pParticle);
    pParticle->mLifeStatus = NW4R_EF_LS_CLOSING;

    return true;
}

int ParticleManager::RetireParticle(Particle* pParticle) {
    if (pParticle->mLifeStatus != NW4R_EF_LS_ACTIVE) {
        return 0;
    }

    mActivityList.ToWait(pParticle);
    pParticle->Destroy();
    return 1;
}

int ParticleManager::RetireParticleAll() {
    int num = 0;

    Particle* pp;
    Particle* pNext;

    for (pp = static_cast<Particle*>(mActivityList.mActiveList.headObject);
         pp != NULL; pp = pNext) {

        pNext = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pp)->nextObject);

        if (pp->mLifeStatus == NW4R_EF_LS_ACTIVE) {
            num += RetireParticle(pp);
        }
    }

    return num;
}

bool ParticleManager::Initialize(Emitter* parent, EmitterResource* resource) {

    NW4R_POINTER_ASSERT(parent, 105);
    NW4R_POINTER_ASSERT(resource, 106);

    ReferencedObject::Initialize();
    mActivityList.Initialize();

    mModifier.mScale.x = 1.0f;
    mModifier.mScale.y = 1.0f;

    mModifier.mRotate.x = 0.0f;
    mModifier.mRotate.y = 0.0f;
    mModifier.mRotate.z = 0.0f;

    mManagerEM = parent;
    parent->Ref();

    mResource = resource;
    mFlag = 0;
    SetMtxDirty();

    EmitterDesc* pDesc = mResource->GetEmitterDesc();

    NW4R_POINTER_ASSERT(mManagerEM, 128);
    NW4R_POINTER_ASSERT(mManagerEM->mManagerEF, 129);
    NW4R_POINTER_ASSERT(mManagerEM->mManagerEF->mManagerES, 130);
    NW4R_POINTER_ASSERT(
        mManagerEM->mManagerEF->mManagerES->mDrawStrategyBuilder, 131);

    mDrawStrategy =
        mManagerEM->mManagerEF->mManagerES->mDrawStrategyBuilder->Create(
            pDesc->drawSetting.ptcltype);

    mLastCalced = NULL;

    Modifier_SetSimpleLightParameter(pDesc->drawSetting);

    return true;
}

ParticleManager::~ParticleManager() {}

Particle*
ParticleManager::CreateParticle(u16 life, math::VEC3 pos, math::VEC3 vel,
                                const math::MTX34* pSpace, f32 momentum,
                                const EmitterInheritSetting* pSetting,
                                Particle* pReferencePtcl, u16 calcRemain) {

    Particle* pParticle =
        mManagerEM->mManagerEF->mManagerES->GetMemoryManager()->AllocParticle();

    if (pParticle == NULL) {
        return NULL;
    }

    if (!pParticle->Initialize(life, pos, vel, this, pSpace, momentum, pSetting,
                               pReferencePtcl)) {
        return NULL;
    }

    pParticle->mCalcRemain += calcRemain;
    mActivityList.ToActive(pParticle);
    pParticle->mLifeStatus = NW4R_EF_LS_ACTIVE;

    return pParticle;
}

static math::VEC3
TransformParticlePosition(bool isLocal, const math::MTX34* mtxLocalToEmitter,
                          const math::MTX34* mtxLocalToGlobal,
                          const math::MTX34* mtxEmitterToGlobal,
                          const math::VEC3* prevPosition) {
    NW4R_POINTER_ASSERT(mtxLocalToEmitter, 186);
    NW4R_POINTER_ASSERT(mtxLocalToGlobal, 187);
    NW4R_POINTER_ASSERT(mtxEmitterToGlobal, 188);

    nw4r::math::VEC3 ptclPos;

    if (isLocal) {
        math::VEC3Transform(&ptclPos, mtxLocalToEmitter, prevPosition);
    } else {
        math::VEC3Transform(&ptclPos, mtxLocalToGlobal, prevPosition);
        math::VEC3 emitPos(0, 0, 0);
        math::VEC3Transform(&emitPos, mtxEmitterToGlobal, &emitPos);
        math::VEC3Sub(&ptclPos, &ptclPos, &emitPos);
    }

    return ptclPos;
}

void ParticleManager::Calc() {
    Particle* firstPP = static_cast<Particle*>(
        ut::List_GetNext(&mActivityList.mActiveList, mLastCalced));

    if (firstPP == NULL) {
        return;
    }

    Particle* pp = firstPP;

    NW4R_POINTER_ASSERT(mManagerEM, 217);
    NW4R_POINTER_ASSERT(mManagerEM->mManagerEF, 218);

    if (mManagerEM->mManagerEF->mCallBack.mPrevPtclCalc) {
        mManagerEM->mManagerEF->mCallBack.mPrevPtclCalc(
            this, &mActivityList.mActiveList, firstPP);
    }

    math::MTX34 mtxLocalToGlobal;
    CalcGlobalMtx(&mtxLocalToGlobal);

    math::MTX34 mtxGlobalToLocal;
    math::MTX34Inv(&mtxGlobalToLocal, &mtxLocalToGlobal);

    math::MTX34 mtxEmitterToGlobal;
    mManagerEM->CalcGlobalMtx(&mtxEmitterToGlobal);

    f32 emitterScale = 1.0f;
    {
        math::VEC3 v(mtxEmitterToGlobal._00 + mtxEmitterToGlobal._01 +
                         mtxEmitterToGlobal._02,
                     mtxEmitterToGlobal._10 + mtxEmitterToGlobal._11 +
                         mtxEmitterToGlobal._12,
                     mtxEmitterToGlobal._20 + mtxEmitterToGlobal._21 +
                         mtxEmitterToGlobal._22);
        emitterScale = math::VEC3Len(&v) * (NW4R_MATH_INVSQRT3);
    }

    math::MTX34 mtxEmitterToLocal;
    math::MTX34Mult(&mtxEmitterToLocal, &mtxGlobalToLocal, &mtxEmitterToGlobal);

    math::MTX34 mtxLocalToEmitter;
    math::MTX34Inv(&mtxLocalToEmitter, &mtxEmitterToLocal);

    math::MTX34 mtxGlobalToLocalNoTrans = mtxGlobalToLocal;
    mtxGlobalToLocalNoTrans._03 = 0.0f;
    mtxGlobalToLocalNoTrans._13 = 0.0f;
    mtxGlobalToLocalNoTrans._23 = 0.0f;

    math::MTX34 mtxLocalToGlobalNoTrans;
    math::MTX34Inv(&mtxLocalToGlobalNoTrans, &mtxGlobalToLocalNoTrans);

    math::MTX34 mtxLocalToEmitterNoTrans = mtxLocalToEmitter;
    mtxLocalToEmitterNoTrans._03 = 0.0f;
    mtxLocalToEmitterNoTrans._13 = 0.0f;
    mtxLocalToEmitterNoTrans._23 = 0.0f;

    math::MTX34 mtxEmitterToLocalNoTrans = mtxEmitterToLocal;
    mtxEmitterToLocalNoTrans._03 = 0.0f;
    mtxEmitterToLocalNoTrans._13 = 0.0f;
    mtxEmitterToLocalNoTrans._23 = 0.0f;

    Particle* pnext;

    for (; pp != NULL; pp = pnext) {
        pnext = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pp)->nextObject);

        if (pp->mLifeStatus != NW4R_EF_LS_ACTIVE ||
            pp->mEvalStatus != NW4R_EF_ES_WAIT) {
            continue;
        }

        pp->mEvalStatus = NW4R_EF_ES_DONE;

        if (pp->mCalcRemain != 0) {
            mManagerEM->mManagerEF->SetFlagExistCalcRemain(TRUE);
        }

        math::VEC3 prevPosition = pp->mParameter.mPosition;
        math::VEC3 prevVelocity = pp->mParameter.mVelocity;

        math::VEC3 prevDir;
        pp->GetMoveDir(&prevDir);

        pp->mParameter.mPrevPosition = pp->mParameter.mPosition;

        if (pp->mLife <= pp->mTick) {
            RetireParticle(pp);
            continue;
        }

        math::VEC3 addVelocity(0.0f, 0.0f, 0.0f);
        math::VEC3 addPosition(0.0f, 0.0f, 0.0f);
        u16 i;

        bool postfieldEnable = FALSE;
        PostFieldInfo::AnimatableParams postfieldAnimParams;
        PostFieldInfo* postfieldResource;
        EmitterResource* postfieldChildRes = NULL;

        bool findEmitterTiming = FALSE;

        for (i = pp->mTick == 0 ? 0 : mResource->NumPtclInitTrack();
             i < mResource->NumPtclTrack(); i++) {

            u8* ptr = mResource->GetPtclTrack(i);

#define pTrackAsHeader reinterpret_cast<AnimCurveHeader*>(ptr)

            if (pTrackAsHeader->processFlag & AC_PROC_FLAG_STOP) {
                continue;
            }

            u32 tick;
            u32 life;
            u16 randSeed;

            if (pTrackAsHeader->processFlag & AC_PROC_FLAG_TIMING) {

                findEmitterTiming = true;

                tick = pp->mParticleManager->mManagerEM->mTick;

                if (pp->mParticleManager->mManagerEM->mParameter.mComFlags &
                    EmitterDesc::CMN_FLAG_MAX_LIFE) {
                    life = 0xFFFFFFFF;
                } else {
                    life =
                        pp->mParticleManager->mManagerEM->mParameter.mEmitSpan;
                }

                randSeed = pp->mParticleManager->mManagerEM->mRandSeed;
            } else {
                tick = pp->mTick;
                life = pp->mLife;
                randSeed = pp->mRandSeed;
            }

            if (pTrackAsHeader->magic == NW4R_EF_MAGIC_ANIMCURVE ||
                pTrackAsHeader->magic == NW4R_EF_MAGIC_ANIMBAKED) {
                const u8 kind = pTrackAsHeader->kindType;
                const u8 curveType = pTrackAsHeader->curveFlag;

                switch (curveType) {
                case AC_TYPE_PARTICLE_TEXTURE: {
                    NW4R_ASSERT(*ptr == 0xac, 365);
                    AnimCurveExecuteTexture(ptr, pp, tick, randSeed, life);
                } break;

                case AC_TYPE_CHILD: {
                    NW4R_ASSERT(*ptr == 0xac, 369);
                    AnimCurveExecuteChild(ptr, pp, tick, randSeed, life);
                } break;

                case AC_TYPE_PARTICLE_ROTATE: {
                    f32* target = &pp->mParameter.mRotate.x;
                    AnimCurveExecuteRotate(ptr, target, tick, randSeed, life);
                } break;

                case AC_TYPE_PARTICLE_U8: {
                    u8* targetU8 =
                        reinterpret_cast<u8*>(&pp->mParameter) + kind;
                    AnimCurveExecuteU8(ptr, targetU8, tick, randSeed, life);
                } break;

                case AC_TYPE_PARTICLE_F32: {
                    f32* targetF32 = reinterpret_cast<f32*>(
                        reinterpret_cast<u8*>(&pp->mParameter) + kind);
                    AnimCurveExecuteF32(ptr, targetF32, tick, randSeed, life);
                } break;

                case AC_TYPE_FIELD: {
                    u8* infoTable =
                        ptr + sizeof(AnimCurveHeader) +
                        pTrackAsHeader->keyTable + pTrackAsHeader->rangeTable +
                        pTrackAsHeader->randomTable + pTrackAsHeader->nameTable;
                    InfoField infoCopy;
                    memcpy(&infoCopy, infoTable, sizeof(InfoField));
                    if (pTrackAsHeader->keyTable > 0) {
                        AnimCurveExecuteF32(ptr, &infoCopy.u.animParam, tick,
                                            randSeed, life);
                    }

                    math::VEC3 ptclPos;
                    if (kind == AC_TARGET_FIELD_SPIN ||
                        kind == AC_TARGET_FIELD_MAGNET ||
                        AC_TARGET_FIELD_NEWTON || AC_TARGET_FIELD_VORTEX) {
                        ptclPos = TransformParticlePosition(
                            (infoCopy.space == InfoField::SPACE_EMITTER),
                            &mtxLocalToEmitter, &mtxLocalToGlobal,
                            &mtxEmitterToGlobal, &prevPosition);
                    }

                    math::VEC3 affect(0, 0, 0);
                    switch (kind) {
                    case AC_TARGET_FIELD_SPEED:
                        CalcFieldSpeed(affect, infoCopy, prevVelocity);
                        break;
                    case AC_TARGET_FIELD_GRAVITY:
                        CalcFieldGravity(affect, infoCopy);
                        break;
                    case AC_TARGET_FIELD_RANDOM:
                        CalcFieldRandom(affect, infoCopy, pTrackAsHeader, ptr,
                                        pp, tick, randSeed, prevVelocity,
                                        prevDir, mtxLocalToEmitterNoTrans);
                        break;
                    case AC_TARGET_FIELD_SPIN:
                        CalcFieldSpin(affect, infoCopy, ptclPos);
                        break;
                    case AC_TARGET_FIELD_MAGNET:
                        CalcFieldMagnet(affect, infoCopy, ptclPos);
                        break;
                    case AC_TARGET_FIELD_NEWTON:
                        CalcFieldNewton(affect, infoCopy, ptclPos);
                        break;
                    case AC_TARGET_FIELD_VORTEX:
                        CalcFieldVortex(affect, infoCopy, ptclPos);
                        break;
                    case AC_TARGET_FIELD_TAIL:
                        if (!isnan(mManagerEM->mPrevGlobalPosition.x)) {
                            math::MTX34 mtx;
                            mManagerEM->CalcGlobalMtx(&mtx);
                            math::VEC3 emGlobalPos(mtx._03, mtx._13, mtx._23);

                            math::VEC3Sub(&affect, &emGlobalPos,
                                          &mManagerEM->mPrevGlobalPosition);
                            affect *= infoCopy.u.tail.power;
                        }
                        break;
                    }

                    switch (infoCopy.space) {
                    case InfoField::SPACE_EMITTER:
                        math::VEC3Transform(&affect, &mtxEmitterToLocalNoTrans,
                                            &affect);
                        break;
                    case InfoField::SPACE_GLOBAL:
                        math::VEC3Transform(&affect, &mtxGlobalToLocalNoTrans,
                                            &affect);
                        break;
                    case InfoField::SPACE_GLOBAL_WITH_SCALE:
                        affect *= emitterScale;
                        math::VEC3Transform(&affect, &mtxGlobalToLocalNoTrans,
                                            &affect);
                        break;
                    }

                    switch (infoCopy.addTarget) {
                    case InfoField::AFFECT_VELOCITY:
                        addVelocity += affect;
                        break;
                    case InfoField::AFFECT_SPEED:
                        addPosition += affect;
                        break;
                    }

                } break;

                case AC_TYPE_POSTFIELD: {
                    if (pTrackAsHeader->infoTable > 0) {
                        AnimCurveNameTable* nameTable =
                            reinterpret_cast<AnimCurveNameTable*>(
                                ptr + sizeof(AnimCurveHeader) +
                                pTrackAsHeader->keyTable +
                                pTrackAsHeader->rangeTable +
                                pTrackAsHeader->randomTable);
                        postfieldResource = reinterpret_cast<PostFieldInfo*>(
                            reinterpret_cast<u8*>(nameTable) +
                            pTrackAsHeader->nameTable);

                        memcpy(&postfieldAnimParams, postfieldResource,
                               sizeof(PostFieldInfo::AnimatableParams));

                        if (pTrackAsHeader->nameTable > 4) {
                            // clang-format off
                        NW4R_ASSERT(postfieldResource->mChildOption.mNameIdx < *reinterpret_cast<u16*>(nameTable), 511);
                        postfieldChildRes = reinterpret_cast<EmitterResource*>(
                            nameTable->datas[postfieldResource->mChildOption.mNameIdx].work);
                            // clang-format on
                        }
                    }

                    if (pTrackAsHeader->keyTable > 0) {
                        f32* targetF32 = reinterpret_cast<f32*>(
                            reinterpret_cast<u8*>(&postfieldAnimParams) + kind);
                        AnimCurveExecuteF32(ptr, targetF32, tick, randSeed,
                                            life);
                    }

                    postfieldEnable = TRUE;
                } break;
                }
            }

#undef pTrackAsHeader
        }

        if (postfieldEnable) {
            math::VEC3 nextVelocity(pp->mParameter.mVelocity);
            nextVelocity += addVelocity;

            bool requestKilling = FALSE;
            bool normalMove = calcPostField(
                pp, postfieldAnimParams, postfieldResource, postfieldChildRes,
                mtxEmitterToGlobal, mtxGlobalToLocal, prevPosition, addPosition,
                &nextVelocity, &requestKilling);

            if (requestKilling) {
                RetireParticle(pp);
                continue;
            }

            if (normalMove) {
                pp->mParameter.mVelocity = nextVelocity;
                pp->AddPosition(&addPosition);
                pp->AddPosition(&pp->mParameter.mVelocity);
            }

            if (postfieldResource->mWrapOption & PostFieldInfo::WRAP_ENABLE) {
                math::MTX34 mtxFormToLocal;
                {
                    math::MTX34Identity(&mtxFormToLocal);
                    MTX34ScaleNonZero(&mtxFormToLocal,
                                      &postfieldResource->mWrapScale,
                                      &mtxFormToLocal);

                    math::MTX34 rot;
                    PSMTX34RotXYZRad(&rot, postfieldResource->mWrapRotate.x,
                                     postfieldResource->mWrapRotate.y,
                                     postfieldResource->mWrapRotate.z);
                    math::MTX34Mult(&mtxFormToLocal, &rot, &mtxFormToLocal);
                    math::MTX34Trans(&mtxFormToLocal,
                                     &postfieldResource->mWrapTranslate,
                                     &mtxFormToLocal);

                    if (postfieldResource->mWrapOption &
                        PostFieldInfo::WRAP_CENTER_EMITTER) {
                        math::VEC3 vec(mtxEmitterToGlobal._03,
                                       mtxEmitterToGlobal._13,
                                       mtxEmitterToGlobal._23);
                        math::MTX34Trans(&mtxFormToLocal, &vec,
                                         &mtxFormToLocal);
                    }

                    math::MTX34Mult(&mtxFormToLocal, &mtxGlobalToLocal,
                                    &mtxFormToLocal);
                }

                math::MTX34 mtxLocalToForm;
                math::MTX34Inv(&mtxLocalToForm, &mtxFormToLocal);

                math::VEC3 posInForm;
                math::VEC3Transform(&posInForm, &mtxLocalToForm,
                                    &pp->mParameter.mPosition);

                bool wrap = FALSE;

                if (posInForm.x > 1) {
                    posInForm.x = std::fmodf(posInForm.x + 1, 2) - 1;
                    wrap = TRUE;
                } else if (posInForm.x < -1) {
                    posInForm.x = std::fmodf(posInForm.x - 1, 2) + 1;
                    wrap = TRUE;
                }

                if (posInForm.y > 1) {
                    posInForm.y = std::fmodf(posInForm.y + 1, 2) - 1;
                    wrap = TRUE;
                } else if (posInForm.y < -1) {
                    posInForm.y = std::fmodf(posInForm.y - 1, 2) + 1;
                    wrap = TRUE;
                }

                if (posInForm.z > 1) {
                    posInForm.z = std::fmodf(posInForm.z + 1, 2) - 1;
                    wrap = TRUE;
                } else if (posInForm.z < -1) {
                    posInForm.z = std::fmodf(posInForm.z - 1, 2) + 1;
                    wrap = TRUE;
                }

                if (wrap) {
                    math::VEC3Transform(&pp->mParameter.mPosition,
                                        &mtxFormToLocal, &posInForm);
                }
            }
        } else {
            pp->mParameter.mVelocity += addVelocity;
            pp->AddPosition(&addPosition);
            pp->AddPosition(&pp->mParameter.mVelocity);
        }

        pp->mTick++;
    }

    mLastCalced =
        static_cast<Particle*>(ut::List_GetLast(&mActivityList.mActiveList));

    if (mManagerEM->mManagerEF->mCallBack.mPostPtclCalc != NULL) {
        mManagerEM->mManagerEF->mCallBack.mPostPtclCalc(
            this, &mActivityList.mActiveList, firstPP);
    }
}

void ParticleManager::Draw(const DrawInfo& drawInfo) {
    const EmitterDesc* ed = mResource->GetEmitterDesc();

    if ((ed->drawSetting.mFlags & EmitterDrawSetting::FLAG_HIDDEN) ||
        (mManagerEM->mParameter.mComFlags &
         EmitterDesc::CMN_FLAG_DISABLE_DRAW)) {
        return;
    }

    DrawInfo drawInfoWithZ(drawInfo);
    f32 zOffset = mResource->GetEmitterDrawSetting()->zOffset;
    if (zOffset != 0.0f) {
        math::MTX34 emMtx;
        mManagerEM->CalcGlobalMtx(&emMtx);
        math::VEC3 orig(emMtx._03, emMtx._13, emMtx._23);

        drawInfoWithZ.SetZOffset(zOffset, orig);
    }

    mDrawStrategy->Draw(drawInfoWithZ, this);
}

math::MTX34* ParticleManager::CalcGlobalMtx(math::MTX34* pResult) {
    if (mMtxDirty) {
        math::MTX34 orig;
        mManagerEM->CalcGlobalMtx(&orig);

        mManagerEM->RestructMatrix(&mMtx, &orig, mFlag & FLAG_MTX_INHERIT_SCALE,
                                   mFlag & FLAG_MTX_INHERIT_ROT,
                                   mInheritTranslate,
                                   mFlag & FLAG_MTX_INHERIT_PIVOT);

        mMtxDirty = false;
    }

    *pResult = mMtx;
    return pResult;
}

void ParticleManager::BeginCalc(bool onlyIfRemain) {
    mLastCalced = NULL;

    Particle* pp = static_cast<Particle*>(mActivityList.mActiveList.headObject);

    // clang-format off
    for (; pp != NULL; pp = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pp)->nextObject))
    // clang-format on
    {
        if (!onlyIfRemain || pp->mCalcRemain != 0) {
            if (pp->mCalcRemain != 0) {
                pp->mCalcRemain--;
            }

            if (pp->GetLifeStatus() == NW4R_EF_LS_ACTIVE &&
                pp->mEvalStatus == NW4R_EF_ES_DONE) {

                pp->mEvalStatus = NW4R_EF_ES_WAIT;
            }
        }
    }
}

void ParticleManager::EndCalc() {
    Particle* pp = static_cast<Particle*>(mActivityList.mActiveList.headObject);

    // clang-format off
    for (; pp != NULL; pp = static_cast<Particle*>(
            NW4R_UT_LIST_GET_LINK(mActivityList.mActiveList, pp)->nextObject))
    // clang-format on
    {
        if (pp->GetLifeStatus() == NW4R_EF_LS_ACTIVE &&
            pp->mEvalStatus == NW4R_EF_ES_SKIP) {

            pp->mEvalStatus = NW4R_EF_ES_DONE;
        }
    }
}

void ParticleManager::BeginDraw() {
    math::MTX34 emMtx;
    math::MTX34 pmMtx;

    mManagerEM->CalcGlobalMtx(&emMtx);
    CalcGlobalMtx(&pmMtx);

    math::MTX34Inv(&emMtx, &emMtx);
    math::MTX34Mult(&smDrawMtxPMtoEM, &emMtx, &pmMtx);
}

const math::MTX34* ParticleManager::Draw_GetMtxPMtoEM() const {
    return &smDrawMtxPMtoEM;
}

void ParticleManager::EndDraw() {}

void ParticleManager::Draw_ModifyColor(Particle* pParticle, GXColor* colorPri,
                                       GXColor* colorSec) {
    switch (mModifier.mLight.mType) {
    case ParticleModifier::SIMPLELIGHT_AMBIENT: {
        COLOR_MODULATE(*colorPri, *colorPri, mModifier.mLight.mAmbient);
        COLOR_MODULATE(*colorSec, *colorSec, mModifier.mLight.mAmbient);
        break;
    }

    case ParticleModifier::SIMPLELIGHT_DIFFUSE: {
        if (mModifier.mLight.mRadius < NW4R_MATH_FLT_EPSILON) {
            // clang-format off
            COLOR_MODULATE(*colorPri, *colorPri, mModifier.mLight.mAmbient);
            COLOR_MODULATE(*colorSec, *colorSec, mModifier.mLight.mAmbient);
            // clang-format on
        } else {
            const math::MTX34* pMtxPMtoEM = Draw_GetMtxPMtoEM();

            math::VEC3 pos;
            math::VEC3Transform(&pos, pMtxPMtoEM,
                                &pParticle->mParameter.mPosition);

            math::VEC3Sub(&pos, &pos, &mModifier.mLight.mPosition);
            f32 dist = math::VEC3Len(&pos);

            if (dist > mModifier.mLight.mRadius) {
                // clang-format off
                COLOR_MODULATE(*colorPri, *colorPri, mModifier.mLight.mAmbient);
                COLOR_MODULATE(*colorSec, *colorSec, mModifier.mLight.mAmbient);
                // clang-format on
            } else {
                s32 attn = (256 * dist) / mModifier.mLight.mRadius;

                u16 lr = mModifier.mLight.mDiffuse.r * 256 +
                         attn * (mModifier.mLight.mAmbient.r -
                                 mModifier.mLight.mDiffuse.r);

                u16 lg = mModifier.mLight.mDiffuse.g * 256 +
                         attn * (mModifier.mLight.mAmbient.g -
                                 mModifier.mLight.mDiffuse.g);

                u16 lb = mModifier.mLight.mDiffuse.b * 256 +
                         attn * (mModifier.mLight.mAmbient.b -
                                 mModifier.mLight.mDiffuse.b);

                u16 la = mModifier.mLight.mDiffuse.a * 256 +
                         attn * (mModifier.mLight.mAmbient.a -
                                 mModifier.mLight.mDiffuse.a);

                COLOR_MODULATE(*colorPri, *colorPri, mModifier.mLight.mAmbient);
                COLOR_MODULATE(*colorSec, *colorSec, mModifier.mLight.mAmbient);
            }
        }
        break;
    }

    default: {
        break;
    }
    }
}

} // namespace ef
} // namespace nw4r
