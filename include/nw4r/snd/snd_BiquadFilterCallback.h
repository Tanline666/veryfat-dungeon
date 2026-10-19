#ifndef NW4R_SND_BIQUAD_FILTER_CALLBACK_H
#define NW4R_SND_BIQUAD_FILTER_CALLBACK_H

#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {

class BiquadFilterCallback {
public:
    struct BiquadCoef {
        u16 b0; // at 0x0
        u16 b1; // at 0x2
        u16 b2; // at 0x4
        u16 a1; // at 0x6
        u16 a2; // at 0x8
    };

    virtual ~BiquadFilterCallback() {} // at 0x8
    virtual void GetCoef(int type, f32 value,
                         BiquadCoef* pCoef) const = 0; // at 0xC
};

} // namespace snd
} // namespace nw4r

#endif
