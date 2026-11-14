#include <nw4r/ut.h>

namespace nw4r {
namespace ut {
namespace {

class LCImpl {
private:
    class Lock_ {
    public:
        Lock_(LCImpl& impl) : mMutex(impl.mMutex) {
            OSLockMutex(&mMutex);
        }
        ~Lock_() {
            OSUnlockMutex(&mMutex);
        }

    private:
        OSMutex& mMutex; // at 0x0
    };

    bool IsEnabled() const {
        return mEnabled;
    }

public:
    LCImpl() : mEnabled(FALSE) {
        OSInitMutex(&mMutex);
    }

    void Enable() {
        Lock_ lock(*this);

        if (!mEnabled) {
            LCEnable();
            mEnabled = TRUE;
        }
    }

    void Disable() {
        Lock_ lock(*this);

        if (mEnabled) {
            LC::QueueWaitEx(0);
            LCDisable();
            mEnabled = FALSE;
        }
    }

    bool Lock() {
        OSLockMutex(&mMutex);

        if (mEnabled) {
            LC::QueueWaitEx(0);
            return TRUE;
        }

        OSUnlockMutex(&mMutex);
        return FALSE;
    }

    void Unlock() {
        NW4R_ASSERT(mEnabled, 89);
        LC::QueueWaitEx(0);
        OSUnlockMutex(&mMutex);
    }

    void LoadBlocks(void* destTag, void* srcAddr, u32 numBlocks) {
#if defined(DEBUG)
        Lock_ lock(*this);
        NW4R_ASSERT(IsEnabled(), 98);
#endif
        LCLoadBlocks(destTag, srcAddr, numBlocks);
    }

    void StoreBlocks(void* destAddr, void* srcTag, u32 numBlocks) {
#if defined(DEBUG)
        Lock_ lock(*this);
        NW4R_ASSERT(IsEnabled(), 116);
#endif
        LCStoreBlocks(destAddr, srcTag, numBlocks);
    }

    u32 StoreData(void* destAddr, void* srcAddr, u32 nBytes) {
        return LCStoreData(destAddr, srcAddr, nBytes);
    }

private:
    bool mEnabled;  // at 0x0
    OSMutex mMutex; // at 0x4
};

static LCImpl sLCImpl;

} // namespace

void LC::Enable() {
    sLCImpl.Enable();
}

void LC::Disable() {
    sLCImpl.Disable();
}

bool LC::Lock() {
    return sLCImpl.Lock();
}

void LC::Unlock() {
    sLCImpl.Unlock();
}

void LC::LoadBlocks(void* pDst, void* pSrc, u32 blocks) {
    sLCImpl.LoadBlocks(pDst, pSrc, blocks);
}

void LC::StoreBlocks(void* pDst, void* pSrc, u32 blocks) {
    sLCImpl.StoreBlocks(pDst, pSrc, blocks);
}

u32 LC::StoreData(void* pDst, void* pSrc, u32 size) {
    return sLCImpl.StoreData(pDst, pSrc, size);
}

} // namespace ut
} // namespace nw4r
