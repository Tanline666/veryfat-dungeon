#ifndef NW4R_UT_DVD_LOCKED_FILE_STREAM_H
#define NW4R_UT_DVD_LOCKED_FILE_STREAM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/DvdFileStream.h>

#include <revolution/OS.h>

namespace nw4r {
namespace ut {

/**
 * @~english @brief Class for ensuring that synchronous DVD reads from
 * high-priority threads are not interrupted by requests from low-priority
 * threads.
 * @~japanese @brief 優先度の低いスレッドのリード予約が、優先度の高いスレッドの
 * 同期DVDリードを割り込むのを防止するためのクラス。
 */
class DvdLockedFileStream : public DvdFileStream {
public:
    NW4R_UT_RTTI_DECL(DvdLockedFileStream);

public:
    /**
     * @~english @brief Constructor
     * @~japanese @brief コンストラクタ
     *
     * @~english @param entryNum Entry number of the file to be opened
     * @~japanese @param entryNum オープンされるファイルのエントリ番号
     */
    explicit DvdLockedFileStream(s32 entryNum);
    explicit DvdLockedFileStream(const DVDFileInfo* openedFile,
                                 bool closeEnable = TRUE);
    /**
     * @~english @brief Destructor
     * @~japanese @brief デストラクタ
     */
    virtual ~DvdLockedFileStream(); // at 0xC

    virtual void Close();

    virtual s32 Read(void* pDst, u32 size); // at 0x14
    virtual s32 Peek(void* pDst, u32 size); // at 0x5C

    virtual bool ReadAsync(void* /* pDst */, u32 /* size */,
                           StreamCallback /* pCallback */,
                           void* /* pCallbackArg */) {

        return false;
    } // at 0x18

    virtual bool PeekAsync(void* /* pDst */, u32 /* size */,
                           StreamCallback /* pCallback */,
                           void* /* pCallbackArg */) {

        return false;
    } // at 0x60

    virtual void Cancel();

    virtual bool CanAsync() const {
        return false;
    } // at 0x28

private:
    static OSThreadQueue sThreadQueue;

    void InitMutex_();
    bool LockMutex();
    void UnlockMutex();
    void CancelMutex();

private:
    volatile bool mCancelFlag; // at 0x6F

    static bool sInitialized;
    static OSMutex sMutex;
};

} // namespace ut
} // namespace nw4r
#endif
