#ifndef NW4R_SND_BASIC_SOUND_H
#define NW4R_SND_BASIC_SOUND_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_MoveValue.h>
#include <nw4r/snd/snd_Types.h>
#include <nw4r/ut.h>

#include <revolution/WPAD.h>

namespace nw4r {
namespace snd {

// Forward declarations
class SoundHandle;
class SoundPlayer;
class SoundActor;

namespace detail {
class BasicPlayer;
class ExternalSoundPlayer;
class PlayerHeap;
} // namespace detail

namespace detail {

class BasicSound {
    friend class nw4r::snd::SoundHandle;

public:
    NW4R_UT_RTTI_DECL(BasicSound);

    class AmbientParamUpdateCallback {
    public:
        virtual ~AmbientParamUpdateCallback() {} // at 0xC
        virtual void
        detail_UpdateAmbientParam(const void* pArg, u32 id, int voiceOutCount,
                                  SoundAmbientParam* pAmbParam) = 0; // at 0x10
        virtual int detail_GetAmbientPriority(const void* pArg,
                                              u32 id) = 0; // at 0x14
        virtual int detail_GetRequiredVoiceOutCount(const void* pArg,
                                                    u32 id) = 0; // at 0x18
    };

    class AmbientArgUpdateCallback {
    public:
        virtual ~AmbientArgUpdateCallback() {} // at 0xC
        virtual void
        detail_UpdateAmbientArg(void* pArg,
                                const BasicSound* pSound) = 0; // at 0x10
    };

    struct AmbientArgAllocaterCallback {
        virtual void* detail_AllocAmbientArg(u32 size) = 0; // at 0xC

        virtual void
        detail_FreeAmbientArg(void* pArg,
                              const BasicSound* pSound) = 0; // at 0x10
    };

    struct AmbientArgInfo {
        AmbientParamUpdateCallback* paramUpdateCallback;   // at 0x0
        AmbientArgUpdateCallback* argUpdateCallback;       // at 0x4
        AmbientArgAllocaterCallback* argAllocaterCallback; // at 0x8
        void* arg;                                         // at 0xC
        u32 argSize;                                       // at 0x10
    };

    static const u32 INVALID_ID = 0xFFFFFFFF;
    static const int PRIORITY_MAX = 127;

public:
    BasicSound(int priority, int ambientPriority);
    virtual ~BasicSound() {} // at 0xC

    void Update();
    void StartPrepared();
    void Stop(int frames);
    void Pause(bool flag, int frames);
    void SetAutoStopCounter(int count);
    void FadeIn(int frames);
    virtual void Shutdown();             // at 0x10
    virtual bool IsPrepared() const = 0; // at 0x14
    bool IsPause() const;

    void SetInitialVolume(f32 vol);
    void SetVolume(f32 vol, int frames);
    void SetPitch(f32 pitch);
    void SetPan(f32 pan);
    void SetSurroundPan(f32 pan);
    void SetLpfFreq(f32 freq);
    void SetPlayerPriority(int priority);
    void SetRemoteFilter(int filter);
    void SetPanMode(PanMode mode);
    void SetPanCurve(PanCurve curve);

    virtual bool IsAttachedTempSpecialHandle() = 0; // at 0x18
    virtual void DetachTempSpecialHandle() = 0;     // at 0x1C

    virtual void InitParam();                              // at 0x20
    virtual BasicPlayer& GetBasicPlayer() = 0;             // at 0x24
    virtual const BasicPlayer& GetBasicPlayer() const = 0; // at 0x28

    virtual void OnUpdatePlayerPriority() {} // at 0x2C
    virtual void UpdateMoveValue();          // at 0x30
    virtual void UpdateParam();              // at 0x34

    PlayerHeap* GetPlayerHeap() {
        return mHeap;
    }
    void SetPlayerHeap(PlayerHeap* pHeap) {
        mHeap = pHeap;
    }

    bool IsAttachedGeneralHandle();
    void DetachGeneralHandle();

    bool IsAttachedTempGeneralHandle();
    void DetachTempGeneralHandle();

    SoundPlayer* GetSoundPlayer() {
        return mSoundPlayer;
    }
    void SetSoundPlayer(SoundPlayer* pPlayer) {
        mSoundPlayer = pPlayer;
    }

    ExternalSoundPlayer* GetExternalSoundPlayer() {
        return mExtSoundPlayer;
    }
    void SetExternalSoundPlayer(ExternalSoundPlayer* pExtPlayer) {
        mExtSoundPlayer = pExtPlayer;
    }

    AmbientParamUpdateCallback* GetAmbientParamUpdateCallback() {
        return mAmbientInfo.paramUpdateCallback;
    }

