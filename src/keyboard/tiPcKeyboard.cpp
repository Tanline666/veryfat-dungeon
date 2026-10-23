#include <keyboard/tiPcKeyboard.h>

namespace textinput {
namespace keyboard {
namespace pctype {

void Base::create(MEMAllocator* pAllocator) {
    mpAllocator = pAllocator;
}

void Base::init() {
    mCurrentState.outOfLength = FALSE;

    CommandReceiver::ChangeKanaMode ck;
    ck.kanaMode = mKeyState.getTranslateMode();
    this->sendCommand(CommandReceiver::CHANGE_KANA_MODE, &ck);
}

Base::TranslateMode Base::KeyState::getTranslateMode() const {
    switch (mABCKeyState & ABC_STAT_MASK) {
    case ABC_STAT_ASCII:
        return TM_DIRECT;
    case ABC_STAT_HIRAGANA:
        return TM_HIRAGANA;
    case ABC_STAT_KATAKANA:
        return TM_KATAKANA;
    default:
        return TM_DIRECT;
    }
}

} // namespace pctype
} // namespace keyboard
} // namespace textinput
