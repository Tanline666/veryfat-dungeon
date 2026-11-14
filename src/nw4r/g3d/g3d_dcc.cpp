#include <nw4r/g3d.h>
#include <nw4r/math.h>

namespace nw4r {
namespace g3d {

void CalcTexMtx(math::MTX34* pM, bool bSet, const TexSrt& srt,
                TexSrt::Flag flag, TexSrtTypedef::TexMatrixMode mode) {
    NW4R_POINTER_ASSERT(pM, 35);
    bool bIdentity = true;

    if (mode == TexSrtTypedef::TEXMATRIXMODE_MAYA) {
        bIdentity = !detail::dcc::CalcTexMtx_Maya(pM, bSet, srt, flag);
    } else if (mode == TexSrtTypedef::TEXMATRIXMODE_XSI) {
        bIdentity = !detail::dcc::CalcTexMtx_Xsi(pM, bSet, srt, flag);
    } else /* TEXMATRIXMODE_3DSMAX */ {
        bIdentity = !detail::dcc::CalcTexMtx_3dsmax(pM, bSet, srt, flag);
    }

    if (bIdentity && bSet) {
        math::MTX34Identity(pM);
    }
}

void CalcTexMtx(math::MTX34* pM, bool bSet, const TexSrt& srt,
                TexSrt::Flag flag) {
    NW4R_POINTER_ASSERT(pM, 76);
    bool bIdentity = TRUE;

    bIdentity = !detail::dcc::CalcTexMtx_Basic(pM, bSet, srt, flag);
    if (bIdentity && bSet) {
        math::MTX34Identity(pM);
    }
}

} // namespace g3d
} // namespace nw4r
