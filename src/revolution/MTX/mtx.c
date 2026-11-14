#include <revolution/OS/OSError.h>
#include <revolution/mtx.h>

static f32 Unit01[] = {0.0f, 1.0f};

void C_MTXIdentity(Mtx m) {
    OS_ASSERT(m != NULL, "MtxIdentity():  NULL Mtx 'm' ");

    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
}

void PSMTXIdentity(register Mtx m) {
    register f32 c_zero = 0.0f;
    register f32 c_one = 1.0f;
    register f32 c_01;
    register f32 c_10;

    ASM (
        psq_st c_zero, 8(m), 0, 0
        ps_merge01 c_01, c_zero, c_one
        psq_st c_zero, 24(m), 0, 0
        ps_merge10 c_10, c_one, c_zero
        psq_st c_zero, 32(m), 0, 0
        psq_st c_01, 16(m), 0, 0
        psq_st c_10, 0(m), 0, 0
        psq_st c_10, 40(m), 0, 0
    )
}

void C_MTXCopy(const Mtx src, Mtx dst) {
    //! @bug Wrong type names
    OS_ASSERT((src != 0), "MTXCopy():  NULL MtxPtr 'src' ");
    OS_ASSERT((dst != 0), "MTXCopy():  NULL MtxPtr 'dst'" );

    if (src == dst) {
        return;
    }

    dst[0][0] = src[0][0];
    dst[0][1] = src[0][1];
    dst[0][2] = src[0][2];
    dst[0][3] = src[0][3];

    dst[1][0] = src[1][0];
    dst[1][1] = src[1][1];
    dst[1][2] = src[1][2];
    dst[1][3] = src[1][3];

    dst[2][0] = src[2][0];
    dst[2][1] = src[2][1];
    dst[2][2] = src[2][2];
    dst[2][3] = src[2][3];
}

asm void PSMTXCopy(const register Mtx src, register Mtx dst) {
    // clang-format off
    nofralloc

    psq_l fp0, 0(src), 0, 0
    psq_st fp0, 0(dst), 0, 0
    psq_l fp1, 8(src), 0, 0
    psq_st fp1, 8(dst), 0, 0
    psq_l fp2, 16(src), 0, 0
    psq_st fp2, 16(dst), 0, 0
    psq_l fp3, 24(src), 0, 0
    psq_st fp3, 24(dst), 0, 0
    psq_l fp4, 32(src), 0, 0
    psq_st fp4, 32(dst), 0, 0
    psq_l fp5, 40(src), 0, 0
    psq_st fp5, 40(dst), 0, 0

    blr
    // clang-format on
}

void C_MTXConcat(const Mtx a, const Mtx b, Mtx ab) {
    Mtx tmp;
    MtxPtr m;

    //! @bug Wrong type names
    OS_ASSERT((a != 0), "MTXConcat():  NULL MtxPtr 'a'  ");
    OS_ASSERT((b != 0), "MTXConcat():  NULL MtxPtr 'b'  ");
    OS_ASSERT((ab != 0), "MTXConcat():  NULL MtxPtr 'ab'  ");

    if ((ab == a) || (ab == b)) {
        m = tmp;
    } else {
        m = ab;
    }

    m[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
    m[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
    m[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];
    m[0][3] =
        a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3];

    m[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
    m[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
    m[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];
    m[1][3] =
        a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3];

    m[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];
    m[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];
    m[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];
    m[2][3] =
        a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3];

    if (m == tmp) {
        C_MTXCopy(tmp, ab);
    }
}

asm void PSMTXConcat(const register Mtx a, const register Mtx b,
                     register Mtx ab) {
    // clang-format off
    nofralloc

    //! @~english Reserve stack size, prepare arguments
    //! @~japanese スタックサイズを予約すると、引数を用意する
    stwu r1, -64(r1)
    psq_l fp0, 0(a), 0, 0
    stfd fp14, 8(r1)
    psq_l fp6, 0(b), 0, 0
    addis r6, 0, Unit01@ha
    psq_l fp7, 8(b), 0, 0
    stfd fp15, 16(r1)
    addi r6, r6, Unit01@l
    stfd fp31, 40(r1)
    psq_l fp8, 16(b), 0, 0

    //! (m[0][0] * m[0][1]) = (b[0][0] * a[0][0]) * (b[0][1] * a[0][0]) 
    ps_muls0 fp12, fp6, fp0
    psq_l fp2, 16(a), 0, 0

    //! (m[0][2] * m[0][3]) = (b[0][2] * a[0][0]) * (b[0][3] * a[0][0]) 
    ps_muls0 fp13, fp7, fp0
    psq_l fp31, 0(r6), 0, 0 

    //! (m[1][0] * m[1][1]) = (a[1][0] * b[0][0]) * (a[1][0] * b[0][1]) 
    ps_muls0 fp14, fp6, fp2
    psq_l fp9, 24(r4), 0, 0

    ps_muls0 fp15, fp7, fp2
    psq_l fp1, 8(a), 0, 0

    //! (m[0][0] * m[0][1]) = (((b[1][0] * a[0][1]) + (b[0][0] * a[0][0])) * ((b[1][1] * a[0][1]) + (b[0][1] * a[0][0])))
    ps_madds1 fp12, fp8, fp0, fp12

    psq_l fp3, 24(a), 0, 0
    ps_madds1 fp14, fp8, fp2, fp14
    
    psq_l fp10, 32(b), 0, 0
    ps_madds1 fp13, fp9, fp0, fp13

    psq_l fp11, 40(b), 0, 0
    ps_madds1 fp15, fp9, fp2, fp15

    psq_l fp4, 32(a), 0, 0
    psq_l fp5, 40(a), 0, 0

    ps_madds0 fp12, fp10, fp1, fp12
    ps_madds0 fp13, fp11, fp1, fp13
    ps_madds0 fp14, fp10, fp3, fp14
    ps_madds0 fp15, fp11, fp3, fp15

    psq_st fp12, 0(ab), 0, 0

    ps_muls0 fp2, fp6, fp4
    ps_madds1 fp13, fp31, fp1, fp13

    ps_muls0 fp0, fp7, fp4
    psq_st fp14, 16(ab), 0, 0

    ps_madds1 fp15, fp31, fp3, fp15
    psq_st fp13, 8(ab), 0, 0
    ps_madds1 fp2, fp8, fp4, fp2
    ps_madds1 fp0, fp9, fp4, fp0
    ps_madds0 fp2, fp10, fp5, fp2

    lfd fp14, 8(r1)

    psq_st fp15, 24(ab), 0, 0
    ps_madds0 fp0, fp11, fp5, fp0

    psq_st fp2, 32(ab), 0, 0 
    ps_madds1 fp0, fp31, fp5, fp0

    //! @~english Start clearing stack
    //! @~japanese スタックをクリアして初める
    lfd fp15, 16(r1)
    //! @~english Save m[2][2] * m[2][3] in ab. Calculation complete!
    //! @~japanese m[2][2] * m[2][3]をabへセーブする。計算完了！
    psq_st fp0, 40(r5), 0, 0

    lfd fp31, 40(r1)
    addi r1, r1, 64
    blr
    // clang-format on
}
