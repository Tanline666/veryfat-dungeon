#ifndef NW4R_EF_EFFECT_SYSTEM_H
#define NW4R_EF_EFFECT_SYSTEM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_creationqueue.h>
#include <nw4r/ef/ef_random.h>
#include <nw4r/math.h>

namespace nw4r {
namespace ef {

// Forward declarations
class ActivityList;
class DrawInfo;
class DrawOrderBase;
class DrawStrategyBuilder;
class EmitFormBuilder;
class MemoryManagerBase;

/**
 * \~english @brief Class that manages all effects.
 * \~japanese @brief 全てのエフェクトを管理するクラス。
 */
class EffectSystem {
private:
    MemoryManagerBase* mMemoryManager; // at 0x0

public:
    DrawOrderBase* mDrawOrderFunc;             // at 0x4
    DrawStrategyBuilder* mDrawStrategyBuilder; // at 0x8
    EmitFormBuilder* mEmitFormBuilder;         // at 0xC
    CreationQueue mCreationQueue;              // at 0x10
    u32 mMaxGroupID;                           // at 0x5014
    ActivityList* mActivityList;               // at 0x5018
    Random mRandom;                            // at 0x501C
    math::VEC3 mProcessCameraPos;              // at 0x5020
    math::MTX34 mProcessCameraMtx;             // at 0x502C
    f32 mProcessCameraFar;                     // at 0x505C
    f32 mProcessCameraNear;                    // at 0x5060
    bool mXFFlushSafe;                         // at 0x5064
    Emitter* mTmpEmitter;                      //!< \~ // at 0x5068

    static int mDisplayVersion;
    static EffectSystem instance;

public:
    static EffectSystem* GetInstance();

    EffectSystem();
    ~EffectSystem();

    bool Initialize(u32 maxGroupID);
    bool Closing(Effect* target);

    /**
     * \~english @brief Creates an effect.
     * \~japanese @brief エフェクトを作成する。
     *
     * \~english @param pName Effect name
     * \~japanese @param pName エフェクト名
     * \~english @param groupID Effect group ID
     * \~japanese @param groupID エフェクトのグループID
     * \~english @param calcRemain Elapsed frames
     * \~japanese @param calcRemain 経過したフレーム
     * \~english @return Pointer to created effect
     * \~japanese @return 作成したエフェクトへのポインタ
     */
    Effect* CreateEffect(const char* pName, u32 groupID, u16 calcRemain);
    Effect* CreateEffect(EmitterResource* pResource, u32 groupID,
                         u16 calcRemain);
    u32 RetireEffect(Effect* pEffect);

    u32 RetireEffectAll(u32 groupID);
    u32 RetireEmitterAll(u32 groupID);
    u32 RetireParticleAll(u32 groupID);

    /**
     * \~english @brief Animates registered groups of effects.
     * \~japanese @brief エフェクトの指定されたグループをアニメートする。
     *
     * @param groupID
     * @param option
     */
    void Calc(u32 groupID, CalcOption option);
    void Draw(const DrawInfo& rInfo, u32 groupID);

    void SetProcessCamera(const math::VEC3& position, const math::MTX34& matrix,
                          f32 nnear, f32 ffar);

    MemoryManagerBase* GetMemoryManager() const {
        return mMemoryManager;
    }
    void SetMemoryManager(MemoryManagerBase* pManager, u32 maxGroupID) {
        mMemoryManager = pManager;

        if (mMemoryManager != NULL) {
            Initialize(maxGroupID);
        }
    }

    void SetXFFlushSafe(bool safe) {
        mXFFlushSafe = safe;
    }

    typedef bool (*SortEffectOperatorLess)(const Effect* op1,
                                           const Effect* op2);

    void SortEffect(SortEffectOperatorLess less, u32 groupID);

protected:
    static DrawInfo mSortEffectOperatorZDrawInfo;

    static bool SortEffectOperatorZ(const Effect* op1, const Effect* op2);
};

} // namespace ef
} // namespace nw4r

#endif
