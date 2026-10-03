#ifndef NW4R_SND_TYPES_H
#define NW4R_SND_TYPES_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {

static const int CHANNEL_MIN = 1;
static const int CHANNEL_MAX = 2;

static const int REMOTE_FILTER_MAX = 127;

// Volume in range [-90.4db, 6.0db]
static const f32 VOLUME_MIN_DB = -90.4f;
static const f32 VOLUME_MAX_DB = 6.0f;
static const f32 VOLUME_RANGE_DB = -(VOLUME_MIN_DB - VOLUME_MAX_DB);
static const int VOLUME_RANGE_MB = static_cast<int>(10 * VOLUME_RANGE_DB);

enum OutputLineFlag {
    OUTPUT_LINE_MAIN = (1 << 0),
    OUTPUT_LINE_REMOTE_N = (1 << 1),
};

enum AuxBus { AUX_A, AUX_B, AUX_C, AUX_BUS_NUM };

enum OutputMode {
    OUTPUT_MODE_STEREO,
    OUTPUT_MODE_SURROUND,
    OUTPUT_MODE_DPL2,
    OUTPUT_MODE_MONO
};

static const int VOICE_OUT_MAX = 4;

struct VoiceOutParam {
    f32 volume;
    f32 pitch;
    f32 pan;
    f32 surroundPan;
    f32 fxSend;
    f32 lpf;

    VoiceOutParam()
        : volume(1.0f),
          pitch(1.0f),
          pan(0.0f),
          surroundPan(0.0f),
          fxSend(0.0f),
          lpf(0.0f) {}
};

struct SoundParam {
    f32 volume;            // at 0x0
    f32 pitch;             // at 0x4
    f32 pan;               // at 0x8
    f32 surroundPan;       // at 0xC
    f32 fxSend;            // at 0x10
    f32 lpf;               // at 0x14
    f32 biquadFilterValue; // at 0x18
    int biquadFilterType;  // at 0x1C
    int priority;          // at 0x20
    u32 userData;          // at 0x24

    SoundParam()
        : volume(1.0f),
          pitch(1.0f),
          pan(0.0f),
          surroundPan(0.0f),
          fxSend(0.0f),
          lpf(0.0f),
          biquadFilterValue(0.0f),
          biquadFilterType(0),
          priority(0),
          userData(0) {}
};

struct SoundAmbientParam {
    f32 volume;                                 // at 0x0
    f32 pitch;                                  // at 0x4
    f32 pan;                                    // at 0x8
    f32 surroundPan;                            // at 0xC
    f32 fxSend;                                 // at 0x10
    f32 lpf;                                    // at 0x14
    f32 biquadFilterValue;                      // at 0x18
    int biquadFilterType;                       // at 0x1C
    int priority;                               // at 0x20
    u32 userData;                               // at 0x24
    VoiceOutParam voiceOutParam[VOICE_OUT_MAX]; // at 0x28

    SoundAmbientParam()
        : volume(1.0f),
          pitch(1.0f),
          pan(0.0f),
          surroundPan(0.0f),
          fxSend(0.0f),
          lpf(0.0f) {}
};

namespace detail {

struct SoundActorParam {
    f32 volume; // at 0x0
    f32 pitch;  // at 0x4
    f32 pan;    // at 0x8

    SoundActorParam() : volume(1.0f), pitch(1.0f), pan(0.0f) {}
};

enum PanMode {
    PAN_MODE_DUAL,
    PAN_MODE_BALANCE,
};

enum PanCurve {
    PAN_CURVE_SQRT,
    PAN_CURVE_SQRT_0DB,
    PAN_CURVE_SQRT_0DB_CLAMP,
    PAN_CURVE_SINCOS,
    PAN_CURVE_SINCOS_0DB,
    PAN_CURVE_SINCOS_0DB_CLAMP,
    PAN_CURVE_LINEAR,
    PAN_CURVE_LINEAR_0DB,
    PAN_CURVE_LINEAR_0DB_CLAMP,
};

enum BiquadFilterType {
    BIQUAD_FILTER_TYPE_NONE = 0,
    BIQUAD_FILTER_TYPE_LPF = 1,
    BIQUAD_FILTER_TYPE_HPF = 2,
    BIQUAD_FILTER_TYPE_BPF512 = 3,
    BIQUAD_FILTER_TYPE_BPF1024 = 4,
    BIQUAD_FILTER_TYPE_BPF2048 = 5
};

struct AdpcmParam {
    u16 coef[16];   // at 0x0
    u16 gain;       // at 0x20
    u16 pred_scale; // at 0x22
    u16 yn1;        // at 0x24
    u16 yn2;        // at 0x26
};

struct AdpcmLoopParam {
    u16 loop_pred_scale; // at 0x0
    u16 loop_yn1;        // at 0x2
    u16 loop_yn2;        // at 0x4
};

struct AdpcmInfo {
    AdpcmParam param;         // at 0x0
    AdpcmLoopParam loopParam; // at 0x28
    u16 PADDING_0x2E;         // at 0x2E
};

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
