#ifndef RP_KERNEL_EFFECT_MGR_H
#define RP_KERNEL_EFFECT_MGR_H
#include <Pack/types_pack.h>

#include <Pack/RPKernel/RPSysEffectCreator.h>
#include <Pack/RPSingleton.h>

#include <egg/core.h>

//! @addtogroup rp_kernel
//! @{

// Forward declarations
class RPSysEffectCreator;

/**
 * \~english @brief Effect manager
 */
class RPSysEffectMgr {
    RP_SINGLETON_DECL_EX(RPSysEffectMgr);

public:
    /**
     * \~english @brief Effect draw scene ID
     */
    enum EDrawScene {
        EDrawScene_Scene,  //!< Scene effects
        EDrawScene_Cursor, //!< Cursor effects
        EDrawScene_Static, //!< Static/common effects

        EDrawScene_Max
    };

    /**
     * \~english @brief Effect draw group ID
     */
    enum EDrawGroup {
        EDrawGroup_Layer0,
        EDrawGroup_Layer1,
        EDrawGroup_Layer2,
        EDrawGroup_Layer3,

        EDrawGroup_Cursor = 15,
        EDrawGroup_16 = 16,

        EDrawGroup_Max = 64,
    };

    /**
     * \~english @brief Effect update type
     */
    enum ECalcSetting {
        ECalcSetting_All,    //!< Update all effects
        ECalcSetting_Cursor, //!< Only update cursor effects
    };

    /**
     * \~english @brief Effect memory settings
     */
    struct EffectArg {
        //! Heap to use for allocations
        EGG::Heap* pHeap; // at 0x0

        //! Effect object pool size
        int maxEffect; // at 0x4
        //! Emitter object pool size
        int maxEmitter; // at 0x8
        //! Particle manager object pool size
        int maxParticleManager; // at 0xC
        //! Particle object pool size
        int maxParticle; // at 0x10
    };

    /**
     * \~english @brief Framebuffer texture information
     */
    struct EfbTexData {
        //! Texture format (GXTexFmt)
        u8 format; // at 0x0
        //! Texture width
        u16 width; // at 0x2
        //! Texture height
        u16 height; // at 0x4
    };

public:
    /**
     * \~english @brief Attaches a new effect creator to the specified scene
     *
     * \~english @param pCreator Effect creator
     * \~english @param drawScene Effect draw scene
     */
    void setCreator(RPSysEffectCreator* pCreator, u32 drawScene);

    /**
     * \~english @brief Releases the specified effect creator from its scene
     *
     * \~english @param pCreator Effect creator
     */
    void resetCreator(RPSysEffectCreator* pCreator);

    /**
     * \~english @brief Kills all effects and particles
     */
    void stopAllEffect();

    /**
     * \~english @brief Updates the state of effects
     *
     * \~english @param setting Effect update type
     */
    void update(ECalcSetting setting);

    /**
     * \~english @brief Applies a framebuffer capture texture to all effects
     *
     * \~english @param pTexture Framebuffer texture
     * \~english @param rData Framebuffer texture information
     */
    void setCaptureEfb(void* pTexture, const EfbTexData& rData);

    /**
     * \~english @brief Draws the specified set of effects
     *
     * \~english @param rViewMtx View matrix
     * \~english @param drawGroup Effect draw group
     * \~english @param drawScene Effect draw scene
     */
    void draw(const nw4r::math::MTX34& rViewMtx, u32 drawGroup, u32 drawScene);

    /**
     * \~english @brief Creates an instance of the specified effect
     *
     * \~english @param pName Effect name
     * \~english @param drawGroup Effect draw group
     * \~english @param drawScene Effect draw scene
     */
    nw4r::ef::Effect* createEffect(const char* pName, u32 drawGroup,
                                   u32 drawScene);

    /**
     * \~english @brief Creates a memory manager using the specified settings
     *
     * \~english @param pArg Effect memory settings
     * \~english @return Effect work memory size
     */
    u32 createMemoryManager(EffectArg* pArg);

    /**
     * \~english @brief Loads the assets required by system Effect draw scenes
     */
    void LoadResource();

private:
    //! Scene effect creators
    RPSysEffectCreator* mpEffectCreators[EDrawScene_Max]; // at 0x8

    void* unk14;
};

//! @}

#endif
