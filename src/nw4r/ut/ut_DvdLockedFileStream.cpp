#include <nw4r/ut.h>

#include <revolution/OS.h>

namespace nw4r {
namespace ut {

NW4R_UT_RTTI_DEF_DERIVED(DvdLockedFileStream, DvdFileStream);

OSThreadQueue DvdLockedFileStream::sThreadQueue;
bool DvdLockedFileStream::sInitialized = false;
OSMutex DvdLockedFileStream::sMutex;

void DvdLockedFileStream::InitMutex_() {
    BOOL enabled = OSDisableInterrupts();

    if (!sInitialized) {
        OSInitMutex(&sMutex);
        OSInitThreadQueue(&sThreadQueue);
        sInitialized = true;
    }

    OSRestoreInterrupts(enabled);
}

DvdLockedFileStream::DvdLockedFileStream(s32 entrynum)
    : DvdFileStream(entrynum), mCancelFlag(FALSE) {
    InitMutex_();
}

DvdLockedFileStream::DvdLockedFileStream(const DVDFileInfo* pInfo, bool close)
    : DvdFileStream(pInfo, close), mCancelFlag(FALSE) {
    InitMutex_();
}

DvdLockedFileStream::~DvdLockedFileStream() {}

void DvdLockedFileStream::Close() {
    DvdFileStream::Close();
    mCancelFlag = FALSE;
}

s32 DvdLockedFileStream::Read(void* pDst, u32 size) {
    if (!LockMutex()) {
        return DVD_RESULT_CANCELED;
    }
    s32 result = DvdFileStream::Read(pDst, size);
    UnlockMutex();
    return result;
}

s32 DvdLockedFileStream::Peek(void* pDst, u32 size) {
    if (!LockMutex()) {
        return DVD_RESULT_CANCELED;
    }
    s32 result = DvdFileStream::Peek(pDst, size);
    UnlockMutex();
    return result;
}

void DvdLockedFileStream::Cancel() {
    CancelMutex();
    DvdFileStream::Cancel();
}

bool DvdLockedFileStream::LockMutex() {
    BOOL enabled = OSDisableInterrupts();

    while (!OSTryLockMutex(&sMutex)) {
        OSSleepThread(&sThreadQueue);
        if (mCancelFlag) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

void DvdLockedFileStream::UnlockMutex() {
    BOOL enabled = OSDisableInterrupts();

    OSUnlockMutex(&sMutex);
    OSWakeupThread(&sThreadQueue);

    OSRestoreInterrupts(enabled);
}

void DvdLockedFileStream::CancelMutex() {
    BOOL enabled = OSDisableInterrupts();

    mCancelFlag = TRUE;
    OSWakeupThread(&sThreadQueue);

    OSRestoreInterrupts(enabled);
}

} // namespace ut
} // namespace nw4r
