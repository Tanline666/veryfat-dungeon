#include <nw4r/snd.h>

#include <climits>

namespace nw4r {
namespace snd {
namespace detail {

NW4R_UT_RTTI_DEF_BASE(BasicSound);

BasicSound::BasicSound(int priority, int ambientPriority)
    : mId(INVALID_ID),
      mSoundPlayer(NULL),
      mSoundActor(NULL),
      mExtSoundPlayer(NULL),
      mHeap(NULL),
      mGeneralHandle(NULL),
      mTempGeneralHandle(NULL) {
    mAmbientInfo.paramUpdateCallback = NULL;
    mAmbientInfo.argUpdateCallback = NULL;
    mAmbientInfo.argAllocaterCallback = NULL;
    mAmbientInfo.arg = NULL;
    mAmbientInfo.argSize = 0;
    mVoiceOutCount = 0;
    mPriority = priority;
    mAmbientParam.priority = ambientPriority;
}

void BasicSound::InitParam() {
    mPauseState = PAUSE_STATE_NORMAL;
    mUnPauseFlag = false;
    mStartFlag = false;
    mStartedFlag = false;
    mAutoStopFlag = false;
    mFadeOutFlag = false;

    mAutoStopCounter = 0;
    mUpdateCounter = 0;

    mFadeVolume.InitValue(0.0f);
    mPauseFadeVolume.InitValue(1.0f);
    mFadeVolume.SetTarget(1.0f, 1);

    mInitVolume = 1.0f;
    mExtPitch = 1.0f;
    mExtPan = 0.0f;
    mExtSurroundPan = 0.0f;
    mExtMoveVolume.InitValue(1.0f);
    mLpfFreq = 0.0f;
    mBiquadFilterType = BIQUAD_FILTER_TYPE_NONE;
    mBiquadFilterType = 0.0f;
    mOutputLineFlag = (mSoundPlayer != NULL)
                          ? mSoundPlayer->GetDefaultOutputLine()
                          : OUTPUT_LINE_MAIN;

    mMainOutVolume = 1.0f;
    mMainSend = 0.0f;
    for (int i = 0; i < AUX_BUS_NUM; i++) {
        mFxSend[i] = 0.0f;
    }
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mRemoteOutVolume[i] = 1.0f;
    }

