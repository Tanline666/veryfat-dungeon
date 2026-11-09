#ifndef NW4R_SND_BANK_FILE_H
#define NW4R_SND_BANK_FILE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Channel.h>
#include <nw4r/snd/snd_Util.h>
#include <nw4r/snd/snd_WaveFile.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace snd {
namespace detail {

inline u8 ReadByte(const void* pAddr) {
    return *static_cast<const u8*>(pAddr);
}

/******************************************************************************
 *
 * RBNK binary format
 *
 ******************************************************************************/
struct BankFile {

    struct InstParam {
        union {
            s32 waveIndex;
            const WaveInfo* waveInfoAddress;
            WaveDataLocationCallback* waveDataLocationCallback;
        };
        u8 attack;               //!< // at 0x4
        u8 decay;                //!< // at 0x5
        u8 sustain;              //!< // at 0x6
        u8 release;              //!< // at 0x7
        u8 hold;                 //!< // at 0x8
        u8 waveDataLocationType; //!< // at 0x9
        u8 noteOffType;          //!< // at 0xA
        u8 alternateAssign;      //!< // at 0xB

        u8 originalKey; //!< // at 0xC
        u8 volume;      //!< // at 0xD
        u8 pan;         //!< // at 0xE
        u8 padding2;    //!< // at 0xF

        f32 tune; //!< // at 0x10

        Util::DataRef<void> lfoTableRef;        //!< // at 0x14
        Util::DataRef<void> graphEnvTablevRef;  //!< // at 0x1C
        Util::DataRef<void> randomizerTableRef; //!< // at 0x24

        u32 reserved; //!< // at 0x2C
    };

    //! Adapted from BrawlCrate
    enum RegionTableType {
        REGION_SET_INVALID,
        REGION_SET_DIRECT,
        REGION_SET_RANGE_TABLE,
        REGION_SET_INDEX_TABLE,
        REGION_SET_NULL,
    };

    struct RangeTable {
        u8 tableSize; // at 0x0
        u8 key[];     // at 0x1
    };

    struct IndexTable {
        u8 min;       // at 0x0
        u8 max;       // at 0x1
        u16 reserved; // at 0x2
        u8 ref[];     // at 0x4
    };

    typedef Util::DataRef<void, InstParam, RangeTable, IndexTable> DataRegion;
    typedef Util::DataRef<WaveFile::WaveInfo> WaveRegion;

    struct Header {
        ut::BinaryFileHeader fileHeader; // at 0x0
        u32 dataBlockOffset;             // at 0x10
        u32 dataBlockSize;               // at 0x14
        u32 waveBlockOffset;             // at 0x18
        u32 waveBlockSize;               // at 0x1C
    };

    struct DataBlock {
        ut::BinaryBlockHeader blockHeader; // at 0x0
        Util::Table<DataRegion> instTable; // at 0x8
    };

    struct WaveBlock {
        ut::BinaryBlockHeader blockHeader;     // at 0x0
        Util::Table<WaveRegion> waveInfoTable; // at 0x8
    };
};

/******************************************************************************
 *
 * BankFileReader
 *
 ******************************************************************************/
struct InstInfo {
    enum WaveDataLocationType {
        WAVE_DATA_LOCATION_INDEX,
        WAVE_DATA_LOCATION_ADDRESS,
        WAVE_DATA_LOCATION_CALLBACK
    };

    struct WaveDataLocation {
        WaveDataLocationType type; // at 0x0
        union {
            s32 index;
            const WaveInfo* address;
            WaveDataLocationCallback* callback;
        };
    };

    WaveDataLocation waveDataLocation; // at 0x0
    u8 attack;                         // at 0x8
    u8 hold;                           // at 0x9
    u8 decay;                          // at 0xA
    u8 sustain;                        // at 0xB
    u8 release;                        // at 0xC
    NoteOffType noteOffType;           // at 0x10
    u8 alternateAssign;                // at 0x14
    u8 originalKey;                    // at 0x15
    u8 pan;                            // at 0x16
    u8 volume;                         // at 0x17
    f32 tune;                          // at 0x18
};

/**
 * \~english @brief Class for reading BankFile objects.
 * \~japanese @brief BankFileオブジェクトを読み込むためのクラス。
 */
class BankFileReader {
public:
    static const u32 SIGNATURE = FOURCC('R', 'B', 'N', 'K');
    static const int VERSION = NW4R_VERSION(1, 2);
    static const u16 VERSION_SUPPORT_INST_VOLUME = NW4R_VERSION(1, 1);

public:
    explicit BankFileReader(const void* pBankBin);

    const BankFile::InstParam* GetInstParam(int prgNo, int key,
                                            int velocity) const;

    bool IsValidFileHeader(const void* pBankBin);

    bool ReadInstInfo(InstInfo* pInfo, int prgNo, int key, int velocity) const;
    bool ReadWaveInfo(WaveInfo* pParam,
                      const InstInfo::WaveDataLocation& rLocation,
                      const void* pWaveDataAddr,
                      const WaveInfo** ppWaveInfoAddr) const;
    bool ReadWaveParam(WaveData* pData, int waveIndex,
                       const void* pWaveAddr) const;

private:
    const BankFile::DataRegion*
    GetReferenceToSubRegion(const BankFile::DataRegion* pRef,
                            int splitKey) const;

private:
    const BankFile::Header* mHeader;       // at 0x0
    const BankFile::DataBlock* mDataBlock; // at 0x4
    const BankFile::WaveBlock* mWaveBlock; // at 0x8
};

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
