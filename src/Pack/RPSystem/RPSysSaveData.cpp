#include <Pack/RPSystem.h>

#include <revolution/OS.h>

/**
 * \~english @brief Constructor
 */
RPSysSaveData::RPSysSaveData()
    : mpSportsSystemData(NULL),
      mpSportsPlayerList(NULL),
      mpPartySystemData(NULL),
      mpPartyPlayerList(NULL),
      unk14(0),
      unk18(0),
      unk1C(0),
      unk20(0),
      unk24(0) {

#if defined(PACK_SPORTS)
    mpSportsSystemData = new RPSportsSystemData();
    mpSportsPlayerList = new RPSportsPlayerData[PLAYER_LIST_SIZE];
#elif defined(PACK_PARTY)
    mpPartySystemData = new RPPartySystemData();
    mpPartyPlayerList = new RPPartyPlayerData[PLAYER_LIST_SIZE];
#endif

    mFlags.makeAllZero();
    reset();
}

/**
 * \~english @brief Resets the save data state
 */
void RPSysSaveData::reset() {
#if defined(PACK_SPORTS)
    mpSportsSystemData->reset();
#elif defined(PACK_PARTY)
    mpPartySystemData->reset();
#endif

    for (int i = 0; i < PLAYER_LIST_SIZE; i++) {
#if defined(PACK_SPORTS)
        mpSportsPlayerList[i].reset();
#elif defined(PACK_PARTY)
        mpPartyPlayerList[i].reset();
#endif
    }
}

/**
 * \~english @brief Tests whether an error occurred while reading the save file
 * @typo
 */
bool RPSysSaveData::isErrorOccured() const {
    return !mFlags.isZero();
}

/**
 * \~english @brief Gets the Sports Pack player data at the specified player
 * list index
 *
 * \~english @param idx Player list index
 */
RPSportsPlayerData* RPSysSaveData::getSportsPlayerData(u32 idx) const {
    return &mpSportsPlayerList[idx];
}

/**
 * \~english @brief Sets the Sports Pack player data at the specified player
 * list index
 *
 * \~english @param pPlayerData Player data
 * \~english @param idx Player list index
 */
void RPSysSaveData::setSportsPlayerData(const RPSportsPlayerData* pPlayerData,
                                        u32 idx) {
    mpSportsPlayerList[idx] = *pPlayerData;
}

/**
 * \~english @brief Gets the Sports Pack system data
 */
RPSportsSystemData* RPSysSaveData::getSportsSystemData() const {
    return mpSportsSystemData;
}

/**
 * \~english @brief Sets the Sports Pack system data
 *
 * \~english @param pSystemData System data
 */
void RPSysSaveData::setSportsSystemData(const RPSportsSystemData* pSystemData) {
    *mpSportsSystemData = *pSystemData;
}

/**
 * \~english @brief Gets the Party Pack player data at the specified player list
 * index
 *
 * \~english @param idx Player list index
 */
RPPartyPlayerData* RPSysSaveData::getPartyPlayerData(u32 idx) const {
    return &mpPartyPlayerList[idx];
}

/**
 * \~english @brief Sets the Party Pack player data at the specified player list
 * index
 *
 * \~english @param pPlayerData Player data
 * \~english @param idx Player list index
 */
void RPSysSaveData::setPartyPlayerData(const RPPartyPlayerData* pPlayerData,
                                       u32 idx) {
    mpPartyPlayerList[idx] = *pPlayerData;
}

/**
 * \~english @brief Gets the Party Pack system data
 */
RPPartySystemData* RPSysSaveData::getPartySystemData() const {
    return mpPartySystemData;
}

/**
 * \~english @brief Sets the Party Pack system data
 *
 * \~english @param pSystemData System data
 */
void RPSysSaveData::setPartySystemData(const RPPartySystemData* pSystemData) {
    *mpPartySystemData = *pSystemData;
}

/**
 * \~english @brief Deserializes this object from the specified stream
 *
 * \~english @param rStrm Memory stream
 * \~english @param pRawSave Raw save data
 */
void RPSysSaveData::read(EGG::RamStream& rStrm, const void* pRawSave) {
    mFlags.makeAllZero();

    if (rStrm.read_u32() != SIGNATURE_PACK) {
        mFlags.setBit(EFlag_BadPack);

        reset();
        return;
    }

    if (rStrm.read_u32() != SIGNATURE_VERSION) {
        mFlags.setBit(EFlag_BadVersion);

        reset();
        return;
    }

#if defined(PACK_SPORTS)
    mpSportsSystemData->read(rStrm);
#elif defined(PACK_PARTY)
    mpPartySystemData->read(rStrm);
#endif

    for (int i = 0; i < PLAYER_LIST_SIZE; i++) {
#if defined(PACK_SPORTS)
        mpSportsPlayerList[i].read(rStrm);
#elif defined(PACK_PARTY)
        mpPartyPlayerList[i].read(rStrm);
#endif
    }

    if (calcCrc(pRawSave, rStrm.getPosition()) != rStrm.read_u32()) {
        mFlags.setBit(EFlag_BadCrc);

        reset();
        return;
    }
}

/**
 * \~english @brief Serializes this object to the specified stream
 *
 * \~english @param rStrm Memory stream
 * \~english @param pRawSave Raw save data
 */
void RPSysSaveData::write(EGG::RamStream& rStrm, const void* pRawSave) const {
    rStrm.write_u32(SIGNATURE_PACK);
    rStrm.write_u32(SIGNATURE_VERSION);

    mpSportsSystemData->write(rStrm);

    for (int i = 0; i < PLAYER_LIST_SIZE; i++) {
        mpSportsPlayerList[i].write(rStrm);
    }

    rStrm.write_u32(calcCrc(pRawSave, rStrm.getPosition()));
}

/**
 * \~english @brief Gets the size of the current pack's save file, in bytes
 */
u32 RPSysSaveData::getSaveFileSize() {
#if defined(PACK_SPORTS)
    return OS_MEM_KB_TO_B(128);
#elif defined(PACK_PARTY)
    return OS_MEM_KB_TO_B(32);
#endif
}

/**
 * \~english @brief Calculates a checksum of the specified data
 *
 * \~english @param pData Data buffer
 * \~english @param size Buffer size
 * \~english @return 32-bit checksum
 */
u32 RPSysSaveData::calcCrc(const void* pData, u32 size) {
    u16 sum = 0;
    u16 isum = 0;
    const u16* p = static_cast<const u16*>(pData);

    for (int i = 0; i < size / sizeof(u16); i++, p++) {
        sum += *p;
        isum += ~*p;
    }

    return sum << 16 | isum;
}