    mAmbientParam.volume = 1.0f;
    mAmbientParam.pitch = 1.0f;
    mAmbientParam.pan = 0.0f;
    mAmbientParam.surroundPan = 0.0f;
    mAmbientParam.fxSend = 0.0f;
    mAmbientParam.lpf = 0.0f;
    mAmbientParam.biquadFilterValue = 0.0f;
    mAmbientParam.biquadFilterType = BIQUAD_FILTER_TYPE_NONE;
    mAmbientParam.priority = 0;
}

void BasicSound::StartPrepared() {
    if (!mStartedFlag) {
        mStartFlag = true;
    }
}

void BasicSound::Stop(int frames) {
    BasicPlayer& rPlayer = GetBasicPlayer();

    if (frames == 0 || !rPlayer.IsActive() || !rPlayer.IsStarted() ||
        rPlayer.IsPause()) {
        Shutdown();
        return;
    }

    int t = frames * mFadeVolume.GetValue();
    mFadeVolume.SetTarget(0.0f, t);

    SetPlayerPriority(0);
    mAutoStopFlag = false;
    mPauseState = PAUSE_STATE_NORMAL;
    mUnPauseFlag = false;
    mFadeOutFlag = true;
}

void BasicSound::Pause(bool flag, int frames) {
    int t;

    if (flag) {
        switch (mPauseState) {
        case PAUSE_STATE_NORMAL:
        case PAUSE_STATE_PAUSING:
        case PAUSE_STATE_UNPAUSING:
            t = frames * mPauseFadeVolume.GetValue();
            if (t <= 0) {
                t = 1;
            }
            mPauseFadeVolume.SetTarget(0.0f, t);
            mPauseState = PAUSE_STATE_PAUSING;
            mUnPauseFlag = FALSE;
            break;

        case PAUSE_STATE_PAUSED:
            return;

        default:
            return;
        }
    } else {
        switch (mPauseState) {
        case PAUSE_STATE_NORMAL:
            return;
        case PAUSE_STATE_PAUSING:
        case PAUSE_STATE_UNPAUSING:
        case PAUSE_STATE_PAUSED:
            t = frames * (1.0f - mPauseFadeVolume.GetValue());
            if (t <= 0) {
                t = 1;
            }
            mPauseFadeVolume.SetTarget(1.0f, t);
            mPauseState = PAUSE_STATE_UNPAUSING;
            mUnPauseFlag = TRUE;
            break;
        default:
            return;
        }
    }
}

void BasicSound::SetAutoStopCounter(int count) {
    mAutoStopCounter = count;
    mAutoStopFlag = count > 0;
}

//! Unused or inlined in Wii Fit Plus
void BasicSound::FadeIn(int frames) {
    if (mFadeOutFlag) {
        return;
    }

    frames = frames * (1.0f - mFadeVolume.GetValue());
    mFadeVolume.SetTarget(1.0f, frames);
}

bool BasicSound::IsPause() const {
    return (mPauseState == PAUSE_STATE_PAUSING) ||
           (mPauseState == PAUSE_STATE_PAUSED);
}

void BasicSound::Update() {
    BasicPlayer& rPlayer = GetBasicPlayer();

    if (mAutoStopFlag && rPlayer.IsActive()) {
        if (mAutoStopCounter == 0) {
            if ((mPauseState == PAUSE_STATE_NORMAL) ||
                (mPauseState == PAUSE_STATE_UNPAUSING)) {
                Stop(0);
                return;
            }
        } else {
            mAutoStopCounter--;
        }
    }

    bool startPlayer = false;
    if (!mStartedFlag) {
        if (!mStartFlag) {
            return;
        }

        if (!IsPrepared()) {
            return;
        }

        startPlayer = true;
    }

    if (rPlayer.IsStarted() && mUpdateCounter < ULONG_MAX) {
        mUpdateCounter++;
    }

    if (!rPlayer.IsActive()) {
        Shutdown();
        return;
    }

    switch (mPauseState) {
    case PAUSE_STATE_PAUSING:
        mPauseFadeVolume.Update();
        break;
    case PAUSE_STATE_UNPAUSING:
        mPauseFadeVolume.Update();
        UpdateMoveValue();
        break;
    case PAUSE_STATE_NORMAL:
        UpdateMoveValue();
        break;
    default:
        break;
    }

    if (mAmbientInfo.argUpdateCallback != NULL) {
        mAmbientInfo.argUpdateCallback->detail_UpdateAmbientArg(
            mAmbientInfo.arg, this);
    }

    if (mAmbientInfo.paramUpdateCallback != NULL) {
        SoundAmbientParam ambParam;
        if (mUpdateCounter > 0) {
            ambParam.volume = mAmbientParam.volume;
            ambParam.pitch = mAmbientParam.pitch;
            ambParam.pan = mAmbientParam.pan;
            ambParam.surroundPan = mAmbientParam.surroundPan;
            ambParam.fxSend = mAmbientParam.fxSend;
            ambParam.lpf = mAmbientParam.lpf;
            ambParam.biquadFilterValue = mAmbientParam.biquadFilterValue;
            ambParam.biquadFilterType = mAmbientParam.biquadFilterType;
            ambParam.priority = mAmbientParam.priority;
            ambParam.userData = mAmbientParam.userData;
        } else {
            ambParam.userData = 0;
        }

        for (int i = 0; i < mVoiceOutCount; i++) {
            ambParam.voiceOutParam[i] = rPlayer.GetVoiceOutParam(i);
        }

        mAmbientInfo.paramUpdateCallback->detail_UpdateAmbientParam(
            mAmbientInfo.arg, mId, mVoiceOutCount, &ambParam);

        mAmbientParam.volume = ambParam.volume;
        mAmbientParam.pitch = ambParam.pitch;
        mAmbientParam.pan = ambParam.pan;
        mAmbientParam.surroundPan = ambParam.surroundPan;

        for (int i = 0; i < mVoiceOutCount; i++) {
            rPlayer.SetVoiceOutParam(i, ambParam.voiceOutParam[i]);
        }
    }

    if (mSoundActor) {
        mActorParam = mSoundActor->detail_GetActorParam();
    }

    UpdateParam();

    if (mFadeOutFlag && mFadeVolume.IsFinished()) {
        mFadeOutFlag = FALSE;
        Shutdown();
        return;
    }

    if (startPlayer) {
        if (rPlayer.Start()) {
            mStartedFlag = TRUE;
            mStartFlag = FALSE;
        } else {
            Shutdown();
            return;
        }
    }

    if (mPauseState == PAUSE_STATE_PAUSING) {
        if (mPauseFadeVolume.IsFinished()) {
            rPlayer.Pause(TRUE);
            mPauseState = PAUSE_STATE_PAUSED;
        }
    } else if (mPauseState == PAUSE_STATE_UNPAUSING) {
        if (mPauseFadeVolume.IsFinished()) {
            mPauseState = PAUSE_STATE_NORMAL;
        }
    }

    if (mUnPauseFlag) {
        rPlayer.Pause(FALSE);
        mUnPauseFlag = FALSE;
    }
}

void BasicSound::UpdateMoveValue() {
    mFadeVolume.Update();
    mExtMoveVolume.Update();
}

void BasicSound::UpdateParam() {
    f32 vol = 1.0f;
    vol *= mInitVolume;
    vol *= GetSoundPlayer()->GetVolume();
    vol *= mExtMoveVolume.GetValue();
    vol *= mFadeVolume.GetValue();
    vol *= mPauseFadeVolume.GetValue();
    vol *= mAmbientParam.volume;
    vol *= mActorParam.volume;

    f32 pan = 0.0f;
    pan += mExtPan;
    pan += mAmbientParam.pan;
    pan += mActorParam.pan;

    f32 surPan = 0.0f;
    surPan += mExtSurroundPan;
    surPan += mAmbientParam.surroundPan;

    f32 pitch = 1.0f;
    pitch *= mExtPitch;
    pitch *= mAmbientParam.pitch;
    pitch *= mActorParam.pitch;

    f32 lpfFreq = mLpfFreq;
    lpfFreq += mAmbientParam.lpf;
    lpfFreq += GetSoundPlayer()->GetLpfFreq();

    int biqFilType = mBiquadFilterType;
    f32 biqFilVal = mBiquadFilterValue;
    if (biqFilType == BIQUAD_FILTER_TYPE_NONE) {
        biqFilType = GetSoundPlayer()->GetBiquadFilterType();
        biqFilVal = GetSoundPlayer()->GetBiquadFilterValue();
    }

    int outFlag = mOutputLineFlag;

    f32 mainVol = 1.0f;
    mainVol *= mMainOutVolume;
    mainVol *= GetSoundPlayer()->GetMainOutVolume();

    f32 rcVol[WPAD_MAX_CONTROLLERS];
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        rcVol[i] = 1.0f;
        rcVol[i] *= GetSoundPlayer()->GetRemoteOutVolume(i);
        rcVol[i] *= mRemoteOutVolume[i];
    }

