#include <nw4r/ef.h>
#include <nw4r/ut.h>

#include <revolution/OS.h>

namespace {

NW4R_LIB_VERSION(EF, "Jun 25 2009", "15:00:26", "0x4302_145");

} // namespace

namespace nw4r {
namespace ef {

static DrawOrder gBasicDrawOrder;
static DrawStrategyBuilder gBasicDrawStrategyBuilder;
static EmitFormBuilder gBasicEmitFormBuilder;

DrawInfo EffectSystem::mSortEffectOperatorZDrawInfo;

int EffectSystem::mDisplayVersion;
EffectSystem EffectSystem::instance;

EffectSystem::EffectSystem() {
    if (mDisplayVersion == FALSE) {
        mDisplayVersion = TRUE;
        OSRegisterVersion(NW4R_EF_Version_);
    }

    mMemoryManager = NULL;
    mMaxGroupID = 0;
    mTmpEmitter = NULL;
    mXFFlushSafe = false;
}

EffectSystem::~EffectSystem() {
    for (u32 i = 0; i < mMaxGroupID; i++) {
        RetireEffectAll(i);
    }
}

EffectSystem* EffectSystem::GetInstance() {
    return &instance;
}

bool EffectSystem::Initialize(u32 maxGroupID) {
    mMaxGroupID = maxGroupID;

    mActivityList =
        new (mMemoryManager->AllocHeap(mMaxGroupID * sizeof(ActivityList) + 32))
            ActivityList[mMaxGroupID];

    for (int i = 0; i < mMaxGroupID; i++) {
        mActivityList[i].SetOffset(offsetof(Effect, mActivityLink));
        mActivityList[i].Initialize();
    }

    mRandom.Srand(0);

    mDrawOrderFunc = &gBasicDrawOrder;
    mDrawStrategyBuilder = &gBasicDrawStrategyBuilder;
    mEmitFormBuilder = &gBasicEmitFormBuilder;

    return true;
}

bool EffectSystem::Closing(Effect* target) {
    mActivityList[target->mGroupID].ToClosing(target);
    target->mLifeStatus = ReferencedObject::NW4R_EF_LS_CLOSING;

    return true;
}

Effect* EffectSystem::CreateEffect(EmitterResource* eh, u32 groupID,
                                   u16 calcRemain) {

    if (eh == NULL) {
        return NULL;
    }

    if (!(eh->GetEmitterDesc()->commonFlag & EmitterDesc::CMN_FLAG_RELOCATED)) {
        NW4R_WARNING(FALSE, 132, "incomplete relocation (emitter:%s)",
                     eh->GetName());
        return NULL;
    }

    Effect* pEffect = GetMemoryManager()->AllocEffect();
    if (pEffect == NULL) {
        return NULL;
    }

    if (!pEffect->Initialize(this, eh, calcRemain)) {
        GetMemoryManager()->FreeEffect(pEffect);
        return NULL;
    }

    pEffect->mGroupID = groupID;

    mActivityList[groupID].ToActive(pEffect);
    pEffect->mLifeStatus = ReferencedObject::NW4R_EF_LS_ACTIVE;

    mActivityList[groupID].ToWait(pEffect);
    pEffect->mLifeStatus = ReferencedObject::NW4R_EF_LS_WAIT;

    return pEffect;
}

u32 EffectSystem::RetireEffect(Effect* pEffect) {
    if (pEffect->mLifeStatus != ReferencedObject::NW4R_EF_LS_ACTIVE) {
        return 0;
    }

    mActivityList[pEffect->mGroupID].ToWait(pEffect);
    pEffect->Destroy();
    return 1;
}

u32 EffectSystem::RetireEffectAll(u32 groupID) {
    u32 num = 0;
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);

        if (pEffect->mLifeStatus == ReferencedObject::NW4R_EF_LS_ACTIVE) {
            num += RetireEffect(pEffect);
        }
    }

    return num;
}

u32 EffectSystem::RetireEmitterAll(u32 groupID) {
    u32 num = 0;
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);
        num += pEffect->RetireEmitterAll();
    }

    return num;
}

u32 EffectSystem::RetireParticleAll(u32 groupID) {
    u32 num = 0;
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);
        num += pEffect->RetireParticleAll();
    }

    return num;
}

void EffectSystem::Calc(u32 groupID, CalcOption option) {
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);
        pEffect->Calc(option);
    }

    mMemoryManager->GarbageCollection();
}

