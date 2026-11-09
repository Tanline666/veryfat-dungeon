#include <MSL/cmath>

#include <nw4r/ef.h>

namespace nw4r {
namespace ef {

void EmitterForm::CalcVelocity(math::VEC3* result, Emitter* em,
                               const math::VEC3& position,
                               const math::VEC3& normalDir,
                               const math::VEC3& fromOrigin,
                               const math::VEC3& fromYAxis) const {

#pragma unused(position)

    math::VEC3 tmpv3;
    f32 tmpf;

    if (em->mParameter.mVelPowerRadiationDir != 0.0f) {
        math::VEC3Scale(result, &fromOrigin,
                        em->mParameter.mVelPowerRadiationDir);
    } else {
        result->x = 0.0f;
        result->y = 0.0f;
        result->z = 0.0f;
    }

    if (em->mParameter.mVelPowerYAxis != 0.0f) {
        math::VEC3Scale(&tmpv3, &fromYAxis, em->mParameter.mVelPowerYAxis);

        math::VEC3Add(result, result, &tmpv3);
    }

    if (em->mParameter.mVelPowerRandomDir != 0.0f) {
        f32 rotx = em->mRandom.RandFloat() * NW4R_MATH_PI * 2;
        f32 roty = em->mRandom.RandFloat() * NW4R_MATH_PI * 2;
        f32 rotz = em->mRandom.RandFloat() * NW4R_MATH_PI * 2;

        f32 sinr;
        f32 cosr;

        f32 sinp;
        f32 cosp;

        f32 sinh;
        f32 cosh;

        PSSinCosRad(&sinr, &cosr, rotx);
        PSSinCosRad(&sinp, &cosp, roty);
        PSSinCosRad(&sinh, &cosh, rotz);

        result->x += (cosr * sinp * cosh + sinr * sinh) *
                     em->mParameter.mVelPowerRandomDir;
        result->y += (cosr * sinp * sinh - sinr * cosh) *
                     em->mParameter.mVelPowerRandomDir;

        result->z += em->mParameter.mVelPowerRandomDir * cosr * cosp;
    }

    if (em->mParameter.mVelPowerNormalDir != 0.0f) {
        result->x += normalDir.x * em->mParameter.mVelPowerNormalDir;
        result->y += normalDir.y * em->mParameter.mVelPowerNormalDir;
        result->z += normalDir.z * em->mParameter.mVelPowerNormalDir;
    }

    if (em->mParameter.mVelPowerSpecDir != 0.0f) {
        if (em->mParameter.mVelDiffusionSpecDir == 0.0f) {
            math::MTX34 mtx;
            PSMTX34RotXYZRad(&mtx, em->mParameter.mVelSpecDir.x,
                             em->mParameter.mVelSpecDir.y,
                             em->mParameter.mVelSpecDir.z);

            math::VEC3 vec(0.0f, 1.0f, 0.0f);
            math::VEC3Transform(&vec, &mtx, &vec);

            result->x += em->mParameter.mVelPowerSpecDir * vec.x;
            result->y += em->mParameter.mVelPowerSpecDir * vec.y;
            result->z += em->mParameter.mVelPowerSpecDir * vec.z;
        } else {
            math::MTX34 mtx;

            // clang-format off
            PSMTX34RotXYZRad(&mtx,
                em->mRandom.RandFloat() * em->mParameter.mVelDiffusionSpecDir,
                em->mRandom.RandFloat() * NW4R_MATH_PI * 2,
                0.0f);
            // clang-format on

            math::MTX34 rotFromZ;
            PSMTX34RotXYZRad(&rotFromZ, em->mParameter.mVelSpecDir.x,
                             em->mParameter.mVelSpecDir.y,
                             em->mParameter.mVelSpecDir.z);

            math::MTX34Mult(&mtx, &rotFromZ, &mtx);

            result->x += mtx._01 * em->mParameter.mVelPowerSpecDir;
            result->y += mtx._11 * em->mParameter.mVelPowerSpecDir;
            result->z += mtx._21 * em->mParameter.mVelPowerSpecDir;
        }
    }

    if (em->mParameter.mVelInitVelocityRandom != 0) {
        tmpf = 1.0f - em->mParameter.mVelInitVelocityRandom * (1.0f / 100.0f) *
                          (em->mRandom.RandFloat() * 2.0f - 1.0f);

        result->x *= tmpf;
        result->y *= tmpf;
        result->z *= tmpf;
    }
}

u16 EmitterForm::CalcLife(u16 life, f32 lifeRnd, Emitter* em) {
    u16 newLife = life;

    if (lifeRnd != 0.0f) {
        f32 rnd = life * em->mRandom.RandFloat() * lifeRnd;

        if (static_cast<f32>(newLife) - rnd < 1.0f) {
            newLife = 1;
        } else if (static_cast<f32>(newLife) - rnd > 65535.0f) {
            newLife = 65535;
        } else {
            newLife -= static_cast<u16>(rnd);
        }
    }

    return newLife;
}

} // namespace ef
} // namespace nw4r
