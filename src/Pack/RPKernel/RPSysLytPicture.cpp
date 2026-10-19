#include <Pack/RPKernel.h>

#include <revolution/GX.h>

/**
 * \~english @brief Applies the specified player material color
 * @details The color is applied to TEV color register 1
 *
 * \~english @param idx Player index
 */
void RPSysLytPicture::applyPlayerColor(u32 idx) {
    nw4r::lyt::Material* pMaterial = GetMaterial();

    const GXColor& rColor = RPSysLayout::getPlayerColor(idx);
    GXColorS10 tevColor = {rColor.r, rColor.g, rColor.b, rColor.a};

    pMaterial->SetTevColor(nw4r::lyt::TEVCOLOR_REG1, tevColor);
}

/**
 * \~english @brief Applies the CPU player material color
 * @details The color is applied to TEV color register 1
 */
void RPSysLytPicture::applyCpuColor() {
    nw4r::lyt::Material* pMaterial = GetMaterial();

    const GXColor& rColor = RPSysLayout::getCpuColor();
    GXColorS10 tevColor = {rColor.r, rColor.g, rColor.b, rColor.a};

    pMaterial->SetTevColor(nw4r::lyt::TEVCOLOR_REG1, tevColor);
}
