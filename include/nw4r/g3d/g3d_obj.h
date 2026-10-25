#ifndef NW4R_G3D_OBJ_H
#define NW4R_G3D_OBJ_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_rtti.h>
#include <nw4r/g3d/platform/g3d_allocator.h>

namespace nw4r {
namespace g3d {
namespace {

/******************************************************************************
 *
 * Alignment
 *
 ******************************************************************************/
inline u32 align4(u32 x) {
    return ROUND_UP(x, 4);
}
inline u32 align32(u32 x) {
    return ROUND_UP(x, 32);
}

} // namespace

/**
 * \~english @brief Abstract class for basic G3D objects
 * \~japanese @brief 基底のG3Dオブジェクトのための抽象クラス
 */
class G3dObj {
public:
    /**
     * \~english @brief Specifies resources within G3dObj source
     * \~japanese @brief G3dObjソース内リソースを定義する
     */
    template <u32 N> struct ResNameDataT {
        u32 len; // at 0x0
        //! \~english @bug 'N' already includes the null terminator
        //! \~japanese @bug 'N'はもう終端文字を持っている
        char str[ROUND_UP(N + 1, 4)]; // at 0x4
    };

    /**
     * \~english @brief Class that provides type info
     * \~japanese @brief タイプ情報を提供するクラス
     */
    class TypeObj {
    public:
        /**
         * \~english @brief Specifies resources within TypeObj source
         * \~japanese @brief TypeObjソース内リソースを定義する
         */
        struct ResNameDataPT {
            u32 len;     //!< \~ // at 0x0
            char str[4]; //!< \~ // at 0x4
        };

    public:
        /**
         * \~english @brief Constructor
         * \~japanese @brief コンストラクタ
         *
         * \~english @tparam N Size
         * \~japanese @tparam N サイズ
         * \~english @param rName Type name
         * \~japanese @param rName タイプ名
         */
        template <u32 N>
        explicit TypeObj(const ResNameDataT<N>& rName)
            : mName(reinterpret_cast<const ResNameDataPT*>(&rName)) {}

        /**
         * \~english @brief Gets type ID.
         * \~japanese @brief タイプIDを取得する。
         */
        u32 GetTypeID() const {
            //! \~english @note Address is used for comparing TypeObjs
            //! \~japanese @note TypeObjを比較するためにアドレスを使う
            return reinterpret_cast<u32>(mName);
        }

        /**
         * \~english @brief Gets type name.
         * \~japanese @brief タイプ名を取得する。
         */
        const char* GetTypeName() const {
            return mName->str;
        }

        /**
         * \~english @brief Checks if type IDs are equal.
         * \~japanese @brief タイプIDが同じかどうか判定する。
         *
         * \~english @param rOther Right side value of comparison (via TypeObj)
         * \~japanese @param rOther 比較の右辺値 (TypeObjを通じて)
         * \~english @return TRUE if own type ID and rOther's are equal
         * \~japanese @return 自らとrOtherのタイプIDが等しければTRUE
         */
        bool operator==(const TypeObj& rOther) const {
            return GetTypeID() == rOther.GetTypeID();
        }

    private:
        const ResNameDataPT* mName; //!< \~english Type name resource \~japanese
                                    //!< タイプ名リソース \~ // at 0x0
    };

    enum G3dProcTask {
        G3DPROC_NONE = 0x00000,

        G3DPROC_CALC_WORLD = 0x00001,
        G3DPROC_CALC_MAT = 0x00002,
        G3DPROC_CALC_VTX = 0x00003,
        G3DPROC_CALC_VIEW = 0x00004,
        G3DPROC_GATHER_SCNOBJ = 0x00005,
        G3DPROC_DRAW_OPA = 0x00006,
        G3DPROC_DRAW_XLU = 0x00007,
        G3DPROC_UPDATEFRAME = 0x00008,

        // Tasks below cannot be disabled
        __G3DPROC_OPTIONAL_END,

        G3DPROC_CHILD_DETACHED = 0x10001,
        G3DPROC_ATTACH_PARENT = 0x10002,
        G3DPROC_DETACH_PARENT = 0x10003,
        G3DPROC_ZSORT = 0x10004
    };

public:
    virtual bool IsDerivedFrom(TypeObj other) const {
        return other == GetTypeObjStatic();
    } // at 0x8

    virtual void G3dProc(u32 task, u32 param, void* pInfo) = 0; // at 0xC
    /**
     * \~english @brief Destructor
     * \~japanese @brief デストラクタ
     */
    virtual ~G3dObj(); // at 0x10

    virtual const TypeObj GetTypeObj() const {
        return TypeObj(TYPE_NAME);
    } // at 0x14
    static const G3dObj::TypeObj GetTypeObjStatic() {
        return TypeObj(TYPE_NAME);
    }
    virtual const char* GetTypeName() const {
        return GetTypeObj().GetTypeName();
    } // at 0x18

    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     *
     * \~english @param pAllocator Pointer to memory allocator
     * \~japanese @param pAllocator メモリアロケータへのポインタ
     * \~english @param pParent Pointer to parent G3dObj
     * \~japanese @param pParent 親G3dObjへのポインタ
     */
    G3dObj(MEMAllocator* pAllocator, G3dObj* pParent)
        : mpHeap(pAllocator), mpParent(pParent) {}

    /**
     * \~english @brief Gets the pointer to the parent G3dObj.
     * \~japanese @brief 親G3dObjへのポインタを取得する。
     */
    G3dObj* GetParent() const {
        return mpParent;
    }
    /**
     * \~english @brief Sets the parent G3dObj.
     * \~japanese @brief 親G3dObjを設定する。
     *
     * \~english @param pParent Pointer to new parent G3dObj
     * \~japanese @param pParent 新しい親G3dObjへのポインタ
     */
    void SetParent(G3dObj* pParent) {
        mpParent = pParent;
    }

    /**
     * \~english @brief Destroys G3dObj instance.
     * \~japanese @brief G3dObjのインスタンスを破棄する。
     */
    void Destroy();

    static void* Alloc(MEMAllocator* pAllocator, u32 size) {
        return detail::AllocFromAllocator(pAllocator, size);
    }
    static void Dealloc(MEMAllocator* pAllocator, void* pBlock) {
        detail::FreeToAllocator(pAllocator, pBlock);
    }

    static inline void* operator new(size_t /* size */, void* pBlock) {
        return pBlock;
    }
    /**
     * \~english @brief Dummy delete operator.
     * \~japanese @brief ダミーdeleteオペレータ。
     */
    static inline void operator delete(void* /* pBlock */) {}

    template <typename TTo> static TTo* DynamicCast(G3dObj* pObj) {
        return (pObj != NULL && pObj->IsDerivedFrom(TTo::GetTypeObjStatic()))
                   ? static_cast<TTo*>(pObj)
                   : NULL;
    }

private:
    G3dObj* mpParent;     // at 0x4
    MEMAllocator* mpHeap; // at 0x8

    __NW4R_G3D_TYPEOBJ_DECL(G3dObj);
};

} // namespace g3d
} // namespace nw4r

#endif
