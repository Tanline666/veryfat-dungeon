#include <nw4r/snd.h>
#include <nw4r/ut.h>

#include <revolution/DVD.h>

#include <cstring>

namespace nw4r {
namespace snd {

DvdSoundArchive::DvdSoundArchive() : mOpen(FALSE) {}

DvdSoundArchive::~DvdSoundArchive() {
    Close();
}

bool DvdSoundArchive::Open(s32 entrynum) {
    if (!DVDFastOpen(entrynum, &mFileInfo)) {
        return FALSE;
    }
    mOpen = TRUE;

    if (!LoadFileHeader()) {
        return FALSE;
    }

    return TRUE;
}

bool DvdSoundArchive::Open(const char* pPath) {
    s32 entrynum = DVDConvertPathToEntrynum(pPath);
    if (entrynum < 0) {
        return FALSE;
    }

    if (!Open(entrynum)) {
        return FALSE;
    }

    size_t length = std::strlen(pPath);
    for (int i = length - 1; i >= 0; i--) {
        const char sym = pPath[i];
        if (sym == '/' || sym == '\\') {
            char dirBuff[FILE_PATH_MAX];
            if (i >= FILE_PATH_MAX) {
                return FALSE;
            }
            std::strncpy(dirBuff, pPath, i);
            dirBuff[i] = '\0';
            SetExternalFileRoot(dirBuff);
            break;
        }
    }

    return TRUE;
}

void DvdSoundArchive::Close() {
    if (mOpen) {
        DVDClose(&mFileInfo);
        mOpen = FALSE;
    }
    Shutdown();
}

ut::FileStream* DvdSoundArchive::OpenStream(void* pBuffer, int size, u32 offset,
                                            u32 length) const {
    if (!mOpen) {
        return NULL;
    }

    if (size < sizeof(DvdFileStream)) {
        return NULL;
    }

    return new (pBuffer) DvdFileStream(&mFileInfo, offset, length);
}

ut::FileStream* DvdSoundArchive::OpenExtStream(void* pBuffer, int size,
                                               const char* pExtPath, u32 offset,
                                               u32 length) const {
    if (!mOpen) {
        return NULL;
    }

    if (size < sizeof(DvdFileStream)) {
        return NULL;
    }

    s32 entrynum = DVDConvertPathToEntrynum(pExtPath);
    if (entrynum < 0) {
        return NULL;
    }

    return new (pBuffer) DvdFileStream(entrynum, offset, length);
}

int DvdSoundArchive::detail_GetRequiredStreamBufferSize() const {
    return sizeof(DvdFileStream);
}

bool DvdSoundArchive::LoadFileHeader() {
    u8 headerArea[detail::SoundArchiveFile::HEADER_AREA_SIZE];

    const u32 headerAlignSize =
        ut::RoundUp(sizeof(detail::SoundArchiveFile::Header), 32);

    void* pFile = ut::RoundUp<u8>(headerArea, 32);

    s32 bytesRead =
        DVDReadPrio(&mFileInfo, pFile, headerAlignSize, 0, DVD_PRIO_MEDIUM);

    if (bytesRead != headerAlignSize) {
        return FALSE;
    }

    mFileReader.Init(pFile);
    Setup(&mFileReader);

    return TRUE;
}

bool DvdSoundArchive::LoadHeader(void* pBuffer, u32 size) {
    u32 infoSize = mFileReader.GetInfoChunkSize();
    s32 infoOffset = mFileReader.GetInfoChunkOffset();

    if (size < infoSize) {
        return FALSE;
    }

    s32 bytesRead =
        DVDReadPrio(&mFileInfo, pBuffer, infoSize, infoOffset, DVD_PRIO_MEDIUM);

    if (bytesRead != infoSize) {
        return FALSE;
    }

    mFileReader.SetInfoChunk(pBuffer, infoSize);
    return TRUE;
}

bool DvdSoundArchive::LoadLabelStringData(void* pBuffer, u32 size) {
    u32 labelSize = mFileReader.GetLabelStringChunkSize();
    s32 labelOffset = mFileReader.GetLabelStringChunkOffset();

    if (size < labelSize) {
        return FALSE;
    }

    s32 bytesRead = DVDReadPrio(&mFileInfo, pBuffer, labelSize, labelOffset,
                                DVD_PRIO_MEDIUM);

    if (bytesRead != labelSize) {
        return FALSE;
    }

    mFileReader.SetStringChunk(pBuffer, labelSize);
    return TRUE;
}

DvdSoundArchive::DvdFileStream::DvdFileStream(const DVDFileInfo* pFileInfo,
                                              u32 offset, u32 size)
    : DvdLockedFileStream(pFileInfo, FALSE), mOffset(offset), mSize(size) {
    if (mSize == 0) {
        mSize = ut::DvdFileStream::GetSize();
    }

    ut::DvdFileStream::Seek(mOffset, SEEK_ORIGIN_BEG);
}

DvdSoundArchive::DvdFileStream::DvdFileStream(s32 entrynum, u32 offset,
                                              u32 size)
    : DvdLockedFileStream(entrynum), mOffset(offset), mSize(size) {
    if (mSize == 0) {
        mSize = ut::DvdFileStream::GetSize();
    }

    ut::DvdFileStream::Seek(mOffset, SEEK_ORIGIN_BEG);
}

s32 DvdSoundArchive::DvdFileStream::Read(void* pDst, u32 size) {
    u32 endOffset = mOffset + mSize;
    u32 startOffset = ut::DvdFileStream::Tell();

    if (startOffset + size > endOffset) {
        size = ut::RoundUp(endOffset - ut::DvdFileStream::Tell(), 32);
    }

    return DvdLockedFileStream::Read(pDst, size);
}

void DvdSoundArchive::DvdFileStream::Seek(s32 offset, u32 origin) {
    switch (origin) {
    case SEEK_ORIGIN_BEG: {
        offset += mOffset;
        break;
    }

    case SEEK_ORIGIN_CUR: {
        offset += ut::DvdFileStream::Tell();
        break;
    }

    case SEEK_ORIGIN_END: {
        offset = mOffset + mSize - offset;
        break;
    }

    default: {
        return;
    }
    }

    if (offset < mOffset) {
        offset = mOffset;
    } else if (offset > mOffset + mSize) {
        offset = mOffset + mSize;
    }

    ut::DvdFileStream::Seek(offset, SEEK_ORIGIN_BEG);
}

} // namespace snd
} // namespace nw4r
