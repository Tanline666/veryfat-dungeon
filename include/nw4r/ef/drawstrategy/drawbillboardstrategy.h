#ifndef NW4R_EF_DRAW_BILLBOARD_STRATEGY_H
#define NW4R_EF_DRAW_BILLBOARD_STRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/drawstrategyimpl.h>

namespace nw4r {
namespace ef {

/**
 * @~english @brief Class for rendering billboard objects.
 * @~japanese @brief ビルボードオブジェクトを描画するクラス。
 */
class DrawBillboardStrategy : public DrawStrategyImpl {
public:
    /**
     * @~english @brief Constructor
     * @~japanese @brief コンストラクタ
     */
    DrawBillboardStrategy();

    /**
     * @~english @brief Renders the specified ParticleManager.
     * @~japanese @brief 指定したParticleManagerを描画する。
     *
     * @~english @param drawInfo Constant reference to rendering information
     * @~japanese @param drawInfo 描画情報への定数の参照
     * @param pm
     */
    virtual void Draw(const DrawInfo& drawInfo,
                      ParticleManager* pm); // at 0xC

    virtual CalcAheadFunc
    GetCalcAheadFunc(ParticleManager* pManager); // at 0x18

    void InitGraphics(const DrawInfo& rInfo, ParticleManager* pManager);

private:
    void DrawNormalBillboard(const DrawInfo& rInfo, ParticleManager* pManager);

    inline void DispParticle_Normal(Particle* pParticle,
                                    const math::MTX34& rViewMtx, f32 vx, f32 vy,
                                    f32 vz, f32 rc, f32 rs, f32 sx, f32 sy,
                                    const math::VEC2& rPivot, int flags);

    void DrawYBillboard(const DrawInfo& rInfo, ParticleManager* pManager);

    inline void DispParticle_YBillboard(Particle* pParticle,
                                        const math::MTX34& rViewMtx, f32 vx,
                                        f32 vy, f32 vz, f32 rc, f32 rs, f32 sx,
                                        f32 sy, const math::VEC2& rPivot,
                                        int flags);

    void DrawDirectionalBillboard(const DrawInfo& rInfo,
                                  ParticleManager* pManager);

    inline void DispParticle_Directional(Particle* pParticle,
                                         const math::MTX34& rViewMtx, f32 vx,
                                         f32 vy, f32 vz, f32 rc, f32 rs, f32 sx,
                                         f32 sy, const math::VEC2& rPivot,
                                         int flags);

    void DispPolygon(const math::VEC3& rP, const math::VEC3& rD1,
                     const math::VEC3& rD2, int flags);

    /**
     * @deprecated
     *
     * @param pMtx
     * @param pManager
     * @param rInfo
     * @param offsetZ
     */
    void CalcZOffset(math::MTX34* pMtx, const ParticleManager* pManager,
                     const DrawInfo& rInfo, f32 offsetZ);
};

} // namespace ef
} // namespace nw4r

#endif
