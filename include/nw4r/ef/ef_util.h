#ifndef NW4R_EF_UTIL_H
#define NW4R_EF_UTIL_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut.h>

namespace nw4r {
namespace ef {

u16 UtlistToArray(const ut::List* pList, void** ppArray, int maxlen);

/**
 * \~english @brief Finds where the Y-axis ends up in an Euler angle's rotation.
 * \~japanese @brief オイラー角による回転でY座軸がどこに行くかを求める。
 *
 * \~english @param rRot Reference to a vector storing the amount of rotation
 * \~japanese @param rRot 回転量を格納するベクトルへの参照
 * \~english @param pVec Pointer to a vector representing the Y-axis
 * \~japanese @param pVec Y座軸を表現するベクトルへのポインタ
 * \~english @return Normalized pVec
 * \~japanese @return 正規化されたpVec
 */
const math::VEC3& Rotation2VecY(const math::VEC3& rRot,
                                register math::VEC3* pVec);
void GetDirMtxY(math::MTX34* pMtx, const math::VEC3& rVec);

void MtxGetRotationMtx(const math::MTX34& rMtx, math::MTX34* pDst);

void MtxSetColVec(math::MTX34* pMtx, int col, const math::VEC3* pVec);
math::VEC3* MtxColVec(const math::MTX34* pMtx, int col, math::VEC3* pVec);

void MtxGetRotation(const math::MTX34& rMtx, math::VEC3* pRot);
void MtxGetTranslate(const math::MTX34& rMtx, math::VEC3* pTrans);
void MtxGetScale(const math::MTX34& rMtx, math::VEC3* pScale);
math::MTX34* PSMTX34RotXYZRad(register math::MTX34* pOut, register f32 x,
                              register f32 y, register f32 z);

math::MTX34* MTX34ScaleNonZero(register math::MTX34* result,
                               const register math::MTX34* pM,
                               const register math::VEC3* vec);
math::MTX34* MTX34ScaleNonZero(register math::MTX34* result,
                               const register math::VEC3* vec,
                               const register math::MTX34* pM);

math::MTX34* MTX34ScaleInv(math::MTX34* result, const math::MTX34* pM,
                           const math::VEC3* vec);

/**
 * \~english @brief Normalizes the given vectors for rendering effects.
 * \~japanese @brief エフェクトを描画するための指定されたベクトルを正規化する。
 *
 * \~english @param dst Pointer to buffer for storing normalized vector
 * \~japanese @param dst 正規化したベクトルを格納するバッファへのポインタ
 * \~english @param vec Pointer to vector to normalize
 * \~japanese @param vec 正規化するベクトルへのポインタ
 * \~english @return FALSE if vec is NULL
 * \~japanese @return vecがNULLならFALSE
 */
bool Normalize(register math::VEC3* dst, const register math::VEC3* vec);

void _PSSinCosRad(register f32* pRes, register f32 val);

/**
 * \~english @brief Calculates a given angle's sine and cosine at the same time.
 * \~japanese @brief 指定の角の正弦と余弦を同時に求める。
 *
 * \~english @param pSin Pointer to buffer for storing sine
 * \~japanese @param pSin 正弦を格納するバッファへのポインタ
 * \~english @param pCos Pointer to buffer for storing cosine
 * \~japanese @param pCos 余弦を格納するバッファへのポインタ
 * \~english @param val Angle in radians
 * \~japanese @param val ラジアン単位での角度
 */
void PSSinCosRad(register f32* pSin, register f32* pCos, register f32 val);

f32 MTXColLen(const register math::MTX34* pSrc, register int col);

} // namespace ef
} // namespace nw4r

#endif
