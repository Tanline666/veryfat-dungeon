#ifndef RP_AUDIO_SND_UTILITY_H
#define RP_AUDIO_SND_UTILITY_H
#include <Pack/types_pack.h>

#include <nw4r/math.h>
#include <nw4r/snd.h>

//! @addtogroup rp_audio
//! @{

/**
 * \~english @brief Sound utilities
 */
class RPSndUtility {
public:
    /**
     * \~english @brief Sets the panning of the sound handle to match the DPD
     * position
     *
     * \~english @param rHandle Sound handle
     * \~english @param rDpdPos DPD position
     * \~english @param scale Panning scale
     * \~english @return Success
     */
    static bool setHandleDpdPan(nw4r::snd::SoundHandle& rHandle,
                                const nw4r::math::VEC2& rDpdPos, f32 scale);

    /**
     * \~english @brief Gets the active sound archive player
     */
    static nw4r::snd::SoundArchivePlayer* getPlayer() {
        return spSoundArchivePlayer;
    }

    /**
     * \~english @brief Sets the active sound archive player
     *
     * \~english @param pPlayer Sound archive player
     */
    static void setPlayer(nw4r::snd::SoundArchivePlayer* pPlayer) {
        spSoundArchivePlayer = pPlayer;
    }

private:
    //! Active sound archive player
    static nw4r::snd::SoundArchivePlayer* spSoundArchivePlayer;
};

//! @}

#endif
