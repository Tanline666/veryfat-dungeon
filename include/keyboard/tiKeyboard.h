#ifndef KEY_TI_KEYBOARD_H
#define KEY_TI_KEYBOARD_H

#include <keyboard/tiTextInputBase.h>

namespace textinput {

namespace keyboard {

enum VKeyCode {
    VKC_ENTER,
    VKC_BACK,
    VKC_SPACE,
    VKC_CAPS,
    VKC_SHIFT,
    VKC_LEFT,
    VKC_UP,
    VKC_RIGHT,
    VKC_DOWN,
    VKC_MARK,
    VKC_PREDICT_LANGUAGE,
    VKC_KEYCHANGE_ASCII,
    VKC_KEYCHANGE_KANA,
    VKC_KEYCHANGE_NOTATION,
    VKC_KEYCHANGE_EUROPE,
    VKC_IMEMODE_DIRECT,
    VKC_IMEMODE_ROMAJI_HIRAKANA,
    VKC_IMEMODE_ROMAJI_KATAKANA,
    VKC_KANACHANGE_HIRAKANA,
    VKC_KANACHANGE_KATAKANA,
    VKC_CONVERT_DAKUTEN,
    VKC_CONVERT_HANDAKUTEN,
    VKC_CONVERT_SMALL,
    VKC_CHANGE_CASE,
    VKC_SIGN_CLOSE,
    VKC_SIGN_NEXT,
    VKC_SIGN_PREV,
    VKC_LAST
};

class KeyboardBase : public textinput::CommandSender {
public:
    enum KeyboardType { TYPE_QWERTY, TYPE_CELLPHONE, TYPE_SIGN, TYPE_MAX };

public:
    KeyboardBase() : mLanguage(JP) {}
    /**
     * \~english @brief Destructor
     * \~japanese @brief デストラクタ
     */
    virtual ~KeyboardBase() {}
    virtual void onKey(u32 event, void* pData) = 0;

    virtual KeyboardType getType() = 0;

    virtual void setLanguage(Language language) {
        mLanguage = language;
    }
    virtual Language getLanguage() const {
        return mLanguage;
    };

    virtual void update() {}
    virtual void onActive() {}

protected:
    Language mLanguage; //!< at 0x10
};

} // namespace keyboard
} // namespace textinput

#endif
