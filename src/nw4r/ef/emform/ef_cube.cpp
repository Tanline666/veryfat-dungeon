#include <nw4r/ef.h>

#include <cmath>

namespace nw4r {
namespace ef {

void EmitterFormCube::EmissionSub(math::VEC3& rPos, math::VEC3& normal,
                                  Emitter* em, ParticleManager* pm,
                                  u16 aPtclLife, f32 aPtclLifeRnd,
                                  const math::MTX34* space) {

    Normalize(&normal, &normal);

    math::VEC3 fromOrigin = rPos;

    if (math::VEC3LenSq(&fromOrigin) <= NW4R_MATH_FLT_MIN) {
        fromOrigin.x = 2.0f * em->mRandom.RandFloat() - 1.0f;
        fromOrigin.y = 2.0f * em->mRandom.RandFloat() - 1.0f;
        fromOrigin.z = 2.0f * em->mRandom.RandFloat() - 1.0f;
    }

    Normalize(&fromOrigin, &fromOrigin);

    math::VEC3 fromYAxis = rPos;
    fromYAxis.y = 0.0f;

    if (math::VEC3LenSq(&fromYAxis) <= NW4R_MATH_FLT_MIN) {
        fromYAxis.x = 2.0f * em->mRandom.RandFloat() - 1.0f;
        fromYAxis.z = 2.0f * em->mRandom.RandFloat() - 1.0f;
    }

    Normalize(&fromYAxis, &fromYAxis);

    math::VEC3 vel;
    CalcVelocity(&vel, em, rPos, normal, fromOrigin, fromYAxis);

    // clang-format off
    pm->CreateParticle(
        CalcLife(aPtclLife, aPtclLifeRnd, em),
        rPos,
        vel,
        space,
        1.0f + em->mParameter.mVelMomentumRandom *
            (1.0f / 100.0f) * em->mRandom.RandFloat(),
        &em->mInheritSetting,
        em->mpReferenceParticle,
        em->mCalcRemain);
    // clang-format on
}

void EmitterFormCube::Emission(Emitter* em, ParticleManager* pm, int count,
                               u32 optionFlag, f32* params, u16 aPtclLife,
                               f32 aPtclLifeRnd, const math::MTX34* space) {

    NW4R_POINTER_ASSERT(em, 94);
    NW4R_POINTER_ASSERT(pm, 95);
    NW4R_POINTER_ASSERT(optionFlag, 96);

    if (count < 1) {
        return;
    }

    f32 outerX, outerY, outerZ;

    outerX = params[PARAM_CUBE_X];

    if (optionFlag & OPTION_CUBE_LINKED_SIZE) {
        outerY = outerX;
        outerZ = outerX;
    } else {
        outerY = params[PARAM_CUBE_Y];
        outerZ = params[PARAM_CUBE_Z];
    }

    if (outerX < 1e-5f) {
        outerX = 1e-5f;
    }
    if (outerY < 1e-5f) {
        outerY = 1e-5f;
    }
    if (outerZ < 1e-5f) {
        outerZ = 1e-5f;
    }

    f32 inner;
    if (params[PARAM_CUBE_INNER] == 0.0f) {
        inner = 1e-5f;
    } else {
        inner = params[PARAM_CUBE_INNER] * (1.0f / 100.0f);
    }

    f32 inner_rev = 1.0f - inner;

    f32 diffusionNormal = em->mParameter.mVelDiffusionEmitterNormal;
    if (std::fabs(diffusionNormal - NW4R_MATH_PI) <
        NW4R_MATH_PI * NW4R_MATH_FLT_EPSILON) {

        diffusionNormal -= NW4R_MATH_PI * NW4R_MATH_FLT_EPSILON;
    }

    if (optionFlag & EmitterDesc::EMIT_FLAG_FIXED_INTERVAL) {
        f32 div = 2.0f / (count + 1);
        {
            int i;
            int xcnt;
            int ycnt;
            //! \~english Negative Y-axis
            //! \~japanese 負数のY座軸
            {
                xcnt = (count / 2) + 1;
                ycnt = ((count - 1) / 2) + 1;

                int dir = 0;
                if (count % 2 == 0) {
                    dir = 2;
                }

                int dircntLife = 1;
                int dircntRemain = dircntLife;

                for (i = 0; i < count * count; i++) {
                    if (i != 0) {
                        switch (dir) {
                        case 0: {
                            ycnt--;
                            break;
                        }

                        case 1: {
                            xcnt++;
                            break;
                        }

                        case 2: {
                            ycnt++;
                            break;
                        }

                        case 3: {
                            xcnt--;
                            break;
                        }
                        }

                        if (--dircntRemain <= 0) {
                            dir++;

                            if (dir == 4) {
                                dir = 0;
                            }

                            if (dir % 2 == 0) {
                                dircntLife++;
                            }

                            dircntRemain = dircntLife;
                        }
                    }
                    {
                        f32 xbase = xcnt * div - 1.0f;
                        f32 ybase = ycnt * div - 1.0f;

                        f32 x = xbase * outerX;
                        f32 y = ybase * outerZ;

                        f32 maxdepth =
                            1.0f - MAX(std::fabs(xbase), std::fabs(ybase));
                        f32 distance = MAX(std::fabs(xbase), std::fabs(ybase));

                        math::VEC3 p;
                        math::VEC3 normal;

                        p.x = x;
                        p.y = -outerY;
                        p.z = y;

                        if (inner_rev != 0.0f) {
                            f32 rnd;
                            if (optionFlag & EmitterDesc::EMIT_FLAG_24) {
                                rnd = em->mRandom.RandFloat();
                                rnd *= rnd;
                                rnd = 1.0f - rnd * inner_rev * maxdepth;
                            } else {
                                rnd = 1.0f - (em->mRandom.RandFloat() *
                                              inner_rev * maxdepth);
                            }

                            p.y *= rnd;
                        }

                        if (diffusionNormal == 0.0f ||
                            (std::fabs(p.x) < 1e-5f &&
                             std::fabs(p.z) < 1e-5f)) {

                            normal.x = 0.0f;
                            normal.y = -1.0f;
                            normal.z = 0.0f;
                        } else {
                            f32 cone = distance * diffusionNormal;
                            f32 ylen = math::FInv(std::tanf(cone));

                            normal.x = p.x;
                            normal.y =
                                ylen * -math::FSqrt(p.x * p.x + p.z * p.z);
                            normal.z = p.z;
                        }

                        EmissionSub(p, normal, em, pm, aPtclLife, aPtclLifeRnd,
                                    space);
                    }
                }
            }

            for (ycnt = 1; ycnt <= count; ycnt++) {
                f32 ybase = ycnt * div - 1.0f;
                f32 y = ybase * outerY;

                //! \~english Negative Z-axis
                //! \~japanese 負数のZ座軸
                for (xcnt = 1; xcnt <= count; xcnt++) {
                    f32 xbase = xcnt * div - 1.0f;
                    f32 x = xbase * outerX;

                    f32 maxdepth =
                        1.0f - MAX(std::fabs(xbase), std::fabs(ybase));
                    f32 distance = MAX(std::fabs(xbase), std::fabs(ybase));

                    math::VEC3 p;
                    math::VEC3 normal;

                    p.x = x;
                    p.y = y;
                    p.z = -outerZ;

                    if (inner_rev != 0.0f) {
                        f32 rnd;
                        if (optionFlag & OPTION_CUBE_FLAT_DENSITY) {
                            rnd = em->mRandom.RandFloat();
                            rnd *= rnd;
                            rnd = 1.0f - rnd * inner_rev * maxdepth;
                        } else {
                            rnd = 1.0f - (em->mRandom.RandFloat() * inner_rev *
                                          maxdepth);
                        }

                        p.z *= rnd;
                    }

                    if (diffusionNormal == 0.0f ||
                        (std::fabs(p.x) < 1e-5f && std::fabs(p.y) < 1e-5f)) {

                        normal.x = 0.0f;
                        normal.y = 0.0f;
                        normal.z = -1.0f;
                    } else {
                        f32 cone = distance * diffusionNormal;
                        f32 ylen = math::FInv(std::tanf(cone));

                        normal.x = p.x;
                        normal.y = p.y;

                        normal.z = ylen * -math::FSqrt(p.x * p.x + p.y * p.y);
                    }

                    EmissionSub(p, normal, em, pm, aPtclLife, aPtclLifeRnd,
                                space);
                }

                //! \~english Positive x-axis
                //! \~japanese 正数のX座軸
                for (xcnt = 1; xcnt <= count; xcnt++) {
                    f32 xbase = xcnt * div - 1.0f;
                    f32 x = xbase * outerZ;

                    f32 maxdepth =
                        1.0f - MAX(std::fabs(xbase), std::fabs(ybase));
                    f32 distance = MAX(std::fabs(xbase), std::fabs(ybase));

                    math::VEC3 p;
                    math::VEC3 normal;

                    p.x = outerX;
                    p.y = y;
                    p.z = x;

                    if (inner_rev != 0.0f) {
                        f32 rnd;
                        if (optionFlag & EmitterDesc::EMIT_FLAG_24) {
                            rnd = em->mRandom.RandFloat();
                            rnd *= rnd;
                            rnd = 1.0f - rnd * inner_rev * maxdepth;
                        } else {
                            rnd = 1.0f - (em->mRandom.RandFloat() * inner_rev *
                                          maxdepth);
                        }

                        p.x *= rnd;
                    }

                    if (diffusionNormal == 0.0f ||
                        (std::fabs(p.y) < 1e-5f && std::fabs(p.z) < 1e-5f)) {

                        normal.x = 1.0f;
                        normal.y = 0.0f;
                        normal.z = 0.0f;
                    } else {
                        f32 cone = distance * diffusionNormal;
                        f32 ylen = math::FInv(std::tanf(cone));

                        normal.x = ylen * math::FSqrt(p.y * p.y + p.z * p.z);

                        normal.y = p.y;
                        normal.z = p.z;
                    }

                    EmissionSub(p, normal, em, pm, aPtclLife, aPtclLifeRnd,
                                space);
                }

                for (xcnt = count; xcnt >= 1; xcnt--) {
                    f32 xbase = xcnt * div - 1.0f;
                    f32 x = xbase * outerX;

                    f32 maxdepth =
                        1.0f - MAX(std::fabs(xbase), std::fabs(ybase));
                    f32 distance = MAX(std::fabs(xbase), std::fabs(ybase));

                    math::VEC3 p;
                    math::VEC3 normal;

                    p.x = x;
                    p.y = y;
                    p.z = outerZ;

                    if (inner_rev != 0.0f) {
                        f32 rnd;
                        if (optionFlag & EmitterDesc::EMIT_FLAG_24) {
                            rnd = em->mRandom.RandFloat();
                            rnd *= rnd;
                            rnd = 1.0f - rnd * inner_rev * maxdepth;
                        } else {
                            rnd = 1.0f - (em->mRandom.RandFloat() * inner_rev *
                                          maxdepth);
                        }

                        p.z *= rnd;
                    }

                    if (diffusionNormal == 0.0f ||
                        (std::fabs(p.x) < 1e-5f && std::fabs(p.y) < 1e-5f)) {

                        normal.x = 0.0f;
                        normal.y = 0.0f;
                        normal.z = 1.0f;
                    } else {
                        f32 cone = distance * diffusionNormal;
                        f32 ylen = math::FInv(std::tanf(cone));

                        normal.x = p.x;
                        normal.y = p.y;
                        normal.z = ylen * math::FSqrt(p.x * p.x + p.y * p.y);
                    }

                    EmissionSub(p, normal, em, pm, aPtclLife, aPtclLifeRnd,
                                space);
                }

                for (xcnt = count; xcnt >= 1; xcnt--) {
                    f32 xbase = xcnt * div - 1.0f;
                    f32 x = xbase * outerZ;

                    f32 maxdepth =
                        1.0f - MAX(std::fabs(xbase), std::fabs(ybase));
                    f32 distance = MAX(std::fabs(xbase), std::fabs(ybase));

                    math::VEC3 p;
                    math::VEC3 normal;

                    p.x = -outerX;
                    p.y = y;
                    p.z = x;

                    if (inner_rev != 0.0f) {
                        f32 rnd;
                        if (optionFlag & OPTION_CUBE_FLAT_DENSITY) {
                            rnd = em->mRandom.RandFloat();
                            rnd *= rnd;
                            rnd = 1.0f - rnd * inner_rev * maxdepth;
                        } else {
                            rnd = 1.0f - (em->mRandom.RandFloat() * inner_rev *
                                          maxdepth);
                        }

                        p.x *= rnd;
                    }

                    if (diffusionNormal == 0.0f ||
                        (std::fabs(p.y) < 1e-5f && std::fabs(p.z) < 1e-5f)) {

                        normal.x = -1.0f;
                        normal.y = 0.0f;
                        normal.z = 0.0f;
                    } else {
                        f32 cone = distance * diffusionNormal;
                        f32 ylen = math::FInv(std::tanf(cone));

                        normal.x = ylen * -math::FSqrt(p.y * p.y + p.z * p.z);
                        normal.y = p.y;
                        normal.z = p.z;
                    }

                    EmissionSub(p, normal, em, pm, aPtclLife, aPtclLifeRnd,
                                space);
                }
            }

            {
                int i;
                int xcnt = 1;
                int ycnt = 1;

                int dir = 2;
                int dircntLife = count - 1;
                int dircntRemain = dircntLife;

                for (i = 0; i < count * count; i++) {
                    if (i != 0) {
                        switch (dir) {
                        case 0: {
                            xcnt--;
                            break;
                        }

                        case 1: {
                            ycnt++;
                            break;
                        }

                        case 2: {
                            xcnt++;
                            break;
                        }

                        case 3: {
                            ycnt--;
                            break;
                        }
                        }

                        if (--dircntRemain <= 0) {
                            if (dir == 0) {
                                dir = 3;
                            } else {
                                dir--;
                            }

                            if (dir % 2 == 1 &&
                                (dir != 1 || dircntLife != count - 1)) {

                                dircntLife--;
                            }

                            dircntRemain = dircntLife;
                        }
                    }

                    f32 xbase = xcnt * div - 1.0f;
                    f32 ybase = ycnt * div - 1.0f;

                    f32 x = xbase * outerX;
                    f32 y = ybase * outerZ;

                    f32 maxdepth =
                        1.0f - MAX(std::fabs(xbase), std::fabs(ybase));
                    f32 distance = MAX(std::fabs(xbase), std::fabs(ybase));

                    math::VEC3 p;
                    math::VEC3 normal;

                    p.x = x;
                    p.y = outerY;
                    p.z = y;

                    if (inner_rev != 0.0f) {
                        f32 rnd;
                        if (optionFlag & EmitterDesc::EMIT_FLAG_24) {
                            rnd = em->mRandom.RandFloat();
                            rnd *= rnd;
                            rnd = 1.0f - rnd * inner_rev * maxdepth;
                        } else {
                            rnd = 1.0f - (em->mRandom.RandFloat() * inner_rev *
                                          maxdepth);
                        }

                        p.y *= rnd;
                    }

                    if (diffusionNormal == 0.0f ||
                        (std::fabs(p.x) < 1e-5f && std::fabs(p.z) < 1e-5f)) {

                        normal.x = 0.0f;
                        normal.y = 1.0f;
                        normal.z = 0.0f;
                    } else {
                        f32 cone = distance * diffusionNormal;
                        f32 ylen = math::FInv(std::tanf(cone));

                        normal.x = p.x;
                        normal.y = ylen * math::FSqrt(p.x * p.x + p.z * p.z);
                        normal.z = p.z;
                    }

                    EmissionSub(p, normal, em, pm, aPtclLife, aPtclLifeRnd,
                                space);
                }
            }
        }

        return;
    }

    for (int i = 0; i < count; i++) {
        f32 x, y, z;

        if (inner == 0.0f) {
            x = em->mRandom.RandFloat() * 2.0f - 1.0f;
            y = em->mRandom.RandFloat() * 2.0f - 1.0f;
            z = em->mRandom.RandFloat() * 2.0f - 1.0f;
        } else if (inner == 1.0f) {
            switch (em->mRandom.Rand() % 6) {
            case 0: {
                x = em->mRandom.RandFloat() * 2.0f - 1.0f;
                y = em->mRandom.RandFloat() * 2.0f - 1.0f;
                z = 1.0f;
                break;
            }

            case 1: {
                x = em->mRandom.RandFloat() * 2.0f - 1.0f;
                y = em->mRandom.RandFloat() * 2.0f - 1.0f;
                z = -1.0f;
                break;
            }

            case 2: {
                x = em->mRandom.RandFloat() * 2.0f - 1.0f;
                y = 1.0f;
                z = em->mRandom.RandFloat() * 2.0f - 1.0f;
                break;
            }

            case 3: {
                x = em->mRandom.RandFloat() * 2.0f - 1.0f;
                y = -1.0f;
                z = em->mRandom.RandFloat() * 2.0f - 1.0f;
                break;
            }

            case 4: {
                x = 1.0f;
                y = em->mRandom.RandFloat() * 2.0f - 1.0f;
                z = em->mRandom.RandFloat() * 2.0f - 1.0f;
                break;
            }

            case 5: {
                x = -1.0f;
                y = em->mRandom.RandFloat() * 2.0f - 1.0f;
                z = em->mRandom.RandFloat() * 2.0f - 1.0f;
                break;
            }
            }
        } else {
            f32 topbottom = outerX * outerY * inner_rev * outerZ;
            f32 frontback = outerX * outerY * inner * outerZ * inner_rev;
            f32 leftright =
                outerX * inner_rev * outerY * inner * outerZ * inner;

            f32 f =
                em->mRandom.RandFloat() * (topbottom + frontback + leftright);

            if (f < topbottom) {
                x = em->mRandom.RandFloat() * 2.0f - 1.0f;

                y = (em->mRandom.RandFloat() * 2.0f - 1.0f) * inner_rev;
                y = y >= 0.0f ? 1.0f - y : -1.0f - y;

                z = em->mRandom.RandFloat() * 2.0f - 1.0f;
            } else if (f < topbottom + frontback) {
                x = em->mRandom.RandFloat() * 2.0f - 1.0f;
                y = (em->mRandom.RandFloat() * 2.0f - 1.0f) * inner;

                z = (em->mRandom.RandFloat() * 2.0f - 1.0f) * inner_rev;
                z = z >= 0.0f ? 1.0f - z : -1.0f - z;
            } else {
                x = (em->mRandom.RandFloat() * 2.0f - 1.0f) * inner_rev;
                x = x >= 0.0f ? 1.0f - x : -1.0f - x;

                y = (em->mRandom.RandFloat() * 2.0f - 1.0f) * inner;
                z = (em->mRandom.RandFloat() * 2.0f - 1.0f) * inner;
            }
        }

        math::VEC3 p;
        math::VEC3 normal(0.0f, 0.0f, 0.0f);

        if (y >= 0.0f && y >= std::fabs(x) && y >= std::fabs(z)) {
            normal.y = 1.0f;
        } else if (y < 0.0f && -y >= std::fabs(x) && -y >= std::fabs(z)) {
            normal.y = -1.0f;
        } else if (x >= 0.0f && x >= std::fabs(y) && x >= std::fabs(z)) {
            normal.x = 1.0f;
        } else if (x < 0.0f && -x >= std::fabs(y) && -x >= std::fabs(z)) {
            normal.x = -1.0f;
        } else if (z >= 0.0f) {
            normal.z = 1.0f;
        } else {
            normal.z = -1.0f;
        }

        p.x = x * outerX;
        p.y = y * outerY;
        p.z = z * outerZ;

        if (diffusionNormal != 0.0f) {
            f32 distance;

            if (normal.x != 0.0f) {
                distance = MAX(std::fabs(p.y), std::fabs(p.z));

                f32 cone = distance * diffusionNormal;
                f32 ylen = math::FInv(std::tanf(cone));

                normal.x *= ylen * math::FSqrt(p.y * p.y + p.z * p.z);
                normal.y = p.y;
                normal.z = p.z;
            } else if (normal.y != 0.0f) {
                distance = MAX(std::fabs(p.x), std::fabs(p.z));

                f32 cone = distance * diffusionNormal;
                f32 ylen = math::FInv(std::tanf(cone));

                normal.x = p.x;
                normal.y *= ylen * math::FSqrt(p.x * p.x + p.z * p.z);
                normal.z = p.z;
            } else {
                distance = MAX(std::fabs(p.x), std::fabs(p.y));

                f32 cone = distance * diffusionNormal;
                f32 ylen = math::FInv(std::tanf(cone));

                normal.x = p.x;
                normal.y = p.y;
                normal.z *= ylen * math::FSqrt(p.x * p.x + p.y * p.y);
            }
        }

        EmissionSub(p, normal, em, pm, aPtclLife, aPtclLifeRnd, space);
    }
}

} // namespace ef
} // namespace nw4r