    f32 mainSend = 0.0f;
    mainSend += mMainSend;
    mainSend += GetSoundPlayer()->GetMainSend();

    f32 fxSend[AUX_BUS_NUM];
    for (int i = 0; i < AUX_BUS_NUM; i++) {
        fxSend[i] = 0.0f;
        fxSend[i] += mFxSend[i];
        fxSend[i] += GetSoundPlayer()->GetFxSend(static_cast<AuxBus>(i));
    }
    fxSend[0] += mAmbientParam.fxSend;

    BasicPlayer& rPlayer = GetBasicPlayer();
    rPlayer.SetVolume(vol);
    rPlayer.SetPan(pan);
    rPlayer.SetSurroundPan(surPan);
    rPlayer.SetPitch(pitch);
    rPlayer.SetLpfFreq(lpfFreq);
    rPlayer.SetBiquadFilter(biqFilType, biqFilVal);
    rPlayer.SetOutputLine(outFlag);
    rPlayer.SetMainOutVolume(mainVol);
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        rPlayer.SetRemoteOutVolume(i, rcVol[i]);
    }
    rPlayer.SetMainSend(mainSend);
    for (int i = 0; i < AUX_BUS_NUM; i++) {
        rPlayer.SetFxSend(static_cast<AuxBus>(i), fxSend[i]);
    }
}

