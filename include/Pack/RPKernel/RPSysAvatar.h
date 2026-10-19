#ifndef RP_KERNEL_AVATAR_H
#define RP_KERNEL_AVATAR_H
#include <Pack/types_pack.h>

#include <Pack/RPKernel/RPSysKokeshiGenInfo.h>
#include <Pack/RPKernel/RPSysKokeshiOverloadInfo.h>
#include <Pack/RPKernel/RP_DEBUG_STUB_1.h>

#include <RVLFaceLib.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Base class for Mii avatars
 */
class RPSysAvatar : public RP_DEBUG_STUB_1 {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pGenInfo Mii generation parameters
     */
    explicit RPSysAvatar(const RPSysKokeshiGenInfo* pGenInfo);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysAvatar() override; // at 0x8

    /**
     * \~english @brief Loads the assets necessary for this avatar
     *
     * \~english @param pOverloadInfo Overload parameters
     */
    virtual void LoadResource(
        const RPSysKokeshiOverloadInfo* pOverloadInfo = NULL); // at 0xC

    /**
     * \~english @brief Applies all lightmap textures to this avatar
     *
     * \~english @param drawScene Draw scene index
     */
    virtual void ApplyLightTexture(u8 drawScene) = 0; // at 0x10

    /**
     * \~english @brief Gets the RFL favorite color of this avatar
     */
    GXColor GetFavoriteColor() const;

    /**
     * \~english @brief Accesses the Mii generation parameters
     */
    const RPSysKokeshiGenInfo& GetGenInfo() const {
        return mGenInfo;
    }

protected:
    /**
     * \~english @brief Handles post-constructor initialization
     */
    void Construct();

    /**
     * \~english @brief Registers this avatar with the kokeshi manager
     */
    void Setup();

protected:
    //! Mii generation parameters
    RPSysKokeshiGenInfo mGenInfo; // at 0x4
    //! Generation overload parameters
    RPSysKokeshiOverloadInfo mOverloadInfo; // at 0x60
    //! Additional info from face library
    RFLAdditionalInfo mAdditionalInfo; // at 0x68
};

//! @}

#endif
