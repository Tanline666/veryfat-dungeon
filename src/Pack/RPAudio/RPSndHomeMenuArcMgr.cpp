#include <Pack/RPAudio.h>

/**
 * \~english @brief Constructor
 */
RPSndHomeMenuArcMgr::RPSndHomeMenuArcMgr()
    : ArcPlayer(&getSoundHeap()), mIsInitialized(false) {}

/**
 * \~english @brief Destructor
 */
RPSndHomeMenuArcMgr::~RPSndHomeMenuArcMgr() {}

/**
 * \~english @brief Mounts the provided sound archive
 *
 * \~english @param pBinary Binary sound archive
 * \~english @return Success
 */
bool RPSndHomeMenuArcMgr::setupArchive(const void* pBinary) {
    bool success = setupMemoryArchive(pBinary, &getSoundHeap()) != NULL;

    mIsInitialized = true;
    return success;
}

/**
 * \~english @brief Updates the state of the sound player
 */
void RPSndHomeMenuArcMgr::calc() {
    if (!mIsInitialized) {
        return;
    }

    EGG::ArcPlayer::calc();
}
