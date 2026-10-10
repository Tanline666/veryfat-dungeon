#include <nw4r/ut.h>

namespace nw4r {
namespace ut {

NW4R_UT_RTTI_DEF_BASE(IOStream);

//! The below functions are stubbed as the I/O stream does not support
//! reading and writing data to it. (texline)
bool IOStream::ReadAsync(void* pDst, u32 size, StreamCallback pCallback,
                         void* pCallbackArg) {
#pragma unused(pDst)
#pragma unused(size)
#pragma unused(pCallback)
#pragma unused(pCallbackArg)
    return FALSE;
}

s32 IOStream::Write(const void* pSrc, u32 size) {
#pragma unused(pSrc)
#pragma unused(size)
    return 0;
}

bool IOStream::WriteAsync(const void* pSrc, u32 size, StreamCallback pCallback,
                          void* pCallbackArg) {
#pragma unused(pSrc)
#pragma unused(size)
#pragma unused(pCallback)
#pragma unused(pCallbackArg)
    return FALSE;
}

bool IOStream::IsBusy() const {
    return FALSE;
}

} // namespace ut
} // namespace nw4r
