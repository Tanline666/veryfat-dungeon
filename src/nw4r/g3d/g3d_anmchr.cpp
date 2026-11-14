#include <nw4r/g3d.h>
#include <nw4r/ut.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

NW4R_G3D_RTTI_DEF(AnmObjChr);
NW4R_G3D_RTTI_DEF(AnmObjChrNode);
NW4R_G3D_RTTI_DEF(AnmObjChrBlend);
NW4R_G3D_RTTI_DEF(AnmObjChrRes);

namespace {

u32 GetPartialNodeEndId(const ResMdl mdl, u32 target) {
    ResNode targetNode = mdl.GetResNode(target);
    ResNode node = targetNode;

    ResNode endNode = node.GetNextSibling();
    while (!endNode.IsValid()) {
        node = node.GetParentNode();
        if (!node.IsValid()) {
            break;
        }

        endNode = node.GetNextSibling();
    }

    if (endNode.IsValid()) {
        return endNode.GetID();
    }

    return mdl.GetResNodeNumEntries();
}

} // namespace

/******************************************************************************
 *
 * AnmObjChr
 *
 ******************************************************************************/
AnmObjChr::AnmObjChr(MEMAllocator* pHeap, u16* pBindingBuffer, int numBinding)
    : AnmObj(pHeap, NULL), mNumBinding(numBinding), mpBinding(pBindingBuffer) {

    Release();
}

bool AnmObjChr::TestExistence(u32 idx) const {
    return !(mpBinding[idx] & (BINDING_UNDEFINED | BINDING_INVALID));
}

bool AnmObjChr::TestDefined(u32 idx) const {
    return !(mpBinding[idx] & BINDING_UNDEFINED);
}

void AnmObjChr::Release() {
    for (int i = 0; i < mNumBinding; i++) {
        mpBinding[i] = BINDING_UNDEFINED;
    }

    SetAnmFlag(FLAG_ANM_BOUND, false);
}

AnmObjChrRes* AnmObjChr::Attach(int idx, AnmObjChrRes* pObj) {
#pragma unused(idx)
#pragma unused(pObj)

    return NULL;
}

AnmObjChrRes* AnmObjChr::Detach(int idx) {
#pragma unused(idx)

    return NULL;
}

void AnmObjChr::SetWeight(int idx, f32 weight) {
#pragma unused(idx)
#pragma unused(weight)
    NW4R_POINTER_ASSERT(this, 160);

    //! @bug Copy-paste error
#if defined(BUG_FIX)
    NW4R_WARNING(
        FALSE, 163,
        "AnmObjChr::SetWeight(%d, %f) called with %s. Maybe uninteded call.",
        idx, weight, GetTypeName());
#else
    NW4R_WARNING(
        FALSE, 163,
        "AnmObjChr::Attach(%d, %f) called with %s. Maybe uninteded call.", idx,
        weight, GetTypeName());
#endif
}

f32 AnmObjChr::GetWeight(int idx) const {
#pragma unused(idx)
    NW4R_POINTER_ASSERT(this, 173);
    NW4R_WARNING(
        FALSE, 175,
        "AnmObjChr::GetWeight(%d) called with %s. Maybe uninteded call.", idx,
        GetTypeName());

    return 0.0f;
}

void AnmObjChr::DetachAll() {
    NW4R_POINTER_ASSERT(this, 187);
    NW4R_WARNING(FALSE, 189,
                 "AnmObjChr::DetachAll() called with %s. Maybe uninteded call.",
                 GetTypeName());
}

/******************************************************************************
 *
 * AnmObjChrNode
 *
 ******************************************************************************/
AnmObjChrNode::AnmObjChrNode(MEMAllocator* pHeap, u16* pBindingBuffer,
                             int numBinding, AnmObjChrRes** pChildrenArray,
                             int numChildren)
    : AnmObjChr(pHeap, pBindingBuffer, numBinding),
      mChildrenArraySize(numChildren),
      mpChildrenArray(pChildrenArray) {

    NW4R_POINTER_ASSERT(pHeap, 212);
    NW4R_POINTER_ASSERT(pBindingBuffer, 213);
    NW4R_POINTER_ASSERT(pChildrenArray, 214);

    for (int i = 0; i < mChildrenArraySize; i++) {
        mpChildrenArray[i] = NULL;
    }
}

