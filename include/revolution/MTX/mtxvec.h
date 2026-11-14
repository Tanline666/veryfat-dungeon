#ifndef RVL_SDK_MTX_MTXVEC_H
#define RVL_SDK_MTX_MTXVEC_H
#include <types.h>

#include <revolution/MTX/mtxtypes.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @~english @brief Multiplies a 3D vector with a 3x4 matrix.
 * @~japanese @brief 3次元ベクトルに3x4行列を乗算する。
 *
 * @~english @param[in] mtx Pointer to multiplying matrix
 * @~japanese @param[in] mtx 乗算する行列へのポインタ
 * @~english @param[in] vec Pointer to multiplied vector
 * @~japanese @param[in] vec 乗算されるベクトルへのポインタ
 * @~english @param[out] out Pointer to buffer storing calculation result
 * @~japanese @param[out] out 計算結果を格納するバッファへのポインタ
 */
void PSMTXMultVec(const Mtx mtx, const Vec* vec, Vec* out);

#ifdef __cplusplus
}
#endif
#endif
