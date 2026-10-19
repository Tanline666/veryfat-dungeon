#ifndef RP_KERNEL_LYT_ANM_OBJ_H
#define RP_KERNEL_LYT_ANM_OBJ_H
#include <Pack/types_pack.h>

#include <Pack/RPKernel/RPSysFrameCtrl.h>

#include <nw4r/lyt.h>
#include <nw4r/ut.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Layout animation object
 */
class RPSysLytAnmObj {
    friend class RPSysLayout;

public:
    /**
     * \~english @brief Constructor
     */
    RPSysLytAnmObj();

    /**
     * \~english @brief Updates the layout animation
     */
    void calc();

    /**
     * \~english @brief Resets the layout animation
     */
    void reset();

    /**
     * \~english @brief Starts playing the animation from the specified frame
     *
     * \~english @param frame Starting frame (-1 to play from the current frame)
     */
    void play(s16 frame);

    /**
     * \~english @brief Initializes the layout animation
     *
     * \~english @param frame Starting frame
     * \~english @param frameNum Number of frames to animate
     * \~english @param playMode Animation policy
     */
    void init(s16 frame, s16 frameNum, RPSysFrameCtrl::EPlayMode playMode);

    /**
     * \~english @brief Stops playing the animation
     */
    void stop() {
        mFrameCtrl.stop();
    }

    /**
     * \~english @brief Gets the number of frames in this animation
     */
    f32 getFrameMax();

    /**
     * \~english @brief Tests whether this animation has finished
     */
    bool isFinished() const {
        return mFrameCtrl.isFinished();
    }

    /**
     * \~english @brief Tests whether this animation has looped at least once
     */
    bool isLooped() const {
        return mFrameCtrl.isLooped();
    }

    /**
     * \~english @brief Sets the animation play policy
     *
     * \~english @param playMode New play policy
     */
    void setPlayMode(RPSysFrameCtrl::EPlayMode playMode) {
        mFrameCtrl.setPlayMode(playMode);
    }

    /**
     * \~english @brief Sets the current animation position
     *
     * \~english @param frame New frame
     */
    void setFrame(f32 frame) {
        mFrameCtrl.setFrame(frame);
    }

    /**
     * \~english @brief Sets the animation update rate
     *
     * \~english @param rate New update rate
     */
    void setRate(f32 rate) {
        mFrameCtrl.setRate(rate);
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
    //! Animation transform
    nw4r::lyt::AnimTransform* mpAnimTrans; // at 0x0
    //! Animation controller
    RPSysFrameCtrl mFrameCtrl; // at 0x4

    //! Number of animation contents
    u16 mNumAnimContent; // at 0x18
    //! Animation contents
    nw4r::lyt::AnimationLink* mpAnimContent; // at 0x1C

public:
    //! Linked-list node
    NW4R_UT_LIST_LINK_DECL(); // at 0x20
};

//! @}

#endif
