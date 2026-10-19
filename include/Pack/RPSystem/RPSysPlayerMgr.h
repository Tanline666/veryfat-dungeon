#ifndef RP_SYSTEM_PLAYER_MGR_H
#define RP_SYSTEM_PLAYER_MGR_H
#include <Pack/types_pack.h>

#include <Pack/RPKernel.h>
#include <Pack/RPSingleton.h>
#include <Pack/RPSystem/RPSysPlayer.h>

#include <RVLFaceLib.h>

#include <egg/core.h>

//! @addtogroup rp_system
//! @{

struct RPSysPlayerOldData {
    //! Mii data source
    s8 dataSource; // at 0x0
    //! Mii database index
    u16 index; // at 0x2
    //! Mii create ID
    RFLCreateID createID; // at 0x4
    //! Controller address
    RPSysCoreAddress address; // at 0xC
};

/**
 * \~english @brief Player manager
 */
class RPSysPlayerMgr {
    RP_SINGLETON_DECL_EX(RPSysPlayerMgr);

public:
    //! Number of old data entries
    // UNDER CONSTRUCTION
    static const int OLD_DATA_LEN = 8;

public:
    /**
     * \~english @brief Resets the old data to a default state
     */
    void resetOldData();

    /**
     * \~english @brief Gets the player at the specified index
     *
     * \~english @param idx Player index
     */
    const RPSysPlayer& getPlayer(s32 idx) const;

    /**
     * \~english @brief Gets the remote channel associated with the specified
     * player
     *
     * \~english @param idx Player index
     */
    u8 getChannel(s32 idx) const;

    /**
     * \~english @brief Gets the speaker flag required to play sounds through
     * the specified player's remote
     *
     * \~english @param player Player index
     */
    u32 getSpeakerFlag(s32 player) const;

    /**
     * \~english @brief Writes the save data for all players
     */
    void saveAllData();

    /**
     * \~english @brief Tests whether the specified player has completed the A+B
     * check
     *
     * \~english @param player Player index
     */
    bool isABPress(s32 player) const;

    /**
     * \~english @brief Sets that the specified player has completed the A+B
     * check
     *
     * \~english @param player Player index
     */
    void setABPress(s32 player);

    /**
     * \~english @brief Resets the manager state
     */
    void reset();

    /**
     * \~english @brief Gets the configured number of players
     */
    s32 getPlayerNum() const {
        return mPlayerNum;
    }

    /**
     * \~english @brief Sets the number of players
     *
     * \~english @param num Player count
     */
    void setPlayerNum(s32 num);

    /**
     * \~english @brief Gets the number of controllers
     */
    s32 getRemoteNum() const {
        return mRemoteNum;
    }

    /**
     * \~english @brief Sets the number of controllers
     *
     * \~english @param num Controller count
     */
    void setRemoteNum(s32 num);

    /**
     * \~english @brief Tests whether the old data configuration for the current
     * player count is usable
     */
    bool isValidOldData();

    /**
     * \~english @brief Gets the old data configuration for the specified player
     *
     * \~english @param[out] pDataSrc Mii data source
     * \~english @param[out] pIndex Mii database index
     * \~english @param idx Player index
     */
    void getOldData(s8* pDataSrc, u16* pIndex, s32 idx);

    /**
     * \~english @brief Tests whether the old data configuration for the
     * specified player describes a Mii from a controller database
     *
     * \~english @param idx Player index
     */
    bool isCtrlOldData(s32 idx);

    /**
     * \~english @brief Updates the old data configuration for the current
     * player count
     */
    void calcOldData();

    /**
     * \~english @brief Loads the old data configuration from the save file
     */
    void loadOldData();

    /**
     * \~english @brief Writes the old data configuration to the save file
     */
    void saveOldData();

    /**
     * \~english @brief Loads the save data for the specified player
     *
     * \~english @param idx Player index
     */
    void loadData(s32 idx);

    /**
     * \~english @brief Loads the save data for all players
     */
    void loadAllData();

    /**
     * \~english @brief Sets the Mii avatar associated with the specified player
     *
     * \~english @param dataSrc Mii data source
     * \~english @param index Mii database index
     * \~english @param rCreateID Mii create ID
     * \~english @param idx Player index
     */
    void setAvatar(s8 dataSrc, u16 index, const RFLCreateID& rCreateID,
                   s32 idx);

private:
    //! Number of active players
    s32 mPlayerNum; // at 0x8
    //! Number of connected controllers
    s32 mRemoteNum; // at 0xC

    //! State flags
    u32 mFlags; // at 0x10

    //! Player objects
    RPSysPlayer* mpPlayers; // at 0x14
    //! Previous player configurations
    RPSysPlayerOldData* mpOldData; // at 0x18

    void* placeholder;
};

//! @}

#endif
