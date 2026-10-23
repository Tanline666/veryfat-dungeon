#ifndef KEY_TI_TEXTINPUTBASE_H
#define KEY_TI_TEXTINPUTBASE_H

#include <nw4r/ut.h>

#include <revolution/MEM.h>
#include <revolution/WPAD.h>

namespace textinput {

#define TI_EXTCHAR_SPACE 0xe057
#define TI_EXTCHAR_RETURN 0xe056

enum Destination { DST_JP, DST_US, DST_EU, DST_CN, DST_KR, DST_MAX };

enum Language { JP, US, UK, FR, GE, IT, SP, EN, CN, KR, MAX };

class Base {
public:
    virtual ~Base();
    virtual void create(MEMAllocator*) {}
    virtual void init() {}
};

class CommandSender;
class CommandReceiver : public Base {
    friend class CommandSender;

public:
    enum INPUT_COMMAND {
        INPUT_CHAR,
        DELETE_CHAR,
        DELETE_FORWARD,
        INPUT_INPUTTING,
        CLEAR_INPUTTING,
        CONFIRM_INPUTTING,
        CONFIRM,
        LINEFEED,
        MOVE_CURSOR_LEFT,
        MOVE_CURSOR_RIGHT,
        MOVE_CURSOR_UP,
        MOVE_CURSOR_DOWN,
        SUSTAIN_ON,
        SUSTAIN_OFF,
        POINTED,
        RELEASED,
        MOVE_POINTER,
        CHANGE_INPUT_MODE,
        CHANGE_KANA_MODE,
        CHANGE_CHAR_INPUTTYPE,
        CHANGE_FIX_MODE,
        APPLY_CANDIDATE,
        SELECTING_CANDIDATE,
        DESELECTING_CANDIDATE,
        SCROLL,
        CONVERT_DAKUTEN,
        CONVERT_HANDAKU,
        CONVERT_ALL,
        CONVERT_SMALL,
        CHANGE_PREDICT_MODE,
        RESET_CANDIDATE,
        GET_PREDICT_MODE,
        IS_PREDICT_EXIST,
        AT_HEAD_OF_SENTENCE,
        INVALID_PREDICT,
        IS_INVALID_PREDICT,
        OUT_OF_LENGTH,
        EXTEND_COMMAND,
        INPUT_CTRL_KEY,
        RESET_DEAD_KEY,
        INPUT_SPACE,
        NOTIFY_CHANGE_MODE,
        MOVE_HWKB_CURSOR_LEFT,
        MOVE_HWKB_CURSOR_RIGHT,
        MOVE_HWKB_CURSOR_UP,
        MOVE_HWKB_CURSOR_DOWN,
        TAPPING_HWKBD_SHIFT,
        INPUT_COMMAND_MAX
    };

    //! \~english Typo
    //! \~japanese タイプミス
    struct ConfirmInputtingData {
        wchar_t inputChar; //!< at 0x0
        u16 letterMode;    //!< at 0x4
        bool isDirect;     //!< at 0x6
        bool isHead;       //!< at 0x7
        bool isCellPhone;  //!< at 0x8
        bool isSEOn;       //!< at 0x9
        void* pData;       //!< at 0xC
    };

    struct InputCharData {
        wchar_t inputChar;      //!< at 0x0
        u32 modifier;           //!< at 0x4
        bool useTransQwerty;    //!< at 0x8
        bool doConvertCombined; //!< at 0x9
        void* pData;            //!< at 0xC
    };

    enum { MOD_NULL, SHIFT_FLAG, CAPS_FLAG };

    struct Pointed {
        f32 x; //!< at 0x0
        f32 y; //!< at 0x4
    };

    struct ApplyCandidate {
        u32 candidate; //!< at 0x0
    };

    struct ChangeInputMode {
        bool toDirect; //!< at 0x0
    };

    struct ChangeKanaMode {
        enum TranslateMode { TM_DIRECT, TM_HIRAGANA, TM_KATAKANA, TM_MAX };
        u32 kanaMode; //!< at 0x0
    };

public:
    CommandReceiver() {
        this->clearSender();
    }

    virtual void clearSender();
    virtual void onCommand(INPUT_COMMAND command, void* pData);

private:
    virtual void addSender(CommandSender* pSender) {
        nw4r::ut::List_Append(&mSenderList, pSender);
    }

public:
    nw4r::ut::List mSenderList; //!< at 0x4
};

class CommandSender : public Base {
public:
    CommandSender(CommandReceiver* pCommandReceiver = NULL)
        : mpCommandReceiver(pCommandReceiver) {}

    virtual void setCommandReceiver(CommandReceiver* pCommandReceiver);
    CommandReceiver* getCommandReceiver() {
        return mpCommandReceiver;
    }

    virtual void sendCommand(u32 command, void* pData) {
        if (mpCommandReceiver) {
            mpCommandReceiver->onCommand(
                CommandReceiver::INPUT_COMMAND(command), pData);
        }
    }

    virtual void updateFromReceiver(u32 command, void* pData);

public:
    NW4R_UT_LIST_LINK_DECL(); //!< at 0x4

private:
    CommandReceiver* mpCommandReceiver; //!< at 0xC
};

} // namespace textinput

#endif
