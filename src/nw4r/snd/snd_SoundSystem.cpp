#include <nw4r/snd.h>

#include <revolution/AX.h>
#include <revolution/OS.h>
#include <revolution/SC.h>

NW4R_LIB_VERSION(SND, "Jun 25 2009", "15:01:07", "0x4302_145");

#define DEFAULT_WORK_SIZE                                                      \
    (DEFAULT_SOUND_THREAD_STACK_SIZE + DEFAULT_DVD_THREAD_STACK_SIZE +         \
     detail::AxVoiceManager::WORK_SIZE_MAX +                                   \
     detail::VoiceManager::WORK_SIZE_MAX +                                     \
     detail::ChannelManager::WORK_SIZE_MAX)

namespace nw4r {
namespace snd {

namespace {
static bool sInitialized = false;
} // namespace

detail::TaskThread SoundSystem::sTaskThread;
int SoundSystem::sMaxVoices;

void SoundSystem::InitSoundSystem(s32 soundThreadPriority,
                                  s32 dvdThreadPriority) {

    static u8 defaultSoundSystemWork[DEFAULT_WORK_SIZE] ALIGN(32);

    SoundSystemParam param;
    param.soundThreadPriority = soundThreadPriority;
    param.dvdThreadPriority = dvdThreadPriority;

    //! @bug This function ignores the specified buffer size
    InitSoundSystem(param, defaultSoundSystemWork,
                    sizeof(defaultSoundSystemWork));
}

u32 SoundSystem::GetRequiredMemSize(const SoundSystemParam& param) {
    NW4R_ASSERT(param.soundThreadStackSize % 8 == 0, 106);
    NW4R_ASSERT(param.dvdThreadStackSize % 8 == 0, 107);

    int maxVoices = AXGetMaxVoices();

    return param.soundThreadStackSize + param.dvdThreadStackSize +
           detail::AxVoiceManager::GetInstance().GetRequiredMemSize(maxVoices) +
           detail::VoiceManager::GetInstance().GetRequiredMemSize(maxVoices) +
           detail::ChannelManager::GetInstance().GetRequiredMemSize(maxVoices);
}

void SoundSystem::InitSoundSystem(const SoundSystemParam& param, void* workMem,
                                  u32 workMemSize) {
    (void)workMemSize;

    NW4R_ALIGN_ASSERT(workMem, 32, 144);
    NW4R_ASSERT(workMemSize >= GetRequiredMemSize(param), 145);

    if (sInitialized) {
        return;
    }

    sInitialized = TRUE;

    OSRegisterVersion(NW4R_SND_Version_);
    //!< @~english For assertions
    //!< @~japanese アサート用
    bool result;

    detail::AxManager::GetInstance().Init();

    SCInit();
    u32 initStatus;
    do {
        initStatus = SCCheckStatus();
    } while (initStatus == SC_STATUS_BUSY);
    NW4R_ASSERT(initStatus == SC_STATUS_OK, 171);

    switch (SCGetSoundMode()) {
    case SC_SND_MONO: {
        detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_MONO);
        break;
    }

    case SC_SND_STEREO: {
        detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_STEREO);
        break;
    }

    case SC_SND_SURROUND: {
        detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_DPL2);
        break;
    }

    default: {
        detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_STEREO);
        break;
    }
    }

    detail::RemoteSpeakerManager::GetInstance().Setup();

    u8* ptr = static_cast<u8*>(workMem);

    void* pDvdThreadStack = ptr;
    ptr += param.dvdThreadStackSize;

    void* pSoundThreadStack = ptr;
    ptr += param.soundThreadStackSize;

    sMaxVoices = AXGetMaxVoices();

    void* pAxVoiceWork = ptr;
    ptr += detail::AxVoiceManager::GetInstance().GetRequiredMemSize(sMaxVoices);

    detail::AxVoiceManager::GetInstance().Setup(
        pAxVoiceWork,
        detail::AxVoiceManager::GetInstance().GetRequiredMemSize(sMaxVoices));

    void* pVoiceWork = ptr;
    ptr += detail::VoiceManager::GetInstance().GetRequiredMemSize(sMaxVoices);

    detail::VoiceManager::GetInstance().Setup(
        pVoiceWork,
        detail::VoiceManager::GetInstance().GetRequiredMemSize(sMaxVoices));

    void* pChannelWork = ptr;
    ptr += detail::ChannelManager::GetInstance().GetRequiredMemSize(sMaxVoices);

    detail::ChannelManager::GetInstance().Setup(
        pChannelWork,
        detail::ChannelManager::GetInstance().GetRequiredMemSize(sMaxVoices));

    detail::SeqPlayer::InitSeqPlayer();

    result = sTaskThread.Create(param.dvdThreadPriority, pDvdThreadStack,
                                param.dvdThreadStackSize);
    NW4R_ASSERT(result, 247);

    result = detail::SoundThread::GetInstance().Create(
        param.soundThreadPriority, pSoundThreadStack,
        param.soundThreadStackSize);
    NW4R_ASSERT(result, 255);

    // clang-format off
    NW4R_ASSERT(ptr <= reinterpret_cast<u8*>(workMem) + workMemSize, 257);
    // clang-format on
}

void SoundSystem::ShutdownSoundSystem() {
    if (!sInitialized) {
        return;
    }

    detail::SoundThread::GetInstance().Shutdown();
    detail::TaskManager::GetInstance().CancelAllTask();
    sTaskThread.Destroy();

    detail::RemoteSpeakerManager::GetInstance().Shutdown();
    detail::ChannelManager::GetInstance().Shutdown();
    detail::VoiceManager::GetInstance().Shutdown();
    detail::AxVoiceManager::GetInstance().Shutdown();
    detail::AxManager::GetInstance().Shutdown();

    sInitialized = false;
}

bool SoundSystem::IsInitializedSoundSystem() {
    return sInitialized;
}

void SoundSystem::WaitForResetReady() {
    if (!sInitialized) {
        return;
    }

    u32 start = OSGetTick();

    while (!detail::AxManager::GetInstance().IsResetReady()) {
        if (OS_TICKS_TO_SEC(OSGetTick() - start) > 0) {
            NW4R_WARNING(FALSE, 329,
                         "SoundSystem::WaitForResetReady is TIME OUT.\n");
            break;
        }
    }
}

} // namespace snd
} // namespace nw4r
