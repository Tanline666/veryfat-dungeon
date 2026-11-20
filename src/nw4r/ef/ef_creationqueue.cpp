#include <nw4r/ef.h>

namespace nw4r {
namespace ef {

CreationQueue::CreationQueue() : mNumItem(0) {}

void CreationQueue::AddParticleCreation(const EmitterInheritSetting* setting,
                                        Particle* caller, EmitterResource* eh,
                                        u16 calcRemain,
                                        const math::VEC3* position,
                                        const math::VEC3* velocity) {
    NW4R_POINTER_ASSERT(setting, 32);
    NW4R_POINTER_ASSERT(eh, 33);

    if (mNumItem >= QUEUE_SIZE) {
        NW4R_WARNING(FALSE, 37, "EffectLib: CreationQueue Full\n");
        return;
    }

    mQueueData[mNumItem].mType = TYPE_PARTICLE;
    mQueueData[mNumItem].mFlags = NULL;
    mQueueData[mNumItem].mCalcRemain = calcRemain;
    mQueueData[mNumItem].mInheritSetting = *setting;
    mQueueData[mNumItem].mReferenceParticle = caller;

    caller->Ref();

    mQueueData[mNumItem].mEmitterResource = eh;

    if (position) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_POSITION;
        mQueueData[mNumItem].mPosition = *position;
    }

    if (velocity) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_VELOCITY;
        mQueueData[mNumItem].mVelocity = *velocity;
    }

    mNumItem++;
}

void CreationQueue::AddEmitterCreation(const EmitterInheritSetting* setting,
                                       Particle* caller, EmitterResource* eh,
                                       u16 calcRemain,
                                       const math::VEC3* position,
                                       const math::VEC3* velocity) {
    NW4R_POINTER_ASSERT(setting, 65);
    NW4R_POINTER_ASSERT(eh, 66);

    if (mNumItem >= QUEUE_SIZE) {
        NW4R_WARNING(FALSE, 70, "EffectLib: CreationQueue Full\n");
        return;
    }

    mQueueData[mNumItem].mType = TYPE_EMITTER;
    mQueueData[mNumItem].mFlags = NULL;
    mQueueData[mNumItem].mCalcRemain = calcRemain;
    mQueueData[mNumItem].mInheritSetting = *setting;
    mQueueData[mNumItem].mReferenceParticle = caller;

    caller->Ref();

    mQueueData[mNumItem].mEmitterResource = eh;

    if (position) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_POSITION;
        mQueueData[mNumItem].mPosition = *position;
    }

    if (velocity) {
        mQueueData[mNumItem].mFlags |=
            CreationQueue::CreationQueueData::OPTION_USE_VELOCITY;
        mQueueData[mNumItem].mVelocity = *velocity;
    }

    mNumItem++;
}

void CreationQueue::Execute() {
    for (int i = 0; i < mNumItem; i++) {
        math::VEC3* position = NULL;
        math::VEC3* velocity = NULL;
        CreationQueueData& data = mQueueData[i];

        if ((data.mFlags & CreationQueueData::OPTION_USE_POSITION)) {
            position = &data.mPosition;
        }

        switch (data.mType) {
        case TYPE_PARTICLE: {
            data.mReferenceParticle->mParticleManager->mManagerEM
                ->CreateEmitterTmp(data.mEmitterResource, &data.mInheritSetting,
                                   data.mReferenceParticle, data.mCalcRemain,
                                   position);

            data.mReferenceParticle->UnRef();
            break;
        }

        case TYPE_EMITTER: {
            data.mReferenceParticle->mParticleManager->mManagerEM
                ->CreateEmitter(data.mEmitterResource, &data.mInheritSetting,
                                data.mReferenceParticle, data.mCalcRemain,
                                position);

            data.mReferenceParticle->UnRef();
            break;
        }
        }
    }

    mNumItem = 0;
}

} // namespace ef
} // namespace nw4r