void EffectSystem::Draw(const DrawInfo& rInfo, u32 groupID) {
    NW4R_UT_LIST_FOREACH (Effect, it, mActivityList[groupID].mActiveList,
                          { it->Draw(rInfo); });
}

void EffectSystem::SetProcessCamera(const math::VEC3& position,
                                    const math::MTX34& matrix, f32 nnear,
                                    f32 ffar) {
    mProcessCameraPos = position;
    math::MTX34Copy(&mProcessCameraMtx, &matrix);
    mProcessCameraNear = nnear;
    mProcessCameraFar = ffar;
}

static void swap(ut::List* utlist, Effect* a, Effect* b) {
    Effect* prev = reinterpret_cast<Effect*>(a->mActivityLink.prevObject);
    Effect* next = reinterpret_cast<Effect*>(b->mActivityLink.nextObject);

    if (!prev) {
        utlist->headObject = b;
    } else {
        prev->mActivityLink.nextObject = b;
    }

    b->mActivityLink.prevObject = prev;
    b->mActivityLink.nextObject = a;
    a->mActivityLink.prevObject = b;
    a->mActivityLink.nextObject = next;

    if (!next) {
        utlist->tailObject = a;
    } else {
        next->mActivityLink.prevObject = a;
    }

    DECOMP_I_BOMB;
}

void EffectSystem::SortEffect(SortEffectOperatorLess less, u32 groupID) {
    NW4R_POINTER_ASSERT(less, 390);

    ut::List& alist = mActivityList[groupID].mActiveList;

    if (UtlistSize(&alist) <= 1) {
        return;
    }

    u16 headIdx = 0;
    u16 tailIdx = static_cast<u16>(UtlistSize(&alist) - 1);

    u16 i, j;

    Effect* lastEffect = NULL;
    for (j = 0; j < 65535; ++j) {
        Effect *a, *b;

        u16 last = headIdx;
        if (!lastEffect) {
            lastEffect = reinterpret_cast<Effect*>(ut::List_GetFirst(&alist));
        }
        a = lastEffect;
        for (i = headIdx; i < tailIdx; ++i) {
            b = reinterpret_cast<Effect*>(ut::List_GetNext(&alist, a));
            if (less(b, a)) {
                swap(&alist, a, b);
                last = i;
                lastEffect = b;
            } else {
                a = b;
            }
        }
        tailIdx = last;
        if (tailIdx == headIdx) {
            return;
        }

        //! Unnecessary assignment? But it is here nonetheless
        last = tailIdx;
        b = lastEffect;
        for (i = tailIdx; i > headIdx; --i) {
            a = reinterpret_cast<Effect*>(ut::List_GetPrev(&alist, b));
            if (less(b, a)) {
                swap(&alist, a, b);
                last = i;
                lastEffect = a;
            } else {
                b = a;
            }
        }
        headIdx = last;
        if (headIdx == tailIdx) {
            return;
        }
    }
}

bool EffectSystem::SortEffectOperatorZ(const Effect* op1, const Effect* op2) {
    NW4R_POINTER_ASSERT(op1, 466);
    NW4R_POINTER_ASSERT(op2, 467);

    const math::MTX34& viewMtx = mSortEffectOperatorZDrawInfo.GetViewMtx();

    const math::MTX34& rootMtx1 = *op1->GetRootMtx();
    const math::MTX34& rootMtx2 = *op2->GetRootMtx();

    f32 z1 = viewMtx._20 * rootMtx1._03 + viewMtx._21 * rootMtx1._13 +
             viewMtx._22 * rootMtx1._23 + viewMtx._23;
    f32 z2 = viewMtx._20 * rootMtx2._03 + viewMtx._21 * rootMtx2._13 +
             viewMtx._22 * rootMtx2._23 + viewMtx._23;

    if (z1 == z2) {
        return FALSE;
    }

    bool result = FALSE;
    if (z1 < 0) {
        if (z2 < 0) {
            result = (z1 < z2);
        } else {
            result = TRUE;
        }
    } else {
        if (z2 < 0) {
            result = FALSE;
        } else {
            result = (z1 > z2);
        }
    }

    const math::MTX34* projMtx = mSortEffectOperatorZDrawInfo.GetProjMtx();
    if (projMtx->_02 == NULL) {
        if (projMtx->_23 < 0) {
            result = !result;
        }
    } else {
        if (projMtx->_22 > 0) {
            result = !result;
        }
    }

    return result;
}

} // namespace ef
} // namespace nw4r
