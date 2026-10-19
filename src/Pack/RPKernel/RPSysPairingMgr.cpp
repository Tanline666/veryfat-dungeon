#include <Pack/RPKernel.h>

#include <revolution/WPAD.h>

/**
 * \~english @brief Whether pairing is ongoing
 */
bool RPSysPairingMgr::sIsBusy = false;

/**
 * \~english @brief WPAD simple sync callback
 *
 * \~english @param result WPAD sync result
 * \~english @param num Number of synced controllers
 */
void RPSysPairingMgr::simpleSyncCallback(s32 result, s32 /* num*/) {
    switch (result) {
    case WPAD_SYNC_WAITING: {
        break;
    }

    case WPAD_SYNC_BUSY: {
        sIsBusy = false;
        break;
    }

    case WPAD_SYNC_DONE: {
        sIsBusy = false;
        break;
    }

    default: {
        break;
    }
    }
}

/**
 * \~english @brief Constructor
 */
RPSysPairingMgr::RPSysPairingMgr()
    : mPairingState(EPairingState_Start), mIsEnabled(false) {

    WPADSetSimpleSyncCallback(simpleSyncCallback);
}

/**
 * \~english @brief Updates the pairing state
 */
void RPSysPairingMgr::calc() {
    switch (mPairingState) {
    case EPairingState_Start: {
        if (!mIsEnabled) {
            break;
        }

        if (WPADStartSimpleSync()) {
            mPairingState = EPairingState_Stop;
            sIsBusy = true;
        }
        break;
    }

    case EPairingState_Stop: {
        if (mIsEnabled) {
            break;
        }

        if (WPADStopSimpleSync()) {
            mPairingState = EPairingState_Wait;
        }
        break;
    }

    case EPairingState_Wait: {
        if (!sIsBusy) {
            mPairingState = EPairingState_Start;
        }
        break;
    }
    }
}
