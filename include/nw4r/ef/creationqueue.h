#ifndef NW4R_EF_CREATION_QUEUE_H
#define NW4R_EF_CREATION_QUEUE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/emitter.h>
#include <nw4r/ef/types.h>

namespace nw4r {
namespace ef {

/**
 * @~english @brief Effects creation queue.
 * @~japanese @brief エフェクトの生成キュー。
 */
class CreationQueue {
public:
    enum Type { TYPE_PARTICLE, TYPE_EMITTER };

private:
    /**
     * @~english @brief Effect creation queue data.
     * @~japanese @brief エフェクトの生成キューのデータ。
     */
    struct CreationQueueData {
        enum OPTION {
            OPTION_USE_POSITION = 1 << 0,
            OPTION_USE_VELOCITY = 1 << 1,
        };

        u8 mType;  //!< @~english Effect type @~japanese エフェクト型 \~ // at
                   //!< 0x0
        u8 mFlags; //!< @~english Effect flags (represented with OPTION enum)
                   //!< @~japanese エフェクトのフラグ (OPTIONのenumで表現) \~ //
                   //!< at 0x1
        u16 mCalcRemain; //!< @~english Elapsed frames @~japanese
                         //!< 経過したフレーム \~ // at 0x2
        EmitterInheritSetting mInheritSetting; //!< // at 0x4
        Particle* mReferenceParticle;          //!< // at 0x10
        EmitterResource*
            mEmitterResource; //!< @~english Pointer to EmitterResource object
                              //!< @~japanese
                              //!< EmitterResourceオブジェクトへのポインタ // at
                              //!< 0x14
        math::VEC3
            mPosition; //!< @~english Effect position when generated
                       //!< @~japanese 発生時のエフェクト位置 \~ // at 0x18
        math::VEC3 mVelocity; //!< @~english Initial velocity of effect
                              //!< @~japanese エフェクトの初速度 \~ // at 0x24
    };

private:
    /**
     * @~english @brief Upper limit on the amount of child effects that can be
     * created during a single process loop.
     * @~japanese @brief 一回の処理で生成可能なチャイルドエフェクトの上限。
     */
    static const int QUEUE_SIZE =
        MAX(NW4R_EF_MAX_PARTICLE, NW4R_EF_MAX_EMITTER);

private:
    int mNumItem;                             //!< // at 0x0
    CreationQueueData mQueueData[QUEUE_SIZE]; //!< // at 0x4

public:
    /**
     * @~english @brief Constructor
     * @~japanese @brief コンストラクタ
     */
    CreationQueue();

    /**
     * @~english @brief Adds a particle to the creation queue.
     * @~japanese @brief パーティクルを生成キューへ追加する。
     *
     * @param setting
     * @~english @param caller Pointer to reference particle
     * @~japanese @param caller 参照パーティクルへのポインタ
     * @~english @param eh Pointer to EmitterResource
     * @~japanese @param eh EmitterResourceへのポインタ
     * @param calcRemain
     * @~english @param position Pointer to vector storing particle position
     * (default is NULL)
     * @~japanese @param position
     * パーティクルの位置を格納するベクトルへのポインタ（デフォルトはNULL）
     * @~english @param velocity Pointer to vector storing particle velocity
     * (default is NULL)
     * @~japanese @param velocity
     * パーティクルの速度を格納するベクトルへのポインタ（デフォルトはNULL）
     */
    void AddParticleCreation(const EmitterInheritSetting* setting,
                             Particle* caller, EmitterResource* eh,
                             u16 calcRemain, const math::VEC3* position = NULL,
                             const math::VEC3* velocity = NULL);

    /**
     * @~english @brief Adds an emitter to the creation queue.
     * @~japanese @brief エミッタを生成キューへ追加する。
     *
     * @param setting
     * @~english @param caller Pointer to reference particle
     * @~japanese @param caller 参照パーティクルへのポインタ
     * @~english @param eh Pointer to EmitterResource
     * @~japanese @param eh EmitterResourceへのポインタ
     * @param calcRemain
     * @~english @param position Pointer to vector storing emitter position
     * (default is NULL)
     * @~japanese @param position
     * エミッタの位置を格納するベクトルへのポインタ（デフォルトはNULL）
     * @~english @param velocity Pointer to vector storing emitter velocity
     * (default is NULL)
     * @~japanese @param velocity
     * エミッタの速度を格納するベクトルへのポインタ（デフォルトはNULL）
     */
    void AddEmitterCreation(const EmitterInheritSetting* setting,
                            Particle* caller, EmitterResource* eh,
                            u16 calcRemain, const math::VEC3* position = NULL,
                            const math::VEC3* velocity = NULL);

    /**
     * @~english @brief Creates the effects within the queue.
     * @~japanese @brief キュー中のエフェクトを生成する。
     */
    void Execute();

    /**
     * @~english @brief Checks if there are no items in the creation queue.
     * @~japanese @brief 生成キューにはアイテムがなしかどうかを調べる。
     *
     * @~english @return TRUE if empty
     * @~japanese @return 空っぽならTRUE
     */
    bool IsEmpty() {
        return mNumItem == 0;
    }
};

} // namespace ef
} // namespace nw4r

#endif
