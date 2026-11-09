#include <nw4r/lyt.h>

#include <revolution/OS.h>

namespace {

NW4R_LIB_VERSION(LYT, "Jun 25 2009", "15:01:26", "0x4302_145");

} // namespace

namespace nw4r {
namespace lyt {

void LytInit() {
    OSRegisterVersion(NW4R_LYT_Version_);
    OSInitFastCast();
}

} // namespace lyt
} // namespace nw4r
