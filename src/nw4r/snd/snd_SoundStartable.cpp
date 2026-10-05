#include <nw4r/snd.h>

namespace nw4r {
namespace snd {

const char*
SoundStartable::detail_ConvertStartResultToString(StartResult result) {
    static const int resultCount = 11;
    static const char* pResults[resultCount] = {
        "START_SUCCESS",
        "START_ERR_LOW_PRIORITY",
        "START_ERR_INVALID_LABEL_STRING",
        "START_ERR_INVALID_SOUNDID",
        "START_ERR_NOT_DATA_LOADED",
        "START_ERR_NOT_ENOUGH_PLAYER_HEAP",
        "START_ERR_CANNOT_OPEN_FILE",
        "START_ERR_NOT_AVAILABLE",
        "START_ERR_CANNOT_ALLOCATE_TRACK",
        "START_ERR_NOT_ENOUGH_INSTANCE",
        "START_ERR_INVALID_PARAMETER"};

    static const char resultErrUser[] = "START_ERR_USER";
    static const char resultErrUnk[] = "START_ERR_UNKNOWN";
    static const char nullResult[] = "";

    if (result < resultCount) {
        return pResults[result];
    } else if (result == START_ERR_USER) {
        return resultErrUser;
    } else if (result == START_ERR_UNKNOWN) {
        return resultErrUnk;
    } else {
        return nullResult;
    }
}

SoundStartable::StartResult
SoundStartable::detail_StartSound(SoundHandle* pHandle, u32 id,
                                  const StartInfo* pStartInfo) {

    StartResult result = detail_SetupSound(pHandle, id, FALSE, pStartInfo);

    if (result != START_SUCCESS) {
        return result;
    }

    pHandle->StartPrepared();
    return START_SUCCESS;
}

SoundStartable::StartResult
SoundStartable::detail_HoldSound(SoundHandle* pHandle, u32 id,
                                 const StartInfo* pStartInfo) {

    if (pHandle->IsAttachedSound() && id == pHandle->GetId()) {
        pHandle->detail_GetAttachedSound()->SetAutoStopCounter(1);
        return START_SUCCESS;
    }

    StartResult result = detail_SetupSound(pHandle, id, TRUE, pStartInfo);

    if (result != START_SUCCESS) {
        return result;
    }

    pHandle->StartPrepared();
    pHandle->detail_GetAttachedSound()->SetAutoStopCounter(1);
    return START_SUCCESS;
}

SoundStartable::StartResult
SoundStartable::detail_PrepareSound(SoundHandle* handle, u32 targetID,
                                    const StartInfo* startInfo) {
    return detail_SetupSound(handle, targetID, FALSE, startInfo);
}

} // namespace snd
} // namespace nw4r
