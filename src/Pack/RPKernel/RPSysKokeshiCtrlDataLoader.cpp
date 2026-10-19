#include <Pack/RPKernel.h>

#include <RVLFaceLib.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param chan Remote channel
 */
RPSysKokeshiCtrlDataLoader::RPSysKokeshiCtrlDataLoader(s32 chan)
    : mChannel(chan), mLoadState(ELoadState_Start) {}

/**
 * \~english @brief Destructor
 */
RPSysKokeshiCtrlDataLoader::~RPSysKokeshiCtrlDataLoader() {}

/**
 * \~english @brief Tests whether an error occurred during loading
 */
BOOL RPSysKokeshiCtrlDataLoader::isError() const {
    return mLoadState != ELoadState_Finish ? TRUE : FALSE;
}

/**
 * \~english @brief Begins loading Mii data from the controller
 */
void RPSysKokeshiCtrlDataLoader::load() {
    if (mLoadState == ELoadState_Wait) {
        return;
    }

    mLoadState = ELoadState_Wait;

    switch (RFLLoadControllerAsync(mChannel)) {
    case RFLErrcode_Controllerfail: {
        mLoadState = ELoadState_Start;
        break;
    }

    case RFLErrcode_Busy: {
        break;
    }

    default: {
        mLoadState = ELoadState_Start;
        break;
    }
    }
}

/**
 * \~english @brief Tests whether the loader is in an idle state
 */
BOOL RPSysKokeshiCtrlDataLoader::isIdle() const {
    return mLoadState != ELoadState_Wait ? TRUE : FALSE;
}

/**
 * \~english @brief Updates the loader state
 */
void RPSysKokeshiCtrlDataLoader::calc() {
    switch (mLoadState) {
    case ELoadState_Start: {
        break;
    }

    case ELoadState_Wait: {
        switch (RFLGetAsyncStatus()) {
        case RFLErrcode_Success:
        case RFLErrcode_NANDCommandfail: {
            mLoadState = ELoadState_Finish;
            break;
        }

        case RFLErrcode_Loadfail:
        case RFLErrcode_Controllerfail: {
            mLoadState = ELoadState_Start;
            break;
        }

        case RFLErrcode_Busy: {
            break;
        }

        default: {
            mLoadState = ELoadState_Start;
            break;
        }
        }

        break;
    }

    case ELoadState_Finish: {
        break;
    }
    }
}
