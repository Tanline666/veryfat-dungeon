#ifndef NW4R_EF_EMFORM_CUBE_H
#define NW4R_EF_EMFORM_CUBE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_emitterform.h>

namespace nw4r {
namespace ef {

class EmitterFormCube : public EmitterForm {
public:
    EmitterFormCube() {}

    virtual void Emission(Emitter* pEmitter, ParticleManager* pManager,
                          int count, u32 flags, f32* pParams, u16 life,
                          f32 lifeRnd, const math::MTX34* pSpace); // at 0x8

public:
    enum ParametersCubeIndex {
        PARAM_CUBE_X,
        PARAM_CUBE_Y,
        PARAM_CUBE_Z,
        PARAM_CUBE_INNER,
        PARAM_CUBE_MAX
    };

    enum OptionsCube {
        OPTION_CUBE_FLAT_DENSITY = (1 << 24),
        OPTION_CUBE_LINKED_SIZE = (1 << 25)
    };

private:
    void EmissionSub(math::VEC3& rPos, math::VEC3& rNormal, Emitter* pEmitter,
                     ParticleManager* pManager, u16 life, f32 lifeRnd,
                     const math::MTX34* pSpace);
};

} // namespace ef
} // namespace nw4r

#endif
