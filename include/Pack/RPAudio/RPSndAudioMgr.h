#ifndef RP_AUDIO_SND_AUDIO_MGR_H
#define RP_AUDIO_SND_AUDIO_MGR_H
#include <Pack/types_pack.h>

#include <Pack/RPAssert.h>
#include <Pack/RPAudio/RPSndMoveParam.h>
#include <Pack/RPAudio/RPSndStaticMgr.h>
#include <Pack/RPSingleton.h>

#include <egg/audio.h>

#include <nw4r/math.h>
#include <nw4r/snd.h>

#include <cstring>

//! @addtogroup rp_audio
//! @{

/**
 * \~english @brief Audio manager
 */
class RPSndAudioMgr : public EGG::ExpAudioMgr {
    RP_SINGLETON_DECL_EX(RPSndAudioMgr);

public:
    /**
     * \~english @brief Callback for configuring audio effects
     */
    typedef void (*SetFxCallback)();

public:
    /**
     * \~english @brief Initializes the audio manager state
     *
     * \~english @param pArg Audio system configuration
     */
    virtual void initialize(Arg* pArg); // at 0x8

    /**
     * \~english @brief Attempts to open the specified sound archive, using
     * storage settings from the project locale
     *
     * \~english @param pPath Sound archive path
     * \~english @param pHeap Sound heap
     * \~english @param pMultiHandle Multi handle for special storage types
     * \~english @return Archive player
     */
    virtual nw4r::snd::SoundArchivePlayer*
    openArchiveRP(const char* pPath, nw4r::snd::SoundHeap* pHeap,
                  void* pMultiHandle); // at 0xB0

    /**
     * \~english @brief Updates the state of the audio manager
     */
    virtual void calc(); // at 0xC

    /**
     * \~english @brief Attempts to restore the specified sound heap state
     *
     * \~english @param id State ID
     * \~english @return Success
     */
    virtual bool loadState(s32 id); // at 0x18

    /**
     * \~english @brief Attempts to load the specified sound group
     *
     * \~english @param id Group ID
     * \~english @param pHeap Sound heap to use for allocations
     * \~english @param blockSize Read block size (zero tries to read it all at
     * once)
     * \~english @return Success
     */
    virtual bool loadGroup(unsigned int id, nw4r::snd::SoundHeap* pHeap = NULL,
                           u32 blockSize = 0); // at 0x28

    /**
     * \~english @brief Attempts to load the specified sound group
     *
     * \~english @param id Group ID
     * \~english @param pHeap Sound heap to use for allocations
     * \~english @param blockSize Read block size (zero tries to read it all at
     * once)
     * \~english @return Success
     */
    virtual bool loadGroup(int id, nw4r::snd::SoundHeap* pHeap = NULL,
                           u32 blockSize = 0); // at 0x2C

    /**
     * \~english @brief Attempts to load the specified sound group
     *
     * \~english @param pName Group name
     * \~english @param pHeap Sound heap to use for allocations
     * \~english @param blockSize Read block size (zero tries to read it all at
     * once)
     * \~english @return Success
     */
    virtual bool loadGroup(const char* pName,
                           nw4r::snd::SoundHeap* pHeap = NULL,
                           u32 blockSize = 0); // at 0x30

    /**
     * \~english @brief Attempts to start the specified sound
     *
     * \~english @param pHandle Sound handle
     * \~english @param id Sound ID
     * \~english @return Success
     */
    virtual bool startSound(nw4r::snd::SoundHandle* pHandle, u32 id); // at 0x38

    /**
     * \~english @brief Attempts to start the specified sound
     *
     * \~english @param pHandle Sound handle
     * \~english @param id Sound ID
     * \~english @return Success
     */
    virtual bool startSound(nw4r::snd::SoundHandle* pHandle,
                            unsigned int id); // at 0x3C

