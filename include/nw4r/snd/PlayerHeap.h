#ifndef NW4R_SND_PLAYER_HEAP_H
#define NW4R_SND_PLAYER_HEAP_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/SoundHeap.h>

namespace nw4r {
namespace snd {

// Forward declarations
class SoundPlayer;

namespace detail {
class BasicSound;
} // namespace detail

namespace detail {

class PlayerHeap : public SoundMemoryAllocatable {
public:
    PlayerHeap() : mSound(NULL), mPlayer(NULL) {}
    virtual ~PlayerHeap() {} //!< // at 0x8

    virtual void* Alloc(u32 size); //!< // at 0xC

    void AttachSoundPlayer(SoundPlayer* player) {
        mPlayer = player;
    }

    void AttachSound(BasicSound* sound);
    void DetachSound(BasicSound* sound);

    bool Create(void* startAddress, u32 size);
    void Destroy();

    void Clear();

    bool IsValid() const {
        return mAllocAddress != NULL;
    }

    u32 GetFreeSize() const;

private:
    BasicSound* mSound;   //!< // at 0x4
    SoundPlayer* mPlayer; //!< // at 0x8

    void* mStartAddress; //!< // at 0xC
    void* mEndAddress;   //!< // at 0x10
    void* mAllocAddress; //!< // at 0x14

public:
    NW4R_UT_LINKLIST_NODE_DECL(); //!< // at 0x18
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(PlayerHeap);

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
