#ifndef NW4R_G3D_RES_RES_COMMON_H
#define NW4R_G3D_RES_RES_COMMON_H
#include <nw4r/types_nw4r.h>

#include <revolution/GX.h>

/******************************************************************************
 *
 * Macros
 *
 ******************************************************************************/

/**
 * Define ResName pascal string for file resource groups.
 */
#define NW4R_G3D_RESFILE_NAME_DEF(VAR, STR)                                    \
    nw4r::g3d::ResNameData27 ResNameData_##VAR ALIGN(32) = {sizeof(STR) - 1,   \
                                                            STR}

/**
 * Similar to "ofs_to_obj" but accounting for the additional -4 offset.
 * Debug builds show this behavior was not achieved through a function.
 */
#define NW4R_G3D_OFS_TO_RESNAME(BASE, OFS)                                     \
    nw4r::g3d::ResName((char*)(BASE) + (OFS) - sizeof(u32))

/**
 * Define common functions for resource classes.
 * \~ @note Hides ResCommon::ref, why did they do this???
 */
#define NW4R_G3D_RESOURCE_FUNC_DEF(T)                                          \
    NW4R_G3D_RESOURCE_FUNC_DEF_IMPL(T, T##Data)
#define NW4R_G3D_RESOURCE_FUNC_DEF_EX(TCLS, TDATA)                             \
    NW4R_G3D_RESOURCE_FUNC_DEF_IMPL(TCLS, TDATA)

#define NW4R_G3D_RESOURCE_FUNC_DEF_IMPL(TCLS, TDATA)                           \
    explicit TCLS(void* pData = NULL) : nw4r::g3d::ResCommon<TDATA>(pData) {}  \
                                                                               \
    TDATA& ref() {                                                             \
        return *ptr();                                                         \
    }                                                                          \
                                                                               \
    const TDATA& ref() const {                                                 \
        return *ptr();                                                         \
    }                                                                          \
                                                                               \
    bool operator==(const TCLS& rOther) const {                                \
        return ptr() == rOther.ptr();                                          \
    }                                                                          \
                                                                               \
    bool operator!=(const TCLS& rOther) const {                                \
        return ptr() != rOther.ptr();                                          \
    }

namespace nw4r {
namespace g3d {

/**
 * \~english @brief Basic class template for resource accessors.
 * \~japanese @brief リソースアクセサの基底クラステンプレート。
 *
 * \~english @tparam T Resource data struct
 * \~japanese @tparam T リソースデータストラクト
 */
template <typename T> class ResCommon {
public:
    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     *
     * \~english @param pData Pointer to binary resource data
     * \~japanese @param pData バイナリリソースデータへのポインタ
     */
    explicit ResCommon(void* pData) : mpData(static_cast<T*>(pData)) {}

    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     *
     * \~english @param pData Pointer to constant binary resource data
     * \~japanese @param pData 定数のバイナリリソースデータへのポインタ
     */
    explicit ResCommon(const void* pData)
        : mpData(static_cast<const T*>(pData)) {}

    bool IsValid() const {
        return mpData != NULL;
    }

    /**
     * \~english @brief Gets a pointer to mpData.
     * \~japanese @brief mpDataへのポインタを取得する。
     */
    T* ptr() {
        return mpData;
    }
    /**
     * \~english @brief Gets a constant pointer to mpData.
     * \~japanese @brief mpDataへの定数のポインタを取得する。
     */
    const T* ptr() const {
        return mpData;
    }

    /**
     * \~english @brief Gets a reference to mpData.
     * \~japanese @brief mpDataへの参照を取得する。
     */
    T& ref() {
        return *mpData;
    }
    /**
     * \~english @brief Gets a constant reference to mpData.
     * \~japanese @brief mpDataへの定数の参照を取得する。
     */
    const T& ref() const {
        return *mpData;
    }

    template <typename TTo> TTo* ofs_to_ptr_raw(s32 ofs) {
        return reinterpret_cast<TTo*>((char*)mpData + ofs);
    }
    template <typename TTo> const TTo* ofs_to_ptr_raw(s32 ofs) const {
        return reinterpret_cast<const TTo*>((char*)mpData + ofs);
    }

    template <typename TTo> TTo* ofs_to_ptr(s32 ofs) {
        u8* pPtr = reinterpret_cast<u8*>(mpData);

        if (ofs != 0) {
            return reinterpret_cast<TTo*>(pPtr + ofs);
        }

        return NULL;
    }
    template <typename TTo> const TTo* ofs_to_ptr(s32 ofs) const {
        const u8* pPtr = reinterpret_cast<const u8*>(mpData);

        if (ofs != 0) {
            return reinterpret_cast<const TTo*>(pPtr + ofs);
        }

        return NULL;
    }

    template <typename TTo> TTo ofs_to_obj(s32 ofs) {
        u8* pPtr = reinterpret_cast<u8*>(mpData);

        if (ofs != 0) {
            return TTo(pPtr + ofs);
        }

        return TTo(NULL);
    }
    template <typename TTo> const TTo ofs_to_obj(s32 ofs) const {
        const u8* pPtr = reinterpret_cast<const u8*>(mpData);

        if (ofs != 0) {
            return TTo(const_cast<u8*>(pPtr + ofs));
        }

        return TTo(NULL);
    }

private:
    T* mpData; //!< \~english Binary resource data \~japanese
               //!< バイナリリソースデータ \~ // at 0x0
};

/**
 * \~english @brief Data block header for resource data structures.
 * \~japanese @brief リソースデータの構造体のデータブロックヘッダ。
 */
struct ResBlockHeaderData {
    char kind[4]; //!< at 0x0
    u32 size;     //!< at 0x4
};

/**
 * \~english @brief Name resource data structure.
 * \~japanese @brief リソース名前データの構造体。
 * \~english @note Max length is 27 characters.
 * \~japanese @note 最大全長は27文字。
 */
struct ResNameData27 {
    u32 len;                    //!< at 0x0
    char str[32 - sizeof(u32)]; //!< at 0x4
};

/**
 * \~english @brief Resource name data structure.
 * \~japanese @brief リソース名前データの構造体。
 */
struct ResNameData {
    u32 len;     //!< at 0x0
    char str[4]; //!< at 0x4
};

/**
 * \~english @brief Class for accessing resource name data.
 * \~japanese @brief リソース名前データをアクセスするためのクラス。
 */
class ResName : public ResCommon<const ResNameData> {
public:
    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     *
     * @param pData
     */
    explicit ResName(const void* pData) : ResCommon(pData) {}

    /**
     * \~english @brief Gets the string length of a name.
     * \~japanese @brief 名前の文字列の長さを取得する。
     */
    u32 GetLength() const {
        return ref().len;
    }

    /**
     * \~english @brief Gets the pointer to a name's string.
     * \~japanese @brief 名前の文字列へのポインタを取得する。
     */
    const char* GetName() const {
        return ref().str;
    }

    /**
     * \~english @brief Checks if two ResName objects are the same.
     * \~japanese @brief 2つのResNameオブジェクトが同じかどうか判定する。
     *
     * \~english @param rhs Right side of operator
     * \~japanese @param rhs オペレータの右辺
     * \~english @return TRUE if equal
     * \~japanese @return 同じならTRUE
     */
    bool operator==(const ResName rhs) const;
};

/******************************************************************************
 *
 * Generic display list
 *
 ******************************************************************************/
struct ResTagDLData {
    u32 bufSize; // at 0x0
    u32 cmdSize; // at 0x4
    s32 toDL;    // at 0x8
};

class ResTagDL : public ResCommon<ResTagDLData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResTagDL);

    u32 GetBufSize() const {
        return ref().bufSize;
    }

    u32 GetCmdSize() const {
        return ref().cmdSize;
    }

    u8* GetDL() {
        return const_cast<u8*>(ofs_to_ptr<u8>(ref().toDL));
    }
    const u8* GetDL() const {
        return ofs_to_ptr<u8>(ref().toDL);
    }
};

/******************************************************************************
 *
 * Model bytecode
 *
 ******************************************************************************/
namespace ResByteCodeData {

enum OpCode {
    NOOP,   // No operation
    END,    // End of bytecode
    CALC,   // Calculate matrix
    WEIGHT, // Apply weighting
    DRAW,   // Draw polygon
    EVPMTX, // Envelope matrix
    MTXDUP  // Duplicate matrix
};

// CALC opcode layout
struct CalcParams {
    u8 opcode;        // at 0x0
    u8 nodeIdHi;      // at 0x1
    u8 nodeIdLo;      // at 0x2
    u8 parentMtxIdHi; // at 0x1
    u8 parentMtxIdLo; // at 0x2
};

// WEIGHT opcode layout
struct WeightParams {
    u8 opcode;      // at 0x0
    u8 tgtIdHi;     // at 0x1
    u8 tgtIdLo;     // at 0x2
    u8 numBlendMtx; // at 0x3
};
// WEIGHT opcode layout - weighting entry
struct WeightEntry {
    u8 mtxIdHi;  // at 0x0
    u8 mtxIdLo;  // at 0x1
    u8 fWeight0; // at 0x2
    u8 fWeight1; // at 0x3
    u8 fWeight2; // at 0x4
    u8 fWeight3; // at 0x5
};

// DRAW opcode layout
struct DrawParams {
    u8 opcode;   // at 0x0
    u8 matIdHi;  // at 0x3
    u8 matIdLo;  // at 0x4
    u8 shpIdHi;  // at 0x1
    u8 shpIdLo;  // at 0x2
    u8 nodeIdHi; // at 0x5
    u8 nodeIdLo; // at 0x6
    u8 priority; // at 0x7
};

// EVPMTX opcode layout
struct EvpMtxParams {
    u8 opcode;   // at 0x0
    u8 mtxIdHi;  // at 0x1
    u8 mtxIdLo;  // at 0x2
    u8 nodeIdHi; // at 0x1
    u8 nodeIdLo; // at 0x2
};

// MTXDUP opcode layout
struct MtxDupParams {
    u8 opcode;      // at 0x0
    u8 toMtxIdHi;   // at 0x1
    u8 toMtxIdLo;   // at 0x2
    u8 fromMtxIdHi; // at 0x1
    u8 fromMtxIdLo; // at 0x2
};

} // namespace ResByteCodeData

namespace detail {

/******************************************************************************
 *
 * Primitive read/write
 *
 ******************************************************************************/
inline u8 ResRead_u8(const u8* pPtr) {
    return *pPtr;
}

inline u32 ResRead_u32(const u8* pPtr) {
    u32 value = ResRead_u8(pPtr++) << 24;
    value |= ResRead_u8(pPtr++) << 16;
    value |= ResRead_u8(pPtr++) << 8;
    value |= ResRead_u8(pPtr++) << 0;
    return value;
}

inline void ResWrite_u8(u8* pPtr, u8 data) {
    *pPtr = data;
}

inline void ResWrite_u16(u8* pPtr, u16 data) {
    ResWrite_u8(pPtr++, data >> 8);
    ResWrite_u8(pPtr++, data >> 0);
}

inline void ResWrite_u32(u8* pPtr, u32 data) {
    ResWrite_u8(pPtr++, data >> 24);
    ResWrite_u8(pPtr++, data >> 16);
    ResWrite_u8(pPtr++, data >> 8);
    ResWrite_u8(pPtr++, data >> 0);
}

/******************************************************************************
 *
 * GX Blitting Processor (BP)
 *
 ******************************************************************************/
inline void ResReadBPCmd(const u8* pPtr, u32* pOut) {
    // Skip over FIFO command byte
    *pOut = ResRead_u32(pPtr + 1);
}

void ResWriteBPCmd(u8* pPtr, u32 reg);
void ResWriteBPCmd(u8* pPtr, u32 reg, u32 mask);
void ResWriteSSMask(u8* pPtr, u32 value);

/******************************************************************************
 *
 * GX Command Processor (CP)
 *
 ******************************************************************************/
inline void ResReadCPCmd(const u8* pPtr, u32* pOut) {
    // Skip over FIFO command byte + addr byte
    *pOut = ResRead_u32(pPtr + 2);
}

void ResWriteCPCmd(u8* pPtr, u8 addr, u32 value);

/******************************************************************************
 *
 * GX Transform Unit (XF)
 *
 ******************************************************************************/
inline void ResReadXFCmd(const u8* pPtr, u32* pOut) {
    // Skip over FIFO command byte + size short + addr short
    *pOut = ResRead_u32(pPtr + 5);
}

void ResWriteXFCmd(u8* pPtr, u16 addr, u32 value);

/******************************************************************************
 *
 * Utility functions
 *
 ******************************************************************************/
inline GXColor GetRGBA(u8 r, u8 g, u8 b, u8 a) {
    return (GXColor){r, g, b, a};
}
inline GXColorS10 GetRGBAS10(s16 r, s16 g, s16 b, s16 a) {
    return (GXColorS10){r, g, b, a};
}

} // namespace detail
} // namespace g3d
} // namespace nw4r

#endif
