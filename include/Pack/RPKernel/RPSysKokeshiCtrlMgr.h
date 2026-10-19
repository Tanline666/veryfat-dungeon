#ifndef RP_KERNEL_KOKESHI_CONTROLLER_MGR_H
#define RP_KERNEL_KOKESHI_CONTROLLER_MGR_H
#include <Pack/types_pack.h>

#include <Pack/RPGraphics.h>
#include <Pack/RPKernel/RPSysKokeshiManager.h>

#include <egg/core.h>

#include <revolution/WPAD.h>

//! @addtogroup rp_kernel
//! @{

// Forward declarations
class IRPSysKokeshiCtrlDataCallback;
class RPSysKokeshiCtrlDataMgr;

/**
 * \~english @brief Controller Mii manager
 */
class RPSysKokeshiCtrlMgr {
private:
    /**
     * \~english @brief Lifetime manager
     */
    class T__Disposer : public EGG::Disposer {
    public:
        /**
         * \~english @brief Destructor
         */
        virtual ~T__Disposer() override { // at 0x8
            RP_GET_INSTANCE(RPSysKokeshiManager)->ClearCtrlMgr();
        }
    };

    //! Disposer for this manager
    T__Disposer mDisposer; // at 0x0

private:
    void* unk10;

    //! Data manager for each controller
    RPSysKokeshiCtrlDataMgr* mpDataMgrs[RP_MAX_CONTROLLERS]; // at 0x14

public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pCallback Controller data callback
     * \~english @param pParentHeap Heap to use as the parent of the controller
     * data heap
     * \~english @param heapSize Size of the controller data heap
     */
    RPSysKokeshiCtrlMgr(IRPSysKokeshiCtrlDataCallback* pCallback,
                        EGG::Heap* pParentHeap, u32 heapSize);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPSysKokeshiCtrlMgr(); // at 0x8

    /**
     * \~english @brief Notifies the user that all resources will be destroyed
     *
     * \~english @param chan WPAD channel of the data manager
     */
    void notifyReleaseResource(s32 chan);

    /**
     * \~english @brief Resets the internal state for creating resources
     *
     * \~english @param chan WPAD channel of the data manager
     */
    void resetResource(s32 chan);

    /**
     * \~english @brief Tests whether the specified data manager is in an idle
     * state
     *
     * \~english @param chan WPAD channel of the data manager
     */
    BOOL isIdle(s32 chan) const;

    /**
     * \~english @brief Tests whether an error occured while loading data
     *
     * \~english @param chan WPAD channel of the data manager
     */
    BOOL isLoadError(s32 chan) const;

    /**
     * \~english @brief Creates resources for the next Mii in each database
     */
    void createResource();

    /**
     * \~english @brief Gets the heap used for resource allocation by the
     * specified data manager
     *
     * \~english @param chan WPAD channel of the data manager
     */
    EGG::Heap* getResourceHeap(s32 chan);

    /**
     * \~english @brief Appends a new file to the file resource list
     *
     * \~english @param chan WPAD channel of the data manager
     * \~english @param handle Resource handle
     */
    void appendFile(s32 chan, RPGrpHandle handle);

    /**
     * \~english @brief Appends a new model to the model resource list
     *
     * \~english @param chan WPAD channel of the data manager
     * \~english @param pModel New model
     */
    void appendModel(s32 chan, RPGrpModel* pModel);

    /**
     * \~english @brief Updates each data manager's state
     */
    void calc();
};

//! @}

#endif
