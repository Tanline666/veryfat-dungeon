#ifndef RVL_SDK_AI_H
#define RVL_SDK_AI_H

/**
 * @addtogroup rvl_ai
 * @{
 */

#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct OSContext OSContext;

typedef void (*AIDMACallback)(void);

typedef enum {
    AI_DSP_32KHZ,
    AI_DSP_48KHZ,
} AIDSPSampleRate;

AIDMACallback AIRegisterDMACallback(AIDMACallback callback);
/**
 * @~english @brief Sets the AI's DMA info.
 * @~japanese @brief AIのDMA情報を設定する。
 *
 * @~english @param buffer Pointer to start address of DMA buffer region
 * @~japanese @param buffer DMAバッファ領域の先頭アドレス
 * @~english @param length Size of DMA buffer region
 * @~japanese @param length DMAバッファ領域のサイズ
 */
void AIInitDMA(void* buffer, u32 length);
void AIStartDMA(void);
u32 AIGetDMABytesLeft(void);
void AISetDSPSampleRate(u32 rate);
u32 AIGetDSPSampleRate(void);
/**
 * @~english @brief Initializes the audio interface (AI).
 * @~japanese @brief オーディオ・インターフェース（Audio Interface,
 * AI）を初期化する。
 *
 * @~english @param stack Pointer to the high address of the stack to be used
 * for AI DMA interrupt callbacks. If NULL, the current stack is used
 * @~japanese @param stack AI DMA割込みのコールバックで使用されるスタックの
 * 上位アドレスへのポインタ。NULLで設定したら、現在のスタックが使用される
 */
void AIInit(void* stack);
/**
 * @~english @brief Checks if AIInit() has been called.
 * @~japanese @brief AIInit()が呼び出したかを調べる。
 *
 * @~english @return TRUE if it has
 * @~japanese @return 呼び出したならTRUE
 */
BOOL AICheckInit(void);
void __AIDHandler(s16 intr, OSContext* ctx);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
