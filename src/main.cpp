#include <Pack/RPSystem.h>

#if defined(__MWERKS__)
void main(int /* argc */, char** /* argv */) {
#else
int main(int /* argc */, char** /* argv */) {
#endif
    RPSysSystem::initialize();
    RPSysSystem::create();
    RP_GET_INSTANCE(RPSysSystem)->setup();
    RP_GET_INSTANCE(RPSysSystem)->mainLoop();
}
