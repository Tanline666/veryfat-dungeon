#include <nw4r/snd.h>
#include <nw4r/ut.h>

#include <cstring>

namespace nw4r {
namespace snd {

SoundPlayer::SoundPlayer()
    : mPlayableCount(1),
      mPlayableLimit(INT_MAX),
      mVolume(1.0f),
      mLpfFreq(0.0f),
      mOutputLineFlag(OUTPUT_LINE_MAIN),
      mMainOutVolume(1.0f),
      mMainSend(0.0f),
      mBiquadType(BIQUAD_FILTER_TYPE_NONE),
      mBiquadValue(0.0f) {
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mRemoteOutVolume[i] = 1.0f;
    }
    for (int i = 0; i < AUX_BUS_NUM; i++) {
        mFxSend[i] = 0.0f;
    }
}

SoundPlayer::~SoundPlayer() {
    StopAllSound(0);
}

void SoundPlayer::Update() {
    detail::SoundThread::AutoLock lock;

    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mSoundList, { it->Update(); })

    detail_SortPriorityList();
}

void SoundPlayer::StopAllSound(int fadeFrames) {
    detail::SoundThread::AutoLock lock;

    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mSoundList, { it->Stop(fadeFrames); })
}

void SoundPlayer::PauseAllSound(bool flag, int fadeFrames) {
    detail::SoundThread::AutoLock lock;

    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mSoundList,
                                   { it->Pause(flag, fadeFrames); })
}

void SoundPlayer::SetVolume(f32 volume) {
    NW4R_ASSERT(volume >= 0.0f, 156);

    if (volume < 0.0f) {
        volume = 0.0f;
    }
    mVolume = volume;
}

f32 SoundPlayer::GetRemoteOutVolume(int remoteIndex) const {
    return mRemoteOutVolume[remoteIndex];
}

void SoundPlayer::RemoveSoundList(detail::BasicSound* sound) {
    detail::SoundThread::AutoLock lock;

    mSoundList.Erase(sound);
    sound->DetachSoundPlayer(this);
}

void SoundPlayer::InsertPriorityList(detail::BasicSound* sound) {
    detail::SoundThread::AutoLock lock;

    detail::BasicSoundPlayerPrioList::Iterator it =
        mPriorityList.GetBeginIter();

    for (; it != mPriorityList.GetEndIter(); ++it) {
        if (sound->CalcCurrentPlayerPriority() <
            it->CalcCurrentPlayerPriority()) {
            break;
        }
    }

    mPriorityList.Insert(it, sound);
}

void SoundPlayer::RemovePriorityList(detail::BasicSound* pSound) {
    detail::SoundThread::AutoLock lock;
    mPriorityList.Erase(pSound);
}

void SoundPlayer::detail_SortPriorityList(detail::BasicSound* sound) {
    RemovePriorityList(sound);
    InsertPriorityList(sound);
}

void SoundPlayer::detail_SortPriorityList() {
    detail::SoundThread::AutoLock lock;

    if (mPriorityList.GetSize() < 2) {
        return;
    }

    //! @~english TMP_NUM is not linked, but it is required for accurate symbol
    //! names
    //! @~japanese TMP_NUMはリンクしていないけど、正確なシンボル名のために必要だ
    static const int TMP_NUM = detail::BasicSound::PRIORITY_MAX + 1;
    static detail::BasicSoundPlayerPrioList tmplist[TMP_NUM];

    while (!mPriorityList.IsEmpty()) {
        detail::BasicSound& front = mPriorityList.GetFront();
        mPriorityList.PopFront();
        tmplist[front.CalcCurrentPlayerPriority()].PushBack(&front);
    }

    for (int i = 0; i < TMP_NUM; i++) {
        while (!tmplist[i].IsEmpty()) {
            detail::BasicSound& front = tmplist[i].GetFront();
            tmplist[i].PopFront();
            mPriorityList.PushBack(&front);
        }
    }
}

bool SoundPlayer::detail_AppendSound(detail::BasicSound* sound) {
    detail::SoundThread::AutoLock lock;

    int allocPriority = sound->CalcCurrentPlayerPriority();

    if (!GetPlayableSoundCount()) {
        return FALSE;
    }

    while (GetPlayingSoundCount() >= GetPlayableSoundCount()) {
        detail::BasicSound* dropSound = GetLowestPrioritySound();
        if (!dropSound) {
            return FALSE;
        }
        if (allocPriority < dropSound->CalcCurrentPlayerPriority()) {
            return FALSE;
        }
        dropSound->Shutdown();
    }

    mSoundList.PushBack(sound);
    InsertPriorityList(sound);
    sound->AttachSoundPlayer(this);

    return TRUE;
}

