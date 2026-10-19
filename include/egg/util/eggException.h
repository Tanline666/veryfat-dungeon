#ifndef EGG_UTIL_EXCEPTION_H
#define EGG_UTIL_EXCEPTION_H
#include <egg/types_egg.h>

#include <nw4r/db.h>

namespace EGG {

/**
 * \~english @brief Callback function to be executed when an exception occurs.
 *
 * \~english @param console Pointer to NintendoWare console
 * \~english @param pArg Additional arguments (in the Pack Project, it is used
 * for the expected button combination to show the handler on screen)
 * \~english @return Always returns TRUE in the Pack Project.
 *
 * \~ @note Implementation-dependent. For example, in Big Brain Academy: Wii
 * Degree, this just calls OSPanic.
 */
bool ExceptionCallBack_(nw4r::db::ConsoleHandle console, void* pArg);

class Exception {
public:
    /**
     * \~english @brief Creates an EGG::Exception object.
     * \~japanese @brief EGG::Exceptionオブジェクトを作成する。
     *
     * \~english @param width Console width
     * \~japanese @param width コンソールの幅
     * \~english @param height Console height
     * \~japanese @param height コンソールの高さ
     * \~english @param attr Console attributes (more investigation needed)
     * \~japanese @param attr コンソールの属性（検査必要）
     * \~english @param pHeap Pointer to exception heap
     * \~japanese @param pHeap 例外ヒープへのポインタ
     * \~english @return The new object.
     * \~japanese @return 新しいオブジェクト。
     */
    static Exception* create(u16 width, u16 height, u16 attr, Heap* pHeap);
    /**
     * \~english @brief Sets the button combination needed for showing the
     * exception handler on the TV screen.
     * \~japanese @brief
     * 例外ハンドラをテレビ画面に表示するボタンシーケンスを設定する。
     *
     * \~english @param pPadInfo Pointer to a struct of WPAD_BUTTON values
     * \~japanese @param pPadInfo WPAD_BUTTON値の構造体へのポインタ
     *
     * \~ @details pPadInfo is used as the void* argument in ExceptionCallBack_.
     * \~japanese @details pPadInfoがExceptionCallBack_のvoid*引数です。
     */
    static void setPadInfo(const u16* pPadInfo);

    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     *
     * \~english @param width Console width
     * \~japanese @param width コンソールの幅
     * \~english @param height Console height
     * \~japanese @param height コンソールの高さ
     * \~english @param attr Console attributes (more investigation needed)
     * \~japanese @param attr コンソールの属性（検査必要）
     * \~english @param pHeap Pointer to exception heap
     * \~japanese @param pHeap 例外ヒープへのポインタ
     */
    Exception(u16 width, u16 height, u16 attr, Heap* pHeap);
    /**
     * \~english @brief Destructor
     * \~japanese @brief デストラクタ
     */
    virtual ~Exception(); // at 0x8

    /**
     * \~english @brief Static EGG::Exception object.
     * \~japanese @brief スタティックなEGG::Exceptionオブジェクト。
     */
    static Exception* sException;
    /**
     * \~english @brief Static NintendoWare console handle.
     * \~japanese @brief NintendoWareコンソールのスタティックなハンドル。
     */
    static nw4r::db::ConsoleHandle sConsoleHandle;
    /**
     * \~english @brief Static buffer for loading a map file.
     * \~japanese @brief マップファイルをロードするスタティックなバッファ。
     */
    static void* sMapFileBuffer;
};

} // namespace EGG

#endif
