#ifndef RP_KERNEL_LYT_TEXT_BOX_H
#define RP_KERNEL_LYT_TEXT_BOX_H
#include <Pack/types_pack.h>

#include <nw4r/lyt.h>

#include <cstdarg>

//! @addtogroup rp_kernel
//! @{

// Forward declarations
class RPSysMessage;

/**
 * \~english @brief NW4R layout textbox extension
 */
class RPSysLytTextBox : public nw4r::lyt::TextBox {
public:
    /**
     * \~english @brief Sets a formatted string to the textbox
     *
     * \~english @param pFmt Format string
     * \~english @param argc Number of format arguments
     * \~english @param ... Format arguments
     */
    void setText(const wchar_t* pFmt, int argc, ...);

    /**
     * \~english @brief Sets a formatted string to the textbox
     *
     * \~english @param pFmt Format string
     * \~english @param argc Number of format arguments
     * \~english @param argv Format arguments
     */
    void setText(const wchar_t* pFmt, int argc, va_list argv);

    /**
     * \~english @brief Sets a formatted message to the textbox
     *
     * \~english @param pMessage Message holder
     * \~english @param group Group ID
     * \~english @param idx Message index
     * \~english @param argc Number of format arguments
     * \~english @param ... Format arguments
     */
    void setMessage(RPSysMessage* pMessage, u32 group, u32 idx, int argc, ...);

    /**
     * \~english @brief Sets a formatted message to the textbox
     *
     * \~english @param pMessage Message holder
     * \~english @param group Group ID
     * \~english @param idx Message index
     * \~english @param argc Number of format arguments
     * \~english @param argv Format arguments
     */
    void setMessage(RPSysMessage* pMessage, u32 group, u32 idx, int argc,
                    va_list argv);

    /**
     * \~english @brief Sets the horizontal scale of the font
     *
     * \~english @param scale New horizontal scale
     */
    void setScaleX(f32 scale);

    /**
     * \~english @brief Sets a string to the textbox
     * @details This function also applies the attributes from the specified
     * message.
     *
     * \~english @param pMessage Message holder
     * \~english @param group Group ID
     * \~english @param idx Message index
     * \~english @param pText New text
     */
    void setMessage(RPSysMessage* pMessage, u32 group, u32 idx,
                    const wchar_t* pText);

    /**
     * \~english @brief Copies the front color from the specified textbox
     *
     * \~english @param rOther Textbox to copy from
     */
    void copyFontColor(const RPSysLytTextBox& rOther);

private:
    //! Size of the text work buffer
    static const int TEXT_BUFFER_SIZE = 512;

private:
    //! Work buffer for string formatting
    static wchar_t sTextBuffer[TEXT_BUFFER_SIZE];
};

//! @}

#endif
