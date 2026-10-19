#ifndef RP_GRAPHICS_LIGHT_MANAGER_H
#define RP_GRAPHICS_LIGHT_MANAGER_H
#include <Pack/types_pack.h>

#include <RPGraphics/RPGrpModelResManager.h>

#include <egg/gfxe.h>

//! @addtogroup rp_graphics
//! @{

/**
 * \~english @brief Light manager
 */
class RPGrpLightManager : public EGG::LightManager {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param lightNum Maximum number of light objects
     * \~english @param ambientNum Maximum number of ambient light objects
     * \~english @param viewNum Maximum number of renderer views
     */
    RPGrpLightManager(u32 lightNum, u32 ambientNum, u8 viewNum)
        : EGG::LightManager(lightNum, ambientNum, viewNum) {}

    /**
     * \~english @brief Applies light settings from the specified scene resource
     *
     * \~english @param handle File resource handle
     * \~english @param pName Scene file name
     * \~english @param frame Animation frame (defaults to the first frame)
     * \~english @param lightSetRef Light set reference number (defaults to
     * index 0)
     */
    void LoadScnLight(RPGrpHandle handle, const char* pName, f32 frame = 0.0f,
                      u32 lightSetRef = -1);
};

//! @}

#endif
