#ifndef NW4R_SND_EXTERNAL_SOUND_PLAYER_H
#define NW4R_SND_EXTERNAL_SOUND_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/BasicSound.h>
#include <nw4r/snd/SoundHandle.h>

namespace nw4r {
namespace snd {
namespace detail {

class ExternalSoundPlayer {
public:
    ExternalSoundPlayer();
    ~ExternalSoundPlayer();

    int GetPlayableSoundCount() const {
        return mPlayableCount;
    }
    void SetPlayableSoundCount(int count);

    int GetPlayingSoundCount() const {
        return mSoundList.GetSize();
    }

    BasicSound* GetLowestPrioritySound();

    void AppendSound(BasicSound* pSound);
    void RemoveSound(BasicSound* pSound);

    template <typename TForEachFunc>
    TForEachFunc ForEachSound(TForEachFunc pFunc, bool reverse) {
        if (reverse) {
            BasicSoundExtPlayList::RevIterator it =
                mSoundList.GetBeginReverseIter();

            while (it != mSoundList.GetEndReverseIter()) {
                BasicSoundExtPlayList::RevIterator curr = it;

                SoundHandle handle;
                handle.detail_AttachSoundAsTempHandle(&*curr);
                pFunc(handle);

                if (handle.IsAttachedSound()) {
                    ++it;
                }
            }
        } else {
            NW4R_UT_LINKLIST_FOREACH_SAFE(it, mSoundList, {
                SoundHandle handle;
                handle.detail_AttachSoundAsTempHandle(&*it);
                pFunc(handle);
            });
        }

        return pFunc;
    }

    void DetachSoundActorAll(SoundActor* actor);

private:
    BasicSoundExtPlayList mSoundList; // at 0x0
    u16 mPlayableCount;               // at 0xC
};

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
