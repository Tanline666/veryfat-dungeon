#include <Pack/RPGraphics.h>

#include <egg/core.h>

/**
 * \~english @brief File extension for RPGrpModelMisc binaries
 */
template <> const char* EGG::IBinary<RPGrpModelMisc>::spExtension = "pmmisc";

/**
 * \~english @brief Gets the binary signature of this object
 */
const char* RPGrpModelMisc::GetBinaryType() const {
    return "MMSC";
}
