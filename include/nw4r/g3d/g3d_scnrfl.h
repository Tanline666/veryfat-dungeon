#ifndef NW4R_G3D_SCN_RFL_H
#define NW4R_G3D_SCN_RFL_H
#include <nw4r/types_nw4r.h>

#include <RVLFaceLib.h>

#include <nw4r/g3d/g3d_scnobj.h>

namespace nw4r {
namespace g3d {

class ScnRfl : public ScnLeaf {
public:
    typedef void (*DrawProc)(ScnRfl* pScnRfl, const RFLCharModel* pModel,
                             u32 diffMask, u32 specMask, GXColor ambColor,
                             bool opa);

private:
    class RflData {
    public:
        static RflData* Construct(MEMAllocator* pHeap, u32* pSize,
                                  RFLResolution resolution, u32 expressionFlag);

    private:
        RflData(MEMAllocator* pHeap, void* pBuffer, RFLResolution resolution,
                u32 expressionFlag);

        void* operator new(size_t, void* buf) {
            return buf;
        }
        void operator delete(void*) {}

    private:
        int mRefCount;
        MEMAllocator* const mpHeap;
        const RFLResolution mResolution;
        const u32 mExpressionFlag;
        s8 mCharModelReady;
        void* const mpNglBuffer;
        RFLErrcode mRFLErrCode;
        RFLCharModel mCharModel;
    };

public:
    static ScnRfl* Construct(MEMAllocator* pAllocator, u32* pSize,
                             RFLResolution resolution, u32 exprFlags,
                             u32 userDataSize = 0);

protected:
    ScnRfl(MEMAllocator* pAllocator, void* pNglBuffer, void* pUserData,
           RFLResolution resolution, u32 exprFlag);

public:
    virtual void G3dProc(u32 task, u32 param, void* pInfo);

    //! Major matching hack for RTTI functions!
    virtual bool IsDerivedFrom(TypeObj other) const {
        return other == GetTypeObjStatic() ? true
                                           : ScnLeaf::IsDerivedFrom(other);
    }

    virtual const char* GetTypeName() const {
        return GetTypeObj().GetTypeName();
    }

    virtual const TypeObj GetTypeObj() const {
        return TypeObj(TYPE_NAME);
    }
    static const TypeObj GetTypeObjStatic() {
        return TypeObj(TYPE_NAME);
    }

private:
    static const ResNameDataT<sizeof("ScnRfl")> TYPE_NAME;

protected:
    virtual ~ScnRfl() {
        ReleaseCharModel();
    }

public:
    bool SetupCharModel(RFLDataSource src, u16 index, RFLMiddleDB* pMiddleDB);
    void ReleaseCharModel();

    bool SetExpression(RFLExpression expression);
    bool GetExpression(RFLExpression* pExpression);

    u32 GetExpressionFlag() const {
        return mExpressionFlag;
    }

    void SetLightSetIdx(int idx);
    s8 GetLightSetIdx() const {
        return mIdxLightSet;
    }

    void SetFogIdx(int idx);
    s8 GetFogIdx() const {
        return mIdxFog;
    }

    void GXSetZCompLoc(GXBool beforeTex) {
        mZCompLoc = beforeTex;
    }
    GXBool GXGetZCompLoc() const {
        return mZCompLoc;
    }

    bool IsCharModelReady() const {
        return mCharModelReady != FALSE;
    }

    void SetAmbientColor(GXColor color) {
        mAmbientColor = color;
    }
    GXColor GetAmbientColor() const {
        return mAmbientColor;
    }

    GXDiffuseFn GetDiffuseFunc() const {
        return mDiffuseFn;
    }
    GXAttnFn GetAttnFunc() const {
        return mAttnFn;
    }

    void SetDrawProc(DrawProc pProc) {
        mpDrawProc = pProc;
    }
    DrawProc GetDrawProc() const {
        return mpDrawProc;
    }

    void* GetUserData() const {
        return mpUserData;
    }

private:
    void CallDrawProc(bool opa);
    void LoadDrawSetting();

private:
    RFLResolution mResolution; // at 0xE8
    u32 mExpressionFlag;       // at 0xEC
    s8 mIdxLightSet;           // at 0xF0
    s8 mIdxFog;                // at 0xF1
    GXBool mZCompLoc;          // at 0xF2
    s8 mCharModelReady;        // at 0xF3
    GXColor mAmbientColor;     // at 0xF4
    GXDiffuseFn mDiffuseFn;    // at 0xF8
    GXAttnFn mAttnFn;          // at 0xFC
    void* mpNglBuffer;         // at 0x100
    RFLErrcode mRFLErrCode;    // at 0x104
    DrawProc mpDrawProc;       // at 0x108
    void* mpUserData;          // at 0x10C
    RFLCharModel mCharModel;   // at 0x110
};

} // namespace g3d
} // namespace nw4r

#endif