void SoundPlayer::detail_RemoveSound(detail::BasicSound* sound) {
    RemovePriorityList(sound);
    RemoveSoundList(sound);
}

void SoundPlayer::InitAmbientArg(detail::BasicSound* pSound,
                                 detail::BasicSound::AmbientInfo* pArgInfo) {

    if (pArgInfo == NULL) {
        return;
    }

    void* pExtArg = pArgInfo->argAllocaterCallback->detail_AllocAmbientArg(
        pArgInfo->argSize);

    if (pExtArg == NULL) {
        return;
    }

    std::memcpy(pExtArg, pArgInfo->arg, pArgInfo->argSize);

    pSound->SetAmbientParamCallback(pArgInfo->paramUpdateCallback,
                                    pArgInfo->argUpdateCallback,
                                    pArgInfo->argAllocaterCallback, pExtArg);
}

void SoundPlayer::SetPlayableSoundCount(int count) {
    detail::SoundThread::AutoLock lock;

    NW4R_WARNING(count <= mPlayableLimit, 461,
                 "playable sound count is over limit.");
    count = ut::Clamp(count, 0, mPlayableLimit);
    mPlayableCount = count;

    while (GetPlayingSoundCount() > GetPlayableSoundCount()) {
        detail::BasicSound* dropSound = GetLowestPrioritySound();
        NW4R_NULL_ASSERT(dropSound, 470);
        dropSound->Shutdown();
    }
}

void SoundPlayer::detail_SetPlayableSoundLimit(int limit) {
    NW4R_ASSERT(limit >= 0, 486);
    mPlayableLimit = limit;
}

bool SoundPlayer::detail_CanPlaySound(int startPriority) {
    detail::SoundThread::AutoLock lock;

    if (!GetPlayableSoundCount()) {
        return FALSE;
    }
    if (GetPlayingSoundCount() >= GetPlayableSoundCount()) {
        detail::BasicSound* dropSound = GetLowestPrioritySound();
        if (!dropSound) {
            return FALSE;
        }
        if (startPriority < dropSound->CalcCurrentPlayerPriority()) {
            return FALSE;
        }
    }

    return TRUE;
}

bool SoundPlayer::CheckPlayableSoundCount(
    int startPriority, detail::ExternalSoundPlayer* pExtPlayer) {

    detail::SoundThread::AutoLock lock;

    if (GetPlayableSoundCount() == 0) {
        return false;
    }

    while (GetPlayingSoundCount() >= GetPlayableSoundCount()) {
        detail::BasicSound* pDropSound = GetLowestPrioritySound();

        if (pDropSound == NULL) {
            return false;
        }

        if (startPriority < pDropSound->CalcCurrentPlayerPriority()) {
            return false;
        }

        pDropSound->Shutdown();
    }

    if (pExtPlayer != NULL) {
        if (pExtPlayer->GetPlayableSoundCount() == 0) {
            return false;
        }

        while (pExtPlayer->GetPlayingSoundCount() >=
               pExtPlayer->GetPlayableSoundCount()) {

            detail::BasicSound* pDropSound =
                pExtPlayer->GetLowestPrioritySound();

            if (pDropSound == NULL) {
                return false;
            }

            if (startPriority < pDropSound->CalcCurrentPlayerPriority()) {
                return false;
            }

            pDropSound->Shutdown();
        }
    }

    return true;
}

void SoundPlayer::detail_AppendPlayerHeap(detail::PlayerHeap* heap) {
    detail::SoundThread::AutoLock lock;

    heap->AttachSoundPlayer(this);
    mHeapList.PushBack(heap);
}

detail::PlayerHeap*
SoundPlayer::detail_AllocPlayerHeap(detail::BasicSound* sound) {
    detail::SoundThread::AutoLock lock;

    if (mHeapList.IsEmpty()) {
        return NULL;
    }

    detail::PlayerHeap& playerHeap = mHeapList.GetFront();
    mHeapList.PopFront();

    playerHeap.AttachSound(sound);
    sound->AttachPlayerHeap(&playerHeap);
    playerHeap.Clear();

    return &playerHeap;
}

void SoundPlayer::detail_FreePlayerHeap(detail::BasicSound* sound) {
    detail::SoundThread::AutoLock lock;

    detail::PlayerHeap* heap = sound->GetPlayerHeap();

    if (!heap) {
        return;
    }

    heap->DetachSound(sound);
    sound->DetachPlayerHeap(heap);
    mHeapList.PushBack(heap);
}

} // namespace snd
} // namespace nw4r