void BasicSound::Shutdown() {
    BasicPlayer& rPlayer = GetBasicPlayer();

    if (rPlayer.IsActive()) {
        if (mFadeOutFlag) {
            rPlayer.SetVolume(0.0f);
        }
        rPlayer.Stop();
    }

    SetId(INVALID_ID);

    if (IsAttachedGeneralHandle()) {
        DetachGeneralHandle();
    }

    if (IsAttachedTempGeneralHandle()) {
        DetachTempGeneralHandle();
    }

    if (IsAttachedTempSpecialHandle()) {
        DetachTempSpecialHandle();
    }

    if (mHeap) {
        mSoundPlayer->detail_FreePlayerHeap(this);
    }

    if (mSoundPlayer) {
        mSoundPlayer->detail_RemoveSound(this);
    }

    if (mExtSoundPlayer) {
        mExtSoundPlayer->RemoveSound(this);
    }

    if (mAmbientInfo.argAllocaterCallback) {
        mAmbientInfo.argAllocaterCallback->detail_FreeAmbientArg(
            mAmbientInfo.arg, this);
        mAmbientInfo.arg = NULL;
    }

    mStartedFlag = FALSE;
    mFadeOutFlag = FALSE;
}

void BasicSound::AttachPlayerHeap(PlayerHeap* pHeap) {
    mHeap = pHeap;
}

void BasicSound::DetachPlayerHeap(PlayerHeap* pHeap) {
//! pHeap is only for debug builds
#pragma unused(pHeap)
    mHeap = NULL;
}

void BasicSound::AttachSoundPlayer(SoundPlayer* pPlayer) {
    mSoundPlayer = pPlayer;
}

void BasicSound::DetachSoundPlayer(SoundPlayer* pPlayer) {
//! pPlayer is only for debug builds
#pragma unused(pPlayer)
    mSoundPlayer = NULL;
}

void BasicSound::AttachSoundActor(SoundActor* pActor) {
    mSoundActor = pActor;
}

void BasicSound::DetachSoundActor(SoundActor* pActor) {
//! pActor is only for debug builds
#pragma unused(pActor)
    mSoundActor = NULL;
}

void BasicSound::AttachExternalSoundPlayer(ExternalSoundPlayer* pExtPlayer) {
    mExtSoundPlayer = pExtPlayer;
}

void BasicSound::DetachExternalSoundPlayer(ExternalSoundPlayer* pExtPlayer) {
#pragma unused(pExtPlayer)
    mExtSoundPlayer = NULL;
}

int BasicSound::GetRemainingFadeFrames() const {
    return mFadeVolume.GetRemainingCount();
}

int BasicSound::GetVoiceOutCount() const {
    return mVoiceOutCount;
}

void BasicSound::SetPlayerPriority(int priority) {
    mPriority = priority;

    if (mSoundPlayer != NULL) {
        mSoundPlayer->detail_SortPriorityList(this);
    }
    OnUpdatePlayerPriority();
}

