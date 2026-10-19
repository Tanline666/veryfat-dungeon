#ifndef RP_KERNEL_KOKESHI_OVERLOAD_INFO_H
#define RP_KERNEL_KOKESHI_OVERLOAD_INFO_H
#include <Pack/types_pack.h>

#include <RVLFaceLib.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Mii generation overload parameters
 */
class RPSysKokeshiOverloadInfo {
private:
    //! The type of clothes to wear
    u16 mClothesType; // at 0x0

public:
    /**
     * \~english @brief Constructor
     */
    RPSysKokeshiOverloadInfo();

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysKokeshiOverloadInfo(); // at 0x8

    /**
     * \~english @brief Gets the type of clothes to wear
     */
    u16 GetClothesType() const {
        return mClothesType;
    }

    /**
     * \~english @brief Sets the type of clothes to wear
     *
     * \~english @param type Clothes type
     */
    void SetClothesType(u16 clothesType) {
        mClothesType = clothesType;
    }
};

//! @}

#endif
