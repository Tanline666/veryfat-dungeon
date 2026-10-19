#ifndef RP_KERNEL_FRAME_CTRL_H
#define RP_KERNEL_FRAME_CTRL_H
#include <Pack/types_pack.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Frame animation controller
 */
class RPSysFrameCtrl {
public:
    /**
     * \~english @brief Animation play policy
     */
    enum EPlayMode {
        EPlayMode_OneTime, //!< Animation plays once
        EPlayMode_Reset,   //!< Animation plays once, then resets to the start
        EPlayMode_Loop,    //!< Animation loops indefinitely
    };

    /**
     * \~english @brief Initializes the animation state
     *
     * \~english @param frameNum Number of frames to animate
     */
    void init(s16 frameNum);

    /**
     * \~english @brief Updates the animation state
     */
    void update();

    /**
     * \~english @brief Resets the animation state
     */
    void reset() {
        mFrame = mStartFrame;
        mFlags = 0;
    }

    /**
     * \~english @brief Starts playing the animation from the specified frame
     *
     * \~english @param frame Starting frame
     */
    void play(s16 frame) {
        mFrame = static_cast<f32>(frame);
        mIsEnabled = true;
    }

    /**
     * \~english @brief Stops playing the animation
     */
    void stop() {
        mIsEnabled = false;
    }

    /**
     * \~english @brief Tests whether this animation has finished
     */
    bool isFinished() const {
        return mFlags & EFlag_Finished;
    }

    /**
     * \~english @brief Tests whether this animation has looped at least once
     */
    bool isLooped() const {
        return mFlags & EFlag_Looped;
    }

    /**
     * \~english @brief Sets the animation play policy
     *
     * \~english @param playMode New play policy
     */
    void setPlayMode(EPlayMode playMode) {
        mPlayMode = playMode;
    }

    /**
     * \~english @brief Sets the starting frame of this animation
     *
     * \~english @param frame New starting frame
     */
    void setStartFrame(s16 frame) {
        mStartFrame = frame;
    }

    /**
     * \~english @brief Sets the animation update rate
     *
     * \~english @param rate New update rate
     */
    void setRate(f32 rate) {
        mUpdateRate = rate;
    }

    /**
     * \~english @brief Gets the current animation position
     */
    f32 getFrame() const {
        return mFrame;
    }
    /**
     * \~english @brief Sets the current animation position
     *
     * \~english @param frame New frame
     */
    void setFrame(f32 frame) {
        mFrame = frame;
    }

private:
    /**
     * \~english @brief Animation result flags
     */
    enum {
        EFlag_Finished = 1 << 0, //!< Non-looping animation has finished
        EFlag_Looped = 1 << 1,   //!< Animation has looped at least once
    };

private:
    //! Animation play policy
    u8 mPlayMode; // at 0x0
    //! Animation result flags
    u8 mFlags; // at 0x1
    //! Whether animation is enabled
    s16 mIsEnabled; // at 0x2

    //! First frame of the animation
    s16 mStartFrame; // at 0x4
    //! Last frame of the animation
    s16 mEndFrame; // at 0x6
    //! Offset to apply when looping
    s16 mLoopFrame; // at 0x8

    //! Animation update speed
    f32 mUpdateRate; // at 0xC
    //! Current animation position
    f32 mFrame; // at 0x10
};

//! @}

#endif
