#ifndef NW4R_SND_SOUND_STARTABLE_H
#define NW4R_SND_SOUND_STARTABLE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>

#define NW4R_SND_DEFINE_START_SOUND_FUNC_INT_VERSION(func, type)               \
    bool func(SoundHandle* pHandle, type id) {                                 \
        return func(pHandle, id);                                              \
    }                                                                          \
    bool func(SoundHandle* pHandle, type id, const StartInfo& rStartInfo) {    \
        return func(pHandle, id, rStartInfo);                                  \
    }                                                                          \
    StartResult func##WithResult(SoundHandle* pHandle, type id) {              \
        return func##WithResult(pHandle, id);                                  \
    }                                                                          \
    StartResult func##WithResult(SoundHandle* pHandle, type id,                \
                                 const StartInfo& rStartInfo) {                \
        return func##WithResult(pHandle, id, rStartInfo);                      \
    }

#define NW4R_SND_DEFINE_START_SOUND_FUNC_CALL_DETAIL(func, type)               \
    bool func(SoundHandle* pHandle, type id) {                                 \
        return detail_##func(pHandle, id, NULL) == START_SUCCESS;              \
    }                                                                          \
    bool func(SoundHandle* pHandle, type id, const StartInfo& rStartInfo) {    \
        return detail_##func(pHandle, id, &rStartInfo) == START_SUCCESS;       \
    }                                                                          \
    StartResult func##WithResult(SoundHandle* pHandle, type id) {              \
        return detail_##func(pHandle, id, NULL);                               \
    }                                                                          \
    StartResult func##WithResult(SoundHandle* pHandle, type id,                \
                                 const StartInfo& rStartInfo) {                \
        return detail_##func(pHandle, id, &rStartInfo);                        \
    }

#define NW4R_SND_DEFINE_START_SOUND_FUNC(func)                                 \
    NW4R_SND_DEFINE_START_SOUND_FUNC_CALL_DETAIL(func, const char*)            \
    NW4R_SND_DEFINE_START_SOUND_FUNC_CALL_DETAIL(func, u32)                    \
    NW4R_SND_DEFINE_START_SOUND_FUNC_INT_VERSION(func, int)                    \
    NW4R_SND_DEFINE_START_SOUND_FUNC_INT_VERSION(func, unsigned int)           \
    NW4R_SND_DEFINE_START_SOUND_FUNC_INT_VERSION(func, s32)

namespace nw4r {
namespace snd {

// Forward declarations
class SoundHandle;

namespace detail {
class ExternalSoundPlayer;
} // namespace detail

class SoundStartable {
public:
    enum StartResult {
        START_SUCCESS,
        START_ERR_LOW_PRIORITY,
        START_ERR_INVALID_LABEL_STRING,
        START_ERR_INVALID_SOUNDID,
        START_ERR_NOT_DATA_LOADED,
        START_ERR_NOT_ENOUGH_PLAYER_HEAP,
        START_ERR_CANNOT_OPEN_FILE,
        START_ERR_NOT_AVAILABLE,
        START_ERR_CANNOT_ALLOCATE_TRACK,
        START_ERR_NOT_ENOUGH_INSTANCE,
        START_ERR_INVALID_PARAMETER,
        START_ERR_INVALID_SEQ_START_LOCATION_LABEL,

        START_ERR_USER = 128,
        START_ERR_UNKNOWN = 255,
    };

    static const char* detail_ConvertStartResultToString(StartResult result);

    struct StartInfo {
        enum EnableFlag {
            ENABLE_START_OFFSET = (1 << 0),
            ENABLE_PLAYER_ID = (1 << 1),
            ENABLE_PLAYER_PRIORITY = (1 << 2)
        };

        enum StartOffsetType {
            START_OFFSET_TYPE_MILLISEC,
            START_OFFSET_TYPE_TICK,
            START_OFFSET_TYPE_SAMPLE
        };

        struct SeqSoundInfo {
            const void* pAddress;    // at 0x0
            const char* pStartLabel; // at 0x4
            SeqSoundInfo() : pAddress(NULL), pStartLabel(NULL) {}
        };

        u32 enableFlag;                  // at 0x0
        StartOffsetType startOffsetType; // at 0x4
        int startOffset;                 // at 0x8
        u32 playerId;                    // at 0xC
        int playerPriority;              // at 0x10
        int actorPlayerId;               // at 0x14
        SeqSoundInfo seqSoundInfo;       // at 0x18

        StartInfo() : enableFlag(NULL) {}
    };

public:
    virtual ~SoundStartable() {} // at 0x8

    NW4R_SND_DEFINE_START_SOUND_FUNC(StartSound)
    NW4R_SND_DEFINE_START_SOUND_FUNC(HoldSound)
    NW4R_SND_DEFINE_START_SOUND_FUNC(PrepareSound)

protected:
    virtual StartResult
    detail_SetupSound(SoundHandle* pHandle, u32 id, bool hold,
                      const StartInfo* pStartInfo) = 0; // at 0xC

    virtual u32
    detail_ConvertLabelStringToSoundId(const char* pLabel) = 0; // at 0x10

private:
    StartResult detail_StartSound(SoundHandle* pHandle, u32 id,
                                  const StartInfo* pStartInfo);

    StartResult detail_StartSound(SoundHandle* pHandle, const char* pName,
                                  const StartInfo* pStartInfo);

    StartResult detail_HoldSound(SoundHandle* pHandle, u32 id,
                                 const StartInfo* pStartInfo);

    StartResult detail_HoldSound(SoundHandle* pHandle, const char* pName,
                                 const StartInfo* pStartInfo);

    StartResult detail_PrepareSound(SoundHandle* pHandle, u32 id,
                                    const StartInfo* pStartInfo);

    StartResult detail_PrepareSound(SoundHandle* pHandle, const char* pName,
                                    const StartInfo* pStartInfo);
};

} // namespace snd
} // namespace nw4r

#undef NW4R_SND_DEFINE_START_SOUND_FUNC
#undef NW4R_SND_DEFINE_START_SOUND_FUNC_CALL_DETAIL
#undef NW4R_SND_DEFINE_START_SOUND_FUNC_INT_VERSION

#endif