    AmbientArgUpdateCallback* GetAmbientArgUpdateCallback() {
        return mAmbientInfo.argUpdateCallback;
    }
    void ClearAmbientArgUpdateCallback() {
        mAmbientInfo.argUpdateCallback = NULL;
    }

    AmbientArgAllocaterCallback* GetAmbientArgAllocaterCallback() {
        return mAmbientInfo.argAllocaterCallback;
    }

    void* GetAmbientArg() {
        return mAmbientInfo.arg;
    }

    SoundParam& GetAmbientParam() {
        return mAmbientParam;
    }

    void SetAmbientParamCallback(AmbientParamUpdateCallback* pParamUpdate,
                                 AmbientArgUpdateCallback* pArgUpdate,
                                 AmbientArgAllocaterCallback* pArgAlloc,
                                 void* pArg);

    void SetPriority(int priority) {
        mPriority = priority;
    }

    u32 GetId() const {
        return mId;
    }
    void SetId(u32 id);

    f32 GetMoveVolume() {
        return mExtMoveVolume.GetValue();
    }

    f32 GetInitialVolume() const;
    f32 GetPan() const;
    f32 GetSurroundPan() const;
    f32 GetPitch() const;

    void SetOutputLine(int flag);
    bool IsEnabledOutputLine() const;
    int GetOutputLine() const;

    f32 GetMainOutVolume() const;
    void SetMainOutVolume(f32 vol);

    f32 GetRemoteOutVolume(int remote) const;
    void SetRemoteOutVolume(int remote, f32 vol);

    void SetFxSend(AuxBus bus, f32 send);

    int CalcCurrentPlayerPriority() const {
        return ut::Clamp(mPriority + mAmbientParam.priority, 0, PRIORITY_MAX);
    }

private:
    enum PauseState {
        PAUSE_STATE_NORMAL,
        PAUSE_STATE_PAUSING,
        PAUSE_STATE_PAUSED,
        PAUSE_STATE_UNPAUSING
    };

    PlayerHeap* mHeap;                    // at 0x4
    SoundHandle* mGeneralHandle;          // at 0x8
    SoundHandle* mTempGeneralHandle;      // at 0xC
    SoundPlayer* mSoundPlayer;            // at 0x10
    SoundActor* mSoundActor;              // at 0x14
    ExternalSoundPlayer* mExtSoundPlayer; // at 0x18

    AmbientArgInfo mAmbientInfo; // at 0x1C
    SoundParam mAmbientParam;    // at 0x30
    SoundActorParam mActorParam; // at 0x4C

    MoveValue<f32, int> mFadeVolume;      // at 0x58
    MoveValue<f32, int> mPauseFadeVolume; // at 0x68

    bool mStartFlag;        // at 0x78
    bool mStartedFlag;      // at 0x79
    bool mAutoStopFlag;     // at 0x7A
    bool mFadeOutFlag;      // at 0x7B
    PauseState mPauseState; // at 0x7C
    bool mUnPauseFlag;      // at 0x80

    int mAutoStopCounter; // at 0x84
    u32 mUpdateCounter;   // at 0x88

    u8 mPriority;         // at 0x8C
    u8 mVoiceOutCount;    // at 0x8D
    u8 mBiquadFilterType; // at 0x8E
    u32 mId;              // at 0x90

    MoveValue<f32, int> mExtMoveVolume; // at 0x94
    f32 mInitVolume;                    // at 0xA4
    f32 mExtPan;                        // at 0xA8
    f32 mExtSurroundPan;                // at 0xAC
    f32 mExtPitch;                      // at 0xB0
    f32 mLpfFreq;                       // at 0xB4
    f32 mBiquadFilterValue;             // at 0xB8

    int mOutputLineFlag;                        // at 0xBC
    f32 mMainOutVolume;                         // at 0xC0
    f32 mMainSend;                              // at 0xC4
    f32 mFxSend[AUX_BUS_NUM];                   // at 0xC8
    f32 mRemoteOutVolume[WPAD_MAX_CONTROLLERS]; // at 0xD4

public:
    NW4R_UT_LINKLIST_NODE_DECL_EX(Prio);       // at 0xE4
    NW4R_UT_LINKLIST_NODE_DECL_EX(PlayerPlay); // at 0xEC
    NW4R_UT_LINKLIST_NODE_DECL_EX(PlayerPrio); // at 0xF4
    NW4R_UT_LINKLIST_NODE_DECL_EX(ExtPlay);    // at 0xFC
};

NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, Prio);
NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, PlayerPlay);
NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, PlayerPrio);
NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, ExtPlay);

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
