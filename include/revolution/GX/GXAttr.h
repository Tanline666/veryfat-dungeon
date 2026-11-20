#ifndef RVL_SDK_GX_ATTR_H
#define RVL_SDK_GX_ATTR_H
#include <types.h>

#include <revolution/GX/GXTypes.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GXVtxDescList {
    GXAttr attr;     // at 0x0
    GXAttrType type; // at  0x4
} GXVtxDescList;

typedef struct _GXVtxAttrFmtList {
    GXAttr attr;         // at 0x0
    GXCompCnt compCnt;   // at 0x4
    GXCompType compType; // at 0x8
    u8 shift;            // at 0xC
} GXVtxAttrFmtList;

/**
 * @~english @brief Sets the reference method (type) for a single attribute
 * within the current vertex descriptor.
 * @~japanese @brief
 * 現在の頂点記述子中に、1つの属性の参照方法（タイプ）を設定する。
 *
 * @~english @param name Attribute name
 * @~japanese @param name 属性名
 * @~english @param type Reference method
 * @~japanese @param type 参照方法
 */
void GXSetVtxDesc(GXAttr name, GXAttrType type);
void GXSetVtxDescv(const GXVtxDescList* list);
void GXGetVtxDesc(GXAttr name, GXAttrType* type);
void GXGetVtxDescv(GXVtxDescList* list);
/**
 * @~english @brief Sets all of the attributes' types to GX_NONE.
 * @~japanese @brief 全ての属性のタイプをGX_NONEに設定する。
 */
void GXClearVtxDesc(void);
void GXSetVtxAttrFmt(GXVtxFmt fmt, GXAttr attr, GXCompCnt compCnt,
                     GXCompType compType, u8 shift);

// TODO(kiwi) Please find a way to get rid of this
#if defined(GXATTR_MATCH_HACK)
void GXSetVtxAttrFmtv(s16 fmt, const GXVtxAttrFmtList* list);
#else
void GXSetVtxAttrFmtv(GXVtxFmt fmt, const GXVtxAttrFmtList* list);
#endif

void GXGetVtxAttrFmt(GXVtxFmt fmt, GXAttr attr, GXCompCnt* compCnt,
                     GXCompType* compType, u8* shift);
void GXGetVtxAttrFmtv(GXVtxFmt fmt, GXVtxAttrFmtList* list);
/**
 * @~english @brief Sets the stride and a base pointer to an array for a single
 * attribute.
 * @~japanese @brief 1つの属性のストライドと配列へのベースポインタを設定する。
 *
 * @~english @param attr Attribute array name
 * @~japanese @param attr 属性配列名
 * @~english @param base Pointer to the attribute data
 * @~japanese @param base 属性データへのポインタ
 * @~english @param stride Stride between the attribute data elements in bytes
 * @~japanese @param stride バイト単位での属性データ要素間のストライド
 */
void GXSetArray(GXAttr attr, const void* base, u8 stride);
void GXInvalidateVtxCache(void);
/**
 * @~english @brief Specifies how texture coordinates are generated.
 * @~japanese @brief テクスチャ座標生成の方法を指定する。
 *
 * @~english @param id Name of the texture coordinate to be generated
 * @~japanese @param id 生成するテクスチャ座標名
 * @~english @param type Generation method
 * @~japanese @param type 生成方法
 * @param src
 * @~english @param texMtxIdx Matrix function ID
 * @~japanese @param texMtxIdx 行列関数のID
 * @~english @param normalize Specifies whether to normalize coordinates after
 * the first pass of conversion operations
 * @~japanese @param normalize
 * 最初のパスの変換処理後に座標を正規化するかどうかを指定する
 * @param dualTexMtxIdx
 */
void GXSetTexCoordGen2(GXTexCoordID id, GXTexGenType type, GXTexGenSrc src,
                       u32 texMtxIdx, GXBool normalize, u32 dualTexMtxIdx);
void GXSetNumTexGens(u8 num);

void __GXSetVCD(void);
void __GXCalculateVLim(void);
void __GXSetVAT(void);

/**
 * @~english @brief Specifies how texture coordinates are generated.
 * @~japanese @brief テクスチャ座標生成の方法を指定する。
 *
 * @~english @param id Name of the texture coordinate to be generated
 * @~japanese @param id 生成するテクスチャ座標名
 * @~english @param type Generation method
 * @~japanese @param type 生成方法
 * @param src
 * @~english @param texMtxIdx Matrix function ID
 * @~japanese @param texMtxIdx 行列関数のID
 */
static void GXSetTexCoordGen(GXTexCoordID id, GXTexGenType type,
                             GXTexGenSrc src, u32 texMtxIdx) {
    GXSetTexCoordGen2(id, type, src, texMtxIdx, FALSE, GX_PTIDENTITY);
}

#ifdef __cplusplus
}
#endif
#endif
