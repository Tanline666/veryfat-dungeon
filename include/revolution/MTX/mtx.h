#ifndef RVL_SDK_MTX_MTX_H
#define RVL_SDK_MTX_MTX_H
#include <types.h>

#include <revolution/MTX/mtxtypes.h>
#ifdef __cplusplus
extern "C" {
#endif

//! @~english Argument names come from TFP2 assertions.
//! @~japanese 引数名の元はTFP2のアサート。

/**
 * @~english @brief Creates a 3x4 identity matrix.
 * @~japanese @brief 3x4単位行列を作成する。
 *
 * @~english @param[out] m Buffer storing identity matrix
 * @~japanese @param[out] m 単位行列を格納するバッファ
 */
void PSMTXIdentity(Mtx m);

/**
 * @~english @brief Copies a matrix to the specified buffer.
 * @~japanese @brief 指定のバッファへ行列をコピーする。
 *
 * @~english @param[in] src Pointer to source of copied matrix
 * @~japanese @param[in] src コピー元の行列へのポインタ
 * @~english @param[out] dst Buffer to receive the copied matrix
 * @~japanese @param[out] dst コピーした行列を受け取るバッファ
 */
void PSMTXCopy(const Mtx src, Mtx dst);
/**
 * @~english @brief Multiplies (concatenates) two matrices together.
 * @~japanese @brief 2つの行列を乗算（結合）する。
 *
 * @~english @param a Left side matrix
 * @~japanese @param a 左辺の行列
 * @~english @param b Right side matrix
 * @~japanese @param b 右辺の行列
 * @~english @param[out] ab Buffer storing result of multiplication
 * @~japanese @param[out] ab 乗算の結果を格納するバッファ
 */
void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
/**
 * @~english @brief Multiplies (concatenates) a matrix and an array of matrices
 * together.
 * @~japanese @brief 行列に行列の配列を乗算（結合）する。
 *
 * @~english @param[in] a Multiplying matrix
 * @~japanese @param[in] a 乗算する行列
 * @~english @param[in] srcBase Pointer to multiplied array of matrices
 * @~japanese @param[in] srcBase 乗算される行列の配列へのポインタ
 * @~english @param[out] dstBase Pointer to the buffer storing the calculated
 * array
 * @~japanese @param[out] dstBase 計算した配列を格納するバッファへのポインタ
 * @param[in] count
 */
void PSMTXConcatArray(const Mtx a, const Mtx* srcBase, Mtx* dstBase, u32 count);
void PSMTXTranspose(const Mtx, Mtx);
u32 PSMTXInverse(const Mtx, Mtx);
u32 PSMTXInvXpose(const Mtx, Mtx);
void PSMTXRotRad(Mtx, char, f32);
void PSMTXRotTrig(Mtx, f32, f32, char);
void PSMTXRotAxisRad(Mtx, const Vec*, f32);
void PSMTXTrans(Mtx, f32, f32, f32);
void PSMTXTransApply(const Mtx, Mtx, f32, f32, f32);
/**
 * @~english @brief Creates a scale matrix from three scale parameters.
 * @~japanese @brief 3つのスケールパラメータからスケール行列を作成する。
 *
 * @~english @param[out] scaleMtx Buffer storing new scale matrix
 * @~japanese @param[out] scaleMtx 新しいスケール行列を格納するバッファ
 * @~english @param xScale Scale x-value
 * @~japanese @param xScale スケールのX値
 * @~english @param yScale Scale y-value
 * @~japanese @param yScale スケールのY値
 * @~english @param zScale Scale z-value
 * @~japanese @param zScale スケールのZ値
 */
void PSMTXScale(Mtx scaleMtx, f32 xScale, f32 yScale, f32 zScale);
void PSMTXScaleApply(const Mtx, Mtx, f32, f32, f32);
void PSMTXQuat(Mtx, const Quaternion*);

//! C functions
/**
 * @~english @brief Creates a 3x4 identity matrix. DEBUG-ONLY
 * @~japanese @brief 3x4単位行列を作成する。デバッグ版だけです。
 *
 * @~english @param[out] m Buffer storing identity matrix
 * @~japanese @param[out] m 単位行列を格納するバッファ
 */
void C_MTXIdentity(Mtx m);
/**
 * @~english @brief Copies a matrix to the specified buffer. DEBUG-ONLY
 * @~japanese @brief 指定のバッファへ行列をコピーする。デバッグ版だけです。
 *
 * @~english @param[in] src Pointer to source of copied matrix
 * @~japanese @param[in] src コピー元の行列へのポインタ
 * @~english @param[out] dst Buffer to receive the copied matrix
 * @~japanese @param[out] dst コピーした行列を受け取るバッファ
 */
void C_MTXCopy(const Mtx src, Mtx dst);
/**
 * @~english @brief Multiplies (concatenates) two matrices together. DEBUG-ONLY
 * @~japanese @brief 2つの行列を乗算（結合）する。デバッグ版だけです。
 *
 * @~english @param a Left side matrix
 * @~japanese @param a 左辺の行列
 * @~english @param b Right side matrix
 * @~japanese @param b 右辺の行列
 * @~english @param[out] ab Buffer storing result of multiplication
 * @~japanese @param[out] ab 乗算の結果を格納するバッファ
 */
void C_MTXConcat(const Mtx a, const Mtx b, Mtx ab);

void C_MTXConcatArray(const Mtx a, const Mtx* srcBase, Mtx* dstBase, u32 count);

#if !defined(DEBUG)
#define MTXCopy PSMTXCopy
#define MTXConcat PSMTXConcat
#define MTXIdentity PSMTXIdentity
#else
#define MTXCopy C_MTXCopy
#define MTXConcat C_MTXConcat
#define MTXIdentity C_MTXIdentity
#endif

/**
 * @~english @brief Sets the matrix to be used when converting
 * from world space to camera space.
 * @~japanese @brief ワールド空間をカメラ空間へ変換する行列を設定する。
 *
 * @~english @param[out] mtx Buffer storing new viewing matrix
 * @~japanese @param[out] mtx 新しいビューイング行列を格納するバッファ
 * @~english @param pCamPos Pointer to vector representing camera position
 * within world space
 * @~japanese @param pCamPos
 * ワールド空間でのカメラの位置を表現するベクトルへのポインタ
 * @~english @param pCamUp Pointer to vector containing direction of
 * camera's upper surface
 * @~japanese @param pCamUp カメラ上部の向きを格納するベクトルへのポインタ
 * @param pTarget
 */
void C_MTXLookAt(Mtx mtx, const Vec* pCamPos, const Vec* pCamUp,
                 const Vec* pTarget);
void C_MTXLightFrustum(Mtx mtx, f32 top, f32 bottom, f32 left, f32 right,
                       f32 near, f32 scaleS, f32 scaleT, f32 transS,
                       f32 transT);
void C_MTXLightPerspective(Mtx, f32, f32, f32, f32, f32, f32);
void C_MTXLightOrtho(Mtx, f32, f32, f32, f32, f32, f32, f32, f32);

#ifdef __cplusplus
}
#endif
#endif