    /**
     * \~english @brief Attempts to start the specified sound
     *
     * \~english @param pHandle Sound handle
     * \~english @param pName Sound name
     * \~english @return Success
     */
    virtual bool startSound(nw4r::snd::SoundHandle* pHandle,
                            const char* pName); // at 0x40

    /**
     * \~english @brief Attempts to open and prepare the specified sound archive
     *
     * \~english @param pPath Sound archive path
     * \~english @param staticPath Whether the archive path is relative to the
     * static resource directory
     * \~english @param pHeap Sound heap
     * \~english @return Archive player
     */
    nw4r::snd::SoundArchivePlayer* attachArchive(const char* pPath,
                                                 bool staticPath,
                                                 nw4r::snd::SoundHeap* pHeap);

    /**
     * \~english @brief Attempts to start the specified BGM ("BGM_") sound
     *
     * \~english @param id Sound ID
     * \~english @return Success
     */
    bool startBgm(u32 id) {
        return startSound(&mHandleBgm, id);
    }
    /**
     * \~english @brief Attempts to start the specified BGM ("BGM_") sound
     *
     * \~english @param pName Sound name
     * \~english @return Success
     */
    bool startBgm(const char* pName) {
        return startBgm(changeNameToIdRP(pName));
    }

    /**
     * \~english @brief Attempts to start the specified stream ("STRM_") sound
     *
     * \~english @param id Sound ID
     * \~english @return Success
     */
    bool startStream(u32 id) {
        return startSound(&mHandleStream, id);
    }
    /**
     * \~english @brief Attempts to start the specified stream ("STRM_") sound
     *
     * \~english @param pName Sound name
     * \~english @return Success
     */
    bool startStream(const char* pName) {
        return startStream(changeNameToIdRP(pName));
    }

    /**
     * \~english @brief Attempts to start the specified system sound effect
     *
     * \~english @param id Sound ID
     * \~english @return Success
     */
    bool startSystemSe(u32 id) {
        return startSystemSe(getSoundArchive().GetSoundLabelString(id));
    }
    /**
     * \~english @brief Attempts to start the specified system sound effect
     *
     * \~english @param pName Sound name
     * \~english @return Success
     */
    bool startSystemSe(const char* pName) {
        if (mIsDisableFanfare) {
            if (pName[7] == 'B' && pName[8] == 'G' && pName[9] == 'M') {
                return false;
            }
        }

        return startSound(&mHandleSystemSe, pName);
    }

    /**
     * \~english @brief Attempts to start the specified system sound effect
     * through the remote speaker(s)
     *
     * \~english @param id Sound ID
     * \~english @param playerFlag Player output line flags
     * \~english @return Success
     */
    bool startSystemSeWithRemote(u32 id, u32 playerFlag) {
        bool success = startSound(&mHandleSystemSe, id);
        if (success) {
            setRemote(playerFlag);
        }

        return success;
    }
    /**
     * \~english @brief Attempts to start the specified system sound effect
     * through the remote speaker(s)
     *
     * \~english @param pName Sound name
     * \~english @param playerFlag Player output line flags
     * \~english @return Success
     */
    bool startSystemSeWithRemote(const char* pName, u32 playerFlag) {
        return startSystemSeWithRemote(changeNameToIdRP(pName), playerFlag);
    }

    /**
     * \~english @brief Attempts to start the specified wave sound
     *
     * \~english @param id Sound ID
     * \~english @return Success
     */
    bool startWaveSound(u32 id) {
        return startWaveSound(getSoundArchive().GetSoundLabelString(id));
    }
    /**
     * \~english @brief Attempts to start the specified wave sound
     *
     * \~english @param pName Sound name
     * \~english @return Success
     */
    bool startWaveSound(const char* pName) {
        if (pName[0] == 'B' && pName[1] == 'g' && pName[2] == 'm') {
            return false;
        }

        return startSound(&mHandleWaveSound, pName);
    }