AnmObjChrNode::~AnmObjChrNode() {
    NW4R_POINTER_ASSERT(this, 225);
    DetachAll();
}

AnmObjChrRes* AnmObjChrNode::Attach(int idx, AnmObjChrRes* pObj) {
    NW4R_POINTER_ASSERT(this, 232);
    NW4R_MINMAX_ASSERT(idx, 0, mChildrenArraySize - 1, 233);
    NW4R_POINTER_ASSERT(pObj, 234);

    AnmObjChrRes* pOld = Detach(idx);
    bool hasAnm = false;

    for (u32 i = 0; i < mNumBinding; i++) {
        if (!pObj->TestDefined(i)) {
            continue;
        }

        hasAnm = true;
        mpBinding[i] = 0;
    }

    if (hasAnm) {
        SetAnmFlag(FLAG_ANM_BOUND, true);
    }

    mpChildrenArray[idx] = pObj;
    pObj->G3dProc(G3DPROC_ATTACH_PARENT, 0, this);
    return pOld;
}

AnmObjChrRes* AnmObjChrNode::Detach(int idx) {
    NW4R_POINTER_ASSERT(this, 263);
    NW4R_MINMAX_ASSERT(idx, 0, mChildrenArraySize - 1, 264);

    AnmObjChrRes* pOld = mpChildrenArray[idx];

    if (pOld != NULL) {
        NW4R_POINTER_ASSERT(pOld, 271);

        pOld->G3dProc(G3DPROC_DETACH_PARENT, 0, this);
        mpChildrenArray[idx] = NULL;

        bool hasAnm = false;
        for (u32 i = 0; i < mNumBinding; i++) {
            u16 binding = BINDING_UNDEFINED;

            for (int j = 0; j < mChildrenArraySize; j++) {
                AnmObjChrRes* pChild = mpChildrenArray[j];

                if (pChild == NULL || !pChild->TestDefined(i)) {
                    continue;
                }

                hasAnm = true;
                binding = 0;
                break;
            }

            mpBinding[i] = binding;
        }

        if (!hasAnm) {
            SetAnmFlag(FLAG_ANM_BOUND, false);
        }
    }

    return pOld;
}

void AnmObjChrNode::DetachAll() {
    NW4R_POINTER_ASSERT(this, 311);

    for (int i = 0; i < mChildrenArraySize; i++) {
        Detach(i);
    }
}

void AnmObjChrNode::UpdateFrame() {
    NW4R_POINTER_ASSERT(this, 322);

    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != NULL) {
            mpChildrenArray[i]->UpdateFrame();
        }
    }
}

void AnmObjChrNode::SetFrame(f32 frame) {
    NW4R_POINTER_ASSERT(this, 336);
    NW4R_FLOAT_ASSERT(frame, 337);

    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != NULL) {
            mpChildrenArray[i]->SetFrame(frame);
        }
    }
}

f32 AnmObjChrNode::GetFrame() const {
    NW4R_POINTER_ASSERT(this, 351);

    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != NULL) {
            return mpChildrenArray[i]->GetFrame();
        }
    }

    return 0.0f;
}

void AnmObjChrNode::SetUpdateRate(f32 rate) {
    NW4R_POINTER_ASSERT(this, 367);
    NW4R_FLOAT_ASSERT(rate, 368);

    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != NULL) {
            mpChildrenArray[i]->SetUpdateRate(rate);
        }
    }
}

f32 AnmObjChrNode::GetUpdateRate() const {
    NW4R_POINTER_ASSERT(this, 382);

    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != NULL) {
            return mpChildrenArray[i]->GetUpdateRate();
        }
    }

    return 1.0f;
}

bool AnmObjChrNode::Bind(const ResMdl resMdl) {
    NW4R_POINTER_ASSERT(this, 398);
    NW4R_ASSERT(resMdl.IsValid(), 399);

    bool success = false;

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChrRes* pChild = mpChildrenArray[i];
        if (pChild == NULL) {
            continue;
        }

        bool childSuccess = pChild->Bind(resMdl);
        success = success || childSuccess;

        for (u32 j = 0; j < mNumBinding; j++) {
            if (pChild->TestDefined(j)) {
                mpBinding[j] = 0;
            }
        }
    }

    SetAnmFlag(FLAG_ANM_BOUND, true);
    return success;
}

