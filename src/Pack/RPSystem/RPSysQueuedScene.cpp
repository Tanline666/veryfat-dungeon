#include <Pack/RPSystem.h>

RP_SINGLETON_IMPL(RPSysQueuedScene);

RPSysQueuedScene::RPSysQueuedScene() {
    mNextSceneID = -1;
}

RPSysQueuedScene::~RPSysQueuedScene() {}
