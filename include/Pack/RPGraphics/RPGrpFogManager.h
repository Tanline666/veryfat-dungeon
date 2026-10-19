#ifndef RP_GRAPHICS_FOG_MANAGER_H
#define RP_GRAPHICS_FOG_MANAGER_H
#include <Pack/types_pack.h>

#include <RPGraphics/RPGrpModelResManager.h>

#include <egg/gfxe.h>

//! @addtogroup rp_graphics
//! @{

/**
 * \~english @brief Fog manager
 */
class RPGrpFogManager : public EGG::FogManager {
public:
    /**
     * \~english @brief Applies fog settings from the specified scene resource
     *
     * \~english @param handle File resource handle
     * \~english @param pName Scene file name
     * \~english @param frame Animation frame (defaults to the first frame)
     */
    void LoadScnFog(RPGrpHandle handle, const char* pName, f32 frame = 0.0f);
};

//! @}

#endif
