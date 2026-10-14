#ifndef RP_SYSTEM_SCENE_CREATOR_H
#define RP_SYSTEM_SCENE_CREATOR_H
#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>

#include <egg/core.h>

//! @addtogroup rp_system
//! @{

// Forward declarations
class RPSysScene;

/**
 * @brief Pack Project scene factory
 */
class RPSysSceneCreator : public EGG::SceneCreator {
    RP_SINGLETON_DECL_EX(RPSysSceneCreator);

public:
    /**
     * @brief Pack Project scene ID
     */
    //! Below names are tentative
    enum ESceneID {
        ESceneID_RPHealthPlayerSelectScene, //!< Wii Fit Plaza
        ESceneID_RPHealthMainMenu,          //!< Main Menu
        ESceneID_RPHealthUserSettings,      //!< User settings

        //! TODO(texline): Is this a channel thing?
        ESceneID_RPHealthLaunchDisk,

        ESceneID_RPHealthBodyTest,       //!< Body Test
        ESceneID_RPHealthBodyTestResult, //!< Body Test results
        ESceneID_RPHealthSimpleCheck,    //!< Simple Test
        ESceneID_RPHealthHoldCheck,      //!< Unknown (TODO)
        ESceneID_RPHealthConnectCheck,   //!< Please connect balance board
        ESceneID_RPHealthBalanceTest,    //!< Balance tests in body check
        ESceneID_RPHealthCapture, //!< Unknown (Points to Wii Fit Plaza with
                                  //!< different args. Screenshot mode?)
        ESceneID_RPHealthPackBootScene,     //!< Unknown (TODO)
        ESceneID_RPHealthTitleScene,        //!< Only used in E3 demo
        ESceneID_RPHealthGuestMode,         //!< Guest training menu
        ESceneID_RPHealthFirstSettingScene, //!< Setting up a new player
        ESceneID_RPHealthContinue, //!< Unknown (Is this even a formal scene?
                                   //!< Wii Fit U seems to indicate that this is
                                   //!< just for animating characters in other
                                   //!< scenes)
        ESceneID_RPHealthBasicSettingScene,    //!< Non-user settings
        ESceneID_RPHealthControllerCheckScene, //!< Please connect/disconnect
                                               //!< nunchuk
        ESceneID_RPHealthPairingScene,         //!< Controller setup?
        ESceneID_RPHealthRest,                 //!< Similar to RPHealthContinue?
        ESceneID_RPHealthGetBack,              //!< Unknown (TODO)

        ESceneID_RPHealthTrainingMenuScene,
        ESceneID_RPHealthMyMenuScene,
        ESceneID_RPHealthGameConnectScene, //!< Unknown (links to My Menu)
        ESceneID_RPHealthTrainingScene,

        //! Wii Fit
        ESceneID_RPHealthSkiScene,       //!< Ski Slalom and Snowboard Slalom
        ESceneID_RPHealthSkiJumpScene,   //!< Ski Jumping
        ESceneID_RPHealthHulaHoopScene,  //!< (Super) Hula Hoop
        ESceneID_RPHealthIraIraScene,    //!< Balance Bubble (Plus)
        ESceneID_RPHealthStepScene,      //!< Basic Step and Advanced Step
        ESceneID_RPHealthKoroScene,      //!< Table Tilt (Plus)
        ESceneID_RPHealthZazenScene,     //!< Lotus Focus
        ESceneID_RPHealthRopeScene,      //!< Tightrope Walking
        ESceneID_RPHealthHeadingScene,   //!< Soccer Heading
        ESceneID_RPHealthPenguinScene,   //!< Penguin Slide
        ESceneID_RPHealthBoxerciseScene, //!< Rhythm Boxing
        ESceneID_RPHealthJoggingScene,   //!< Basic Run, 2P Run, Free Run

        //! Wii Fit Plus
        ESceneID_RPHealthRhythmKungfuScene,   //!< Rhythm Kung Fu
        ESceneID_RPHealthDungeonJoggingScene, //!< Obstacle Course
        ESceneID_RPHealthSkateBoardScene,     //!< Skateboard Arena
        ESceneID_RPHealthJugglingScene,       //!< Big-Top Juggling
        ESceneID_RPHealthParadeScene,         //!< Rhythm Parade
        ESceneID_RPHealthCalculationScene,    //!< Perfect 10
        ESceneID_RPHealthBicycleScene,        //!< Island Cycling
        ESceneID_RPHealthGolfScene,           //!< Driving Range
        ESceneID_RPHealthJogQuizScene,        //!< Basic Run Plus
        ESceneID_RPHealthPataScene,           //!< Birds-Eye Bulls-Eye
        ESceneID_RPHealthSegwayScene,         //!< Segway Circuit
        ESceneID_RPHealthShootingScene,       //!< Snowball Fight
        ESceneID_RPHealthColorScene,          //!< Tilt City

