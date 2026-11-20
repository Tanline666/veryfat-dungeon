#ifndef NW4R_MATH_ARITHMETIC_H
#define NW4R_MATH_ARITHMETIC_H
#include <nw4r/types_nw4r.h>

#include <MSL/cmath>

#include <nw4r/db/assert.h>
#include <nw4r/math/constant.h>

#include <revolution/OS.h>

namespace nw4r {
namespace math {

/******************************************************************************
 *
 * Implementation details
 *
 ******************************************************************************/
namespace detail {

/**
 * @~english @brief Finds e^x.
 * @~japanese @brief e^xを求める。
 *
 * @~english @param x Exponent value
 * @~japanese @param x 指数の値
 * @return e^x
 */
f32 FExp(f32 x);
/**
 * @~english @brief Finds the natural logarithm of x.
 * @~japanese @brief xの自然対数を求める。
 *
 * @param x
 * @~english @return Natural logarithm of x
 * @~japanese @return xの自然対数
 */
f32 FLog(f32 x);

} // namespace detail

/**
 * @~english @brief Calculates the inverse square root of x.
 * @~japanese @brief xの平方根の逆数を計算する。
 *
 * @param x
 * @~english @return Inverse square root of x
 * @~japanese @return xの平方根の逆数
 */
f32 FrSqrt(f32 x);

inline f32 FAbs(register f32 x) {
    register f32 ax;

    ASM (
        fabs ax, x
    )

    return ax;
}

inline f32 FCeil(f32 x) {
    return std::ceilf(x);
}

/**
 * @~english @brief Finds e^x.
 * @~japanese @brief e^xを求める。
 *
 * @~english @param x Exponent value
 * @~japanese @param x 指数の値
 * @return e^x
 *
 * @~english @note This automatically redirects to the detail namespace
 * FExp function.
 * @~japanese @note
 * ここは自動的にdetailネームスペースのFExp関数へリダイレクトする。
 */
inline f32 FExp(f32 x) {
    return detail::FExp(x);
}

inline f32 FFloor(f32 x) {
    return std::floorf(x);
}

/**
 * @~english @brief Calculates the inverse of x.
 * @~japanese @brief xの逆数を計算する。
 *
 * @param x
 * @~english @return Inverse of x
 * @~japanese @return xの逆数
 */
inline f32 FInv(register f32 x) {
    register f32 work0, work1, work2, work3;

    ASM (
        fmr  work1, x     // x
        fres work0, work1 // 1/x


        // Refine estimate
        ps_add   work2, work0, work0        // 2/x
        ps_mul   work3, work0, work0        // 1/x^2
        ps_nmsub work0, work1, work3, work2 // -(x * 1/x^2 - 2/x)
    )

    return work0;
}

/**
 * @~english @brief Calculates the remainder of the division of x and y.
 * @~japanese @brief xとyの除算の剰余を計算する。
 *
 * @~english @param x Left-side value
 * @~japanese @param x 左辺の値
 * @~english @param y Right-side value
 * @~japanese @param y 右辺の値
 * @~english @return Remainder
 * @~japanese @return 剰余
 */
inline f32 FMod(f32 x, f32 y) {
    return std::fmodf(x, y);
}

/**
 * @~english @brief Splits a float into an integer component and a decimal
 * component.
 * @~japanese @brief 浮動小数点を整数部と小数部に分ける。
 *
 * @~english @param x Original float
 * @~japanese @param x 元の浮動小数点
 * @~english @param[out] y Pointer to buffer storing integer component
 * @~japanese @param[out] y 整数部を格納するバッファへのポインタ
 * @~english @return Decimal component
 * @~japanese @return 小数部
 */
inline f32 FModf(f32 x, f32* y) {
    return std::modff(x, y);
}

/**
 * @~english @brief Calculates the square root of x.
 * @~japanese @brief xの平方根を計算する。
 *
 * @param x
 * @~english @return Square root of x
 * @~japanese @return xの平方根
 */
inline f32 FSqrt(f32 x) {
    NW4R_WARNING(x >= 0.0f, 627, "FSqrt: Input is out of the domain.");
    return x <= 0.0f ? 0.0f : x * FrSqrt(x);
}

/**
 * @~english @brief Finds the natural logarithm of x.
 * @~japanese @brief xの自然対数を求める。
 *
 * @param x
 * @~english @return Natural logarithm of x
 * @~japanese @return xの自然対数
 *
 * @~english @note This automatically redirects to the detail namespace
 * FLog function.
 * @~japanese @note
 * ここは自動的にdetailネームスペースのFLog関数へリダイレクトする。
 */
inline f32 FLog(f32 x) {
    NW4R_WARNING(x > 0.0f, 295, "FLog: Input is out of the domain.");
    if (x > 0.0f) {
        return detail::FLog(x);
    }

    return NW4R_MATH_QNAN;
}

/**
 * @~english @brief Returns ifPos or ifNeg depending on if cond is positive or
 * negative.
 * @~japanese @brief condが正数か負数の場合はifPosかifNegを返す。
 * @~english @note If cond is 0, ifPos is returned.
 * @~japanese @note condが0であれば、ifPosが返る。
 *
 * @~english @param cond Value determining which argument is selected
 * @~japanese @param cond どっちの引数が選択されるかを決まってる値
 * @~english @param ifPos Value to be selected when cond is positive
 * @~japanese @param ifPos condが正数の場合の選択される値
 * @~english @param ifNeg Value to be selected when cond is negative
 * @~japanese @param ifNeg condが負数の場合の選択される値
 * @~english @return ifPos or ifNeg
 * @~japanese @return ifPosかifNeg
 */
inline f32 FSelect(register f32 cond, register f32 ifPos, register f32 ifNeg) {
    register f32 ret;

    ASM (
        fsel ret, cond, ifPos, ifNeg
    )

    return ret;
}

/******************************************************************************
 *
 * Fastcast functions
 *
 ******************************************************************************/
inline f32 U16ToF32(u16 arg) {
    f32 ret;
    OSu16tof32(&arg, &ret);
    return ret;
}
inline u16 F32ToU16(f32 arg) {
    u16 ret;
    OSf32tou16(&arg, &ret);
    return ret;
}

inline f32 S16ToF32(s16 arg) {
    f32 ret;
    OSs16tof32(&arg, &ret);
    return ret;
}
inline s16 F32ToS16(f32 arg) {
    s16 ret;
    OSf32tos16(&arg, &ret);
    return ret;
}

inline u32 F32AsU32(f32 arg) {
    return *reinterpret_cast<u32*>(&arg);
}
inline f32 U32AsF32(u32 arg) {
    return *reinterpret_cast<f32*>(&arg);
}

inline s32 FGetExpPart(f32 x) {
    s32 s = F32AsU32(x);
    return ((s >> 23) & 0xFF) - 127;
}
inline f32 FGetMantPart(f32 x) {
    u32 u = F32AsU32(x);
    return U32AsF32((u & 0x807FFFFF) | 0x3F800000);
}

/**
 * @~english @brief Performs Hermite interpolation.
 * @~japanese @brief エルミート補間を行う。
 *
 * @~english @param p1 Value of point 1
 * @~japanese @param p1 点1の値
 * @param t1
 * @~english @param p2 Value of point 2
 * @~japanese @param p2 点2の値
 * @param t2
 * @param s
 * @return f32
 */
f32 Hermite(f32 p1, f32 t1, f32 p2, f32 t2, f32 s);

} // namespace math
} // namespace nw4r

#endif
