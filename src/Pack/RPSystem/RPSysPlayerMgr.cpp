#include <Pack/RPSystem.h>

#include <RVLFaceLib.h>

#include <egg/core.h>

RP_SINGLETON_IMPL_EX(RPSysPlayerMgr);

/**
 * \~english @brief Resets the old data to a default state
 */
void RPSysPlayerMgr::resetOldData() {
    for (int i = 0; i < OLD_DATA_LEN; i++) {
        mpOldData[i].dataSource = -1;
        mpOldData[i].index = -1;

        for (int j = 0; j < RFL_CREATEID_LEN; j++) {
            mpOldData[i].createID.data[j] = 0;
        }

        mpOldData[i].address.reset();
    }
}

/**
 * \~english @brief Gets the player at the specified index
 *
 * \~english @param idx Player index
 */
const RPSysPlayer& RPSysPlayerMgr::getPlayer(s32 idx) const {
    return mpPlayers[idx];
}

/**
 * \~english @brief Gets the remote channel associated with the specified player
 *
 * \~english @param idx Player index
 */
u8 RPSysPlayerMgr::getChannel(s32 idx) const {
    return mpPlayers[idx].getChannel();
}

/**
 * \~english @brief Gets the speaker flag required to play sounds through the
 * specified player's remote
 *
 * \~english @param player Player index
 */
u32 RPSysPlayerMgr::getSpeakerFlag(s32 player) const {
    // TODO(kiwi) Uses something from RPSysCoreController
    s32 chan = getChannel(player);

    if (chan == WPAD_CHAN0) {
        return 1 << WPAD_CHAN0;
    }

    if (chan == WPAD_CHAN1) {
        return 1 << WPAD_CHAN1;
    }

    if (chan == WPAD_CHAN2) {
        return 1 << WPAD_CHAN2;
    }

    if (chan == WPAD_CHAN3) {
        return 1 << WPAD_CHAN3;
    }

    return 1 << WPAD_CHAN0 | //
           1 << WPAD_CHAN1 | //
           1 << WPAD_CHAN2 | //
           1 << WPAD_CHAN3;
}

/**
 * \~english @brief Tests whether the specified player has completed the A+B
 * check
 *
 * \~english @param player Player index
 */
bool RPSysPlayerMgr::isABPress(s32 player) const {
    return mFlags & 1 << static_cast<u8>(player);
}

/**
 * \~english @brief Sets that the specified player has completed the A+B check
 *
 * \~english @param player Player index
 */
void RPSysPlayerMgr::setABPress(s32 player) {
    mFlags |= 1 << static_cast<u8>(player);
}

/**
 * \~english @brief Resets the manager state
 */
void RPSysPlayerMgr::reset() {
    mPlayerNum = 0;
    mRemoteNum = 0;

    for (int i = 0; i < RP_MAX_PLAYERS; i++) {
        mpPlayers[i].reset();
    }
}

/**
 * \~english @brief Sets the number of players
 *
 * \~english @param num Player count
 */
void RPSysPlayerMgr::setPlayerNum(s32 num) {
    mPlayerNum = num;
}

/**
 * \~english @brief Sets the number of controllers
 *
 * \~english @param num Controller count
 */
void RPSysPlayerMgr::setRemoteNum(s32 num) {
    mRemoteNum = num < mPlayerNum ? num : mPlayerNum;

    s32 chan = WPAD_CHAN0;
    for (int i = 0; i < mPlayerNum; i++) {
        mpPlayers[i].setChannel(chan);

        if (++chan > num - 1) {
            chan = WPAD_CHAN0;
        }
    }
}

/**
 * \~english @brief Tests whether the old data configuration for the current
 * player count is usable
 */
bool RPSysPlayerMgr::isValidOldData() {
    // Can't use old data when people are sharing remotes
    if (mPlayerNum == mRemoteNum) {
        s32 rel2Abs[RP_MAX_PLAYERS] = {
            0,     // 1 player
            0 + 1, // 2 players
#if defined(PACK_SPORTS)
            0 + 1 + 2,     // 3 players
            0 + 1 + 2 + 3, // 4 players
#endif
        };

        s32 relIdx = mPlayerNum;
        s32 absIdx = rel2Abs[--relIdx];

        for (int i = absIdx; i < absIdx + mPlayerNum; i++) {
            if (mpOldData[i].dataSource != RFLDataSource_Official) {
                continue;
            }

            s16 expected = RPSysKokeshiManager::GetOfficialDBIndexFromCreateID(
                &mpOldData[i].createID);

            // Mii database has changed since last play
            if (mpOldData[i].index != expected) {
                return false;
            }
        }

        s32 player = 0;
        s32 found = 0;

        for (int i = absIdx; i < absIdx + mPlayerNum; i++, player++) {
            for (int j = absIdx; j < absIdx + mPlayerNum; j++) {
                if (!mpOldData[j].address.isValid()) {
                    continue;
                }

                if (RP_GET_CORE_CTRL(player)->getAddress().isEqual(
                        mpOldData[j].address)) {

                    found++;
                    mpOldData[i] = RPSysPlayerOldData(mpOldData[j]);
                    break;
                }
            }
        }

        return found == mPlayerNum;
    }

    return false;
}

