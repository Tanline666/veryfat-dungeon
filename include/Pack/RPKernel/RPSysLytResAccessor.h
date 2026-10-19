#ifndef RP_KERNEL_LYT_RES_ACCESSOR_H
#define RP_KERNEL_LYT_RES_ACCESSOR_H
#include <Pack/types_pack.h>

#include <egg/core.h>

#include <nw4r/lyt.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief ArcResourceAccessor wrapper
 * @details Allows linking fonts from the font manager
 */
class RPSysLytResAccessor {
    friend class RPSysLayout;

public:
    /**
     * \~english @brief Creates a resource accessor from the specified heap
     *
     * \~english @param pHeap Heap to use for allocations
     */
    static RPSysLytResAccessor* create(EGG::Heap* pHeap = NULL);

    /**
     * \~english @brief Attempts to mount an archive
     *
     * \~english @param pBinary Archive binary data
     * \~english @param pRootDir Root directory to open (optional)
     * \~english @return Success
     */
    bool mountArchive(void* pBinary, const char* pRootDir = NULL);

    /**
     * \~english @brief Gets the specified resource from the currently open
     * archive
     * @details Font queries are redirected to the font manager
     *
     * \~english @param type Resource type
     * \~english @param pName Resource name
     * \~english @param[out] pSize Resource data size
     */
    void* getResource(u32 type, const char* pName, u32* pSize = NULL) {
        return mpImpl->GetResource(type, pName, pSize);
    }

private:
    /**
     * \~english @brief Internal implementation
     */
    class InternalAccessor : public nw4r::lyt::ArcResourceAccessor {
    public:
        /**
         * \~english @brief Constructor
         */
        InternalAccessor();

        /**
         * \~english @brief Gets the specified resource from the currently open
         * archive
         * @details Font queries are redirected to the font manager
         *
         * \~english @param type Resource type
         * \~english @param pName Resource name
         * \~english @param[out] pSize Resource data size
         */
        virtual void* GetResource(u32 type, const char* pName,
                                  u32* pSize) override; // at 0xC
    };

private:
    /**
     * \~english @brief Constructor
     */
    RPSysLytResAccessor();

private:
    //! Internal accessor implementation
    InternalAccessor* mpImpl; // at 0x0
};

//! @}

#endif
