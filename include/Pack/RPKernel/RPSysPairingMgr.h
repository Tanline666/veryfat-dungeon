#ifndef RP_KERNEL_PAIRING_MGR_H
#define RP_KERNEL_PAIRING_MGR_H
#include <Pack/types_pack.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Controller pairing manager
 */
class RPSysPairingMgr {
public:
    /**
     * \~english @brief Constructor
     */
    RPSysPairingMgr();

    /**
     * \~english @brief Updates the pairing state
     */
    void calc();

    /**
     * \~english @brief Sets whether pairing is enabled
     *
     * \~english @param enabled Whether to enable pairing
     */
    void setEnabled(bool enabled) {
        mIsEnabled = enabled;
    }

private:
    /**
     * \~english @brief Pairing state
     */
    enum EPairingState {
        EPairingState_Start, //! Begin sync search
        EPairingState_Stop,  //!< End sync search
        EPairingState_Wait,  //!< Wait for results
    };

private:
    /**
     * \~english @brief WPAD simple sync callback
     *
     * \~english @param result WPAD sync result
     * \~english @param num Number of synced controllers
     */
    static void simpleSyncCallback(s32 result, s32 num);

private:
    //! Whether pairing is ongoing
    static bool sIsBusy;

    //! Current pairing state
    EPairingState mPairingState; // at 0x0
    //! Whether pairing is enabled
    bool mIsEnabled; // at 0x4
};

//! @}

#endif
