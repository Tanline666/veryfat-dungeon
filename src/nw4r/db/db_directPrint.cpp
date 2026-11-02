#include <nw4r/db.h>

#include <revolution/VI.h>

#include <cstring>

namespace nw4r {
namespace db {

static FrameBufferInfo sFrameBufferInfo;
static YUVColorInfo sFrameBufferColor;
static BOOL sInitialized = FALSE;

static const u8 sAsciiTable[128] = {
    122, 122, 122, 122, 122, 122, 122, 122, 122, 253, 254, 122, 122, 122, 122,
    122, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 41,  100, 101, 102, 43,  103, 104, 37,  38,  105, 42,  106,
    39,  44,  107, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   36,  108,
    109, 110, 111, 40,  112, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
    20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,
    35,  113, 114, 115, 116, 117, 255, 125, 126, 127, 128, 129, 130, 131, 132,
    133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147,
    148, 149, 150, 118, 119, 120, 121, 122,
};

static const u32 sFontData[64] = {
    0x70871C30, 0x8988A250, 0x88808290, 0x88830C90, 0x888402F8, 0x88882210,
    0x71CF9C10, 0xF9CF9C70, 0x8208A288, 0xF200A288, 0x0BC11C78, 0x0A222208,
    0x8A222208, 0x71C21C70, 0x23C738F8, 0x5228A480, 0x8A282280, 0x8BC822F0,
    0xFA282280, 0x8A28A480, 0x8BC738F8, 0xF9C89C08, 0x82288808, 0x82088808,
    0xF2EF8808, 0x82288888, 0x82288888, 0x81C89C70, 0x8A08A270, 0x920DA288,
    0xA20AB288, 0xC20AAA88, 0xA208A688, 0x9208A288, 0x8BE8A270, 0xF1CF1CF8,
    0x8A28A220, 0x8A28A020, 0xF22F1C20, 0x82AA0220, 0x82492220, 0x81A89C20,
    0x8A28A288, 0x8A28A288, 0x8A289488, 0x8A2A8850, 0x894A9420, 0x894AA220,
    0x70852220, 0xF8011000, 0x08020800, 0x10840400, 0x20040470, 0x40840400,
    0x80020800, 0xF8011000, 0x70800000, 0x88822200, 0x08820400, 0x108F8800,
    0x20821000, 0x00022200, 0x20800020, 0x00000000,
};

static const u32 sFontData2[77] = {
    0x51421820, 0x53e7a420, 0x014a2c40, 0x01471000, 0x0142aa00, 0x03eaa400,
    0x01471a78, 0x00000000, 0x50008010, 0x20010820, 0xf8020040, 0x20420820,
    0x50441010, 0x00880000, 0x00070e00, 0x01088840, 0x78898820, 0x004a8810,
    0x788a8810, 0x01098808, 0x00040e04, 0x70800620, 0x11400820, 0x12200820,
    0x10001020, 0x10000820, 0x100f8820, 0x70000620, 0x60070000, 0x110f82a0,
    0x12aa8ae0, 0x084f92a0, 0x100fbe1c, 0x10089008, 0x60070808, 0x00000000,
    0x02000200, 0x7a078270, 0x8bc81e88, 0x8a2822f8, 0x9a282280, 0x6bc79e78,
    0x30000000, 0x48080810, 0x41e80000, 0x422f1830, 0xfbe88810, 0x40288890,
    0x43c89c60, 0x81000000, 0x81000000, 0x990f3c70, 0xa10aa288, 0xe10aa288,
    0xa10aa288, 0x98caa270, 0x00000000, 0x00000020, 0xf1ef1e20, 0x8a28a0f8,
    0x8a281c20, 0xf1e80220, 0x80283c38, 0x00000000, 0x00000000, 0x8a28b688,
    0x8a2a8888, 0x8a2a8878, 0x894a8808, 0x788536f0, 0x00000000, 0x00000000,
    0xf8000000, 0x10000000, 0x20000000, 0x40000000, 0xf8000000,
};

static const char* DrawStringLineToXfb_(int posh, int posv, const char* str,
                                        int width);
static void DrawStringToXfb_(int posh, int posv, const char* str, bool turnOver,
                             bool backErase);
static void DrawCharToXfb_(int posh, int posv, int code);

static inline int GetDotWidth_() {
    return (sFrameBufferInfo.frameWidth < 400) ? 1 : 2;
}

static inline int GetDotHeight_() {
    return (sFrameBufferInfo.frameHeight < 300) ? 1 : 2;
}

static inline int StrLineWidth_(const char* str) {
    int len = 0;
    char c;

    NW4R_NULL_ASSERT(str, 304);

    while (TRUE) {
        c = *str++;

        if (c == '\0' || c == '\n') {
            return len;
        }

        if (c == '\t') {
            len = (len + 4) & ~(3);
        } else {
            ++len;
        }
    }
}

void DirectPrint_Init() {
    if (!sInitialized) {
        DirectPrint_ChangeXfb(NULL, 640, 480);
        DirectPrint_SetColor(255, 255, 255);
        sInitialized = TRUE;
    }
}

bool DirectPrint_IsActive() {
    return sInitialized && sFrameBufferInfo.frameMemory;
}

void DirectPrint_EraseXfb(int posh, int posv, int sizeh, int sizev) {
    int posEndH, posEndV;

    if (sFrameBufferInfo.frameMemory) {

        if (GetDotWidth_() == 2) {
            posh *= 2;
            sizeh *= 2;
        }
        posEndH = posh + sizeh;
        posh = (posh >= 0) ? posh : 0;
        posEndH = (posEndH <= sFrameBufferInfo.frameWidth)
                      ? posEndH
                      : sFrameBufferInfo.frameWidth;
        sizeh = posEndH - posh;

        if (GetDotHeight_() == 2) {
            posv *= 2;
            sizev *= 2;
        }
        posEndV = posv + sizev;
        posv = (posv >= 0) ? posv : 0;
        posEndV = (posEndV <= sFrameBufferInfo.frameHeight)
                      ? posEndV
                      : sFrameBufferInfo.frameHeight;
        sizev = posEndV - posv;

        u16* pixel = (u16*)sFrameBufferInfo.frameMemory +
                     sFrameBufferInfo.frameRow * posv + posh;
        for (int cntv = 0; cntv < sizev; cntv++) {
            for (int cnth = 0; cnth < sizeh; cnth++) {
                *pixel++ = 4224;
            }
            pixel += sFrameBufferInfo.frameRow - sizeh;
        }
    }
}

void DirectPrint_ChangeXfb(void* framebuf, u16 width, u16 height) {
    sFrameBufferInfo.frameMemory = (u8*)framebuf;
    sFrameBufferInfo.frameWidth = width;
    sFrameBufferInfo.frameHeight = height;
    sFrameBufferInfo.frameRow = VI_PAD_FB_WIDTH(width);
    sFrameBufferInfo.frameSize =
        sFrameBufferInfo.frameRow * sFrameBufferInfo.frameHeight * sizeof(u16);
}

void DirectPrint_ChangeXfb(void* framebuf) {
    sFrameBufferInfo.frameMemory = (u8*)framebuf;
}

void DirectPrint_StoreCache() {
    DCStoreRange(sFrameBufferInfo.frameMemory, sFrameBufferInfo.frameSize);
}

void DirectPrint_DrawString(int posh, int posv, const char* format, ...) {
    if (sFrameBufferInfo.frameMemory) {
        std::va_list vargs;

        va_start(vargs, format);
        detail::DirectPrint_DrawStringToXfb(posh, posv, format, vargs, TRUE,
                                            FALSE);
        va_end(vargs);
    }
}

void DirectPrint_DrawString(int posh, int posv, bool turnOver,
                            const char* format, ...) {
    if (sFrameBufferInfo.frameMemory) {
        std::va_list vargs;

        va_start(vargs, format);
        detail::DirectPrint_DrawStringToXfb(posh, posv, format, vargs, turnOver,
                                            FALSE);
        va_end(vargs);
    }
}

void DirectPrint_SetColor(u8 r, u8 g, u8 b) {
    int y = 0.257f * r + 0.504f * g + 0.098f * b + 16.0f;
    int u = -0.148f * r - 0.291f * g + 0.439f * b + 128.0f;
    int v = 0.439f * r - 0.368f * g - 0.071f * b + 128.0f;

    sFrameBufferColor.colorRGBA.r = r;
    sFrameBufferColor.colorRGBA.g = g;
    sFrameBufferColor.colorRGBA.b = b;
    sFrameBufferColor.colorRGBA.a = 255;

    sFrameBufferColor.colorY256 = y << 8;

    sFrameBufferColor.colorU = u;
    sFrameBufferColor.colorU2 = u / 2;
    sFrameBufferColor.colorU4 = u / 4;

    sFrameBufferColor.colorV = v;
    sFrameBufferColor.colorV2 = v / 2;
    sFrameBufferColor.colorV4 = v / 4;
}

namespace detail {

void DirectPrint_DrawStringToXfb(int posh, int posv, const char* format,
                                 std::va_list vargs, bool turnOver,
                                 bool backErase) {
    NW4R_ASSERT(sFrameBufferInfo.frameMemory != NULL, 645);

    char string[256];
    int length = std::vsnprintf(string, 256, format, vargs);
    int posLeftStart = posh;
    va_end(vargs);

    if (length > 0) {
        DrawStringToXfb_(posh, posv, string, turnOver, backErase);
    }
}

} // namespace detail

static void DrawStringToXfb_(int posh, int posv, const char* str, bool turnOver,
                             bool backErase) {
    int basePosH = posh;
    int width;
    int frameWidth = sFrameBufferInfo.frameWidth / GetDotWidth_();

    while (*str != '\0') {
        if (backErase) {
            int len = StrLineWidth_(str);
            DirectPrint_EraseXfb(posh - 6, posv - 3, (len + 2) * 6, 13);
        }

        width = (frameWidth - posh) / 6;
        str = DrawStringLineToXfb_(posh, posv, str, width);
        posv += 10;
        if (*str == '\n') {
            ++str;
            posh = basePosH;
        } else if (*str != '\0') {
            ++str;
            if (!turnOver) {
                if (!(str = std::strchr(str, '\n'))) {
                    return;
                }
                ++str;
                posh = basePosH;
            } else {
                posh = 0;
            }
        }
    }
}

static const char* DrawStringLineToXfb_(int posh, int posv, const char* str,
                                        int width) {
    char c;
    int code;
    int cnt = 0;

    NW4R_NULL_ASSERT(str, 743);
    NW4R_ASSERT(width > 0, 744);

    while ((c = *str) != '\0') {
        if (c == '\n' || c == '\0') {
            return str;
        }

        code = sAsciiTable[c & 0x7F];
        if (code == 253) {
            int tab_size = 4 - (cnt & 3);
            posh += 6 * tab_size;
            cnt += tab_size;
        } else {
            if (code != 255) {
                DrawCharToXfb_(posh, posv, code);
            }
            posh += 6;
            ++cnt;
        }

        if (cnt >= width) {
            if (*(str + 1) == '\n') {
                ++str;
            }
            return str;
        }
        ++str;
    }
    return str;
}

static void DrawCharToXfb_(int posh, int posv, int code) {
    int ncode = (code >= 100) ? (code - 100) : code;
    int fonth = (ncode % 5) * 6;
    int fontv = (ncode / 5) * 7;

    const u32* fontLine =
        (code < 100) ? (sFontData + fontv) : (sFontData2 + fontv);
    static u32 twiceBit[] = {0, 3, 12, 15};

    int wH = GetDotWidth_();
    int wV = GetDotHeight_();

    u16* pixel = (u16*)sFrameBufferInfo.frameMemory +
                 sFrameBufferInfo.frameRow * posv * wV + posh * wH;

    if (posv < 0 || posh < 0) {
        return;
    }

    if (sFrameBufferInfo.frameWidth <= wH * (posh + 6) ||
        sFrameBufferInfo.frameHeight <= wV * (posv + 7)) {
        return;
    }

    for (int cntv = 0; cntv < 7; cntv++) {
        u32 fontBits = (*fontLine++) << fonth;
        if (wH == 1) {
            fontBits = (fontBits & 0xFC000000) >> 1;
        } else {
            fontBits = (twiceBit[(fontBits >> 26) & 3] |
                        (twiceBit[(fontBits >> 28) & 3] << 4) |
                        (twiceBit[(fontBits >> 30)] << 8))
                       << 19;
        }

        for (int cnth = 0; cnth < 6 * wH; cnth += 2) {
            u16 pixColor;

            //! \~english Get the even bits...
            //! \~japanese 偶数ビットを取得し・・・
            pixColor =
                (u16)(((fontBits & 0x40000000) ? sFrameBufferColor.colorY256
                                               : 0) |
                      (((fontBits & 0x80000000) ? sFrameBufferColor.colorU4
                                                : 32) +
                       ((fontBits & 0x40000000) ? sFrameBufferColor.colorU2
                                                : 64) +
                       ((fontBits & 0x20000000) ? sFrameBufferColor.colorU4
                                                : 32)));
            *(pixel) = pixColor;
            if (wV > 1) {
                *(pixel + sFrameBufferInfo.frameRow) = pixColor;
            }
            pixel++;

            //! \~english Then the odd ones.
            //! \~japanese 奇数ビットも取得します。
            pixColor =
                (u16)(((fontBits & 0x20000000) ? sFrameBufferColor.colorY256
                                               : 0) |
                      (((fontBits & 0x40000000) ? sFrameBufferColor.colorV4
                                                : 32) +
                       ((fontBits & 0x20000000) ? sFrameBufferColor.colorV2
                                                : 64) +
                       ((fontBits & 0x10000000) ? sFrameBufferColor.colorV4
                                                : 32)));
            *(pixel) = pixColor;
            if (wV > 1) {
                *(pixel + sFrameBufferInfo.frameRow) = pixColor;
            }
            pixel++;

            fontBits <<= 2;
        }
        pixel += sFrameBufferInfo.frameRow * wV - 6 * wH;
    }
}

namespace detail {

static void WaitVIRetrace_() {
    BOOL enabled = OSEnableInterrupts();
    u32 preCnt;

    preCnt = VIGetRetraceCount();
    while (preCnt == VIGetRetraceCount()) {
    }

    OSRestoreInterrupts(enabled);
}

static void* CreateFB_(const GXRenderModeObj* rmode) {
    u32 arenaHi = (u32)OSGetArenaHi();
    u32 memSize = (u32)VI_PAD_FB_WIDTH(rmode->fbWidth) * rmode->xfbHeight * 2;
    u32 frameBuf = ROUND_DOWN(arenaHi - memSize, 32);

    VIConfigure(rmode);
    VISetNextFrameBuffer((void*)frameBuf);
    return (void*)frameBuf;
}

void* DirectPrint_SetupFB(const GXRenderModeObj* rmode) {
    void* frameMemory;
    DirectPrint_Init();

    frameMemory = VIGetCurrentFrameBuffer();
    if (!frameMemory) {
        if (rmode == NULL) {
            //! \~english @bug Undefined behaviour when TV format is DEBUG
            //! \~japanese @bug テレビフォーマットはDEBUGの場合には未定義の動作
            switch (u32 tv = VIGetTvFormat()) {
            case VI_TVFORMAT_NTSC:
                rmode = &GXNtsc480IntDf;
                break;
            case VI_TVFORMAT_PAL:
                rmode = &GXPal528IntDf;
                break;
            case VI_TVFORMAT_EURGB60:
                rmode = &GXEurgb60Hz480IntDf;
                break;
            case VI_TVFORMAT_MPAL:
                rmode = &GXMpal480IntDf;
                break;
            }
        }
        frameMemory = CreateFB_(rmode);
    }
    VISetBlack(FALSE);
    VIFlush();
    WaitVIRetrace_();
    if (rmode) {
        DirectPrint_ChangeXfb(frameMemory, rmode->fbWidth, rmode->xfbHeight);
    } else {
        DirectPrint_ChangeXfb(frameMemory);
    }
    return frameMemory;
}

} // namespace detail
} // namespace db
} // namespace nw4r
