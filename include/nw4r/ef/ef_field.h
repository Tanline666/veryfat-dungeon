#ifndef NW4R_EF_FIELD_H
#define NW4R_EF_FIELD_H

#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_random.h>
#include <nw4r/ef/ef_res_animcurve.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/math.h>

namespace nw4r {
namespace ef {

struct InfoField {

    enum Space {
        SPACE_GLOBAL,
        SPACE_EMITTER,
        SPACE_PARTICLEMANAGER,
        SPACE_GLOBAL_WITH_SCALE
    };

    enum Affect {
        AFFECT_VELOCITY,
        AFFECT_SPEED,
    };

    enum OptionRandom {
        OPTION_RANDOM_SAVEVELOCITY = 1 << 0,
        OPTION_RANDOM_ALLDIRECTION = 1 << 1,
        OPTION_RANDOM_ENABLE_X = 1 << 2,
        OPTION_RANDOM_ENABLE_Y = 1 << 3,
        OPTION_RANDOM_ENABLE_Z = 1 << 4
    };

    u8 space;     //!< \~ // at 0x0
    u8 addTarget; //!< \~ // at 0x1
    u8 padding;   //!< \~ // at 0x2
    u8 option;    //!< \~ // at 0x3
    union {
        struct InfoFieldSpeed {
            f32 speed; //!< \~ // at 0x0
        };

        struct InfoFieldGravity {
            f32 power;          //!< \~ // at 0x0
            math::_VEC3 rotate; //!< \~ // at 0x4
        };

        struct InfoFieldRandom {
            f32 power;     //!< \~ // at 0x0
            f32 diffusion; //!< \~ // at 0x4
            u16 interval;  //!< \~ // at 0x8
            u8 padding[2]; //!< \~ // at 0xA
        };

        struct InfoFieldSpin {
            f32 speed;          //!< \~ // at 0x0
            math::_VEC3 rotate; //!< \~ // at 0x4
        };

        struct InfoFieldMagnet {
            f32 power;             //!< \~ // at 0x0
            math::_VEC3 translate; //!< \~ // at 0x4
        };

        struct InfoFieldNewton {
            f32 power;             //!< \~ // at 0x0
            f32 refDistance;       //!< \~ // at 0x4
            math::_VEC3 translate; //!< \~ // at 0x8
        };

        struct InfoFieldVortex {
            f32 innerSpeed;     //!< \~ // at 0x0
            f32 outerSpeed;     //!< \~ // at 0x4
            f32 distance;       //!< \~ // at 0x8
            math::_VEC3 rotate; //!< \~ // at 0xC
        };

        struct InfoFieldTail {
            f32 power; //!< \~ // at 0x0
        };

