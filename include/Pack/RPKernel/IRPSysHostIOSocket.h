#ifndef RP_KERNEL_I_HOST_IO_SOCKET_H
#define RP_KERNEL_I_HOST_IO_SOCKET_H
#include <Pack/types_pack.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Host I/O socket
 */
class IRPSysHostIOSocket {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pName Socket name
     * \~english @param ... Format string arguments
     */
    IRPSysHostIOSocket(const char* pName, ...);
    /**
     * \~english @brief Constructor
     */
    IRPSysHostIOSocket();

    /**
     * \~english @brief Destructor
     */
    virtual ~IRPSysHostIOSocket(); // at 0x8

    /**
     * \~english @brief Handles property events from the host
     */
    virtual void ListenPropertyEvent() { // at 0xC
        // Stubbed for release
    }

    /**
     * \~english @brief Generates UI control messages for the host
     */
    virtual void GenMessage() { // at 0x10
        // Stubbed for release
    }
};

//! @}

#endif
