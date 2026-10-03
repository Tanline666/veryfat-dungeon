#ifndef NW4R_EF_UTIL_H
#define NW4R_EF_UTIL_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut.h>

namespace nw4r {
namespace ef {

u16 UtlistToArray(const ut::List* pList, void** ppArray, int maxlen);

const math::VEC3& Rotation2VecY(const math::VEC3& rRot,
                                register math::VEC3* pVec);
void GetDirMtxY(math::MTX34* pMtx, const math::VEC3& rVec);

void MtxGetRotationMtx(const math::MTX34& rMtx, math::MTX34* pDst);

void MtxSetColVec(math::MTX34* pMtx, int col, const math::VEC3* pVec);
math::VEC3* MtxColVec(const math::MTX34* pMtx, int col, math::VEC3* pVec);

void MtxGetRotation(const math::MTX34& rMtx, math::VEC3* pRot);
void MtxGetTranslate(const math::MTX34& rMtx, math::VEC3* pTrans);
void MtxGetScale(const math::MTX34& rMtx, math::VEC3* pScale);

bool Normalize(register math::VEC3* pDst, const register math::VEC3* pVec);

f32 MTXColLen(const register math::MTX34* pSrc, register int col);

} // namespace ef
} // namespace nw4r

#endif
