// TODO(kiwi) REMOVE AFTER REFACTOR
#pragma ipa file

#include <egg/math.h>

#include <revolution/GX.h>

namespace EGG {

// clang-format off
Matrix34f Matrix34f::ident(1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f);
// clang-format on

DECOMP_FORCEACTIVE_DTOR(eggMatrix_cpp,
                        Quatf);

DECOMP_FORCELITERAL(eggMatrix_cpp_1,
                    1.0f, 2.0f, 0.0f);

Matrix34f::Matrix34f(f32 m00, f32 m01, f32 m02, f32 m03, f32 m10, f32 m11,
                     f32 m12, f32 m13, f32 m20, f32 m21, f32 m22, f32 m23) {

    _00 = m00;
    _01 = m01;
    _02 = m02;
    _03 = m03;
    _10 = m10;
    _11 = m11;
    _12 = m12;
    _13 = m13;
    _20 = m20;
    _21 = m21;
    _22 = m22;
    _23 = m23;
}

void Matrix34f::makeIdentity() {
    makeZero();
    _00 = _11 = _22 = 1.0f;
}

void Matrix34f::makeQ(const Quatf& rQuat) {
    f32 yy = 2.0f * rQuat.y * rQuat.y;
    f32 zz = 2.0f * rQuat.z * rQuat.z;
    f32 xx = 2.0f * rQuat.x * rQuat.x;
    f32 xy = 2.0f * rQuat.x * rQuat.y;
    f32 xz = 2.0f * rQuat.x * rQuat.z;
    f32 yz = 2.0f * rQuat.y * rQuat.z;
    f32 wz = 2.0f * rQuat.w * rQuat.z;
    f32 wx = 2.0f * rQuat.w * rQuat.x;
    f32 wy = 2.0f * rQuat.w * rQuat.y;

    _00 = 1.0f - yy - zz;
    _01 = xy - wz;
    _02 = xz + wy;

    _10 = xy + wz;
    _11 = 1.0f - xx - zz;
    _12 = yz - wx;

    _20 = xz - wy;
    _21 = yz + wx;
    _22 = 1.0f - xx - yy;

    _03 = 0.0f;
    _13 = 0.0f;
    _23 = 0.0f;
}

void Matrix34f::makeS(const Vector3f& rScale) {
    _00 = rScale.x;
    _01 = 0.0f;
    _02 = 0.0f;

    _10 = 0.0f;
    _11 = rScale.y;
    _12 = 0.0f;

    _20 = 0.0f;
    _21 = 0.0f;
    _22 = rScale.z;

    _03 = 0.0f;
    _13 = 0.0f;
    _23 = 0.0f;
}

//! The below is just ripped from the Skyward Sword decomp.
//! TODO(texline) A small section is missing
void Matrix34f::toQuat(Quatf& rQuat) const {
    const f32 temp0 = (m[0][0] + m[1][1] + m[2][2] + 1.0f) * 0.25f;
    const f32 temp1 = temp0 - (m[1][1] + m[2][2]) * 0.5f;
    const f32 temp2 = temp0 - (m[2][2] + m[0][0]) * 0.5f;
    const f32 temp3 = temp0 - (m[0][0] + m[1][1]) * 0.5f;

    // The fun method
    // int tempMax = temp0 > temp1 ?
    //         (temp0 > temp2 ? (temp0 > temp3 ? 0 : 3) : (temp2 > temp3 ? 2 :
    //         3)) : (temp1 > temp2 ? temp1 > temp3 ? 1 : 3 : (temp2 > temp3 ? 2
    //         : 3));

    int tempMax;
    if (temp0 > temp1) {
        if (temp0 > temp2) {
            if (temp0 > temp3) {
                tempMax = 0;
            } else {
                tempMax = 3;
            }
        } else {
            if (temp2 > temp3) {
                tempMax = 2;
            } else {
                tempMax = 3;
            }
        }
    } else if (temp1 > temp2) {
        if (temp1 > temp3) {
            tempMax = 1;
        } else {
            tempMax = 3;
        }
    } else if (temp2 > temp3) {
        tempMax = 2;
    } else {
        tempMax = 3;
    }

    switch (tempMax) {
    case 0:
        rQuat.w = Math<f32>::sqrt(temp0);
        rQuat.x = (0.25f / rQuat.w) * (m[2][1] - m[1][2]);
        rQuat.y = (0.25f / rQuat.w) * (m[0][2] - m[2][0]);
        rQuat.z = (0.25f / rQuat.w) * (m[1][0] - m[0][1]);
        break;
    case 1:
        rQuat.x = Math<f32>::sqrt(temp1);
        rQuat.w = (0.25f / rQuat.x) * (m[2][1] - m[1][2]);
        rQuat.y = (0.25f / rQuat.x) * (m[0][1] + m[1][0]);
        rQuat.z = (0.25f / rQuat.x) * (m[0][2] + m[2][0]);
        break;
    case 2:
        rQuat.y = Math<f32>::sqrt(temp2);
        rQuat.w = (0.25f / rQuat.y) * (m[0][2] - m[2][0]);
        rQuat.z = (0.25f / rQuat.y) * (m[1][2] + m[2][1]);
        rQuat.x = (0.25f / rQuat.y) * (m[1][0] + m[0][1]);
        break;
    case 3:
        rQuat.z = Math<f32>::sqrt(temp3);
        rQuat.w = (0.25f / rQuat.z) * (m[1][0] - m[0][1]);
        rQuat.x = (0.25f / rQuat.z) * (m[2][0] + m[0][2]);
        rQuat.y = (0.25f / rQuat.z) * (m[2][1] + m[1][2]);
        break;
    default:
        break;
    }

    if (rQuat.w < 0.0f) {
        rQuat.w = -rQuat.w;
        rQuat.x = -rQuat.x;
        rQuat.y = -rQuat.y;
        rQuat.z = -rQuat.z;
    }
    rQuat.multScalar(Math<f32>::inv(rQuat.length()));
}

void Matrix34f::slerpTo(const Matrix34f& rMtx, Matrix34f& rOut, f32 t) const {
    Quatf q1, q2, q3;
    rMtx.toQuat(q1);
    toQuat(q2);
    q2.slerpTo(q1, t, q3);
    rOut.makeQ(q3);
}

void Matrix34f::setAxisRotation(const Vector3f& rAxis, f32 angle) {
    Quatf q;
    q.setAxisRotation(rAxis, angle);
    makeQ(q);
}

//! Credits to Vabold for writing this out! (texline)
Vector3f Matrix34f::multVector(const Vector3f& rVec) const {
    Vector3f ret;
    PSMTXMultVec((MtxRefConst)this, (const Vec*)rVec, (Vec*)ret);
    return ret;
}

void Matrix34f::loadPosMtx(u32 id) {
    GXLoadPosMtxImm((MtxRefConst)this, id);
}

void Matrix34f::multiplyTo(const Matrix34f& rOp, Matrix34f& rResult) const {
    PSMTXConcat((MtxRefConst)this, rOp, rResult);
}

} // namespace EGG