void BasicSound::SetInitialVolume(f32 vol) {
    if (vol < 0.0f) {
        vol = 0.0f;
    }
    mInitVolume = vol;
}

void BasicSound::SetVolume(f32 vol, int frames) {
    if (vol < 0.0f) {
        vol = 0.0f;
    }
    mExtMoveVolume.SetTarget(vol, frames);
}

void BasicSound::SetPitch(f32 pitch) {
    mExtPitch = pitch;
}

void BasicSound::SetPan(f32 pan) {
    mExtPan = pan;
}

void BasicSound::SetSurroundPan(f32 pan) {
    mExtSurroundPan = pan;
}

void BasicSound::SetLpfFreq(f32 freq) {
    mLpfFreq = freq;
}

//! Yes, mBiquadFilterType is a char, not an int, but the symbols in TFP2 and
//! such have it this way for some reason. A common theme
void BasicSound::SetBiquadFilter(int type, f32 val) {
    mBiquadFilterType = type;
    mBiquadFilterValue = val;
}

void BasicSound::SetOutputLine(int flag) {
    mOutputLineFlag = flag;
}

int BasicSound::GetOutputLine() const {
    return mOutputLineFlag;
}

void BasicSound::SetMainOutVolume(f32 vol) {
    if (vol < 0.0f) {
        vol = 0.0f;
    }
    mMainOutVolume = vol;
}

void BasicSound::SetRemoteOutVolume(int remote, f32 vol) {
    if (vol < 0.0f) {
        vol = 0.0f;
    }
    mRemoteOutVolume[remote] = vol;
}

void BasicSound::SetMainSend(f32 send) {
    mMainSend = send;
}

void BasicSound::SetFxSend(AuxBus bus, f32 send) {
    mFxSend[bus] = send;
}

void BasicSound::SetRemoteFilter(int filter) {
    GetBasicPlayer().SetRemoteFilter(filter);
}

void BasicSound::SetPanMode(PanMode mode) {
    GetBasicPlayer().SetPanMode(mode);
}

void BasicSound::SetPanCurve(PanCurve curve) {
    GetBasicPlayer().SetPanCurve(curve);
}

void BasicSound::SetAmbientInfo(const AmbientInfo& rInfo) {
    void* pAmbArg =
        rInfo.argAllocaterCallback->detail_AllocAmbientArg(rInfo.argSize);
    if (!pAmbArg) {
        return;
    }
    memcpy(pAmbArg, rInfo.arg, rInfo.argSize);
    mAmbientInfo = rInfo;
    mAmbientInfo.arg = pAmbArg;

    if (rInfo.paramUpdateCallback) {
        int voiceOutCount =
            mAmbientInfo.paramUpdateCallback->detail_GetRequiredVoiceOutCount(
                mAmbientInfo.arg, mId);
        if (voiceOutCount > VOICE_OUT_MAX) {
            voiceOutCount = VOICE_OUT_MAX;
        }
        mVoiceOutCount = voiceOutCount;
    }
}

int BasicSound::GetAmbientPriority(const AmbientInfo& rInfo, u32 id) {
    if (!rInfo.paramUpdateCallback) {
        return FALSE;
    }

    int prio =
        rInfo.paramUpdateCallback->detail_GetAmbientPriority(rInfo.arg, id);
    return prio;
}

bool BasicSound::IsAttachedGeneralHandle() {
    return mGeneralHandle != NULL;
}

bool BasicSound::IsAttachedTempGeneralHandle() {
    return mTempGeneralHandle != NULL;
}

void BasicSound::DetachGeneralHandle() {
    mGeneralHandle->DetachSound();
}

void BasicSound::DetachTempGeneralHandle() {
    mTempGeneralHandle->DetachSound();
}

void BasicSound::SetId(u32 id) {
    mId = id;
    GetBasicPlayer().SetId(id);
}

} // namespace detail
} // namespace snd
} // namespace nw4r
