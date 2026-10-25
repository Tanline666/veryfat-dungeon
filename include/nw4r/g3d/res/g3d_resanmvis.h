#ifndef NW4R_G3D_RES_RES_ANM_VIS_H
#define NW4R_G3D_RES_RES_ANM_VIS_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>

namespace nw4r {
namespace g3d {

struct ResAnmVisAnmData {
    enum Flag { FLAG_ENABLE = (1 << 0), FLAG_CONST = (1 << 1) };

    s32 name;                        //!< at 0x0
    u32 flags;                       //!< at 0x4
    ResBoolAnmFramesData visibility; //!< at 0x8
};

/**
 * \~english @brief Info for visibility animation data
 * \~japanese @brief ビジビリティアニメデータの情報
 */
struct ResAnmVisInfoData {
    u16 numFrame; //!< \~english Number of frames in animation \~japanese
                  //!< アニメのフレーム数 \~ // at 0x0
    u16 numNode;  //!< \~english Number of nodes with defined animations
                  //!< \~japanese アニメが定義されているノード数 \~ // at 0x2
    AnmPolicy policy; //!< \~english Animation replay policy \~japanese
                      //!< アニメ再生ポリシー \~ // at 0x4
};

/**
 * \~english @brief Visibility animation data
 * \~japanese @brief ビジビリティアニメデータ
 */
struct ResAnmVisData {
    ResBlockHeaderData header; //!< \~english Common header \~japanese
                               //!< 共通ヘッダー \~ // at 0x0
    u32 revision; //!< \~english Version number \~japanese バージョン番号 \~ //
                  //!< at 0x8
    s32 toResFileData; //!< \~english Offset to parent ResFileData \~japanese
                       //!< 親ResFileDataへのオフセット \~ // at 0xC
    s32 toVisDataDic;  //!< at 0x10
    s32 toResUserData; //!< at 0x14
    s32 name; //!< \~english File name of rvia source (without extension)
              //!< \~japanese 元となったrviaファイル名 (拡張子除く) \~ // at
              //!< 0x18
    s32 original_path; //!< \~english File path to rvia source \~japanese
                       //!< 元となったrviaファイルのパス \~ // at 0x1C
    ResAnmVisInfoData
        info; //!< \~english Animation info \~japanese アニメ情報 \~ // at 0x20
};

class ResAnmVis : public ResCommon<ResAnmVisData> {
public:
    static const u32 SIGNATURE = FOURCC('V', 'I', 'S', '0');
    static const int REVISION = 4;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmVis);

    /**
     * \~english @brief Returns the binary revision.
     * \~japanese @brief バイナリリビジョンを取得する。
     */
    u32 GetRevision() const {
        return ref().revision;
    }

    /**
     * \~english @brief Checks the binary revision.
     * \~japanese @brief バイナリリビジョンをチェックする。
     * \~english @return TRUE if file revision matches header revision
     * \~japanese @return ファイルリビジョンがヘッダーリビジョンと同じならTRUE
     */
    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    bool GetAnmResult(u32 idx, f32 frame) const;

    const ResAnmVisAnmData* GetNodeAnm(int idx) const {
        return static_cast<ResAnmVisAnmData*>(
            ofs_to_obj<ResDic>(ref().toVisDataDic)[idx]);
    }
    const ResAnmVisAnmData* GetNodeAnm(u32 idx) const {
        return static_cast<ResAnmVisAnmData*>(
            ofs_to_obj<ResDic>(ref().toVisDataDic)[idx]);
    }

    int GetNumFrame() const {
        return ref().info.numFrame;
    }

    /**
     * \~english @brief Gets number of nodes with defined animations.
     * \~japanese @brief アニメが定義されているノード数を取得する。
     */
    int GetNumNode() const {
        return ref().info.numNode;
    }

    /**
     * \~english @brief Gets animation replay policy.
     * \~japanese @brief アニメ再生ポリシーを取得する。
     */
    AnmPolicy GetAnmPolicy() const {
        return ref().info.policy;
    }
};

} // namespace g3d
} // namespace nw4r

#endif
