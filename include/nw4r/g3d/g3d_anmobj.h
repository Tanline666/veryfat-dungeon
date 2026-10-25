#ifndef NW4R_G3D_ANM_OBJ_H
#define NW4R_G3D_ANM_OBJ_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/res/g3d_resanm.h>

namespace nw4r {
namespace g3d {

// Forward declarations
class ResMdl;

/**
 * \~english @brief Abstract class for basic animation objects
 * \~japanese @brief 基底のアニメオブジェクトのための抽象クラス
 */
class AnmObj : public G3dObj {
    NW4R_G3D_RTTI_DECL_DERIVED(AnmObj, G3dObj);

public:
    enum AnmFlag {
        FLAG_NEED_UPDATE = (1 << 0),
        FLAG_CACHE_OBSOLETE = (1 << 1),
        FLAG_ANM_BOUND = (1 << 2),
        FLAG_USE_QUATERNION_ROTATION_BLEND = (1 << 3),
        FLAG_USE_ACCURATE_SCALE_BLEND = (1 << 4)
    };

public:
    AnmObj(MEMAllocator* pAllocator, G3dObj* pParent)
        : G3dObj(pAllocator, pParent), mFlags(0) {}

    virtual void G3dProc(u32 task, u32 param, void* pInfo) = 0; // at 0xC
    virtual ~AnmObj() {}                                        // at 0x10

    virtual void SetFrame(f32 frame) = 0; // at 0x1C
    virtual f32 GetFrame() const = 0;     // at 0x20
    virtual void UpdateFrame() = 0;       // at 0x24

    virtual void SetUpdateRate(f32 rate) = 0; // at 0x28
    virtual f32 GetUpdateRate() const = 0;    // at 0x2C

    virtual bool Bind(const ResMdl mdl) = 0; // at 0x30
    virtual void Release();                  // at 0x34

    void SetAnmFlag(AnmFlag flag, bool value);
    bool TestAnmFlag(AnmFlag flag) const;

    bool IsBound() const {
        return TestAnmFlag(FLAG_ANM_BOUND);
    }

private:
    u32 mFlags; // at 0xC
};

/******************************************************************************
 *
 * PlayPolicy
 *
 ******************************************************************************/
f32 PlayPolicy_Onetime(f32 start, f32 end, f32 frame);
f32 PlayPolicy_Loop(f32 start, f32 end, f32 frame);

typedef f32 (*PlayPolicyFunc)(f32 start, f32 end, f32 frame);

/**
 * \~english @brief Gets the animation policy corresponding
 * to the AnmPolicy parameter.
 * \~japanese @brief AnmPolicyパラメータに応じたアニメポリシーを取得する。
 *
 * \~english @param policy Animation replay policy
 * \~japanese @param policy アニメ再生ポリシー
 * \~english @return Pointer to animation replay policy function
 * \~japanese @return アニメ再生ポリシー関数へのポインタ
 */
inline PlayPolicyFunc GetAnmPlayPolicy(AnmPolicy policy) {
    static PlayPolicyFunc policyTable[ANM_POLICY_MAX] = {PlayPolicy_Onetime,
                                                         PlayPolicy_Loop};
    return policyTable[policy];
}

/**
 * \~english @brief Concrete class for frame control.
 * \~japanese @brief フレーム制御のための具象クラス。
 */
class FrameCtrl {
public:
    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     *
     * \~english @param start Start frame of animation
     * \~japanese @param start アニメの開始フレーム
     * \~english @param end End frame of animation
     * \~japanese @param end アニメの終了フレーム
     * \~english @param pPolicy Replay policy function
     * \~japanese @param pPolicy 再生ポリシー関数
     */
    FrameCtrl(f32 start, f32 end, PlayPolicyFunc pPolicy)
        : mFrame(0.0f),
          mUpdateRate(1.0f),
          mStartFrame(start),
          mEndFrame(end),
          mpPlayPolicy(pPolicy) {}

    /**
     * \~english @brief Gets the location of the current frame.
     * \~japanese @brief 現在フレームの位置を取得する。
     */
    f32 GetFrm() const {
        return mFrame;
    }
    /**
     * \~english @brief Sets the location of the current frame.
     * \~japanese @brief 現在フレームの位置を設定する。
     *
     * \~english @param frame New location of current frame
     * \~japanese @param frame 現在フレームの新しい位置
     */
    void SetFrm(f32 frame) {
        mFrame = mpPlayPolicy(mStartFrame, mEndFrame, frame);
    }

    void UpdateFrm() {
        SetFrm(mUpdateRate * smBaseUpdateRate + mFrame);
    }

    /**
     * \~english @brief Gets the frame update rate.
     * \~japanese @brief フレーム更新レートを取得する。
     */
    f32 GetRate() const {
        return mUpdateRate;
    }
    /**
     * \~english @brief Sets the frame update rate.
     * \~japanese @brief フレーム更新レートを設定する。
     *
     * \~english @param rate New rate
     * \~japanese @param rate 新しいレート
     */
    void SetRate(f32 rate) {
        mUpdateRate = rate;
    }

    /**
     * \~english @brief Sets the animation replay policy.
     * \~japanese @brief アニメ再生ポリシーを設定する。
     *
     * \~english @param func New replay policy function
     * \~japanese @param func 新しい再生ポリシー関数
     */
    void SetPlayPolicy(PlayPolicyFunc func) {
        mpPlayPolicy = func;
    }

private:
    f32 mFrame;      //!< \~english Location of current frame \~japanese
                     //!< 現在フレームの位置 \~ // at 0x0
    f32 mUpdateRate; //!< \~english Frame update rate \~japanese
                     //!< フレーム更新レート \~ // at 0x4
    f32 mStartFrame; //!< \~english Start frame of animation \~japanese
                     //!< アニメの開始フレーム \~ // at 0x8
    f32 mEndFrame;   //!< \~english End frame of animation \~japanese
                     //!< アニメの終了フレーム \~ // at 0xC
    PlayPolicyFunc mpPlayPolicy; //!< \~english Replay policy function
                                 //!< \~japanese 再生ポリシー関数 \~ // at 0x10

    static f32 smBaseUpdateRate; //!< \~english Standard frame update rate
                                 //!< \~japanese 基準のフレーム更新レート
};

} // namespace g3d
} // namespace nw4r

#endif
