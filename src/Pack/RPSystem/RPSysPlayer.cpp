#include <Pack/RPSystem.h>

#include <RVLFaceLib.h>

namespace {

/**
 * \~english @brief Tests whether two given RFL create IDs are equivalent
 *
 * \~english @param rLhs Left-hand side ID
 * \~english @param rRhs Right-hand side ID
 */
bool IsSameCreateID(const RFLCreateID& rLhs, const RFLCreateID& rRhs) {
    for (u32 i = 0; i < RFL_CREATEID_LEN; i++) {
        if (rLhs.data[i] != rRhs.data[i]) {
            return false;
        }
    }

    return true;
}

} // namespace

/**
 * \~english @brief Constructor
 */
RPSysPlayer::RPSysPlayer() {}

/**
 * \~english @brief Resets the data to a default state
 */
void RPSysPlayer::reset() {
    mChannel = WPAD_CHAN0;
    mDataSource = -1;
    mIndex = -1;
    mPlayerDataIndex = -1;

    for (int i = 0; i < RFL_CREATEID_LEN; i++) {
        mCreateID.data[i] = 0;
    }

    // UNDER CONSTRUCTION
}

/**
 * \~english @brief Sets the Mii avatar associated with this player
 *
 * \~english @param dataSrc Mii data source
 * \~english @param index Mii database index
 * \~english @param rCreateID Mii create ID
 */
void RPSysPlayer::setAvatar(s8 dataSrc, u16 index,
                            const RFLCreateID& rCreateID) {
    mDataSource = dataSrc;
    mIndex = index;
    mCreateID = rCreateID;
}
