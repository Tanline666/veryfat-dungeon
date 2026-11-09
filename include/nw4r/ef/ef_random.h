#ifndef NW4R_EF_RANDOM_H
#define NW4R_EF_RANDOM_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ef {

class Random {
private:
    u32 mSeed; // at 0x0

public:
    void Srand(u32 seed) {
        mSeed = seed;
    }

    int Rand() {
        MixRandomSeed();
        return static_cast<int>(mSeed >> 16);
    }

    /**
     * \~english @brief Obtains a random number between 0.0 and 1.0.
     * \~japanese @brief 0.0と1.0の間からの乱数を得る。
     * \~english @note 0.0 is included, while 1.0 is excluded.
     * \~japanese @note 0.0は含まれ、1.0は含まれない。
     */
    f32 RandFloat() {
        MixRandomSeed();
        return static_cast<f32>(mSeed >> 16) / 0x10000;
    }

private:
    void MixRandomSeed() {
        mSeed = mSeed * 0x343FD + 0x269EC3;
    }
};

inline u32 RANDOM_INIT(u16 objectSeed, u16 keySeed, u16 keyIdx, u32 keyDiv) {
    union {
        u32 whole;
        u8 part[4];
    } result;

    result.whole = objectSeed * 0x3F81F635 + keySeed * 0x30A74193 +
                   keyIdx * 0x371097E7 + keyDiv * 0x7B929 + 0x4BF53;

    result.part[2] ^= result.part[3];
    result.part[1] ^= result.part[2];
    result.part[0] ^= result.part[1];

    return result.whole;
}

inline u32 RANDOM_NEXT(u32 prev) {
    return prev * 214013 + 2531011;
}

} // namespace ef
} // namespace nw4r

#endif
