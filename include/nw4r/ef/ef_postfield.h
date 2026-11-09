#ifndef NW4R_EF_POSTFIELD_H
#define NW4R_EF_POSTFIELD_H
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/math.h>

namespace nw4r {
namespace ef {

enum CollisionStatus {
    COLLISION_CROSS = 0x80,

    COLLISION_STATUS_NONE = 0,
    COLLISION_STATUS_BORDER,
    COLLISION_STATUS_INNER,
    COLLISION_STATUS_OUTER,
    COLLISION_STATUS_PLANEOUTER,

    COLLISION_STATUS_BORDER_CROSS = COLLISION_STATUS_BORDER | COLLISION_CROSS,
    COLLISION_STATUS_INNER_CROSS = COLLISION_STATUS_INNER | COLLISION_CROSS,
    COLLISION_STATUS_OUTER_CROSS = COLLISION_STATUS_OUTER | COLLISION_CROSS
};

struct PostFieldInfo {
    struct AnimatableParams {
        math::VEC3 mSize;
        math::VEC3 mRotate;
        math::VEC3 mTranslate;
    };

    struct ChildOption {
        EmitterInheritSetting mInheritSetting;
        u16 mNameIdx;
    };

    enum ControlSpeedType {
        CONTROL_SPEED_NONE,
        CONTROL_SPEED_LIMIT,
        CONTROL_SPEED_STOP,
    };

    enum WrapOption {
        WRAP_ENABLE = 1 << 0,
        WRAP_CENTER_ORIGIN = 0 << 1,
        WRAP_CENTER_EMITTER = 1 << 1
    };

    AnimatableParams mAnimatableParams; //!< \~ // at 0x0
    f32 mReferenceSpeed;                //!< \~ // at 0x24
    u8 mControlSpeedType;               //!< \~ // at 0x28
    u8 mCollisionShapeType;             //!< \~ // at 0x29
    u8 mCollisionShapeOption;           //!< \~ // at 0x2A
    u8 mCollisionType;                  //!< \~ // at 0x2B
    u16 mCollisionOption;               //!< \~ // at 0x2C
    u16 mStartFrame;                    //!< \~ // at 0x2E
    math::VEC3 mSpeedFactor;            //!< \~ // at 0x30
    ChildOption mChildOption;           //!< \~ // at 0x3C
    u8 mWrapOption;                     //!< \~ // at 0x48
    u8 padding[3];                      //!< \~ // at 0x49
    math::VEC3 mWrapScale;              //!< \~ // at 0x4C
    math::VEC3 mWrapRotate;             //!< \~ // at 0x58
    math::VEC3 mWrapTranslate;          //!< \~ // at 0x64
};

u8 calcPostField(Particle* pp,
                 PostFieldInfo::AnimatableParams& postfieldAnimParams,
                 PostFieldInfo* postfieldResource,
                 EmitterResource* postfieldChildRes,
                 const math::MTX34& mtxEmitterToGlobal,
                 const math::MTX34& mtxGlobalToLocal,
                 const math::VEC3& prevPosition, math::VEC3 addPosition,
                 math::VEC3* nextVelocity, bool* requestKilling);

} // namespace ef
} // namespace nw4r

#endif
