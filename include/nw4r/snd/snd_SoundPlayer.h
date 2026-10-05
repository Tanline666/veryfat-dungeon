#ifndef NW4R_SND_SOUND_PLAYER_H
#define NW4R_SND_SOUND_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_PlayerHeap.h>

#include <revolution/OS.h>
#include <revolution/WPAD.h>

namespace nw4r {
namespace snd {

// Forward declarations
namespace detail {
class ExternalSoundPlayer;
class SeqSound;
template <typename T> class SoundInstanceManager;
class StrmSound;
class WaveSound;
} // namespace detail

class SoundPlayer {
public:
    SoundPlayer();
    ~SoundPlayer();

    void InitParam();
    void Update();

    void StopAllSound(int frames);
    void PauseAllSound(bool flag, int frames);

    void SetVolume(f32 volume);

    void SetLpfFreq(f32 lpfFreq);
    f32 GetLpfFreq() const {
        return mLpfFreq;
    }

    void SetBiquadFilter(int type, f32 value);
    int GetBiquadFilterType() const {
        return mBiquadFilterType;
    }
    f32 GetBiquadFilterValue() const {
        return mBiquadFilterValue;
    }

    int detail_GetOutputLine() const;
    bool detail_IsEnabledOutputLine() const;
    int GetDefaultOutputLine() const {
        return mOutputLineFlag;
    }

    void detail_InsertSoundList(detail::BasicSound* pSound);
    void detail_RemoveSoundList(detail::BasicSound* pSound);

    void detail_InsertPriorityList(detail::BasicSound* pSound);
    void detail_RemovePriorityList(detail::BasicSound* pSound);

    void detail_SortPriorityList(detail::BasicSound* pSound);
    void detail_SortPriorityList();

    detail::SeqSound* detail_AllocSeqSound(
        int priority, int startPriority,
        detail::BasicSound::AmbientInfo* pArgInfo,
        detail::ExternalSoundPlayer* pExtPlayer, u32 id,
        detail::SoundInstanceManager<detail::SeqSound>* pManager);

    detail::StrmSound* detail_AllocStrmSound(
        int priority, int startPriority,
        detail::BasicSound::AmbientInfo* pArgInfo,
        detail::ExternalSoundPlayer* pExtPlayer, u32 id,
        detail::SoundInstanceManager<detail::StrmSound>* pManager);

    detail::WaveSound* detail_AllocWaveSound(
        int priority, int startPriority,
        detail::BasicSound::AmbientInfo* pArgInfo,
        detail::ExternalSoundPlayer* pExtPlayer, u32 id,
        detail::SoundInstanceManager<detail::WaveSound>* pManager);

    int CalcPriorityReduction(detail::BasicSound::AmbientInfo* pArgInfo,
                              u32 id);

    void InitAmbientArg(detail::BasicSound* pSound,
                        detail::BasicSound::AmbientInfo* pArgInfo);

    void SetPlayableSoundCount(int count);
    void detail_SetPlayableSoundLimit(int limit);

    bool CheckPlayableSoundCount(int startPriority,
                                 detail::ExternalSoundPlayer* pExtPlayer);

    void detail_AppendPlayerHeap(detail::PlayerHeap* pHeap);
    detail::PlayerHeap* detail_AllocPlayerHeap(detail::BasicSound* pSound);
    void detail_FreePlayerHeap(detail::BasicSound* pSound);

    int GetPlayingSoundCount() const {
        return mSoundList.GetSize();
    }
    int GetPlayableSoundCount() const {
        return mPlayableCount;
    }

    f32 GetVolume() const {
        return mVolume;
    }

    void SetMainOutVolume(f32 volume);
    f32 GetMainOutVolume() const {
        return mMainOutVolume;
    }

    void SetRemoteOutVolume(int remoteIndex, f32 volume);
    f32 GetRemoteOutVolume(int remoteIndex) const;

    void SetMainSend(f32 send);
    f32 GetMainSend() const {
        return mMainSend;
    }

    void SetFxSend(AuxBus bus, f32 send);
    f32 GetFxSend(AuxBus bus) const {
        return mFxSend[bus];
    }

    void detail_RemoveSound(detail::BasicSound* sound);

private:
    detail::BasicSound* detail_GetLowestPrioritySound() {
        // @bug UB when the list is empty
        return &mPriorityList.GetFront();
    }

    detail::BasicSoundPlayerPlayList mSoundList;    // at 0x0
    detail::BasicSoundPlayerPrioList mPriorityList; // at 0xC
    detail::PlayerHeapList mHeapList;               // at 0x18

    u16 mPlayableCount; // at 0x24
    u16 mPlayableLimit; // at 0x26

    f32 mVolume;                                // at 0x28
    f32 mLpfFreq;                               // at 0x2C
    int mOutputLineFlag;                        // at 0x30
    f32 mMainOutVolume;                         // at 0x34
    int mBiquadFilterType;                      // at 0x38
    int mBiquadFilterValue;                     // at 0x3C
    f32 mRemoteOutVolume[WPAD_MAX_CONTROLLERS]; // at 0x40
    f32 mMainSend;                              // at 0x50
    f32 mFxSend[AUX_BUS_NUM];                   // at 0x54
};

} // namespace snd
} // namespace nw4r

#endif