    /**
     * \~english @brief Converts a sound effect name to its corresponding ID
     *
     * \~english @param pName Sound effect name
     * \~english @return Sound effect ID
     */
    u32 changeNameToIdRP(const char* pName) {
        u32 id = mArchive->ConvertLabelStringToSoundId(pName);

#line 206
        RP_ASSERT_MSG(id != nw4r::snd::SoundArchive::INVALID_ID, "Wrong Sound Name < %s >\n", pName);
        return id;
    }

    /**
     * \~english @brief Configures the audio system for title scenes
     */
    void configureTitle();

    /**
     * \~english @brief Configures the audio system for scenes without BGM
     */
    void configureNoBgm();

    /**
     * \~english @brief Configures the audio system for game scenes
     *
     * \~english @param cameraNum 3D audio camera count
     */
    void configureMiniGame(s32 cameraNum);

    /**
     * \~english @brief Fades out all audio in the specified number of frames
     *
     * \~english @param frames Fade out length
     */
    void fadeOutAllSound(u16 frames);

    /**
     * \~english @brief Applies the fade out for game Retry buttons
     */
    void setAutoFadeOnRetry();

    /**
     * \~english @brief Prepares the audio system for a scene change
     */
    void changeScene();

    /**
     * \~english @brief Resets the audio system configuration
     */
    void exitAudio();

    /**
     * \~english @brief Sets the volume of all sound players
     *
     * \~english @param volume New volume
     * \~english @param frames Interpolation time
     */
    void setArcVolume(f32 volume, u32 frames);

    /**
     * \~english @brief Resets the volume of all sound players
     */
    void resetArcVolume();

    /**
     * \~english @brief Sets the volume of the current BGM
     *
     * \~english @param volume New volume
     * \~english @param frames Interpolation time
     */
    void setBgmVolume(f32 volume, int frames);

    /**
     * \~english @brief Sets the volume of the current stream
     *
     * \~english @param volume New volume
     * \~english @param frames Interpolation time
     */
    void setStreamVolume(f32 volume, int frames);

    /**
     * \~english @brief Gets the position of the current stream, in seconds
     */
    u32 getStreamPlaySecond();

    /**
     * \~english @brief Attempts to start a cursor sound effect
     *
     * \~english @param pName Sound name
     * \~english @param rDpdPos Cursor DPD/IR position
     * \~english @param pan Sound pan
     * \~english @param playerFlag Player output line flag
     * \~english @return Success
     */
    bool startPointCursorSe(const char* pName, const nw4r::math::VEC2& rDpdPos,
                            f32 pan, u32 playerFlag);

    /**
     * \~english @brief Attempts to start a pause sound effect
     *
     * \~english @param pNameRemote Remote speaker sound name
     * \~english @param pNameTV TV speaker sound name
     * \~english @param waitForDpd Whether to wait until the controller
     * reconnects
     * \~english @param player Player whose remote speaker should be used
     */
    void startPauseSe(const char* pNameRemote, const char* pNameTV,
                      bool waitForDpd, s32 player);

    /**
     * \~english @brief Applies the default audio effect configuration
     */
    void setDefaultFx();

    /**
     * \~english @brief Configures the specified remote speaker(s) for the
     * current system sound effect
     *
     * \~english @param playerFlag Player output line flags
     */
    void setRemote(u32 playerFlag);

    /**
     * \~english @brief Prepares the audio system for entering the home button
     * menu
     */
    void enterMenu();
    /**
     * \~english @brief Prepares the audio system for returning from the home
     * button menu
     */
    void exitMenu();

    /**
     * \~english @brief Prepares the audio system for entering the pause menu
     */
    void enterPause();
    /**
     * \~english @brief Prepares the audio system for returning from the pause
     * menu
     */
    void exitPause();

    /**
     * \~english @brief Pauses the audio system when a DVD error occurs
     */
    void onDvdError();
    /**
     * \~english @brief Resumes the audio system after a DVD error
     */
    void exitDvdError();

