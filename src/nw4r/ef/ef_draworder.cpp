#include <nw4r/ef.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace ef {

ut::List* DrawOrderBase::GetPMList(Effect* pEffect) {
    return &pEffect->mParticleManager;
}

void DrawOrder::Draw(Effect* pEffect, const DrawInfo& rInfo) {
    ut::List* pList = GetPMList(pEffect);

    NW4R_UT_LIST_FOREACH (ParticleManager, it, *pList, {
        it->BeginDraw();
        it->Draw(rInfo);
        it->EndDraw();
    })
}

void DrawOrder::Add(Effect* ef, ParticleManager* pm) {
    NW4R_POINTER_ASSERT(ef, 49);
    NW4R_POINTER_ASSERT(ef, 50);

    ut::List* pList = GetPMList(ef);
    ParticleManager* pIt = NULL;

    while (
        (pIt = static_cast<ParticleManager*>(ut::List_GetPrev(pList, pIt)))) {

        if (pIt->mWeight <= pm->mWeight) {
            break;
        }
    }

    pIt = static_cast<ParticleManager*>(ut::List_GetNext(pList, pIt));
    ut::List_Insert(pList, pIt, pm);
}

void DrawOrder::Remove(Effect* pEffect, ParticleManager* pManager) {
    ut::List* pList = GetPMList(pEffect);
    ut::List_Remove(pList, pManager);
}

} // namespace ef
} // namespace nw4r
