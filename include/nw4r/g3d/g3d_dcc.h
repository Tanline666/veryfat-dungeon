#ifndef NW4R_G3D_DCC_H
#define NW4R_G3D_DCC_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanmtexsrt.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <nw4r/math.h>

namespace nw4r {
namespace g3d {

/**
 * \~english @brief Calculates a texture matrix from the given texture SRT.
 * \~japanese @brief 指定のテクスチャSRTからテクスチャ行列を計算する。
 *
 * \~english @param pMtx Storage pointer for the calculated matrix
 * \~japanese @param pMtx 計算した行列への格納ポインタ
 * \~english @param set Controls whether to overwrite or multiply the matrix
 * \~japanese @param set 上書きするか乗算するかの管理するbool
 * \~english @param rSrt Reference to texture SRT data
 * \~japanese @param rSrt テクスチャSRTデータへの参照
 * \~english @param flag Texture SRT data flag
 * \~japanese @param flag テクスチャSRTデータのフラグ
 * \~english @param mode Matrix calculation method
 * \~japanese @param mode 行列計算方法
 */
void CalcTexMtx(math::MTX34* pMtx, bool set, const TexSrt& rSrt,
                TexSrt::Flag flag, TexSrtTypedef::TexMatrixMode mode);

/**
 * \~english @brief Calculates a texture matrix from the given texture SRT.
 * \~japanese @brief 指定のテクスチャSRTからテクスチャ行列を割り出す。
 *
 * \~english @param pMtx Storage pointer for the calculated matrix
 * \~japanese @param pMtx 割り出した行列への格納ポインタ
 * \~english @param set Controls whether to overwrite or multiply the matrix
 * \~japanese @param set 上書きするか乗算するかの管理するbool
 * \~english @param rSrt Reference to texture SRT data
 * \~japanese @param rSrt テクスチャSRTデータへの参照
 * \~english @param flag Texture SRT data flag
 * \~japanese @param flag テクスチャSRTデータのフラグ
 */
void CalcTexMtx(math::MTX34* pMtx, bool set, const TexSrt& rSrt,
                TexSrt::Flag flag);

namespace detail {

class WorldMtxAttr {
public:
    enum Attr {
        ATTR_BILLBOARD_MASK = (1 << 8) - 1,

        ATTR_T_IGNORE = (1 << 27),
        ATTR_S_UNIFORM = (1 << 28),
        ATTR_ALL_S_UNIFORM = (1 << 29),
        ATTR_S_ONE = (1 << 30),
        ATTR_ALL_S_ONE = (1 << 31),
    };

public:
    static ResNodeData::Billboard GetBillboard(u32 attr) {
        return static_cast<ResNodeData::Billboard>(attr & ATTR_BILLBOARD_MASK);
    }
    static u32 SetBillboard(u32 attr, ResNodeData::Billboard billboard) {
        return (attr & ~ATTR_BILLBOARD_MASK) | billboard;
    }

    static bool IsIgnoreTrans(u32 attr) {
        return (attr & ATTR_T_IGNORE) ? true : false;
    }
    static u32 AnmIgnoreTrans(u32 attr) {
        return attr | ATTR_T_IGNORE;
    }
    static u32 AnmNotIgnoreTrans(u32 attr) {
        return attr & ~ATTR_T_IGNORE;
    }

    static bool IsScaleUniform(u32 attr) {
        return (attr & ATTR_S_UNIFORM) ? true : false;
    }
    static u32 AnmScaleUniform(u32 attr) {
        return attr | ATTR_S_UNIFORM;
    }
    static u32 AnmNotScaleUniform(u32 attr) {
        return attr & ~(ATTR_S_UNIFORM | ATTR_ALL_S_UNIFORM | ATTR_S_ONE |
                        ATTR_ALL_S_ONE);
    }

    static bool IsAllScaleUniform(u32 attr) {
        return (attr & ATTR_ALL_S_UNIFORM) ? true : false;
    }
    static u32 AnmAllScaleUniform(u32 attr) {
        return attr | ATTR_ALL_S_UNIFORM;
    }
    static u32 AnmNotAllScaleUniform(u32 attr) {
        return attr & ~(ATTR_ALL_S_UNIFORM | ATTR_ALL_S_ONE);
    }

    static bool IsScaleOne(u32 attr) {
        return (attr & ATTR_S_ONE) ? true : false;
    }
    static u32 AnmScaleOne(u32 attr) {
        return attr | ATTR_S_ONE;
    }
    static u32 AnmNotScaleOne(u32 attr) {
        return attr & ~(ATTR_S_ONE | ATTR_ALL_S_ONE);
    }

    static bool IsAllScaleOne(u32 attr) {
        return (attr & ATTR_ALL_S_ONE) ? true : false;
    }
    static u32 AnmAllScaleOne(u32 attr) {
        return attr | ATTR_ALL_S_ONE;
    }
    static u32 AnmNotAllScaleOne(u32 attr) {
        return attr & ~ATTR_ALL_S_ONE;
    }

    static u32 GetRootMtxAttr() {
        return ATTR_S_UNIFORM | ATTR_ALL_S_UNIFORM | ATTR_S_ONE |
               ATTR_ALL_S_ONE;
    }
};

} // namespace detail
} // namespace g3d
} // namespace nw4r

#endif
