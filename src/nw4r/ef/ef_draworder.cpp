#include <nw4r/ef.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace ef {

ut::List* DrawOrderBase::GetPMList(Effect* ef) {
    NW4R_POINTER_ASSERT(ef, 27);

    return &ef->mParticleManager;
}

void DrawOrder::Draw(Effect* ef, const DrawInfo& drawInfo) {
    NW4R_POINTER_ASSERT(ef, 35);

    ut::List* pList = GetPMList(ef);

    NW4R_UT_LIST_FOREACH (ParticleManager, it, *pList, {
        it->BeginDraw();
        it->Draw(drawInfo);
        it->EndDraw();
    })
}

void DrawOrder::Add(Effect* ef, ParticleManager* pm) {
    NW4R_POINTER_ASSERT(ef, 49);
    NW4R_POINTER_ASSERT(pm, 50);

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

void DrawOrder::Remove(Effect* ef, ParticleManager* pm) {
    NW4R_POINTER_ASSERT(ef, 70);
    NW4R_POINTER_ASSERT(pm, 71);

    ut::List* pmlist = GetPMList(ef);
    ut::List_Remove(pmlist, pm);
}

} // namespace ef
} // namespace nw4r
