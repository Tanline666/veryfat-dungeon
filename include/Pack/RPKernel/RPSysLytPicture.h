#ifndef RP_KERNEL_LYT_PICTURE_H
#define RP_KERNEL_LYT_PICTURE_H
#include <Pack/types_pack.h>

#include <nw4r/lyt.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief NW4R layout picture extension
 */
class RPSysLytPicture : public nw4r::lyt::Picture {
public:
    /**
     * \~english @brief Applies the specified player material color
     * @details The color is applied to TEV color register 1
     *
     * \~english @param idx Player index
     */
    void applyPlayerColor(u32 idx);

    /**
     * \~english @brief Applies the CPU player material color
     * @details The color is applied to TEV color register 1
     */
    void applyCpuColor();
};

//! @}

#endif
