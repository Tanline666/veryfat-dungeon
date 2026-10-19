#include <Pack/RPGraphics.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param flags Bound type flags
 * \~english @param pModel Owner model
 */
RPGrpModelBoundingInfo::RPGrpModelBoundingInfo(u32 flags, RPGrpModel* pModel)
    : EGG::ModelBoundingInfo(flags, pModel->GetModelEx()) {}
