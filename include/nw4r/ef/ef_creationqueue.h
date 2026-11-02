#ifndef NW4R_EF_CREATION_QUEUE_H
#define NW4R_EF_CREATION_QUEUE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_types.h>

namespace nw4r {
namespace ef {

/**
 * \~english @brief Effects creation queue.
 * \~japanese @brief エフェクトの生成キュー。
 */
class CreationQueue {
public:
    enum Type { TYPE_PARTICLE, TYPE_EMITTER };

private:
    /**
     * \~english @brief Effect creation queue data.
     * \~japanese @brief エフェクトの生成キューのデータ。
     */
    struct CreationQueueData {
        enum OPTION {
            OPTION_USE_POSITION = 1 << 0,
            OPTION_USE_VELOCITY = 1 << 1,
        };

        u8 mType;  //!< \~english Effect type \~japanese エフェクト型 \~ // at
                   //!< 0x0
        u8 mFlags; //!< \~english Effect flags (represented with OPTION enum)
                   //!< \~japanese エフェクトのフラグ (OPTIONのenumで表現) \~ //
                   //!< at 0x1
        u16 mCalcRemain; //!< \~english Elapsed frames \~japanese
                         //!< 経過したフレーム \~ // at 0x2
        EmitterInheritSetting mInheritSetting; //!< // at 0x4
        Particle* mReferenceParticle;          //!< // at 0x10
        EmitterResource*
            mEmitterResource; //!< \~english Pointer to EmitterResource object
                              //!< \~japanese
                              //!< EmitterResourceオブジェクトへのポインタ // at
                              //!< 0x14
        math::VEC3
            mPosition; //!< \~english Effect position when generated
                       //!< \~japanese 発生時のエフェクト位置 \~ // at 0x18
        math::VEC3 mVelocity; //!< \~english Initial velocity of effect
                              //!< \~japanese エフェクトの初速度 \~ // at 0x24
    };

private:
    /**
     * \~english @brief Upper limit on the amount of child effects that can be
     * created during a single process loop.
     * \~japanese @brief 一回の処理で生成可能なチャイルドエフェクトの上限。
     */
    static const int QUEUE_SIZE =
        MAX(NW4R_EF_MAX_PARTICLE, NW4R_EF_MAX_EMITTER);

private:
    int mNumItem;                             //!< // at 0x0
    CreationQueueData mQueueData[QUEUE_SIZE]; //!< // at 0x4

public:
    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     */
    CreationQueue();

    void AddParticleCreation(const EmitterInheritSetting* pSetting,
                             Particle* pParticle, EmitterResource* pResource,
                             u16 calcRemain, const math::VEC3* pPosition = NULL,
                             const math::VEC3* pVelocity = NULL);

    void AddEmitterCreation(const EmitterInheritSetting* pSetting,
                            Particle* pParticle, EmitterResource* pResource,
                            u16 calcRemain, const math::VEC3* pPosition = NULL,
                            const math::VEC3* pVelocity = NULL);

    void Execute();

    bool IsEmpty() {
        return mNumItem == 0;
    }
};

} // namespace ef
} // namespace nw4r

#endif
