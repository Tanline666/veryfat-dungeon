#ifndef NW4R_SND_SOUND_ACTOR_H
#define NW4R_SND_SOUND_ACTOR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_ExternalSoundPlayer.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>

#include <climits>

namespace nw4r {
namespace snd {

// Forward declarations
class SoundHandle;

class SoundActor : public SoundStartable {
    typedef detail::ExternalSoundPlayer ActorPlayer;

public:
    static const int ACTOR_PLAYER_COUNT = 4;
    SoundActor(SoundArchivePlayer& rArcPlayer);
    virtual ~SoundActor();

    const detail::SoundActorParam& detail_GetActorParam() const {
        return mActorParam;
    }

protected:
    virtual StartResult SetupSound(SoundHandle* pHandle, u32 id,
                                   const StartInfo* pStartInfo,
                                   void* pArg); // at 0x10
    virtual StartResult detail_SetupSoundWithAmbientInfo(
        SoundHandle* pHandle, u32 id, const StartInfo* pStartInfo,
        detail::BasicSound::AmbientArgInfo* pAmbInfo, void* pArg); // at 0x14

private:
    virtual StartResult
    detail_SetupSound(SoundHandle* pHandle, u32 id, bool hold,
                      const StartInfo* pStartInfo); // at 0x18

    virtual u32
    detail_ConvertLabelStringToSoundId(const char* pLabel); // at 0x1C

    ActorPlayer* detail_GetActorSoundPlayer(int idx) {
        if (idx < 0 || idx >= ACTOR_PLAYER_COUNT) {
            return NULL;
        }

        return &mActorPlayer[idx];
    }

    template <class Function>
    inline Function ForEachSound(Function function, bool reverse = FALSE) {
        for (int actorPlayerIndex = 0; actorPlayerIndex < ACTOR_PLAYER_COUNT;
             actorPlayerIndex++) {
            mActorPlayer[actorPlayerIndex].ForEachSound(function, reverse);
        }

        return function;
    }

private:
    SoundArchivePlayer& mSoundArchivePlayer;      // at 0x4
    ActorPlayer mActorPlayer[ACTOR_PLAYER_COUNT]; // at 0x8
    detail::SoundActorParam mActorParam;          // at 0x58
};
// namespace detail
} // namespace snd
} // namespace nw4r

#endif
