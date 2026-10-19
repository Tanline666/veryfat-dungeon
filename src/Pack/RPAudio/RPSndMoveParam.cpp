#include <Pack/RPAudio.h>

#include <nw4r/snd.h>
#include <nw4r/ut.h>

/******************************************************************************
 *
 * RPSndMoveValueF32
 *
 ******************************************************************************/

/**
 * \~english @brief Constructor
 */
RPSndMoveValueF32::RPSndMoveValueF32()
    : mFrame(0), mTarget(1.0f), mValue(1.0f), mDelta(1.0f) {

    mpCallback = NULL;
}

/**
 * \~english @brief Begins interpolation of this value
 *
 * \~english @param target Interpolation target
 * \~english @param frame Interpolation duration
 */
void RPSndMoveValueF32::moveValue(f32 target, u32 frame) {
    mTarget = target;
    mFrame = frame;

    if (frame == 0) {
        mValue = target;
    } else {
        mDelta = (mValue - target) / frame;
    }
}

/**
 * \~english @brief Updates the interpolation state
 *
 * \~english @return Whether interpolation is ongoing
 */
bool RPSndMoveValueF32::update() {
    if (mFrame > 0) {
        if (--mFrame > 0) {
            mValue -= mDelta;
        } else {
            mValue = mTarget;
        }

        if (mpCallback != NULL) {
            mpCallback(mValue, mpUserData);
        }

        return true;
    }

    return false;
}

/******************************************************************************
 *
 * RPSndMoveParam
 *
 ******************************************************************************/

/**
 * \~english @brief Constructor
 *
 * \~english @param category Sound parameter category
 * \~english @param rHandle Sound handle
 */
RPSndMoveParam::RPSndMoveParam(ParamCategory category,
                               nw4r::snd::SoundHandle& rHandle)
    : mCategory(category), mrHandle(rHandle) {}

/**
 * \~english @brief Updates the interpolation state
 *
 * \~english @return Whether interpolation is ongoing
 */
bool RPSndMoveParam::update() {
    if (mrHandle.IsAttachedSound()) {
        nw4r::snd::SeqSoundHandle seqHandle(&mrHandle);
        bool ongoing = RPSndMoveValueF32::update();

        switch (mCategory) {
        case ParamCategory_SoundPitch: {
            mrHandle.SetPitch(mValue);
            break;
        }

        case ParamCategory_SoundLpf: {
            mrHandle.SetLpfFreq(mValue);
            break;
        }

        case ParamCategory_TrackVolume: {
            seqHandle.SetTrackVolume(mTrackFlags, mValue);
            break;
        }

        case ParamCategory_TrackPitch: {
            seqHandle.SetTrackPitch(mTrackFlags, mValue);
            break;
        }

        default: {
            break;
        }
        }

        return ongoing;
    }

    return false;
}

/******************************************************************************
 *
 * RPSndMoveParamMgr
 *
 ******************************************************************************/

/**
 * \~english @brief List of all active move parameters
 */
nw4r::ut::List RPSndMoveParamMgr::sMoveParamList;

/**
 * \~english @brief Updates the state of all move parameters
 */
void RPSndMoveParamMgr::update() {
    RPSndMoveParamLink* pIt = static_cast<RPSndMoveParamLink*>(
        nw4r::ut::List_GetFirst(&sMoveParamList));

    RPSndMoveParamLink* pNext = NULL;

    for (; pIt != NULL; pIt = pNext) {
        bool ongoing = static_cast<RPSndMoveParam*>(pIt)->doUpdate();

        pNext = static_cast<RPSndMoveParamLink*>(
            nw4r::ut::List_GetNext(&sMoveParamList, pIt));

        if (!ongoing) {
            nw4r::ut::List_Remove(&sMoveParamList, pIt);
        }
    }
}

DECOMP_FORCELITERAL(RPSndMoveParam_cpp,
                    4503599627370496.0);
