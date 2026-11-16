#include <nw4r/snd.h>

namespace nw4r {
namespace snd {

Sound3DActor::Sound3DActor(SoundArchivePlayer& rPlayer,
                           Sound3DManager& rManager)
    : SoundActor(rPlayer),
      m3DManager(rManager),
      mSoundArchivePlayer(&rPlayer),
      mPosition(0.0f, 0.0f, 0.0f),
      mVelocity(0.0f, 0.0f, 0.0f),
      mUserParam(NULL),
      mResetPositionFlag(TRUE) {}

Sound3DActor::~Sound3DActor() {
    SoundActor::ForEachSound(ClearUpdateCallback);
}

SoundStartable::StartResult
Sound3DActor::SetupSound(SoundHandle* pHandle, u32 id,
                         const SoundStartable::StartInfo* pStartInfo,
                         void* setupArg) {

    Sound3DParam param;
    param.position = mPosition;
    param.velocity = mVelocity;
    param.actorUserParam = mUserParam;
    if (mSoundArchivePlayer) {
        const SoundArchive& soundArchive =
            mSoundArchivePlayer->GetSoundArchive();
        SoundArchive::Sound3DParam p;
        if (soundArchive.detail_ReadSound3DParam(id, &p)) {
            param.ctrl = p.flags;
            param.decayRatio = p.decayRatio;
            param.dopplerFactor = p.dopplerFactor;

            switch (p.decayCurve) {
            case DECAY_CURVE_LOGARITHMIC:
                param.decayCurve = DECAY_CURVE_LOGARITHMIC;
                break;
            case DECAY_CURVE_LINEAR:
                param.decayCurve = DECAY_CURVE_LINEAR;
                break;
            default:
                param.decayCurve = DECAY_CURVE_LOGARITHMIC;
                break;
            }
        }
        param.soundUserParam = soundArchive.GetSoundUserParam(id);
    }

    detail::BasicSound::AmbientInfo argInfo = {
        &m3DManager,  // paramUpdateCallback
        this,         // argUpdateCallback
        &m3DManager,  // argAllocaterCallback
        &param,       // arg
        sizeof(param) // argSize
    };

    StartResult result = SoundActor::detail_SetupSoundWithAmbientInfo(
        pHandle, id, pStartInfo, &argInfo, setupArg);

    if (pHandle->IsAttachedSound()) {
        pHandle->detail_GetAttachedSound()->SetPanCurve(PAN_CURVE_SINCOS);
    }

    return result;
}

void Sound3DActor::SetPosition(const math::VEC3& rPosition) {
    if (!mResetPositionFlag) {
        math::VEC3Sub(&mVelocity, &rPosition, &mPosition);
    }
    mPosition = rPosition;
    mResetPositionFlag = FALSE;
}

void Sound3DActor::detail_UpdateAmbientArg(void* pArg,
                                           const detail::BasicSound* pSound) {
#pragma unused(pSound)

    Sound3DParam* pParam = static_cast<Sound3DParam*>(pArg);

    pParam->position = mPosition;
    pParam->velocity = mVelocity;
    pParam->actorUserParam = mUserParam;
}

void Sound3DActor::ClearUpdateCallback(SoundHandle& rHandle) {
    if (rHandle.IsAttachedSound()) {
        rHandle.detail_GetAttachedSound()->ClearAmbientArgUpdateCallback();
    }
}

} // namespace snd
} // namespace nw4r
