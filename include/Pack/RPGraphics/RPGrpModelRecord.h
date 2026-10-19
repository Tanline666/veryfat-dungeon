#ifndef RP_GRAPHICS_MODEL_RECORD_H
#define RP_GRAPHICS_MODEL_RECORD_H
#include <Pack/types_pack.h>

//! @addtogroup rp_graphics
//! @{

// Forward declarations
class RPGrpModel;

/**
 * \~english @brief Model transformation recorder
 */
class RPGrpModelRecord {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param flags Bound type flags
     * \~english @param pModel Owner model
     */
    RPGrpModelRecord(u16 frames, RPGrpModel* pModel);

    /**
     * \~english @brief Destructor
     */
    virtual ~RPGrpModelRecord() {} // at 0x8

    /**
     * \~english @brief Clears the recorded data
     */
    void Clear();

    /**
     * \~english @brief Records the current frame
     *
     * \~english @param pModel Model to record
     */
    void Calc(RPGrpModel* pModel);

private:
    char unk4[0x48 - 0x4];
};

//! @}

#endif
