#ifndef RVL_SDK_GX_GEOMETRY_H
#define RVL_SDK_GX_GEOMETRY_H
#include <types.h>

#include <revolution/GX/GXTypes.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @~english @brief Function called prior to drawing GX primitives.
 * @~japanese @brief GXプリミティブを描画する前に呼ぶ関数。
 *
 * @~english @param prim Primitive type
 * @~japanese @param prim プリミティブタイプ
 * @~english @param fmt Vertex attribute format
 * @~japanese @param fmt 頂点属性フォーマット
 * @~english @param verts Vertices to be drawn until GXEnd() is called
 * @~japanese @param verts GXEnd()を呼ぶまでの描画する頂点
 */
void GXBegin(GXPrimitive prim, GXVtxFmt fmt, u16 verts);
static void GXEnd(void) {
#if defined(DEBUG)
    extern GXBool __GXinBegin;
    if (!__GXinBegin) {
        GX_ASSERT(FALSE, "GXEnd: called without a GXBegin");
    }
    __GXinBegin = GX_FALSE;
#endif
}

void GXSetLineWidth(u8 width, u32 offset);
void GXSetPointSize(u8 size, u32 offset);
/**
 * @~english @brief Sets whether the offset parameter should be read when using
 * GXSetLineWidth() or GXSetPointSize() for the given texture coordinate.
 * @~japanese @brief
 * 指定したテクスチャ座標にGXSetLineWidth()かGXSetPointSize()を呼ぶ時に、offsetの引数を読むかどうかを設定する。
 *
 * @~english @param coordId Specified texture coordinate's ID
 * @~japanese @param coordId 指定したテクスチャ座標のID
 * @~english @param lineOfs Specifies whether to add offsets to line widths
 * @~japanese @param lineOfs オフセットとライン幅を足すかどうかを指定する
 * @~english @param pointOfs Specifies whether to add offsets to point sizes
 * @~japanese @param pointOfs オフセットとポイントサイズを足すかどうかを指定する
 */
void GXEnableTexOffsets(GXTexCoordID coordId, GXBool lineOfs, GXBool pointOfs);
void GXSetCullMode(GXCullMode mode);
void GXGetCullMode(GXCullMode* out);
void GXSetCoPlanar(GXBool coplanar);

void __GXSetDirtyState(void);
void __GXSendFlushPrim(void);
void __GXSetGenMode(void);

#ifdef __cplusplus
}
#endif
#endif
