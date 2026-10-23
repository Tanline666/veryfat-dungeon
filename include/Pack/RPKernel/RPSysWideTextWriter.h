#ifndef RP_KERNEL_WIDE_TEXT_WRITER_H
#define RP_KERNEL_WIDE_TEXT_WRITER_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>

#include <nw4r/ut.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Wide-char screen text writer
 */
class RPSysWideTextWriter : public nw4r::ut::WideTextWriter {
    RP_SINGLETON_DECL(RPSysWideTextWriter);

public:
    /**
     * \~english @brief Creates the text buffer
     */
    static void Initialize();

    static void Begin(nw4r::ut::Font& rFont);

private:
    //! Size of the text work buffer
    static const int TEXT_BUFFER_SIZE = 256;

    //! Work buffer for string formatting
    static wchar_t* spTextBuffer;
};

//! @}

#endif
