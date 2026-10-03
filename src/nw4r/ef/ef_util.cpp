#include <nw4r/ef.h>
#include <nw4r/math.h>
#include <nw4r/ut.h>

#include <cmath>

namespace nw4r {
namespace ef {

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
    //! Is there a better way to write this... like, without gotos?
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

} // namespace ef
} // namespace nw4r
