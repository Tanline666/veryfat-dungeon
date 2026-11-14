#include <nw4r/snd.h>

namespace nw4r {
namespace snd {

SoundActor::SoundActor(SoundArchivePlayer& soundArchivePlayer)
    : mSoundArchivePlayer(soundArchivePlayer) {
    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++) {
        const int playableSoundCount = (i == NULL) ? INT_MAX : 1;
        mActorPlayer[i].SetPlayableSoundCount(playableSoundCount);
    }
}

SoundActor::~SoundActor() {
    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++) {
        mActorPlayer[i].DetachSoundActorAll(this);
    }
}

SoundStartable::StartResult SoundActor::SetupSound(SoundHandle* handle,
                                                   u32 soundId,
                                                   const StartInfo* startInfo,
                                                   void* setupArg) {

    const SetupInfo* setupInfo = reinterpret_cast<SetupInfo*>(setupArg);

    return mSoundArchivePlayer.detail_SetupSoundImpl(
        handle, soundId, NULL, this, setupInfo->holdFlag, startInfo);
}

SoundStartable::StartResult SoundActor::detail_SetupSoundWithAmbientInfo(
    SoundHandle* handle, u32 soundId, const StartInfo* startInfo,
    detail::BasicSound::AmbientInfo* ambientInfo, void* setupArg) {

    NW4R_NULL_ASSERT(setupArg, 160);

    const SetupInfo* setupInfo = reinterpret_cast<SetupInfo*>(setupArg);

    return mSoundArchivePlayer.detail_SetupSoundImpl(
        handle, soundId, ambientInfo, this, setupInfo->holdFlag, startInfo);
}

SoundStartable::StartResult
SoundActor::detail_SetupSound(SoundHandle* handle, u32 soundId, bool holdFlag,
                              const StartInfo* startInfo) {
    SetupInfo setupInfo;
    setupInfo.holdFlag = holdFlag;

    return SetupSound(handle, soundId, startInfo, &setupInfo);
}

u32 SoundActor::detail_ConvertLabelStringToSoundId(const char* label) {
    return mSoundArchivePlayer.detail_ConvertLabelStringToSoundId(label);
}

} // namespace snd
} // namespace nw4r
