#ifndef NW4R_SND_FX_CHORUS_H
#define NW4R_SND_FX_CHORUS_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_AxfxImpl.h>
#include <nw4r/snd/snd_FxBase.h>

#include <revolution/AXFX.h>

namespace nw4r {
namespace snd {

namespace detail {
//! Struct arrangement and detail namespace from Tokyo Friend Park II
struct FxChorusParam {
    float delayTime; //!< \~english Time for delay effect \~japanese
                     //!< ディレイエフェクトの遅延時間 \~ // at 0x0
    float depth;     //!< \~english Length of delay modulation \~japanese
                     //!< ディレイ変調の幅 \~ // at 0x4
    float rate;     //!< \~english Delay rate \~japanese ディレイ率 \~ // at 0x8
    float feedback; //!< \~japanese フィードバック \~ // at 0xC
    float
        outGain; //!< \~english Output gain \~japanese 出力ゲイン \~ // at 0x10

    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     */
    FxChorusParam()
        : delayTime(10.0f),
          depth(0.5f),
          rate(1.0f),
          feedback(0.0f),
          outGain(1.0f) {}
};
} // namespace detail

class FxChorus : public FxBase {
public:
    typedef detail::FxChorusParam ChorusParam;

public:
    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     */
    FxChorus();
    /**
     * \~english @brief Destructor
     * \~japanese @brief デストラクタ
     */
    virtual ~FxChorus() {
        Shutdown();
        ReleaseWorkBuffer();
    } // at 0x8
    /**
     * \~english @brief Allocates a buffer region for chorus effects.
     * \~japanese @brief コーラスエフェクトのバッファ領域を割り当てる。
     *
     * \~english @param pBuffer Start address of the buffer region
     * \~japanese @param pBuffer バッファ領域の先頭アドレス
     * \~english @param size Size of the buffer region
     * \~japanese @param size バッファ領域のサイズ
     * \~english @return TRUE if allocation succeeds
     * \~japanese @return 割り当てに成功したらTRUE
     */
    virtual bool AssignWorkBuffer(void* pBuffer, u32 size); // at 0xC
    /**
     * @brief Frees a buffer region for chorus effects.
     * \~japanese @brief コーラスエフェクトのバッファ領域を解放する。
     */
    virtual void ReleaseWorkBuffer(); // at 0x10
    /**
     * \~english @brief Begins chorus effect process.
     * \~japanese @brief コーラスエフェクトの開始処理を行う。
     *
     * \~english @return TRUE if startup succeeds
     * \~japanese @return 開始成功したらTRUE
     */
    virtual bool StartUp(); // at 0x14
    /**
     * \~english @brief Ends chorus effect process.
     * \~japanese @brief コーラスエフェクトの終了処理を行う。
     */
    virtual void Shutdown(); // at 0x18

    /**
     * \~english @brief Chorus effect callback.
     * \~japanese @brief コーラスエフェクトのコールバック。
     *
     * \~english @param channels Number of channels in buffer
     * \~japanese @param channels バッファのチャンネル数
     * \~english @param ppBuffer Array of pointers to buffer
     * \~japanese @param ppBuffer バッファへのポインタ配列
     * \~english @param size Buffer size per channel
     * \~japanese @param 1チャンネルあたりのバッファサイズ
     * \~english @param format Audio sample format
     * \~japanese @param オーディオのサンプルフォーマット
     * \~english @param sampleRate Audio sample rate
     * \~japanese @param オーディオのサンプルレート
     * \~english @param mode Output mode
     * \~japanese @param mode 出力モード
     */
    virtual void UpdateBuffer(int channels, void** ppBuffer, u32 size,
                              SampleFormat format, f32 sampleRate,
                              OutputMode mode); // at 0x1C

    /**
     * \~english @brief Returns the required amount of memory for performing
     * effect operations.
     * \~japanese @brief
     * エフェクト処理を行うために必要とするメモリサイズを取得する。
     *
     * \~english @return The memory size that FxChorus needs
     * \~japanese @return FxChorusの必要となるメモリサイズ
     */
    u32 GetRequiredMemSize();
    /**
     * \~english @brief Sets chorus parameters.
     * \~japanese @brief コーラスパラメータを設定する。
     *
     * \~english @param rParam New parameters
     * \~japanese @param rParam 新しいパラメータ
     * \~english @return TRUE if executed before chorus effect process begins or
     * when successfully performed while effect is operating
     * \~japanese @return
     * コーラスエフェクトの処理を開始する前に実行したらかコーラスエフェクト動作中には成功したらTRUE
     */
    bool SetParam(const ChorusParam& rParam);
    /**
     * @brief Gets current chorus parameters.
     * \~japanese @brief 現在のコーラスパラメータを取得する。
     *
     * \~english @return Chorus parameter values
     * \~japanese @return コーラスパラメータの値
     */
    const ChorusParam& GetParam() const;

private:
    bool mActive;           // at 0xC
    detail::AxfxImpl mImpl; // at 0x10
    ChorusParam mParam;     // at 0x1C
    AXFX_CHORUS mAxfxParam; // at 0x24
};

} // namespace snd
} // namespace nw4r

#endif
