#ifndef RP_AUDIO_SND_SPEAKER_MGR_H
#define RP_AUDIO_SND_SPEAKER_MGR_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>

#include <egg/core.h>

#include <nw4r/snd.h>

//! @addtogroup rp_audio
//! @{

/**
 * \~english @brief Remote speaker manager
 */
class RPSndSpeakerMgr : public EGG::Disposer {
    RP_SINGLETON_DECL_EX(RPSndSpeakerMgr);

public:
    /**
     * \~english @brief Resets the state of each remote speaker
     */
    void reset();

    /**
     * \~english @brief Updates the state of each remote speaker
     */
    void calc();

    /**
     * \~english @brief Finalizes the state of each remote speaker
     */
    void exit();

    /**
     * \~english @brief Fetches the latest WPAD master volume
     */
    void updateMasterVolume();

    /**
     * \~english @brief Toggles enabling output of the specified remote speaker
     *
     * \~english @param chan Remote speaker channel
     * \~english @param enable Whether to enable the speaker output
     * \~english @return Previous enable status
     */
    bool setEnableSw(s32 chan, bool enable);

    /**
     * \~english @brief Configures the main and remote output volumes of the
     * specified sound handle
     *
     * \~english @param pHandle Sound handle
     * \~english @param playerFlag Player output line flags
     * \~english @param mainOut Main output volume
     * \~english @param remoteOut Remote output volume
     * \~english @param muteOut Main output volume when the remote speaker is
     * muted
     */
    void setRemoteSend(nw4r::snd::SoundHandle* pHandle, u32 playerFlag,
                       f32 mainOut, f32 remoteOut, f32 muteOut);

    /**
     * \~english @brief Callback for new remote connections
     *
     * \~english @param chan Remote channel
     * \~english @param result WPAD library result
     */
    static void setSpeakerOnCallback(s32 chan, s32 result);

    /**
     * \~english @brief Callback for remote disconnections
     *
     * \~english @param chan Remote channel
     * \~english @param result WPAD library result
     */
    static void setSpeakerOffCallback(s32 chan, s32 result);

    /**
     * \~english @brief Callback for initializing remote speaker connections
     *
     * \~english @param chan Remote channel
     * \~english @param result WPAD library result
     */
    static void setSpeakerSetupCallback(s32 chan, s32 result);

    /**
     * \~english @brief Callback for finalizing remote speaker connections
     *
     * \~english @param chan Remote channel
     * \~english @param result WPAD library result
     */
    static void setSpeakerShutdownCallback(s32 chan, s32 result);

    /**
     * \~english @brief Attempts to connect to all remote speakers
     */
    void connectAll();

    /**
     * \~english @brief Attempts to disconnect from all remote speakers
     */
    void disconnectAll();

    /**
     * \~english @brief Tests whether the manager has no connections and is in
     * an idle state
     */
    bool isDisconnectAllFinished();

private:
    /**
     * \~english @brief Speaker connection task
     */
    struct ConnectTask {
        //! Whether to turn the speakers on
        bool on; // at 0x0
        //! Whether this command has finished
        bool finish; // at 0x1
        //! Time-out threshold
        u16 timeOut; // at 0x2
        //! Remote channel index
        s32 chan; // at 0x4
    };

    //! Task queue size
    static const int QUEUE_SIZE = 20;
    //! Task time-out threshold
    static const int TIMEOUT = 180;

private:
    /**
     * \~english @brief Converts player-indexed bitflags to remote channel
     * output flags
     *
     * \~english @param playerFlag Player flags
     * \~english @return Remote channel output line flags
     */
    u32 changeBitPlayerToChannel(u32 playerFlag);

    /**
     * \~english @brief Requests a change in the specified remote speaker's
     * state
     *
     * \~english @param chan Remote speaker channel
     * \~english @param on Whether to turn the speaker on
     */
    static void setSpeakerSw(s32 chan, bool on);

    /**
     * \~english @brief Processes the specified remote speaker connection
     *
     * \~english @param chan Remote speaker channel
     * \~english @param on Whether to turn the speaker on
     */
    void taskSpeakerSw(s32 chan, bool on);

    /**
     * \~english @brief Tests whether the specified connection task has
     * completed
     *
     * \~english @param chan Remote speaker channel
     */
    static bool isTaskFinished(s32 chan);

private:
    s32 unk14;
    u8 unk18;

    //! Whether a connect task is currently ongoing
    bool mIsBusy; // at 0x19
    //! Whether all speakers have been disconnected
    bool mIsDisconnected; // at 0x1A

    //! Master speaker volume
    u8 mMasterVolume; // at 0x1B

    u32 unk1C[RP_MAX_CONTROLLERS];
    u32 unk2C[RP_MAX_CONTROLLERS];
    u32 unk3C[RP_MAX_CONTROLLERS];
    u32 unk4C[RP_MAX_CONTROLLERS];

    //! Whether each speaker has been initialized
    bool mIsSpeakerSetUp[RP_MAX_CONTROLLERS]; // at 0x5C
    //! Whether each speaker is currently turned on
    bool mIsSpeakerOn[RP_MAX_CONTROLLERS]; // at 0x60
    //! Whether each speaker has audio output enabled
    bool mIsEnabledOutput[RP_MAX_CONTROLLERS]; // at 0x64

    u8 unk68[RP_MAX_CONTROLLERS];

    //! Speaker connection task queue
    ConnectTask mTaskQueue[QUEUE_SIZE]; // at 0x6C
    //! Number of finished tasks
    u32 mTaskFinishCount; // at 0x10C
    //! Number of requested tasks
    u32 mTaskRequestCount; // at 0x110

    u32 unk114; // at 0x114
};

//! @}

#endif
