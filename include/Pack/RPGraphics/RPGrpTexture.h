#ifndef RP_GRAPHICS_TEXTURE_H
#define RP_GRAPHICS_TEXTURE_H
#include <Pack/types_pack.h>

#include <Pack/RPGraphics/RPGrpModelResManager.h>

#include <egg/gfxe.h>

#include <revolution/GX.h>

//! @addtogroup rp_graphics
//! @{

// Forward declarations
class RPGrpCaptureTexture;

/**
 * \~english @brief GPU texture wrapper
 */
class RPGrpTexture {
private:
    //! GX texture object
    GXTexObj mTexObj; // at 0x0

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pCpuTexture CPU texture
     */
    explicit RPGrpTexture(const EGG::CpuTexture* pCpuTexture);

    /**
     * \~english @brief Constructor
     *
     * \~english @param pResTIMG Texture image resource
     */
    explicit RPGrpTexture(const EGG::ResTIMG* pResTIMG);

    /**
     * \~english @brief Constructor
     *
     * \~english @param pCapTexture Capture texture
     */
    explicit RPGrpTexture(const RPGrpCaptureTexture* pCapTexture);

    /**
     * \~english @brief Constructor
     *
     * \~english @param handle Resource file handle
     * \~english @param pName Texture name
     */
    RPGrpTexture(RPGrpHandle handle, const char* pName);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPGrpTexture() {} // at 0x8

    /**
     * \~english @brief Accesses the underlying GX texture object
     */
    const GXTexObj& GetTexObj() const {
        return mTexObj;
    }
};

//! @}

#endif
