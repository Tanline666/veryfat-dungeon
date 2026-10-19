#ifndef RP_KERNEL_SYSTEM_H
#define RP_KERNEL_SYSTEM_H
#include <Pack/types_pack.h>

#include <Pack/RPSystem/RPSysRenderMode.h>
#include <Pack/RPSystem/RPSysSceneCreator.h>

#include <egg/core.h>

#include <nw4r/ut.h>

#include <revolution/GX.h>
#include <revolution/OS.h>

//! @addtogroup rp_system
//! @{

/**
 * \~english @brief Pack Project configuration data. Derives from
 * EGG::ConfigurationData.
 */

class RPSysConfigData {
    friend class RPSysSystem;

public:
    explicit RPSysConfigData(u32 systemHeapSize)
        : mSystemHeapSize(systemHeapSize) {}

    virtual EGG::Video* GetVideo() {
        return mVideo;
    }
    virtual EGG::Heap* GetSystemHeap() {
        return mSystemHeap;
    }
    virtual EGG::Display* GetDisplay() {
        return mDisplay;
    }
    virtual EGG::XfbManager* GetXfbMgr() {
        return mXfbMgr;
    }
    virtual EGG::PerformanceView* GetPerfView() {
        return mPerfView;
    }
    virtual EGG::SceneManager* GetSceneMgr() {
        return mSceneMgr;
    }
    virtual EGG::IAudioMgr* GetAudioMgr() {
        return mAudioMgr;
    }
    virtual void StubA() {}
    virtual void StubB() {}

private:
    void* mMem1Start;
    void* mMem1End;
    void* mMem2Start;
    void* mMem2End;
    u32 mMemSize;
    EGG::Heap* mRootHeapMem1;
    EGG::Heap* mRootHeapMem2;
    EGG::Heap* mRootHeapDebug;

protected:
    EGG::Heap* mSystemHeap;
    EGG::Thread* mCreatorThread;
    OSBootInfo* mCodeStart;
    void* mCodeEnd;
    u32 mSystemHeapSize;
    EGG::Video* mVideo;
    EGG::XfbManager* mXfbMgr;
    EGG::Display* mDisplay;
    EGG::PerformanceView* mPerfView;
    EGG::SceneManager* mSceneMgr;
    EGG::IAudioMgr* mAudioMgr;
};

/**
 * \~english @brief Pack Project engine system
 */
class RPSysSystem {
    friend class RPSysConfigData;

public:
    /**
     * \~english @brief Gets this class's singleton instance
     */
    static RPSysSystem* instance() {
        return spInstance;
    }

    /**
     * \~english @brief Auto-detects the screen's aspect ratio
     */
    static void setupTVMode();
    /**
     * \~english @brief Auto-detects the render mode and progressive setting
     */
    static void setupRenderMode();

    /**
     * \~english @brief Initializes the RP engine and its core components
     */
    static void initialize();

    /**
     * \~english @brief Creates this class's singleton instance
     */
    static void create();

    /**
     * \~english @brief Gets the width of the framebuffer, in pixels
     */
    u16 getFBWidth() const;
    /**
     * \~english @brief Gets the height of the framebuffer, in pixels
     */
    u16 getFBHeight() const;

    /**
     * \~english @brief Loads the Pack Project build timestamp from the disc
     */
    void createTimeStamp();
    /**
     * \~english @brief Creates a work buffer for the effect manager
     */
    void createEffectWork();
    /**
     * \~english @brief Creates the engine's core components using the system
     * heap
     */
    void createFromSystemHeap();

    /**
     * \~english @brief Prepares the engine for the first frame
     */
    void setup();
    /**
     * \~english @brief Enters the engine's main loop
     */
    void mainLoop();

    /**
     * \~english @brief Gets the ID of the boot scene
     */
    RPSysSceneCreator::ESceneID getBootScene();

    /**
     * \~english @brief Gets the current clock time represented as a 32-bit
     * calendar time
     */
    RPTime32 getCalendarTime32();
    /**
     * \~english @brief Gets the current clock time represented as a 16-bit
     * calendar time
     */
    RPTime16 getCalendarTime16();

    /**
     * \~english @brief Splits 32-bit calendar time into parts
     * \~ @note If no input time is specified, this function operates on the
     * current clock time.
     *
     * \~english @param pTime32 Input time
     * \~english @param rWDay Day of the week
     * \~english @param rYear Year
     * \~english @param rMon Month (one-indexed)
     * \~english @param rMDay Day of the month
     * \~english @param rHour Hour
     */
    void getCalendarTime(const RPTime32* pTime32, u32& rWDay, u32& rYear,
                         u32& rMon, u32& rMDay, u32& rHour);

    /**
     * \~english @brief Splits 16-bit calendar time into parts
     * \~ @note If no input time is specified, this function operates on the
     * current clock time.
     *
     * \~english @param pTime16 Input time
     * \~english @param rYear Year
     * \~english @param rMon Month (one-indexed)
     * \~english @param rMDay Day of the month
     */
    void getCalendarTime(const RPTime16* pTime16, u32& rYear, u32& rMon,
                         u32& rMDay);

    /**
     * \~english @brief Shuts down the engine and exits the application
     * @details Depending on the instructions from the scene manager, this will
     * either exit to the system menu or shutdown the console.
     */
    void systemShutDown();

    /**
     * \~english @brief Updates the system state during resource loading
     */
    void loadFrameWork();

    /**
     * \~english @brief Gets the number of ticks per frame
     */
    u8 getFrameRate() const;
    /**
     * \~english @brief Gets the modify ratio of the display
     */
    f32 getModifyRatio() const;

    /**
     * \~english @brief Sets the background clear color of the EFB
     *
     * \~english @param color Clear color
     */
    void setDisplayClearColor(nw4r::ut::Color color);

