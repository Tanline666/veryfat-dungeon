#include <Pack/RPKernel.h>
#include <Pack/RPSystem.h>

/**
 * \~english @brief Constructor
 */
RPSysLytAnmObj::RPSysLytAnmObj() {
    mFrameCtrl.init(1);
    mpAnimTrans = NULL;
    mNumAnimContent = 0;
    mpAnimContent = NULL;
}

/**
 * \~english @brief Updates the animation playback
 */

//! TODO: Update for Wii Fit Plus (texline)
void RPSysLytAnmObj::calc() {
    switch (RP_GET_INSTANCE(RPSysProjectLocal)->getPack()) {
    case RPSysSceneCreator::EPackID_SportsPack:
    case RPSysSceneCreator::EPackID_PartyPack: {
        // TODO(kiwi) Why does this work differently in Wii Sports/Wii Play?
        mpAnimTrans->SetFrame(mFrameCtrl.getFrame());
        mFrameCtrl.update();
        break;
    }

    /* RPSysSceneCreator::EPackID_HealthPack */
    /* RPSysSceneCreator::EPackID_MusicPack */
    default: {
        mFrameCtrl.update();
        mpAnimTrans->SetFrame(mFrameCtrl.getFrame());
        break;
    }
    }
}

/**
 * \~english @brief Resets the animation playback
 */
void RPSysLytAnmObj::reset() {
    mFrameCtrl.reset();
}

/**
 * \~english @brief Starts playing the animation from the specified frame
 *
 * \~english @param frame Starting frame
 */
void RPSysLytAnmObj::play(s16 frame) {
    if (frame < 0) {
        frame = mFrameCtrl.getFrame();
    }

    reset();
    mFrameCtrl.play(frame);
}

/**
 * \~english @brief Initializes the layout animation
 *
 * \~english @param frame Starting frame
 * \~english @param frameNum Number of frames to animate
 * \~english @param playMode Animation policy
 */
void RPSysLytAnmObj::init(s16 frame, s16 frameNum,
                          RPSysFrameCtrl::EPlayMode playMode) {
    mFrameCtrl.init(frameNum);
    mFrameCtrl.setStartFrame(frame);
    mFrameCtrl.setPlayMode(playMode);
    mFrameCtrl.play(frame);
}

/**
 * \~english @brief Gets the number of frames in this animation
 */
f32 RPSysLytAnmObj::getFrameMax() {
    return mpAnimTrans->GetFrameMax();
}
