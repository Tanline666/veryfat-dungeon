#ifndef RVL_SDK_GX_LIGHT_H
#define RVL_SDK_GX_LIGHT_H
#include <types.h>

#include <revolution/GX/GXInternal.h>
#include <revolution/GX/GXTypes.h>
#ifdef __cplusplus
extern "C" {
#endif

GX_PUBLIC_STRUCT_DECL(GXLightObj, 64);

void GXInitLightAttn(GXLightObj* light, f32 aa, f32 ab, f32 ac, f32 ka, f32 kb,
                     f32 kc);
void GXInitLightAttnA(GXLightObj* light, f32 a, f32 b, f32 c);
void GXInitLightAttnK(GXLightObj* light, f32 a, f32 b, f32 c);
void GXInitLightSpot(GXLightObj* light, f32 angle, GXSpotFn fn);
void GXInitLightDistAttn(GXLightObj* light, f32 distance, f32 brightness,
                         GXDistAttnFn fn);
void GXInitLightPos(GXLightObj* light, f32 x, f32 y, f32 z);
void GXGetLightPos(const GXLightObj* light, f32* x, f32* y, f32* z);
void GXInitLightDir(GXLightObj* light, f32 x, f32 y, f32 z);
void GXGetLightDir(const GXLightObj* light, f32* x, f32* y, f32* z);
void GXInitSpecularDir(GXLightObj* light, f32 x, f32 y, f32 z);
void GXInitLightColor(GXLightObj* light, GXColor color);
void GXLoadLightObjImm(const GXLightObj* light, GXLightID id);
void GXLoadLightObjIndx(u16 index, GXLightID id);
/**
 * @~english @brief Sets the specified colour channel's ambient colour register.
 * @~japanese @brief
 * 指定したカラーチャンネルのアンビエントカラーレジスタを設定する。
 *
 * @~english @param chan Colour channel ID
 * @~japanese @param chan カラーチャンネルのID
 * @~english @param color New ambient colour
 * @~japanese @param color 新しいアンビエントカラー
 */
void GXSetChanAmbColor(GXChannelID chan, GXColor color);
/**
 * @~english @brief Sets the specified colour channel's material colour
 * register.
 * @~japanese @brief
 * 指定したカラーチャンネルのマテリアルカラーレジスタを設定する。
 *
 * @~english @param chan Colour channel ID
 * @~japanese @param chan カラーチャンネルのID
 * @~english @param color New material colour
 * @~japanese @param color 新しいマテリアルカラー
 */
void GXSetChanMatColor(GXChannelID chan, GXColor color);
/**
 * @~english @brief Sets the number of colour channels output to the TEV stages.
 * @~japanese @brief TEVステージへ出力されたカラーチャンネル数を設定する。
 *
 * @~english @param num Number of colour channels (min 0, max 2)
 * @~japanese @param num カラーチャンネル数（最小0、最大2）
 */
void GXSetNumChans(u8 num);
/**
 * @~english @brief Sets the lighting controls for the specified colour channel.
 * @~japanese @brief 指定したカラーチャンネル用のライトコントロールを設定する。
 *
 * @~english @param chan Colour channel ID
 * @~japanese @param chan カラーチャンネルのID
 * @~english @param enable Enables or disables lighting for the colour channel
 * @~japanese @param enable
 * カラーチャンネル用のライティングの使用を可能か不可能になる
 * @~english @param ambSrc Source of ambient colours
 * @~japanese @param ambSrc アンビエントカラーのソース
 * @~english @param matSrc Source of material colours
 * @~japanese @param matSrc マテリアルカラーのソース
 * @param lightMask
 * @~english @param diffFn Lighting diffusion function
 * @~japanese @param diffFn ライティングの拡散関数
 * @~english @param attnFn Lighting attenuation function
 * @~japanese @param attnFn ライティングの減衰関数
 */
void GXSetChanCtrl(GXChannelID chan, GXBool enable, GXColorSrc ambSrc,
                   GXColorSrc matSrc, GXLightID lightMask, GXDiffuseFn diffFn,
                   GXAttnFn attnFn);

#ifdef __cplusplus
}
#endif
#endif
