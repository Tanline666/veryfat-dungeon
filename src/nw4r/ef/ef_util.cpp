#include <nw4r/ef.h>
#include <nw4r/math.h>
#include <nw4r/ut.h>

#include <cmath>

namespace nw4r {
namespace ef {

static const f32 EMITTER_ZERO_SCALE = 1e-5;
static f32 sPS_Three_Half[2] = {3.0f, 0.5f};

#define PS_SIN_COS_RAD_3_REGISTER(valX, valY, valZ, sinCosX, sinCosY, sinCosZ) \
    {                                                                          \
        register u32 idxXY;                                                    \
        register u32 idxZ;                                                     \
                                                                               \
        const register f32* pTblBase =                                         \
            reinterpret_cast<const f32*>(&nw4r::math::detail::gSinCosTbl[0]);  \
        register f32* pTbl;                                                    \
        register f32* pTblY;                                                   \
                                                                               \
        register __vec2x32float__ c_idxmax = 65536.0f;                         \
        register __vec2x32float__ c_idxmaxneg;                                 \
        register f32 c_radToFidx = 256.0f / (2 * NW4R_MATH_PI);                \
        register __vec2x32float__ c_zero;                                      \
                                                                               \
        register __vec2x32float__ abs_fidxXY;                                  \
        register f32 abs_fidxZ;                                                \
        register __vec2x32float__ fidxXY;                                      \
        register f32 fidxZ;                                                    \
                                                                               \
        register __vec2x32float__ r;                                           \
        register __vec2x32float__ scval, scdel;                                \
        register f32 work;                                                     \
                                                                               \
        register f32* pTmp;                                                    \
        f32 tmp;                                                               \
        pTmp = &tmp;                                                           \
                                                                               \
        asm { \
            ps_merge00 c_idxmax, c_idxmax, c_idxmax; \
            ps_neg c_idxmaxneg, c_idxmax; \
            ps_sub c_zero, c_idxmax, c_idxmax; \
 \
 \
            ps_merge00 fidxXY, valX, valY; \
            ps_muls0 fidxXY, fidxXY, c_radToFidx; \
 \
            ps_abs abs_fidxXY, fidxXY; \
 \
            ps_cmpu0 cr0, abs_fidxXY, c_idxmax; \
            ble fidxendx; \
fidxloopx: \
            ps_sum0 abs_fidxXY, abs_fidxXY, abs_fidxXY, c_idxmaxneg; \
            ps_cmpu0 cr0, abs_fidxXY, c_idxmax; \
            bge fidxloopx; \
fidxendx: \
 \
            ps_cmpu1 cr0, abs_fidxXY, c_idxmax; \
            ble fidxendy; \
            ps_merge10 abs_fidxXY, abs_fidxXY, abs_fidxXY; \
fidxloopy: \
            ps_sum0 abs_fidxXY, abs_fidxXY, abs_fidxXY, c_idxmaxneg; \
            ps_cmpu0 cr0, abs_fidxXY, c_idxmax; \
            bge fidxloopy; \
            ps_merge10 abs_fidxXY, abs_fidxXY, abs_fidxXY; \
fidxendy: \
 \
            psq_st abs_fidxXY, 0(pTmp), 0, 3; \
            fmuls fidxZ, valZ, c_radToFidx; \
            psq_l r, 0(pTmp), 0, 3; \
            fabs abs_fidxZ, fidxZ; \
            fcmpu cr0, abs_fidxZ, c_idxmax; \
            lwz idxXY, 0(pTmp); \
            ble fidxendz; \
fidxloopz: \
            fsubs abs_fidxZ, abs_fidxZ, c_idxmax; \
            fcmpu cr0, abs_fidxZ, c_idxmax; \
            bge fidxloopz; \
fidxendz: \
            psq_st abs_fidxZ, 0(pTmp), 1, 3; \
            ps_sub r, abs_fidxXY, r; \
 \
            rlwinm pTbl, idxXY, 20, 20, 27; \
            add pTbl, pTblBase, pTbl; \
 \
            psq_l scval, 0(pTbl), 0, 0; \
            rlwinm pTblY, idxXY, 4, 20, 27; \
            psq_l scdel, 8(pTbl), 0, 0; \
            add pTblY, pTblBase, pTblY; \
            ps_madds0 sinCosX, scdel, r, scval; \
 \
            psq_l scval, 0(pTblY), 0, 0; \
            ps_cmpu0 cr0, fidxXY, c_zero; \
            psq_l scdel, 8(pTblY), 0, 0; \
            lhz idxZ, 0(pTmp); \
 \
            bge sincosendx; \
            ps_neg work, sinCosX; \
            ps_merge01 sinCosX, work, sinCosX; \
sincosendx: \
            ps_madds1 sinCosY, scdel, r, scval; \
 \
            psq_l r, 0(pTmp), 1, 3; \
 \
            rlwinm idxZ, idxZ, 4, 20, 27; \
            ps_cmpu1 cr0, fidxXY, c_zero; \
            add pTbl, pTblBase, idxZ; \
            psq_l scval, 0(pTbl), 0, 0; \
            fsubs r, abs_fidxZ, r; \
            psq_l scdel, 8(pTbl), 0, 0; \
 \
            bge sincosendy; \
            ps_neg work, sinCosY; \
            ps_merge01 sinCosY, work, sinCosY; \
sincosendy: \
 \
            ps_madds0 sinCosZ, scdel, r, scval; \
 \
            fcmpu cr0, fidxZ, c_zero; \
            bge sincosendz; \
            ps_neg work, sinCosZ; \
            ps_merge01 sinCosZ, work, sinCosZ; \
sincosendz:                                                               \
        }                                                                      \
    }

//! Dummy function for sdata2 emission order. This is not used anywhere
//! in favour of the macro.
void _PSSinCosRad3(register f32* pRes, register f32 valX, register f32 valY,
                   register f32 valZ) {
    register u32 idxXY;
    register u32 idxZ;

    const register f32* pTblBase =
        reinterpret_cast<const f32*>(&nw4r::math::detail::gSinCosTbl[0]);
    register f32* pTbl;
    register f32* pTblY;

    register __vec2x32float__ c_idxmax = 65536.0F;
    register __vec2x32float__ c_idxmaxneg;
    register f32 c_radToFidx = 256.0F / (2 * NW4R_MATH_PI);
    register __vec2x32float__ c_zero;

    register __vec2x32float__ abs_fidxXY;
    register f32 abs_fidxZ;
    register __vec2x32float__ fidxXY;
    register f32 fidxZ;

    register __vec2x32float__ r;
    register __vec2x32float__ result;
    register __vec2x32float__ scval, scdel;
    register f32 work;

    asm
    {
        ps_merge00 c_idxmax, c_idxmax, c_idxmax;
        ps_neg c_idxmaxneg, c_idxmax;
        ps_sub c_zero, c_idxmax, c_idxmax;


        ps_merge00 fidxXY, valX, valY;
        ps_muls0 fidxXY, fidxXY, c_radToFidx;

        ps_abs abs_fidxXY, fidxXY;

        ps_cmpu0 cr0, abs_fidxXY, c_idxmax;
        ble fidxendx;
fidxloopx:
        ps_sum0 abs_fidxXY, abs_fidxXY, abs_fidxXY, c_idxmaxneg;
        ps_cmpu0 cr0, abs_fidxXY, c_idxmax;
        bge fidxloopx;
fidxendx:

        ps_cmpu1 cr0, abs_fidxXY, c_idxmax;
        ble fidxendy;
        ps_merge10 abs_fidxXY, abs_fidxXY, abs_fidxXY;
fidxloopy:
        ps_sum0 abs_fidxXY, abs_fidxXY, abs_fidxXY, c_idxmaxneg;
        ps_cmpu0 cr0, abs_fidxXY, c_idxmax;
        bge fidxloopy;
        ps_merge10 abs_fidxXY, abs_fidxXY, abs_fidxXY;
fidxendy:

        psq_st abs_fidxXY, 16(pRes), 0, 3;
        fmuls fidxZ, valZ, c_radToFidx;
        psq_l r, 16(pRes), 0, 3;
        fabs abs_fidxZ, fidxZ;
        fcmpu cr0, abs_fidxZ, c_idxmax;
        lwz idxXY, 16(pRes);
        ble fidxendz;
fidxloopz:
        fsubs abs_fidxZ, abs_fidxZ, c_idxmax;
        fcmpu cr0, abs_fidxZ, c_idxmax;
        bge fidxloopz;
fidxendz:
        psq_st abs_fidxZ, 16(pRes), 1, 3;
        ps_sub r, abs_fidxXY, r;

        rlwinm pTbl, idxXY, 20, 20, 27;
        add pTbl, pTblBase, pTbl;

        psq_l scval, 0(pTbl), 0, 0;
        rlwinm pTblY, idxXY, 4, 20, 27;
        psq_l scdel, 8(pTbl), 0, 0;
        add pTblY, pTblBase, pTblY;
        ps_madds0 result, scdel, r, scval;

        psq_l scval, 0(pTblY), 0, 0;
        ps_cmpu0 cr0, fidxXY, c_zero;
        psq_l scdel, 8(pTblY), 0, 0;
        lhz idxZ, 16(pRes);

        bge sincosendx;
        ps_neg work, result;
        ps_merge01 result, work, result;
sincosendx:
        psq_st result, 0(pRes), 0, 0;

        ps_madds1 result, scdel, r, scval;

        psq_l r, 16(pRes), 1, 3;

        rlwinm idxZ, idxZ, 4, 20, 27;
        ps_cmpu1 cr0, fidxXY, c_zero;
        add pTbl, pTblBase, idxZ;
        psq_l scval, 0(pTbl), 0, 0;
        fsubs r, abs_fidxZ, r;
        psq_l scdel, 8(pTbl), 0, 0;

        bge sincosendy;
        ps_neg work, result;
        ps_merge01 result, work, result;
sincosendy:
          psq_st result, 8(pRes), 0, 0;

            ps_madds0 result, scdel, r, scval;

            fcmpu cr0, fidxZ, c_zero;
            bge sincosendz;
            ps_neg work, result;
            ps_merge01 result, work, result;
sincosendz:
            psq_st result, 16(pRes), 0, 0;
    }
}

u16 UtlistToArray(const ut::List* pList, void** ppArray, int maxlen) {
    u16 num = 0;

    NW4R_UT_LIST_FOREACH (void, it, *pList, {
        ppArray[num++] = it;

        if (num >= maxlen) {
            break;
        }
    })

    return num;
}

void GetDirMtxY(math::MTX34* pMtx, const math::VEC3& rVec) {
    f32 sx, cx;
    f32 sz, cz;

    sx = rVec.z;

    if (1.0f - math::FAbs(sx) < NW4R_MATH_FLT_EPSILON) {
        cx = 0;
        sz = 0.0f;
        cz = 1.0f;
    } else {
        cx = 1.0f - sx * sx;
        cx = math::FSqrt(cx);
        cz = rVec.y / cx;
        sz = rVec.x / -cx;
    }

    pMtx->_00 = cz;
    pMtx->_01 = rVec.x;
    pMtx->_02 = sx * sz;
    pMtx->_03 = 0.0f;

    pMtx->_10 = sz;
    pMtx->_11 = rVec.y;
    pMtx->_12 = -cz * sx;
    pMtx->_13 = 0.0f;

    pMtx->_20 = 0.0f;
    pMtx->_21 = rVec.z;
    pMtx->_22 = cx;
    pMtx->_23 = 0.0f;
}

void MtxGetRotationMtx(const math::MTX34& rMtx, math::MTX34* pDst) {
    math::VEC3 x, y, z;

    MtxColVec(&rMtx, 0, &x);
    if (!Normalize(&x, &x)) {
        x.x = 1.0f;
    }

    MtxColVec(&rMtx, 1, &y);
    if (!Normalize(&y, &y)) {
        y.y = 1.0f;
    }

    math::VEC3Cross(&z, &x, &y);
    math::VEC3Cross(&y, &z, &x);

    MtxSetColVec(pDst, 0, &x);
    MtxSetColVec(pDst, 1, &y);
    MtxSetColVec(pDst, 2, &z);

    pDst->_03 = 0.0f;
    pDst->_13 = 0.0f;
    pDst->_23 = 0.0f;
}

void MtxSetColVec(math::MTX34* pMtx, int col, const math::VEC3* pVec) {
    pMtx->m[0][col] = pVec->x;
    pMtx->m[1][col] = pVec->y;
    pMtx->m[2][col] = pVec->z;
}

math::VEC3* MtxColVec(const math::MTX34* pMtx, int col, math::VEC3* pVec) {
    pVec->x = pMtx->m[0][col];
    pVec->y = pMtx->m[1][col];
    pVec->z = pMtx->m[2][col];

    return pVec;
}

void MtxGetRotation(const math::MTX34& rMtx, math::VEC3* pRot) {
    f32 C;
    f32 sx, sy, sz;
    f32 f;

    sx = MTXColLen(&rMtx, 0);
    if (sx < NW4R_MATH_FLT_MIN) {
        goto DEFAULT;
    }

    sy = MTXColLen(&rMtx, 1);
    if (sy < NW4R_MATH_FLT_MIN) {
        goto DEFAULT;
    }

    sz = MTXColLen(&rMtx, 2);
    if (sz < NW4R_MATH_FLT_MIN) {
        goto DEFAULT;
    }

    f = -rMtx._20 / sx;

    if (f > 1.0f) {
        f = 1.0f;
    }

    if (f < -1.0f) {
        f = -1.0f;
    }

    pRot->y = std::asinf(f);
    C = math::CosRad(pRot->y);
    if (C >= NW4R_MATH_FLT_MIN) {
        pRot->x = std::atan2f(rMtx._21 / sy, rMtx._22 / sz);
        pRot->z = std::atan2f(rMtx._10, rMtx._00);
    } else {
        pRot->x = std::atan2f(rMtx._01, rMtx._11);
        pRot->z = 0.0f;
    }
    return;

DEFAULT:
    pRot->x = 0.0f;
    pRot->y = 0.0f;
    pRot->z = 0.0f;
}

void MtxGetTranslate(const math::MTX34& rMtx, math::VEC3* pTrans) {
    pTrans->x = rMtx._03;
    pTrans->y = rMtx._13;
    pTrans->z = rMtx._23;
}

void MtxGetScale(const math::MTX34& rMtx, math::VEC3* pScale) {
    f32 sh[3];
    math::VEC3 v0, v1, v2, v;
    f32 mag;

    MtxColVec(&rMtx, 0, &v0);
    mag = math::VEC3LenSq(&v0);

    if (mag > NW4R_MATH_FLT_EPSILON) {
        mag = math::FrSqrt(mag);
        pScale->x = math::FInv(mag);
        math::VEC3Scale(&v0, &v0, mag);

        MtxColVec(&rMtx, 1, &v1);
        sh[0] = math::VEC3Dot(&v0, &v1);
        math::VEC3Scale(&v, &v0, sh[0]);
        math::VEC3Sub(&v1, &v1, &v);
        mag = math::VEC3LenSq(&v1);

        if (mag > NW4R_MATH_FLT_EPSILON) {
            mag = math::FrSqrt(mag);
            pScale->y = math::FInv(mag);
            sh[0] *= mag;
            math::VEC3Scale(&v1, &v1, mag);

            MtxColVec(&rMtx, 2, &v2);
            sh[2] = math::VEC3Dot(&v1, &v2);
            math::VEC3Scale(&v, &v1, sh[2]);
            math::VEC3Sub(&v2, &v2, &v);

            sh[1] = math::VEC3Dot(&v0, &v2);
            math::VEC3Scale(&v, &v0, sh[1]);
            math::VEC3Sub(&v2, &v2, &v);

            mag = math::VEC3LenSq(&v2);

            if (mag > NW4R_MATH_FLT_EPSILON) {
                pScale->z = math::FSqrt(mag);
                math::VEC3Cross(&v, &v1, &v2);

                if (math::VEC3Dot(&v0, &v) < 0.0f) {
                    // Double precision
                    pScale->x *= -1.0;
                    pScale->y *= -1.0;
                    pScale->z *= -1.0;
                }
            } else {
                pScale->z = 0.0f;
            }
        } else {
            pScale->y = 0.0f;
            MtxColVec(&rMtx, 2, &v2);

            sh[1] = math::VEC3Dot(&v0, &v2);
            math::VEC3Scale(&v, &v0, sh[1]);
            math::VEC3Sub(&v2, &v2, &v);

            mag = math::VEC3LenSq(&v2);

            if (mag > NW4R_MATH_FLT_EPSILON) {
                pScale->z = math::FSqrt(mag);
            } else {
                pScale->z = 0.0f;
            }
        }

    } else {
        pScale->x = 0.0f;

        MtxColVec(&rMtx, 1, &v1);
        mag = math::VEC3LenSq(&v1);

        if (mag > NW4R_MATH_FLT_EPSILON) {
            mag = math::FrSqrt(mag);
            pScale->y = math::FInv(mag);

            math::VEC3Scale(&v1, &v1, mag);
            MtxColVec(&rMtx, 2, &v2);

            sh[2] = math::VEC3Dot(&v1, &v2);
            math::VEC3Scale(&v, &v1, sh[2]);
            math::VEC3Sub(&v2, &v2, &v);
            pScale->z = math::VEC3Len(&v2);
        } else {
            pScale->y = 0.0f;

            MtxColVec(&rMtx, 2, &v2);
            pScale->z = math::VEC3Len(&v2);
        }
    }
}

bool Normalize(register math::VEC3* pDst, const register math::VEC3* pVec) {
    register f32 cHalf = 0.5f;
    register f32 cThree = 3.0f;
    register f32 cZero;
    register f32 v1_xy, v1_z;
    register f32 xx_zz, xx_yy;
    register f32 sqSum;
    register f32 rSqrt;
    register f32 nWork0, nWork1;

    asm {
        psq_l v1_xy, 0(pVec), 0, 0;
        ps_mul xx_yy, v1_xy, v1_xy;
        psq_l v1_z, 8(pVec), 1, 0;
        ps_madd xx_zz, v1_z, v1_z, xx_yy;
        fsubs cZero, cHalf, cHalf;
        ps_sum0 sqSum, xx_zz, v1_z, xx_yy;
        fcmpu cr0, sqSum, cZero;
        beq _exit_false;

        //! Estimate...
        frsqrte rSqrt, sqSum;

        //! Then refine with Newton-Raphson
        fmuls nWork0, rSqrt, rSqrt;
        fmuls nWork1, rSqrt, cHalf;
        fnmsubs nWork0, nWork0, sqSum, cThree;
        fmuls rSqrt, nWork0, nWork1;

        ps_muls0 v1_xy, v1_xy, rSqrt;
        psq_st v1_xy, 0(pDst), 0, 0;

        ps_muls0 v1_z, v1_z, rSqrt;
        psq_st v1_z, 8(pDst), 1, 0;
    }
    return TRUE;

    asm {
    _exit_false:
        psq_st v1_xy, 0(pDst), 0, 0;
        psq_st v1_z, 8(pDst), 1, 0;
    }
    return FALSE;
}

void _PSSinCosRad(register f32* pRes, register f32 value) {
    register f32 fIdx;
    register u32 idx;
    register f32 absFIdx;
    register f32 r;
    register f32 result;
    register f32 idxMax = 65536.0f;
    register f32 scaleVal, scaleDel;
    register f32 cRadToFIdx = 256.0f / (2 * M_PI);
    register f32 cZero;

    const register f32* pTbl =
        reinterpret_cast<const f32*>(&math::detail::gSinCosTbl[0]);

    asm {
        fmuls fIdx, value, cRadToFIdx;
        fabs absFIdx, fIdx;
        psq_st absFIdx, 0(pRes), 1, 3;
        fcmpu cr0, absFIdx, idxMax;
        ble fIdxEnd;
fIdxLoop:
        fsubs absFIdx, absFIdx, idxMax;
        fcmpu cr0, absFIdx, idxMax;
        bge fIdxLoop;
        psq_st absFIdx, 0(pRes), 1, 3;
fIdxEnd:
        lhz idx, 0(pRes);
        fsubs cZero, idxMax, idxMax;
        rlwinm idx, idx, 4, 20, 27;
        add pTbl, pTbl, idx;
        psq_l r, 0(pRes), 1, 3;
        fsubs r, absFIdx, r;

        psq_l scaleVal, 0(pTbl), 0, 0;
        psq_l scaleDel, 8(pTbl), 0, 0;
        ps_madds0 result, scaleDel, r, scaleVal;

        fcmpu cr0, fIdx, cZero;
        bge sinCosEnd;
        ps_neg fIdx, result;
        ps_merge01 result, fIdx, result;

sinCosEnd:
        psq_st result, 0(pRes), 0, 0;
    }
}

void PSSinCosRad(register f32* pSin, register f32* pCos, register f32 value) {
    register f32 fIdx;
    register u32 idx;
    register f32 absFIdx;
    register f32 r;
    register f32 result;
    register f32 idxMax = 65536.0f;
    register f32 scaleVal, scaleDel;
    register f32 cRadToFIdx = 256.0f / (2 * M_PI);
    register f32 cZero;

    const register f32* pTbl =
        reinterpret_cast<const f32*>(&math::detail::gSinCosTbl[0]);

    asm {
        fmuls fIdx, value, cRadToFIdx;
        fabs absFIdx, fIdx;
        psq_st absFIdx, 0(pSin), 1, 3;
        fcmpu cr0, absFIdx, idxMax;
        ble fIdxEnd;
fIdxLoop:
        fsubs absFIdx, absFIdx, idxMax;
        fcmpu cr0, absFIdx, idxMax;
        bge fIdxLoop;
        psq_st absFIdx, 0(pSin), 1, 3;
fIdxEnd:
        lhz idx, 0(pSin);
        fsubs cZero, idxMax, idxMax;
        rlwinm idx, idx, 4, 20, 27;
        add pTbl, pTbl, idx;
        psq_l r, 0(pSin), 1, 3;
        fsubs r, absFIdx, r;

        psq_l scaleVal, 0(pTbl), 0, 0;
        psq_l scaleDel, 8(pTbl), 0, 0;
        ps_madds0 result, scaleDel, r, scaleVal;

        ps_merge10 r, result, result;
        psq_st r, 0(pCos), 1, 0;
        fcmpu cr0, fIdx, cZero;
        bge sinCosEnd;
        ps_neg result, result;
sinCosEnd:
        psq_st result, 0(pSin), 1, 0;
    }
}

const math::VEC3& Rotation2VecY(const math::VEC3& rotate,
                                register math::VEC3* pOut) {
    NW4R_POINTER_ASSERT(pOut, 830);

    const register f32* pAngle = reinterpret_cast<const f32*>(&rotate);
    register f32 angleX;
    register f32 angleY;
    register f32 angleZ;

    register __vec2x32float__ sincosr;
    register __vec2x32float__ sincosp;
    register __vec2x32float__ sincosh;

    register f32 work;

    register f32 sssc;
    register f32 res1;
    register f32 add1;

    asm {
        lfs angleX, 0(pAngle);
        lfs angleY, 4(pAngle);
        lfs angleZ, 8(pAngle);
    }

    PS_SIN_COS_RAD_3_REGISTER(angleX, angleY, angleZ, sincosr, sincosp,
                              sincosh);

    asm {
        ps_muls0 sssc, sincosp, sincosr;
        ps_merge10 res1, sincosh, sincosh;

        ps_muls1 add1, sincosh, sincosr;
        ps_neg work, add1;
        ps_merge01 add1, work, add1;

        ps_madds0 res1, res1, sssc, add1;

        psq_st res1, 0(pOut), 0, 0;
        ps_merge10 sssc, sssc, sssc;
        psq_st sssc, 8(pOut), 1, 0;
    }

    return *pOut;
}

math::MTX34* PSMTX34RotXYZRad(register nw4r::math::MTX34* pOut, register f32 x,
                              register f32 y, register f32 z) {
    register f32 sincosx;
    register f32 sincosy;
    register f32 sincosz;

    register f32 sincosxneg;
    register f32 mcosxsinx;
    register f32 scx_sinz;
    register f32 scz_cosy;
    register f32 res;
    register f32 c_zero;
    register f32 mf11f21;
    register f32 f22mf12;

    PS_SIN_COS_RAD_3_REGISTER(x, y, z, sincosx, sincosy, sincosz);
    asm {
        ps_sub c_zero, sincosx, sincosx;

        psq_st c_zero, 44(pOut), 1, 0;

        ps_neg sincosxneg, sincosx;
        ps_merge10 mcosxsinx, sincosxneg, sincosx;

        ps_muls0 scx_sinz, sincosx, sincosz;

        ps_muls1 scz_cosy, sincosz, sincosy;

        ps_merge10 res, scz_cosy, scz_cosy;
        psq_st res, 0(pOut), 1, 0;

        ps_muls1 res, sincosx, sincosz;
        ps_muls0 mf11f21, mcosxsinx, sincosz;
        ps_madds0 res, res, sincosy, mf11f21;
        psq_st res, 4(pOut), 0, 0;

        ps_merge00 res, c_zero, scz_cosy;
        psq_st res, 12(pOut), 0, 0;

        ps_muls1 f22mf12, mcosxsinx, sincosz;
        ps_neg f22mf12, f22mf12
        ps_madds0 res, scx_sinz, sincosy, f22mf12;
        psq_st res, 20(pOut), 0, 0;

        ps_neg res, sincosy;
        ps_merge00 res, c_zero, res;
        psq_st res, 28(pOut), 0, 0;

        ps_muls1 res, sincosx, sincosy;
        psq_st res, 36(pOut), 0, 0;
    }

    return pOut;
}

math::MTX34* MTX34ScaleNonZero(register nw4r::math::MTX34* pOut,
                               const register nw4r::math::MTX34* pM,
                               const register nw4r::math::VEC3* pS) {
    register f32 xy, z1;
    register f32 row0a, row0b;
    register f32 row1a, row1b;
    register f32 row2a, row2b;
    register f32 c_zero;
    register f32 c_eps = nw4r::ef::EMITTER_ZERO_SCALE;
    asm
    {
        psq_l xy, 0(pS), 0, 0;
        ps_sub c_zero, xy, xy;
        psq_l z1, 8(pS), 1, 0
        ps_cmpu0 cr0, xy, c_zero;
        psq_l row0a, 0(pM), 0, 0
        psq_l row0b, 8(pM), 0, 0
        bne xnonzero;
        ps_merge01 xy, c_eps, xy;
xnonzero:
        ps_cmpu1 cr0, xy, c_zero;
        psq_l row1a, 16(pM), 0, 0
        psq_l row1b, 24(pM), 0, 0
        bne ynonzero;
        ps_merge00 xy, xy, c_eps;
ynonzero:
        ps_cmpu0 cr0, z1, c_zero;
          psq_l row2a, 32(pM), 0, 0
          psq_l row2b, 40(pM), 0, 0
        bne znonzero;
        ps_merge01 z1, c_eps, z1;
znonzero:
       
        ps_mul row0a, row0a, xy
        ps_mul row1a, row1a, xy
        ps_mul row2a, row2a, xy
        ps_mul row0b, row0b, z1
        ps_mul row1b, row1b, z1
        ps_mul row2b, row2b, z1

        psq_st row0a, 0(pOut), 0, 0
        psq_st row0b, 8(pOut), 0, 0
        psq_st row1a, 16(pOut), 0, 0
        psq_st row2a, 32(pOut), 0, 0
        psq_st row1b, 24(pOut), 0, 0
        psq_st row2b, 40(pOut), 0, 0
    }
    return pOut;
}

math::MTX34* MTX34ScaleNonZero(register nw4r::math::MTX34* dst,
                               const register nw4r::math::VEC3* pS,
                               const register nw4r::math::MTX34* src) {
    asm {
#define xS fp9
#define yS fp10
#define zS fp11
#define eps fp12
#define c_zero fp13

        lfs         xS, 0(pS);
        addis r6, 0, EMITTER_ZERO_SCALE@ha;
        lfs         yS, 4(pS);
        addi  r6, r6, EMITTER_ZERO_SCALE@l;
        lfs         eps, 0(r6);
        lfs         zS, 8(pS);
        fsubs       c_zero, eps, eps;

        psq_l       fp4, 0(src),        0, 0;
        fcmpu cr0, xS, c_zero;
        psq_l       fp5, 8(src),        0, 0;
        bne xnonzero;
        fmr xS, eps;
xnonzero:
        psq_l       fp6, 16(src),       0, 0;
        fcmpu cr0, yS, c_zero;
        psq_l       fp7, 24(src),       0, 0;
        bne ynonzero;
        fmr yS, eps;
ynonzero:
        psq_l       fp8, 32(src),       0, 0;
        fcmpu cr0, zS, c_zero;
        psq_l       fp2, 40(src),       0, 0;
        bne znonzero;
        fmr zS, eps;
znonzero:
        ps_muls0    fp4, fp4, xS;
        psq_st      fp4, 0(dst),        0, 0;
        ps_muls0    fp5, fp5, xS;
        psq_st      fp5, 8(dst),        0, 0;
        ps_muls0    fp6, fp6, yS;
        psq_st      fp6, 16(dst),       0, 0;
        ps_muls0    fp7, fp7, yS;
        psq_st      fp7, 24(dst),       0, 0;
        ps_muls0    fp8, fp8, zS;
        psq_st      fp8, 32(dst),       0, 0;
        ps_muls0    fp2, fp2, zS;
        psq_st      fp2, 40(dst),       0, 0;

#undef xS
#undef yS
#undef zS
#undef eps
#undef c_zero
    }
    return dst;
}

math::MTX34* MTX34ScaleInv(nw4r::math::MTX34* pOut, const nw4r::math::MTX34* pM,
                           const nw4r::math::VEC3* pS) {
    nw4r::math::VEC3 vec;
    vec.x = (pS->x != 0.0f) ? (nw4r::math::FInv(pS->x))
                            : (1.0f / EMITTER_ZERO_SCALE);
    vec.y = (pS->y != 0.0f) ? (nw4r::math::FInv(pS->y))
                            : (1.0f / EMITTER_ZERO_SCALE);
    vec.z = (pS->z != 0.0f) ? (nw4r::math::FInv(pS->z))
                            : (1.0f / EMITTER_ZERO_SCALE);
    return nw4r::math::MTX34Scale(pOut, pM, &vec);
}

f32 MTXColLen(const register math::MTX34* pSrc, register int col) {
    register f32 vxy, vzz, sqmag;
    register f32 rmag, nwork0, nwork1, c_zero;
    register __vec2x32float__ c_three_half;
    register f32* v;

    asm {
                slwi v, col, 2; // v = col * 4
        add v, pSrc, v; // v = pSrc[col]

        // Square mag calculation
        psq_l       vxy, 0(v), 1, 0;
        psq_l       sqmag, 16(v), 1, 0; // Borrow sqmag
        ps_merge00  vxy, vxy, sqmag;
        ps_mul      vxy, vxy, vxy
        lfs         vzz, 32(v)
        fsubs       c_zero, vxy, vxy;
        ps_madd     sqmag, vzz, vzz, vxy

            // Square mag
        ps_sum0     sqmag, sqmag, vxy, vxy

            // Zero check
        fcmpu       cr0, sqmag, c_zero
        beq-        __exit

                      // 1.0/sqrt : estimation[E]
        frsqrte     rmag, sqmag
          addis r6, 0, sPS_Three_Half@ha;
          addi  r6, r6, sPS_Three_Half@l;
          psq_l c_three_half, 0(r6), 0, 0;

        // Refinement x 1 : E' = (E/2)(3 - X*E*E)
        fmuls       nwork0, rmag, rmag
        ps_muls1    nwork1, rmag, c_three_half
        fnmsubs     nwork0, nwork0, sqmag, c_three_half
        fmuls       rmag, nwork0, nwork1

            // 1/sqrt(X) * X = sqrt(X)
        fmuls       sqmag, sqmag, rmag
    __exit:
    }

    return sqmag;
}

} // namespace ef
} // namespace nw4r