    /**
     * \~english @brief Destroys the specified heap
     *
     * \~english @param pHeap Heap to destroy
     */
    void destroyHeap(EGG::Heap* pHeap);

    /**
     * \~english @brief Allocates memory from the specified heap
     *
     * \~english @param pHeap Heap to allocate from
     * \~english @param size Amount of memory required
     * \~english @param align Required byte-alignment
     * \~english @return void* Allocated memory block
     */
    void* alloc(EGG::Heap* pHeap, u32 size, s32 align = 4) {
        return EGG::Heap::alloc(size, align, pHeap);
    }

    /**
     * \~english @brief Frees memory to the specified heap
     *
     * \~english @param pHeap Heap to free to
     * \~english @param pBlock Memory block to free
     */
    void free(EGG::Heap* pHeap, void* pBlock) {
        return EGG::Heap::free(pBlock, pHeap);
    }

    /**
     * \~english @brief Initializes the load count to zero frames
     */
    void startLoadCount();
    /**
     * \~english @brief Tests whether the current load count is over five
     * seconds (600f)
     */
    bool isLoadCountEnd() const;

    void returnToMenu();

    /**
     * \~english @brief Turns on/off screen dimming
     * \~ @note If "Screen Saver Mode" is disabled, dimming is always OFF.
     *
     * \~english @param dim Whether to dim the screen
     */
    void setDimming(BOOL dim);

    /**
     * \~english @brief Gets the heap for the resource manager
     */
    EGG::Heap* getResourceHeap() const {
        return mpResourceHeap;
    }

    /**
     * \~english @brief Sets how many minutes it will take until
     * the remote powers off when left idle
     *
     * \~english @param time Minutes
     */
    void setAutoSleepTime(u8 time);

    /**
     * \~english @brief Gets the thread for asynchronous NAND operations
     */
    EGG::TaskThread* getNandThread();
    /**
     * \~english @brief Gets the thread for asynchronous disc operations
     */
    EGG::TaskThread* getDvdThread();
    /**
     * \~english @brief Gets the thread for asynchronous WiiConnect24 operations
     */
    EGG::TaskThread* getWc24Thread();

    /**
     * \~english @brief Gets the task-end thread message for asynchronous NAND
     * operations
     */
    OSMessage getNandEndMessage() const;
    /**
     * \~english @brief Gets the task-end thread message for asynchronous disc
     * operations
     */
    OSMessage getDvdEndMessage() const;

    /**
     * \~english @brief Gets the Pack Project build information
     */
    const char* getTimeStampString();

    /**
     * \~english @brief Auto-detects project locale settings
     */
    void setupLocalSettings();

    /**
     * \~english @brief Blocks the main thread until it is safe to change the
     * render mode
     * @details Waits for either 100 frames or 1.6 seconds to pass.
     *
     * @history This arbitrary 1.6 second delay is likely from performance
     * metrics taken during development. This code exists in other EAD games
     * (notably Big Brain Academy and Mario Kart Wii), where the delay is
     * slightly changed.
     */
    void waitRenderModeChange();

    /**
     * \~english @brief Installs reset and shutdown callbacks to the OS
     */
    void setCallBack();

private:
    /**
     * \~english @brief Constructor
     */
    RPSysSystem();
    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysSystem(); // at 0x8

    /**
     * \~english @brief OS reset callback
     */
    static void softResetCallBack();
    /**
     * \~english @brief OS shutdown callback
     */
    static void shutdownSystemCallBack();

private:
    //! Heap used by the resource manager
    EGG::Heap* mpResourceHeap; // at 0x4
    //! Heap reserving memory in the MEM1 region
    EGG::Heap* mpReserveHeap; // at 0x8
    //! Heap used for error handling when other heaps are exhausted
    EGG::Heap* mpAssertHeap; // at 0xC
    //! @unused
    EGG::Heap* mpDebugHeap; // at 0x10

    //! Backup of the EGG current heap while it is changed
    EGG::Heap* mpCurrentHeap; // at 0x14
    u32 mUNK_0x18;
    u32 mUNK_0x1C;
    u32 mUNK_0x20;
    u32 mUNK_0x24;
    //! Mutex to guard changing the current heap
    OSMutex mCurrentHeapMutex; // at 0x28

    //! Thread for asynchronous NAND operations
    EGG::TaskThread* mpNandThread; // at 0x40
    //! Thread for asynchronous disc operations
    EGG::TaskThread* mpDvdThread; // at 0x44

    //! Task-end thread message for asynchronous NAND operations
    u32 mNandEndMessage; // at 0x48
    //! Task-end thread message for asynchronous disc operations
    u32 mDvdEndMessage; // at 0x4C

    //! Determines if power operations are safe or have been triggered
    //! (tentative)
    bool mPowerFlag; // at 0x50
    //! Number of ticks per frame
    u8 mFrameRate; // at 0x51
    //! Average frame duration, in seconds (assumes 60fps)
    f32 mFrameTime; // at 0x54
    //! Total number of frames spent loading resources
    u32 mLoadCount; // at 0x58

    //! Pack Project build information
    char* mpTimeStampString; // at 0x5C

    //! Engine configuration
    static RPSysConfigData sConfigData;

    //! Active render mode
    static GXRenderModeObj* spRenderModeObj;
    //! Render mode format
    static RPSysRenderMode::EFormat sRenderModeFormat;
    //! @brief Time (in milliseconds) when the render mode was setup
    //! @remark This value is only non-zero when scan mode/TV format is changed
    //! and the system must wait.
    static u32 sRenderModeWaitStart;

    //! Class singleton instance
    static RPSysSystem* spInstance;
};

//! @}

#endif
