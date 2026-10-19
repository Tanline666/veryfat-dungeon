#include <Pack/RPAudio.h>

#include <nw4r/ut.h>

/******************************************************************************
 *
 * RPSndAudioActorBase
 *
 ******************************************************************************/

/**
 * \~english @brief List of all active actors
 */
nw4r::ut::List RPSndAudioActorBase::sActorList;

/******************************************************************************
 *
 * RPSndObjMgr
 *
 ******************************************************************************/

/**
 * \~english @brief Fades out all active sounds in the specified number of
 * frames
 *
 * \~english @param frames Fade out time
 */
void RPSndObjMgr::stopAllActorSound(int frames) {
    RP_NW4R_LIST_FOREACH (RPSndAudioActorBase, it,
                          RPSndAudioActorBase::sActorList) {
        it->stopAllSound(frames);
    }
}
