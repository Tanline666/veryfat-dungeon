#ifndef NW4R_EF_DRAW_STRIPE_STRATEGY_H
#define NW4R_EF_DRAW_STRIPE_STRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawStripeStrategy : public DrawStrategyImpl {
public:
    DrawStripeStrategy();

    /**
     * \~english @brief Renders a specified ParticleManager.
     * \~japanese @brief 指定のParticleManagerを描画する。
     *
     * \~english @param rInfo Reference to rendering information
     * \~japanese @param rInfo 描画情報への参照
     * \~english @param pManager Pointer to ParticleManager object
     * \~japanese @param pManager ParticleManagerオブジェクトへのポインタ
     */
    virtual void Draw(const DrawInfo& rInfo,
                      ParticleManager* pManager); //!< at 0xC

protected:
    struct Trigonometric {
        f32 sin; //!< \~english Sine \~japanese 正弦 \~ // at 0x0
        f32 cos; //!< \~english Cosine \~japanese 余弦 \~ // at 0x4
    };

    struct AheadContextStripe : public AheadContext {
        math::VEC3
            mEmitterAxisX; //!< \~english Emitter x-axis in the ParticleManager
                           //!< coordinate system \~japanese
                           //!< ParticleManager座標系におけるエミッタX軸 \~ //
                           //!< at 0xBC
        math::VEC3
            mScreenAxisZ; //!< \~english Screen z-axis in the ParticleManager
                          //!< coordinate system \~japanese
                          //!< ParticleManager座標系におけるスクリーンZ軸 \~ //
                          //!< at 0xC8
        const Trigonometric* mpTrigonometric; //!< at 0xD4

        /**
         * \~english @brief Constructor
         * \~japanese @brief コンストラクタ
         *
         * @param rMtx
         * @param pPM
         */
        AheadContextStripe(const math::MTX34& rMtx, ParticleManager* pManager);
    };

    /**
     * \~english @brief Function typedef that initializes context.
     * \~japanese @brief コンテキストを初期化する関数のtypedef。
     */
    typedef void (*InitAheadFunc)(AheadContextStripe* pContext,
                                  const math::MTX34& rMtx,
                                  ParticleManager* pManager);

    typedef void (*CalcAheadFunc)(math::VEC3* pAxisY,
                                  AheadContextStripe* pContext,
                                  Particle* pParticle);

    struct VertexTube {
        math::VEC3 mCenter; //!< at 0x0
        math::VEC3 mX;      //!< at 0xC
        math::VEC3 mZ;      //!< at 0x18
        f32 mTexCoord;      //!< at 0x24
    };

private:
    virtual CalcAheadFunc
    GetCalcAheadFunc(ParticleManager* pManager); //!< at 0x18

    /**
     * \~english @brief Initializes graphics.
     * \~japanese @brief グラフィックスを初期化する。
     *
     * @param rDrawInfo
     * @param pManager
     */
    void InitGraphics(const DrawInfo& rDrawInfo, ParticleManager* pManager);
};

} // namespace ef
} // namespace nw4r

#endif
