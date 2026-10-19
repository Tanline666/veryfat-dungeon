#ifndef RP_GRAPHICS_CAPTURE_TEXTURE_H
#define RP_GRAPHICS_CAPTURE_TEXTURE_H
#include <Pack/types_pack.h>

#include <egg/gfxe.h>

//! @addtogroup rp_graphics
//! @{

// Forward declarations
class RPGrpScreen;

/**
 * \~english @brief Screen capture texture wrapper
 */
class RPGrpCaptureTexture {
public:
    /**
     * \~english @brief Flags for use with the constructor
     */
    enum {
        //! Whether to enable the mipmap filter
        EFlag_MipMap = 1 << 0,
    };

private:
    //! EFB setting flags
    u16 mEfbFlags; // at 0x0
    //! Texture setting flags
    u16 mTexFlags; // at 0x2

    //! Capture texture implementation
    EGG::CapTexture* mpCapTexture; // at 0x4
    //! Capture texture source
    RPGrpScreen* mpScreen; // at 0x8

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pScreen Capture source
     * \~english @param format Texture format
     * \~english @param flags Creation flags
     */
    RPGrpCaptureTexture(RPGrpScreen* pScreen, GXTexFmt format, u32 flags = 0);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPGrpCaptureTexture() {} // at 0x8

    /**
     * \~english @brief Captures the EFB contents as a texture
     *
     * \~english @param format Texture format override (optional)
     */
    void Capture(int format = -1);

    /**
     * \~english @brief Accesses the underlying capture texture
     */
    EGG::CapTexture* GetCapTexture() const {
        return mpCapTexture;
    }

    /**
     * \~english @brief Tests whether the mipmap filter is enabled
     */
    bool IsMipMapEnable() const {
        return (mTexFlags & ETexFlag_MipMap) != 0;
    }

private:
    /**
     * \~english @brief EFB setting flag
     */
    enum EEfbFlag {
        //! Whether to clear the EFB color channel when copying the texture
        EEfbFlag_ColorUpdate = 1 << 0,
        //! Whether to clear the EFB alpha channel when copying the texture
        EEfbFlag_AlphaUpdate = 1 << 1,
        //! Whether to clear the Z-buffer when copying the texture
        EEfbFlag_ZBufferUpdate = 1 << 2,
    };

    /**
     * \~english @brief Texture settting flag
     */
    enum ETexFlag {
        //! Whether to enable the mipmap filter
        ETexFlag_MipMap = 1 << 0,
    };
};

//! @}

#endif