bool AnmObjChrNode::Bind(const ResMdl resMdl, u32 target, BindOption option) {
    NW4R_POINTER_ASSERT(this, 438);
    NW4R_ASSERT(resMdl.IsValid(), 439);

    bool success = false;

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChrRes* pChild = mpChildrenArray[i];
        if (pChild == NULL) {
            continue;
        }

        bool childSuccess = pChild->Bind(resMdl, target, option);
        success = success || childSuccess;

        for (u32 j = 0; j < mNumBinding; j++) {
            if (pChild->TestDefined(j)) {
                mpBinding[j] = 0;
            }
        }
    }

    SetAnmFlag(FLAG_ANM_BOUND, true);
    return success;
}

void AnmObjChrNode::Release() {
    NW4R_POINTER_ASSERT(this, 475);

    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != NULL) {
            mpChildrenArray[i]->Release();
        }
    }

    AnmObjChr::Release();
}

void AnmObjChrNode::Release(const ResMdl resMdl, u32 target,
                            BindOption option) {
    NW4R_POINTER_ASSERT(this, 496);
    NW4R_ASSERT(resMdl.IsValid(), 497);

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChrRes* pChild = mpChildrenArray[i];

        if (pChild != NULL) {
            pChild->Release(resMdl, target, option);
        }
    }

    AnmObjChr::Release();

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChrRes* pChild = mpChildrenArray[i];
        if (pChild == NULL) {
            continue;
        }

        for (u32 j = 0; j < mNumBinding; j++) {
            if (pChild->TestDefined(j)) {
                mpBinding[j] = 0;
            }
        }
    }
}

void AnmObjChrNode::G3dProc(u32 task, u32 param, void* pInfo) {
#pragma unused(param)
    NW4R_POINTER_ASSERT(this, 536);

    switch (task) {
    case G3DPROC_CHILD_DETACHED: {
        int idx;
        for (idx = 0; idx < mChildrenArraySize; idx++) {
            if (mpChildrenArray[idx] == pInfo) {
                Detach(idx);
                return;
            }
        }

        NW4R_WARNING(idx < mChildrenArraySize, 554,
                     "AnmObjChrNode::G3dProc(G3DPROC_CHILD_DETACHED,%p,%p)\n"
                     "The child not found.",
                     param, pInfo);

        break;
    }

    case G3DPROC_DETACH_PARENT: {
        NW4R_ASSERT(GetParent(), 563);
        SetParent(NULL);
        break;
    }

    case G3DPROC_ATTACH_PARENT: {
        NW4R_ASSERT(!GetParent(), 570);
        SetParent(static_cast<G3dObj*>(pInfo));
        break;
    }
    }
}

/******************************************************************************
 *
 * AnmObjChrBlend
 *
 ******************************************************************************/
AnmObjChrBlend* AnmObjChrBlend::Construct(MEMAllocator* pHeap, u32* pSize,
                                          ResMdl mdl, int numChildren) {
    if (!mdl.IsValid()) {
        return NULL;
    }

    int bindNum = mdl.GetResNodeNumEntries();

    u32 bindSize = bindNum * sizeof(u16);
    u32 childrenSize = numChildren * sizeof(AnmObjChrRes*);
    u32 weightSize = numChildren * sizeof(f32);

    u32 bindOfs = ut::RoundUp(sizeof(AnmObjChrBlend), 4);
    u32 childrenOfs = ut::RoundUp(bindOfs + bindSize, 4);
    u32 weightOfs = ut::RoundUp(childrenOfs + childrenSize, 4);

    u32 size = ut::RoundUp(weightOfs + weightSize, 4);
    if (pSize != NULL) {
        NW4R_POINTER_ASSERT(pSize, 615);
        *pSize = size;
    }

    if (pHeap == NULL) {
        return NULL;
    }
    NW4R_POINTER_ASSERT(pHeap, 624);

    u8* buf = reinterpret_cast<u8*>(Alloc(pHeap, size));
    if (buf == NULL) {
        return NULL;
    }
    NW4R_POINTER_ASSERT(buf, 634);
    NW4R_ALIGN_ASSERT(buf, 4, 635);

    // clang-format off
    return new (buf) AnmObjChrBlend(
        pHeap,
        reinterpret_cast<u16*>(buf + bindOfs),
        bindNum,
        reinterpret_cast<AnmObjChrRes**>(buf + childrenOfs),
        numChildren,
        reinterpret_cast<f32*>(buf + weightOfs));
    // clang-format on
}

