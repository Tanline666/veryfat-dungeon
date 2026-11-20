#ifndef RVL_SDK_GX_TRANSFORM_H
#define RVL_SDK_GX_TRANSFORM_H
#include <types.h>

#include <revolution/GX/GXTypes.h>
#include <revolution/MTX.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GX_PROJECTION_SZ 7

// Projection field indices
#define GX_PROJECTION_TP 0
#define GX_PROJECTION_A 1
#define GX_PROJECTION_B 2
#define GX_PROJECTION_C 3
#define GX_PROJECTION_D 4
#define GX_PROJECTION_E 5
#define GX_PROJECTION_F 6

#define GX_VIEWPORT_SZ 6

// Viewport field indices
#define GX_VIEWPORT_L 0
#define GX_VIEWPORT_T 1
#define GX_VIEWPORT_W 2
#define GX_VIEWPORT_H 3
#define GX_VIEWPORT_N 4
#define GX_VIEWPORT_F 5

typedef enum {
    GX_FIELD_EVEN,
    GX_FIELD_ODD,
} GXInterlaceField;

void GXSetProjection(const Mtx44 proj, GXProjectionType type);
void GXSetProjectionv(const f32 proj[GX_PROJECTION_SZ]);
void GXGetProjectionv(f32 proj[GX_PROJECTION_SZ]);
void GXLoadPosMtxImm(const Mtx mtx, u32 id);
void GXLoadPosMtxIndx(u16 index, u32 id);
void GXLoadNrmMtxImm(const Mtx mtx, u32 id);
void GXLoadNrmMtxIndx3x3(u16 index, u32 id);
void GXSetCurrentMtx(u32 id);
void GXLoadTexMtxImm(const Mtx mtx, u32 id, GXMtxType type);
void GXSetViewportJitter(f32 ox, f32 oy, f32 sx, f32 sy, f32 near, f32 far,
                         u32 nextField);
void GXSetViewport(f32 ox, f32 oy, f32 sx, f32 sy, f32 near, f32 far);
void GXGetViewportv(f32 view[GX_VIEWPORT_SZ]);
void GXSetZScaleOffset(f32 scale, f32 offset);
/**
 * @~english @brief Sets the scissor box region.
 * @~japanese @brief シザリング矩形の領域を設定する。
 *
 * @~english @param x Upper left position of the scissor box region (X-axis)
 * @~japanese @param x シザリング矩形の領域の左上の位置（X座軸）
 * @~english @param y Upper left position of the scissor box region (Y-axis)
 * @~japanese @param y シザリング矩形の領域の左上の位置（Y座軸）
 * @param w
 * @param h
 */
void GXSetScissor(u32 x, u32 y, u32 w, u32 h);
/**
 * @~english @brief Gets the current scissor box region.
 * @~japanese @brief 現在のシザリング矩形の領域を取得する。
 *
 * @param[out] x
 * @param[out] y
 * @param[out] w
 * @param[out] h
 */
void GXGetScissor(u32* x, u32* y, u32* w, u32* h);
void GXSetScissorBoxOffset(u32 ox, u32 oy);
void GXSetClipMode(GXClipMode mode);

void __GXSetProjection(void);
void __GXSetViewport(void);
void __GXSetMatrixIndex(GXAttr index);

static void GXSetViewportv(const f32 view[GX_VIEWPORT_SZ]) {
    GXSetViewport(view[0], view[1], view[2], view[3], view[4], view[5]);
}

#ifdef __cplusplus
}
#endif
#endif
