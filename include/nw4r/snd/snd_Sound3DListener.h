#ifndef NW4R_SND_SOUND_3D_LISTENER_H
#define NW4R_SND_SOUND_3D_LISTENER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace snd {

class Sound3DListener {
public:
    Sound3DListener();

    const math::MTX34& GetMatrix() const {
        return mMtx;
    }
    void SetMatrix(const math::MTX34& rMtx) {
        mMtx = rMtx;
    }

    f32 GetInteriorSize() const {
        return mInteriorSize;
    }
    void SetInteriorSize(f32 size) {
        mInteriorSize = size;
    }

    f32 GetMaxVolumeDistance() const {
        return mMaxVolumeDistance;
    }
    void SetMaxVolumeDistance(f32 distance) {
        mMaxVolumeDistance = distance;
    }

    f32 GetUnitDistance() const {
        return mUnitDistance;
    }
    void SetUnitDistance(f32 distance) {
        mUnitDistance = distance;
    }

private:
    void CalcPositionFromMatrix(const math::MTX34& rMtx, math::VEC3* pPos);

    math::MTX34 mMtx;           // at 0x0
    math::VEC3 mPosition;       // at 0x30
    math::VEC3 mVelocity;       // at 0x3C
    f32 mInteriorSize;          // at 0x48
    f32 mMaxVolumeDistance;     // at 0x4C
    f32 mUnitDistance;          // at 0x50
    u32 mUserParam;             // at 0x54
    bool mResetMatrixFlag;      // at 0x58
    f32 mUnitBiquadFilterValue; // at 0x5C
    f32 mMaxBiquadFilterValue;  // at 0x60
public:
    NW4R_UT_LINKLIST_NODE_DECL(); // at 0x64
};

} // namespace snd
} // namespace nw4r

#endif
