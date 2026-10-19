#ifndef RP_SYSTEM_PLAYER_H
#define RP_SYSTEM_PLAYER_H
#include <Pack/types_pack.h>

#include <RVLFaceLib.h>

//! UNDER CONSTRUCTION

//! @addtogroup rp_system
//! @{
/**
 * \~english @brief Player function parameter set
 */
struct RPSysPlayerArg {
    u8 placeholder[1000];
};

/**
 * \~english @brief Player object
 */
class RPSysPlayer {
public:
    /**
     * \~english @brief Constructor
     */
    RPSysPlayer();

    /**
     * \~english @brief Destructor
     */
    ~RPSysPlayer() {
        // @bug Memory leak
#if defined(BUG_FIX)
        delete mpSportsPlayerData;
        mpSportsPlayerData = NULL;

        delete mpPartyPlayerData;
        mpPartyPlayerData = NULL;
#endif
    }

    /**
     * \~english @brief Resets the data to a default state
     */
    void reset();

    /**
     * \~english @brief Sets the Mii avatar associated with this player
     *
     * \~english @param dataSrc Mii data source
     * \~english @param index Mii database index
     * \~english @param rCreateID Mii create ID
     */
    void setAvatar(s8 dataSrc, u16 index, const RFLCreateID& rCreateID);

    /**
     * \~english @brief Loads this player's data
     *
     * \~english @param rArg Parameter set
     */
    void loadData(const RPSysPlayerArg& rArg);

    /**
     * \~english @brief Saves this player's data
     *
     * \~english @param rArg Parameter set
     */
    void saveData(const RPSysPlayerArg& rArg);

    /**
     * \~english @brief Gets the controller channel used by this player
     */
    u8 getChannel() const {
        return mChannel;
    }
    /**
     * \~english @brief Sets the controller channel used by this player
     *
     * \~english @param chan Controller channel
     */
    void setChannel(u8 chan) {
        mChannel = chan;
    }

    /**
     * \~english @brief Gets the RFL data source where this player's avatar can
     * be found
     * @details If the data source was never configured, this value will be -1.
     */
    s8 getDataSource() const {
        return mDataSource;
    }

    /**
     * \~english @brief Gets the database index where this player's avatar can
     * be found
     */
    u16 getIndex() const {
        return mIndex;
    }

    /**
     * \~english @brief Accesses the Mii create ID of this player's avatar
     */
    const RFLCreateID& getCreateID() const {
        return mCreateID;
    }

private:
    //! Controller channel
    u8 mChannel; // at 0x0

    //! Mii data source
    s8 mDataSource; // at 0x1
    //! Mii database index
    u16 mIndex; // at 0x2
    //! Mii create ID
    RFLCreateID mCreateID; // at 0x4

    //! Player data list index
    s32 mPlayerDataIndex; // at 0xC

    void* placeholder;
};

//! @}

#endif
