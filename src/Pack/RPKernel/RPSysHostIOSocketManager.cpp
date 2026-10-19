#include <Pack/RPKernel.h>

RP_SINGLETON_IMPL(RPSysHostIOSocketManager);

/**
 * \~english @brief Constructor
 */
RPSysHostIOSocketManager::RPSysHostIOSocketManager() {
    // Stubbed for release
}

/**
 * \~english @brief Destructor
 */
RPSysHostIOSocketManager::~RPSysHostIOSocketManager() {
    // Stubbed for release
}

/**
 * \~english @brief Adds a new host I/O socket to the node tree
 *
 * \~english @param pChild New child socket
 * \~english @param pParent Parent socket
 */
void RPSysHostIOSocketManager::AddHostIOSocket(
    IRPSysHostIOSocket* /* pChild */, IRPSysHostIOSocket* /* pParent */) {
    // Stubbed for release
}

/**
 * \~english @brief Updates the state of the host I/O sockets
 */
void RPSysHostIOSocketManager::Update() {
    // Stubbed for release
}
