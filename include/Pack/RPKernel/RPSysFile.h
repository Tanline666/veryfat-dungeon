#ifndef RP_KERNEL_FILE_H
#define RP_KERNEL_FILE_H
#include <Pack/types_pack.h>

#include <egg/core.h>

#include <nw4r/ut.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Cached resource file
 */
class RPSysFile : public EGG::Disposer {
public:
    //! Maximum file path length
    static const u32 PATH_MAX = 128;

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pPath File path (absolute)
     * \~english @param size File data size
     * \~english @param pData File data pointer
     */
    RPSysFile(const char* pPath, s32 size, const void* pData);

    /**
     * \~english @brief Destructor
     * @details The file is automatically removed from the resource cache.
     */
    virtual ~RPSysFile() override; // at 0x8

    /**
     * \~english @brief Gets the path to this file
     */
    const char* GetPath() const {
        return mPath;
    }

    /**
     * \~english @brief Gets the size of this file
     */
    s32 GetSize() const {
        return mSize;
    }

    /**
     * \~english @brief Gets the data of this file
     */
    const u8* GetData() const {
        return mpData;
    }

public:
    //! Linked-list node used for cache lists
    NW4R_UT_LIST_LINK_DECL(); // at 0x10

private:
    //! File path
    char mPath[PATH_MAX]; // at 0x18
    //! File data size
    s32 mSize; // at 0x98
    //! File data contents
    const u8* mpData; // at 0x9C
};

//! @}

#endif
