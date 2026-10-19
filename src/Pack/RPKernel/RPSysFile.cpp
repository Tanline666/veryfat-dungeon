#include <Pack/RPKernel.h>
#include <Pack/RPSystem.h>

#include <cstring>

/**
 * \~english @brief Constructor
 *
 * \~english @param pPath File path (absolute)
 * \~english @param size File data size
 * \~english @param pData File data pointer
 */
RPSysFile::RPSysFile(const char* pPath, s32 size, const void* pData)
    : mSize(size) {

    // @bug Unsafe string operations
#if defined(BUG_FIX)
    std::strncpy(mPath, pPath, ARRAY_SIZE(mPath));
#else
    std::strcpy(mPath, pPath);
#endif

    mpData = static_cast<const u8*>(pData);
}

/**
 * \~english @brief Destructor
 * @details The file is automatically removed from the resource cache.
 */
RPSysFile::~RPSysFile() {
    RP_GET_INSTANCE(RPSysResourceManager)->RemoveFromFileList(this);
}
