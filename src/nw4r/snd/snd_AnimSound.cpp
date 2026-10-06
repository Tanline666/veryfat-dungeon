#include <nw4r/snd.h>

namespace nw4r {
namespace snd {

//! None of the functions in AnimSound are used in the Pack Project
//! (though Wii Music may be an exception). All this file provides
//! is the destructor for SoundHandle. Why in the world was it not
//! just in the SoundHandle file then??? (texline)
DECOMP_FORCEACTIVE_DTOR(snd_AnimSound_cpp, SoundHandle);

} // namespace snd
} // namespace nw4r