AnmObjChrBlend::AnmObjChrBlend(MEMAllocator* pHeap, u16* pBindingBuffer,
                               int numBinding, AnmObjChrRes** ppChildrenBuf,
                               int numChildren, f32* pWeightBuf)
    : AnmObjChrNode(pHeap, pBindingBuffer, numBinding, ppChildrenBuf,
                    numChildren),
      mpWeightArray(pWeightBuf) {

    NW4R_POINTER_ASSERT(this, 658);

    for (int i = 0; i < mChildrenArraySize; i++) {
        mpWeightArray[i] = 1.0f;
    }
}

const ChrAnmResult* AnmObjChrBlend::GetResult(ChrAnmResult* pResult,
                                              u32 nodeId) {
    NW4R_POINTER_ASSERT(this, 669);
    NW4R_POINTER_ASSERT(pResult, 670);
    NW4R_MAX_ASSERT(int(nodeId), mNumBinding - 1, 671);
    AnmObjChrRes* pFirstChild = NULL;
    int blendNum = 0;
    f32 weightSum = 0.0f;

    for (u32 i = 0; static_cast<int>(i) < mChildrenArraySize; i++) {
        f32 weight = mpWeightArray[i];
        AnmObjChrRes* pChild = mpChildrenArray[i];

        if (pChild == NULL) {
            continue;
        }

        if (weight == 0.0f || !pChild->TestExistence(nodeId)) {
            continue;
        }

        NW4R_ASSERT(weight > 0.0f, 685);

        blendNum++;
        weightSum += weight;

        if (pFirstChild == NULL) {
            pFirstChild = pChild;
        }
    }

    if (blendNum == 0) {
        pResult->flags = 0;
        return pResult;
    }

    if (blendNum == 1) {
        return pFirstChild->GetResult(pResult, nodeId);
    }

    bool bUseQuaternion = TestAnmFlag(FLAG_USE_QUATERNION_ROTATION_BLEND);
    bool useAccScale = TestAnmFlag(FLAG_USE_ACCURATE_SCALE_BLEND);

    f32 esx = 0.0f;
    f32 esy = 0.0f;
    f32 esz = 0.0f;
    math::QUAT rot(0.0f, 0.0f, 0.0f, 1.0f);

    f32 addedWeight = 0.0f;
    math::MTX33 firstRot;
    f32 invWeightSum = math::FInv(weightSum);

    pResult->flags = 0xFFFFFFFF;
    pResult->s.x = 0.0f;
    pResult->s.y = 0.0f;
    pResult->s.z = 0.0f;
    math::MTX34Zero(&pResult->rt);

    for (int i = 0; i < mChildrenArraySize; i++) {
        ChrAnmResult resultBuf;

        AnmObjChrRes* pChild = mpChildrenArray[i];
        f32 weight = mpWeightArray[i];

        if (pChild == NULL || weight == 0.0f ||
            !pChild->TestExistence(nodeId)) {
            continue;
        }

        const ChrAnmResult* pMyResult = pChild->GetResult(&resultBuf, nodeId);

        f32 ratio = weight * invWeightSum;
        u32 flags = pMyResult->flags;

        NW4R_ASSERT(pMyResult->flags & ChrAnmResult::FLAG_ANM_EXISTS, 748);

        if (flags &
            (ChrAnmResult::FLAG_PATCH_SCALE | ChrAnmResult::FLAG_PATCH_ROT |
             ChrAnmResult::FLAG_PATCH_TRANS)) {
            NW4R_INTERNAL_ERROR(755,
                                "Blending is not yet implemented for character "
                                "animations that use model data.");
        }

        if (useAccScale) {
            if (!(flags & ChrAnmResult::FLAG_SCALE_ONE)) {
                esx += ratio * math::FLog(pMyResult->s.x);
                esy += ratio * math::FLog(pMyResult->s.y);
                esz += ratio * math::FLog(pMyResult->s.z);
            }
        } else if (!(flags & ChrAnmResult::FLAG_SCALE_ONE)) {
            pResult->s.x += pMyResult->s.x * ratio;
            pResult->s.y += pMyResult->s.y * ratio;
            pResult->s.z += pMyResult->s.z * ratio;
        } else {
            pResult->s.x += ratio;
            pResult->s.y += ratio;
            pResult->s.z += ratio;
        }

        if (bUseQuaternion) {
            math::QUAT q;

            if (!(flags & ChrAnmResult::FLAG_ROT_ZERO)) {
                math::MTX34ToQUAT(&q, &pMyResult->rt);
            } else {
                q.x = 0.0f;
                q.y = 0.0f;
                q.z = 0.0f;
                q.w = 1.0f;
            }

            addedWeight += weight;
            f32 invAddedWeight = math::FInv(addedWeight);

            f32 t = weight * invAddedWeight;
            math::C_QUATSlerp(&rot, &rot, &q, t);
        } else if (!(flags & ChrAnmResult::FLAG_ROT_ZERO)) {
            if (!i) {
                firstRot._00 = pMyResult->rt._00;
                firstRot._01 = pMyResult->rt._01;
                firstRot._02 = pMyResult->rt._02;
                firstRot._10 = pMyResult->rt._10;
                firstRot._11 = pMyResult->rt._11;
                firstRot._12 = pMyResult->rt._12;
                firstRot._20 = pMyResult->rt._20;
                firstRot._21 = pMyResult->rt._21;
                firstRot._22 = pMyResult->rt._22;
            }
            pResult->rt._00 += pMyResult->rt._00 * ratio;
            pResult->rt._01 += pMyResult->rt._01 * ratio;
            pResult->rt._02 += pMyResult->rt._02 * ratio;

            pResult->rt._10 += pMyResult->rt._10 * ratio;
            pResult->rt._11 += pMyResult->rt._11 * ratio;
            pResult->rt._12 += pMyResult->rt._12 * ratio;
        } else {
            if (!i) {
                math::MTX33Identity(&firstRot);
            }
            pResult->rt._00 += ratio;
            pResult->rt._11 += ratio;
        }

        if (!(flags & ChrAnmResult::FLAG_TRANS_ZERO)) {
            pResult->rt._03 += pMyResult->rt._03 * ratio;
            pResult->rt._13 += pMyResult->rt._13 * ratio;
            pResult->rt._23 += pMyResult->rt._23 * ratio;
        }

        pResult->flags &= flags;
    }

    if (useAccScale) {
        pResult->s.x = math::FExp(esx);
        pResult->s.y = math::FExp(esy);
        pResult->s.z = math::FExp(esz);
    }

    if (bUseQuaternion) {
        Vec t;
        t.x = pResult->rt._03;
        t.y = pResult->rt._13;
        t.z = pResult->rt._23;

        math::QUATToMTX34(&pResult->rt, &rot);

        pResult->rt._03 = t.x;
        pResult->rt._13 = t.y;
        pResult->rt._23 = t.z;
    } else {
        math::VEC3* v0 = reinterpret_cast<math::VEC3*>(&pResult->rt._00);
        math::VEC3* v1 = reinterpret_cast<math::VEC3*>(&pResult->rt._10);
        math::VEC3* v2 = reinterpret_cast<math::VEC3*>(&pResult->rt._20);
        math::VEC3Cross(v2, v0, v1);

        if (math::VEC3LenSq(v0) == 0.0f || math::VEC3LenSq(v2) == 0.0f) {
#if defined(DEBUG) || defined(TEX_DEV)
            NW4R_WARNING(FALSE, 907,
                         "The axis of the blended rotation is a zero vector.");
#endif
            pResult->rt._00 = firstRot._00;
            pResult->rt._01 = firstRot._01;
            pResult->rt._02 = firstRot._02;
            pResult->rt._10 = firstRot._10;
            pResult->rt._11 = firstRot._11;
            pResult->rt._12 = firstRot._12;
            pResult->rt._20 = firstRot._20;
            pResult->rt._21 = firstRot._21;
            pResult->rt._22 = firstRot._22;
        } else {
            math::VEC3Normalize(v0, v0);
            math::VEC3Normalize(v2, v2);
            math::VEC3Cross(v1, v2, v0);
        }
    }

    pResult->flags &= ~ChrAnmResult::FLAG_ROT_RAW_FMT;
    return pResult;
}

