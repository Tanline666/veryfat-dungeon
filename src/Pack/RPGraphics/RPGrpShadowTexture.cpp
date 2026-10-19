#include <Pack/RPGraphics.h>

#include <cstdio>

/**
 * \~english @brief Replaces a texture on the specified model
 *
 * \~english @param pModel Target model
 * \~english @param pName Texture name (format string)
 * \~english @param ... Format string arguments
 */
void RPGrpShadowTexture::Replace(RPGrpModel* pModel, const char* pName, ...) {
    char buffer[256];

    std::va_list list;
    va_start(list, pName);

    // @bug Unsafe string operations
#if defined(BUG_FIX)
    std::vsnprintf(buffer, sizeof(buffer), pName, list);
#else
    std::vsprintf(buffer, pName, list);
#endif

    EGG::ShadowTexture::Replace(pModel->GetModelEx(), buffer);

    va_end(list);
}

/**
 * \~english @brief Adds a new model to the shadow texture
 *
 * \~english @param pModel Model
 * \~english @param density Model density
 */
void RPGrpShadowTexture::AddModel(RPGrpModel* pModel, f32 density) {
    AddModelEx(pModel->GetModelEx(), density);
}

/**
 * \~english @brief Adds a new model with materials to the shadow texture
 *
 * \~english @param pModel Model
 * \~english @param density Model density
 */
void RPGrpShadowTexture::AddModelUseMaterial(RPGrpModel* pModel, f32 density) {
    AddModelEx(pModel->GetModelEx(), density);
    m_mdlList[m_mdlNum - 1].useMat = true;
}
