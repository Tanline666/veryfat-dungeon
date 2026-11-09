#include <nw4r/ef.h>

#include <cmath>

namespace nw4r {
namespace ef {

Particle::Particle() {}

Particle::~Particle() {}

void Particle::SendClosing() {
    mParticleManager->Closing(this);
}

void Particle::DestroyFunc() {}

void ParticleParameter::Initialize(ParticleParameterDesc* ppd,
                                   ParticleManager* pm) {

    NW4R_POINTER_ASSERT(ppd, 55);
    NW4R_POINTER_ASSERT(pm, 56);

    int i, j;

    for (i = 0; i < COLOR_LAYER_MAX; i++) {
        for (j = 0; j < COLOR_IDX_MAX; j++) {
            mColor[i][j] = ppd->mColor[i][j];
        }
    }

    mSize = ppd->size;
    mScale = ppd->scale;
    mRotate = ppd->rotate;

    for (i = 0; i < AXIS_MAX; i++) {
        f32 rad = ppd->rotateOffset[i];

        if (ppd->rotateOffsetRandom[i]) {
            // clang-format off
            rad += ppd->rotateOffsetRandom[i] *
                   (rad * (2.0f * pm->mManagerEM->mRandom.RandFloat() - 1.0f)) / 100.0f;
            // clang-format on
        }

        mRotateOffset[i] = ConvertF32RadToU8(rad);
    }

    for (i = 0; i < TEX_LAYER_MAX; i++) {
        mTexture[i] = ppd->mTexture[i];
        mTextureScale[i] = ppd->textureScale[i];
        mTextureRotate[i] = ppd->textureRotate[i];
        mTextureTranslate[i] = ppd->textureTranslate[i];
    }

    mTextureWrap = ppd->textureWrap;
    mTextureReverse = ppd->textureReverse;
    mACmpRef0 = ppd->mACmpRef0;
    mACmpRef1 = ppd->mACmpRef1;

    mCollisionStatus = COLLISION_STATUS_NONE;
}

bool Particle::Initialize(u16 aLife, math::VEC3 aPosition, math::VEC3 aVelocity,
                          ParticleManager* pm, const math::MTX34* space,
                          float momentum, const EmitterInheritSetting* setting,
                          Particle* referencePP) {

    NW4R_POINTER_ASSERT(pm, 114);

    ReferencedObject::Initialize();

    if (aLife == 0) {
        aLife = 1;
    }

    mParticleManager = pm;
    pm->Ref();

    mEvalStatus = NW4R_EF_ES_WAIT;
    mCalcRemain = 0;

    (void)Resource::GetInstance(); // unused

    EmitterDesc* ed = pm->mResource->GetEmitterDesc();
    ParticleParameterDesc* ppd = pm->mResource->GetParticleParameterDesc();

    mParameter.Initialize(ppd, pm);
    mParameter.mMomentum = momentum;

    mTick = 0;
    mRandSeed = pm->mManagerEM->mRandom.Rand();
    mLife = aLife;
    mParameter.mAlphaFlickRnd = (pm->mManagerEM->mRandom.Rand() % 254) - 126;

    mParameter.mPosition = aPosition;
    mParameter.mPrevPosition = mParameter.mPosition;
    mParameter.mVelocity = aVelocity;

    NW4R_POINTER_ASSERT(pm->mManagerEM->mManagerEF, 146);

    const math::VEC3* pEffVel = pm->mManagerEM->mManagerEF->GetVelocity();

    if (pEffVel->x != 0.0f || pEffVel->y != 0.0f || pEffVel->z != 0.0f) {
        math::MTX34 glbMtx;
        pm->mManagerEM->CalcGlobalMtx(&glbMtx);

        math::MTX34Inv(&glbMtx, &glbMtx);
        glbMtx._03 = glbMtx._13 = glbMtx._23 = 0.0f;

        math::VEC3 glbVel;
        math::VEC3Transform(&glbVel, &glbMtx, pEffVel);
        mParameter.mVelocity += glbVel;
    }

    if (space != NULL) {
        math::VEC3Transform(&mParameter.mPosition, space,
                            &mParameter.mPosition);

        math::MTX34 spaceProj(*space);
        spaceProj._03 = spaceProj._13 = spaceProj._23 = 0.0f;

        math::VEC3Transform(&mParameter.mVelocity, &spaceProj,
                            &mParameter.mVelocity);
    }

    mPrevAxis = math::VEC3(2.0f, 0.0f, 0.0f);

    if (referencePP != NULL && setting != NULL) {
        if (setting->speed != 0) {
            math::VEC3 glbSpeed;
            math::MTX34 glbMtx;

            referencePP->mParticleManager->CalcGlobalMtx(&glbMtx);
            glbMtx._03 = glbMtx._13 = glbMtx._23 = 0.0f;
            math::VEC3Transform(&glbSpeed, &glbMtx,
                                &referencePP->mParameter.mVelocity);

            mParticleManager->CalcGlobalMtx(&glbMtx);
            glbMtx._03 = glbMtx._13 = glbMtx._23 = 0.0f;
            math::MTX34Inv(&glbMtx, &glbMtx);
            math::VEC3Transform(&glbSpeed, &glbMtx, &glbSpeed);

            mParameter.mVelocity.x +=
                glbSpeed.x * setting->speed * (1.0f / 100.0f);
            mParameter.mVelocity.y +=
                glbSpeed.y * setting->speed * (1.0f / 100.0f);
            mParameter.mVelocity.z +=
                glbSpeed.z * setting->speed * (1.0f / 100.0f);
        }

        if (setting->scale != 0) {
            mParameter.mSize.x =
                setting->scale * referencePP->Draw_GetSizeX() * (1.0f / 100.0f);
            mParameter.mSize.y =
                setting->scale * referencePP->Draw_GetSizeY() * (1.0f / 100.0f);
        }

        if (setting->alpha != 0) {
            GXColor modify0 = {255, 255, 255, 255};
            GXColor modify1 = {255, 255, 255, 255};
            referencePP->mParticleManager->Draw_ModifyColor(referencePP,
                                                            &modify0, &modify1);
            for (int i = 0; i < COLOR_LAYER_MAX; i++) {
                if (setting->alphaFuncPri) {
                    u32 alpha = 255;
                    if (setting->alphaFuncPri & 1) {
                        alpha = alpha * referencePP->GetAlpha(i, 0) + 255;
                        alpha >>= 8;
                    }
                    if (setting->alphaFuncPri & 2) {
                        alpha = alpha * referencePP->GetAlpha(i, 1) + 255;
                        alpha >>= 8;
                    }
                    if ((setting->alphaFuncPri & 4) != 0) {
                        alpha = alpha * referencePP->GetAlphaFlick() + 255;
                        alpha = alpha * modify0.a + 255;
                        alpha >>= 16;
                    }

                    mParameter.mColor[i][0].a =
                        static_cast<u8>(alpha * setting->alpha / 100);
                }
                if (setting->alphaFuncSec) {
                    u32 alpha = 255;
                    if (setting->alphaFuncSec & 1) {
                        alpha = alpha * referencePP->GetAlpha(i, 0) + 255;
                        alpha >>= 8;
                    }
                    if (setting->alphaFuncSec & 2) {
                        alpha = alpha * referencePP->GetAlpha(i, 1) + 255;
                        alpha >>= 8;
                    }
                    if (setting->alphaFuncSec & 4) {
                        alpha = alpha * referencePP->GetAlphaFlick() + 255;
                        alpha = alpha * modify0.a + 255;
                        alpha >>= 16;
                    }

                    mParameter.mColor[i][1].a =
                        static_cast<u8>(alpha * setting->alpha / 100);
                }
            }
        }

        if (setting->color != 0) {
            for (int i = 0; i < COLOR_LAYER_MAX; i++) {
                for (int j = 0; j < COLOR_IDX_MAX; j++) {
                    GXColor col;
                    referencePP->GetColorRGB(i, j, &col);
                    mParameter.mColor[i][j].r = static_cast<u8>(
                        static_cast<int>(col.r) * setting->color / 100);
                    mParameter.mColor[i][j].g = static_cast<u8>(
                        static_cast<int>(col.g) * setting->color / 100);
                    mParameter.mColor[i][j].b = static_cast<u8>(
                        static_cast<int>(col.b) * setting->color / 100);
                }
            }
        }

        if (setting->flag & EmitterInheritSetting::FLAG_INHERIT_ROT) {
            f32 rad;

            rad = referencePP->mParameter.mRotate.x;
            rad += NW4R_MATH_FIDX_TO_RAD(
                referencePP->mParameter.mRotateOffset[AXIS_X]);
            mParameter.mRotateOffset[AXIS_X] = ConvertF32RadToU8(rad);

            rad = referencePP->mParameter.mRotate.y;
            rad += NW4R_MATH_FIDX_TO_RAD(
                referencePP->mParameter.mRotateOffset[AXIS_Y]);
            mParameter.mRotateOffset[AXIS_Y] = ConvertF32RadToU8(rad);

            rad = referencePP->mParameter.mRotate.z;
            rad += NW4R_MATH_FIDX_TO_RAD(
                referencePP->mParameter.mRotateOffset[AXIS_Z]);
            mParameter.mRotateOffset[AXIS_Z] = ConvertF32RadToU8(rad);
        }
    }

    return true;
}

u8 Particle::GetAlphaFlick() const {
    const EmitterDesc* ed = mParticleManager->mResource->GetEmitterDesc();
    const EmitterDrawSetting& drawSetting = ed->drawSetting;

    if (drawSetting.mAlphaFlickType == EmitterDrawSetting::ALPHAFLICK_NONE) {
        return 255;
    }

    u32 cycleL = (drawSetting.mAlphaFlickCycle +
                  drawSetting.mAlphaFlickCycle * mParameter.mAlphaFlickRnd *
                      drawSetting.mAlphaFlickRandom / (127 * 100));
    u16 cycle;

    if (cycleL <= 65535) {
        cycle = cycleL;
    } else {
        cycle = 65535;
    }

    u16 pos = (mTick - 1) % cycle;
    s16 alpha = 0;

    switch (drawSetting.mAlphaFlickType) {
    case EmitterDrawSetting::ALPHAFLICK_TRIANGLE:
        if (pos * 2 <= cycle) {
            alpha = pos * 4 * drawSetting.mAlphaFlickAmplitude / cycle +
                    (128 - drawSetting.mAlphaFlickAmplitude);
        } else {
            alpha = pos * -4 * drawSetting.mAlphaFlickAmplitude / cycle +
                    (128 + 3 * drawSetting.mAlphaFlickAmplitude);
        }
        break;

    case EmitterDrawSetting::ALPHAFLICK_SAWTOOTH1:
        alpha = pos * -2 * drawSetting.mAlphaFlickAmplitude / cycle +
                (128 + drawSetting.mAlphaFlickAmplitude);
        break;

    case EmitterDrawSetting::ALPHAFLICK_SAWTOOTH2:
        alpha = pos * 2 * drawSetting.mAlphaFlickAmplitude / cycle +
                (128 - drawSetting.mAlphaFlickAmplitude);
        break;

    case EmitterDrawSetting::ALPHAFLICK_SQUARE:
        if (pos * 2 <= cycle) {
            alpha = 128 + drawSetting.mAlphaFlickAmplitude;
        } else {
            alpha = 128 - drawSetting.mAlphaFlickAmplitude;
        }
        break;

    case EmitterDrawSetting::ALPHAFLICK_SINE:
        alpha = 128 + drawSetting.mAlphaFlickAmplitude *
                          math::SinRad(2.0f * NW4R_MATH_PI * pos / cycle);
        break;

    default:
        NW4R_ASSERT(FALSE, 351);
        break;
    }

    if (alpha < 0) {
        alpha = 0;
    }
    if (alpha > 255) {
        alpha = 255;
    }

    return alpha;
}

void Particle::Draw_GetColor(int idx, GXColor* mColor0, GXColor* mColor1) {

    const EmitterDesc* pDesc = mParticleManager->mResource->GetEmitterDesc();
    const EmitterDrawSetting& rSetting = pDesc->drawSetting;

    GetColor(idx, COLOR_IDX_PRI, mColor0);
    GetColor(idx, COLOR_IDX_SEC, mColor1);

    if (rSetting.mAlphaFlickType != EmitterDrawSetting::ALPHAFLICK_NONE) {
        u8 alpha = GetAlphaFlick();

        mColor0->a = static_cast<u8>((mColor0->a * alpha + 128) >> 8);
        mColor1->a = static_cast<u8>((mColor1->a * alpha + 128) >> 8);
    }

    mParticleManager->Draw_ModifyColor(this, mColor0, mColor1);
}

} // namespace ef
} // namespace nw4r
