#ifndef NW4R_SND_BASIC_PLAYER_H
#define NW4R_SND_BASIC_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Types.h>
#include <nw4r/ut.h>

#include <revolution/WPAD.h>

namespace nw4r {
namespace snd {
namespace detail {

struct PlayerParamSet {
    f32 volume;
    f32 pitch;
    f32 pan;
    f32 surroundPan;
    f32 lpfFreq;
    f32 biquadValue;
    u8 biquadType;
    u8 remoteFilter;
    int outputLineFlag;
    f32 mainOutVolume;
    f32 mainSend;
    PanMode panMode;
    PanCurve panCurve;
    f32 fxSend[AUX_BUS_NUM];
    f32 remoteOutVolume[WPAD_MAX_CONTROLLERS];
    VoiceOutParam voiceOutParam[VOICE_OUT_MAX];

    PlayerParamSet() {
        Init();
    }
    void Init();
};

class BasicPlayer {
public:
    BasicPlayer();
    virtual ~BasicPlayer() {} // at 0x8

    virtual bool Start() = 0;           // at 0xC
    virtual void Stop() = 0;            // at 0x10
    virtual void Pause(bool flag) = 0;  // at 0x14
    virtual bool IsActive() const = 0;  // at 0x18
    virtual bool IsStarted() const = 0; // at 0x1C
    virtual bool IsPause() const = 0;   // at 0x20

    void InitParam();

    u32 GetId() const {
        return mId;
    }
    void SetId(u32 id) {
        mId = id;
    }

    f32 GetVolume() const {
        return mPlayerParamSet.volume;
    }
    void SetVolume(f32 volume) {
        mPlayerParamSet.volume = volume;
    }

    f32 GetPitch() const {
        return mPlayerParamSet.pitch;
    }
    void SetPitch(f32 pitch) {
        mPlayerParamSet.pitch = pitch;
    }

    f32 GetPan() const {
        return mPlayerParamSet.pan;
    }
    void SetPan(f32 pan) {
        mPlayerParamSet.pan = pan;
    }

    f32 GetSurroundPan() const {
        return mPlayerParamSet.surroundPan;
    }
    void SetSurroundPan(f32 pan) {
        mPlayerParamSet.surroundPan = pan;
    }

    f32 GetLpfFreq() const {
        return mPlayerParamSet.lpfFreq;
    }
    void SetLpfFreq(f32 freq) {
        mPlayerParamSet.lpfFreq = freq;
    }

    int GetBiquadFilterType() const {
        return mPlayerParamSet.biquadType;
    }

    f32 GetBiquadFilterValue() const {
        return mPlayerParamSet.biquadValue;
    }

    void SetBiquadFilter(int type, f32 value);

    int GetOutputLine() const {
        return mPlayerParamSet.outputLineFlag;
    }
    void SetOutputLine(int flags) {
        mPlayerParamSet.outputLineFlag = flags;
    }

    f32 GetMainOutVolume() const {
        return mPlayerParamSet.mainOutVolume;
    }
    void SetMainOutVolume(f32 volume) {
        mPlayerParamSet.mainOutVolume = volume;
    }

    f32 GetMainSend() const {
        return mPlayerParamSet.mainSend;
    }
    void SetMainSend(f32 send) {
        mPlayerParamSet.mainSend = send;
    }

    void SetFxSend(AuxBus bus, f32 send);
    f32 GetFxSend(AuxBus bus) const;

    void SetRemoteOutVolume(int remote, f32 volume);
    f32 GetRemoteOutVolume(int remote) const;

    f32 GetRemoteSend(int remote) const;
    f32 GetRemoteFxSend(int remote) const;

    int GetRemoteFilter() const {
        return mPlayerParamSet.remoteFilter;
    }
    void SetRemoteFilter(int filter) {
        mPlayerParamSet.remoteFilter = filter;
    }

    PanMode GetPanMode() const {
        return mPlayerParamSet.panMode;
    }
    void SetPanMode(PanMode mode) {
        mPlayerParamSet.panMode = mode;
    }

    PanCurve GetPanCurve() const {
        return mPlayerParamSet.panCurve;
    }
    void SetPanCurve(PanCurve curve) {
        mPlayerParamSet.panCurve = curve;
    }

    const VoiceOutParam& GetVoiceOutParam(int index) const {
        return mPlayerParamSet.voiceOutParam[index];
    }

    void SetVoiceOutParam(int index, const VoiceOutParam& param) {
        mPlayerParamSet.voiceOutParam[index] = param;
    }

private:
    PlayerParamSet mPlayerParamSet; // at 0x4
    u32 mId;                        // at 0xB0
};

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