/**
 * \~english @brief Gets the old data configuration for the current player count
 *
 * \~english @param[out] pDataSrc Mii data source
 * \~english @param[out] pIndex Mii database index
 * \~english @param idx Player index
 */
void RPSysPlayerMgr::getOldData(s8* pDataSrc, u16* pIndex, s32 idx) {
    s32 rel2Abs[RP_MAX_PLAYERS] = {
        0,     // 1 player
        0 + 1, // 2 players
#if defined(PACK_SPORTS)
        0 + 1 + 2,     // 3 players
        0 + 1 + 2 + 3, // 4 players
#endif
    };

    s32 relIdx = mPlayerNum;
    s32 absIdx = rel2Abs[--relIdx];

    *pDataSrc = mpOldData[idx + absIdx].dataSource;
    *pIndex = mpOldData[idx + absIdx].index;
}

/**
 * \~english @brief Tests whether the old data configuration for the specified
 * player describes a Mii from a controller database
 *
 * \~english @param idx Player index
 */
bool RPSysPlayerMgr::isCtrlOldData(s32 idx) {
    s32 rel2Abs[RP_MAX_PLAYERS] = {
        0,     // 1 player
        0 + 1, // 2 players
#if defined(PACK_SPORTS)
        0 + 1 + 2,     // 3 players
        0 + 1 + 2 + 3, // 4 players
#endif
    };

    s32 relIdx = mPlayerNum;
    s32 absIdx = rel2Abs[--relIdx];

    if (mpOldData[idx + absIdx].dataSource >= RFLDataSource_Controller1 &&
        mpOldData[idx + absIdx].dataSource <= RFLDataSource_Controller4 &&
        mpOldData[idx + absIdx].index < RFL_CTRL_CHAR_MAX) {

        return true;
    }

    return false;
}

/**
 * \~english @brief Updates the old data configuration for the current player
 * count
 */
void RPSysPlayerMgr::calcOldData() {
    // Can't use old data when people are sharing remotes
    if (mPlayerNum == mRemoteNum) {
        s32 rel2Abs[RP_MAX_PLAYERS] = {
            0,     // 1 player
            0 + 1, // 2 players
#if defined(PACK_SPORTS)
            0 + 1 + 2,     // 3 players
            0 + 1 + 2 + 3, // 4 players
#endif
        };

        s32 relIdx = mPlayerNum;
        s32 absIdx = rel2Abs[--relIdx];

        for (int i = 0; i < mPlayerNum; i++) {
            mpOldData[i + absIdx].dataSource = mpPlayers[i].getDataSource();
            mpOldData[i + absIdx].index = mpPlayers[i].getIndex();
            mpOldData[i + absIdx].createID = mpPlayers[i].getCreateID();
            mpOldData[i + absIdx].address = RP_GET_CORE_CTRL(i)->getAddress();
        }
    }
}

/**
 * \~english @brief Sets the Mii avatar associated with the specified player
 *
 * \~english @param dataSrc Mii data source
 * \~english @param index Mii database index
 * \~english @param rCreateID Mii create ID
 * \~english @param idx Player index
 */
void RPSysPlayerMgr::setAvatar(s8 dataSrc, u16 index,
                               const RFLCreateID& rCreateID, s32 idx) {

    mpPlayers[idx].setAvatar(dataSrc, index, rCreateID);
}

/**
 * \~english @brief Constructor
 *
 * \~english @param pHeap Parent heap
 */
RPSysPlayerMgr::RPSysPlayerMgr(EGG::Heap* pHeap)
    : mpParentHeap(pHeap),
      mPlayerNum(0),
      mRemoteNum(0),
      mFlags(0),
      mpPlayers(new RPSysPlayer[RP_MAX_PLAYERS]),
      mpOldData(new RPSysPlayerOldData[OLD_DATA_LEN]) {

#if defined(PACK_SPORTS)
    mpDefaultSportsPlayerData = new RPSportsPlayerData[RFL_DEFAULT_CHAR_MAX];
    mppCtrlSportsPlayerData = new RPSportsPlayerData*[RP_MAX_CONTROLLERS];

    for (int i = 0; i < RP_MAX_CONTROLLERS; i++) {
        mppCtrlSportsPlayerData[i] = new RPSportsPlayerData[OLD_DATA_LEN];
    }
#elif defined(PACK_PARTY)
    mpDefaultPartyPlayerData = new RPPartyPlayerData[RFL_DEFAULT_CHAR_MAX];
    mppCtrlPartyPlayerData = new RPPartyPlayerData*[RP_MAX_CONTROLLERS];

    for (int i = 0; i < RP_MAX_CONTROLLERS; i++) {
        mppCtrlPartyPlayerData[i] = new RPSportsPlayerData[OLD_DATA_LEN];
    }
#endif

    resetOldData();
}

/**
 * \~english @brief Destructor
 */
RPSysPlayerMgr::~RPSysPlayerMgr() {}
