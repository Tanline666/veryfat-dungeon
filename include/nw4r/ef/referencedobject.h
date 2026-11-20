#ifndef NW4R_EF_REFERENCED_OBJECT_H
#define NW4R_EF_REFERENCED_OBJECT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/linkedobject.h>
#include <nw4r/math.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace ef {

/**
 * @~english @brief Base class for classes managed by ActivityList.
 * @~japanese @brief ActivityListが管理するクラスの基底クラス。
 */
class ReferencedObject : public LinkedObject {
public:
    enum LifeStatus {
        NW4R_EF_LS_CLOSED,
        NW4R_EF_LS_ACTIVE,
        NW4R_EF_LS_WAIT,
        NW4R_EF_LS_CLOSING,
    };

protected:
    LifeStatus mLifeStatus; // at 0xC
    u32 mRefCount;          // at 0x10

public:
    ut::Link mActivityLink; // at 0x14

public:
    /**
     * @~english @brief Sends a command to close the opened effects. By default,
     * this does nothing.
     * @~japanese @brief
     * オープンしたエフェクトをクローズするコマンドを送る。デフォルトでは何をしてない。
     */
    virtual void SendClosing() {} // at 0x8

protected:
    /**
     * @~english @brief Base function for destroying effect class instances. By
     * default, this does nothing.
     * @~japanese @brief
     * エフェクトクラスインスタンスを破棄する基底関数。デフォルトでは何をしてない。
     */
    virtual void DestroyFunc() {} //!< // at 0xC

public:
    void ChangeLifeStatus(LifeStatus status) {
        mLifeStatus = status;
    }
    LifeStatus GetLifeStatus() const {
        return mLifeStatus;
    }

    u32 GetRefCount() const {
        return mRefCount;
    }

    bool Initialize() {
        mRefCount = 0;
        mLifeStatus = NW4R_EF_LS_ACTIVE;
        return true;
    }
    void Destroy() {
        DestroyFunc();
        mLifeStatus = NW4R_EF_LS_WAIT;

        if (mRefCount == 0) {
            SendClosing();
        }
    }

    /**
     * @~english @brief Increments the reference counter by one.
     * @~japanese @brief リファレンスカウンタを1つ増やす。
     *
     * @~english @return Reference count value after incrementation
     * @~japanese @return 増やした後のリファレンスカウント値
     */
    u32 Ref() {
        return mRefCount++;
    }
    /**
     * @~english @brief Decrements the reference counter by one.
     * @~japanese @brief リファレンスカウンタを1つ減らす。
     *
     * @~english @return Reference count value after decrementation
     * @~japanese @return 減らした後のリファレンスカウント値
     */
    u32 UnRef() {
        NW4R_ASSERT(mRefCount > 0, 108);

        if (--mRefCount == 0 && mLifeStatus == NW4R_EF_LS_WAIT) {
            SendClosing();
        }

        return mRefCount;
    }
};

} // namespace ef
} // namespace nw4r

#endif
