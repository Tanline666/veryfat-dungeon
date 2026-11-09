#ifndef NW4R_EF_TYPES_H
#define NW4R_EF_TYPES_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math.h>

#define NW4R_EF_MAX_EMITTER 1024
#define NW4R_EF_MAX_EFFECT 1024
#define NW4R_EF_MAX_PARTICLEMANAGER 1024
#define NW4R_EF_MAX_PARTICLE 1024

// Epsilon... but not actually epsilon???
#define NW4R_EF_EMIT_ANGLE_MIN (1.917476e-4f)
// 2 * PI... but not actually PI???
#define NW4R_EF_EMIT_ANGLE_MAX (2 * 3.1414969f)

namespace nw4r {
namespace ef {

static const u32 NUM_PARAMS = 6;

enum Axis {
    AXIS_X,
    AXIS_Y,
    AXIS_Z,

    AXIS_MAX
};

enum ColorLayer {
    COLOR_LAYER_0,
    COLOR_LAYER_1,

    COLOR_LAYER_MAX
};

enum ColorIndex {
    COLOR_IDX_PRI,
    COLOR_IDX_SEC,

    COLOR_IDX_MAX
};

enum TextureLayer {
    TEX_LAYER_1,
    TEX_LAYER_2,
    TEX_LAYER_IND,

    TEX_LAYER_MAX
};

/**
 * \~english @brief Enum that evaluates effect status.
 * \~japanese @brief エフェクトの状態を評価するenum。
 */
enum EvaluateStatus {
    NW4R_EF_ES_WAIT,
    NW4R_EF_ES_DONE,
    NW4R_EF_ES_STOP,
    NW4R_EF_ES_SKIP,
};

/**
 * \~english @brief Effect calculation options.
 * \~japanese @brief エフェクトの計算オプション。
 */
enum CalcOption {
    CALC_TYPE_NORMAL,
    CALC_TYPE_BILLBOARD,
    CALC_TYPE_EXTENDED,
};

typedef u32 ForEachParam;
typedef void (*ForEachFunc)(void* pObject, ForEachParam param);

#define NW4R_EF_LIST_GET_NEXT(list, object)                                    \
    ((ut::Link*)(((u32)(object)) + (list)->offset))->nextObject
#define NW4R_EF_LIST_GET_PREV(list, object)                                    \
    ((ut::Link*)(((u32)(object)) + (list)->offset))->prevObject

inline u8 ConvertF32RadToU8(f32 rad) {
    rad = math::FCeil(rad / NW4R_MATH_PI * 128.0f - 0.5f);
    return static_cast<int>(rad) % 256;
}

#define ALPHA_MODULATE(result, alpha1, alpha2)                                 \
    (result) = static_cast<u8>(((alpha1) * (alpha2) + 128) >> 8);

#define COLOR_MODULATE(result, color1, color2)                                 \
    ALPHA_MODULATE((result).r, (color1).r, (color2).r);                        \
    ALPHA_MODULATE((result).g, (color1).g, (color2).g);                        \
    ALPHA_MODULATE((result).b, (color1).b, (color2).b);                        \
    ALPHA_MODULATE((result).a, (color1).a, (color2).a);

} // namespace ef
} // namespace nw4r

#endif
