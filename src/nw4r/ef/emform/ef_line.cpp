#include <nw4r/ef.h>

#include <cmath>

namespace nw4r {
namespace ef {

void EmitterFormLine::Emission(Emitter* em, ParticleManager* pm, int count,
                               u32 flags, f32* params, u16 life, f32 lifeRnd,
                               const math::MTX34* pSpace) {

    NW4R_POINTER_ASSERT(em, 42);
    NW4R_POINTER_ASSERT(pm, 43);
    NW4R_POINTER_ASSERT(params, 44);
    if (count < 1) {
        return;
    }

    for (int i = 0; i < count; i++) {
        f32 fpos;

        if (!(flags & EmitterDesc::EMIT_FLAG_FIXED_INTERVAL)) {
            fpos = em->mRandom.RandFloat();
        } else if (count > 1) {
            fpos = static_cast<f32>(i) / static_cast<f32>(count - 1);
        } else {
            fpos = 0.0f;
        }

        if (flags & EmitterDesc::EMIT_FLAG_26) {
            fpos -= 0.5f;
        }

        fpos *= params[0];

        f32 sx;
        f32 cx;
        f32 sy;
        f32 cy;
        f32 sz;
        f32 cz;
        PSSinCosRad(&sx, &cx, params[1]);
        PSSinCosRad(&sy, &cy, params[2]);
        PSSinCosRad(&sz, &cz, params[3]);

        math::VEC3 pos;
        pos.x = (cx * cz * sy + sx * sz) * fpos;
        pos.y = (-cz * sx + cx * sy * sz) * fpos;
        pos.z = (cx * cy) * fpos;

        math::VEC3 normal;
        normal.x = 0.0f;
        normal.y = 1.0f;
        normal.z = 0.0f;

        math::VEC3 fromYAxis(pos.x, 0.0f, pos.z);

        math::VEC3 vel;
        CalcVelocity(&vel, em, pos, normal, pos, fromYAxis);

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