void AnmObjChrBlend::SetWeight(int idx, f32 weight) {
    NW4R_POINTER_ASSERT(this, 942);
    NW4R_MINMAX_ASSERT(idx, 0, mChildrenArraySize - 1, 943);
    NW4R_FMIN_ASSERT(weight, 0, 944);
    mpWeightArray[idx] = weight;
}

f32 AnmObjChrBlend::GetWeight(int idx) const {
    NW4R_POINTER_ASSERT(this, 951);
    NW4R_MINMAX_ASSERT(idx, 0, mChildrenArraySize - 1, 952);
    return mpWeightArray[idx];
}

/******************************************************************************
 *
 * AnmObjChrRes
 *
 ******************************************************************************/
AnmObjChrRes* AnmObjChrRes::Construct(MEMAllocator* pHeap, u32* pSize,
                                      ResAnmChr chr, ResMdl mdl, bool cache) {
    if (!chr.IsValid() || !mdl.IsValid()) {
        return NULL;
    }

    int numAnim = chr.GetNumNode();
    int numMat = mdl.GetResNodeNumEntries();

    int bindNum = numMat;
    int cacheNum = cache ? numAnim : 0;

    u32 bindSize = bindNum * sizeof(u16);
    u32 cacheSize = cacheNum * sizeof(ChrAnmResult);

    u32 size = bindSize + cacheSize + sizeof(AnmObjChrRes);
    if (pSize != NULL) {
        NW4R_POINTER_ASSERT(pSize, 987);
        *pSize = size;
    }

    if (pHeap == NULL) {
        return NULL;
    }
    NW4R_POINTER_ASSERT(pHeap, 996);

    u8* buf = reinterpret_cast<u8*>(Alloc(pHeap, size));
    if (buf == NULL) {
        return NULL;
    }
    NW4R_POINTER_ASSERT(buf, 1006);

    ChrAnmResult* pCacheBuf =
        cache ? reinterpret_cast<ChrAnmResult*>(buf + sizeof(AnmObjChrRes))
              : NULL;

    u16* pBindBuf =
        reinterpret_cast<u16*>(cacheSize + (buf + sizeof(AnmObjChrRes)));

    NW4R_ALIGN_ASSERT(buf, 4, 1010);
    NW4R_ALIGN_ASSERT(pCacheBuf, 4, 1011);
    NW4R_ALIGN_ASSERT(pBindBuf, 2, 1012);

    return new (buf) AnmObjChrRes(pHeap, chr, pBindBuf, bindNum, pCacheBuf);
}

