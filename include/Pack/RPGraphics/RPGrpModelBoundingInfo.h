#ifndef RP_GRAPHICS_MODEL_BOUNDING_INFO_H
#define RP_GRAPHICS_MODEL_BOUNDING_INFO_H
#include <Pack/types_pack.h>

#include <egg/gfxe.h>

//! @addtogroup rp_graphics
//! @{

// Forward declarations
class RPGrpModel;

/**
 * \~english @brief AABB and spherical model bounds
 */
class RPGrpModelBoundingInfo : public EGG::ModelBoundingInfo {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param flags Bound type flags
     * \~english @param pModel Owner model
     */
    RPGrpModelBoundingInfo(u32 flags, RPGrpModel* pModel);
};

//! @}

#endif
