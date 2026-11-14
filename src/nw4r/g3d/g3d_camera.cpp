#include <nw4r/g3d.h>
#include <nw4r/math.h>

#include <revolution/GX.h>
#include <revolution/MTX.h>

namespace nw4r {
namespace g3d {

Camera::Camera(CameraData* pData) : ResCommon(pData) {}

void Camera::Init() {
    const GXRenderModeObj* rmode = G3DState::GetRenderModeObj();

    NW4R_NULL_ASSERT(rmode, 45);

    Init(rmode->fbWidth, rmode->efbHeight, rmode->fbWidth, rmode->xfbHeight,
         rmode->viWidth, rmode->viHeight);
}

void Camera::Init(u16 efbWidth, u16 efbHeight, u16 xfbWidth, u16 xfbHeight,
                  u16 viWidth, u16 viHeight) {
    NW4R_ASSERT(IsValid(), 57);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.flags = CameraData::FLAG_CAM_LOOKAT | CameraData::FLAG_PROJ_PERSP;

    c.cameraPos.x = 0.0f;
    c.cameraPos.y = 0.0f;
    c.cameraPos.z = 15.0f;
    c.cameraUp.x = 0.0f;
    c.cameraUp.y = 1.0f;
    c.cameraUp.z = 0.0f;
    c.cameraTarget.x = 0.0f;
    c.cameraTarget.y = 0.0f;
    c.cameraTarget.z = 0.0f;
    c.cameraRotate.x = 0.0f;
    c.cameraRotate.y = 0.0f;
    c.cameraRotate.z = 0.0f;
    c.cameraTwist = 0.0f;

    c.projType = GX_PERSPECTIVE;
    c.projFovy = 60.0f;
    c.projAspect = 4.0f / 3.0f;
    c.projNear = 0.1f;
    c.projFar = 1000.f;
    c.projTop = 0.0f;
    c.projBottom = static_cast<f32>(viHeight);
    c.projLeft = 0.0f;
    c.projRight = static_cast<f32>(viWidth);

    c.lightScaleS = 0.5f;
    c.lightScaleT = 0.5f;
    c.lightTransS = 0.5f;
    c.lightTransT = 0.5f;

    c.viewportOrigin.x = 0.0f;
    c.viewportOrigin.y = 0.0f;
    c.viewportSize.x = static_cast<f32>(xfbWidth);
    c.viewportSize.y = static_cast<f32>(xfbHeight);
    c.viewportNear = 0.0f;
    c.viewportFar = 1.0f;

    c.scissorX = 0;
    c.scissorY = 0;
    c.scissorWidth = efbWidth;
    c.scissorHeight = efbHeight;
    c.scissorOffsetX = 0;
    c.scissorOffsetY = 0;
}

void Camera::SetPosition(f32 x, f32 y, f32 z) {
    NW4R_ASSERT(IsValid(), 127);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.cameraPos.x = x;
    c.cameraPos.y = y;
    c.cameraPos.z = z;
    c.flags &= ~CameraData::FLAG_CAM_MTX_READY;
}

void Camera::SetPosition(const math::VEC3& pos) {
    NW4R_ASSERT(IsValid(), 145);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.cameraPos = pos;
    c.flags &= ~CameraData::FLAG_CAM_MTX_READY;
}

void Camera::SetPosture(const PostureInfo& info) {
    NW4R_ASSERT(IsValid(), 191);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    switch (info.tp) {
    case POSTURE_LOOKAT: {
        if (c.flags & CameraData::FLAG_CAM_LOOKAT) {
            if (!(info.cameraUp != c.cameraUp) &&
                !(info.cameraTarget != c.cameraTarget)) {
                return;
            }
        }

        c.flags &= ~(CameraData::FLAG_CAM_LOOKAT | CameraData::FLAG_CAM_ROTATE |
                     CameraData::FLAG_CAM_AIM);

        c.flags |= CameraData::FLAG_CAM_LOOKAT;

        c.cameraUp = info.cameraUp;
        c.cameraTarget = info.cameraTarget;

        c.flags &= ~CameraData::FLAG_CAM_MTX_READY;
        break;
    }

    case POSTURE_ROTATE: {
        if (c.flags & CameraData::FLAG_CAM_ROTATE) {
            if (!(info.cameraRotate != c.cameraRotate)) {
                return;
            }
        }

        c.flags &= ~(CameraData::FLAG_CAM_LOOKAT | CameraData::FLAG_CAM_ROTATE |
                     CameraData::FLAG_CAM_AIM);

        c.flags |= CameraData::FLAG_CAM_ROTATE;

        c.cameraRotate = info.cameraRotate;

        c.flags &= ~CameraData::FLAG_CAM_MTX_READY;
        break;
    }

    case POSTURE_AIM: {
        if (c.flags & CameraData::FLAG_CAM_AIM) {
            if (!(info.cameraTarget != c.cameraTarget) &&
                info.cameraTwist == c.cameraTwist) {
                return;
            }
        }

        c.flags &= ~(CameraData::FLAG_CAM_LOOKAT | CameraData::FLAG_CAM_ROTATE |
                     CameraData::FLAG_CAM_AIM);

        c.flags |= CameraData::FLAG_CAM_AIM;

        c.cameraTarget = info.cameraTarget;
        c.cameraTwist = info.cameraTwist;

        c.flags &= ~CameraData::FLAG_CAM_MTX_READY;
        break;
    }

    default: {
        break;
    }
    }
}

void Camera::SetCameraMtxDirectly(const math::MTX34& mtx) {
    NW4R_ASSERT(IsValid(), 276);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    math::MTX34Copy(&c.cameraMtx, &mtx);
    c.flags |= CameraData::FLAG_CAM_MTX_READY;
}

void Camera::SetPerspective(f32 fovy, f32 aspect, f32 near, f32 far) {
    NW4R_ASSERT(IsValid(), 294);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.projType = GX_PERSPECTIVE;

    c.projFovy = fovy;
    c.projAspect = aspect;
    c.projNear = near;
    c.projFar = far;

    c.flags &= ~(CameraData::FLAG_PROJ_FRUSTUM | CameraData::FLAG_PROJ_PERSP |
                 CameraData::FLAG_PROJ_ORTHO | CameraData::FLAG_PROJ_MTX_READY);

    c.flags |= CameraData::FLAG_PROJ_PERSP;
}

void Camera::SetOrtho(f32 top, f32 bottom, f32 left, f32 right, f32 near,
                      f32 far) {
    NW4R_ASSERT(IsValid(), 336);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.projType = GX_ORTHOGRAPHIC;

    c.projTop = top;
    c.projBottom = bottom;
    c.projLeft = left;
    c.projRight = right;
    c.projNear = near;
    c.projFar = far;

    c.flags &= ~(CameraData::FLAG_PROJ_FRUSTUM | CameraData::FLAG_PROJ_PERSP |
                 CameraData::FLAG_PROJ_ORTHO | CameraData::FLAG_PROJ_MTX_READY);

    c.flags |= CameraData::FLAG_PROJ_ORTHO;
}

void Camera::SetProjectionMtxDirectly(const math::MTX44* pMtx) {
    NW4R_ASSERT(IsValid(), 358);

    if (pMtx != NULL && IsValid()) {
        CameraData& c = ref();

        math::MTX44Copy(&c.projMtx, pMtx);
        c.flags |= CameraData::FLAG_PROJ_MTX_READY;
    }
}

void Camera::SetScissor(u32 x, u32 y, u32 width, u32 height) {
    NW4R_ASSERT(IsValid(), 395);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.scissorX = x;
    c.scissorY = y;
    c.scissorWidth = width;
    c.scissorHeight = height;
}

void Camera::SetScissorBoxOffset(s32 ox, s32 oy) {
    NW4R_ASSERT(IsValid(), 413);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.scissorOffsetX = ox;
    c.scissorOffsetY = oy;
}

void Camera::SetViewport(f32 x, f32 y, f32 width, f32 height) {
    NW4R_ASSERT(IsValid(), 428);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.viewportOrigin.x = x;
    c.viewportOrigin.y = y;
    c.viewportSize.x = width;
    c.viewportSize.y = height;

    SetScissor(static_cast<u32>(x), static_cast<u32>(y),
               static_cast<u32>(width), static_cast<u32>(height));
}

void Camera::SetViewportZRange(f32 near, f32 far) {
    NW4R_ASSERT(IsValid(), 450);

    if (!IsValid()) {
        return;
    }

    CameraData& c = ref();

    c.viewportNear = near;
    c.viewportFar = far;
}

void Camera::GetViewport(f32* pX, f32* pY, f32* pWidth, f32* pHeight,
                         f32* pNear, f32* pFar) const {
    NW4R_ASSERT(IsValid(), 486);

    if (!IsValid()) {
        return;
    }

    const CameraData& c = ref();

    if (pX != NULL) {
        *pX = c.viewportOrigin.x;
    }
    if (pY != NULL) {
        *pY = c.viewportOrigin.y;
    }

    if (pWidth != NULL) {
        *pWidth = c.viewportSize.x;
    }
    if (pHeight != NULL) {
        *pHeight = c.viewportSize.y;
    }

    if (pNear != NULL) {
        *pNear = c.viewportNear;
    }
    if (pFar != NULL) {
        *pFar = c.viewportFar;
    }
}

void Camera::GetCameraMtx(math::MTX34* pMtx) const {
    NW4R_ASSERT(IsValid(), 594);

    if (pMtx != NULL && IsValid()) {
        const CameraData& c = ref();

        if (!(c.flags & CameraData::FLAG_CAM_MTX_READY)) {
            UpdateCameraMtx();
        }

        math::MTX34Copy(pMtx, &c.cameraMtx);
    }
}

void Camera::GetProjectionMtx(math::MTX44* pMtx) const {
    NW4R_ASSERT(IsValid(), 612);

    if (pMtx != NULL && IsValid()) {
        const CameraData& c = ref();

        if (!(c.flags & CameraData::FLAG_PROJ_MTX_READY)) {
            UpdateProjectionMtx();
        }

        math::MTX44Copy(pMtx, &c.projMtx);
    }
}

void Camera::GetProjectionTexMtx(math::MTX34* pMtx) const {
    NW4R_ASSERT(IsValid(), 630);

    if (pMtx != NULL && IsValid()) {
        const CameraData& c = ref();

        if (c.flags & CameraData::FLAG_PROJ_ORTHO) {
            C_MTXLightOrtho(*pMtx, c.projTop, c.projBottom, c.projLeft,
                            c.projRight, c.lightScaleS, -c.lightScaleT,
                            c.lightTransS, c.lightTransT);
        } else if (c.flags & CameraData::FLAG_PROJ_FRUSTUM) {
            C_MTXLightFrustum(*pMtx, c.projTop, c.projBottom, c.projLeft,
                              c.projRight, c.projNear, c.lightScaleS,
                              -c.lightScaleT, c.lightTransS, c.lightTransT);
        } else /* FLAG_PROJ_PERSP */ {
            C_MTXLightPerspective(*pMtx, c.projFovy, c.projAspect,
                                  c.lightScaleS, -c.lightScaleT, c.lightTransS,
                                  c.lightTransT);
        }
    }
}

void Camera::GetEnvironmentTexMtx(math::MTX34* pMtx) const {
    NW4R_ASSERT(IsValid(), 678);

    if (pMtx != NULL && IsValid()) {
        const CameraData& c = ref();

        math::MTX34Identity(pMtx);
        pMtx->_00 = c.lightScaleS;
        pMtx->_03 = c.lightTransS;
        pMtx->_11 = -c.lightScaleT;
        pMtx->_13 = c.lightTransT;
        pMtx->_22 = 0.0f;
        pMtx->_23 = 1.0f;
    }
}

void Camera::GXSetViewport() const {
    NW4R_ASSERT(IsValid(), 701);

    if (!IsValid()) {
        return;
    }

    const CameraData& c = ref();
    const GXRenderModeObj* pObj = G3DState::GetRenderModeObj();

    if (pObj->field_rendering) {
        ::GXSetViewportJitter(
            c.viewportOrigin.x, c.viewportOrigin.y, c.viewportSize.x,
            c.viewportSize.y, c.viewportNear, c.viewportFar,
            c.flags & CameraData::FLAG_VI_ODD_FIELD ? GX_FIELD_ODD
                                                    : GX_FIELD_EVEN);
    } else {
        ::GXSetViewport(c.viewportOrigin.x, c.viewportOrigin.y,
                        c.viewportSize.x, c.viewportSize.y, c.viewportNear,
                        c.viewportFar);
    }
}

void Camera::GXSetProjection() const {
    NW4R_ASSERT(IsValid(), 738);

    if (!IsValid()) {
        return;
    }

    const CameraData& c = ref();

    if (!(c.flags & CameraData::FLAG_PROJ_MTX_READY)) {
        UpdateProjectionMtx();
    }

    ::GXSetProjection(c.projMtx, c.projType);
}

void Camera::GXSetScissor() const {
    NW4R_ASSERT(IsValid(), 752);

    if (!IsValid()) {
        return;
    }

    const CameraData& c = ref();
    ::GXSetScissor(c.scissorX, c.scissorY, c.scissorWidth, c.scissorHeight);
}

void Camera::GXSetScissorBoxOffset() const {
    NW4R_ASSERT(IsValid(), 771);

    if (!IsValid()) {
        return;
    }

    const CameraData& c = ref();
    ::GXSetScissorBoxOffset(c.scissorOffsetX, c.scissorOffsetY);
}

void Camera::UpdateCameraMtx() const {
    CameraData& c = const_cast<CameraData&>(ref());

    if (c.flags & CameraData::FLAG_CAM_LOOKAT) {
        NW4R_ASSERTMSG(c.cameraPos != c.cameraTarget, 794,
                       "nw4r::g3d::Camera::UpdateCameraMtx:\n"
                       "camera position and target are the same(%f %f %f).",
                       c.cameraPos.x, c.cameraPos.y, c.cameraPos.z);
        C_MTXLookAt(c.cameraMtx, c.cameraPos, c.cameraUp, c.cameraTarget);
    } else if (c.flags & CameraData::FLAG_CAM_AIM) {
        math::MTX34& rMtx = c.cameraMtx;
        math::VEC3& rPos = c.cameraPos;
        math::VEC3& rTarget = c.cameraTarget;

        math::VEC3 back(rPos.x - rTarget.x, rPos.y - rTarget.y,
                        rPos.z - rTarget.z);

        if (back.x == 0.0f && back.z == 0.0f) {
            rMtx._00 = 1.0f;
            rMtx._01 = 0.0f;
            rMtx._02 = 0.0f;
            rMtx._03 = -rPos.x;

            rMtx._10 = 0.0f;
            rMtx._11 = 0.0f;
            rMtx._20 = 0.0f;
            rMtx._22 = 0.0f;

            if (back.y <= 0.0f) {
                rMtx._12 = 1.0f;
                rMtx._13 = -rPos.z;
                rMtx._21 = -1.0f;
                rMtx._23 = rPos.y;
            } else {
                rMtx._12 = -1.0f;
                rMtx._13 = rPos.z;
                rMtx._21 = 1.0f;
                rMtx._23 = -rPos.y;
            }
        } else {
            math::VEC3 _r(back.z, 0.0f, -back.x);

            math::VEC3 u;
            math::VEC3Normalize(&back, &back);
            math::VEC3Normalize(&_r, &_r);
            math::VEC3Cross(&u, &back, &_r);

            f32 st, ct;
            math::SinCosDeg(&st, &ct, c.cameraTwist);

            math::VEC3 right, up;
            right.x = st * u.x + ct * _r.x;
            right.y = st * u.y;
            right.z = st * u.z + ct * _r.z;

            up.x = ct * u.x - st * _r.x;
            up.y = ct * u.y;
            up.z = ct * u.z - st * _r.z;

            rMtx._00 = right.x;
            rMtx._01 = right.y;
            rMtx._02 = right.z;
            rMtx._03 = -math::VEC3Dot(&rPos, &right);

            rMtx._10 = up.x;
            rMtx._11 = up.y;
            rMtx._12 = up.z;
            rMtx._13 = -math::VEC3Dot(&rPos, &up);

            rMtx._20 = back.x;
            rMtx._21 = back.y;
            rMtx._22 = back.z;
            rMtx._23 = -math::VEC3Dot(&rPos, &back);
        }
    } else /* FLAG_CAM_ROTATE */ {
        NW4R_ASSERTMSG((c.flags & CameraData::FLAG_CAMERA_ROTATE), 891,
                       "Unknown camera type(%x)", c.flags);

        math::MTX34& rMtx = c.cameraMtx;
        math::VEC3& rPos = c.cameraPos;

        f32 sx, sy, sz, cx, cy, cz;
        math::SinCosDeg(&sx, &cx, c.cameraRotate.x);
        math::SinCosDeg(&sy, &cy, c.cameraRotate.y);
        math::SinCosDeg(&sz, &cz, c.cameraRotate.z);

        math::VEC3 right, up, back;

        right.x = sx * sy * sz + cy * cz;
        right.y = cx * sz;
        right.z = sx * cy * sz - sy * cz;

        up.x = sx * sy * cz - cy * sz;
        up.y = cx * cz;
        up.z = sx * cy * cz + sy * sz;

        back.x = cx * sy;
        back.y = -sx;
        back.z = cx * cy;

        rMtx._00 = right.x;
        rMtx._01 = right.y;
        rMtx._02 = right.z;
        rMtx._03 = -math::VEC3Dot(&rPos, &right);

        rMtx._10 = up.x;
        rMtx._11 = up.y;
        rMtx._12 = up.z;
        rMtx._13 = -math::VEC3Dot(&rPos, &up);

        rMtx._20 = back.x;
        rMtx._21 = back.y;
        rMtx._22 = back.z;
        rMtx._23 = -math::VEC3Dot(&rPos, &back);
    }

    c.flags |= CameraData::FLAG_CAM_MTX_READY;
}

void Camera::UpdateProjectionMtx() const {
    CameraData& c = const_cast<CameraData&>(ref());

    if (c.flags & CameraData::FLAG_PROJ_ORTHO) {
        C_MTXOrtho(c.projMtx, c.projTop, c.projBottom, c.projLeft, c.projRight,
                   c.projNear, c.projFar);
    } else if (c.flags & CameraData::FLAG_PROJ_FRUSTUM) {
        C_MTXFrustum(c.projMtx, c.projTop, c.projBottom, c.projLeft,
                     c.projRight, c.projNear, c.projFar);
    } else /* FLAG_PROJ_PERSP */ {
        C_MTXPerspective(c.projMtx, c.projFovy, c.projAspect, c.projNear,
                         c.projFar);
    }

    c.flags |= CameraData::FLAG_PROJ_MTX_READY;
}

} // namespace g3d
} // namespace nw4r
