/**
 * @addtogroup rvl_ax
 * @{
 */
#ifndef RVL_SDK_AX_H
#define RVL_SDK_AX_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \~english @brief Initializes the AX library.
 * \~japanese @brief AXライブラリの初期化を行う。
 */
void AXInit(void);
/**
 * \~english @brief Sets the DSP output buffer structure
 * and initializes the AX library.
 * \~japanese @brief DSP出力バッファ構成を設定してAXライブラリの初期化を行う。
 *
 * \~english @param mode Triple-buffer if TRUE, double-buffer if FALSE
 * \~japanese @param mode TRUEであればトリプルバッファ、
 * FALSEであればダブルバッファ
 */
void AXInitEx(u32 mode);

#ifdef __cplusplus
}
#endif
#endif
/**@} */
