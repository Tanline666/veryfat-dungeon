#ifndef RP_KERNEL_KOKESHI_ICON_H
#define RP_KERNEL_KOKESHI_ICON_H
#include <Pack/types_pack.h>

#include <Pack/RPGraphics.h>
#include <Pack/RPKernel/RPSysAvatar.h>
#include <Pack/RPKernel/RPSysKokeshiManager.h>

#include <RVLFaceLib.h>

#include <egg/core.h>
#include <egg/gfx.h>

#include <nw4r/g3d.h>

#include <revolution/GX.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Mii face icon
 */
class RPSysKokeshiIcon : public RPSysAvatar {
public:
    /**
     * \~english @brief Light map texture index
     */
    enum ELightMap {
        ELightMap_lm_0, //!< "lm_0.bti" texture
        ELightMap_lm_1, //!< "lm_1.bti" texture

        ELightMap_Max
    };

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pGenInfo Mii generation parameters
     */
    explicit RPSysKokeshiIcon(const RPSysKokeshiGenInfo* pGenInfo);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysKokeshiIcon() override; // at 0x8

    /**
     * \~english @brief Loads the assets necessary for this avatar
     *
     * \~english @param pOverloadInfo Overload parameters
     */
    virtual void LoadResource(const RPSysKokeshiOverloadInfo* pOverloadInfo =
                                  NULL) override; // at 0xC

    /**
     * \~english @brief Applies all lightmap textures to this avatar
     *
     * \~english @param drawScene Draw scene index
     */
    virtual void ApplyLightTexture(u8 drawScene) override; // at 0x10

    /**
     * \~english @brief Gets the texture created for this icon
     */
    EGG::ResTIMG* GetResTIMG();

    /**
     * \~english @brief Gets the Mii capture texture
     */
    EGG::CapTexture* GetCapTexture() const {
        return mpCapTexture;
    }

private:
    /**
     * \~english @brief Configures a model for use with icon rendering
     *
     * \~english @param pModel Mii model
     */
    static void SetupModel(RPGrpModel* pModel);

    /**
     * \~english @brief Creates a cap texture for icon rendering
     *
     * \~english @param width Texture width
     * \~english @param height Texture height
     * \~english @param format Texture format
     */
    void InitCapTexture(u16 width, u16 height, GXTexFmt format);

    /**
     * \~english @brief Configures a NW4R camera to capture view this icon's
     * model
     *
     * \~english @param cam G3D camera
     * \~english @param width Texture width
     * \~english @param height Texture heigth
     * \~english @param view Icon viewpoint
     */
    void SetupCamera(nw4r::g3d::Camera cam, u16 width, u16 height,
                     RPSysKokeshiManager::IconView view);

    /**
     * \~english @brief Prepares the GP rendering state
     */
    void BeginMakeTexture() const;

    /**
     * \~english @brief Renders this icon and captures it to a texture
     *
     * \~english @param pModel Mii model
     * \~english @param rLocation Mii storage location
     * \~english @param pAllocator Memory allocator
     * \~english @param width Texture width
     * \~english @param height Texture height
     * \~english @param format Texture format
     * \~english @param bgType Background color type
     * \~english @param bgColor Background color (for RFLIconBG_Direct)
     * \~english @param view Icon viewpoint
     */
    void MakeTexture(RPGrpModel* pModel, const RPSysKokeshiLocation& rLocation,
                     EGG::Allocator* pAllocator, u16 width, u16 height,
                     GXTexFmt format, RFLIconBGType bgType, GXColor bgColor,
                     RPSysKokeshiManager::IconView view);

    /**
     * \~english @brief Finalizes the GP rendering state
     */
    void EndMakeTexture() const;

    /**
     * \~english @brief Renders an outline in the icon around the Mii's face
     */
    void MakeEdge();

    /**
     * \~english @brief Applies the background color to the icon texture
     *
     * \~english @param rLocation Mii storage location
     * \~english @param width Texture width
     * \~english @param height Texture height
     * \~english @param format Texture format
     * \~english @param bgType Background color type
     * \~english @param bgColor Background color (for RFLIconBG_Direct)
     */
    void ModifyBgColor(const RPSysKokeshiLocation& rLocation, u16 width,
                       u16 height, GXTexFmt format, RFLIconBGType bgType,
                       GXColor bgColor);

    /**
     * \~english @brief Handles post-constructor initialization
     */
    void Construct();

private:
    //! Lightmap texture file names
    static const char* LIGHT_TEXTURE_FILE_NAMES[ELightMap_Max];

private:
    //! Mii capture texture
    EGG::CapTexture* mpCapTexture; // at 0xA4
};

//! @}

#endif
