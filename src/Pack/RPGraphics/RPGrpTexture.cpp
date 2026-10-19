#include <Pack/RPGraphics.h>

#include <egg/gfxe.h>

#include <nw4r/g3d.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param pCpuTexture CPU texture
 */
RPGrpTexture::RPGrpTexture(const EGG::CpuTexture* pCpuTexture) {
    pCpuTexture->getTexObj(&mTexObj);
}

/**
 * \~english @brief Constructor
 *
 * \~english @param pResTIMG Texture image resource
 */
RPGrpTexture::RPGrpTexture(const EGG::ResTIMG* pResTIMG) {
    EGG::GXUtility::getTexObj(&mTexObj, *pResTIMG);
}

/**
 * \~english @brief Constructor
 *
 * \~english @param pCapTexture Capture texture
 */
RPGrpTexture::RPGrpTexture(const RPGrpCaptureTexture* pCapTexture) {
    pCapTexture->GetCapTexture()->getTexObj(&mTexObj);
}

/**
 * \~english @brief Constructor
 *
 * \~english @param handle Resource file handle
 * \~english @param pName Texture name
 */
RPGrpTexture::RPGrpTexture(RPGrpHandle handle, const char* pName) {
    const nw4r::g3d::ResTex tex =
        RPGrpModelResManager::GetCurrent()
            ->GetData<RPGrpModelResManager::Type_ResTex>(handle, pName);

    EGG::GXUtility::getTexObj(&mTexObj, tex, GX_CLAMP, GX_CLAMP, GX_LINEAR,
                              GX_LINEAR);
}
