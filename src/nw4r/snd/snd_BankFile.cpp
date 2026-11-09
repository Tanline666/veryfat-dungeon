#include <nw4r/snd.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace snd {
namespace detail {

// See BankFile::Region
enum {
    DATATYPE_NONE = Util::DATATYPE_T0,
    DATATYPE_INSTPARAM = Util::DATATYPE_T1,
    DATATYPE_RANGETABLE = Util::DATATYPE_T2,
    DATATYPE_INDEXTABLE = Util::DATATYPE_T3,
};

bool BankFileReader::IsValidFileHeader(const void* pBankBin) {
    const ut::BinaryFileHeader* pFileHeader =
        static_cast<const ut::BinaryFileHeader*>(pBankBin);

    if (pFileHeader->signature != SIGNATURE) {
        return false;
    }

    if (pFileHeader->version < NW4R_VERSION(1, 0)) {
        return false;
    }

    if (pFileHeader->version > VERSION) {
        return false;
    }

    return true;
}

BankFileReader::BankFileReader(const void* bankData)
    : mHeader(NULL), mDataBlock(NULL), mWaveBlock(NULL) {

    NW4R_NULL_ASSERT(bankData, 93);

    if (!IsValidFileHeader(bankData)) {
        return;
    }

    mHeader = static_cast<const BankFile::Header*>(bankData);
    if (mHeader->dataBlockOffset) {
        mDataBlock = static_cast<const BankFile::DataBlock*>(
            ut::AddOffsetToPtr(mHeader, mHeader->dataBlockOffset));
    }
    if (mHeader->waveBlockOffset) {
        mWaveBlock = static_cast<const BankFile::WaveBlock*>(
            ut::AddOffsetToPtr(mHeader, mHeader->waveBlockOffset));
    }
}

const BankFile::InstParam* BankFileReader::GetInstParam(int prgNo, int key,
                                                        int velocity) const {
    NW4R_NULL_ASSERT(mHeader, 135);
    if (mHeader == NULL) {
        return NULL;
    }

    if (prgNo < 0 || prgNo >= (int)mDataBlock->instTable.count) {
        return NULL;
    }

    const BankFile::DataRegion* ref = &mDataBlock->instTable.item[prgNo];

    //! Key region info
    if (ref->dataType == BankFile::REGION_SET_NULL) {
        return NULL;
    }
    if (ref->dataType != BankFile::REGION_SET_DIRECT) {
        ref = GetReferenceToSubRegion(ref, key);
        if (!ref) {
            return NULL;
        }
    }

    //! Velocity region info
    if (ref->dataType == BankFile::REGION_SET_NULL) {
        return NULL;
    }
    if (ref->dataType != BankFile::REGION_SET_DIRECT) {
        ref = GetReferenceToSubRegion(ref, velocity);
        if (!ref) {
            return NULL;
        }
    }

    if (ref->dataType != BankFile::REGION_SET_DIRECT) {
        return NULL;
    }

    const BankFile::InstParam* instParam =
        Util::GetDataRefAddress1(*ref, &mDataBlock->instTable);
    return instParam;
}

bool BankFileReader::ReadInstInfo(InstInfo* instInfo, int prgNo, int key,
                                  int velocity) const {

    const BankFile::InstParam* instParam = GetInstParam(prgNo, key, velocity);

    if (instParam == NULL) {
        return false;
    }

    if (instParam->waveDataLocationType == InstInfo::WAVE_DATA_LOCATION_INDEX) {

        if (instParam->waveIndex < 0) {
            return FALSE;
        }
        instInfo->waveDataLocation.type = InstInfo::WAVE_DATA_LOCATION_INDEX;
        instInfo->waveDataLocation.index = instParam->waveIndex;

    } else if (instParam->waveDataLocationType ==
               InstInfo::WAVE_DATA_LOCATION_ADDRESS) {

        if (instParam->waveInfoAddress == NULL) {
            return FALSE;
        }
        instInfo->waveDataLocation.type = InstInfo::WAVE_DATA_LOCATION_ADDRESS;
        instInfo->waveDataLocation.address = instParam->waveInfoAddress;

    } else if (instParam->waveDataLocationType ==
               InstInfo::WAVE_DATA_LOCATION_CALLBACK) {

        if (instParam->waveDataLocationCallback == NULL) {
            return FALSE;
        }
        instInfo->waveDataLocation.type = InstInfo::WAVE_DATA_LOCATION_CALLBACK;
        instInfo->waveDataLocation.callback =
            instParam->waveDataLocationCallback;

    } else {
        NW4R_ASSERTMSG(FALSE, 210, "Invalid waveDataLocationType %d",
                       instParam->waveDataLocationType);
        return FALSE;
    }

    instInfo->attack = instParam->attack;
    instInfo->hold = instParam->hold;
    instInfo->decay = instParam->decay;
    instInfo->sustain = instParam->sustain;
    instInfo->release = instParam->release;
    instInfo->originalKey = instParam->originalKey;
    instInfo->pan = instParam->pan;

    if (mHeader->fileHeader.version >= VERSION_SUPPORT_INST_VOLUME) {
        instInfo->volume = instParam->volume;
        instInfo->tune = instParam->tune;
    } else {
        instInfo->volume = 127;
        instInfo->tune = 1.0f;
    }

    switch (instParam->noteOffType) {
    case NOTE_OFF_TYPE_RELEASE:
        instInfo->noteOffType = NOTE_OFF_TYPE_RELEASE;
        break;
    case NOTE_OFF_TYPE_IGNORE:
        instInfo->noteOffType = NOTE_OFF_TYPE_IGNORE;
        break;
    default:
        NW4R_ASSERTMSG(FALSE, 240, "Invalid noteOffType %d",
                       instParam->noteOffType);
        return FALSE;
    }

    instInfo->alternateAssign = instParam->alternateAssign;

    return TRUE;
}

const BankFile::DataRegion*
BankFileReader::GetReferenceToSubRegion(const BankFile::DataRegion* pRef,
                                        int splitKey) const {
    const BankFile::DataRegion* pSub = NULL;

    switch (pRef->dataType) {
    case DATATYPE_NONE: {
        break;
    }

    case DATATYPE_INSTPARAM: {
        pSub = pRef;
        break;
    }

    case DATATYPE_RANGETABLE: {
        const BankFile::RangeTable* pRangeTable =
            Util::GetDataRefAddress2(*pRef, &mDataBlock->instTable);

        if (pRangeTable == NULL) {
            return NULL;
        }

        int i = 0;
        while (splitKey > ReadByte(pRangeTable->key + i)) {
            if (++i >= pRangeTable->tableSize) {
                return NULL;
            }
        }

        const u8* pBase = reinterpret_cast<const u8*>(pRangeTable);
        u32 refOffset = i * sizeof(BankFile::DataRegion);
        u32 refStart = ut::RoundUp<u32>(pRangeTable->tableSize + 1, 4);

        pSub = reinterpret_cast<const BankFile::DataRegion*>(pBase + refOffset +
                                                             refStart);
        break;
    }

    case DATATYPE_INDEXTABLE: {
        const BankFile::IndexTable* pIndexTable =
            Util::GetDataRefAddress3(*pRef, &mDataBlock->instTable);

        if (pIndexTable == NULL) {
            return NULL;
        }

        if (splitKey < pIndexTable->min || splitKey > pIndexTable->max) {
            return NULL;
        }

        pSub = reinterpret_cast<const BankFile::DataRegion*>(
            pIndexTable->ref +
            (splitKey - pIndexTable->min) * sizeof(BankFile::DataRegion));
        break;
    }
    }

    return pSub;
}

bool BankFileReader::ReadWaveInfo(
    WaveInfo* waveParam, const InstInfo::WaveDataLocation& waveDataLocation,
    const void* waveDataAddress, const WaveInfo** waveInfoAddress) const {

    NW4R_NULL_ASSERT(waveParam, 331);

    if (waveInfoAddress) {
        *waveInfoAddress = NULL;
    }

    if (!mHeader) {
        return FALSE;
    }

    if (waveDataLocation.type == InstInfo::WAVE_DATA_LOCATION_INDEX) {
        int waveIndex = waveDataLocation.index;

        if (!mWaveBlock) {
            WaveArchiveReader waveArchiveReader(waveDataAddress);
            const WaveFile::FileHeader* fileHeader =
                waveArchiveReader.GetWaveFile(waveIndex);
            if (!fileHeader) {
                return FALSE;
            }

            WaveFileReader waveFileReader(fileHeader);
            return waveFileReader.ReadWaveInfo(waveParam);
        } else {
            if (waveIndex >= mWaveBlock->waveInfoTable.count) {
                return FALSE;
            }

            const WaveFile::WaveInfo* waveInfo = Util::GetDataRefAddress0(
                mWaveBlock->waveInfoTable.item[waveIndex],
                &mWaveBlock->waveInfoTable);
            if (!waveInfo) {
                return FALSE;
            }

            WaveFileReader waveFileReader(waveInfo);
            return waveFileReader.ReadWaveInfo(waveParam, waveDataAddress);
        }
    } else if (waveDataLocation.type == InstInfo::WAVE_DATA_LOCATION_ADDRESS) {
        if (!waveDataLocation.address) {
            return FALSE;
        }

        if (waveInfoAddress) {
            *waveInfoAddress = waveDataLocation.address;
        }

        *waveParam = *waveDataLocation.address;
        return TRUE;
    } else if (waveDataLocation.type == InstInfo::WAVE_DATA_LOCATION_CALLBACK) {
        if (!waveDataLocation.callback) {
            return FALSE;
        }

        const WaveInfo* waveInfo = waveDataLocation.callback->AttachWaveInfo();
        if (!waveInfo) {
            return FALSE;
        }
        if (waveInfoAddress) {
            *waveInfoAddress = waveInfo;
        }

        *waveParam = *waveInfo;
        return TRUE;
    } else {
        NW4R_ASSERTMSG(FALSE, 389, "Invalid waveDataLocation.type %d",
                       waveDataLocation.type);
        return FALSE;
    }
}

} // namespace detail
} // namespace snd
} // namespace nw4r