AnmObjChrRes::AnmObjChrRes(MEMAllocator* pHeap, ResAnmChr chr,
                           u16* pBindingBuffer, int numBinding,
                           ChrAnmResult* pCacheBuf)
    : AnmObjChr(pHeap, pBindingBuffer, numBinding),
      FrameCtrl(0.0f, chr.GetNumFrame(), GetAnmPlayPolicy(chr.GetAnmPolicy())),
      mRes(chr),
      mpResultCache(pCacheBuf) {

    NW4R_POINTER_ASSERT(this, 1039);

    if (mpResultCache != NULL) {
        UpdateCache();
    }
}

void AnmObjChrRes::SetFrame(f32 frame) {
    NW4R_POINTER_ASSERT(this, 1049);
    NW4R_FLOAT_ASSERT(frame, 1050);

    SetFrm(frame);

    if (mpResultCache != NULL) {
        UpdateCache();
    }
}

f32 AnmObjChrRes::GetFrame() const {
    NW4R_POINTER_ASSERT(this, 1064);

    return GetFrm();
}

void AnmObjChrRes::SetUpdateRate(f32 rate) {
    NW4R_POINTER_ASSERT(this, 1072);
    NW4R_FLOAT_ASSERT(rate, 1073);

    SetRate(rate);
    if (rate == 0.0f && mpResultCache) {
        UpdateCache();
    }
}

