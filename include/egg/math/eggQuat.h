#ifndef EGG_MATH_QUAT_H
#define EGG_MATH_QUAT_H
#include <egg/types_egg.h>

#include <egg/math/eggVector.h>

namespace EGG {

struct Quatf : Vector3f {
public:
    Quatf() {}
    Quatf(f32 fw, Vector3f vxyz) : w(fw), Vector3f(vxyz) {}
    ~Quatf() {}

    void multScalar(f32 scale) {
        w *= scale;
        x *= scale;
        y *= scale;
        z *= scale;
    }

    void set(f32 fw, f32 fx, f32 fy, f32 fz);
    void setAxisRotation(const Vector3f& rAxis, f32 angle);
    void slerpTo(const Quatf& rQuat, f32 t, Quatf& rOut) const;

public:
    f32 w; // at 0xC
};

} // namespace EGG

#endif