        ESceneID_RPHealthStaffCredit,     //!< Wii Fit credits
        ESceneID_RPHealthStaffCreditPlus, //!< Wii Fit Plus credits
    };

    /**
     * @brief Scene create type
     */
    enum ECreateType {
        ECreateType_Standard, //!< Overwrite outgoing scene
        ECreateType_Sibling,  //!< Become sibling of outgoing scene
        ECreateType_Child,    //!< Become child of outgoing scene
    };

    /**
     * @brief Scene exit type
     */
    enum EExitType {
        EExitType_Standard, //!< Incoming scene overwrites this scene
        EExitType_Sibling,  //!< Incoming scene is a sibling of this scene
        EExitType_2,
        EExitType_3,
        EExitType_Child, //!< Incoming scene is a child of this scene
    };

    /**
     * @brief Scene audio type
     */
    enum EAudioType {
        EAudioType_Scene,  //!< Use the scene's sound archive
        EAudioType_Common, //!< Use the common sound archive
    };

    /**
     * @brief Sport ID
     */
    enum ESportID {
        ESportID_Bsb, //!< Baseball
        ESportID_Tns, //!< Tennis
        ESportID_Gol, //!< Golf
        ESportID_Box, //!< Boxing
        ESportID_Bow, //!< Bowling

        ESportID_Max,
    };

    /**
     * @brief Scene attributes
     */
    struct SceneAttr {
        //! Scene ID
        ESceneID scene; // at 0x0

        //! How to create the scene
        ECreateType createType; // at 0x4
        //! How to exit the scene
        EExitType exitType; // at 0x8
        //! How to configure the scene audio
        EAudioType audioType; // at 0xC

        //! Root resource directory
        const char* pResDirName; // at 0x10
        //! Readable name
        const char* pSceneName; // at 0x14
    };

public:
    /**
     * @brief Creates the specified scene
     *
     * @param id Scene ID
     */
    virtual EGG::Scene* create(s32 id); // at 0x8

    /**
     * @brief Destroys the specified scene
     *
     * @param id Scene ID
     */
    virtual void destroy(s32 id); // at 0xC

    /**
     * @brief Requests a scene change after fading out
     * @details A scene id of -1 will select the current scene.
     *
     * @param id New scene ID
     * @param reconfigure Whether to fully reconfigure the scene
     * @return Success
     */
    bool changeSceneAfterFade(s32 id, bool reconfigure = false)
        DECOMP_DONT_INLINE;

    /**
     * @brief Changes to the title scene
     *
     * @return Success
     */
    bool changeSoftReset();

    /**
     * @brief Gets the specified scene's resource directory
     * @details A scene id of -1 will select the current scene.
     *
     * @param id Scene ID
     */
    const char* getResDirName(s32 id = -1);

    /**
     * @brief Gets the specified scene's sport ID
     * @details A scene id of -1 will select the current scene.
     * @note If no sport corresponds to the scene, this function returns -1.
     *
     * @param id Scene ID
     */
    s32 getSportID(s32 id = -1);

    /**
     * @brief Gets the specified sport's scene ID
     * @note If no scene corresponds to the sport, this function returns -1.
     *
     * @param sport Sport ID
     */
    s32 getSceneID(ESportID sport);

    /**
     * @brief Gets the specified scene's creation type
     *
     * @param id Scene ID
     */
    ECreateType getCreateType(s32 id);

    /**
     * @brief Gets the specified scene's exit type
     *
     * @param id Scene ID
     */
    EExitType getExitType(s32 id);

    /**
     * @brief Gets the specified scene's audio type
     *
     * @param id Scene ID
     */
    EAudioType getAudioType(s32 id);

private:
    /**
     * @brief Gets the index of the specified scene's entry in the table
     *
     * @param id Scene ID
     * @return Entry index, or -1 if not found
     */
    s32 getEntryIndex(ESceneID id);

    /**
     * @brief Creates the specified Health Pack scene
     *
     * @param id Scene ID
     */
    RPSysScene* createFitScene(s32 id) DECOMP_DONT_INLINE;

private:
    //! Information about every Pack Project scene
    static const SceneAttr SCENE_ATTR_TABLE[];

    //! ID of the previous scene
    s32 mPrevSceneID; // at 0x8
};

//! @}

#endif
