#ifndef RP_KERNEL_OFFICIAL_DB_H
#define RP_KERNEL_OFFICIAL_DB_H
#include <Pack/types_pack.h>

#include <Pack/RPKernel/RPSysKokeshiManager.h>

#include <RVLFaceLib.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief NAND Mii database accessor
 */
class RPSysOfficialDB {
private:
    //! Mii states by database index
    RPSysKokeshiManager::OfficialDBState mStates[RFL_DB_CHAR_MAX]; // at 0x0

public:
    /**
     * \~english @brief Constructor
     */
    RPSysOfficialDB();

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysOfficialDB(); // at 0x8

    /**
     * \~english @brief Finds the index holding the Mii with the specified
     * create ID
     *
     * \~english @param pCreateID Mii create ID
     * \~english @return Database index, or -1 if not found
     */
    s16 GetIndexFromCreateID(const RFLCreateID* pCreateID);

    /**
     * \~english @brief Resets the database to the latest states
     */
    void Reset();

    u16 GetOfficialDBValidNum();

    /**
     * \~english @brief Gets the number of valid Miis in the database
     */
    u16 GetValidNum();

    /**
     * \~english @brief Gets the state of the specified database index
     *
     * \~english @param index Database index
     */
    RPSysKokeshiManager::OfficialDBState GetState(u16 index) {
        return mStates[index];
    }

    /**
     * \~english @brief Sets the state of the specified database index
     *
     * \~english @param index Database index
     * \~english @param state New state
     */
    void SetState(u16 index, RPSysKokeshiManager::OfficialDBState state) {
        mStates[index] = state;
    }
};

//! @}

#endif
