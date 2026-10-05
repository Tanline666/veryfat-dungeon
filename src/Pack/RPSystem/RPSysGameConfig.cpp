#include <Pack/RPSystem.h>

RP_SINGLETON_IMPL_EX(RPSysGameConfig);

/**
 * @brief Constructor
 *
 * @param pHeap Parent heap
 */
RPSysGameConfig::RPSysGameConfig(EGG::Heap* pHeap)
    : RPSysTagParameters("GameConfig"),
      mpParentHeap(pHeap),
      mRootScene(this, "RootScene"),
      mTVMode(this, "TVMode"),
      mLanguage(this, "Language"),
      mSound(this, "Sound"),
      mTrainingLevel(this, "TrainingLevel"),

      mRPPrint(this, "RPPrint"),
      mRPSysPrint(this, "RPSysPrint"),
      mRPAudPrint(this, "RPAudPrint"),
      mRPSndPrint(this, "RPSndPrint"),
      mRPUserPrint(this, "RPUserPrint") {

    mRootScene.set(NULL);
    mTVMode.set(NULL);
    mLanguage.set(NULL);

    mRPPrint.set(false);
    mRPSysPrint.set(false);
    mRPAudPrint.set(false);
    mRPSndPrint.set(false);
    mRPUserPrint.set(false);
}

/**
 * @brief Destructor
 */
RPSysGameConfig::~RPSysGameConfig() {}
