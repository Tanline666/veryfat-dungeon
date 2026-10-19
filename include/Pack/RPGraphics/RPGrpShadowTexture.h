#ifndef RP_GRAPHICS_SHADOW_TEXTURE_H
#define RP_GRAPHICS_SHADOW_TEXTURE_H
#include <Pack/types_pack.h>

#include <egg/gfxe.h>

//! @addtogroup rp_graphics
//! @{

class RPGrpModel;

class RPGrpShadowTexture : public EGG::ShadowTexture {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param size Texture size (width/height)
     * \~english @param format Texture format
     * \~english @param mdlNum Maximum number of scene models to gather
     */
    RPGrpShadowTexture(u16 size, int format, u16 mdlNum)
        : EGG::ShadowTexture(size, format, mdlNum) {}

    /**
     * \~english @brief Destructor
     */
    virtual ~RPGrpShadowTexture() override {} // at 0x8

    /**
     * \~english @brief Replaces a texture on the specified model
     *
     * \~english @param pModel Target model
     * \~english @param pName Texture name (format string)
     * \~english @param ... Format string arguments
     */
    void Replace(RPGrpModel* pModel, const char* pName, ...);

    /**
     * \~english @brief Adds a new model to the shadow texture
     *
     * \~english @param pModel Model
     * \~english @param density Model density
     */
    void AddModel(RPGrpModel* pModel, f32 density);

    /**
     * \~english @brief Adds a new model with materials to the shadow texture
     *
     * \~english @param pModel Model
     * \~english @param density Model density
     */
    void AddModelUseMaterial(RPGrpModel* pModel, f32 density);
};

//! @}

#endif
