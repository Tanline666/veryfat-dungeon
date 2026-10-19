#ifndef RP_SYSTEM_SCENE_MGR_H
#define RP_SYSTEM_SCENE_MGR_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>
#include <Pack/RPSystem/RPSysScene.h>

#include <egg/core.h>

#include <nw4r/ut.h>

//! @addtogroup rp_system
//! @{

/**
 * \~english @brief Pack Project scene manager
 */
class RPSysSceneMgr : public EGG::SceneManager {
    RP_SINGLETON_DECL(RPSysSceneMgr);

private:
    enum EMenu {
        EMenu_Main,        //!< Wii Menu itself
        EMenu_DataManager, //!< Save data and channel management
        EMenu_Calendar,    //!< Date and time
        EMenu_Display,     //!< Display settings
        EMenu_Sound,       //!< Sound settings
        EMenu_Parental,    //!< Parental controls
        EMenu_Internet,    //!< Internet settings
        EMenu_WC24,        //!< WiiConnect24 settings
        EMenu_Update       //!< Update console
    };

public:
    /**
     * \~english @brief Updates the state of the current scene
     */
    virtual void calcCurrentScene() override; // at 0x10

    /**
     * \~english @brief Gets the currently active scene as the Pack Project type
     */
    RPSysScene* getCurrentSceneRP() const;

    /**
     * \~english @brief Performs a soft reset
     *
     * \~english @param fromHBM Whether the request came from HBM
     * \~english @return Success
     */
    bool softReset(bool fromHBM);

    /**
     * \~english @brief Returns to the IPL
     *
     * \~english @param fromHBM Whether the request came from HBM
     * \~english @return Success
     */
    bool returnToMenu(bool fromHBM);

    /**
     * \~english @brief Powers off the console
     *
     * \~english @param fromHBM Whether the request came from HBM
     * \~english @return Success
     */
    bool shutdownSystem(bool fromHBM);

    /**
     * \~english @brief Tests whether this manager is operating normally
     */
    bool isNormalState() const;

    /**
     * \~english @brief Tests whether this manager is currently shutting down
     * the system
     */
    bool isShutDownReserved() const;

    /**
     * \~english @brief Tests whether this manager is currently returning to the
     * IPL
     */
    bool getReturnToMenu() const;

    /**
     * \~english @brief Gets the Wii Menu page the game will go to upon exiting
     *
     * \~english @return EMenu: ID for page
     */
    EMenu getMenuPage() const;

    /**
     * \~english @brief Disables the guard state and returns to normal operation
     */
    void resetGuard();

    /**
     * \~english @brief Tests whether this manager is currently exiting the
     * program
     */
    bool isApplicationEndReserved();

    /**
     * \~english @brief Tests whether a DVD error should exit the program
     */
    bool isDvdErrorApplicationEnd();

    /**
     * \~english @brief Requests a scene change after fading out
     * @details A scene id of -1 will select the current scene.
     *
     * \~english @param id New scene ID
     * \~english @param reconfigure Whether to fully reconfigure the scene
     * \~english @return Success
     */
    bool changeNextSceneAfterFade(s32 id, bool reconfigure = false);

    /**
     * \~english @brief Resets the duration of all faders
     */
    void resetFadeFrame();

    /**
     * \~english @brief Gets the duration of the scene fader
     */
    u16 getFadeFrame();

    /**
     * \~english @brief Sets the duration of all faders
     *
     * \~english @param frame Fade duration, in frames
     */
    void setFadeFrame(u16 frame);

    /**
     * \~english @brief Resets the color of all faders
     * @details The default fader color is black.
     */
    void resetFadeColor();

    /**
     * \~english @brief Gets the color of the manager fader
     */
    nw4r::ut::Color getFadeColor();

    /**
     * \~english @brief Sets the color of all faders
     *
     * \~english @param color Fade color
     */
    void setFadeColor(nw4r::ut::Color color);

    /**
     * \~english @brief Sets the color of the scene fader
     *
     * \~english @param color Fade color
     */
    void setSceneFadeColor(nw4r::ut::Color color);

    /**
     * \~english @brief Gets the status of the scene fader
     */
    EGG::Fader::EStatus getSceneStatus();

    /**
     * \~english @brief Sets the status of the scene fader
     *
     * \~english @param status Fade status
     */
    void setSceneStatus(EGG::Fader::EStatus status);

    /**
     * \~english @brief Begins fading in the manager fader
     *
     * \~english @return Success
     */
    bool startMgrFadeIn();

    /**
     * \~english @brief Begins fading in the scene fader
     *
     * \~english @return Success
     */
    bool startSceneFadeIn();

    /**
     * \~english @brief Begins fading out the scene fader
     *
     * \~english @return Success
     */
    bool startSceneFadeOut();

    /**
     * \~english @brief Tests whether the scene is fully visible
     */
    bool isDisplay();

    /**
     * \~english @brief Tests whether the scene is visibly obscured
     */
    bool isBlank();

    /**
     * \~english @brief Tests whether the scene is fading out
     */
    bool isFadeOut();

    /**
     * \~english @brief Tests whether all asynchronous tasks have finished
     */
    bool isTaskFinished();

    /**
     * \~english @brief Tests whether a scene change would be allowed
     */
    bool isSceneChangeEnable();

    /**
     * \~english @brief Updates the state of the scene fader
     *
     * \~english @return Whether the fade has finished
     */
    bool calcSceneFader();

    /**
     * \~english @brief Displays the state of the scene fader
     */
    void drawSceneFader();

    /**
     * \~english @brief Displays the state of the manager fader
     */
    void drawMgrFader();

    /**
     * \~english @brief Updates the state of this manager
     */
    void updateState();

private:
    /**
     * \~english @brief Scene manager state
     */
    enum EState {
        EState_Normal,       //!< Normal operation
        EState_Guard,        //!< Prevents operations during soft reset
        EState_SoftReset,    //!< Perform soft reset
        EState_ReturnToMenu, //!< Return to system menu
        EState_Shutdown      //!< Shutdown the system
    };

    //! The default fade duration, in frames
    static const int DEFAULT_FADE_FRAME = 20;

    //! The default fade color
    static const u32 DEFAULT_FADE_COLOR = nw4r::ut::Color::BLACK;

private:
    //! Manager state
    EState mState; // at 0x2C
    //! Page to access when returning to Wii Menu
    EMenu mMenuPage; // at 0x30
    u8 UNK_0x34;
    //! Fader usable by the current scene
    EGG::ColorFader* mpSceneFader; // at 0x30
};

//! @}

#endif
