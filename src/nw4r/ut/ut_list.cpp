#include <nw4r/ut.h>

namespace nw4r {
namespace ut {

void List_Init(List* list, u16 offset) {
    list->headObject = NULL;
    list->tailObject = NULL;
    list->numObjects = 0;
    list->offset = offset;
}

static void SetFirstObject(List* list, void* object) {
    Link* pLink = NW4R_UT_LIST_GET_LINK(*list, object);
    pLink->nextObject = NULL;
    pLink->prevObject = NULL;

    list->headObject = object;
    list->tailObject = object;
    list->numObjects++;
}

void List_Append(List* list, void* object) {
    if (list->headObject == NULL) {
        SetFirstObject(list, object);
        return;
    }

    // Old tail <- New tail relationship
    Link* pLink = NW4R_UT_LIST_GET_LINK(*list, object);
    pLink->prevObject = list->tailObject;
    pLink->nextObject = NULL;

    // Old tail -> New tail relationship
    NW4R_UT_LIST_GET_LINK(*list, list->tailObject)->nextObject = object;
    list->tailObject = object;

    list->numObjects++;
}

//! Unused in Wii Fit Plus. Kept for reference
void List_Prepend(List* list, void* object) {
    if (list->headObject == NULL) {
        SetFirstObject(list, object);
        return;
    }

    // New head -> Old head relationship
    Link* pLink = NW4R_UT_LIST_GET_LINK(*list, object);
    pLink->prevObject = NULL;
    pLink->nextObject = list->headObject;

    // New head <- Old head relationship
    NW4R_UT_LIST_GET_LINK(*list, list->headObject)->prevObject = object;
    list->headObject = object;

    list->numObjects++;
}

void List_Insert(List* list, void* pTarget, void* object) {
    if (pTarget == NULL) {
        List_Append(list, object);
        return;
    }

    if (pTarget == list->headObject) {
        List_Prepend(list, object);
        return;
    }

    Link* pLink = NW4R_UT_LIST_GET_LINK(*list, object);
    void* pPrev = NW4R_UT_LIST_GET_LINK(*list, pTarget)->prevObject;
    Link* pPrevLink = NW4R_UT_LIST_GET_LINK(*list, pPrev);

    // pPrev <- object
    pLink->prevObject = pPrev;
    // pPrev <- object -> pTarget
    pLink->nextObject = pTarget;
    // pPrev <-> object -> pTarget
    pPrevLink->nextObject = object;
    // pPrev <-> object <-> pTarget
    NW4R_UT_LIST_GET_LINK(*list, pTarget)->prevObject = object;

    list->numObjects++;
}

void List_Remove(List* list, void* object) {
    NW4R_NULL_ASSERT(list, 206);
    NW4R_NULL_ASSERT(object, 207);

    Link* pLink = NW4R_UT_LIST_GET_LINK(*list, object);

    // Fix previous node relationship
    if (pLink->prevObject == NULL) {
        list->headObject = pLink->nextObject;
    } else {
        NW4R_UT_LIST_GET_LINK(*list, pLink->prevObject)->nextObject =
            pLink->nextObject;
    }

    // Fix next node relationship
    if (pLink->nextObject == NULL) {
        list->tailObject = pLink->prevObject;
    } else {
        NW4R_UT_LIST_GET_LINK(*list, pLink->nextObject)->prevObject =
            pLink->prevObject;
    }

    pLink->prevObject = NULL;
    pLink->nextObject = NULL;

    list->numObjects--;
}

void* List_GetNext(const List* list, const void* object) {
    if (object == NULL) {
        return list->headObject;
    }

    return NW4R_UT_LIST_GET_LINK(*list, object)->nextObject;
}

void* List_GetPrev(const List* list, const void* object) {
    if (object == NULL) {
        return list->tailObject;
    }

    return NW4R_UT_LIST_GET_LINK(*list, object)->prevObject;
}

void* List_GetNth(const List* list, u16 n) {
    int i;
    void* pIt = NULL;

    for (i = 0, pIt = NULL; (pIt = List_GetNext(list, pIt)) != NULL; i++) {
        if (n == i) {
            return pIt;
        }
    }

    return NULL;
}

} // namespace ut
} // namespace nw4r
