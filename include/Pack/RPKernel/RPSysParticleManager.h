#ifndef RP_KERNEL_PARTICLE_MANAGER_H
#define RP_KERNEL_PARTICLE_MANAGER_H
#include <Pack/types_pack.h>

#include <nw4r/ef.h>
#include <nw4r/math.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Particle manager
 */
class RPSysParticleManager : public nw4r::ef::ParticleManager {
public:
    /**
     * \~english @brief Constructor
     */
    RPSysParticleManager();

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysParticleManager() override; // at 0x20

    /**
     * \~english @brief Prepares this particle manager for creation
     *
     * \~english @param pParent Parent emitter
     * \~english @param pResource Emitter resource data
     * \~english @return Success
     */
    virtual bool
    Initialize(nw4r::ef::Emitter* pParent,
               nw4r::ef::EmitterResource* pResource) override; // at 0x10

    /**
     * \~english @brief Updates the particles' state and applies any modifiers
     */
    virtual void Calc() override; // at 0x18

    /**
     * \~english @brief Sets the life of all active particles
     *
     * \~english @param life New life
     */
    virtual void SetParticleLife(u16 life); // at 0x24

    /**
     * \~english @brief Sets the size of all active particles
     *
     * \~english @param rSize New size
     */
    virtual void SetParticleSize(const nw4r::math::VEC2& rSize); // at 0x28

    /**
     * \~english @brief Sets the rotation of all active particles
     *
     * \~english @param rRotate New rotation
     */
    virtual void SetParticleRotate(const nw4r::math::VEC3& rRotate); // at 0x2C

private:
    /**
     * \~english @brief Particle modifier flags
     */
    enum {
        EFlag_ModifyLife = 1 << 0,   //!< Life modifier is set
        EFlag_ModifySize = 1 << 1,   //!< Size modifier is set
        EFlag_ModifyRotate = 1 << 2, //!< Rotate modifier is set
    };

private:
    /**
     * \~english @brief Resets all particle modifiers
     */
    virtual void Reset(); // at 0x30

private:
    //! Which modifiers have been set
    u32 mModifyFlags; // at 0xBC

    //! Particle life modifier
    u16 mModifyLife; // at 0xC0
    //! Particle size modifier
    nw4r::math::VEC2 mModifySize; // at 0xC4
    //! Particle rotation modifier
    nw4r::math::VEC3 mModifyRotate;
};

//! @}

#endif
