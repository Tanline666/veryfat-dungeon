#ifndef NW4R_EF_DRAW_STRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ef {

// Forward declarations
class DrawInfo;
class ParticleManager;

/**
 * @~english @brief Base class for rendering particles.
 * @~japanese @brief パーティクルを描画する基底クラス。
 */
class DrawStrategy {
public:
    /**
     * @~english @brief Constructor
     * @~japanese @brief コンストラクタ
     */
    DrawStrategy() {}
    /**
     * @~english @brief Destructor
     * @~japanese @brief デストラクタ
     */
    virtual ~DrawStrategy() {} // at 0x8

    /**
     * @~english @brief Renders the specified ParticleManager.
     * @~japanese @brief 指定したParticleManagerを描画する。
     *
     * @~english @param drawInfo Constant reference to rendering information
     * @~japanese @param drawInfo 描画情報への定数の参照
     * @param pm
     */
    virtual void Draw(const DrawInfo& drawInfo,
                      ParticleManager* pm) = 0; // at 0xC
};

} // namespace ef
} // namespace nw4r

#endif
