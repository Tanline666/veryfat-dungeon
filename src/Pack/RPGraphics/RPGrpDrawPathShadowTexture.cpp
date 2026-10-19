#include <Pack/RPGraphics.h>

#include <egg/gfxe.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param pRenderer Scene renderer
 */
RPGrpDrawPathShadowTexture::RPGrpDrawPathShadowTexture(
    EGG::ScnRenderer* pRenderer)
    : RPGrpDrawPath(pRenderer) {

    mpShadowTextureManager = pRenderer->getShadowTextureManager();
}

/**
 * \~english @brief Inserts a new shadow texture
 *
 * \~english @param idx Texture index
 * \~english @param pTexture Shadow texture
 */
void RPGrpDrawPathShadowTexture::SetTexture(u16 idx,
                                            RPGrpShadowTexture* pTexture) {
    mpShadowTextureManager->Set(idx, pTexture);
}

/**
 * \~english @brief Appends a new shadow texture
 *
 * \~english @param pTexture Shadow texture
 */
void RPGrpDrawPathShadowTexture::SetTexture(RPGrpShadowTexture* pTexture) {
    mpShadowTextureManager->Append(pTexture);
}
