#ifndef NW4R_SND_SOUND_3D_MANAGER_H
#define NW4R_SND_SOUND_3D_MANAGER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math.h>
#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_InstancePool.h>
#include <nw4r/snd/snd_Sound3DListener.h>
#include <nw4r/snd/snd_SoundArchive.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace snd {

// Forward declarations
class Sound3DManager;
class SoundHandle;
struct SoundParam;

struct Sound3DParam {
    math::VEC3 position; // at 0x0
    math::VEC3 velocity; // at 0xC
    u32 ctrl;            // at 0x18
    u8 decayCurve;       // at 0x1C
    u8 decayRatio;       // at 0x1D
    u8 dopplerFactor;    // at 0x1E
    u32 actorUserParam;  // at 0x20
    u32 soundUserParam;  // at 0x24

    Sound3DParam();
};

namespace detail {
class Sound3DEngineInterface {
public:
    virtual ~Sound3DEngineInterface() {} // at 0xC
    virtual void
    UpdateAmbientParam(const Sound3DManager* pManager,
                       const Sound3DParam* pActorParam, u32 id,
                       int voiceOutCount,
                       SoundAmbientParam* pAmbParam) = 0; // at 0x10
    virtual int GetAmbientPriority(const Sound3DManager* pManager,
                                   const Sound3DParam* pParam,
                                   u32 id) = 0; // at 0x14
    virtual int GetRequiredVoiceOutCount(const Sound3DManager* pManager,
                                         const Sound3DParam* pParam,
                                         u32 id) = 0; // at 0x18
};
} // namespace detail

class Sound3DManager : public detail::BasicSound::AmbientParamUpdateCallback,
                       public detail::BasicSound::AmbientArgAllocaterCallback {
public:
    NW4R_UT_LINKLIST_TYPEDEF_DECL(Sound3DListener);

public:
    Sound3DManager();

    u32 GetRequiredMemSize(const SoundArchive* pArchive);
    bool Setup(const SoundArchive* pArchive, void* pBuffer, u32 size);

    void AddListener(Sound3DListener* pListener) {
        mListenerList.PushBack(pListener);
    }
    void RemoveListener(Sound3DListener* pListener) {
        mListenerList.Erase(pListener);
    }
    const Sound3DListenerList& GetListenerList() const {
        return mListenerList;
    }

    int GetMaxPriorityReduction() const {
        return mMaxPriorityReduction;
    }
    void SetMaxPriorityReduction(int max) {
        mMaxPriorityReduction = max;
    }

    f32 GetPanRange() const {
        return mPanRange;
    }

private:
    enum ParamDecayCurve {
        DECAY_CURVE_NONE,
        DECAY_CURVE_LOGARITHMIC,
        DECAY_CURVE_LINEAR,
    };

private:
    virtual void detail_UpdateAmbientParam(const void* pArg, u32 id,
                                           int voiceOutCount,
                                           SoundAmbientParam* pParam);
    virtual int detail_GetAmbientPriority(const void* pArg, u32 id);
    virtual int detail_GetRequiredVoiceOutCount(const void* pArg, u32 id);
    virtual void* detail_AllocAmbientArg(u32 size);
    virtual void detail_FreeAmbientArg(void* pArg,
                                       const detail::BasicSound* pSound);

private:
    detail::InstancePool<Sound3DParam> mParamPool; // at 0x8
    Sound3DListenerList mListenerList;             // at 0xC
    detail::Sound3DEngineInterface* mpEngine;      // at 0x18

    s32 mMaxPriorityReduction; // at 0x1C
    f32 mPanRange;             // at 0x20
    f32 mSonicVelocity;        // at 0x24
    f32 mBiquadFilterType;     // at 0x28
};

} // namespace snd
} // namespace nw4r

#endif
