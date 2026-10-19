#ifndef RP_KERNEL_MIDDLE_DB_H
#define RP_KERNEL_MIDDLE_DB_H
#include <Pack/types_pack.h>

#include <RVLFaceLib.h>

//! @addtogroup rp_kernel
//! @{

// Forward declarations
class RPSysMiddleDBGenInfo;

/**
 * \~english @brief In-memory Mii database
 */
class RPSysMiddleDB {
private:
    //! Middle/hidden database
    RFLMiddleDB* mpMiddleDB; // at 0x0
    //! Random database for bad queries
    RFLMiddleDB* mpRandomDB; // at 0x4

    //! Database capacity
    u16 mSize; // at 0x8
    //! Number of Miis stored in the middle database
    u16 mStoredNum; // at 0xA
    //! Current database index (only for RFLMiddleDBType_UserSet)
    u16 mCurrentIndex; // at 0xC

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pGenInfo Middle DB generation parameters
     */
    RPSysMiddleDB(const RPSysMiddleDBGenInfo* pGenInfo);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysMiddleDB(); // at 0x8

    /**
     * \~english @brief Increments the current database index
     * @details The current index wraps around when it becomes invalid
     */
    void IncrementCurrentIndex();

    /**
     * \~english @brief Gets the RFL parameters necessary to access the
     * specified Mii
     * @details If the index is invalid for the middle database, the parameters
     * returned will instead fetch a Mii from the random database.
     *
     * \~english @param index Database index
     * \~english @param ppMiddleDB Middle DB to use with RFL
     * \~english @param pIndex Database index to use with RFL
     */
    void GetRFLMiddleDB(u16 index, RFLMiddleDB** ppMiddleDB, u16* pIndex);

    /**
     * \~english @brief Gets the number of Miis stored in the middle database
     */
    u16 GetStoredNum() const {
        return mStoredNum;
    }

    /**
     * \~english @brief Gets the current database index
     */
    u16 GetCurrentIndex() const {
        return mCurrentIndex;
    }
};

//! @}

#endif
