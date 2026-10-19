#ifndef NW4R_SND_BIQUAD_FILTER_PRESET_H
#define NW4R_SND_BIQUAD_FILTER_PRESET_H

#include <nw4r/snd/snd_BiquadFilterCallback.h>

namespace nw4r {
namespace snd {
namespace detail {

class BiquadFilterLpf : public BiquadFilterCallback {
public:
    virtual void GetCoef(int type, f32 val, BiquadCoef* pCoef) const;

private:
    static const int COEF_TABLE_SIZE = 112;
    static const BiquadCoef coefTable[COEF_TABLE_SIZE];
};

class BiquadFilterHpf : public BiquadFilterCallback {
public:
    virtual void GetCoef(int type, f32 val, BiquadCoef* pCoef) const;

private:
    static const int COEF_TABLE_SIZE = 97;
    static const BiquadCoef coefTable[COEF_TABLE_SIZE];
};

class BiquadFilterBpf512 : public BiquadFilterCallback {
public:
    virtual void GetCoef(int type, f32 val, BiquadCoef* pCoef) const;

private:
    static const int COEF_TABLE_SIZE = 122;
    static const BiquadCoef coefTable[COEF_TABLE_SIZE];
};

class BiquadFilterBpf1024 : public BiquadFilterCallback {
public:
    virtual void GetCoef(int type, f32 val, BiquadCoef* pCoef) const;

private:
    static const int COEF_TABLE_SIZE = 93;
    static const BiquadCoef coefTable[COEF_TABLE_SIZE];
};

class BiquadFilterBpf2048 : public BiquadFilterCallback {
public:
    virtual void GetCoef(int type, f32 val, BiquadCoef* pCoef) const;

private:
    static const int COEF_TABLE_SIZE = 93;
    static const BiquadCoef coefTable[COEF_TABLE_SIZE];
};

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
