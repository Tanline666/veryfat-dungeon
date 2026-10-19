#ifndef RP_GRAPHICS_DRAW_PATH_SHADOW_TEXTURE_H
#define RP_GRAPHICS_DRAW_PATH_SHADOW_TEXTURE_H
#include <Pack/types_pack.h>

#include <Pack/RPGraphics/RPGrpDrawPath.h>

#include <egg/gfxe.h>

//! @addtogroup rp_graphics
//! @{

// Forward declarations
class RPGrpShadowTexture;

/**
 * \~english @brief Shadow texture draw path
 */
class RPGrpDrawPathShadowTexture : public RPGrpDrawPath {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pRenderer Scene renderer
     */
    RPGrpDrawPathShadowTexture(EGG::ScnRenderer* pRenderer);

    /**
     * \~english @brief Inserts a new shadow texture
     *
     * \~english @param idx Texture index
     * \~english @param pTexture Shadow texture
     */
    void SetTexture(u16 idx, RPGrpShadowTexture* pTexture);

    /**
     * \~english @brief Appends a new shadow texture
     *
     * \~english @param pTexture Shadow texture
     */
    void SetTexture(RPGrpShadowTexture* pTexture);

    /**
     * \~english @brief Enables this draw path
     */
    virtual void On() override { // at 0xC
        mpShadowTextureManager->SetEnable(true);
    }

    /**
     * \~english @brief Disables this draw path
     */
    virtual void Off() override { // at 0x10
        mpShadowTextureManager->SetEnable(false);
    }

    /**
     * \~english @brief Tests whether this draw path is enabled
     */
    virtual bool IsEnable() const override { // at 0x10
        return mpShadowTextureManager->IsEnable();
    }

private:
    //! Shadow texture manager
    EGG::ShadowTextureManager* mpShadowTextureManager; // at 0xC
};

//! @}

#endif
