#include <nw4r/snd.h>

namespace nw4r {
namespace snd {
namespace detail {

void PlayerParamSet::Init() {
    volume = 1.0f;
    pitch = 1.0f;
    pan = 0.0f;
    surroundPan = 0.0f;
    lpfFreq = 0.0f;
    biquadType = BIQUAD_FILTER_TYPE_NONE;
    biquadValue = 0.0f;
    remoteFilter = 0;
    outputLineFlag = OUTPUT_LINE_MAIN;
    mainOutVolume = 1.0f;
    mainSend = 0.0f;

    panMode = PAN_MODE_DUAL;
    panCurve = PAN_CURVE_SQRT;

    for (int i = 0; i < AUX_BUS_NUM; i++) {
        fxSend[i] = 0.0f;
    }
    for (int i = 0; i < VOICE_OUT_MAX; i++) {
        remoteOutVolume[i] = 1.0f;
    }
    for (int i = 0; i < VOICE_OUT_MAX; i++) {
        VoiceOutParam* pParam = &voiceOutParam[i];
        pParam->volume = 1.0f;
        pParam->pitch = 1.0f;
        pParam->pan = 0.0f;
        pParam->surroundPan = 0.0f;
        pParam->fxSend = 0.0f;
        pParam->lpf = 0.0f;
    }
}

BasicPlayer::BasicPlayer() : mId(BasicSound::INVALID_ID) {
    InitParam();
}

void BasicPlayer::InitParam() {
    mPlayerParamSet.Init();
}

void BasicPlayer::SetFxSend(AuxBus bus, f32 send) {
    mPlayerParamSet.fxSend[bus] = send;
}

f32 BasicPlayer::GetFxSend(AuxBus bus) const {
    return mPlayerParamSet.fxSend[bus];
}

void BasicPlayer::SetBiquadFilter(int type, f32 val) {
    mPlayerParamSet.biquadType = type;
    mPlayerParamSet.biquadValue = val;
}

void BasicPlayer::SetRemoteFilter(int filter) {
    mPlayerParamSet.remoteFilter = filter;
}

void BasicPlayer::SetRemoteOutVolume(int remote, f32 volume) {
    mPlayerParamSet.remoteOutVolume[remote] = volume;
}

f32 BasicPlayer::GetRemoteOutVolume(int remote) const {
    return mPlayerParamSet.remoteOutVolume[remote];
}

} // namespace detail
} // namespace snd
} // namespace nw4r
