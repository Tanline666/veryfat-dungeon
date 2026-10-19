#ifndef RP_KERNEL_KOKESHI_ITERATER_H
#define RP_KERNEL_KOKESHI_ITERATER_H
#include <Pack/types_pack.h>

#include <RVLFaceLib.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Mii data source iterator
 * @typo
 */
class RPSysKokeshiIterater {
private:
    //! Whether this iterator has been initialized
    BOOL mIsInitialized; // at 0x0
    //! Selected data source
    RFLDataSource mDataSource; // at 0x4
    //! Current database index
    u16 mIndex; // at 0x8

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param src Mii data source to view
     */
    explicit RPSysKokeshiIterater(RFLDataSource src);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysKokeshiIterater(); // at 0x8

    /**
     * \~english @brief Moves the iterator back to the start
     */
    void Reset();

    /**
     * \~english @brief Advances the iterator to the next Mii
     *
     * \~english @return Whether the iterator is valid
     */
    bool Update();

    /**
     * \~english @brief Gets the current database index
     */
    u16 GetIndex() const {
        return mIndex;
    }

private:
    /**
     * \~english @brief Advances the iterator one index
     *
     * \~english @return Whether the iterator is valid
     */
    bool Increment();
};

//! @}

#endif
