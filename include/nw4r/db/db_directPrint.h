#ifndef NW4R_DB_DIRECT_PRINT_H
#define NW4R_DB_DIRECT_PRINT_H

#include <nw4r/types_nw4r.h>

#include <revolution/GX.h>

//! SOURCE: TFP2

namespace nw4r {
namespace db {

/**
 * \~english @brief Struct that stores information related to the frame buffer
 * used for rendering characters.
 * \~japanese @brief
 * 文字を描画するためのフレームバッファに関する情報を保持するストラクト。
 */
struct FrameBufferInfo {
    u8* frameMemory; //!< \~english Pointer to frame buffer memory \~japanese
                     //!< フレームバッファメモリへのポインタ \~ // at 0x0
    u32 frameSize;   //!< \~english Frame buffer size \~japanese
                     //!< フレームバッファの大きさ \~ // at 0x4
    u16 frameWidth;  //!< \~english Frame buffer width in pixels \~japanese
                     //!< ピクセル単位でのフレームバッファの幅 \~ // at 0x8
    u16 frameHeight; //!< \~english Frame buffer height in pixels \~japanese
                     //!< ピクセル単位でのフレームバッファの高さ \~ // at 0xA
    u16 frameRow;    //!< \~ // at 0xC
    u16 reserved;    //!< \~japanese 予約済み \~ // at 0xE
};

/**
 * \~english @brief Struct that stores info related to YUV-formatted
 * character colours.
 * \~japanese @brief YUV形式の文字色に関する情報を保持するストラクト。
 */
struct YUVColorInfo {
    GXColor colorRGBA; //!< \~english RGB colour data (alpha channel is a dummy
                       //!< variable) \~japanese RGBのカラーデータ
                       //!< (アルファチャンネルはダミー変数）\~ // at 0x0
    u16 colorY256;     //!< \~english Y-component multiplied by 256 \~japanese
                       //!< 256倍したY成分 \~ // at 0x4
    u16 colorU;        //!< \~english U-component \~japanese U成分 \~ // at 0x6
    u16 colorU2; //!< \~english Halved U-component \~japanese 1/2したU成分 \~ //
                 //!< at 0x8
    u16 colorU4; //!< \~english Quartered U-component \~japanese 1/4したU成分 \~
                 //!< // at 0xA
    u16 colorV;  //!< \~english V-component \~japanese V成分 \~ // at 0xC
    u16 colorV2; //!< \~english Halved V-component \~japanese 1/2したV成分 \~ //
                 //!< at 0xE
    u16 colorV4; //!< \~english Quartered V-component \~japanese 1/4したV成分 \~
                 //!< // at 0x10
    u16 reserved; //!< \~japanese 予約済み \~ // at 0x12
};

/**
 * \~english @brief Initializes DirectPrint.
 * \~japanese @brief DirectPrintを初期化する。
 */
void DirectPrint_Init();

/**
 * \~english @brief Checks if DirectPrint is active.
 * \~japanese @brief DirectPrintがアクティブ（使用可能）かを調べる。
 *
 * \~english @return TRUE if active
 * \~japanese @return アクティブであればTRUE
 */
bool DirectPrint_IsActive();

/**
 * \~english @brief Changes the external frame buffer used for display.
 * \~japanese @brief 表示に使用されている外部フレームバッファを変更する。
 *
 * \~english @param framebuf Pointer to the XFB
 * \~japanese @param framebuf XFBへのポインタ
 * \~english @param width XFB width
 * \~japanese @param width XFBの幅
 * \~english @param height XFB height
 * \~japanese @param height XFBの高さ
 */
void DirectPrint_ChangeXfb(void* framebuf, u16 width, u16 height);
/**
 * \~english @brief Changes the external frame buffer used for display.
 * \~japanese @brief 表示に使用されている外部フレームバッファを変更する。
 *
 * \~english @param framebuf Pointer to the XFB
 * \~japanese @param framebuf XFBへのポインタ
 */
void DirectPrint_ChangeXfb(void* framebuf);

/**
 * \~english @brief Sets the character colour.
 * \~japanese @brief 文字色を設定する。
 *
 * \~english @param color Colour in GXColor struct format
 * \~japanese @param color GXColorストラクト形式の色
 */
void DirectPrint_SetColor(GXColor color);
/**
 * \~english @brief Sets the character colour.
 * \~japanese @brief 文字色を設定する。
 *
 * \~english @param r Red component
 * \~japanese @param r 赤成分
 * \~english @param g Green component
 * \~japanese @param g 緑成分
 * \~english @param b Blue component
 * \~japanese @param b 青成分
 */
void DirectPrint_SetColor(u8 r, u8 g, u8 b);

/**
 * \~english @brief Fills a specified rectangular XFB region with black.
 * \~japanese @brief 指定の矩形XFB領域を黒で塗りつぶす。
 *
 * \~english @param posh Upper left position of the region to be filled (X-axis)
 * \~japanese @param posh 塗りつぶす領域の左上の位置（X座軸）
 * \~english @param posv Upper left position of the region to be filled (Y-axis)
 * \~japanese @param posv 塗りつぶす領域の左上の位置（Y座軸）
 * \~english @param sizeh Width of the region to be filled
 * \~japanese @param sizeh 塗りつぶす領域の幅
 * \~english @param sizev Height of the region to be filled
 * \~japanese @param sizev 塗りつぶす領域の高さ
 */
void DirectPrint_EraseXfb(int posh, int posv, int sizeh, int sizev);

/**
 * \~english @brief Renders a string to the XFB using the
 * specified coordinate position.
 * \~japanese @brief 指定の座標位置を使って、XFBに文字列を描画する。
 *
 * \~english @param posh X-coordinate of character rendering position
 * \~japanese @param posh 文字を描画する位置のX座軸
 * \~english @param posv Y-coordinate of character rendering position
 * \~japanese @param posv 文字を描画する位置のY座軸
 * \~english @param format Formatted string
 * \~japanese @param format 書式文字列
 * \~english @param ... Individual variable arguments
 * \~japanese @param ... 個別の可変引数
 */
void DirectPrint_DrawString(int posh, int posv, const char* format, ...);
/**
 * \~english @brief Renders a string to the XFB using the
 * specified coordinate position.
 * \~japanese @brief 指定の座標位置を使って、XFBに文字列を描画する。
 *
 * \~english @param posh X-coordinate of character rendering position
 * \~japanese @param posh 文字を描画する位置のX座軸
 * \~english @param posv Y-coordinate of character rendering position
 * \~japanese @param posv 文字を描画する位置のY座軸
 * \~english @param turnOver Specifies whether to wrap the text if it
 * extends beyond the right side of the screen
 * \~japanese @param turnOver
 * テキストが画面右端を越えた場合には折り返すかどうかを指定する
 * \~english @param format Formatted string
 * \~japanese @param format 書式文字列
 * \~english @param ... Individual variable arguments
 * \~japanese @param ... 個別の可変引数
 */
void DirectPrint_DrawString(int posh, int posv, bool turnOver,
                            const char* format, ...);

/**
 * \~english @brief Stores cached XFB data in memory.
 * \~japanese @brief XFBデータをキャッシュからメモリへストアする。
 */
void DirectPrint_StoreCache();

namespace detail {

void DirectPrint_DrawStringToXfb(int posh, int posv, const char* format,
                                 std::va_list vargs, bool turnOver,
                                 bool backErase);
void* DirectPrint_SetupFB(const GXRenderModeObj* rmode);

} // namespace detail

} // namespace db
} // namespace nw4r

#endif
