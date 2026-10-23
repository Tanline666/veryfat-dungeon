#include <Pack/RPKernel.h>
#include <Pack/RPSingleton.h>
#include <Pack/RPSystem.h>

#include <revolution/DVD.h>
#include <revolution/VI.h>

RP_SINGLETON_IMPL(RPSysDvdStatus);

/**
 * \~english @brief Game disc name
 */

const char* RPSysDvdStatus::GAME_DISK_NAME = "Wii Fit Plus";

/**
 * \~english @brief Tests whether a disc drive error has occurred
 * @typo
 */
bool RPSysDvdStatus::isErrorOccured() const {
    return mErrorStatus > EErrorStatus_Busy;
}

/**
 * \~english @brief Updates the status of the disc drive
 * \~english @return Whether the disc drive status is OK
 */
bool RPSysDvdStatus::update() {
    s32 status = DVDGetDriveStatus();

    if (status == DVD_STATE_FATAL) {
        mErrorStatus = EErrorStatus_Fatal;
    } else if (status == DVD_STATE_NO_DISK) {
        mErrorStatus = EErrorStatus_NoDisk;
    } else if (status == DVD_STATE_WRONG_DISK_ID) {
        mErrorStatus = EErrorStatus_WrongDisk;
    } else if (status == DVD_STATE_BUSY) {
        mErrorStatus = EErrorStatus_Busy;
    } else {
        mErrorStatus = EErrorStatus_Success;
    }

    if (isErrorOccured()) {
        RPSysCoreController::stopMotorAll();

        RP_GET_INSTANCE(RPSysHomeMenuMgr)->update();

        if (RP_GET_INSTANCE(RPSysSceneMgr)->isDvdErrorApplicationEnd()) {
            RP_GET_INSTANCE(RPSysSystem)->systemShutDown();
        }

        return false;
    }

    return true;
}

void RPSysDvdStatus::draw() {
    int handling;

    if (mErrorStatus < 0) {
        VISetBlack(FALSE);
        mpErrorFader->draw();
        handling = EErrorHandling_Fatal;
        if (mErrorStatus == EErrorStatus_Retry) {
            handling = EErrorHandling_Retry;
        } else if (mErrorStatus != EErrorStatus_Fatal) {
            handling = EErrorHandling_Default;
        }

        RPSysWideTextWriter::CreateInstance();
    }
}

/**
 * \~english @brief Constructor
 */
RPSysDvdStatus::RPSysDvdStatus() : mErrorStatus(EErrorStatus_Success) {
    mpErrorFader->setStatus(EGG::ColorFader::STATUS_PREPARE_IN);
}

/**
 * \~english @brief Destructor
 */
RPSysDvdStatus::~RPSysDvdStatus() {}
