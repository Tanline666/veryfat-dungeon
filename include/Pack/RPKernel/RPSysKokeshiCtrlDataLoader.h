#ifndef RP_KERNEL_KOKESHI_CTRL_DATA_LOADER_H
#define RP_KERNEL_KOKESHI_CTRL_DATA_LOADER_H
#include <Pack/types_pack.h>

//! @addtogroup rp_kernel
//! @{

/**
 * \~english @brief Controller Mii data loader
 */
class RPSysKokeshiCtrlDataLoader {
private:
    /**
     * \~english @brief Data loading state
     */
    enum ELoadState {
        ELoadState_Start,  //!< Begin request
        ELoadState_Wait,   //!< Wait for RFL
        ELoadState_Finish, //!< Request complete
    };

private:
    //! Selected WPAD channel
    s32 mChannel; // at 0x0
    //! Current load state
    ELoadState mLoadState; // at 0x4

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param chan Remote channel
     */
    explicit RPSysKokeshiCtrlDataLoader(s32 chan);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysKokeshiCtrlDataLoader(); // at 0x8

    /**
     * \~english @brief Tests whether an error occurred while loading data
     */
    BOOL isError() const;

    /**
     * \~english @brief Begins loading Mii data from the controller
     */
    void load();

    /**
     * \~english @brief Tests whether the loader is in an idle state
     */
    BOOL isIdle() const;

    /**
     * \~english @brief Updates the loader state
     */
    void calc();

    /**
     * \~english @brief Gets the WPAD channel this loader is using
     */
    s32 getChannel() const {
        return mChannel;
    }
};

//! @}

#endif
