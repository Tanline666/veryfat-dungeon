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
 * \~english @brief Deserializes this object from the specified stream
 *
 * \~english @param rStrm Memory stream
 * \~english @param pRawSave Raw save data
 */
void RPSysSaveData::read(EGG::RamStream& rStrm, const void* pRawSave) {
    mFlags.makeAllZero();

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