        f32 animParam;
        InfoFieldSpeed speed;
        InfoFieldGravity gravity;
        InfoFieldRandom random;
        InfoFieldSpin spin;
        InfoFieldMagnet magnet;
        InfoFieldNewton newton;
        InfoFieldVortex vortex;
        InfoFieldTail tail;
    } u; //!< \~ // at 0x4
};

/**
 * \~english @brief Calculates an acceleration field for particles.
 * \~japanese @brief パーティクルの加減速フィールドを計算する。
 *
 * \~english @param affect Reference to buffer storing calculation results
 * \~japanese @param affect 計算結果を格納するバッファへの参照
 * @param infoCopy
 * \~english @param prevVelocity Reference to initial velocity
 * \~japanese @param prevVelocity 初速への参照
 * \~english @return Always TRUE
 * \~japanese @return 常にTRUE
 */
inline bool CalcFieldSpeed(math::VEC3& affect, InfoField& infoCopy,
                           math::VEC3& prevVelocity) {
    math::VEC3Scale(&affect, &prevVelocity, infoCopy.u.speed.speed - 1.0f);
    return TRUE;
}

/**
 * \~english @brief Calculates a gravitational field for particles.
 * \~japanese @brief パーティクルの重力フィールドを計算する。
 *
 * \~english @param affect Reference to buffer storing calculation results
 * \~japanese @param affect 計算結果を格納するバッファへの参照
 * @param infoCopy
 * \~english @return Always TRUE
 * \~japanese @return 常にTRUE
 */
inline bool CalcFieldGravity(math::VEC3& affect, InfoField& infoCopy) {
    Rotation2VecY(infoCopy.u.gravity.rotate, &affect);
    affect *= infoCopy.u.gravity.power;
    return TRUE;
}

inline bool CalcFieldRandom(math::VEC3& affect, InfoField& infoCopy,
                            const AnimCurveHeader* header, const u8* ptr,
                            const Particle* pp, u32 tick, u16 randSeed,
                            math::VEC3& prevVelocity, math::VEC3& prevDir,
                            math::MTX34& mtxLocalToEmitterNoTrans) {

    u32 rand = RANDOM_INIT(randSeed, header->randomSeed, tick, 0);

    if (!(ptr[4] & AC_PROC_FLAG_TIMING)) {
        if (!tick || tick % (infoCopy.u.random.interval + 1)) {
            return FALSE;
        }
    } else {
        if (pp->mTick && tick % (infoCopy.u.random.interval + 1)) {
            return FALSE;
        }
    }

    if (infoCopy.option & InfoField::OPTION_RANDOM_SAVEVELOCITY) {
        math::VEC3 dir;
        switch (infoCopy.addTarget) {
        case InfoField::AFFECT_VELOCITY:
            dir = prevVelocity;
            break;

        case InfoField::AFFECT_SPEED:
            if (!pp->mTick) {
                dir = prevVelocity;
            } else {
                dir = prevDir / pp->mParameter.mMomentum;
            }
            break;
        }

        math::VEC3Transform(&dir, &mtxLocalToEmitterNoTrans, &dir);
        infoCopy.u.random.power = math::VEC3Len(&dir);
    }

    if (infoCopy.option & InfoField::OPTION_RANDOM_ALLDIRECTION) {
        affect.x = (rand >> 16) / 32768.0f;
        rand = RANDOM_NEXT(rand);
        affect.y = (rand >> 16) / 32768.0f;
        rand = RANDOM_NEXT(rand);
        affect.z = (rand >> 16) / 32768.0f;
        rand = RANDOM_NEXT(rand);

        if (infoCopy.option & InfoField::OPTION_RANDOM_SAVEVELOCITY) {
            if (!Normalize(&affect, &affect)) {
                affect.y = 1.0f;
            }
        }

        math::VEC3Scale(&affect, &affect, infoCopy.u.random.power);
    } else {
        math::VEC3 dir;
        if (TRUE) {
            if (!pp->mTick) {
                dir = prevVelocity;
            } else {
                dir = prevDir;
            }
            math::VEC3Transform(&dir, &mtxLocalToEmitterNoTrans, &dir);
        }

        if (!Normalize(&dir, &dir)) {
            dir.y = 1.0f;
        }

        math::MTX34 mtx;
        GetDirMtxY(&mtx, dir);

        if (infoCopy.u.random.diffusion) {
            f32 xrot = (rand >> 16) / 65535.0f * infoCopy.u.random.diffusion;
            rand = RANDOM_NEXT(rand);
            f32 yrot = (rand >> 16) / 65535.0f * NW4R_MATH_PI * 2.0f;
            rand = RANDOM_NEXT(rand);

            _PSSinCosRad(&affect.x, xrot);
            affect.z = affect.x;
            f32 sn, cs;
            PSSinCosRad(&sn, &cs, yrot);
            affect.x *= sn;
            affect.z *= cs;

            if (infoCopy.option & InfoField::OPTION_RANDOM_SAVEVELOCITY) {
                math::VEC3Scale(&affect, &affect, infoCopy.u.random.power);
            } else {
                math::VEC3Scale(&affect, &affect,
                                (rand >> 16) / 65535.0f *
                                    infoCopy.u.random.power);
            }
        } else {
            affect.x = 0;

            if (infoCopy.option & InfoField::OPTION_RANDOM_SAVEVELOCITY) {
                affect.y = infoCopy.u.random.power;
            } else {
                affect.y = (rand >> 16) / 32768.0f * infoCopy.u.random.power;
            }

            affect.z = 0;
        }
        math::VEC3Transform(&affect, &mtx, &affect);
    }

    if (!(infoCopy.option & InfoField::OPTION_RANDOM_ENABLE_X)) {
        affect.x = 0;
    }

    if (!(infoCopy.option & InfoField::OPTION_RANDOM_ENABLE_Y)) {
        affect.y = 0;
    }

    if (!(infoCopy.option & InfoField::OPTION_RANDOM_ENABLE_Z)) {
        affect.z = 0;
    }

    return TRUE;
}

inline bool CalcFieldSpin(math::VEC3& affect, InfoField& infoCopy,
                          const math::VEC3& ptclPos) {
    math::VEC3 forcedir;
    Rotation2VecY(infoCopy.u.spin.rotate, &forcedir);

    math::MTX34 mtx;
    math::MTX34RotAxisRad(&mtx, &forcedir, infoCopy.u.spin.speed);

    math::VEC3Transform(&affect, &mtx, &ptclPos);
    math::VEC3Sub(&affect, &affect, &ptclPos);

    return TRUE;
}

inline bool CalcFieldMagnet(math::VEC3& affect, InfoField& infoCopy,
                            math::VEC3& ptclPos) {
    math::VEC3Sub(&affect,
                  reinterpret_cast<math::VEC3*>(&infoCopy.u.magnet.translate),
                  &ptclPos);
    Normalize(&affect, &affect);
    math::VEC3Scale(&affect, &affect, infoCopy.u.magnet.power);

    return TRUE;
}

inline bool CalcFieldNewton(math::VEC3& affect, InfoField& infoCopy,
                            const math::VEC3& ptclPos) {
    math::VEC3Sub(&affect,
                  reinterpret_cast<math::VEC3*>(&infoCopy.u.newton.translate),
                  &ptclPos);

    f32 len = math::VEC3LenSq(&affect);

    Normalize(&affect, &affect);
    math::VEC3Scale(&affect, &affect, infoCopy.u.newton.power);

    f32 distanceSq =
        infoCopy.u.newton.refDistance * infoCopy.u.newton.refDistance;
    if (len > distanceSq) {
        math::VEC3Scale(&affect, &affect, distanceSq / len);
    }

    return TRUE;
}

inline bool CalcFieldVortex(math::VEC3& affect, InfoField& infoCopy,
                            const math::VEC3& ptclPos) {
    math::VEC3 forcedir;
    Rotation2VecY(infoCopy.u.vortex.rotate, &forcedir);

    infoCopy.u.vortex.distance *= infoCopy.u.vortex.distance;

    math::VEC3 fvec;
    math::VEC3Scale(&fvec, &forcedir, math::VEC3Dot(&forcedir, &ptclPos));
    math::VEC3Sub(&fvec, &ptclPos, &fvec);

    f32 len = math::VEC3LenSq(&fvec);
    if (!len) {
        return FALSE;
    }

    f32 power;
    if (len >= infoCopy.u.vortex.distance) {
        power = infoCopy.u.vortex.outerSpeed;
    } else {
        power = len / infoCopy.u.vortex.distance;
        power = (1.0f - power) * infoCopy.u.vortex.innerSpeed +
                power * infoCopy.u.vortex.outerSpeed;
    }

    Normalize(&fvec, &fvec);
    math::VEC3Cross(&affect, &fvec, &forcedir);
    math::VEC3Scale(&affect, &affect, power);

    return TRUE;
}

} // namespace ef
} // namespace nw4r

#endif
