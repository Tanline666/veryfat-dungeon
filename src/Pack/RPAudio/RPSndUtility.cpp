#include <Pack/RPAudio.h>

#include <nw4r/math.h>
#include <nw4r/snd.h>
#include <nw4r/ut.h>

/**
 * \~english @brief Active sound archive player
 */
nw4r::snd::SoundArchivePlayer* RPSndUtility::spSoundArchivePlayer = NULL;

/**
 * \~english @brief Sets the panning of the sound handle to match the DPD
 * position
 *
 * \~english @param rHandle Sound handle
 * \~english @param rDpdPos DPD position
 * \~english @param scale Panning scale
 * \~english @return Success
 */
bool RPSndUtility::setHandleDpdPan(nw4r::snd::SoundHandle& rHandle,
                                   const nw4r::math::VEC2& rDpdPos, f32 scale) {

    if (rHandle.IsAttachedSound()) {
        f32 pan = rDpdPos.x * scale;

        if (pan > 1.0f) {
            pan = 1.0f;
        } else if (pan < -1.0f) {
            pan = -1.0f;
        }

        rHandle.SetPan(pan);

        if (scale > 1.0f && pan > 0.8f) {
            f32 surroundPan = scale - 0.8f;

            if (surroundPan > 0.5f) {
                surroundPan = 0.5f;
            }

            rHandle.SetSurroundPan(surroundPan);
        }

        return true;
    }

    return false;
}
