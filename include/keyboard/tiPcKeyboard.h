#ifndef KEY_TI_PC_KEYBOARD_H
#define KEY_TI_PC_KEYBOARD_H

#include <keyboard/tiKeyboard.h>

namespace textinput {

namespace keyboard {

namespace pctype {

struct PaneNameToControlKey {
    char szPaneName[20];
};

struct PaneNameToCharCodeWithModeAscii {
    const char* pSzPaneName; //!< at 0x0
    wchar_t wChar[4];        //!< at 0x4
};

struct PaneNameToCharCodeWithModeGrid {
    char szPaneName[17]; //!< at 0x0
    wchar_t wChar[4];    //!< at 0x14
};

struct LanguageDependency {
    PaneNameToCharCodeWithModeAscii const* pPNToCCTableAscii; //!< at 0x0
    PaneNameToCharCodeWithModeGrid const* pPNToCCTableGrid;   //!< at 0x4
};

class Base : public KeyboardBase {
public:
    enum TranslateMode { TM_DIRECT, TM_HIRAGANA, TM_KATAKANA, TM_MAX };

public:
    struct State {
        bool outOfLength;                   //!< at 0x0
        u32 letterMode;                     //!< at 0x4
        LanguageDependency const* pLangDep; //!< at 0x8
    };

    class KeyState {
    public:
        enum {
            ABC_STAT_ASCII = 0,

            ABC_STAT_HIRAGANA = 1 << 0,
            ABC_STAT_KATAKANA = 1 << 1,

            ABC_STAT_MASK = (1 << 4) - 1,

            ABC_STAT_CAPS_LOCK = 1 << 6,
            ABC_STAT_SHIFT_KEY = 1 << 7,
        };

    public:
        void init();
        TranslateMode getTranslateMode() const;

    private:
        void refresh_();

    private:
        u32 mABCKeyState;                               //!< at 0x0
        u32 mMode;                                      //!< at 0x4
        u32 mAIUKeyState;                               //!< at 0x8
        Language mLanguage;                             //!< at 0xC
        const LanguageDependency* mpLanguageDependency; //!< at 0x10
        Base* mpParent;                                 //!< at 0x14
    };

public:
    virtual void create(MEMAllocator* pAllocator);
    virtual void init();

    virtual TranslateMode getTranslateMode() const {
        return mKeyState.getTranslateMode();
    }

protected:
    State mCurrentState;       //!< at 0x14
    KeyState mKeyState;        //!< at 0x20
    MEMAllocator* mpAllocator; //!< at 0x38

private:
    bool mIsOnlyQwerty;    //!< at 0x3C
    bool mIsLangKeyActive; //!< at 0x3D
    // textinput::Manager* mpManager
};

} // namespace pctype

} // namespace keyboard

} // namespace textinput

#endif
