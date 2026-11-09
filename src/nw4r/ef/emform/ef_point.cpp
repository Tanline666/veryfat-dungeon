#include <nw4r/ef.h>

#include <cmath>

namespace nw4r {
namespace ef {

void EmitterFormPoint::Emission(Emitter* em, ParticleManager* pm, int count,
                                u32 flags, f32* params, u16 life, f32 lifeRnd,
                                const math::MTX34* pSpace) {

    NW4R_POINTER_ASSERT(em, 42);
    NW4R_POINTER_ASSERT(pm, 43);
    NW4R_POINTER_ASSERT(params, 44);

#pragma unused(flags)
#pragma unused(params)

    if (count < 1) {
        return;
    }

    for (int i = 0; i < count; i++) {
        math::VEC3 pos(0.0f, 0.0f, 0.0f);
        math::VEC3 normal;

        f32 rx = em->mRandom.RandFloat() * 2.0f - 1.0f;
        if (rx >= 0.0f) {
            normal.x = (0.66f + 0.34f * rx) * rx;
        } else {
            normal.x = (0.66f - 0.34f * rx) * rx;
        }

        f32 radius = math::FSqrt(1.0f - normal.x * normal.x);
        rx = em->mRandom.RandFloat() * NW4R_MATH_PI * 2;
        PSSinCosRad(&normal.z, &normal.y, rx);
        normal.y *= radius;
        normal.z *= radius;

        math::VEC3 fromYAxis(normal.x, 0.0f, normal.z);
        Normalize(&fromYAxis, &fromYAxis);

        math::VEC3 vel;
        CalcVelocity(&vel, em, pos, normal, normal, fromYAxis);

        // clang-format off
        pm->CreateParticle(
            CalcLife(life, lifeRnd, em),
            pos,
            vel,
            pSpace,
            1.0f + em->mParameter.mVelMomentumRandom *
                (1.0f / 100.0f) * em->mRandom.RandFloat(),
            &em->mInheritSetting,
            em->mpReferenceParticle,
            em->mCalcRemain);
        // clang-format on
    }
}

} // namespace ef
} // namespace nw4r
