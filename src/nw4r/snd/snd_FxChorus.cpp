#include <nw4r/snd.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace snd {

FxChorus::FxChorus() : FxBase(), mActive(FALSE) {
    const ChorusParam param;
    SetParam(param);
}

u32 FxChorus::GetRequiredMemSize() {
    u32 requiredSize = AXFXChorusExpGetMemSize(&mAxfxParam);
    return ut::RoundUp(requiredSize + detail::AxfxImpl::HEAP_SIZE_MIN, 32);
}

bool FxChorus::AssignWorkBuffer(void* pBuffer, u32 size) {
    return mImpl.CreateHeap(pBuffer, size);
}

void FxChorus::ReleaseWorkBuffer() {
    mImpl.DestroyHeap();
}

bool FxChorus::StartUp() {
    u32 memSize = GetRequiredMemSize();
    if (memSize > mImpl.GetHeapTotalSize()) {
        return false;
    }

    AXFXAllocHook alloc;
    AXFXFreeHook free;
    mImpl.HookAlloc(&alloc, &free);
    int result = AXFXChorusExpInit(&mAxfxParam);
    u32 allocatedSize = mImpl.RestoreAlloc(alloc, free);

    //! Variable is debug leftover
    u32 requiredMemSize = GetRequiredMemSize();

    mActive = TRUE;
    return result;
}

void FxChorus::Shutdown() {

    if (!mActive) {
        return;
    }

    mActive = FALSE;

    AXFXAllocHook alloc;
    AXFXFreeHook free;
    mImpl.HookAlloc(&alloc, &free);

    AXFXChorusExpShutdown(&mAxfxParam);

    mImpl.RestoreAlloc(alloc, free);
}

bool FxChorus::SetParam(const ChorusParam& param) {
    DECOMP_I_BOMB;
    mParam = param;

    mAxfxParam.delayTime = ut::Clamp(param.delayTime, 0.1f, 50.0f);
    mAxfxParam.depth = ut::Clamp(param.depth, 0.0f, 1.0f);
    mAxfxParam.rate = ut::Clamp(param.rate, 0.1f, 2.0f);
    mAxfxParam.feedback = ut::Clamp(param.feedback, 0.0f, 0.99f);
    mAxfxParam.outGain = ut::Clamp(param.outGain, 0.0f, 1.0f);

    mAxfxParam.busIn = NULL;
    mAxfxParam.busOut = NULL;
    mAxfxParam.sendGain = 0.0f;

    if (!mActive) {
        return TRUE;
    }

    if (GetRequiredMemSize() > mImpl.GetHeapTotalSize()) {
        return FALSE;
    }

    int result = AXFXChorusExpSettingsUpdate(&mAxfxParam);

    return result;
}

void FxChorus::UpdateBuffer(int numChannels, void* buffer[], u32 bufferSize,
                            SampleFormat format, f32 sampleRate,
                            OutputMode mode) {
#pragma unused(sampleRate)

    NW4R_ASSERT(numChannels >= 3, 246);
    NW4R_ASSERT(bufferSize == AUX_BUFFER_SIZE, 247);
    NW4R_ASSERT(format == SAMPLE_FORMAT_PCM_S32, 248);

    if (!mActive) {
        return;
    }

    if (mode == OUTPUT_MODE_DPL2) {
        return;
    }

    (void)numChannels;
    (void)bufferSize;
    (void)format;

    AXFX_BUFFERUPDATE axfxbuf = {static_cast<s32*>(buffer[0]),
                                 static_cast<s32*>(buffer[1]),
                                 static_cast<s32*>(buffer[2])};

    AXFXChorusExpCallback(&axfxbuf, &mAxfxParam);
}

} // namespace snd
} // namespace nw4r
