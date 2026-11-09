#include <nw4r/ef.h>

namespace nw4r {
namespace ef {

CreationQueue::CreationQueue() : mNumItem(0) {}

void CreationQueue::AddParticleCreation(const EmitterInheritSetting* pSetting,
                                        Particle* pParticle,
                                        EmitterResource* pResource,
                                        u16 calcRemain,
                                        const math::VEC3* pPosition,
                                        const math::VEC3* pVelocity) {

    if (mNumItem >= QUEUE_SIZE) {
        NW4R_WARNING(FALSE, 37, "EffectLib: CreationQueue Full\n");
        return;
    }

    mQueueData[mNumItem].mType = TYPE_PARTICLE;
    mQueueData[mNumItem].mFlags = NULL;
    mQueueData[mNumItem].mCalcRemain = calcRemain;
    mQueueData[mNumItem].mInheritSetting = *pSetting;
    mQueueData[mNumItem].mReferenceParticle = pParticle;

    pParticle->Ref();

    mQueueData[mNumItem].mEmitterResource = pResource;

    if (pPosition) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_POSITION;
        mQueueData[mNumItem].mPosition = *pPosition;
    }

    if (pVelocity) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_VELOCITY;
        mQueueData[mNumItem].mVelocity = *pVelocity;
    }

    mNumItem++;
}

void CreationQueue::AddEmitterCreation(const EmitterInheritSetting* pSetting,
                                       Particle* pParticle,
                                       EmitterResource* pResource,
                                       u16 calcRemain,
                                       const math::VEC3* pPosition,
                                       const math::VEC3* pVelocity) {

    if (mNumItem >= QUEUE_SIZE) {
        NW4R_WARNING(FALSE, 70, "EffectLib: CreationQueue Full\n");
        return;
    }

    mQueueData[mNumItem].mType = TYPE_EMITTER;
    mQueueData[mNumItem].mFlags = NULL;
    mQueueData[mNumItem].mCalcRemain = calcRemain;
    mQueueData[mNumItem].mInheritSetting = *pSetting;
    mQueueData[mNumItem].mReferenceParticle = pParticle;

    pParticle->Ref();

    mQueueData[mNumItem].mEmitterResource = pResource;

    if (pPosition) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_POSITION;
        mQueueData[mNumItem].mPosition = *pPosition;
    }

    if (pVelocity) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_VELOCITY;
        mQueueData[mNumItem].mVelocity = *pVelocity;
    }

    mNumItem++;
}

void CreationQueue::Execute() {
    for (int i = 0; i < mNumItem; i++) {
        math::VEC3* pPosition = NULL;
        math::VEC3* pVelocity = NULL;
        CreationQueueData& rData = mQueueData[i];

        if ((rData.mFlags & CreationQueueData::OPTION_USE_POSITION)) {
            pPosition = &rData.mPosition;
        }

        switch (rData.mType) {
        case TYPE_PARTICLE: {
            rData.mReferenceParticle->mParticleManager->mManagerEM
                ->CreateEmitterTmp(
                    rData.mEmitterResource, &rData.mInheritSetting,
                    rData.mReferenceParticle, rData.mCalcRemain, pPosition);

            rData.mReferenceParticle->UnRef();
            break;
        }

        case TYPE_EMITTER: {
            rData.mReferenceParticle->mParticleManager->mManagerEM
                ->CreateEmitter(rData.mEmitterResource, &rData.mInheritSetting,
                                rData.mReferenceParticle, rData.mCalcRemain,
                                pPosition);

            rData.mReferenceParticle->UnRef();
            break;
        }
        }
    }

    mNumItem = 0;
}

} // namespace ef
} // namespace nw4r
