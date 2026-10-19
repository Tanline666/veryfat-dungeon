#include <Pack/RPGraphics.h>

#include <nw4r/g3d.h>

/**
 * \~english @brief Applies light settings from the specified scene resource
 *
 * \~english @param handle File resource handle
 * \~english @param pName Scene file name
 * \~english @param frame Animation frame (defaults to the first frame)
 * \~english @param lightSetRef Light set reference number (defaults to index 0)
 */
void RPGrpLightManager::LoadScnLight(RPGrpHandle handle, const char* pName,
                                     f32 frame, u32 lightSetRef) {

    const nw4r::g3d::ResAnmScn scn =
        RPGrpModelResManager::GetCurrent()
            ->GetData<RPGrpModelResManager::Type_ResAnmScn>(handle, pName);

    LoadScnLightInner(scn, frame, lightSetRef);
}