    /**
     * \~english @brief Stops all sounds when exiting the scene from the pause
     * menu
     */
    void stopAllSoundOnPauseOff();

    /**
     * \~english @brief Requests the audio system to shutdown
     *
     * \~english @param frames Time to fade out the master volume
     */
    void shutdownSystem(int frames);

    /**
     * \~english @brief Requests the audio system to reset
     *
     * \~english @param frames Time to fade out the archive volume
     */
    void resetSystem(int frames);

    /**
     * \~english @brief Attempts to start the specified fanfare sound effect
     *
     * \~english @param id Sound ID
     * \~english @return Success
     */
    bool startFanfare(u32 id);
    /**
     * \~english @brief Attempts to start the specified fanfare sound effect
     *
     * \~english @param pName Sound name
     * \~english @return Success
     */
    bool startFanfare(const char* pName);

    /**
     * \~english @brief Gets the user parameters attached to the specified sound
     *
     * \~english @param rArchive Sound archive
     * \~english @param id Sound ID
     */
    u32 getUserParam(const nw4r::snd::SoundArchive& rArchive, u32 id);

    /**
     * \~english @brief Sets the output mode of the audio system
     *
     * \~english @param confMode Sound mode from the SYSCONF
     */
    void setOutputMode(u8 confMode);

    /**
     * \~english @brief Gets the number of game players
     */
    u32 getPlayerNum() const {
        return mPlayerNum;
    }

    /**
     * \~english @brief Accesses the static sound archive manager
     */
    RPSndStaticMgr& getStaticMgr() {
        return mStaticMgr;
    }

private:
    /**
     * \~english @brief Audio manager flags
     */
    enum {
        EFlag_IsPause = 1 << 0,    //!< Pause state was entered
        EFlag_IsMenu = 1 << 1,     //!< Menu state was entered
        EFlag_IsDvdError = 1 << 2, //!< DVD error state was entered
    };

    /**
     * \~english @brief Audio manager state
     */
    enum EState {
        EState_Exited,   //!< Needs configuration for the current scene
        EState_Title,    //!< Configured for a title scene
        EState_MiniGame, //!< Configured for a minigame scene
        EState_NoBgm,    //!< Configured for a scene without BGM
    };

    //! Maximum file path length
    static const u32 PATH_MAX = 128;

    //! Time to fadeout all sounds
    static const u32 RETRY_FADE_TIME = 60;
    //! Time to fade in when exiting the pause/HBM menu
    static const u32 EXIT_FADE_TIME = 20;

    //! Time to clear audio bus effects
    static const u32 MENU_FX_CLEAR_TIME = 15;

    //! Stream sound tickrate
    static const u32 STREAM_TICKRATE = 60;

private:
    /**
     * \~english @brief Sets the specified bitflags
     *
     * \~english @param flag Bitflags
     */
    void setFlag(u8 flag) {
        mFlags |= flag;
    }
    /**
     * \~english @brief Clears the specified bitflags
     *
     * \~english @param flag Bitflags
     */
    void clearFlag(u8 flag) {
        mFlags &= static_cast<u8>(~flag);
    }
    /**
     * \~english @brief Tests whether the specified bitflags are set
     *
     * \~english @param flag Bitflags
     */
    bool testFlag(u8 flag) {
        return mFlags & flag;
    }