f32 AnmObjChrRes::GetUpdateRate() const {
    NW4R_POINTER_ASSERT(this, 1088);

    return GetRate();
}

void AnmObjChrRes::UpdateFrame() {
    NW4R_POINTER_ASSERT(this, 1096);

    if (GetRate() == 0.0f) {
        return;
    }

    UpdateFrm();

    if (mpResultCache != NULL) {
        UpdateCache();
    }
}

bool AnmObjChrRes::Bind(const ResMdl mdl) {
    int numAnim = mRes.GetNumNode();
    bool success = false;

    for (u16 i = 0; i < numAnim; i++) {
        const ResAnmChrNodeData* pData = mRes.GetNodeAnm(i);

        // Seek back from name string to start of ResName
        ResName name(ut::AddOffsetToPtr(pData, pData->name - 4));

        ResNode node = mdl.GetResNode(name);
        if (!node.IsValid()) {
            continue;
        }

        mpBinding[node.GetID()] = i;
        success = true;
    }

    SetAnmFlag(FLAG_ANM_BOUND, true);
    return success;
}

bool AnmObjChrRes::Bind(const ResMdl mdl, u32 target, BindOption option) {
    bool success = false;

    switch (option) {
    case BIND_PARTIAL: {
        u32 bindBegin = target;
        u32 bindEnd = GetPartialNodeEndId(mdl, bindBegin);

        for (u32 i = bindBegin; i < bindEnd; i++) {
            ResNode node = mdl.GetResNode(i);
            ResName name = node.GetResName();

            int id = mRes.GetNodeAnmIndex(name);
            if (id != ResDic::NOT_FOUND) {
                mpBinding[i] = static_cast<u16>(id);
                success = true;
            }
        }

        break;
    }

    case BIND_ONE: {
        ResNode node = mdl.GetResNode(target);
        ResName name = node.GetResName();

        int id = mRes.GetNodeAnmIndex(name);
        if (id != ResDic::NOT_FOUND) {
            mpBinding[target] = static_cast<u16>(id);
            success = true;
        }
        break;
    }

    default: {
        break;
    }
    }

    SetAnmFlag(FLAG_ANM_BOUND, true);
    return success;
}

void AnmObjChrRes::Release(const ResMdl mdl, u32 target, BindOption option) {
    switch (option) {
    case BIND_PARTIAL: {
        u32 bindBegin = target;
        u32 bindEnd = GetPartialNodeEndId(mdl, bindBegin);

        for (u32 i = bindBegin; i < bindEnd; i++) {
            mpBinding[i] = BINDING_UNDEFINED;
        }

        break;
    }

    case BIND_ONE: {
        mpBinding[target] = BINDING_UNDEFINED;
        break;
    }

    default: {
        break;
    }
    }
}

const ChrAnmResult* AnmObjChrRes::GetResult(ChrAnmResult* pResult, u32 idx) {
    u32 id = mpBinding[idx];

    if (id & (BINDING_UNDEFINED | BINDING_INVALID)) {
        pResult->flags = 0;
        return pResult;
    }

    if (mpResultCache != NULL) {
        return &mpResultCache[id];
    }

    mRes.GetAnmResult(pResult, id, GetFrm());
    return pResult;
}

void AnmObjChrRes::UpdateCache() {
    f32 frame = GetFrm();

    for (u32 i = 0; i < mNumBinding; i++) {
        u16 bind = mpBinding[i];

        if (!(bind & BINDING_UNDEFINED)) {
            u32 id = bind & BINDING_ID_MASK;
            mRes.GetAnmResult(&mpResultCache[id], id, frame);
        }
    }
}

void AnmObjChrRes::G3dProc(u32 task, u32 param, void* pInfo) {
#pragma unused(param)

    switch (task) {
    case G3DPROC_UPDATEFRAME: {
        UpdateFrame();
        break;
    }

    case G3DPROC_DETACH_PARENT: {
        SetParent(NULL);
        break;
    }

    case G3DPROC_ATTACH_PARENT: {
        SetParent(static_cast<G3dObj*>(pInfo));
        break;
    }
    }
}

} // namespace g3d
} // namespace nw4r
