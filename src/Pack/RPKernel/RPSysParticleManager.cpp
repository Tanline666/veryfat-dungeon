#include <Pack/RPKernel.h>

/**
 * \~english @brief Constructor
 */
RPSysParticleManager::RPSysParticleManager() : mModifyFlags(0) {
    Reset();
}

/**
 * \~english @brief Destructor
 */
RPSysParticleManager::~RPSysParticleManager() {}

/**
 * \~english @brief Prepares this particle manager for creation
 *
 * \~english @param pParent Parent emitter
 * \~english @param pResource Emitter resource data
 * \~english @return Success
 */
bool RPSysParticleManager::Initialize(nw4r::ef::Emitter* pParent,
                                      nw4r::ef::EmitterResource* pResource) {
    bool success = nw4r::ef::ParticleManager::Initialize(pParent, pResource);

    Reset();
    return success;
}

/**
 * \~english @brief Updates the particles' state and applies any modifiers
 */
void RPSysParticleManager::Calc() {
    nw4r::ef::ParticleManager::Calc();

    if (mModifyFlags == 0) {
        return;
    }

    nw4r::ef::Particle* pPrev = NULL;

    RP_NW4R_LIST_FOREACH (nw4r::ef::Particle, it,
                          GetParticleList()->mActiveList) {
        if (it->mTick != 1) {
            continue;
        }

        if (it->GetLifeStatus() ==
                nw4r::ef::ReferencedObject::NW4R_EF_LS_WAIT ||
            it->GetLifeStatus() ==
                nw4r::ef::ReferencedObject::NW4R_EF_LS_CLOSING) {

            continue;
        }

        if (mModifyFlags & EFlag_ModifyLife) {
            it->mLife = mModifyLife;
        }

        nw4r::ef::ParticleParameter* pParameter = it->GetParticleParameter();
        if (pParameter == NULL) {
            continue;
        }

        if (mModifyFlags & EFlag_ModifySize) {
            pParameter->mSize = mModifySize;
        }

        if (mModifyFlags & EFlag_ModifyRotate) {
            pParameter->mRotate = mModifyRotate;
        }

        pPrev = it;
    }
}

/**
 * \~english @brief Sets the life of all active particles
 *
 * \~english @param life New life
 */
void RPSysParticleManager::SetParticleLife(u16 life) {
    mModifyFlags |= EFlag_ModifyLife;
    mModifyLife = life;
}

/**
 * \~english @brief Sets the size of all active particles
 *
 * \~english @param rSize New size
 */
void RPSysParticleManager::SetParticleSize(const nw4r::math::VEC2& rSize) {
    mModifyFlags |= EFlag_ModifySize;
    mModifySize = rSize;
}

/**
 * \~english @brief Sets the rotation of all active particles
 *
 * \~english @param rRotate New rotation
 */
void RPSysParticleManager::SetParticleRotate(const nw4r::math::VEC3& rRotate) {
    mModifyFlags |= EFlag_ModifyRotate;
    mModifyRotate = rRotate;
}

/**
 * \~english @brief Resets all particle modifiers
 */
void RPSysParticleManager::Reset() {
    mModifyFlags = 0;

    mModifyLife = 0;

    mModifySize.x = 1.0f;
    mModifySize.y = 1.0f;

    mModifyRotate.x = 0.0f;
    mModifyRotate.y = 0.0f;
    mModifyRotate.z = 0.0f;
}