    /**
     * \~english @brief Gets the playtime of the specified fanfare sound effect,
     * in milliseconds
     *
     * \~english @param pName Fanfare sound effect name
     */
    u32 getFanfareLength(const char* pName) {
        if (std::strcmp("RP_COM_BGM_GET_COPPER", pName) == 0) {
            return 1900;
        }
        if (std::strcmp("RP_COM_BGM_GET_SILVER", pName) == 0) {
            return 3400;
        }
        if (std::strcmp("RP_COM_BGM_GET_GOLD", pName) == 0) {
            return 3100;
        }
        if (std::strcmp("RP_COM_BGM_GET_PLATINA", pName) == 0) {
            return 4900;
        }

        if (std::strcmp("RP_COMMON_BGM_RANK_COMPLETE", pName) == 0) {
            return 3300;
        }

        if (std::strcmp("RP_PNP_BGM_FANFARE_WIN", pName) == 0) {
            return 3000;
        }

        if (std::strcmp("FANFARE_HomeRun", pName) == 0) {
            return 1736;
        }
        if (std::strcmp("FANFARE_OutOfTheParkHomer", pName) == 0) {
            return 2576;
        }

        if (std::strcmp("FANFARE_Strike", pName) == 0) {
            return 1760;
        }
        if (std::strcmp("FANFARE_SpareGet", pName) == 0) {
            return 2192;
        }

        // Assume it runs until the sound is detached
        return 0xFFFFFFFF;
    }

private:
    //! Sound archive file path
    char mArchivePath[PATH_MAX]; // at 0x8A0

    //! BGM sound handle
    nw4r::snd::SoundHandle mHandleBgm; // at 0x920
    //! Stream sound handle
    nw4r::snd::SoundHandle mHandleStream; // at 0x924
    //! System sound effect handle
    nw4r::snd::SoundHandle mHandleSystemSe; // at 0x928
    //! Wave sound handle
    nw4r::snd::SoundHandle mHandleWaveSound; // at 0x92C
    //! Fanfare sound handle
    nw4r::snd::SoundHandle mHandleFanfare; // at 0x930

    //! Current sound heap level
    s32 mSoundHeapLevel; // at 0x934
    //! Shutdown status
    EShutDownStatus mShutDownStatusRP; // at 0x938

    //! Audio manager flags
    u8 mFlags; // at 0x93C
    u32 unk940;

    //! Current stream sound tick
    u32 mStreamPlayTick; // at 0x944
    u32 unk948;

    //! Sound archive volume
    RPSndMoveValueF32 mArcVolume; // at 0x94C

    //! Number of game players
    u32 mPlayerNum; // at 0x968

    //! Time until the audio system is reset
    u32 mResetTimer; // at 0x96C

    //! Whether fanfare sounds are disabled
    bool mIsDisableFanfare; // at 0x970

    //! Static sound manager
    RPSndStaticMgr mStaticMgr; // at 0x974

    //! Whether a pause sound effect task is queued
    bool mIsPauseSe; // at 0xF7C
    //! Whether the pause sound effect should wait for the controller
    bool mPauseSeWaitForDpd; // at 0xF7D
    //! Whether a DVD error has just occurred
    bool mIsNewDvdError; // at 0xF7E
    //! Previous DPD enable state when the pause sound effect was requested
    bool mPauseSePrevDpdEnable; // at 0xF7F
    //! Remote speaker pause sound effect name
    const char* mpPauseSeNameRemote; // at 0xF80
    //! TV speaker pause sound effect name
    const char* mpPauseSeName; // at 0xF84

    //! Fanfare start time
    u32 mFanfareStart; // at 0xF88
    //! Pause sound effect player output line flags
    u32 mPauseSePlayerFlag; // at 0xF8C
    //! Pause sound effect channel
    u32 mPauseSeChan; // at 0xF90
    //! Fanfare sound effect length, in milliseconds
    u32 mFanfareLength; // at 0xF94

    //! User-set fade time to use instead of the default
    u32 mMenuPauseFrame; // at 0xF98
    //! Time until an exit fade in is triggered
    u32 mMenuPauseFadeInTimer; // at 0xF9C

    //! Callback for re-configuring audio effects
    SetFxCallback mpSetFxCallback; // at 0xFA0

    //! BGM volume control
    RPSndMoveParam* mpBgmVolume; // at 0xFA4

    //! Audio system state
    EState mState; // at 0xFA8
};

//! @}

#endif
