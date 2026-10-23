#ifndef RP_SYSTEM_QUEUED_SCENE_H
#define RP_SYSTEM_QUEUED_SCENE_H

#include <Pack/types_pack.h>

#include <Pack/RPSingleton.h>

class RPSysQueuedScene {
    RP_SINGLETON_DECL(RPSysQueuedScene);

public:
    /**
     * \~english @deprecated Sets the next scene to the current one. No longer
     * used as the singleton does not have a heap, so the constructor takes its
     * place.
     * \~japanese @deprecated 次のシーンを現在のシーンに設定する。
     * Singletonのヒープがなくなったから、この関数をコンストラクタと取り替えてる。
     */
    void reset();

    s32 getNextSceneID() const {
        return mNextSceneID;
    }
    void setNextSceneID(s32 id) {
        mNextSceneID = id;
    }

private:
    s32 mNextSceneID; // at 0x8
};

#endif
