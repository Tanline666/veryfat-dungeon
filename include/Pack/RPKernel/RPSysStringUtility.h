#ifndef RP_KERNEL_STRING_UTILITY_H
#define RP_KERNEL_STRING_UTILITY_H
#include <Pack/types_pack.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief String utility functions
 */
class RPSysStringUtility {
public:
    /**
     * \~english @brief Decimal point fill type
     */
    enum FillType {
        FillType_HalfWidth, //!< Use half-width characters
        FillType_FullWidth, //!< Use full-width characters
    };

public:
    /**
     * \~english @brief Tests whether the specified integral value is a singular
     * noun
     *
     * \~english @param x Integral value
     */
    static bool IsSingular(s32 x);

    /**
     * \~english @brief Tests whether the specified decimal value is a singular
     * noun
     *
     * \~english @param x Decimal value
     */
    static bool IsSingular(f32 x);

    /**
     * \~english @brief Converts the specified integer to a half-width string
     *
     * \~english @param x Integer value
     * \~english @param[out] pBuffer String buffer
     * \~english @param maxLen Buffer length
     * \~english @param plusSign Whether to include signage for positive values
     */
    static void GetStringHalfSizeNumber(s32 x, wchar_t* pBuffer, u32 maxLen,
                                        bool plusSign = false);

    /**
     * \~english @brief Converts the specified integer to a full-width string
     *
     * \~english @param x Integer value
     * \~english @param[out] pBuffer String buffer
     * \~english @param maxLen Buffer length
     * \~english @param plusSign Whether to include signage for positive values
     */
    static void GetStringFullSizeNumber(s32 x, wchar_t* pBuffer, u32 maxLen,
                                        bool plusSign = false);

    /**
     * \~english @brief Converts the specified decimal to a half-width string
     *
     * \~english @param x Decimal value
     * \~english @param width Value string width
     * \~english @param precision Value string precision
     * \~english @param[out] pBuffer String buffer
     * \~english @param maxLen Buffer length
     * \~english @param plusSign Whether to include signage for positive values
     * \~english @param fillType Decimal point fill type
     */
    static void GetStringHalfSizeNumber(f32 x, s32 width, s32 precision,
                                        wchar_t* pBuffer, u32 maxLen,
                                        bool plusSign, FillType fillType);

    /**
     * \~english @brief Converts half-width characters in the specified string
     * to full-width characters
     *
     * \~english @param[in,out] pStr Input string
     */
    static void ChangeToFullSize(wchar_t* pStr);

    /**
     * \~english @brief Converts the specified date to a half-width string
     *
     * \~english @param year Year
     * \~english @param month Month
     * \~english @param day Day
     * \~english @param[out] pBuffer String buffer
     * \~english @param maxLen Buffer length
     * \~english @param yearSize Number of digits to use when expressing the
     * year
     */
    static void GetStringHalfSizeDate(u32 year, u32 month, u32 day,
                                      wchar_t* pBuffer, u32 maxLen,
                                      s32 yearSize = 4);

    /**
     * \~english @brief Converts the specified time to a half-width string
     *
     * \~english @param hour Hour
     * \~english @param min Minute
     * \~english @param[out] pBuffer String buffer
     */
    static void GetStringHalfSizeTime(u32 hour, u32 min, wchar_t* pBuffer);

    /**
     * \~english @brief Parses a float value from the specified string
     * @details This function supports strings using commas as the decimal
     *
     * \~english @param pStr Input string
     */
    static f32 GetFloat(const wchar_t* pStr);

    /**
     * \~english @brief Changes decimal points in the specified string to match
     * the locale
     *
     * \~english @param pStr Input string
     */
    static void ChangePoint(wchar_t* pStr);

    /**
     * \~english @brief Swaps decimal points in the specified string between
     * half and full width
     *
     * \~english @param pStr Input string
     */
    static void ChangePointHalfFull(wchar_t* pStr);
};

//! @}

#endif
